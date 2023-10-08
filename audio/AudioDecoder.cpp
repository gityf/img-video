/*
** Copyright (C) 2018-2023 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of AacDecoder.
*/

#include "AudioDecoder.h"

AudioDecoder::AudioDecoder() {

}

AudioDecoder::~AudioDecoder() {

}

typedef struct AACDContext {
    AVCodecContext *pCodecCtx;
    AVFrame *pFrame;
    AVCodecParserContext *pParser;
} AACDContext;

static AACDContext *
aac_decoder_create(AVCodecID codecid, int sample_rate, int channels, int bit_rate) {
    AACDContext *aacdContext = (AACDContext *) malloc(sizeof(AACDContext));
    const AVCodec *pCodec = avcodec_find_decoder(codecid);//AV_CODEC_ID_AAC
    if (pCodec == NULL) {
        printf("find %d decoder error\n", codecid);
        return 0;
    }
    printf("aac_decoder_create codecid=%d\n", codecid);
    // 创建显示contedxt
    aacdContext->pCodecCtx = avcodec_alloc_context3(pCodec);
    aacdContext->pCodecCtx->channels = channels;
    aacdContext->pCodecCtx->sample_rate = sample_rate;
    aacdContext->pCodecCtx->bit_rate = bit_rate;
    aacdContext->pCodecCtx->bits_per_coded_sample = 0;
    printf("bits_per_coded_sample:%d\n", aacdContext->pCodecCtx->bits_per_coded_sample);
    if (avcodec_open2(aacdContext->pCodecCtx, pCodec, NULL) < 0) {
        printf("open codec error\r\n");
        return 0;
    }

    aacdContext->pFrame = av_frame_alloc();
    aacdContext->pParser = av_parser_init(codecid);
    printf("aac_decoder_create end\n");
    return aacdContext;
}

static int
aac_decode_frame(AACDContext *pContext, unsigned char *pData, int nLen, std::string &outPcm, AVSampleFormat &pcmFormat) {
    int dst_len = 0;
    AVPacket packet;
    av_init_packet(&packet);
    printf("aac_decode_frame nLen=%d\n", nLen);
    packet.size = nLen;
    packet.data = pData;
    packet.flags = AV_SAMPLE_FMT_FLTP;
    /*
    av_parser_parse2(pParam->pParser, pParam->pCodecCtx, &packet.data, &packet.size,
                     pData, nLen, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
    //*/

    int ret = avcodec_send_packet(pContext->pCodecCtx, &packet);
    if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
        printf("Failed to avcodec_send_packet ret: %d\n", ret);
        return -1;
    }

    while (ret >= 0) {
        ret = avcodec_receive_frame(pContext->pCodecCtx, pContext->pFrame);
        if (ret < 0 && ret != AVERROR_EOF) {
            printf("Failed to avcodec_receive_frame ret: %d\n", ret);
            return -1;
        }
        pcmFormat = (AVSampleFormat) pContext->pFrame->format;
        // 采样位数 4位
        int data_size = av_get_bytes_per_sample(pcmFormat);
        if (data_size < 0) {
            /* This should not occur, checking just for paranoia */
            printf("Failed to calculate data size\n");
            return -1;
        }
        for (int i = 0; i < pContext->pFrame->nb_samples; i++) {
            // 通道数遍历
            for (int ch = 0; ch < pContext->pCodecCtx->channels; ch++) {
                outPcm.append((char *) pContext->pFrame->data[ch] + data_size * i, data_size);
                dst_len += data_size;
            }
        }
    }

    printf("aac_decode_frame outLen=%d, pcmFormat:%d\n", dst_len, pcmFormat);

    av_packet_unref(&packet);

    return dst_len;
}

static void aac_decode_close(AACDContext *pContext) {
    if (pContext == NULL) {
        return;
    }

    if (pContext->pFrame != NULL) {
        av_frame_free(&pContext->pFrame);
        pContext->pFrame = NULL;
    }

    if (pContext->pCodecCtx != NULL) {
        avcodec_close(pContext->pCodecCtx);
        avcodec_free_context(&pContext->pCodecCtx);
        pContext->pCodecCtx = NULL;
    }

    free(pContext);
    pContext = NULL;
}

int AudioDecoder::Decode(unsigned char *aac, int aac_len, std::string &out, int &format) {
    AVSampleFormat pcm_format = AV_SAMPLE_FMT_FLTP;
    AACDContext *pContext = aac_decoder_create(codec_id_, pcm_sample_rate_, pcm_channels_, pcm_bit_rate_);
    int pcm_len = aac_decode_frame(pContext, aac, aac_len, out, pcm_format);
    aac_decode_close(pContext);
    format = (int) pcm_format;
    return pcm_len;
}