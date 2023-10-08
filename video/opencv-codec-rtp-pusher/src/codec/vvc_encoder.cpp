/*
** Copyright (C) 2018 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class X264Encoder.
*/

#include "vvc_encoder.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "../common/timemeasurer.h"
#include "../common/log.h"

VvcEncoder::VvcEncoder() {
}

VvcEncoder::~VvcEncoder() {
    if (NULL != vvc_encoder_) {
        vvenc_encoder_close(vvc_encoder_);
    }
    vvenc_YUVBuffer_free_buffer(&yuv_buffer_);
}


static void ff_vvenc_log_callback(void *avctx, int level, const char *fmt, va_list args) {
    vfprintf(level == 1 ? stderr : stdout, fmt, args);
}

bool VvcEncoder::InitEncoder(int width, int height, int channel, int fps, int fourcc) {
    width_ = width;
    height_ = height;
    channel_ = channel;
    fps_ = fps;
    fourcc_ = fourcc;
    vvenc_config_default(&params_);
    int framerate = 1;
    int bit_rate = 800;
    vvencPresetMode preset = VVENC_FASTER;                 // preset 0: faster  4: slower
    int qp = 32;                    // quantization parameter 0-63
    vvenc_init_default(&params_, width_, height_, framerate, bit_rate, qp, preset);
    params_.m_RCTargetBitrate = 1400;
    params_.m_profile = VVENC_PROFILE_AUTO;
    params_.m_FrameRate = 1;
    params_.m_FrameScale = 1;
    params_.m_RCNumPasses = 1;
    params_.m_inputBitDepth[0] = 8;
    params_.m_internalBitDepth[0] = 8;
    params_.m_outputBitDepth[0] = 8;
    params_.m_GOPSize = 1;
    params_.m_IntraPeriod = 1;
    //params_.m_levelTier = VVENC_TIER_MAIN;
    params_.m_AccessUnitDelimiter = true;
    params_.m_internChromaFormat = VVENC_CHROMA_420;
    params_.m_vuiParametersPresent = 1;
    params_.m_colourDescriptionPresent = true;

    vvc_encoder_ = vvenc_encoder_create();
    if (NULL == vvc_encoder_) {
        printf("Fail to vvenc_encoder_create\n");
        return false;
    }

    vvenc_set_msg_callback(&params_, vvc_encoder_, ff_vvenc_log_callback);
    int ret = vvenc_encoder_open(vvc_encoder_, &params_);
    if (0 != ret) {
        printf("cannot open vvc encoder (vvenc): %s\n", vvenc_get_last_error(vvc_encoder_));
        return false;
    }
    // get the adapted config, because changes are needed for the yuv reader
    // (uninitialized parameter may be set during vvenc_encoder_open)
    vvenc_get_config(vvc_encoder_, &params_);

    printf("VVC:Conf: %s\n", vvenc_get_config_as_string(&params_, VVENC_DETAILS));

    vvenc_accessUnit_default(&vvenc_access_unit_);
    vvenc_accessUnit_alloc_payload(&vvenc_access_unit_, width_ * height_);
    vvenc_print_summary(vvc_encoder_);

    vvenc_YUVBuffer_default(&yuv_buffer_);
    // VVENC_CHROMA_420        = 1,
    vvenc_YUVBuffer_alloc_buffer(&yuv_buffer_, VVENC_CHROMA_420, width_, height_);
    return true;
}

bool VvcEncoder::EncodeOneBuf(cv::Mat *yuvMat, Str *resStr) {
    TimeMeasurer tm;
    uint8_t *yuv_buffer = (uint8_t *) yuvMat->data;

    int i = 0;
    for (i = 0; i < width_ * height_; i++) {
        yuv_buffer_.planes[0].ptr[i] = (uint16_t) yuv_buffer[i];
    }
    i = width_ * height_;
    for (; i < width_ * height_ + width_ * height_ / 4; i++) {
        yuv_buffer_.planes[1].ptr[i] = (uint16_t) yuv_buffer[i];
    }
    i = width_ * height_ + width_ * height_ / 4;
    for (; i < width_ * height_ + width_ * height_ / 2; i++) {
        yuv_buffer_.planes[2].ptr[i] = (uint16_t) yuv_buffer[i];
    }
    yuv_buffer_.cts = 1;
    yuv_buffer_.ctsValid = true;

    bool encodeDone = false;
    // flushing encoder
    yuv_buffer_.sequenceNumber = 0;
    int ret = vvenc_encode(vvc_encoder_, &yuv_buffer_, &vvenc_access_unit_, &encodeDone);
    if (ret != 0) {
        printf("error in vvenc::encode - ret:%d\n", ret);
        printf("vvc encoder (vvenc) err: %s\n", vvenc_get_last_error(vvc_encoder_));
        return false;
    }
    resStr->size = 0;
    memcpy((char *) resStr->data+resStr->size, vvenc_access_unit_.payload, vvenc_access_unit_.payloadUsedSize);
    resStr->size += vvenc_access_unit_.payloadUsedSize;

    LOG_INFO("vvc.encode.cost: %lu, size: %d, sliceType:%d", tm.Elapsed(), resStr->size, vvenc_access_unit_.sliceType);
    vvenc_accessUnit_reset(&vvenc_access_unit_);
    return true;
}
