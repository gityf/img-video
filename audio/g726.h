#pragma once

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus  */

/*! Bitstream handler state */
typedef struct bitstream_state_s {
    /*! The bit stream. */
    unsigned int bitstream;
    /*! The residual bits in bitstream. */
    int residue;
} bitstream_state_t;

typedef struct g726_state_s g726_state_t;

typedef short (*g726_decoder_func_t)(g726_state_t *s, unsigned char code);

typedef unsigned char (*g726_encoder_func_t)(g726_state_t *s, short amp);


/*!
* The following is the definition of the state structure
* used by the G.726 encoder and decoder to preserve their internal
* state between successive calls.  The meanings of the majority
* of the state structure fields are explained in detail in the
* CCITT Recommendation G.726.  The field names are essentially indentical
* to variable names in the bit level description of the coding algorithm
* included in this recommendation.
*/
struct g726_state_s {
    /*! The bit rate */
    int rate;
    /*! The external coding, for tandem operation */
    //int ext_coding;
    /*! The number of bits per sample */
    int bits_per_sample;
    /*! One of the G.726_PACKING_xxx options */
    //int packing;

    /*! Locked or steady state step size multiplier. */
    int yl;
    /*! Unlocked or non-steady state step size multiplier. */
    short yu;
    /*! short term energy estimate. */
    short dms;
    /*! Long term energy estimate. */
    short dml;
    /*! Linear weighting coefficient of 'yl' and 'yu'. */
    short ap;

    /*! Coefficients of pole portion of prediction filter. */
    short a[2];
    /*! Coefficients of zero portion of prediction filter. */
    short b[6];
    /*! Signs of previous two samples of a partially reconstructed signal. */
    short pk[2];
    /*! Previous 6 samples of the quantized difference signal represented in
    an internal floating point format. */
    short dq[6];
    /*! Previous 2 samples of the quantized difference signal represented in an
    internal floating point format. */
    short sr[2];
    /*! Delayed tone detect */
    int td;

    /*! \brief The bit stream processing context. */
    bitstream_state_t bs;

    /*! \brief The current encoder function. */
    g726_encoder_func_t enc_func;
    /*! \brief The current decoder function. */
    g726_decoder_func_t dec_func;
};

g726_state_t *g726_init(g726_state_t *s, int bit_rate);

int g726_decode(g726_state_t *s, short amp[], const unsigned char g726_data[], int g726_bytes);

int g726_encode(g726_state_t *s, unsigned char g726_data[], const short amp[], int len);

#ifdef __cplusplus
}
#endif /* __cplusplus  */