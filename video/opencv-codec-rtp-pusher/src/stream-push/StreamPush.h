/*
** Copyright (C) 2018 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of class StreamPush.
*/
#pragma once

#include <opencv2/opencv.hpp>
#include "../codec/opencv_codec.h"
#include "../rtp-x264/RtpStream.h"
#include "../common/Str.h"

enum CodecParam {
    AV_CODEC_ID_H264,
    AV_CODEC_ID_H265,
    AV_CODEC_ID_VVC,
    AV_CODEC_ID_VP8,
    AV_CODEC_ID_VP9,
    AV_CODEC_ID_AV1,
    AV_CODEC_ID_AVS2,
    AV_CODEC_ID_AVS3,
};

class StreamPush {
public:
    StreamPush();

    ~StreamPush();

    void useCodec(CodecParam codecParam) {
        mCodecParam = codecParam;
    }

    void init(int w, int h, int channel, int fps);

    bool push(cv::Mat *frame);

    bool encodeOneFrame(cv::Mat *frame, Str *resStr);

    bool saveFile(cv::Mat *frame);

    RtpStream *getRtpStream() {
        return mRtpStream;
    }

    void setRtpStream(RtpStream *rtpStream) {
        mRtpStream = rtpStream;
    }

    void setRtpEnable(bool enabled) {
        mRtpEnabled = enabled;
    }

private:
    Encoder *mEncoder;
    RtpStream *mRtpStream;
    bool mRtpEnabled;
    CodecParam mCodecParam;

    Str mStrBuf;

    int mBufSize;
};
