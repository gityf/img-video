/*
** Copyright (C) 2018 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class RtpStream.
*/

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include "common/log.h"
#include "h264tortp.h"
#include "RtpStream.h"

using namespace std;

#define RTP_PAYLOAD_MAX_SIZE        1400
#define SEND_BUF_SIZE               1500
#define NAL_BUF_SIZE                1500 * 50
#define SSRC_NUM                    10

#define DEBUG_PRINT         0
#define debug_print(fmt, ...) \
    do { if (DEBUG_PRINT) fprintf(stderr, "-------%s: %d: %s():---" fmt "----\n", \
            __FILE__, __LINE__, __func__, ##__VA_ARGS__);} while (0)

RtpStream::RtpStream() {
    mEndPointMgr = nullptr;
}

RtpStream::~RtpStream() {
    if (nullptr != mEndPointMgr) {
        delete  mEndPointMgr;
    }
}

void RtpStream::SetEndPointMgr(EndPointMgr *endPointMgr) {
    mEndPointMgr = endPointMgr;
}

int RtpStream::SendDataToEveryEdge(unsigned char *send_buf, size_t len_sendbuf) {
    return mEndPointMgr->sendData(send_buf, len_sendbuf);
}

#define END_SEQ_NO 65530

int RtpStream::H264Nal2RtpSend(int framerate, unsigned char *pstStream, size_t nalu_len) {
    uint8_t SENDBUFFER[SEND_BUF_SIZE];
    uint8_t nal_buf[NAL_BUF_SIZE];

    uint8_t *nalu_buf = pstStream;
    // int nalu_len;   /* 不包括0x00000001起始码, 但包括nalu头部的长度 */
    static uint32_t ts_current = 0;
    uint16_t seq_num = 0;
    rtp_header_t *rtp_hdr;
    nalu_header_t *nalu_hdr;
    fu_indicator_t *fu_ind;
    fu_header_t *fu_hdr;
    size_t len_sendbuf;

    int fu_pack_num;        /* nalu 需要分片发送时分割的个数 */
    int last_fu_pack_size;  /* 最后一个分片的大小 */
    int fu_seq;             /* fu-A 序号 */

    debug_print();
    ts_current += (90000 / framerate);  /* 90000 / 25 = 3600 */

    /*
     * 加入长度判断，
     * 当 nalu_len == 0 时， 必须跳到下一轮循环
     * nalu_len == 0 时， 若不跳出会发生段错误!
     * fix by hmg
     */
    if (nalu_len < 1) {
        return -1;
    }

    if (nalu_len <= RTP_PAYLOAD_MAX_SIZE) {
        /*
         * single nal unit
         */

        memset(SENDBUFFER, 0, SEND_BUF_SIZE);

        /*
         * 1. 设置 rtp 头
         */
        rtp_hdr = (rtp_header_t *)SENDBUFFER;
        rtp_hdr->csrc_len = 0;
        rtp_hdr->extension = 0;
        rtp_hdr->padding = 0;
        rtp_hdr->version = 2;
        rtp_hdr->payload_type = H264;
        rtp_hdr->marker = 0;
        // rtp_hdr->marker = (pstStream->u32PackCount - 1 == i) ? 1 : 0;   /* 该包为一帧的结尾则置为1, 否则为0. rfc 1889 没有规定该位的用途 */
        rtp_hdr->seq_no = htons(++seq_num % UINT16_MAX);
        rtp_hdr->timestamp = htonl(ts_current);
        rtp_hdr->ssrc = htonl(SSRC_NUM);

        debug_print();
        /*
         * 2. 设置rtp荷载 single nal unit 头
         */
        nalu_hdr = (nalu_header_t *)&SENDBUFFER[12];
        nalu_hdr->f = (nalu_buf[0] & 0x80) >> 7;        /* bit0 */
        nalu_hdr->nri = (nalu_buf[0] & 0x60) >> 5;      /* bit1~2 */
        nalu_hdr->type = (nalu_buf[0] & 0x1f);
        debug_print();

        /*
         * 3. 填充nal内容
         */
        debug_print();
        memcpy(SENDBUFFER + 13, nalu_buf + 1, nalu_len - 1);    /* 不拷贝nalu头 */

        /*
         * 4. 发送打包好的rtp到客户端
         */
        len_sendbuf = 12 + nalu_len;
        SendDataToEveryEdge(SENDBUFFER, len_sendbuf);
        debug_print();
    } else {    /* nalu_len > RTP_PAYLOAD_MAX_SIZE */
        /*
         * FU-A分割
         */
        debug_print();

        /*
         * 1. 计算分割的个数
         *
         * 除最后一个分片外，
         * 每一个分片消耗 RTP_PAYLOAD_MAX_SIZE BYLE
         */
        fu_pack_num = nalu_len % RTP_PAYLOAD_MAX_SIZE ? (nalu_len / RTP_PAYLOAD_MAX_SIZE + 1) : nalu_len / RTP_PAYLOAD_MAX_SIZE;
        last_fu_pack_size = nalu_len % RTP_PAYLOAD_MAX_SIZE ? nalu_len % RTP_PAYLOAD_MAX_SIZE : RTP_PAYLOAD_MAX_SIZE;
        fu_seq = 0;

        for (fu_seq = 0; fu_seq < fu_pack_num; fu_seq++) {
            memset(SENDBUFFER, 0, SEND_BUF_SIZE);

            /*
             * 根据FU-A的类型设置不同的rtp头和rtp荷载头
             */
            if (fu_seq == 0) {  /* 第一个FU-A */
                /*
                 * 1. 设置 rtp 头
                 */
                rtp_hdr = (rtp_header_t *)SENDBUFFER;
                rtp_hdr->csrc_len = 0;
                rtp_hdr->extension = 0;
                rtp_hdr->padding = 0;
                rtp_hdr->version = 2;
                rtp_hdr->payload_type = H264;
                rtp_hdr->marker = 0;    /* 该包为一帧的结尾则置为1, 否则为0. rfc 1889 没有规定该位的用途 */
                rtp_hdr->seq_no = htons(++seq_num % UINT16_MAX);
                rtp_hdr->timestamp = htonl(ts_current);
                rtp_hdr->ssrc = htonl(SSRC_NUM);

                /*
                 * 2. 设置 rtp 荷载头部
                 */
                fu_ind = (fu_indicator_t *)&SENDBUFFER[12];
                fu_ind->f = (nalu_buf[0] & 0x80) >> 7;
                fu_ind->nri = (nalu_buf[0] & 0x60) >> 5;
                fu_ind->type = 28;

                fu_hdr = (fu_header_t *)&SENDBUFFER[13];
                fu_hdr->s = 1;
                fu_hdr->e = 0;
                fu_hdr->r = 0;
                fu_hdr->type = nalu_buf[0] & 0x1f;

                /*
                 * 3. 填充nalu内容
                 */
                memcpy(SENDBUFFER + 14, nalu_buf+1, RTP_PAYLOAD_MAX_SIZE-1);    /* 不拷贝nalu头 */

                /*
                 * 4. 发送打包好的rtp包到客户端
                 */
                len_sendbuf = 12 + 2 + (RTP_PAYLOAD_MAX_SIZE-1);  /* rtp头 + nalu头 + nalu内容 */
                SendDataToEveryEdge(SENDBUFFER, len_sendbuf);

            } else if (fu_seq < fu_pack_num - 1) { /* 中间的FU-A */
                /*
                 * 1. 设置 rtp 头
                 */
                rtp_hdr = (rtp_header_t *)SENDBUFFER;
                rtp_hdr->csrc_len = 0;
                rtp_hdr->extension = 0;
                rtp_hdr->padding = 0;
                rtp_hdr->version = 2;
                rtp_hdr->payload_type = H264;
                rtp_hdr->marker = 0;    /* 该包为一帧的结尾则置为1, 否则为0. rfc 1889 没有规定该位的用途 */
                rtp_hdr->seq_no = htons(++seq_num % UINT16_MAX);
                rtp_hdr->timestamp = htonl(ts_current);
                rtp_hdr->ssrc = htonl(SSRC_NUM);

                /*
                 * 2. 设置 rtp 荷载头部
                 */
                fu_ind = (fu_indicator_t *)&SENDBUFFER[12];
                fu_ind->f = (nalu_buf[0] & 0x80) >> 7;
                fu_ind->nri = (nalu_buf[0] & 0x60) >> 5;
                fu_ind->type = 28;

                fu_hdr = (fu_header_t *)&SENDBUFFER[13];
                fu_hdr->s = 0;
                fu_hdr->e = 0;
                fu_hdr->r = 0;
                fu_hdr->type = nalu_buf[0] & 0x1f;

                /*
                 * 3. 填充nalu内容
                 */
                memcpy(SENDBUFFER + 14, nalu_buf + RTP_PAYLOAD_MAX_SIZE * fu_seq, RTP_PAYLOAD_MAX_SIZE);    /* 不拷贝nalu头 */

                /*
                 * 4. 发送打包好的rtp包到客户端
                 */
                len_sendbuf = 12 + 2 + RTP_PAYLOAD_MAX_SIZE;
                SendDataToEveryEdge(SENDBUFFER, len_sendbuf);

            } else { /* 最后一个FU-A */
                /*
                 * 1. 设置 rtp 头
                 */
                rtp_hdr = (rtp_header_t *)SENDBUFFER;
                rtp_hdr->csrc_len = 0;
                rtp_hdr->extension = 0;
                rtp_hdr->padding = 0;
                rtp_hdr->version = 2;
                rtp_hdr->payload_type = H264;
                rtp_hdr->marker = 1;    /* 该包为一帧的结尾则置为1, 否则为0. rfc 1889 没有规定该位的用途 */
                rtp_hdr->seq_no = htons(++seq_num % UINT16_MAX);
                //rtp_hdr->seq_no = htons(END_SEQ_NO);
                rtp_hdr->timestamp = htonl(ts_current);
                rtp_hdr->ssrc = htonl(SSRC_NUM);

                /*
                 * 2. 设置 rtp 荷载头部
                 */
                fu_ind = (fu_indicator_t *)&SENDBUFFER[12];
                fu_ind->f = (nalu_buf[0] & 0x80) >> 7;
                fu_ind->nri = (nalu_buf[0] & 0x60) >> 5;
                fu_ind->type = 28;

                fu_hdr = (fu_header_t *)&SENDBUFFER[13];
                fu_hdr->s = 0;
                fu_hdr->e = 1;
                fu_hdr->r = 0;
                fu_hdr->type = nalu_buf[0] & 0x1f;

                /*
                 * 3. 填充rtp荷载
                 */
                memcpy(SENDBUFFER + 14, nalu_buf + RTP_PAYLOAD_MAX_SIZE * fu_seq, last_fu_pack_size);    /* 不拷贝nalu头 */

                /*
                 * 4. 发送打包好的rtp包到客户端
                 */
                len_sendbuf = 12 + 2 + last_fu_pack_size;
                SendDataToEveryEdge(SENDBUFFER, len_sendbuf);

            }
        }

    }

    debug_print();
    return 0;
}