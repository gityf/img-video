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
#include "../rtp-x264/RtpStream.h"
#include "../common/Str.h"

class StreamPush {
public:
    StreamPush();
    ~StreamPush();

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
    RtpStream* mRtpStream;
    bool mRtpEnabled;

    Str mX264StrBuf;

    int mBufSize;
};
