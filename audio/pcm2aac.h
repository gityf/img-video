//
// Created by wangyaofu on 2018/10/12.
//

#pragma once

class Pcm2Aac {
public:
    Pcm2Aac();
    ~Pcm2Aac();

    int Encode(char *inPcmBuff, int inPcmBuffSize, unsigned char *outAacBuff, int outAacBuffSize);
};