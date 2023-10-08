/*
** Copyright (C) 2018-2023 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of AudioPlayer.
*/

#ifndef IMG_VIDEO_AUDIO_PLAYER_H
#define IMG_VIDEO_AUDIO_PLAYER_H

#include "SDL2/SDL.h"

class AudioPlayer {
public:
    AudioPlayer();

    ~AudioPlayer();

    int InitParams(int channel, int sample_rate, int bytes_per_sample, int format);

    void SetAudioData(const char *pcm_data, int len) {
        printf("SetAudioData len:%d\n", len);
        pcm_data_ = pcm_data;
        pcm_data_len_ = len;
    }

    bool IsEnd() {
        return pcm_data_offset_ >= pcm_data_len_;
    }

    int Start();

    int Pause();

    int Stop();

private:
    SDL_AudioDeviceID audio_dev_;
    int channel_ = 0;
    int sample_rate_ = 0;
    int bytes_per_sample_ = 0;
public:
    const char *pcm_data_ = NULL;
    int pcm_data_len_ = 0;
    int pcm_data_offset_ = 0;
};


#endif //IMG_VIDEO_AUDIO_PLAYER_H
