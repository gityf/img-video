/*
** Copyright (C) 2018 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of class X264Encoder.
*/
#ifndef _X264_ENCODER_H
#define _X264_ENCODER_H

#include <stdint.h>
#include <x264.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "../common/Str.h"
#include "opencv_codec.h"

// refer to :https://github.com/tolkkiMetal/x264Encoder/blob/master/jni/encoder/X264_Encoder.hpp

class X264Encoder : public Encoder {
public:
    X264Encoder();

    ~X264Encoder();

    bool InitEncoder(int width, int height, int channel = 3, int fps = 25, int fourcc = 0);

    virtual bool EncodeOneBuf(cv::Mat *yuvMat, Str *resStr);

private:

    x264_t* x264_encoder_;
    x264_param_t param_;
    x264_picture_t picture_in_, picture_out_;

    uint8_t *yuv_buffer_;
};

#endif