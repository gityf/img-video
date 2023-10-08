/*
** Copyright (C) 2018 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of class X265Encoder.
*/

#ifndef IMG_VIDEO_VPX_ENCODER_H
#define IMG_VIDEO_VPX_ENCODER_H

#include <stdint.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "common/Str.h"
#include "opencv_codec.h"

#define VPX_CODEC_DISABLE_COMPAT 1

#include "vpx/vpx_encoder.h"
#include "vpx/vp8cx.h"

#define vp8_fourcc    0x30385056
#define vp9_fourcc    0x30395056

class VpxEncoder : public Encoder {
public:
    VpxEncoder();

    ~VpxEncoder();

    bool InitEncoder(int width, int height, int channel = 3, int fps = 25, int fourcc = 0);

    virtual bool EncodeOneBuf(cv::Mat *yuvMat, Str *resStr);

private:
    // outHeader[32]
    void WriteIvfFileHeader(char *outHeader, const vpx_codec_enc_cfg_t *cfg, int frame_cnt);

    // outHeader【12】
    void WriteIvfFrameHeader(char *outHeader, const vpx_codec_cx_pkt_t *pkt);

    vpx_codec_ctx_t vpx_codec_;
    vpx_codec_enc_cfg_t vpx_cfg_;
    vpx_image_t vpx_image_raw_;
    vpx_codec_iface_t *vpx_codec_iface_;
    uint8_t *yuv_buffer_;
};

#endif //IMG_VIDEO_VPX_ENCODER_H
