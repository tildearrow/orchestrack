/* https://github.com/schismtracker/schismtracker/blob/master/scripts/lutgen.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// quantizer scale of window coefs
#define WFIR_QUANTBITS      15
#define WFIR_QUANTSCALE     (1L << WFIR_QUANTBITS)
#define WFIR_8SHIFT         (WFIR_QUANTBITS - 8)
#define WFIR_16BITSHIFT     (WFIR_QUANTBITS)

// log2(number)-1 of precalculated taps range is [4..12]
#define WFIR_FRACBITS       10
#define WFIR_LUTLEN         ((1L << (WFIR_FRACBITS + 1)) + 1)

// number of samples in window
#define WFIR_LOG2WIDTH      3
#define WFIR_WIDTH          (1L << WFIR_LOG2WIDTH)
#define WFIR_SMPSPERWING    ((WFIR_WIDTH - 1) >> 1)

// cutoff (1.0 == pi/2)
#define WFIR_CUTOFF         0.90f

// wfir type
#define WFIR_HANN           0
#define WFIR_HAMMING        1
#define WFIR_BLACKMANEXACT  2
#define WFIR_BLACKMAN3T61   3
#define WFIR_BLACKMAN3T67   4
#define WFIR_BLACKMAN4T92   5
#define WFIR_BLACKMAN4T74   6
#define WFIR_KAISER4T       7
#define WFIR_TYPE           WFIR_BLACKMANEXACT

// wfir help
#ifndef M_zPI
#define M_zPI               3.1415926535897932384626433832795
#endif
#define M_zEPS              1e-8
#define M_zBESSELEPS        1e-21

//float windowed_fir_lut[WFIR_LUTLEN*WFIR_WIDTH];
float coef(int pc_nr, float p_ofs, float p_cut, int p_width, int p_type);
void windowed_fir_init(float* where);
