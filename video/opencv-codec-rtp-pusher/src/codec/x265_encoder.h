/*
** Copyright (C) 2018 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of class X265Encoder.
*/

#ifndef _X265_ENCODER_H
#define _X265_ENCODER_H

#include <stdint.h>
#include <x265.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "common/Str.h"
#include "opencv_codec.h"

class X265Encoder : public Encoder {
public:
    X265Encoder();

    ~X265Encoder();

    bool InitEncoder(int width, int height, int channel = 3, int fps = 25, int fourcc = 0);

    virtual bool EncodeOneBuf(cv::Mat *yuvMat, Str *resStr);

private:

    x265_encoder *x265_encoder_;
    x265_param param_;
    x265_picture *picture_in_;
    uint8_t *yuv_buffer_;
};

#endif
