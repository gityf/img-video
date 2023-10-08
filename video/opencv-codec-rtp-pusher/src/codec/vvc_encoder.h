/*
** Copyright (C) 2018 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of class X264Encoder.
*/
#ifndef _VVENC_ENCODER_H
#define _VVENC_ENCODER_H

#include <stdint.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "../common/Str.h"
#include "opencv_codec.h"
#include <vvenc/vvenc.h>
#include <vvenc/vvencCfg.h>

class VvcEncoder : public Encoder {
public:
    VvcEncoder();

    ~VvcEncoder();

    bool InitEncoder(int width, int height, int channel = 3, int fps = 25, int fourcc = 0);

    virtual bool EncodeOneBuf(cv::Mat *yuvMat, Str *resStr);

private:

    vvencEncoder *vvc_encoder_;
    vvenc_config params_;
    vvencYUVBuffer yuv_buffer_;
    vvencAccessUnit vvenc_access_unit_;
};

#endif // _VVENC_ENCODER_H