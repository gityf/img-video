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

// refer to :https://github.com/tolkkiMetal/x264Encoder/blob/master/jni/encoder/X264_Encoder.hpp

class X264Encoder {
public:

    X264Encoder();

    X264Encoder(int videoWidth, int videoHeight, int channel, int fps);

    ~X264Encoder();

    /** 创建X264编码器
     * @param[in] videoWidth  视频宽度
     * @param[in] videoHeight 视频高度
     * @param[in] fps 帧率
     * @return 成功返回true, 失败返回false.
     */
    bool Create(int videoWidth, int videoHeight, int channel = 3, int fps = 30);

    bool InitEncoder();

    /** 编码一帧
     * @param[in] frame 输入的一帧图像
     * @return 返回编码后数据尺寸, 0表示编码失败
     */
    bool EncodeOneFrame(cv::Mat* frame, Str* respStr);

    bool EncodeOneBuf(cv::Mat *yuvMat, Str* resStr);

private:

    void Init();

    x264_t* x264_encoder_;
    x264_param_t param_;
    x264_picture_t picture_in_, picture_out_;

    uint8_t *yuv_buffer_;

public:
    int m_width;
    int m_height;
    int m_channel;
    int m_fps;
};

#endif