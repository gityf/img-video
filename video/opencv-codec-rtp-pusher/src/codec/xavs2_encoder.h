/*
** Copyright (C) 2018 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of class X264Encoder.
*/
#ifndef _XAVS2_ENCODER_H
#define _XAVS2_ENCODER_H

#include <stdint.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "../common/Str.h"
#include "opencv_codec.h"
#include "xavs2.h"

struct xavs2_handler_t;
class Xavs2Encoder : public Encoder {
public:
    Xavs2Encoder();

    ~Xavs2Encoder();

    bool InitEncoder(int width, int height, int channel = 3, int fps = 25, int fourcc = 0);

    virtual bool EncodeOneBuf(cv::Mat *yuvMat, Str *resStr);

private:

    const xavs2_api_t *api_;
    xavs2_param_t *params_;
    xavs2_picture_t picture_in_;
    xavs2_handler_t *encoder_;
};

#endif // _XAVS2_ENCODER_H