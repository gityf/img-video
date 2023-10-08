/*
** Copyright (C) 2023 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of class Av1Encoder.
*/

#ifndef IMG_VIDEO_AV1_ENCODER_H
#define IMG_VIDEO_AV1_ENCODER_H

#include <stdint.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "common/Str.h"
#include "opencv_codec.h"
#include "aom/aom_encoder.h"
#include "aom/aomcx.h"

class Av1Encoder : public Encoder {
public:

    Av1Encoder();

    ~Av1Encoder();

    bool InitEncoder(int width, int height, int channel = 3, int fps = 25, int fourcc = 0);

    virtual bool EncodeOneBuf(cv::Mat *yuvMat, Str *resStr);

private:
    aom_codec_ctx_t aom_codec_;
    aom_codec_enc_cfg_t aom_cfg_;
    aom_image_t aom_image_raw_;
    aom_codec_iface_t *aom_codec_iface_;
    int fourcc_;

    uint8_t *yuv_buffer_;
};

#endif //IMG_VIDEO_AV1_ENCODER_H
