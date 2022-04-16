/*************************************************************************
	> File Name    : softfloat-type.h
	> Author       : jiachao
	> Mail         : jiachao@digiproto.com
	> Created Time : 2021/10/26 10:46:59
************************************************************************/

#ifndef _SOFTFLOAT_TYPE_H
#define _SOFTFLOAT_TYPE_H

#include <skyeye_types.h>

/* This 'flag' type must be able to hold at least 0 and 1. It should
 * probably be replaced with 'bool' but the uses would need to be audited
 * to check that they weren't accidentally relying on it being a larger type.
 */
typedef uint8_t flag;

/*
 * Software IEC/IEEE floating-point types.
 */

typedef uint16_t float16;
typedef uint32_t float32;
typedef uint64_t float64;
#define float16_val(x) (x)
#define float32_val(x) (x)
#define float64_val(x) (x)
#define make_float16(x) (x)
#define make_float32(x) (x)
#define make_float64(x) (x)
#define const_float16(x) (x)
#define const_float32(x) (x)
#define const_float64(x) (x)
typedef struct
{
    uint64_t low;
    uint16_t high;
} floatx80;
#define make_floatx80(exp, mant) ((floatx80) { mant, exp })
#define make_floatx80_init(exp, mant) { .low = mant, .high = exp }
typedef struct
{
#ifdef HOST_WORDS_BIGENDIAN
    uint64_t high, low;
#else
    uint64_t low, high;
#endif
} float128;
#define make_float128(high_, low_) ((float128) { .high = high_, .low = low_ })
#define make_float128_init(high_, low_) { .high = high_, .low = low_ }

/*
 * Software IEC/IEEE floating-point underflow tininess-detection mode.
 */

enum
{
    float_tininess_after_rounding  = 0,
    float_tininess_before_rounding = 1
};

/*
 *Software IEC/IEEE floating-point rounding mode.
 */

enum
{
    float_round_nearest_even = 0,
    float_round_down         = 1,
    float_round_up           = 2,
    float_round_to_zero      = 3,
    float_round_ties_away    = 4,
    /* Not an IEEE rounding mode: round to the closest odd mantissa value */
    float_round_to_odd       = 5,
};

/*
 * Software IEC/IEEE floating-point exception flags.
 */

enum
{
    float_flag_invalid   =  1,
    float_flag_divbyzero =  4,
    float_flag_overflow  =  8,
    float_flag_underflow = 16,
    float_flag_inexact   = 32,
    float_flag_input_denormal = 64,
    float_flag_output_denormal = 128
};


/*
 * Floating Point Status. Individual architectures may maintain
 * several versions of float_status for different functions. The
 * correct status for the operation is then passed by reference to
 * most of the softfloat functions.
 */

typedef struct float_status
{
    signed char float_detect_tininess;
    signed char float_rounding_mode;
    uint8_t     float_exception_flags;
    signed char floatx80_rounding_precision;
    /* should denormalised results go to zero and set the inexact flag? */
    flag flush_to_zero;
    /* should denormalised inputs go to zero and set the input_denormal flag? */
    flag flush_inputs_to_zero;
    flag default_nan_mode;
    /* not always used -- see snan_bit_is_one() in softfloat-specialize.h */
    flag snan_bit_is_one;
} float_status;
#endif
