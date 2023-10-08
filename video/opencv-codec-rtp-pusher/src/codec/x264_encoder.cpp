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
#include "../common/timemeasurer.h"
#include "../common/log.h"

X264Encoder::X264Encoder() {
}

X264Encoder::~X264Encoder() {
    if (NULL != x264_encoder_) {
        x264_picture_clean(&picture_in_);
        x264_encoder_close(x264_encoder_);
    }
}

bool X264Encoder::InitEncoder(int width, int height, int channel, int fps, int fourcc) {
    width_ = width;
    height_ = height;
    channel_ = channel;
    fps_ = fps;
    fourcc_ = fourcc;
    // ultrafast > superfast > veryfast faster fast
    x264_param_default_preset(&param_, "ultrafast", "zerolatency");
    // film animation grain stillimage psnr ssim fastdecode zerolatency touhou
    param_.i_threads = 1;
    param_.i_width = width_;
    param_.i_height = height_;
    param_.i_fps_num = fps_;//视频数据帧率
    param_.rc.i_bitrate = fps_ * 1024;
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

    x264_picture_alloc(&picture_in_, X264_CSP_I420, width_, height_);

    yuv_buffer_ = (uint8_t *) malloc(width_ * height_ * 3);
    picture_in_.img.plane[0] = yuv_buffer_;
    picture_in_.img.plane[1] = picture_in_.img.plane[0] + width_ * height_;
    picture_in_.img.plane[2] = picture_in_.img.plane[1] + width_ * height_ / 4;

    return true;
}

bool X264Encoder::EncodeOneBuf(cv::Mat *yuvMat, Str *resStr) {
    TimeMeasurer tm;
    memset(yuv_buffer_, 0, width_ * height_ * 3);

    uint8_t *yuv_buffer = (uint8_t *) yuvMat->data;

    memcpy(picture_in_.img.plane[0], yuv_buffer, width_ * height_);
    yuv_buffer += width_ * height_;
    memcpy(picture_in_.img.plane[1], yuv_buffer, width_ * height_ / 4);
    yuv_buffer += width_ * height_ / 4;
    memcpy(picture_in_.img.plane[2], yuv_buffer, width_ * height_ / 4);
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
        memcpy((char *) resStr->data + h264size, nal->p_payload, nal->i_payload);
        h264size = h264size + nal->i_payload;
    }

    resStr->size = h264size;

    LOG_INFO("x264.encode.cost: %lu, size: %d", tm.Elapsed(), h264size);
    return true;
}
