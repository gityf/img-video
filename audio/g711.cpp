//
// Created by wangyaofu on 2018/10/12.
//

#include "g711.h"

const static int kMaxPcmValue = 32635;

#define    SIGN_BIT    (0x80)        /* Sign bit for a A-law byte. */
#define    QUANT_MASK    (0xf)        /* Quantization field mask. */
#define    NSEGS        (8)            /* Number of A-law segments. */
#define    SEG_SHIFT    (4)            /* Left shift for segment number. */
#define    SEG_MASK    (0x70)        /* Segment field mask. */

static short seg_end[8] = {0xFF, 0x1FF, 0x3FF, 0x7FF,
                           0xFFF, 0x1FFF, 0x3FFF, 0x7FFF};

/* copy from CCITT G.711 specifications */
unsigned char _u2a[128] = {            /* u- to A-law conversions */
        1, 1, 2, 2, 3, 3, 4, 4,
        5, 5, 6, 6, 7, 7, 8, 8,
        9, 10, 11, 12, 13, 14, 15, 16,
        17, 18, 19, 20, 21, 22, 23, 24,
        25, 27, 29, 31, 33, 34, 35, 36,
        37, 38, 39, 40, 41, 42, 43, 44,
        46, 48, 49, 50, 51, 52, 53, 54,
        55, 56, 57, 58, 59, 60, 61, 62,
        64, 65, 66, 67, 68, 69, 70, 71,
        72, 73, 74, 75, 76, 77, 78, 79,
        81, 82, 83, 84, 85, 86, 87, 88,
        89, 90, 91, 92, 93, 94, 95, 96,
        97, 98, 99, 100, 101, 102, 103, 104,
        105, 106, 107, 108, 109, 110, 111, 112,
        113, 114, 115, 116, 117, 118, 119, 120,
        121, 122, 123, 124, 125, 126, 127, 128
};

unsigned char _a2u[128] = {            /* A- to u-law conversions */
        1, 3, 5, 7, 9, 11, 13, 15,
        16, 17, 18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29, 30, 31,
        32, 32, 33, 33, 34, 34, 35, 35,
        36, 37, 38, 39, 40, 41, 42, 43,
        44, 45, 46, 47, 48, 48, 49, 49,
        50, 51, 52, 53, 54, 55, 56, 57,
        58, 59, 60, 61, 62, 63, 64, 64,
        65, 66, 67, 68, 69, 70, 71, 72,
        73, 74, 75, 76, 77, 78, 79, 79,
        80, 81, 82, 83, 84, 85, 86, 87,
        88, 89, 90, 91, 92, 93, 94, 95,
        96, 97, 98, 99, 100, 101, 102, 103,
        104, 105, 106, 107, 108, 109, 110, 111,
        112, 113, 114, 115, 116, 117, 118, 119,
        120, 121, 122, 123, 124, 125, 126, 127
};

/* G711 A-law to u-law conversion */
unsigned char alaw2ulaw(unsigned char aval) {
    aval &= 0xff;
    return ((aval & 0x80) ? (0xFF ^ _a2u[aval ^ 0xD5]) :
            (0x7F ^ _a2u[aval ^ 0x55]));
}

/* G711 u-law to A-law conversion */
unsigned char ulaw2alaw(unsigned char uval) {
    uval &= 0xff;
    return ((uval & 0x80) ? (0xD5 ^ (_u2a[0xFF ^ uval] - 1)) :
            (0x55 ^ (_u2a[0x7F ^ uval] - 1)));
}

G711::G711() {

}

G711::~G711() {

}

int G711::Pcm2G711a(unsigned char *outCodecBits, const char *pcmBuff, int pcmBuffSize) {
    short *buffer = static_cast<short *>(pcmBuff);
    for (int i = 0; i < pcmBuffSize / 2; i++) {
        outCodecBits[i] = EncodeToG711(buffer[i]);
    }

    return pcmBuffSize / 2;
}

int G711::Pcm2G711u(unsigned char *outCodecBits, const char *pcmBuff, int pcmBuffSize) {
    short *buffer = static_cast<short *>(pcmBuff);
    for (int i = 0; i < pcmBuffSize / 2; i++) {
        outCodecBits[i] = alaw2ulaw(EncodeToG711(buffer[i]));
    }

    return pcmBuffSize / 2;
}

int G711::G711a2Pcm(char *outPcmBits, unsigned char *g711Buff, int g711BuffSize) {
    short *out_data = (short *) outPcmBits;

    for (int i = 0; i < g711BuffSize; i++) {
        out_data[i] = DecodeFromG711a(ulaw2alaw(g711Buff[i]));
    }

    return g711BuffSize * 2;
}

int G711::G711u2Pcm(char *outPcmBits, unsigned char *g711Buff, int g711BuffSize) {
    short *out_data = (short *) outPcmBits;

    for (int i = 0; i < g711BuffSize; i++) {
        out_data[i] = DecodeFromG711a(ulaw2alaw(g711Buff[i]));
    }

    return g711BuffSize * 2;
}

unsigned char G711::EncodeToG711a(short pcm) {
    int sign = (pcm & 0x8000) >> 8;
    if (sign != 0)
        pcm = -pcm;
    if (pcm > kMaxPcmValue) pcm = kMaxPcmValue;
    int exponent = 7;
    int expMask;
    for (expMask = 0x4000; (pcm & expMask) == 0
                           && exponent > 0; exponent--, expMask >>= 1) {}
    int mantissa = (pcm >> ((exponent == 0) ? 4 : (exponent + 3))) & 0x0f;
    unsigned char alaw = (unsigned char) (sign | exponent << 4 | mantissa);
    return (unsigned char) (alaw ^ 0xD5);
}

short G711::DecodeFromG711a(unsigned char alaw) {
    alaw ^= 0x55;
    int seg = (alaw & 0x70) >> 4;
    int data = (alaw & 0x0f) << 4;
    data += 8;
    if (seg != 0) {
        data += 0x100;
    }
    if (seg > 1) {
        data <<= (seg - 1);
    }

    return (short) ((alaw & 0x80) ? data : -data);
}
