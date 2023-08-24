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
#define VPX_CODEC_DISABLE_COMPAT 1
#include "vpx/vpx_encoder.h"
#include "vpx/vp8cx.h"

#define vp8_fourcc    0x30385056
#define vp9_fourcc    0x30395056

class VpxEncoder {
public:

    VpxEncoder();

    VpxEncoder(int videoWidth, int videoHeight, int channel, int fps);

    ~VpxEncoder();

    /** 创建X265编码器
     * @param[in] videoWidth  视频宽度
     * @param[in] videoHeight 视频高度
     * @param[in] fps 帧率
     * @return 成功返回true, 失败返回false.
     */
    bool Create(int videoWidth, int videoHeight, int channel = 3, int fps = 30);

    bool InitEncoder(int fourcc = vp8_fourcc);

    /** 编码一帧
     * @param[in] frame 输入的一帧图像
     * @return 返回编码后数据尺寸, 0表示编码失败
     */
    bool EncodeOneFrame(cv::Mat* frame, Str* respStr);

    bool EncodeOneBuf(cv::Mat *yuvMat, Str* resStr);

private:

    void Init();

    // outHeader[32]
    void WriteIvfFileHeader(char *outHeader, const vpx_codec_enc_cfg_t *cfg, int frame_cnt);
    // outHeader【12】
    void WriteIvfFrameHeader(char *outHeader, const vpx_codec_cx_pkt_t *pkt);

    vpx_codec_ctx_t vpx_codec_;
    vpx_codec_enc_cfg_t vpx_cfg_;
    vpx_image_t vpx_image_raw_;
    vpx_codec_iface_t *vpx_codec_iface_;
    int fourcc_;

    uint8_t *yuv_buffer_;

public:
    int m_width;
    int m_height;
    int m_channel;
    int m_fps;
};

#endif //IMG_VIDEO_VPX_ENCODER_H
