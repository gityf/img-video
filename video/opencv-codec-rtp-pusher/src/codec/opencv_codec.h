/*
** Copyright (C) 2023 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of class Av1Encoder.
*/

#ifndef IMG_VIDEO_OPENCV_CODEC_H
#define IMG_VIDEO_OPENCV_CODEC_H

#include <stdint.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "common/Str.h"

class Encoder {
public:
    Encoder();

    virtual ~Encoder() {};

    virtual bool InitEncoder(int width, int height, int channel = 3, int fps = 25, int fourcc = 0) = 0;

    virtual bool EncodeOneBuf(cv::Mat *yuvMat, Str *resStr) = 0;

public:
    int width_;
    int height_;
    int channel_;
    int fps_;
    int fourcc_;
};

#endif //IMG_VIDEO_OPENCV_CODEC_H
