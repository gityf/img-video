/*
** Copyright (C) 2023 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class OpencvEncoder.
*/

#include "opencv_codec.h"
#include <opencv2/imgproc.hpp>

Encoder::Encoder() {
    width_ = 0;
    height_ = 0;
    channel_ = 0;
    fps_ = 25;
    fourcc_ = 0;
}
