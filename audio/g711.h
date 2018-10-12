//
// Created by wangyaofu on 2018/10/12.
//

#pragma once

enum EG711Type {
    EG711TypeAlaw = 1,
    EG711TypeUlaw = 2
};

class G711 {
public:
    G711();

    ~G711();

    /**
     * To convert pcm buffer to G711 buffer.
     *
     * @param outCodecBits
     * @param pcmBuff
     * @param pcmBuffSize
     * @param toType
     * @return
     */
    int Pcm2G711(unsigned char *outCodecBits, const char *pcmBuff, int pcmBuffSize, EG711Type toType = EG711TypeAlaw);

    /**
     * To convert g711 buffer to pcm buffer.
     *
     * @param outPcmBits
     * @param g711Buff
     * @param g711BuffSize
     * @param toType
     * @return
     */
    int G7112Pcm(char *outPcmBits, unsigned char *g711Buff, int g711BuffSize, EG711Type toType = EG711TypeAlaw);

private:

    /**
     * To encode pcm bits(16) to G711 alaw codec.
     *
     * @param pcm
     * @return
     */
    unsigned char EncodeToG711(short pcm);

    /**
     * To decode G711 alaw codec to pcm bits(16).
     * @param alaw
     * @return
     */
    short DecodeFromG711(unsigned char alaw);
};
