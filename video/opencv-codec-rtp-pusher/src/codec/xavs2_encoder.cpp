/*
** Copyright (C) 2018 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class X264Encoder.
*/

#include "xavs2_encoder.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "../common/timemeasurer.h"
#include "../common/log.h"

Xavs2Encoder::Xavs2Encoder() {
}

Xavs2Encoder::~Xavs2Encoder() {
    if (params_ != NULL) {
        // free spaces
        api_->opt_destroy(params_);
    }
}


bool Xavs2Encoder::InitEncoder(int width, int height, int channel, int fps, int fourcc) {
    width_ = width;
    height_ = height;
    channel_ = channel;
    fps_ = fps;
    fourcc_ = fourcc;

    int guess_bit_depth = 8;
    api_ = xavs2_api_get(guess_bit_depth);
    if (api_ == NULL) {
        printf("xavs2_api_get err");
        return false;
    }
    // parse parameters and modify the parameters
    params_ = api_->opt_alloc();

    /* create the xavs2 video encoder */
    encoder_ = (xavs2_handler_t *) api_->encoder_create(params_);

    if (api_->encoder_get_buffer(encoder_, &picture_in_) < 0) {
        printf("encoder_get_buffer failed to get frame buffer.\n");
        return false;
    }
    return true;
}

bool Xavs2Encoder::EncodeOneBuf(cv::Mat *yuvMat, Str *resStr) {
    TimeMeasurer tm;
    uint8_t *yuv_buffer = (uint8_t *) yuvMat->data;
    int y_size = width_ * height_;

    memcpy(picture_in_.img.img_planes[0], yuv_buffer, y_size);        //Y
    yuv_buffer += y_size;
    memcpy(picture_in_.img.img_planes[1], yuv_buffer, y_size / 4);    //U
    yuv_buffer += y_size / 4;
    memcpy(picture_in_.img.img_planes[2], yuv_buffer, y_size / 4);    //V
    yuv_buffer += y_size / 4;

    picture_in_.i_state = 0;
    picture_in_.i_type = XAVS2_TYPE_AUTO;
    picture_in_.i_pts = 1;

    xavs2_outpacket_t packet = {0};
    api_->encoder_encode(encoder_, &picture_in_, &packet);

    memcpy((char *) resStr->data, packet.stream, packet.len);
    resStr->size = packet.len;

    LOG_INFO("vvc.encode.cost: %lu, size: %d", tm.Elapsed(), resStr->size);
    return true;
}
