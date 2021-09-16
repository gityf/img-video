/*
** Copyright (C) 2018 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of class X265Encoder.
*/

#ifndef _X264_ENCODER_H
#define _X264_ENCODER_H

#include <stdint.h>
#include <x265.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "common/Str.h"

class X265Encoder {
public:

    X265Encoder();

    X265Encoder(int videoWidth, int videoHeight, int channel, int fps);

    ~X265Encoder();

    /** 创建X265编码器
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

    x265_encoder* x265_encoder_;
    x265_param param_;
    x265_picture *picture_in_;

    uint8_t *yuv_buffer_;

public:
    int m_width;
    int m_height;
    int m_channel;
    int m_fps;
};

#endif
