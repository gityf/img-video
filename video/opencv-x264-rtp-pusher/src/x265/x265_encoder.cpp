/*
** Copyright (C) 2018 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class X265Encoder.
*/

#include "x265_encoder.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <opencv2/imgproc.hpp>
#include "../common/timemeasurer.h"
#include "../common/log.h"

X265Encoder::X265Encoder() {
    Init();
}

X265Encoder::X265Encoder(int videoWidth, int videoHeight, int channel, int fps) {
    Init();
    Create(videoWidth, videoHeight, channel, fps);
}

X265Encoder::~X265Encoder() {
    if (NULL != x265_encoder_) {
        x264_picture_clean(&picture_in_);
        x265_encoder_close(x265_encoder_);
    }
}

void X265Encoder::Init() {
    m_width = 0;
    m_height = 0;
    m_channel = 0;
    m_fps = 25;
    x265_encoder_ = NULL;
}

bool X265Encoder::Create(int videoWidth, int videoHeight, int channel, int fps) {
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

bool X265Encoder::InitEncoder() {
    // ultrafast > superfast > veryfast faster fast
    // film animation grain stillimage psnr ssim fastdecode zerolatency touhou

    x265_param_default_preset(&param_, "ultrafast", "zerolatency");

    param_.bRepeatHeaders = 1;//write sps,pps before keyframe
    param_.internalCsp = X265_CSP_I420;
    param_.sourceWidth = m_width;
    param_.sourceHeight = m_height;
    param_.fpsNum = m_fps;
    param_.fpsDenom = 1;

    x265_encoder_ = x265_encoder_open(&param_);

    picture_in_ = x265_picture_alloc();
    x265_picture_init(&param_, picture_in_);

    yuv_buffer_ = (uint8_t *) malloc(m_width * m_height * 3 / 2);
    int y_size = m_width * m_height;
    picture_in_->planes[0] = yuv_buffer_;
    picture_in_->planes[1] = yuv_buffer_ + y_size;
    picture_in_->planes[2] = yuv_buffer_ + y_size * 5 / 4;
    picture_in_->stride[0] = m_width;
    picture_in_->stride[1] = m_width / 2;
    picture_in_->stride[2] = m_width / 2;

    return true;
}

bool X265Encoder::EncodeOneBuf(cv::Mat *yuvMat, Str *resStr) {
    TimeMeasurer tm;
    memset(yuv_buffer_, 0, m_width * m_height * 3 / 2);

    uint8_t *yuv_buffer = (uint8_t *) yuvMat->data;

    int y_size = m_width * m_height;

    memcpy(picture_in_->planes[0], yuv_buffer, y_size);        //Y
    yuv_buffer += y_size;
    memcpy(picture_in_->planes[1], yuv_buffer, y_size / 4);    //U
    yuv_buffer += y_size / 4;
    memcpy(picture_in_->planes[2], yuv_buffer, y_size / 4);    //V
    yuv_buffer += y_size / 4;

    x265_nal *nals = NULL;
    uint32_t nnal = 0;

    int ret = x265_encoder_encode(x265_encoder_, &nals, &nnal, picture_in_, NULL);


    int h264size = 0;
    for (j = 0; j < nnal; j++) {
        memcpy(resStr->data + h264size, nals[j].payload, nals[j].sizeBytes);
        h264size = h264size + nals[j].sizeBytes;
    }

    ret = x265_encoder_encode(x265_encoder_, &nals, &nnal, NULL, NULL);
    if (ret > 0) {
        for (j = 0; j < nnal; j++) {
            memcpy(resStr->data + h264size, nals[j].payload, nals[j].sizeBytes);
            h264size = h264size + nals[j].sizeBytes;
        }
    }

    resStr->size = h264size;

    LOG_INFO("x264.encode.cost: %lu", tm.Elapsed());
    return true;
}

bool X265Encoder::EncodeOneFrame(cv::Mat *frame, Str *resStr) {
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