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
#include "../codec/av1_encoder.h"
#include "../codec/vpx_encoder.h"
#include "../codec/x264_encoder.h"
#include "../codec/x265_encoder.h"
#include "../codec/vvc_encoder.h"
#include "../codec/xavs2_encoder.h"

StreamPush::StreamPush() {
    mRtpStream = nullptr;
    mEncoder = nullptr;
    mRtpEnabled = true;
    mCodecParam = AV_CODEC_ID_H264;
}

StreamPush::~StreamPush() {
    if (nullptr != mRtpStream) {
        delete mRtpStream;
    }
    if (nullptr != mEncoder) {
        delete mEncoder;
    }
}

void StreamPush::init(int w, int h, int channel, int fps) {
    int fourcc = 0;
    switch (mCodecParam) {
        case AV_CODEC_ID_H264:
            mEncoder = new X264Encoder();
            break;
        case AV_CODEC_ID_H265:
            mEncoder = new X265Encoder();
            break;
        case AV_CODEC_ID_VVC:
            mEncoder = new VvcEncoder();
            break;
        case AV_CODEC_ID_AVS2:
            mEncoder = new Xavs2Encoder();
            break;
        case AV_CODEC_ID_AV1:
            mEncoder = new Av1Encoder();
            break;
        case AV_CODEC_ID_VP8:
        case AV_CODEC_ID_VP9:
            mEncoder = new VpxEncoder();
            if (mCodecParam == AV_CODEC_ID_VP8) {
                fourcc = vp8_fourcc;
            } else {
                fourcc = vp9_fourcc;
            }
            break;
        default:
            mEncoder = new X264Encoder();
            break;
    }

    mEncoder->InitEncoder(w, h, channel, fps, fourcc);

    mBufSize = w * h * 3 + 1024;
    mStrBuf.size = 0;
    mStrBuf.data = malloc(mBufSize);
    memset(mStrBuf.data, 0, mBufSize);
}

bool StreamPush::push(cv::Mat *frame) {
    if (frame->empty()) {
        return false;
    }
    if (this->encodeOneFrame(frame, &mStrBuf)) {
        int ret = 0;
        if (mRtpEnabled) {
            //LOG_HEX_ASC(LL_WARN, respStr.data, respStr.size);
            ret = mRtpStream->H264Nal2RtpSend(mEncoder->fps_, (uint8_t *) mStrBuf.data, mStrBuf.size);
        }
    }
    //std::cout << "rtp.send.cost: " << tm.Elapsed() << std::endl;
    return true;
}

bool StreamPush::encodeOneFrame(cv::Mat *frame, Str *resStr) {
    if (frame->empty()) {
        return false;
    }
    cv::Mat yuv;

    if (1 == frame->channels()) {
        cv::Mat bgr(*frame);
        cv::cvtColor(*frame, bgr, CV_GRAY2BGR);
        cv::cvtColor(bgr, yuv, CV_BGR2YUV_I420);
    } else {
        cv::cvtColor(*frame, yuv, CV_BGR2YUV_I420);
    }
    //Str reqStr;
    //reqStr.data = yuv.data;
    //reqStr.size = width_ * height_ * 3 / 2;

    bool ret = mEncoder->EncodeOneBuf(&yuv, resStr);
    // WARN::: data is yuv data pointer, it will delete by default.
    //reqStr.data = nullptr;
    return ret;
}

bool StreamPush::saveFile(cv::Mat *frame) {
    if (frame->empty()) {
        return false;
    }
    bool encode_succ = this->encodeOneFrame(frame, &mStrBuf);
    if (!encode_succ) {
        return false;
    }

    // TODO to file

    return true;
}