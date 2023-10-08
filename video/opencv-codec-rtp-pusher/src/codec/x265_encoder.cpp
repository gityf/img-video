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
#include "../common/timemeasurer.h"
#include "../common/log.h"

X265Encoder::X265Encoder() {
    x265_encoder_ = NULL;
    yuv_buffer_ = nullptr;
}

X265Encoder::~X265Encoder() {
    if (nullptr != x265_encoder_) {
        x265_picture_free(picture_in_);
        x265_encoder_close(x265_encoder_);
    }
    if (nullptr != yuv_buffer_) {
        delete yuv_buffer_;
    }
}

bool X265Encoder::InitEncoder(int width, int height, int channel, int fps, int fourcc) {
    width_ = width;
    height_ = height;
    channel_ = channel;
    fps_ = fps;
    fourcc_ = fourcc;
    // ultrafast > superfast > veryfast faster fast
    // film animation grain stillimage psnr ssim fastdecode zerolatency touhou

    x265_param_default_preset(&param_, "ultrafast", "zerolatency");

    param_.frameNumThreads = 1;
    param_.bRepeatHeaders = 1;//write sps,pps before keyframe
    param_.internalCsp = X265_CSP_I420;
    param_.sourceWidth = width_;
    param_.sourceHeight = height_;
    param_.fpsNum = 1;
    param_.rc.bitrate = fps_ * 1024;
    param_.fpsDenom = 1;
    param_.totalFrames = 1;

    x265_encoder_ = x265_encoder_open(&param_);

    picture_in_ = x265_picture_alloc();
    x265_picture_init(&param_, picture_in_);

    int y_size = width_ * height_;
    yuv_buffer_ = (uint8_t *) malloc(width_ * height_ * 3);
    picture_in_->planes[0] = yuv_buffer_;
    picture_in_->planes[1] = yuv_buffer_ + y_size;
    picture_in_->planes[2] = yuv_buffer_ + y_size * 5 / 4;
    picture_in_->stride[0] = width_;
    picture_in_->stride[1] = width_ / 2;
    picture_in_->stride[2] = width_ / 2;

    return true;
}

bool X265Encoder::EncodeOneBuf(cv::Mat *yuvMat, Str *resStr) {
    TimeMeasurer tm;
    memset(yuv_buffer_, 0, width_ * height_ * 3 / 2);

    uint8_t *yuv_buffer = (uint8_t *) yuvMat->data;

    int y_size = width_ * height_;

    memcpy(picture_in_->planes[0], yuv_buffer, y_size);        //Y
    yuv_buffer += y_size;
    memcpy(picture_in_->planes[1], yuv_buffer, y_size / 4);    //U
    yuv_buffer += y_size / 4;
    memcpy(picture_in_->planes[2], yuv_buffer, y_size / 4);    //V
    yuv_buffer += y_size / 4;

    int h265size = 0;

    // encode hevc header: avoid err 'Could not find ref with POC' in ffmpeg hevc decode
    x265_nal *header_nals = NULL;
    uint32_t header_nnal = 0;
    int ret = x265_encoder_headers(x265_encoder_, &header_nals, &header_nnal);
    for (int j = 0; j < header_nnal; j++) {
        memcpy((char *) resStr->data + h265size, header_nals[j].payload, header_nals[j].sizeBytes);
        h265size = h265size + header_nals[j].sizeBytes;
    }

    // encode hevc payload
    x265_nal *nals = NULL;
    uint32_t nnal = 0;
    ret = x265_encoder_encode(x265_encoder_, &nals, &nnal, picture_in_, NULL);
    for (int j = 0; j < nnal; j++) {
        memcpy((char *) resStr->data + h265size, nals[j].payload, nals[j].sizeBytes);
        h265size = h265size + nals[j].sizeBytes;
    }

    // encode hevc tail
    x265_nal *tail_nals = NULL;
    uint32_t tail_nnal = 0;
    ret = x265_encoder_encode(x265_encoder_, &tail_nals, &tail_nnal, NULL, NULL);
    for (int j = 0; j < tail_nnal; j++) {
        //printf("nals[%d].type=%d\n", j, nals[j].type);
        memcpy((char *) resStr->data + h265size, tail_nals[j].payload, tail_nals[j].sizeBytes);
        h265size = h265size + tail_nals[j].sizeBytes;
    }

    resStr->size = h265size;

    LOG_INFO("x265.encode.cost: %lu, size:%d", tm.Elapsed(), h265size);
    return true;
}
