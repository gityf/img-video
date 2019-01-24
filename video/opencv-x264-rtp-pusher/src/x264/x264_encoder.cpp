/*
** Copyright (C) 2018 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class X264Encoder.
*/

#include "x264_encoder.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <opencv2/imgproc.hpp>
#include "../common/timemeasurer.h"
#include "../common/log.h"

X264Encoder::X264Encoder() {
    Init();
}

X264Encoder::X264Encoder(int videoWidth, int videoHeight, int channel, int fps) {
    Init();
    Create(videoWidth, videoHeight, channel, fps);
}

X264Encoder::~X264Encoder() {
    if (NULL != x264_encoder_) {
        x264_picture_clean(&picture_in_);
        x264_encoder_close(x264_encoder_);
    }
}

void X264Encoder::Init() {
    m_width = 0;
    m_height = 0;
    m_channel = 0;
    m_fps = 25;
    x264_encoder_ = NULL;
}

bool X264Encoder::Create(int videoWidth, int videoHeight, int channel, int fps) {
    if (videoWidth <= 0 || videoHeight <= 0 || channel < 0 || fps <= 0) {
        printf("wrong input param\n");
        return false;
    }
    m_width = videoWidth;
    m_height = videoHeight;
    m_channel = channel;
    m_fps = fps;

    return true;
}

bool X264Encoder::InitEncoder() {
    // ultrafast > superfast > veryfast faster fast
    x264_param_default_preset(&param_, "ultrafast", "zerolatency");
    // film animation grain stillimage psnr ssim fastdecode zerolatency touhou
    param_.i_threads = 1;
    param_.i_width = m_width;
    param_.i_height = m_height;
    param_.i_fps_num = m_fps;//视频数据帧率
    param_.rc.i_bitrate = m_fps * 1024;
    param_.i_fps_den = 1;
    param_.i_fps_num = 1;
    param_.i_timebase_den = 1;
    param_.i_timebase_num = 1;
    param_.i_log_level = X264_LOG_ERROR;

    param_.i_frame_total = 1;
    param_.i_keyint_max = 10;

    //* bitstream parameters
    /**
    param_.i_bframe = 0;//5;
    param_.b_open_gop = 0;
    param_.i_bframe_pyramid = 0;
    param_.i_bframe_adaptive = X264_B_ADAPT_TRELLIS;
     */

    //param_.i_level_idc = 41;
    //param_.i_threads = 2;

    //param_.i_keyint_max = 25;
    //param_.b_intra_refresh = 1;

    param_.i_csp = X264_CSP_I420;

    //param_.b_annexb = 1;

    x264_param_apply_profile(&param_, "baseline");

    x264_encoder_ = x264_encoder_open(&param_);
    x264_encoder_parameters(x264_encoder_, &param_);

    x264_picture_init(&picture_in_);
    x264_picture_init(&picture_out_);

    x264_picture_alloc(&picture_in_, X264_CSP_I420, m_width, m_height);

    yuv_buffer_ = (uint8_t *)malloc(m_width * m_height * 3);
    picture_in_.img.plane[0] = yuv_buffer_;
    picture_in_.img.plane[1] = picture_in_.img.plane[0] + m_width * m_height;
    picture_in_.img.plane[2] = picture_in_.img.plane[1] + m_width * m_height / 4;

    return true;
}

bool X264Encoder::EncodeOneBuf(cv::Mat *yuvMat, Str *resStr) {
    TimeMeasurer tm;
    memset(yuv_buffer_, 0, m_width * m_height * 3);

    uint8_t* yuv_buffer =(uint8_t*) yuvMat->data;

    memcpy(picture_in_.img.plane[0], yuv_buffer, m_width*m_height);
    yuv_buffer += m_width*m_height;
    memcpy(picture_in_.img.plane[1], yuv_buffer, m_width*m_height / 4);
    yuv_buffer += m_width*m_height / 4;
    memcpy(picture_in_.img.plane[2], yuv_buffer, m_width*m_height / 4);
    picture_in_.i_type = X264_TYPE_IDR;

    int64_t i_pts = 0;
    picture_in_.i_pts = i_pts++;

    x264_nal_t *nals;
    int nnal;

    int h264size = 0;

    x264_picture_t pic_out;
    x264_picture_init(&pic_out);
    x264_encoder_encode(x264_encoder_, &nals, &nnal, &picture_in_, &pic_out);
    x264_nal_t *nal;
    for (nal = nals; nal < nals + nnal; nal++) {
        memcpy((char*)resStr->data + h264size,nal->p_payload,nal->i_payload);
        h264size = h264size + nal->i_payload;
    }

    resStr->size = h264size;

    LOG_INFO("x264.encode.cost: %lu", tm.Elapsed());
    return true;
}

bool X264Encoder::EncodeOneFrame(cv::Mat *frame, Str *resStr) {
    if (frame->empty()) {
        return false;
    }
    cv::Mat yuv;

    if (1 == frame->channels()) {
        cv::Mat bgr(*frame);
        cv::cvtColor(*frame, bgr, CV_GRAY2BGR);
        cv::cvtColor(bgr, yuv, CV_BGR2YUV_I420);
    } else {
        cv::cvtColor(*frame, yuv, CV_BGR2YUV_I420);
    }
    //Str reqStr;
    //reqStr.data = yuv.data;
    //reqStr.size = m_width * m_height * 3 / 2;

    bool ret = EncodeOneBuf(&yuv, resStr);
    // WARN::: data is yuv data pointer, it will delete by default.
    //reqStr.data = nullptr;
    return ret;
}