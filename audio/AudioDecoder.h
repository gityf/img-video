/*
** Copyright (C) 2018-2023 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of AacDecoder.
*/
#ifndef IMG_VIDEO_AACDECODER_H
#define IMG_VIDEO_AACDECODER_H

#include <string>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus  */

#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libavcodec/avcodec.h"

#ifdef __cplusplus
}
#endif /* __cplusplus  */

class AudioDecoder {
public:
    AudioDecoder();

    ~AudioDecoder();

    void InitParam(int pcm_format, int pcm_sample_rate, int pcm_channels, int pcm_bit_rate, AVCodecID codec_id) {
        pcm_format_ = pcm_format;
        pcm_sample_rate_ = pcm_sample_rate;
        pcm_channels_ = pcm_channels;
        pcm_bit_rate_ = pcm_bit_rate;
        codec_id_ = codec_id;
    }

    /**
     * Decode
     * @param aac in aac buffer
     * @param aac_len aac buffer length
     * @param out pcm buffer
     * @param format pcm AVSampleFormat
     * @return
     *  out_pcm pcm out
     */
    int Decode(unsigned char *aac, int aac_len, std::string &out, int &format);

private:


    /*
     * Set output sample format
     *    1:  16 bit PCM data (default). FAAD_FMT_16BIT
     *    2:  24 bit PCM data. FAAD_FMT_24BIT
     *    3:  32 bit PCM data. FAAD_FMT_32BIT
     *    4:  32 bit floating point data. FAAD_FMT_FLOAT
     *    5:  64 bit floating point data.
     */
    int pcm_format_;
    // Force the sample rate to X (for RAW files). 44100, 48000
    int pcm_sample_rate_;
    int pcm_channels_;
    int pcm_bit_rate_;
    AVCodecID codec_id_;
};


#endif //IMG_VIDEO_AACDECODER_H
