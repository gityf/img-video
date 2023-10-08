/*
** Copyright (C) 2018-2023 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of AudioPlayer.
*/

#include "AudioPlayer.h"

AudioPlayer::AudioPlayer() {
    SDL_Init(SDL_INIT_AUDIO);
}

AudioPlayer::~AudioPlayer() {

}

static void SdlAudioCallback(void *opaque, unsigned char *out_stream, int len) {
    AudioPlayer *audioPlayer = (AudioPlayer *) opaque;
    if (len > audioPlayer->pcm_data_len_ - audioPlayer->pcm_data_offset_) {
        len = audioPlayer->pcm_data_len_ - audioPlayer->pcm_data_offset_;
    }
    memcpy(out_stream, audioPlayer->pcm_data_ + audioPlayer->pcm_data_offset_, len);
    audioPlayer->pcm_data_offset_ += len;
    //printf("SdlAudioCallback: %p, len: %d, offset: %d\n", opaque, len, audioPlayer->pcm_data_offset_);
}

int AudioPlayer::InitParams(int channel, int sample_rate, int bytes_per_sample, int format) {
    SDL_ClearQueuedAudio(audio_dev_);
    SDL_CloseAudioDevice(audio_dev_);

    SDL_AudioSpec wanted_spec, spec;
    SDL_zero(wanted_spec);
    wanted_spec.channels = channel;
    wanted_spec.freq = sample_rate;
    // format is same to pcm codec， such as 'pcm_f32le=AUDIO_F32SYS', 'pcm_s32le=AUDIO_S32SYS', ...
    wanted_spec.format = format;
    wanted_spec.silence = 0;
    wanted_spec.samples = 1024;
    wanted_spec.callback = SdlAudioCallback;
    wanted_spec.userdata = this;

    if (SDL_OpenAudio(&wanted_spec, NULL) < 0) {
        printf("SDL_OpenAudio err: %s\n", SDL_GetError());
    }
    return 0;
}

int AudioPlayer::Start() {
    SDL_PauseAudio(0);
    printf("Start res: %s\n", SDL_GetError());
    return 0;
}

int AudioPlayer::Pause() {
    SDL_PauseAudio(1);
    printf("Pause res: %s\n", SDL_GetError());
    return 0;
}

int AudioPlayer::Stop() {
    SDL_CloseAudio();
    printf("Stop res: %s\n", SDL_GetError());
    return 0;
}