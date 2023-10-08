/*
** Copyright (C) 2023 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class Av1Encoder.
*/

#include "av1_encoder.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "../common/timemeasurer.h"
#include "../common/log.h"

Av1Encoder::Av1Encoder() {
}

Av1Encoder::~Av1Encoder() {
    aom_codec_destroy(&aom_codec_);
}

bool Av1Encoder::InitEncoder(int width, int height, int channel, int fps, int fourcc) {
    width_ = width;
    height_ = height;
    channel_ = channel;
    fps_ = fps;
    fourcc_ = fourcc;
    // allow av1 image raw
    if (!aom_img_alloc(&aom_image_raw_, AOM_IMG_FMT_I420, width_, height_, 16)) {
        printf("Fail to allocate image\n");
        return false;
    }
    const int usage = 1;
    const int speed = 7;
    aom_codec_iface_ = aom_codec_av1_cx();
    printf("Using %s\n", aom_codec_iface_name(aom_codec_iface_));
    // Populate encoder configuration
    aom_codec_err_t ret = aom_codec_enc_config_default(aom_codec_iface_, &aom_cfg_, usage);
    if (ret) {
        printf("Failed to get config: %s\n", aom_codec_err_to_string(ret));
        return false;
    }
    aom_cfg_.rc_target_bitrate = 100;
    aom_cfg_.g_w = width_;
    aom_cfg_.g_h = height_;
    aom_cfg_.g_profile = 0;
    aom_cfg_.g_timebase.num = 1;
    aom_cfg_.g_input_bit_depth = 8;
    aom_cfg_.g_timebase.den = 1;

    // Initialize codec
    if (aom_codec_enc_init(&aom_codec_, aom_codec_iface_, &aom_cfg_, 0)) {
        printf("Failed to initialize encoder\n");
        return false;
    }
    if (aom_codec_control(&aom_codec_, AOME_SET_CPUUSED, speed)) {
        printf("Failed to set cpu-used\n");
        return false;
    }

    return true;
}

bool Av1Encoder::EncodeOneBuf(cv::Mat *yuvMat, Str *resStr) {
    TimeMeasurer tm;
    uint8_t *yuv_buffer = (uint8_t *) yuvMat->data;
    memcpy(aom_image_raw_.planes[0], yuv_buffer, width_ * height_ * 3 / 2);

    int frame_cnt = 1;
    int flags = 0;
    const int speed = 7;

    // Initialize codec
    if (aom_codec_enc_init(&aom_codec_, aom_codec_iface_, &aom_cfg_, 0)) {
        printf("Failed to initialize encoder\n");
        return false;
    }

    if (aom_codec_control(&aom_codec_, AOME_SET_CPUUSED, speed)) {
        printf("Failed to set cpu-used\n");
        return false;
    }
    //*/
    // Encode av1
    aom_codec_err_t ret = aom_codec_encode(&aom_codec_, &aom_image_raw_, frame_cnt, 1, flags);
    if (ret != AOM_CODEC_OK) {
        return false;
    }

    /*
    ret = aom_codec_encode(&aom_codec_, NULL, 0, 1, flags);
    if (ret != AOM_CODEC_OK) {
        return false;
    }
    //*/

    int payload_size = 0;
    aom_codec_iter_t iter = NULL;
    const aom_codec_cx_pkt_t *aom_pkt = NULL;
    while ((aom_pkt = aom_codec_get_cx_data(&aom_codec_, &iter))) {
        switch (aom_pkt->kind) {
            printf("aom_pkt->kind:%d\n", aom_pkt->kind);
            case AOM_CODEC_CX_FRAME_PKT:
                //WriteIvfFrameHeader((char *) resStr->data + payload_size, aom_pkt);
                //payload_size += IVF_FRAME_HDR_SZ;
                memcpy((char *) resStr->data + payload_size, aom_pkt->data.frame.buf, aom_pkt->data.frame.sz);
                payload_size += aom_pkt->data.frame.sz;
                // break;
            default:
                break;
        }
    }

    resStr->size = payload_size;
    aom_codec_destroy(&aom_codec_);

    LOG_INFO("av1.encode.cost: %lu,%d,%s", tm.Elapsed(), payload_size, aom_codec_err_to_string(aom_codec_.err));
    return true;
}
