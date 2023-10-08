//
// Created by wangyaofu on 2018/10/12.
//

#include "AacEncoder.h"
#include <stdio.h>
#include <faac.h>

AacEncoder::AacEncoder() {}

AacEncoder::~AacEncoder() {}

int AacEncoder::Encode(char *inPcmBuff, int inPcmBuffSize, std::string &outAac) {
    faacEncHandle hEncoder;
    faacEncConfigurationPtr pConfiguration;

    unsigned int objectType = LOW;
    unsigned int mpegVersion = MPEG2;
    static unsigned int useTns = 0; //#define DEFAULT_TNS     0
    unsigned int nChannels = 2;
    unsigned long nInputSamples = 0;
    unsigned long nSampleRate = 44100;
    unsigned long nMaxOutputBytes = 0;

    /*open FAAC engine*/
    hEncoder = faacEncOpen(nSampleRate, nChannels, &nInputSamples, &nMaxOutputBytes);

    /*get current encoding configuration*/
    pConfiguration = faacEncGetCurrentConfiguration(hEncoder);
    pConfiguration->inputFormat = FAAC_INPUT_32BIT;

    /*0 - raw; 1 - ADTS*/
    pConfiguration->outputFormat = 1;
    pConfiguration->useTns = useTns;
    pConfiguration->aacObjectType = objectType;
    pConfiguration->mpegVersion = mpegVersion;

    /*set encoding configuretion*/
    faacEncSetConfiguration(hEncoder, pConfiguration);

    unsigned char *outAacBuff = (unsigned char *) malloc(nMaxOutputBytes);
    memset(outAacBuff, 0, nMaxOutputBytes);


    nInputSamples = (inPcmBuffSize / (32 / 8) / 4);
    printf("nInputSamples:%lu\n", nInputSamples);
    unsigned long offset = 0;
    while (offset < inPcmBuffSize) {
        int ret = faacEncEncode(hEncoder, (int *) (inPcmBuff + offset), nInputSamples, outAacBuff, nMaxOutputBytes);
        printf("ret:%d\n", ret);
        offset += nMaxOutputBytes;
        if (ret > 0) {
            outAac.append((char *) outAacBuff, ret);
        }
    }
    faacEncClose(hEncoder);
    free(outAacBuff);
    return 0;
}