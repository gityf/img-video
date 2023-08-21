/*
** Copyright (C) 2018 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class StreamPush.
*/

#include "StreamPush.h"
#include "../common/log.h"
#include "../common/timemeasurer.h"

StreamPush::StreamPush() {
    mRtpStream = nullptr;
    mX264Encoder = nullptr;
    mRtpEnabled = true;
    mCodecParam = AV_CODEC_ID_H264;
}

StreamPush::~StreamPush() {
    if (nullptr != mRtpStream) {
        delete mRtpStream;
    }
    if (nullptr != mX264Encoder) {
        delete mX264Encoder;
    }
}

void StreamPush::init(int w, int h, int channel, int fps) {
    if (nullptr == mX264Encoder) {
        mX264Encoder = new X264Encoder(w, h, channel, fps);
        mX264Encoder->InitEncoder();
    }
    if (nullptr == mX265Encoder) {
        mX265Encoder = new X265Encoder(w, h, channel, fps);
        mX265Encoder->InitEncoder();
    }
    mBufSize = w*h*3+1024;
    mX264StrBuf.size = 0;
    mX264StrBuf.data = malloc(mBufSize);
    memset(mX264StrBuf.data, 0, mBufSize);
}

bool StreamPush::push(cv::Mat *frame) {
    if (!frame->empty()){
         bool encode_succ = false;
         if (mCodecParam == AV_CODEC_ID_H265) {
             encode_succ = mX265Encoder->EncodeOneFrame(frame, &mX264StrBuf);
         } else {
             encode_succ = mX264Encoder->EncodeOneFrame(frame, &mX264StrBuf);
         }
         //std::cout << "x264.encode.cost: " << tm.Elapsed() << std::endl;
         if (encode_succ) {
             int ret = 0;
             if (mRtpEnabled) {
                 //LOG_HEX_ASC(LL_WARN, respStr.data, respStr.size);
                 ret = mRtpStream->H264Nal2RtpSend(mX264Encoder->m_fps, (uint8_t *) mX264StrBuf.data, mX264StrBuf.size);
             }
         }
        //std::cout << "rtp.send.cost: " << tm.Elapsed() << std::endl;
    } else {
        return false;
    }
    return true;
}
