/*
** Copyright (C) 2018 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of class StreamPush.
*/
#pragma once

#include <opencv2/opencv.hpp>
#include "../x264/x264_encoder.h"
#include "../x265/x265_encoder.h"
#include "../rtp-x264/RtpStream.h"
#include "../common/Str.h"

enum CodecParam {
    AV_CODEC_ID_H264,
    AV_CODEC_ID_H265
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

    RtpStream* getRtpStream() {
        return mRtpStream;
    }

    void setRtpStream(RtpStream* rtpStream) {
        mRtpStream = rtpStream;
    }

    void setRtpEnable(bool enabled) {
        mRtpEnabled = enabled;
    }
private:
    X264Encoder* mX264Encoder;
    X265Encoder *mX265Encoder;

    RtpStream* mRtpStream;
    bool mRtpEnabled;
    CodecParam mCodecParam;

    Str mX264StrBuf;

    int mBufSize;
};
