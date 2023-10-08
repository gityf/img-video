/*
** Copyright (C) 2018 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class X265Encoder.
*/

#include "vpx_encoder.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "../common/timemeasurer.h"
#include "../common/log.h"

#define IVF_FILE_HDR_SZ  (32)
#define IVF_FRAME_HDR_SZ (12)

static void mem_put_le16(char *mem, unsigned int val) {
    mem[0] = val;
    mem[1] = val >> 8;
}

static void mem_put_le32(char *mem, unsigned int val) {
    mem[0] = val;
    mem[1] = val >> 8;
    mem[2] = val >> 16;
    mem[3] = val >> 24;
}

// outHeader[32]
void VpxEncoder::WriteIvfFileHeader(char *outHeader, const vpx_codec_enc_cfg_t *cfg, int frame_cnt) {
    if (cfg->g_pass != VPX_RC_ONE_PASS && cfg->g_pass != VPX_RC_LAST_PASS)
        return;
    outHeader[0] = 'D';
    outHeader[1] = 'K';
    outHeader[2] = 'I';
    outHeader[3] = 'F';
    mem_put_le16(outHeader + 4, 0);                   /* version */
    mem_put_le16(outHeader + 6, 32);                  /* headersize */
    mem_put_le32(outHeader + 8, fourcc_);              /* headersize */
    mem_put_le16(outHeader + 12, cfg->g_w);            /* width */
    mem_put_le16(outHeader + 14, cfg->g_h);            /* height */
    mem_put_le32(outHeader + 16, cfg->g_timebase.den); /* rate */
    mem_put_le32(outHeader + 20, cfg->g_timebase.num); /* scale */
    mem_put_le32(outHeader + 24, frame_cnt);           /* length */
    mem_put_le32(outHeader + 28, 0);                   /* unused */
}

// outHeader[12]
void VpxEncoder::WriteIvfFrameHeader(char *outHeader, const vpx_codec_cx_pkt_t *pkt) {
    vpx_codec_pts_t pts;

    if (pkt->kind != VPX_CODEC_CX_FRAME_PKT) {
        return;
    }
    pts = pkt->data.frame.pts;
    mem_put_le32(outHeader, pkt->data.frame.sz);
    mem_put_le32(outHeader + 4, pts & 0xFFFFFFFF);
    mem_put_le32(outHeader + 8, pts >> 32);
}

VpxEncoder::VpxEncoder() {
    fourcc_ = 0;
}

VpxEncoder::~VpxEncoder() {
    vpx_codec_destroy(&vpx_codec_);
}

bool VpxEncoder::InitEncoder(int width, int height, int channel, int fps, int fourcc) {
    width_ = width;
    height_ = height;
    channel_ = channel;
    fps_ = fps;
    fourcc_ = fourcc;
    // allow vpx image raw
    if (!vpx_img_alloc(&vpx_image_raw_, VPX_IMG_FMT_I420, width_, height_, 16)) {
        printf("Fail to allocate image\n");
        return false;
    }
    if (fourcc_ == vp8_fourcc) {
        vpx_codec_iface_ = vpx_codec_vp8_cx();
    } else {
        vpx_codec_iface_ = vpx_codec_vp9_cx();
    }
    printf("Using %s\n", vpx_codec_iface_name(vpx_codec_iface_));
    // Populate encoder configuration
    vpx_codec_err_t ret = vpx_codec_enc_config_default(vpx_codec_iface_, &vpx_cfg_, 0);
    if (ret) {
        printf("Failed to get config: %s\n", vpx_codec_err_to_string(ret));
        return false;
    }
    vpx_cfg_.rc_target_bitrate = 800;
    vpx_cfg_.g_w = width_;
    vpx_cfg_.g_h = height_;
    vpx_cfg_.g_profile = 0;
    vpx_cfg_.g_timebase.num = 1;
    vpx_cfg_.g_input_bit_depth = 8;
    vpx_cfg_.g_timebase.den = 1;

    // Initialize codec
    if (vpx_codec_enc_init(&vpx_codec_, vpx_codec_iface_, &vpx_cfg_, 0)) {
        printf("Failed to initialize encoder\n");
        return false;
    }

    return true;
}

bool VpxEncoder::EncodeOneBuf(cv::Mat *yuvMat, Str *resStr) {
    TimeMeasurer tm;
    uint8_t *yuv_buffer = (uint8_t *) yuvMat->data;
    memcpy(vpx_image_raw_.planes[0], yuv_buffer, width_ * height_ * 3 / 2);

    int frame_cnt = 1;
    int flags = 0;
    // Initialize codec
    if (vpx_codec_enc_init(&vpx_codec_, vpx_codec_iface_, &vpx_cfg_, 0)) {
        printf("Failed to initialize encoder\n");
        return false;
    }
    // Encode vpx
    flags |= VPX_EFLAG_FORCE_KF;
    vpx_codec_err_t ret = vpx_codec_encode(&vpx_codec_, &vpx_image_raw_, frame_cnt, 1, flags, VPX_DL_REALTIME);
    // vpx_codec_err_t ret = vpx_codec_encode(&vpx_codec_, &vpx_image_raw_, frame_cnt, 1, flags, VPX_DL_GOOD_QUALITY);
    if (ret) {
        return false;
    }

    if (fourcc_ == vp9_fourcc) {
        ret = vpx_codec_encode(&vpx_codec_, NULL, 0, 1, flags, VPX_DL_REALTIME);
        if (ret) {
            return false;
        }
    }

    int payload_size = 0;
    //WriteIvfFileHeader((char *) resStr->data, &vpx_cfg_, 1);
    //payload_size += IVF_FILE_HDR_SZ;
    vpx_codec_iter_t iter = NULL;
    const vpx_codec_cx_pkt_t *vpx_pkt = NULL;
    while ((vpx_pkt = vpx_codec_get_cx_data(&vpx_codec_, &iter))) {
        switch (vpx_pkt->kind) {
            printf("vpx_pkt->kind:%d\n", vpx_pkt->kind);
            case VPX_CODEC_CX_FRAME_PKT:
                //WriteIvfFrameHeader((char *) resStr->data + payload_size, vpx_pkt);
                //payload_size += IVF_FRAME_HDR_SZ;
                memcpy((char *) resStr->data + payload_size, vpx_pkt->data.frame.buf, vpx_pkt->data.frame.sz);
                payload_size += vpx_pkt->data.frame.sz;
                // break;
            default:
                break;
        }
    }

    resStr->size = payload_size;
    vpx_codec_destroy(&vpx_codec_);

    LOG_INFO("vpx.encode.cost: %lu,%d,%s", tm.Elapsed(), payload_size, vpx_codec_err_to_string(vpx_codec_.err));
    return true;
}