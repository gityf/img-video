/*
** Copyright (C) 2018-2023 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of AccEncoder.
*/

#pragma once

#include <string>

class AacEncoder {
public:
    AacEncoder();

    ~AacEncoder();

    /**
     * Encode pcm to aac
     * @param inPcmBuff
     * @param inPcmBuffSize
     * @param outAac
     * @return
     */
    int Encode(char *inPcmBuff, int inPcmBuffSize, std::string &outAac);
};