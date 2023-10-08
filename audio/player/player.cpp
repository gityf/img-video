/*
** Copyright (C) 2018-2023 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of Main process.
*/

#include "../AudioPlayer.h"
#include "../AudioDecoder.h"
#include "../AacEncoder.h"
#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void sig_action(int s) {
    printf("process exit!!!");
    fflush(stdout);
    exit(0);
}

std::string loadFile(const std::string &file) {
    std::ifstream data_file(file, std::ifstream::binary);
    if (data_file.is_open()) {
        std::stringstream buffer;
        buffer << data_file.rdbuf();
        return buffer.str();
    } else {
        return "";
    }
}

int overWriteFile(const std::string &file, const char *dataPtr, int len) {
    std::ofstream ofs(file, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
    if (ofs.is_open() && ofs.good()) {
        ofs.write(dataPtr, len);
    }
    ofs.close();
    return len;
}

int pcm2aac(const std::string &pcmData) {
    AacEncoder *aacEncoder = new AacEncoder();
    std::string outAac;
    aacEncoder->Encode(const_cast<char *>(pcmData.data()), pcmData.length(), outAac);
    printf("pcm2aac aacLen:%lu\n", outAac.length());
    overWriteFile("pcm-aac.aac", outAac.data(), outAac.length());
    delete aacEncoder;
    return 0;
}

int playerPcm(const char *pcm_data, int len, int format) {
    AudioPlayer *audioPlayer = new AudioPlayer();
    audioPlayer->SetAudioData(pcm_data, len);
    audioPlayer->InitParams(2, 44100, 16, format);
    audioPlayer->Start();
    while (!audioPlayer->IsEnd()) {
        SDL_Delay(10);
    }
    audioPlayer->Stop();
    delete audioPlayer;
    return 0;
}

int main(int argc, char *argv[]) {
    signal(SIGINT, sig_action);
    signal(SIGTERM, sig_action);
    if (argc < 3) {
        printf("Usage: %s audio-file aac\n", argv[0]);
        return 0;
    }
    std::string audioData = loadFile(argv[1]);
    AVCodecID codec_id = AV_CODEC_ID_NONE;
    int pcm_sample_rate = 44100;
    int bit_rate = 1024;
    if (strcmp(argv[2], "aac") == 0) {
        codec_id = AV_CODEC_ID_AAC;
    } else if (strcmp(argv[2], "opus") == 0) {
        codec_id = AV_CODEC_ID_OPUS;
        pcm_sample_rate = 48000;
        bit_rate = 101679;
    }
    if (codec_id != AV_CODEC_ID_NONE) {
        AudioDecoder *audioDecoder = new AudioDecoder();
        audioDecoder->InitParam(32, pcm_sample_rate, 2, bit_rate, codec_id);
        std::string pcmData;
        int format = AUDIO_F32SYS;
        audioDecoder->Decode((unsigned char *) const_cast<char *>(audioData.data()), audioData.length(), pcmData,
                             format);
        if (format == 8) {
            // AV_SAMPLE_FMT_FLTP = 8
            format = AUDIO_F32SYS;
        }
        playerPcm(pcmData.data(), pcmData.length(), format);
        delete audioDecoder;
    } else if (strcmp(argv[2], "pcm") == 0) {
        playerPcm(audioData.data(), audioData.length(), AUDIO_F32SYS);
    } else if (strcmp(argv[2], "pcm2aac") == 0) {
        pcm2aac(audioData);
    }

    return 0;
}