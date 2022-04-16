#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <fenv.h>
#pragma STDC FENV_ACCESS ON
#ifdef linux
#define FE_DENORMAL 0 // glibc doesn't support this macro
#endif

#include "ppc_cpu.h"
#include "ppc_dec.h"
#include "ppc_mmu.h"
#include "ppc_spe.h"
#include "ppc_tools.h"
#include "skyeye_log.h"
#include "skyeye_types.h"

#define SIGN   true
#define UNSIGN false
#define UPPER  true
#define LOWER  false
#define ROUND  true
#define TRUNC  false
#define F      true
#define I      false

static inline void ppc_clear_float_status()
{
    feclearexcept(FE_ALL_EXCEPT);
}

static inline void ppc_set_float_status_high(e500_core_t *current_core)
{
    int status = fetestexcept(FE_ALL_EXCEPT);

    if (status & FE_INVALID || status & FE_DENORMAL)
    {
        current_core->spefscr.FINVH = 1;
        current_core->spefscr.FINVS = 1;
        current_core->spefscr.FGH = 0;
        current_core->spefscr.FXH = 0;
    }
    if (status & FE_DIVBYZERO)
    {
        current_core->spefscr.FDBZH = 1;
        current_core->spefscr.FDBZS = 1;
    }
    if (status & FE_OVERFLOW)
    {
        current_core->spefscr.FOVFH = 1;
        current_core->spefscr.FOVFS = 1;
        current_core->spefscr.FGH = 0;
        current_core->spefscr.FXH = 0;
        if (current_core->spefscr.FOVFE == 0)
        {
            current_core->spefscr.FINXS = 1;
        }
    }
    if (status & FE_UNDERFLOW)
    {
        current_core->spefscr.FUNFH = 1;
        current_core->spefscr.FUNFS = 1;
        current_core->spefscr.FGH = 0;
        current_core->spefscr.FXH = 0;
        if (current_core->spefscr.FUNFE == 0)
        {
            current_core->spefscr.FINXS = 1;
        }
    }
    if (status & FE_INEXACT)
    {
        current_core->spefscr.FINXS = 1;
    }

#ifdef linux
    skyeye_log(Warning_log, __FUNCTION__, "FE_DENORMAL isn't supported on GNU/Linux platform. Some SPE floating status maybe incorrect.\n");
#endif
}

static inline void ppc_set_float_status(e500_core_t *current_core)
{
    int status = fetestexcept(FE_ALL_EXCEPT);

    if (status & FE_INVALID || status & FE_DENORMAL)
    {
        current_core->spefscr.FINV = 1;
        current_core->spefscr.FINVS = 1;
        current_core->spefscr.FG = 0;
        current_core->spefscr.FX = 0;
    }
    if (status & FE_DIVBYZERO)
    {
        current_core->spefscr.FDBZ = 1;
        current_core->spefscr.FDBZS = 1;
    }
    if (status & FE_OVERFLOW)
    {
        current_core->spefscr.FOVF = 1;
        current_core->spefscr.FOVFS = 1;
        current_core->spefscr.FG = 0;
        current_core->spefscr.FX = 0;
        if (current_core->spefscr.FOVFE == 0)
        {
            current_core->spefscr.FINXS = 1;
        }
    }
    if (status & FE_UNDERFLOW)
    {
        current_core->spefscr.FUNF = 1;
        current_core->spefscr.FUNFS = 1;
        current_core->spefscr.FG = 0;
        current_core->spefscr.FX = 0;
        if (current_core->spefscr.FUNFE == 0)
        {
            current_core->spefscr.FINXS = 1;
        }
    }
    if (status & FE_INEXACT)
    {
        current_core->spefscr.FINXS = 1;
    }

#ifdef linux
    skyeye_log(Warning_log, __FUNCTION__, "FE_DENORMAL isn't supported on GNU/Linux platform. Some SPE floating status maybe incorrect.\n");
#endif
}

static inline void ppc_switch_rounding_mode(e500_core_t *current_core)
{
    switch (current_core->spefscr.FRMC)
    {
        case 0:
            fesetround(FE_TONEAREST);
            break;
        case 1:
            fesetround(FE_TOWARDZERO);
            break;
        case 2:
            fesetround(FE_UPWARD);
            break;
        case 3:
            fesetround(FE_DOWNWARD);
            break;
    }
}

// Determine if fp value is a NaN or infinity
static inline bool Isa32NaNorInfinity(uint32 fp)
{
    return (ppc_opc_get_bits(fp, 1, 8) == 255);
}

static inline bool Isa32NaN(uint32 fp)
{
    return ((ppc_opc_get_bits(fp, 1, 8) == 255) & (ppc_opc_get_bits(fp, 9, 31) != 0));
}

static inline bool Isa32Infinity(uint32 fp)
{
    return ((ppc_opc_get_bits(fp, 1, 8) == 255) & (ppc_opc_get_bits(fp, 9, 31) == 0));
}

// Determine if fp value is denormalized
static inline bool Isa32Denorm(uint32 fp)
{
    return ((ppc_opc_get_bits(fp, 1, 8) == 0) & (ppc_opc_get_bits(fp, 9, 31) != 0));
}

// Determine if fp value is a NaN or Infinity
static inline bool Isa64NaNorInfinity(uint64 fp)
{
    return (ppc_opc_get_bits_64(fp, 1, 11) == 2047);
}

static inline bool Isa64NaN(uint64 fp)
{
    return ((ppc_opc_get_bits_64(fp, 1, 11) == 2047) & (ppc_opc_get_bits_64(fp, 12, 63) != 0));
}

static inline bool Isa64Infinity(uint64 fp)
{
    return ((ppc_opc_get_bits_64(fp, 1, 11) == 2047) & (ppc_opc_get_bits_64(fp, 12, 63) == 0));
}

// Determine if fp value is denormalized
static inline bool Isa64Denorm(uint64 fp)
{
    return ((ppc_opc_get_bits_64(fp, 1, 11) == 0) & (ppc_opc_get_bits_64(fp, 12, 63) != 0));
}

/**
 * Signal a Floating-Point Error in the SPEFSCR
 */
static void SignalFPError(bool upper, uint32 bits)
{
    // implementation expected
}

/**
 * Round a result
 */
static uint32 Round32(e500_core_t *current_core, uint32 fp, int guard, int sticky)
{
    uint32 v = 0;
    if (current_core->spefscr.FINXE == 0)
    {
        if (current_core->spefscr.FRMC == 0) // nearest
        {
            if (guard)
            {
                if (sticky | ppc_opc_get_bit(fp, 31))
                {
                    ppc_opc_set_bits(v, 0, 23, (ppc_opc_get_bits(fp, 9, 31) + 1));
                    if (ppc_opc_get_bit(v, 0))
                    {
                        if (ppc_opc_get_bits(fp, 1, 8) >= 254)
                        {
                            // overflow
                            ppc_opc_set_bits(fp, 1, 31, 0x7F7FFFFF);
                        }
                        else
                        {
                            ppc_opc_set_bits(fp, 1, 8, (ppc_opc_get_bits(fp, 1, 8) + 1));
                            ppc_opc_set_bits(fp, 9, 31, ppc_opc_get_bits(v, 1, 23));
                        }
                    }
                    else
                    {
                        ppc_opc_set_bits(fp, 9, 31, ppc_opc_get_bits(v, 1, 23));
                    }
                }
            }
        }
        else if ((current_core->spefscr.FRMC & 0x2) == 2) // infinity modes
        {
            // implementation dependent
        }
    }
    return fp;
}

/**
 * Round a result
 */
static uint64 Round64(e500_core_t *current_core, uint64 fp, int guard, int sticky)
{
    uint64 v = 0LL;
    if (current_core->spefscr.FINXE == 0)
    {
        if (current_core->spefscr.FRMC == 0) // nearest
        {
            if (guard)
            {
                if (sticky || ppc_opc_get_bit_64(fp, 63))
                {
                    ppc_opc_set_bits_64(v, 0, 52, (ppc_opc_get_bits_64(fp, 12, 63) + 1));
                    if (ppc_opc_get_bit_64(v, 0))
                    {
                        if (ppc_opc_get_bits_64(fp, 1, 11) >= 2046)
                        {
                            // overflow
                            ppc_opc_set_bits_64(fp, 1, 11, 0x7FE);
                            ppc_opc_set_bits_64(fp, 12, 63, 0xFFFFFFFFFFFFFLL);
                        }
                        else
                        {
                            uint64 temp = ppc_opc_get_bits_64(fp, 1, 11) + 1;
                            ppc_opc_set_bits_64(fp, 1, 11, temp);
                            ppc_opc_set_bits_64(fp, 12, 63, ppc_opc_get_bits_64(v, 1, 52));
                        }
                    }
                    else
                    {
                        ppc_opc_set_bits_64(fp, 12, 63, ppc_opc_get_bits_64(v, 1, 52));
                    }
                }
            }
        }
        else if ((current_core->spefscr.FRMC & 0x2) == 2) // infinity modes
        {
            // implementation dependent
        }
    }
    return fp;
}

/**
 * Convert 32-bit floating point to integer/factional
 * signed = SIGN or UNSIGN
 * upper_lower = UPPER or LOWER
 * round = ROUND or TRUNC
 * fractional = F (fractional) or I (integer)
 */
static uint32 CnvtFP32ToI32Sat(e500_core_t *current_core, uint32 fp, bool sign, bool upper, bool round, bool fractional)
{
    uint32 result = 0;
    int maxexp, shift, guard, sticky;

    if (Isa32NaNorInfinity(fp)) // SNaN, QNaN, +-INF
    {
        // SignalFPError(...);
        if (Isa32NaN(fp))
        {
            return 0x00000000; // all NaNs
        }
        if (sign)
        {
            if (ppc_opc_get_bit(fp, 0) == 1)
            {
                return 0x80000000;
            }
            else
            {
                return 0x7FFFFFFF;
            }
        }
        else
        {
            if (ppc_opc_get_bit(fp, 0) == 1)
            {
                return 0x00000000;
            }
            else
            {
                return 0xFFFFFFFF;
            }
        }
    }

    if (Isa32Denorm(fp))
    {
        // SignalFPError(...);
        return 0x00000000; // regardless of sign
    }

    if (!sign && ppc_opc_get_bit(fp, 0) == 1)
    {
        // SignalFPError(...); // overflow
        return 0x00000000;
    }

    if (ppc_opc_get_bits(fp, 1, 8) == 0 && ppc_opc_get_bits(fp, 9, 31) == 0)
    {
        return 0x00000000; // all zero values
    }

    if (!fractional) // convert to integer
    {
        maxexp = 158;
        shift = 158 - ppc_opc_get_bits(fp, 1, 8);
        if (sign)
        {
            if (ppc_opc_get_bits(fp, 1, 8) != 158 || ppc_opc_get_bits(fp, 9, 31) != 0 || ppc_opc_get_bit(fp, 0) != 1)
            {
                maxexp -= 1;
            }
        }
    }
    else // fractional conversion
    {
        maxexp = 126;
        shift = 126 - ppc_opc_get_bits(fp, 1, 8);
        if (sign)
        {
            shift += 1;
        }
    }

    if (ppc_opc_get_bits(fp, 1, 8) > maxexp)
    {
        // SignalFPError(...); // overflow
        if (sign)
        {
            if (ppc_opc_get_bit(fp, 0) == 1)
            {
                return 0x80000000;
            }
            else
            {
                return 0x7FFFFFFF;
            }
        }
        else
        {
            return 0xFFFFFFFF;
        }
    }

    result = (1 << 31) | (ppc_opc_get_bits(fp, 9, 31) << 8); // add U to frac
    guard = 0;
    sticky = 0;

    int n;
    for (n = 0; n < shift; n++)
    {
        sticky = sticky || guard;
        guard = result & 0x00000001;
        result >>= 1;
    }

    // Report sticky and guard bits
    if (upper)
    {
        current_core->spefscr.FGH = guard;
        current_core->spefscr.FXH = sticky;
    }
    else
    {
        current_core->spefscr.FG = guard;
        current_core->spefscr.FX = sticky;
    }
    if (guard || sticky)
    {
        current_core->spefscr.FINXS = 1;
    }

    // Round the integer result
    if (round && current_core->spefscr.FINXE == 0)
    {
        if (current_core->spefscr.FRMC == 0) // nearest
        {
            if (guard)
            {
                if (sticky || (result & 0x00000001))
                {
                    result += 1;
                }
            }
        }
        else if ((current_core->spefscr.FRMC & 0x2) == 2) // infinity modes
        {
            // implementation dependent
        }
    }

    if (sign)
    {
        if (ppc_opc_get_bit(fp, 0) == 1)
        {
            result = ~result + 1;
        }
    }

    return result;
}

/**
 * Convert 64-bit floating point to integer/fractional
 * signed = SIGN or UNSIGN
 * round = ROUND or TRUNC
 * fractional = F (fractional) or I (integer)
 */
static uint32 CnvtFP64ToI32Sat(e500_core_t *current_core, uint64 fp, bool sign, bool round, bool fractional)
{
    uint32 result = 0;
    int maxexp, shift, guard, sticky;

    if (Isa64NaNorInfinity(fp)) // SNaN, QNaN, +-INF
    {
        // SignalFPError(...);
        if (Isa64NaN(fp))
        {
            return 0x00000000; // all NaNs
        }
        if (sign)
        {
            if (ppc_opc_get_bit_64(fp, 0) == 1)
            {
                return 0x80000000;
            }
            else
            {
                return 0x7FFFFFFF;
            }
        }
        else
        {
            if (ppc_opc_get_bit_64(fp, 0) == 1)
            {
                return 0x00000000;
            }
            else
            {
                return 0xFFFFFFFF;
            }
        }
    }

    if (Isa64Denorm(fp))
    {
        // SignalFPError(...);
        return 0x00000000; // regardless of sign
    }

    if (!sign && ppc_opc_get_bit_64(fp, 0) == 1)
    {
        // SignalFPError(...); // overflow
        return 0x00000000;
    }

    if (ppc_opc_get_bits_64(fp, 1, 11) == 0 && ppc_opc_get_bits_64(fp, 12, 63) == 0)
    {
        return 0x00000000; // all zero values
    }

    if (!fractional) // convert to integer
    {
        maxexp = 1054;
        shift = 1054 - ppc_opc_get_bits_64(fp, 1, 11);
        if (sign)
        {
            if (ppc_opc_get_bits_64(fp, 1, 11) != 1054 || ppc_opc_get_bits_64(fp, 12, 63) != 0 || ppc_opc_get_bit_64(fp, 0) != 1)
            {
                maxexp -= 1;
            }
        }
    }
    else // fractional conversion
    {
        maxexp = 1022;
        shift = 1022 - ppc_opc_get_bits_64(fp, 1, 11);
        if (sign)
        {
            shift += 1;
        }
    }

    if (ppc_opc_get_bits_64(fp, 1, 11) > maxexp)
    {
        // SignalFPError(...); // overflow
        if (sign)
        {
            if (ppc_opc_get_bit_64(fp, 0) == 1)
            {
                return 0x80000000;
            }
            else
            {
                return 0x7FFFFFFF;
            }
        }
        else
        {
            return 0xFFFFFFFF;
        }
    }

    result = (1 << 31) | ppc_opc_get_bits_64(fp, 12, 42); // add U to frac
    guard = ppc_opc_get_bit_64(fp, 43);
    sticky = ppc_opc_get_bits_64(fp, 44, 63) != 0;

    int n;
    for (n = 0; n < shift; n++)
    {
        sticky = sticky || guard;
        guard = result & 0x00000001;
        result >>= 1;
    }

    // Report sticky and guard bits
    current_core->spefscr.FG = guard;
    current_core->spefscr.FX = sticky;

    if (guard || sticky)
    {
        current_core->spefscr.FINXS = 1;
    }

    // Round the result
    if (round && current_core->spefscr.FINXE == 0)
    {
        if (current_core->spefscr.FRMC == 0) // nearest
        {
            if (guard)
            {
                if (sticky || (result & 0x00000001))
                {
                    result += 1;
                }
            }
        }
        else if ((current_core->spefscr.FRMC & 0x2) == 2) // infinity modes
        {
            // implementation dependent
        }
    }

    if (sign)
    {
        if (ppc_opc_get_bit_64(fp, 0) == 1)
        {
            result = ~result + 1;
        }
    }

    return result;
}

/**
 * Convert 64-bit floating point to integer/fractional
 * signed = SIGN or UNSIGN
 * round = ROUND or TRUNC
 */
static uint64 CnvtFP64ToI64Sat(e500_core_t *current_core, uint64 fp, bool sign, bool round)
{
    uint64 result = 0LL;
    int maxexp, shift, guard, sticky;

    if (Isa64NaNorInfinity(fp)) // SNaN, QNaN, +-INF
    {
        // SignalFPError(...);
        if (Isa64NaN(fp))
        {
            return 0x0000000000000000LL; // all NaNs
        }
        if (sign)
        {
            if (ppc_opc_get_bit_64(fp, 0) == 1)
            {
                return 0x8000000000000000LL;
            }
            else
            {
                return 0x7FFFFFFFFFFFFFFFLL;
            }
        }
        else
        {
            if (ppc_opc_get_bit_64(fp, 0) == 1)
            {
                return 0x0000000000000000LL;
            }
            else
            {
                return 0xFFFFFFFFFFFFFFFFLL;
            }
        }
    }

    if (Isa64Denorm(fp))
    {
        // SignalFPError(...);
        return 0x0000000000000000LL; // regardless of sign
    }

    if (!sign && ppc_opc_get_bit_64(fp, 0) == 1)
    {
        // SignalFPError(...); // overflow
        return 0x0000000000000000LL;
    }

    if (ppc_opc_get_bits_64(fp, 1, 11) == 0 && ppc_opc_get_bits_64(fp, 12, 63) == 0)
    {
        return 0x0000000000000000LL; // all zero values
    }

    maxexp = 1086;
    shift = 1086 - ppc_opc_get_bits_64(fp, 1, 11);
    if (sign)
    {
        if (ppc_opc_get_bits_64(fp, 1, 11) != 1086 || ppc_opc_get_bits_64(fp, 12, 63) != 0LL || ppc_opc_get_bit_64(fp, 0) != 1)
        {
            maxexp -= 1;
        }
    }

    if (ppc_opc_get_bits_64(fp, 1, 11) > maxexp)
    {
        // SignalFPError(...); // overflow
        if (sign)
        {
            if (ppc_opc_get_bit_64(fp, 0) == 1)
            {
                return 0x8000000000000000LL;
            }
            else
            {
                return 0x7FFFFFFFFFFFFFFFLL;
            }
        }
        else
        {
            return 0xFFFFFFFFFFFFFFFFLL;
        }
    }

    result = (1LL << 63) | (ppc_opc_get_bits_64(fp, 12, 63) << 11); // add U to frac
    guard = 0;
    sticky = 0;

    int n;
    for (n = 0; n < shift; n++)
    {
        sticky = sticky || guard;
        guard = result & 0x0000000000000001LL;
        result >>= 1;
    }

    // Report sticky and guard bits
    current_core->spefscr.FG = guard;
    current_core->spefscr.FX = sticky;

    if (guard || sticky)
    {
        current_core->spefscr.FINXS = 1;
    }

    // Round the result
    if (round && current_core->spefscr.FINXE == 0)
    {
        if (current_core->spefscr.FRMC == 0) // nearest
        {
            if (guard)
            {
                if (sticky || (result & 0x0000000000000001LL))
                {
                    result += 1;
                }
            }
        }
        else if ((current_core->spefscr.FRMC & 0x2) == 2) // infinity modes
        {
            // implementation dependent
        }
    }

    if (sign)
    {
        if (ppc_opc_get_bit_64(fp, 0) == 1)
        {
            result = ~result + 1;
        }
    }

    return result;
}

/**
 * Convert from integer/factional to 32-bit floating point
 * signed = SIGN or UNSIGN
 * upper_lower = UPPER or LOWER
 * fractional = F (fractional) or I (integer)
 */
static uint32 CnvtI32ToFP32Sat(e500_core_t *current_core, uint32 v, bool sign, bool upper, bool fractional)
{
    uint32 result = 0;
    int maxexp, sc, guard, sticky;
    ppc_opc_set_bit(result, 0, 0);
    if (v == 0)
    {
        result = 0;
        if (upper)
        {
            current_core->spefscr.FGH = 0;
            current_core->spefscr.FXH = 0;
        }
        else
        {
            current_core->spefscr.FG = 0;
            current_core->spefscr.FX = 0;
        }
    }
    else
    {
        if (sign)
        {
            if (ppc_opc_get_bit(v, 0) == 1)
            {
                v = ~v + 1;
                ppc_opc_set_bit(result, 0, 1);
            }
        }
        if (fractional) // fractional bit pos alignment
        {
            maxexp = 127;
            if (!sign)
            {
                maxexp -= 1;
            }
        }
        else
        {
            maxexp = 158; // integer bit pos alignment
        }
        sc = 0;
        while (ppc_opc_get_bit(v, 0) == 0)
        {
            v <<= 1;
            sc += 1;
        }
        ppc_opc_set_bit(v, 0, 0); // clear U bit
        ppc_opc_set_bits(result, 1, 8, (maxexp - sc));
        guard = ppc_opc_get_bit(v, 24);
        sticky = (ppc_opc_get_bits(v, 25, 31) != 0);
        // Report sticky and guard bits
        if (upper)
        {
            current_core->spefscr.FGH = guard;
            current_core->spefscr.FXH = sticky;
        }
        else
        {
            current_core->spefscr.FG = guard;
            current_core->spefscr.FX = sticky;
        }
        if (guard || sticky)
        {
            current_core->spefscr.FINXS = 1;
        }
        // Round the result
        uint32 frac = ppc_opc_get_bits(v, 1, 23);
        ppc_opc_set_bits(result, 9, 31, frac);
        result = Round32(current_core, result, guard, sticky);
    }
    return result;
}

/**
 * Convert from integer/factional to 64-bit floating point
 * signed = SIGN or UNSIGN
 * fractional = F (fractional) or I (integer)
 */
static uint64 CnvtI32ToFP64Sat(e500_core_t *current_core, uint32 v, bool sign, bool fractional)
{
    uint64 result = 0LL;
    int maxexp, sc;

    ppc_opc_set_bit_64(result, 0, 0);
    if (v == 0)
    {
        result = 0LL;
        current_core->spefscr.FG = 0;
        current_core->spefscr.FX = 0;
    }
    else
    {
        if (sign)
        {
            if (ppc_opc_get_bit(v, 0) == 1)
            {
                v = ~v + 1;
                ppc_opc_set_bit_64(result, 0, 1);
            }
        }
        if (fractional) // fractional bit pos alignment
        {
            maxexp = 1023;
            if (!sign)
            {
                maxexp -= 1;
            }
        }
        else
        {
            maxexp = 1054; // integer bit pos alignment
        }
        sc = 0;
        while (ppc_opc_get_bit(v, 0) == 0)
        {
            v <<= 1;
            sc += 1;
        }
        ppc_opc_set_bit(v, 0, 0); // clear U bit
        ppc_opc_set_bits_64(result, 1, 11, (maxexp - sc));

        // Report sticky and guard bits
        current_core->spefscr.FG = 0;
        current_core->spefscr.FX = 0;

        ppc_opc_set_bits_64(result, 12, 63, ((uint64)ppc_opc_get_bits(v, 1, 31) << 21));
    }
    return result;
}

/**
 * Convert from 64 integer to 64-bit floating point
 * signed = SIGN or UNSIGN
 */
static uint64 CnvtI64ToFP64Sat(e500_core_t *current_core, uint64 v, bool sign)
{
    uint64 result = 0LL;
    int maxexp, sc, guard, sticky;

    ppc_opc_set_bit_64(result, 0, 0);
    if (v == 0)
    {
        result = 0LL;
        current_core->spefscr.FG = 0;
        current_core->spefscr.FX = 0;
    }
    else
    {
        if (sign)
        {
            if (ppc_opc_get_bit_64(v, 0) == 1)
            {
                v = ~v + 1;
                ppc_opc_set_bit_64(result, 0, 1);
            }
        }
        maxexp = 1054;
        sc = 0;

        while (ppc_opc_get_bit_64(v, 0) == 0)
        {
            v <<= 1;
            sc += 1;
        }
        ppc_opc_set_bit_64(v, 0, 0); // clear U bit
        ppc_opc_set_bits_64(result, 1, 11, (maxexp - sc));
        guard = ppc_opc_get_bit_64(v, 53);
        sticky = ppc_opc_get_bits_64(v, 54, 63) != 0;

        // Report sticky and guard bits
        current_core->spefscr.FG = guard;
        current_core->spefscr.FX = sticky;
        if (guard || sticky)
        {
            current_core->spefscr.FINXS = 1;
        }

        // Round the result
        ppc_opc_set_bits_64(result, 12, 63, ppc_opc_get_bits_64(v, 1, 52));
        result = Round64(current_core, result, guard, sticky);
    }
    return result;
}

/**
 * Saturate after addition
 */
static uint32 SATURATE(int ovf, int carry, uint32 neg_sat, uint32 pos_sat, uint32 value)
{
    if (ovf)
    {
        if (carry)
        {
            return neg_sat;
        }
        else
        {
            return pos_sat;
        }
    }
    else
    {
        return value;
    }
}

static uint64_t BITREVERSE(uint64_t value)
{
    uint64_t result = 0;
    uint64_t mask = 1;
    uint64_t shift = 31;
    uint64_t cnt = 32;
    uint64_t t;
    while (cnt > 0)
    {
        t = value & mask;
        if (shift >= 0)
        {
            result |= t << shift;
        }
        else
        {
            result |= t << (-shift);
        }
        cnt--;
        shift = shift - 2;
        mask = mask << 1;
    }
    return result;
}

/*
 *      brinc            Bit Reversed Increment
 *      .527
 */
void ppc_opc_brinc(e500_core_t *current_core)
{
    int rD, rA, rB, rd, a, n;
    uint64_t mask, d;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    rd = current_core->gpr[rB];
    if (rd == 8)
    {
        n = 3;
    }
    else if (rd == 16)
    {
        n = 4;
    }
    else if (rd == 32)
    {
        n = 5;
    }
    else
    {
        n = 6;
    }

    mask = rd;
    a = current_core->gpr[rA] << (32 - n) >> (32 - n);
    d = BITREVERSE(1 + BITREVERSE(a | ~mask));

    current_core->gprh[rD] = current_core->gprh[rA];
    current_core->gpr[rD] = (current_core->gpr[rA] >> n << n) | (d & mask);
}

/**
 * Floating-Point Double-Precision Absolute Value
 * .740
 */
void ppc_opc_efdabs(e500_core_t *current_core)
{
    int rD, rA;
    PPC_OPC_TEMPL_XI(current_core->current_opc, rD, rA);
    uint64 dataA = ppc_opc_get_reg_64(current_core->gprh[rA], current_core->gpr[rA]);

    ppc_switch_rounding_mode(current_core);
    ppc_clear_float_status(current_core);
    double result = fabs(*(double *)&dataA);
    uint64 data = *(uint64 *)&result;
    current_core->gprh[rD] = ppc_opc_get_reg_64_high(data);
    current_core->gpr[rD] = ppc_opc_get_reg_64_low(data);
    ppc_set_float_status(current_core);
}

/**
 * Floating-Point Double-Precision Add
 * .736
 */
void ppc_opc_efdadd(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    uint64 dataA = ppc_opc_get_reg_64(current_core->gprh[rA], current_core->gpr[rA]);
    uint64 dataB = ppc_opc_get_reg_64(current_core->gprh[rB], current_core->gpr[rB]);

    ppc_switch_rounding_mode(current_core);
    ppc_clear_float_status(current_core);
    double result = *(double *)&dataA + *(double *)&dataB;
    uint64 data = *(uint64 *)&result;
    current_core->gprh[rD] = ppc_opc_get_reg_64_high(data);
    current_core->gpr[rD] = ppc_opc_get_reg_64_low(data);
    ppc_set_float_status(current_core);
}

/**
 * Floating-Point Double-Precision Convert from Single-Precision
 * .751
 */
void ppc_opc_efdcfs(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    // uint32 f = current_core->gpr[rB];
    // uint64 result = 0LL;
    // if (ppc_opc_get_bits(f, 1, 8) == 0 && ppc_opc_get_bits(f, 9, 31) == 0)
    // {
    //     ppc_opc_set_bit_64(result, 0, ppc_opc_get_bit(f, 0)); // signed zero value
    // }
    // else if (Isa32NaNorInfinity(f) || Isa32Denorm(f))
    // {
    //     current_core->spefscr.FINV = 1;
    //     ppc_opc_set_bit_64(result, 0, ppc_opc_get_bit(f, 0)); // max value
    //     ppc_opc_set_bits_64(result, 1, 11, 0x7FE);
    //     ppc_opc_set_bits_64(result, 12, 63, 0xFFFFFFFFFFFFFLL);
    // }
    // else if (Isa32Denorm(f))
    // {
    //     current_core->spefscr.FINV = 1;
    //     ppc_opc_set_bit_64(result, 0, ppc_opc_get_bit(f, 0));
    // }
    // else
    // {
    //     ppc_opc_set_bit_64(result, 0, ppc_opc_get_bit(f, 0));
    //     ppc_opc_set_bits_64(result, 1, 11, (ppc_opc_get_bits(f, 1, 8) - 127 + 1023));
    //     ppc_opc_set_bits_64(result, 12, 34, ppc_opc_get_bits(f, 9, 31));
    // }
    // current_core->gprh[rD] = ppc_opc_get_reg_64_high(result);
    // current_core->gpr[rD] = ppc_opc_get_reg_64_low(result);

    // or you can just
    ppc_switch_rounding_mode(current_core);
    ppc_clear_float_status(current_core);
    double result = *(float *)&current_core->gpr[rB];
    uint64 data = *(uint64 *)&result;
    current_core->gprh[rD] = ppc_opc_get_reg_64_high(data);
    current_core->gpr[rD] = ppc_opc_get_reg_64_low(data);
    ppc_set_float_status(current_core);
}

/**
 * Convert Floating-Point Double-Precision from Signed Fraction
 * .755
 */
void ppc_opc_efdcfsf(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    uint64 result = CnvtI32ToFP64Sat(current_core,  current_core->gpr[rB], SIGN, F);
    current_core->gprh[rD] = ppc_opc_get_reg_64_high(result);
    current_core->gpr[rD] = ppc_opc_get_reg_64_low(result);
}

/**
 * Convert Floating-Point Double-Precision from Signed Integer
 * .753
 */
void ppc_opc_efdcfsi(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    // uint64 result = CnvtI32ToFP64Sat(current_core->gpr[rB], SIGN, I);
    // current_core->gprh[rD] = ppc_opc_get_reg_64_high(result);
    // current_core->gpr[rD] = ppc_opc_get_reg_64_low(result);

    ppc_switch_rounding_mode(current_core);
    ppc_clear_float_status(current_core);
    double result = (sint32)current_core->gpr[rB];
    uint64 data = *(uint64 *)&result;
    current_core->gprh[rD] = ppc_opc_get_reg_64_high(data);
    current_core->gpr[rD] = ppc_opc_get_reg_64_low(data);
    ppc_set_float_status(current_core);
}

/**
 * Convert Floating-Point Double-Precision from Signed Integer Doubleword
 * .739
 */
void ppc_opc_efdcfsid(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    // uint64 result = CnvtI64ToFP64Sat(ppc_opc_get_reg_64(current_core->gprh[rB], current_core->gpr[rB]), SIGN);
    // current_core->gprh[rD] = ppc_opc_get_reg_64_high(result);
    // current_core->gpr[rD] = ppc_opc_get_reg_64_low(result);

    ppc_clear_float_status(current_core);
    double result = (sint64)ppc_opc_get_reg_64(current_core->gprh[rB], current_core->gpr[rB]);
    uint64 data = *(uint64 *)&result;
    current_core->gprh[rD] = ppc_opc_get_reg_64_high(data);
    current_core->gpr[rD] = ppc_opc_get_reg_64_low(data);
    ppc_set_float_status(current_core);
}

/**
 * Convert Floating-Point Double-Precision from Unsigned Fraction
 * .754
 */
void ppc_opc_efdcfuf(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    uint64 result = CnvtI32ToFP64Sat(current_core, current_core->gpr[rB], UNSIGN, F);
    current_core->gprh[rD] = ppc_opc_get_reg_64_high(result);
    current_core->gpr[rD] = ppc_opc_get_reg_64_low(result);
}

/**
 * Convert Floating-Point Double-Precision from Unsigned Integer
 * .752
 */
void ppc_opc_efdcfui(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    // uint64 result = CnvtI32ToFP64Sat(current_core->gpr[rB], UNSIGN, I);
    // current_core->gprh[rD] = ppc_opc_get_reg_64_high(result);
    // current_core->gpr[rD] = ppc_opc_get_reg_64_low(result);

    ppc_switch_rounding_mode(current_core);
    ppc_clear_float_status(current_core);
    double result = current_core->gpr[rB];
    uint64 data = *(uint64 *)&result;
    current_core->gprh[rD] = ppc_opc_get_reg_64_high(data);
    current_core->gpr[rD] = ppc_opc_get_reg_64_low(data);
    ppc_set_float_status(current_core);
}

/**
 * Convert Floating-Point Double-Precision from Unsigned Integer Doubleword
 * .738
 */
void ppc_opc_efdcfuid(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    // uint64 result = CnvtI64ToFP64Sat(ppc_opc_get_reg_64(current_core->gprh[rB], current_core->gpr[rB]), UNSIGN);
    // current_core->gprh[rD] = ppc_opc_get_reg_64_high(result);
    // current_core->gpr[rD] = ppc_opc_get_reg_64_low(result);

    ppc_switch_rounding_mode(current_core);
    ppc_clear_float_status(current_core);
    double result = ppc_opc_get_reg_64(current_core->gprh[rB], current_core->gpr[rB]);
    uint64 data = *(uint64 *)&result;
    current_core->gprh[rD] = ppc_opc_get_reg_64_high(data);
    current_core->gpr[rD] = ppc_opc_get_reg_64_low(data);
    ppc_set_float_status(current_core);
}

/**
 * Floating-Point Double-Precision Compare Equal
 * .750
 */
void ppc_opc_efdcmpeq(e500_core_t *current_core)
{
    int crfD, rA, rB;
    PPC_OPC_TEMPL_CRX(current_core->current_opc, crfD, rA, rB);
    int cl, cr;

    ppc_clear_float_status(current_core);
    uint64 al = ppc_opc_get_reg_64(current_core->gprh[rA], current_core->gpr[rA]);
    uint64 bl = ppc_opc_get_reg_64(current_core->gprh[rB], current_core->gpr[rB]);
    if (*(double *)&al == *(double *)&bl)
    {
        cl = 1;
    }
    else
    {
        cl = 0;
    }
    ppc_set_float_status(current_core);
    cr = (cl << 2);
    ppc_opc_set_cr(current_core->cr, crfD, cr);
}

/**
 * Floating-Point Double-Precision Compare Greater Than
 * .748
 */
void ppc_opc_efdcmpgt(e500_core_t *current_core)
{
    int crfD, rA, rB;
    PPC_OPC_TEMPL_CRX(current_core->current_opc, crfD, rA, rB);
    int cl, cr;

    ppc_clear_float_status(current_core);
    uint64 al = ppc_opc_get_reg_64(current_core->gprh[rA], current_core->gpr[rA]);
    uint64 bl = ppc_opc_get_reg_64(current_core->gprh[rB], current_core->gpr[rB]);
    if (*(double *)&al > *(double *)&bl)
    {
        cl = 1;
    }
    else
    {
        cl = 0;
    }
    ppc_set_float_status(current_core);
    cr = (cl << 2);
    ppc_opc_set_cr(current_core->cr, crfD, cr);
}

/**
 * Floating-Point Double-Precision Compare Less Than
 * .749
 */
void ppc_opc_efdcmplt(e500_core_t *current_core)
{
    int crfD, rA, rB;
    PPC_OPC_TEMPL_CRX(current_core->current_opc, crfD, rA, rB);
    int cl, cr;

    ppc_clear_float_status(current_core);
    uint64 al = ppc_opc_get_reg_64(current_core->gprh[rA], current_core->gpr[rA]);
    uint64 bl = ppc_opc_get_reg_64(current_core->gprh[rB], current_core->gpr[rB]);
    if (*(double *)&al < *(double *)&bl)
    {
        cl = 1;
    }
    else
    {
        cl = 0;
    }
    ppc_set_float_status(current_core);
    cr = (cl << 2);
    ppc_opc_set_cr(current_core->cr, crfD, cr);
}

/**
 * Convert Floating-Point Double-Precision to Signed Fraction
 * .759
 */
void ppc_opc_efdctsf(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gpr[rD] = CnvtFP64ToI32Sat(current_core, ppc_opc_get_reg_64(current_core->gprh[rB], current_core->gpr[rB]), SIGN, ROUND, F);
}

/**
 * Convert Floating-Point Double-Precision to Signed Integer
 * .757
 */
void ppc_opc_efdctsi(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gpr[rD] = CnvtFP64ToI32Sat(current_core, ppc_opc_get_reg_64(current_core->gprh[rB], current_core->gpr[rB]), SIGN, ROUND, I);
}

/**
 * Convert Floating-Point Double-Precision to Signed Integer Doubleword with Round toward Zero
 * .747
 */
void ppc_opc_efdctsidz(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    uint64 result = CnvtFP64ToI64Sat(current_core, ppc_opc_get_reg_64(current_core->gprh[rB], current_core->gpr[rB]), SIGN, TRUNC);
    current_core->gprh[rD] = ppc_opc_get_reg_64_high(result);
    current_core->gpr[rD] = ppc_opc_get_reg_64_low(result);
}

/**
 * Convert Floating-Point Double-Precision to Signed Integer with Round toward Zero
 * .762
 */
void ppc_opc_efdctsiz(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gpr[rD] = CnvtFP64ToI32Sat(current_core, ppc_opc_get_reg_64(current_core->gprh[rB], current_core->gpr[rB]), SIGN, TRUNC, I);
}

/**
 * Convert Floating-Point Double-Precision to Unsigned Fraction
 * .758
 */
void ppc_opc_efdctuf(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gpr[rD] = CnvtFP64ToI32Sat(current_core, ppc_opc_get_reg_64(current_core->gprh[rB], current_core->gpr[rB]), UNSIGN, ROUND, F);
}

/**
 * Convert Floating-Point Double-Precision to Unsigned Integer
 * .756
 */
void ppc_opc_efdctui(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gpr[rD] = CnvtFP64ToI32Sat(current_core, ppc_opc_get_reg_64(current_core->gprh[rB], current_core->gpr[rB]), UNSIGN, ROUND, I);
}

/**
 * Convert Floating-Point Double-Precision to Unsigned Integer Doubleword with Round toward Zero
 * .746
 */
void ppc_opc_efdctuidz(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    uint64 result = CnvtFP64ToI64Sat(current_core, ppc_opc_get_reg_64(current_core->gprh[rB], current_core->gpr[rB]), UNSIGN, TRUNC);
    current_core->gprh[rD] = ppc_opc_get_reg_64_high(result);
    current_core->gpr[rD] = ppc_opc_get_reg_64_low(result);
}

/**
 * Convert Floating-Point Double-Precision to Unsigned Integer with Round toward Zero
 * .760
 */
void ppc_opc_efdctuiz(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gpr[rD] = CnvtFP64ToI32Sat(current_core, ppc_opc_get_reg_64(current_core->gprh[rB], current_core->gpr[rB]), UNSIGN, TRUNC, I);
}

/**
 * Floating-Point Double-Precision Divide
 * .745
 */
void ppc_opc_efddiv(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    ppc_switch_rounding_mode(current_core);
    ppc_clear_float_status(current_core);
    uint64 al = ppc_opc_get_reg_64(current_core->gprh[rA], current_core->gpr[rA]);
    uint64 bl = ppc_opc_get_reg_64(current_core->gprh[rB], current_core->gpr[rB]);
    double result = *(double *)&al / *(double *)&bl;
    uint64 data = *(uint64 *)&result;
    current_core->gprh[rD] = ppc_opc_get_reg_64_high(data);
    current_core->gpr[rD] = ppc_opc_get_reg_64_low(data);
    ppc_set_float_status(current_core);
}

/**
 * Floating-Point Double-Precision Multiply
 * .744
 */
void ppc_opc_efdmul(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    uint64 dataA = ppc_opc_get_reg_64(current_core->gprh[rA], current_core->gpr[rA]);
    uint64 dataB = ppc_opc_get_reg_64(current_core->gprh[rB], current_core->gpr[rB]);

    ppc_switch_rounding_mode(current_core);
    ppc_clear_float_status(current_core);
    double result = *(double *)&dataA * *(double *)&dataB;
    uint64 data = *(uint64 *)&result;
    current_core->gprh[rD] = ppc_opc_get_reg_64_high(data);
    current_core->gpr[rD] = ppc_opc_get_reg_64_low(data);
    ppc_set_float_status(current_core);
}

/**
 * Floating-Point Double-Precision Negative Absolute Value
 * .741
 */
void ppc_opc_efdnabs(e500_core_t *current_core)
{
    int rD, rA;
    PPC_OPC_TEMPL_XI(current_core->current_opc, rD, rA);
    uint64 dataA = ppc_opc_get_reg_64(current_core->gprh[rA], current_core->gpr[rA]);

    ppc_switch_rounding_mode(current_core);
    ppc_clear_float_status(current_core);
    double result = -fabs(*(double *)&dataA);
    uint64 data = *(uint64 *)&result;
    current_core->gprh[rD] = ppc_opc_get_reg_64_high(data);
    current_core->gpr[rD] = ppc_opc_get_reg_64_low(data);
    ppc_set_float_status(current_core);
}

/**
 * Floating-Point Double-Precision Negate
 * .742
 */
void ppc_opc_efdneg(e500_core_t *current_core)
{
    int rD, rA;
    PPC_OPC_TEMPL_XI(current_core->current_opc, rD, rA);
    uint64 dataA = ppc_opc_get_reg_64(current_core->gprh[rA], current_core->gpr[rA]);

    ppc_switch_rounding_mode(current_core);
    ppc_clear_float_status(current_core);
    double result = -*(double *)&dataA;
    uint64 data = *(uint64 *)&result;
    current_core->gprh[rD] = ppc_opc_get_reg_64_high(data);
    current_core->gpr[rD] = ppc_opc_get_reg_64_low(data);
    ppc_set_float_status(current_core);
}

/**
 * Floating-Point Double-Precision Subtract
 * .737
 */
void ppc_opc_efdsub(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    uint64 dataA = ppc_opc_get_reg_64(current_core->gprh[rA], current_core->gpr[rA]);
    uint64 dataB = ppc_opc_get_reg_64(current_core->gprh[rB], current_core->gpr[rB]);

    ppc_switch_rounding_mode(current_core);
    ppc_clear_float_status(current_core);
    double result = *(double *)&dataA - *(double *)&dataB;
    uint64 data = *(uint64 *)&result;
    current_core->gprh[rD] = ppc_opc_get_reg_64_high(data);
    current_core->gpr[rD] = ppc_opc_get_reg_64_low(data);
    ppc_set_float_status(current_core);
}

/**
 * Floating-Point Double-Precision Test Equal
 * .766
 */
void ppc_opc_efdtsteq(e500_core_t *current_core)
{
    int crfD, rA, rB;
    PPC_OPC_TEMPL_CRX(current_core->current_opc, crfD, rA, rB);
    int cl, cr;

    uint64 al = ppc_opc_get_reg_64(current_core->gprh[rA], current_core->gpr[rA]);
    uint64 bl = ppc_opc_get_reg_64(current_core->gprh[rB], current_core->gpr[rB]);
    if (*(double *)&al == *(double *)&bl)
    {
        cl = 1;
    }
    else
    {
        cl = 0;
    }
    cr = (cl << 2);
    ppc_opc_set_cr(current_core->cr, crfD, cr);
}

/**
 * Floating-Point Double-Precision Test Greater Than
 * .764
 */
void ppc_opc_efdtstgt(e500_core_t *current_core)
{
    int crfD, rA, rB;
    PPC_OPC_TEMPL_CRX(current_core->current_opc, crfD, rA, rB);
    int cl, cr;

    uint64 al = ppc_opc_get_reg_64(current_core->gprh[rA], current_core->gpr[rA]);
    uint64 bl = ppc_opc_get_reg_64(current_core->gprh[rB], current_core->gpr[rB]);
    if (*(double *)&al > *(double *)&bl)
    {
        cl = 1;
    }
    else
    {
        cl = 0;
    }
    cr = (cl << 2);
    ppc_opc_set_cr(current_core->cr, crfD, cr);
}

/**
 * Floating-Point Double-Precision Test Less Than
 * .765
 */
void ppc_opc_efdtstlt(e500_core_t *current_core)
{
    int crfD, rA, rB;
    PPC_OPC_TEMPL_CRX(current_core->current_opc, crfD, rA, rB);
    int cl, cr;

    uint64 al = ppc_opc_get_reg_64(current_core->gprh[rA], current_core->gpr[rA]);
    uint64 bl = ppc_opc_get_reg_64(current_core->gprh[rB], current_core->gpr[rB]);
    if (*(double *)&al < *(double *)&bl)
    {
        cl = 1;
    }
    else
    {
        cl = 0;
    }
    cr = (cl << 2);
    ppc_opc_set_cr(current_core->cr, crfD, cr);
}

/**
 * Floating-Point Absolute Value
 * .708
 */
void ppc_opc_efsabs(e500_core_t *current_core)
{
    int rD, rA;
    PPC_OPC_TEMPL_XI(current_core->current_opc, rD, rA);

    ppc_switch_rounding_mode(current_core);
    ppc_clear_float_status(current_core);
    float result = fabs(*(float *)&current_core->gpr[rA]);
    current_core->gpr[rD] = *(uint32 *)&result;
    ppc_set_float_status(current_core);
}

/**
 * Floating-Point Add
 * .704
 */
void ppc_opc_efsadd(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    ppc_switch_rounding_mode(current_core);
    ppc_clear_float_status(current_core);
    float result = *(float *)&current_core->gpr[rA] + *(float *)&current_core->gpr[rB];
    current_core->gpr[rD] = *(uint32 *)&result;
    ppc_set_float_status(current_core);
}

/**
 * Floating-Point Single-Precision Convert from Double-Precision
 * .719
 */
void ppc_opc_efscfd(e500_core_t *current_core)
{
    int rD, rA, rB;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    PPC_OPC_ASSERT(rD == 0 && rB == 0);
    int64_t src_rb = (int64_t)current_core->gprh[rB];

    // get signed integer
    int64_t src_tmp = current_core->gpr[rB] | (src_rb << 32);

    // convert to float
    float dst_fp = (float)(*(double *)&src_tmp);

    // return result value
    uint32_t result = *(uint32_t *)&dst_fp;

    current_core->gpr[rD] = result;
}

/**
 * Convert Floating-Point from Signed Fraction
 * .723
 */
void ppc_opc_efscfsf(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gpr[rD] = CnvtI32ToFP32Sat(current_core, current_core->gpr[rB], SIGN, LOWER, F);
}

/**
 * Convert Floating-Point from Signed Integer
 * .721
 */
void ppc_opc_efscfsi(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gpr[rD] = CnvtI32ToFP32Sat(current_core, current_core->gpr[rB], SIGN, LOWER, I);
}

/**
 * Convert Floating-Point from Unsigned Fraction
 * .722
 */
void ppc_opc_efscfuf(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gpr[rD] = CnvtI32ToFP32Sat(current_core, current_core->gpr[rB], UNSIGN, LOWER, F);
}

/**
 * Convert Floating-Point from Unsigned Integer
 * .720
 */
void ppc_opc_efscfui(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gpr[rD] = CnvtI32ToFP32Sat(current_core, current_core->gpr[rB], UNSIGN, LOWER, I);
}

/**
 * Floating-Point Compare Equal
 * .718
 */
void ppc_opc_efscmpeq(e500_core_t *current_core)
{
    int crfD, rA, rB;
    PPC_OPC_TEMPL_CRX(current_core->current_opc, crfD, rA, rB);
    int cl, cr;

    ppc_clear_float_status(current_core);
    if (*(float *)&current_core->gpr[rA] == *(float *)&current_core->gpr[rB])
    {
        cl = 1;
    }
    else
    {
        cl = 0;
    }
    ppc_set_float_status(current_core);
    cr = (cl << 2);
    ppc_opc_set_cr(current_core->cr, crfD, cr);
}

/**
 * Floating-Point Compare Greater Than
 * .716
 */
void ppc_opc_efscmpgt(e500_core_t *current_core)
{
    int crfD, rA, rB;
    PPC_OPC_TEMPL_CRX(current_core->current_opc, crfD, rA, rB);
    int cl, cr;

    ppc_clear_float_status(current_core);
    if (*(float *)&current_core->gpr[rA] > *(float *)&current_core->gpr[rB])
    {
        cl = 1;
    }
    else
    {
        cl = 0;
    }
    ppc_set_float_status(current_core);
    cr = (cl << 2);
    ppc_opc_set_cr(current_core->cr, crfD, cr);
}

/**
 * Floating-Point Compare Less Than
 * .717
 */
void ppc_opc_efscmplt(e500_core_t *current_core)
{
    int crfD, rA, rB;
    PPC_OPC_TEMPL_CRX(current_core->current_opc, crfD, rA, rB);
    int cl, cr;

    ppc_clear_float_status(current_core);
    if (*(float *)&current_core->gpr[rA] < *(float *)&current_core->gpr[rB])
    {
        cl = 1;
    }
    else
    {
        cl = 0;
    }
    ppc_set_float_status(current_core);
    cr = (cl << 2);
    ppc_opc_set_cr(current_core->cr, crfD, cr);
}

/**
 * Convert Floating-Point to Signed Fraction
 * .727
 */
void ppc_opc_efsctsf(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gpr[rD] = CnvtFP32ToI32Sat(current_core, current_core->gpr[rB], SIGN, LOWER, ROUND, F);
}

/**
 * Convert Floating-Point to Signed Integer
 * .725
 */
void ppc_opc_efsctsi(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gpr[rD] = CnvtFP32ToI32Sat(current_core, current_core->gpr[rB], SIGN, LOWER, ROUND, I);
}

/**
 * Convert Floating-Point to Signed Integer with Round toward Zero
 * .730
 */
void ppc_opc_efsctsiz(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gpr[rD] = CnvtFP32ToI32Sat(current_core, current_core->gpr[rB], SIGN, LOWER, TRUNC, I);
}

/**
 * Convert Floating-Point to Unsigned Fraction
 * .726
 */
void ppc_opc_efsctuf(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gpr[rD] = CnvtFP32ToI32Sat(current_core, current_core->gpr[rB], UNSIGN, LOWER, ROUND, F);
}

/**
 * Convert Floating-Point to Unsigned Integer
 * .724
 */
void ppc_opc_efsctui(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gpr[rD] = CnvtFP32ToI32Sat(current_core, current_core->gpr[rB], UNSIGN, LOWER, ROUND, I);
}

/**
 * Convert Floating-Point to Unsigned Integer with Round toward Zero
 * .728
 */
void ppc_opc_efsctuiz(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gpr[rD] = CnvtFP32ToI32Sat(current_core, current_core->gpr[rB], UNSIGN, LOWER, TRUNC, I);
}

/**
 * Floating-Point Divide
 * .713
 */
void ppc_opc_efsdiv(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    ppc_switch_rounding_mode(current_core);
    ppc_clear_float_status(current_core);
    float result = *(float *)&current_core->gpr[rA] / *(float *)&current_core->gpr[rB];
    current_core->gpr[rD] = *(uint32 *)&result;
    ppc_set_float_status(current_core);
}

/**
 * Floating-Point Multiply
 * .712
 */
void ppc_opc_efsmul(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    ppc_switch_rounding_mode(current_core);
    ppc_clear_float_status(current_core);
    float result = *(float *)&current_core->gpr[rA] * *(float *)&current_core->gpr[rB];
    current_core->gpr[rD] = *(uint32 *)&result;
    ppc_set_float_status(current_core);
}

/**
 * Floating-Point Negative Absolute Value
 * .709
 */
void ppc_opc_efsnabs(e500_core_t *current_core)
{
    int rD, rA;
    PPC_OPC_TEMPL_XI(current_core->current_opc, rD, rA);

    ppc_switch_rounding_mode(current_core);
    ppc_clear_float_status(current_core);
    float result = -fabs(*(float *)&current_core->gpr[rA]);
    current_core->gpr[rD] = *(uint32 *)&result;
    ppc_set_float_status(current_core);
}

/**
 * Floating-Point Negate
 * .710
 */
void ppc_opc_efsneg(e500_core_t *current_core)
{
    int rD, rA;
    PPC_OPC_TEMPL_XI(current_core->current_opc, rD, rA);

    ppc_switch_rounding_mode(current_core);
    ppc_clear_float_status(current_core);
    float result = -*(float *)&current_core->gpr[rA];
    current_core->gpr[rD] = *(uint32 *)&result;
    ppc_set_float_status(current_core);
}

/**
 * Floating-Point Subtract
 * .705
 */
void ppc_opc_efssub(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    ppc_switch_rounding_mode(current_core);
    ppc_clear_float_status(current_core);
    float result = *(float *)&current_core->gpr[rA] - *(float *)&current_core->gpr[rB];
    current_core->gpr[rD] = *(uint32 *)&result;
    ppc_set_float_status(current_core);
}

/**
 * Floating-Point Test Equal
 * .734
 */
void ppc_opc_efststeq(e500_core_t *current_core)
{
    int crfD, rA, rB;
    PPC_OPC_TEMPL_CRX(current_core->current_opc, crfD, rA, rB);
    int cl, cr;

    if (*(float *)&current_core->gpr[rA] == *(float *)&current_core->gpr[rB])
    {
        cl = 1;
    }
    else
    {
        cl = 0;
    }
    cr = (cl << 2);
    ppc_opc_set_cr(current_core->cr, crfD, cr);
}

/**
 * Floating-Point Test Greater Than
 * .732
 */
void ppc_opc_efststgt(e500_core_t *current_core)
{
    int crfD, rA, rB;
    PPC_OPC_TEMPL_CRX(current_core->current_opc, crfD, rA, rB);
    int cl, cr;

    if (*(float *)&current_core->gpr[rA] > *(float *)&current_core->gpr[rB])
    {
        cl = 1;
    }
    else
    {
        cl = 0;
    }
    cr = (cl << 2);
    ppc_opc_set_cr(current_core->cr, crfD, cr);
}

/**
 * Floating-Point Test Less Than
 * .733
 */
void ppc_opc_efststlt(e500_core_t *current_core)
{
    int crfD, rA, rB;
    PPC_OPC_TEMPL_CRX(current_core->current_opc, crfD, rA, rB);
    int cl, cr;

    if (*(float *)&current_core->gpr[rA] < *(float *)&current_core->gpr[rB])
    {
        cl = 1;
    }
    else
    {
        cl = 0;
    }
    cr = (cl << 2);
    ppc_opc_set_cr(current_core->cr, crfD, cr);
}

/*
 *      evabs            Vector Absolute Value
 *      .520
 */
void ppc_opc_evabs(e500_core_t *current_core)
{
    int rD, rA;
    PPC_OPC_TEMPL_XI(current_core->current_opc, rD, rA);

    current_core->gprh[rD] = abs((sint32)current_core->gprh[rA]);
    current_core->gpr[rD] = abs((sint32)current_core->gpr[rA]);
}

/*
 *      evaddiw            Vector Add Immediate Word
 *      .514
 */
void ppc_opc_evaddiw(e500_core_t *current_core)
{
    int rD, UIMM, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, UIMM, rB);

    current_core->gprh[rD] = (sint32)current_core->gprh[rB] + UIMM;
    current_core->gpr[rD] = (sint32)current_core->gpr[rB] + UIMM;
}

/*
 *      evaddsmiaaw            Vector Add Signed, Modulo, Integer to Accumulator Word
 *      .1225
 */
void ppc_opc_evaddsmiaaw(e500_core_t *current_core)
{
    int rD, rA;
    PPC_OPC_TEMPL_XI(current_core->current_opc, rD, rA);

    current_core->gpr[rD] = (int)(current_core->gpr[rA]) + (current_core->acc & 0x00000000ffffffff);
    current_core->gprh[rD] = (int)(current_core->gprh[rA]) + (current_core->acc >> 32);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *      evaddssiaaw            Vector Add Signed, Saturate, Integer to Accumulator Word
 *      .1217
 */
void ppc_opc_evaddssiaaw(e500_core_t *current_core)
{
    int rD, rA, ovh, ovl;
    int temp_31;
    int64_t temp;
    PPC_OPC_TEMPL_XI(current_core->current_opc, rD, rA);

    temp = (current_core->acc >> 32) + (((int64_t)(current_core->gprh[rA]) << 32) >> 32);
    ovh = (((temp << 31) >> 63) & 0x1) ^ (((temp << 32) >> 63) & 0x1);
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gprh[rD] = SATURATE(ovh, temp_31, 0x80000000, 0x7fffffff, temp);

    temp = (((current_core->acc & 0xFFFFffff) << 32) >> 32) + (((int64_t)(current_core->gpr[rA]) << 32) >> 32);
    ovl = (((temp << 31) >> 63) & 0x1) ^ (((temp << 32) >> 63) & 0x1);
    temp_31 = ((temp << 31) >> 63) & 0x1;

    current_core->gpr[rD] = SATURATE(ovl, temp_31, 0x80000000, 0x7fffffff, temp);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->spefscr.OVH = ovh;
    current_core->spefscr.OV = ovl;
    current_core->spefscr.SOVH |= ovh;
    current_core->spefscr.SOV |= ovl;
}

/*
 *      evaddumiaaw            Vector Add Unsigned, Modulo, Integer to Accumulator Word
 *      .1224
 */
void ppc_opc_evaddumiaaw(e500_core_t *current_core)
{
    int rD, rA;
    PPC_OPC_TEMPL_XI(current_core->current_opc, rD, rA);

    current_core->gpr[rD] = current_core->gpr[rA] + (current_core->acc & 0xFFFFffff);
    current_core->gprh[rD] = (current_core->gprh[rA]) + (current_core->acc >> 32);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *      evaddusiaaw            Vector Add Unsigned, Saturate, Integer to Accumulator Word
 *      .1216
 */
void ppc_opc_evaddusiaaw(e500_core_t *current_core)
{
    int rD, rA, ovh, ovl;
    int temp_31;
    uint64_t temp;
    PPC_OPC_TEMPL_XI(current_core->current_opc, rD, rA);

    temp = ((current_core->acc >> 32) & 0xFFFFffff) + (uint64_t)(current_core->gprh[rA]);
    ovh = ((temp << 31) >> 63) & 0x1;
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gprh[rD] = SATURATE(ovh, temp_31, 0xFFFFffff, 0xFFFFffff, temp);

    temp = (current_core->acc & 0xFFFFffff) + (uint64_t)(current_core->gpr[rA]);
    ovl = ((temp << 31) >> 63) & 0x1;
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gpr[rD] = SATURATE(ovl, temp_31, 0xFFFFffff, 0xFFFFffff, temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->spefscr.OVH = ovh;
    current_core->spefscr.OV = ovl;
    current_core->spefscr.SOVH |= ovh;
    current_core->spefscr.SOV |= ovl;
}

/*
 *      evaddw            Vector Add Word
 *      .512
 */
void ppc_opc_evaddw(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    current_core->gprh[rD] = (sint32)current_core->gprh[rA] + (sint32)current_core->gprh[rB];
    current_core->gpr[rD] = (sint32)current_core->gpr[rA] + (sint32)current_core->gpr[rB];
}

/*
 *      evand            Vector AND
 *      .529
 */
void ppc_opc_evand(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    current_core->gprh[rD] = current_core->gprh[rA] & current_core->gprh[rB];
    current_core->gpr[rD] = current_core->gpr[rA] & current_core->gpr[rB];
}

/*
 *      evandc            Vector Add Word
 *      .530
 */
void ppc_opc_evandc(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    current_core->gprh[rD] = current_core->gprh[rA] & ~current_core->gprh[rB];
    current_core->gpr[rD] = current_core->gpr[rA] & ~current_core->gpr[rB];
}

/*
 *      evcmpeq            Vector Compare Equal
 *      .564
 */
void ppc_opc_evcmpeq(e500_core_t *current_core)
{
    int crD, rA, rB;
    PPC_OPC_TEMPL_CRX(current_core->current_opc, crD, rA, rB);

    int ch, cl, cr;
    if ((sint32)current_core->gprh[rA] == (sint32)current_core->gprh[rB])
    {
        ch = 1;
    }
    else
    {
        ch = 0;
    }
    if ((sint32)current_core->gpr[rA] == (sint32)current_core->gpr[rB])
    {
        cl = 1;
    }
    else
    {
        cl = 0;
    }
    cr = (ch << 3) | (cl << 2) | ((ch | cl) << 1) | (ch & cl);
    ppc_opc_set_cr(current_core->cr, crD, cr);
}

/*
 *      evcmpgts            Vector Compare Greater Than Signed
 *      .561
 */
void ppc_opc_evcmpgts(e500_core_t *current_core)
{
    int crD, rA, rB;
    PPC_OPC_TEMPL_CRX(current_core->current_opc, crD, rA, rB);

    int ch, cl, cr;
    if ((sint32)current_core->gprh[rA] > (sint32)current_core->gprh[rB])
    {
        ch = 1;
    }
    else
    {
        ch = 0;
    }
    if ((sint32)current_core->gpr[rA] > (sint32)current_core->gpr[rB])
    {
        cl = 1;
    }
    else
    {
        cl = 0;
    }
    cr = (ch << 3) | (cl << 2) | ((ch | cl) << 1) | (ch & cl);
    ppc_opc_set_cr(current_core->cr, crD, cr);
}

/*
 *      evcmpgtu            Vector Compare Greater Than Unsigned
 *      .560
 */
void ppc_opc_evcmpgtu(e500_core_t *current_core)
{
    int crD, rA, rB;
    PPC_OPC_TEMPL_CRX(current_core->current_opc, crD, rA, rB);

    int ch, cl, cr;
    if (current_core->gprh[rA] > current_core->gprh[rB])
    {
        ch = 1;
    }
    else
    {
        ch = 0;
    }
    if (current_core->gpr[rA] > current_core->gpr[rB])
    {
        cl = 1;
    }
    else
    {
        cl = 0;
    }
    cr = (ch << 3) | (cl << 2) | ((ch | cl) << 1) | (ch & cl);
    ppc_opc_set_cr(current_core->cr, crD, cr);
}

/*
 *      evcmplts            Vector Compare Less Than Signed
 *      .563
 */
void ppc_opc_evcmplts(e500_core_t *current_core)
{
    int crD, rA, rB;
    PPC_OPC_TEMPL_CRX(current_core->current_opc, crD, rA, rB);

    int ch, cl, cr;
    if ((sint32)current_core->gprh[rA] < (sint32)current_core->gprh[rB])
    {
        ch = 1;
    }
    else
    {
        ch = 0;
    }
    if ((sint32)current_core->gpr[rA] < (sint32)current_core->gpr[rB])
    {
        cl = 1;
    }
    else
    {
        cl = 0;
    }
    cr = (ch << 3) | (cl << 2) | ((ch | cl) << 1) | (ch & cl);
    ppc_opc_set_cr(current_core->cr, crD, cr);
}

/*
 *      evcmpltu            Vector Compare Less Than Unsigned
 *      .562
 */
void ppc_opc_evcmpltu(e500_core_t *current_core)
{
    int crD, rA, rB;
    PPC_OPC_TEMPL_CRX(current_core->current_opc, crD, rA, rB);

    int ch, cl, cr;
    if (current_core->gprh[rA] < current_core->gprh[rB])
    {
        ch = 1;
    }
    else
    {
        ch = 0;
    }
    if (current_core->gpr[rA] < current_core->gpr[rB])
    {
        cl = 1;
    }
    else
    {
        cl = 0;
    }
    cr = (ch << 3) | (cl << 2) | ((ch | cl) << 1) | (ch & cl);
    ppc_opc_set_cr(current_core->cr, crD, cr);
}

/*
 *      evcntlsw            Vector Count Leading Signed Bits Word
 *      .526
 */
void ppc_opc_evcntlsw(e500_core_t *current_core)
{
    int rD, rA, rB;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    PPC_OPC_ASSERT(rB == 0);
    uint32_t n = 0;
    uint32_t x = 0x80000000;
    int vl = (int)(current_core->gpr[rA]);
    int vh = (int)(current_core->gprh[rA]);
    if (vh & 0x80000000)
    {
        x = 0x80000000;
        n = 0;
        while (vh & x)
        {
            n++;
            if (n == 32)
                break;
            x >>= 1;
        }
        current_core->gprh[rD] = n;
    }
    else
    {
        x = 0x80000000;
        n = 0;
        while (!(vh & x))
        {
            n++;
            if (n == 32)
                break;
            x >>= 1;
        }
        current_core->gprh[rD] = n;
    }

    if (vl & 0x80000000)
    {
        x = 0x80000000;
        n = 0;
        while (vl & x)
        {
            n++;
            if (n == 32)
                break;
            x >>= 1;
        }
        current_core->gpr[rD] = n;
    }
    else
    {
        x = 0x80000000;
        n = 0;
        while (!(vl & x))
        {
            n++;
            if (n == 32)
                break;
            x >>= 1;
        }
        current_core->gpr[rD] = n;
    }
}

/*
 *      evcntlzw            Vector Count Leading Zeros Word
 *      .525
 */
void ppc_opc_evcntlzw(e500_core_t *current_core)
{
    int rD, rA, rB;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    PPC_OPC_ASSERT(rB == 0);
    uint32_t n = 0;
    uint32_t x = 0x80000000;
    int vl = (int)(current_core->gpr[rA]);
    int vh = (int)(current_core->gprh[rA]);

    while (!(vl & x))
    {
        n++;
        if (n == 32)
            break;
        x >>= 1;
    }
    current_core->gpr[rD] = n;

    x = 0x80000000;
    n = 0;
    while (!(vh & x))
    {
        n++;
        if (n == 32)
            break;
        x >>= 1;
    }
    current_core->gprh[rD] = n;
}

/*
 *      evdivws            Vector Divide Word Signed
 *      .1222
 */
void ppc_opc_evdivws(e500_core_t *current_core)
{
    int rD, rA, rB;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    int32_t dividendh = current_core->gprh[rA];
    int32_t dividendl = current_core->gpr[rA];
    int32_t divisorh = current_core->gprh[rB];
    int32_t divisorl = current_core->gpr[rB];
    uint32_t ovh = 0;
    uint32_t ovl = 0;
    int rdh;

    if ((dividendh < 0) & (divisorh == 0))
    {
        current_core->gprh[rD] = 0x80000000;
        ovh = 1;
    }
    else if ((dividendh >= 0) & (divisorh == 0))
    {
        current_core->gprh[rD] = 0x7FFFFFFF;
        ovh = 1;
    }
    else if ((dividendh == 0x80000000) & (divisorh == 0xFFFFFFFF))
    {
        current_core->gprh[rD] = 0x7FFFFFFF;
        ovh = 1;
    }
    else
    {
        current_core->gprh[rD] = dividendh / divisorh;
    }

    if ((dividendl < 0) & (divisorl == 0))
    {
        current_core->gpr[rD] = 0x80000000;
        ovl = 1;
    }
    else if ((dividendl >= 0) & (divisorl == 0))
    {
        current_core->gpr[rD] = 0x7FFFFFFF;
        ovl = 1;
    }
    else if ((dividendl == 0x80000000) & (divisorl == 0xFFFFFFFF))
    {
        current_core->gpr[rD] = 0x7FFFFFFF;
        ovh = 1;
    }
    else
    {
        current_core->gpr[rD] = dividendl / divisorl;
    }

    current_core->spefscr.OVH = ovh;
    current_core->spefscr.OV = ovl;
    current_core->spefscr.SOVH |= ovh;
    current_core->spefscr.SOV |= ovl;
}

/*
 *      evdivwu            Vector Divide Word Unsigned
 *      .1223
 */
void ppc_opc_evdivwu(e500_core_t *current_core)
{
    int rD, rA, rB;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    uint32_t dividendh = current_core->gprh[rA];
    uint32_t dividendl = current_core->gpr[rA];
    uint32_t divisorh = current_core->gprh[rB];
    uint32_t divisorl = current_core->gpr[rB];
    uint32_t ovh = 0;
    uint32_t ovl = 0;

    if (divisorh == 0)
    {
        current_core->gprh[rD] = 0xFFFFffff;
        current_core->gpr[rD] = dividendl / divisorl;
        ovh = 1;
    }
    else if (divisorl == 0)
    {
        current_core->gprh[rD] = dividendh / divisorh;
        current_core->gpr[rD] = 0xFFFFffff;
        ovl = 1;
    }
    else
    {
        current_core->gprh[rD] = dividendh / divisorh;
        current_core->gpr[rD] = dividendl / divisorl;
    }

    current_core->spefscr.OVH = ovh;
    current_core->spefscr.OV = ovl;
    current_core->spefscr.SOVH |= ovh;
    current_core->spefscr.SOV |= ovl;
}

/*
 *      eveqv            Vector Equivalent
 *      .537
 */
void ppc_opc_eveqv(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    current_core->gprh[rD] = current_core->gprh[rA] ^ ~current_core->gprh[rB];
    current_core->gpr[rD] = current_core->gpr[rA] ^ ~current_core->gpr[rB];
}

/*
 *      evextsb            Vector Extend Sign Byte
 *      .522
 */
void ppc_opc_evextsb(e500_core_t *current_core)
{
    int rD, rA, rB;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    PPC_OPC_ASSERT(rB == 0);
    current_core->gpr[rD] = current_core->gpr[rA];
    if (current_core->gpr[rA] & 0x80)
    {
        current_core->gpr[rD] |= 0xFFFFff00;
    }
    else
    {
        current_core->gpr[rD] &= ~0xFFFFff00;
    }

    current_core->gprh[rD] = current_core->gprh[rA];
    if (current_core->gprh[rA] & 0x80)
    {
        current_core->gprh[rD] |= 0xFFFFff00;
    }
    else
    {
        current_core->gprh[rD] &= ~0xFFFFff00;
    }
}

/*
 *      evextsh            Vector Extend Sign Half Word
 *      .523
 */
void ppc_opc_evextsh(e500_core_t *current_core)
{
    int rD, rA, rB;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    PPC_OPC_ASSERT(rB == 0);
    current_core->gpr[rD] = current_core->gpr[rA];
    if (current_core->gpr[rD] & 0x8000)
    {
        current_core->gpr[rD] |= 0xFFFF0000;
    }
    else
    {
        current_core->gpr[rD] &= ~0xFFFF0000;
    }

    current_core->gprh[rD] = current_core->gprh[rA];
    if (current_core->gprh[rD] & 0x8000)
    {
        current_core->gprh[rD] |= 0xFFFF0000;
    }
    else
    {
        current_core->gprh[rD] &= ~0xFFFF0000;
    }
}

/*
 *      evfsabs            Vector Floating-Point Single-Precision Absolute Value
 *      .644
 */
void ppc_opc_evfsabs(e500_core_t *current_core)
{
    int rD, rA;
    PPC_OPC_TEMPL_XI(current_core->current_opc, rD, rA);
    float result;

    ppc_clear_float_status(current_core);
    result = fabs(*(float *)&current_core->gprh[rA]);
    current_core->gprh[rD] = *(uint32 *)&result;
    ppc_set_float_status_high(current_core);

    ppc_clear_float_status(current_core);
    result = fabs(*(float *)&current_core->gpr[rA]);
    current_core->gpr[rD] = *(uint32 *)&result;
    ppc_set_float_status(current_core);
}

/*
 *      evfsadd            Vector Floating-Point Single-Precision Add
 *      .640
 */
void ppc_opc_evfsadd(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    ppc_switch_rounding_mode(current_core);

    ppc_clear_float_status(current_core);
    float resulth = *(float *)&current_core->gprh[rA] + *(float *)&current_core->gprh[rB];
    current_core->gprh[rD] = *(uint32_t *)&resulth;
    ppc_set_float_status_high(current_core);

    ppc_clear_float_status(current_core);
    float result = *(float *)&current_core->gpr[rA] + *(float *)&current_core->gpr[rB];
    current_core->gpr[rD] = *(uint32_t *)&result;
    ppc_set_float_status(current_core);
}

/*
 *      evfscfsf            Vector Convert Floating-Point Single-Precision from Signed Fraction
 *      .659
 */
void ppc_opc_evfscfsf(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gprh[rD] = CnvtI32ToFP32Sat(current_core, current_core->gprh[rB], SIGN, UPPER, F);
    current_core->gpr[rD] = CnvtI32ToFP32Sat(current_core, current_core->gpr[rB], SIGN, LOWER, F);
}

/*
 *      evfscfsi            Vector Convert Floating-Point Single-Precision from Signed Integer
 *      .657
 */
void ppc_opc_evfscfsi(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gprh[rD] = CnvtI32ToFP32Sat(current_core, current_core->gprh[rB], SIGN, UPPER, I);
    current_core->gpr[rD] = CnvtI32ToFP32Sat(current_core, current_core->gpr[rB], SIGN, LOWER, I);
}

/*
 *      evfscfuf            Vector Convert Floating-Point Single-Precision from Unsigned Fraction
 *      .658
 */
void ppc_opc_evfscfuf(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gprh[rD] = CnvtI32ToFP32Sat(current_core, current_core->gprh[rB], UNSIGN, UPPER, F);
    current_core->gpr[rD] = CnvtI32ToFP32Sat(current_core, current_core->gpr[rB], UNSIGN, LOWER, F);
}

/*
 *      evfscfui            Vector Convert Floating-Point Single-Precision from Unsigned Integer
 *      .656
 */
void ppc_opc_evfscfui(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gprh[rD] = CnvtI32ToFP32Sat(current_core, current_core->gprh[rB], UNSIGN, UPPER, I);
    current_core->gpr[rD] = CnvtI32ToFP32Sat(current_core, current_core->gpr[rB], UNSIGN, LOWER, I);
}

/*
 *      evfscmpeq            Vector Floating-Point Single-Precision Compare Equal
 *      .654
 */
void ppc_opc_evfscmpeq(e500_core_t *current_core)
{
    int crfD, rA, rB;
    PPC_OPC_TEMPL_CRX(current_core->current_opc, crfD, rA, rB);
    int ch, cl, cr;

    ppc_clear_float_status(current_core);
    if (*(float *)&current_core->gprh[rA] == *(float *)&current_core->gprh[rB])
    {
        ch = 1;
    }
    else
    {
        ch = 0;
    }
    ppc_set_float_status_high(current_core);

    ppc_clear_float_status(current_core);
    if (*(float *)&current_core->gpr[rA] == *(float *)&current_core->gpr[rB])
    {
        cl = 1;
    }
    else
    {
        cl = 0;
    }
    ppc_set_float_status(current_core);

    cr = (ch << 3) | (cl << 2) | ((ch | cl) << 1) | (ch & cl);
    ppc_opc_set_cr(current_core->cr, crfD, cr);
}

/*
 *      evfscmpgt            Vector Floating-Point Single-Precision Compare Greater Than
 *      .652
 */
void ppc_opc_evfscmpgt(e500_core_t *current_core)
{
    int crfD, rA, rB;
    PPC_OPC_TEMPL_CRX(current_core->current_opc, crfD, rA, rB);
    int ch, cl, cr;

    ppc_clear_float_status(current_core);
    if (*(float *)&current_core->gprh[rA] > *(float *)&current_core->gprh[rB])
    {
        ch = 1;
    }
    else
    {
        ch = 0;
    }
    ppc_set_float_status_high(current_core);

    ppc_clear_float_status(current_core);
    if (*(float *)&current_core->gpr[rA] > *(float *)&current_core->gpr[rB])
    {
        cl = 1;
    }
    else
    {
        cl = 0;
    }
    ppc_set_float_status(current_core);

    cr = (ch << 3) | (cl << 2) | ((ch | cl) << 1) | (ch & cl);
    ppc_opc_set_cr(current_core->cr, crfD, cr);
}

/*
 *      evfscmplt            Vector Floating-Point Single-Precision Compare Less Than
 *      .653
 */
void ppc_opc_evfscmplt(e500_core_t *current_core)
{
    int crfD, rA, rB;
    PPC_OPC_TEMPL_CRX(current_core->current_opc, crfD, rA, rB);
    int ch, cl, cr;

    ppc_clear_float_status(current_core);
    if (*(float *)&current_core->gprh[rA] < *(float *)&current_core->gprh[rB])
    {
        ch = 1;
    }
    else
    {
        ch = 0;
    }
    ppc_set_float_status_high(current_core);

    ppc_clear_float_status(current_core);
    if (*(float *)&current_core->gpr[rA] < *(float *)&current_core->gpr[rB])
    {
        cl = 1;
    }
    else
    {
        cl = 0;
    }
    ppc_set_float_status(current_core);

    cr = (ch << 3) | (cl << 2) | ((ch | cl) << 1) | (ch & cl);
    ppc_opc_set_cr(current_core->cr, crfD, cr);
}

/*
 *      evfsctsf            Vector Convert Floating-Point Single-Precision to Signed Fraction
 *      .663
 */
void ppc_opc_evfsctsf(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gprh[rD] = CnvtFP32ToI32Sat(current_core, current_core->gprh[rB], SIGN, UPPER, ROUND, F);
    current_core->gpr[rD] = CnvtFP32ToI32Sat(current_core, current_core->gpr[rB], SIGN, LOWER, ROUND, F);
}

/*
 *      evfsctsi            Vector Convert Floating-Point Single-Precision to Signed Integer
 *      .661
 */
void ppc_opc_evfsctsi(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gprh[rD] = CnvtFP32ToI32Sat(current_core, current_core->gprh[rB], SIGN, UPPER, ROUND, I);
    current_core->gpr[rD] = CnvtFP32ToI32Sat(current_core, current_core->gpr[rB], SIGN, LOWER, ROUND, I);
}

/*
 *      evfsctsiz            Vector Convert Floating-Point Single-Precision to Signed Integer with Round toward Zero
 *      .666
 */
void ppc_opc_evfsctsiz(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gprh[rD] = CnvtFP32ToI32Sat(current_core, current_core->gprh[rB], SIGN, UPPER, TRUNC, I);
    current_core->gpr[rD] = CnvtFP32ToI32Sat(current_core, current_core->gpr[rB], SIGN, LOWER, TRUNC, I);
}

/*
 *      evfsctuf            Vector Convert Floating-Point Single-Precision to Unsigned Fraction
 *      .662
 */
void ppc_opc_evfsctuf(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gprh[rD] = CnvtFP32ToI32Sat(current_core, current_core->gprh[rB], UNSIGN, UPPER, ROUND, F);
    current_core->gpr[rD] = CnvtFP32ToI32Sat(current_core, current_core->gpr[rB], UNSIGN, LOWER, ROUND, F);
}

/*
 *      evfsctui            Vector Convert Floating-Point Single-Precision to Unsigned Integer
 *      .660
 */
void ppc_opc_evfsctui(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gprh[rD] = CnvtFP32ToI32Sat(current_core, current_core->gprh[rB], UNSIGN, UPPER, ROUND, I);
    current_core->gpr[rD] = CnvtFP32ToI32Sat(current_core, current_core->gpr[rB], UNSIGN, LOWER, ROUND, I);
}

/*
 *      evfsctuiz            Vector Convert Floating-Point Single-Precision to Unsigned Integer with Round toward Zero
 *      .664
 */
void ppc_opc_evfsctuiz(e500_core_t *current_core)
{
    int rD, rB;
    PPC_OPC_TEMPL_IX(current_core->current_opc, rD, rB);

    current_core->gprh[rD] = CnvtFP32ToI32Sat(current_core, current_core->gprh[rB], UNSIGN, UPPER, TRUNC, I);
    current_core->gpr[rD] = CnvtFP32ToI32Sat(current_core, current_core->gpr[rB], UNSIGN, LOWER, TRUNC, I);
}

/*
 *      evfsdiv            Vector Floating-Point Single-Precision Divide
 *      .649
 */
void ppc_opc_evfsdiv(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    ppc_switch_rounding_mode(current_core);

    ppc_clear_float_status(current_core);
    float resulth = *(float *)&current_core->gprh[rA] / *(float *)&current_core->gprh[rB];
    current_core->gprh[rD] = *(uint32_t *)&resulth;
    ppc_set_float_status_high(current_core);

    ppc_clear_float_status(current_core);
    float result = *(float *)&current_core->gpr[rA] / *(float *)&current_core->gpr[rB];
    current_core->gpr[rD] = *(uint32_t *)&result;
    ppc_set_float_status(current_core);
}

/*
 *      evfsmul            Vector Floating-Point Single-Precision Multiply
 *      .648
 */
void ppc_opc_evfsmul(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    ppc_switch_rounding_mode(current_core);

    ppc_clear_float_status();
    float resulth = *(float *)&current_core->gprh[rA] * *(float *)&current_core->gprh[rB];
    current_core->gprh[rD] = *(uint32_t *)&resulth;
    ppc_set_float_status_high(current_core);

    ppc_clear_float_status(current_core);
    float result = *(float *)&current_core->gpr[rA] * *(float *)&current_core->gpr[rB];
    current_core->gpr[rD] = *(uint32_t *)&result;
    ppc_set_float_status(current_core);
}

/*
 *      evfsnabs            Vector Floating-Point Single-Precision Negative Absolute Value
 *      .645
 */
void ppc_opc_evfsnabs(e500_core_t *current_core)
{
    int rD, rA;
    PPC_OPC_TEMPL_XI(current_core->current_opc, rD, rA);

    current_core->gprh[rD] = current_core->gprh[rA] | 0x80000000;
    current_core->gpr[rD] = current_core->gpr[rA] | 0x80000000;
}

/*
 *      evfsneg            Vector Floating-Point Single-Precision Negate
 *      .646
 */
void ppc_opc_evfsneg(e500_core_t *current_core)
{
    int rD, rA;
    PPC_OPC_TEMPL_XI(current_core->current_opc, rD, rA);

    current_core->gprh[rD] = current_core->gprh[rA] ^ 0x80000000;
    current_core->gpr[rD] = current_core->gpr[rA] ^ 0x80000000;
}

/*
 *      evfssub            Vector Floating-Point Single-Precision Subtract
 *      .641
 */
void ppc_opc_evfssub(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    ppc_switch_rounding_mode(current_core);

    ppc_clear_float_status(current_core);
    float resulth = *(float *)&current_core->gprh[rA] - *(float *)&current_core->gprh[rB];
    current_core->gprh[rD] = *(uint32_t *)&resulth;
    ppc_set_float_status_high(current_core);

    ppc_clear_float_status(current_core);
    float result = *(float *)&current_core->gpr[rA] - *(float *)&current_core->gpr[rB];
    current_core->gpr[rD] = *(uint32_t *)&result;
    ppc_set_float_status(current_core);
}

/*
 *      evfststeq            Vector Floating-Point Single-Precision Test Equal
 *      .670
 */
void ppc_opc_evfststeq(e500_core_t *current_core)
{
    int crfD, rA, rB;
    PPC_OPC_TEMPL_CRX(current_core->current_opc, crfD, rA, rB);

    int ch, cl, cr;
    if (*(float *)&current_core->gprh[rA] == *(float *)&current_core->gprh[rB])
    {
        ch = 1;
    }
    else
    {
        ch = 0;
    }
    if (*(float *)&current_core->gpr[rA] == *(float *)&current_core->gpr[rB])
    {
        cl = 1;
    }
    else
    {
        cl = 0;
    }
    cr = (ch << 3) | (cl << 2) | ((ch | cl) << 1) | (ch & cl);
    ppc_opc_set_cr(current_core->cr, crfD, cr);
}

/*
 *      evfststgt            Vector Floating-Point Single-Precision Test Greater Than
 *      .668
 */
void ppc_opc_evfststgt(e500_core_t *current_core)
{
    int crfD, rA, rB;
    PPC_OPC_TEMPL_CRX(current_core->current_opc, crfD, rA, rB);

    int ch, cl, cr;
    if (*(float *)&current_core->gprh[rA] > *(float *)&current_core->gprh[rB])
    {
        ch = 1;
    }
    else
    {
        ch = 0;
    }
    if (*(float *)&current_core->gpr[rA] > *(float *)&current_core->gpr[rB])
    {
        cl = 1;
    }
    else
    {
        cl = 0;
    }
    cr = (ch << 3) | (cl << 2) | ((ch | cl) << 1) | (ch & cl);
    ppc_opc_set_cr(current_core->cr, crfD, cr);
}

/*
 *      evfststlt            Vector Floating-Point Single-Precision Test Less Than
 *      .669
 */
void ppc_opc_evfststlt(e500_core_t *current_core)
{
    int crfD, rA, rB;
    PPC_OPC_TEMPL_CRX(current_core->current_opc, crfD, rA, rB);

    int ch, cl, cr;
    if (*(float *)&current_core->gprh[rA] < *(float *)&current_core->gprh[rB])
    {
        ch = 1;
    }
    else
    {
        ch = 0;
    }
    if (*(float *)&current_core->gpr[rA] < *(float *)&current_core->gpr[rB])
    {
        cl = 1;
    }
    else
    {
        cl = 0;
    }
    cr = (ch << 3) | (cl << 2) | ((ch | cl) << 1) | (ch & cl);
    ppc_opc_set_cr(current_core->cr, crfD, cr);
}

/*
 *  Vector load doubleword into doubleword
 *  .769
 */
void ppc_opc_evldd(e500_core_t *current_core)
{
    int rD, rA, UIMM;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, UIMM);

    uint32 EA = current_core->gpr[rA] + UIMM * 8;
    uint64 gprD;
    ppc_read_effective_dword(current_core, EA, &gprD);
    current_core->gprh[rD] = gprD >> 32;
    current_core->gpr[rD] = gprD;
}

/*
 *      evlddx            Vector Load Double Word into Double Word Indexed
 *      .768
 */
void ppc_opc_evlddx(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    uint32 EA = current_core->gpr[rA] + current_core->gpr[rB];
    uint64 gprD;
    ppc_read_effective_dword(current_core, EA, &gprD);
    current_core->gprh[rD] = gprD >> 32;
    current_core->gpr[rD] = gprD;
}

/*
 *      evldh            Vector Load Double into Four Half Words
 *      .773
 */
void ppc_opc_evldh(e500_core_t *current_core)
{
    int rD, rA, UIMM;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, UIMM);

    uint32 EA = current_core->gpr[rA] + UIMM * 8;
    uint32 gprhhD, gprhlD, gprhD, gprlD;
    ppc_read_effective_half(current_core, EA, &gprhhD);
    ppc_read_effective_half(current_core, EA + 2, &gprhlD);
    ppc_read_effective_half(current_core, EA + 4, &gprhD);
    ppc_read_effective_half(current_core, EA + 6, &gprlD);
    current_core->gprh[rD] = gprhhD << 16 | gprhlD & 0xFFFF;
    current_core->gpr[rD] = gprhD << 16 | gprlD & 0xFFFF;
}

/*
 *      evldhx            Vector Load Double into Four Half Words Indexed
 *      .772
 */
void ppc_opc_evldhx(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    uint32 EA = current_core->gpr[rA] + current_core->gpr[rB];
    uint32 gprhhD, gprhlD, gprhD, gprlD;
    ppc_read_effective_half(current_core, EA, &gprhhD);
    ppc_read_effective_half(current_core, EA + 2, &gprhlD);
    ppc_read_effective_half(current_core, EA + 4, &gprhD);
    ppc_read_effective_half(current_core, EA + 6, &gprlD);
    current_core->gprh[rD] = gprhhD << 16 | gprhlD & 0xFFFF;
    current_core->gpr[rD] = gprhD << 16 | gprlD & 0xFFFF;
}

/*
 *      Vector Load Double into Two Words
 *      .771
 */
void ppc_opc_evldw(e500_core_t *current_core)
{
    int rD, rA, UIMM;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, UIMM);

    uint32 EA = current_core->gpr[rA] + UIMM * 8;
    uint32 gprhD, gprD;
    ppc_read_effective_word(current_core, EA, &gprhD);
    ppc_read_effective_word(current_core, EA + 4, &gprD);
    current_core->gprh[rD] = gprhD;
    current_core->gpr[rD] = gprD;
}

/*
 *      evldwx            Vector Load Double into Two Words Indexed
 *      .770
 */
void ppc_opc_evldwx(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    uint32 EA = current_core->gpr[rA] + current_core->gpr[rB];
    uint32 gprhD, gprD;
    ppc_read_effective_word(current_core, EA, &gprhD);
    ppc_read_effective_word(current_core, EA + 4, &gprD);
    current_core->gprh[rD] = gprhD;
    current_core->gpr[rD] = gprD;
}

/*
 *      evlhhesplat            Vector Load Half Word into Half Words Even and Splat
 *      .777
 */
void ppc_opc_evlhhesplat(e500_core_t *current_core)
{
    uint32_t rdhh, rdlh;
    int rD, rA;
    uint32_t EA, UIMM;
    PPC_OPC_TEMPL_D_UImm(current_core->current_opc, rD, rA, UIMM);
    if (rA == 0)
    {
        EA = (UIMM >> 11) * 2;
    }
    else
    {
        EA = current_core->gpr[rA] + (UIMM >> 11) * 2;
    }
    ppc_read_effective_half(current_core, EA, &rdhh);
    current_core->gprh[rD] = rdhh << 16;
    ppc_read_effective_half(current_core, EA, &rdlh);
    current_core->gpr[rD] = rdlh << 16;
}

/*
 *      evlhhesplatx            Vector Load Half Word into Half Words Even and Splat Indexed
 *      .776
 */
void ppc_opc_evlhhesplatx(e500_core_t *current_core)
{
    uint32_t rdhh, rdlh;
    int rD, rA, rB;
    uint32_t EA;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    if (rA == 0)
    {
        EA = current_core->gpr[rB];
    }
    else
    {
        EA = current_core->gpr[rA] + current_core->gpr[rB];
    }
    ppc_read_effective_half(current_core, EA, &rdhh);
    current_core->gprh[rD] = rdhh << 16;
    ppc_read_effective_half(current_core, EA, &rdlh);
    current_core->gpr[rD] = rdlh << 16;
}

/*
 *      evlhhossplat            Vector Load Half Word into Half Word Odd Signed and Splat
 *      .783
 */
void ppc_opc_evlhhossplat(e500_core_t *current_core)
{
    int rdhh, rdlh;
    int rD, rA;
    uint32_t EA, UIMM;
    PPC_OPC_TEMPL_D_UImm(current_core->current_opc, rD, rA, UIMM);
    if (rA == 0)
    {
        EA = ((UIMM >> 11) & 0x1f) * 2;
    }
    else
    {
        EA = current_core->gpr[rA] + ((UIMM >> 11) & 0x1f) * 2;
    }
    ppc_read_effective_half(current_core, EA, &rdhh);
    current_core->gprh[rD] = (rdhh << 16) >> 16;
    ppc_read_effective_half(current_core, EA, &rdlh);
    current_core->gpr[rD] = (rdlh << 16) >> 16;
}

/*
 *      evlhhossplatx            Vector Load Half Word into Half Word Odd Signed and Splat Indexed
 *      .782
 */
void ppc_opc_evlhhossplatx(e500_core_t *current_core)
{
    int rdhh, rdlh;
    int rD, rA, rB;
    uint32_t EA;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    if (rA == 0)
    {
        EA = current_core->gpr[rB];
    }
    else
    {
        EA = current_core->gpr[rA] + current_core->gpr[rB];
    }
    ppc_read_effective_half(current_core, EA, &rdhh);
    current_core->gprh[rD] = (rdhh << 16) >> 16;
    ppc_read_effective_half(current_core, EA, &rdlh);
    current_core->gpr[rD] = (rdlh << 16) >> 16;
}

/*
 *      evlhhousplat            Vector Load Half Word into Half Word Odd Unsigned and Splat
 *      .781
 */
void ppc_opc_evlhhousplat(e500_core_t *current_core)
{
    uint32_t rdhh, rdlh;
    int rD, rA;
    uint32_t EA, UIMM;
    PPC_OPC_TEMPL_D_UImm(current_core->current_opc, rD, rA, UIMM);
    if (rA == 0)
    {
        EA = ((UIMM >> 11) & 0x1f) * 2;
    }
    else
    {
        EA = current_core->gpr[rA] + ((UIMM >> 11) & 0x1f) * 2;
    }
    ppc_read_effective_half(current_core, EA, &rdhh);
    current_core->gprh[rD] = (rdhh << 16) >> 16;
    ppc_read_effective_half(current_core, EA, &rdlh);
    current_core->gpr[rD] = (rdlh << 16) >> 16;
}

/*
 *      evlhhousplatx            Vector Load Half Word into Half Word Odd Unsigned and Splat Indexed
 *      .780
 */
void ppc_opc_evlhhousplatx(e500_core_t *current_core)
{
    uint32_t rdhh, rdlh;
    int rD, rA, rB;
    uint32_t EA;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    if (rA == 0)
    {
        EA = current_core->gpr[rB];
    }
    else
    {
        EA = current_core->gpr[rA] + current_core->gpr[rB];
    }
    ppc_read_effective_half(current_core, EA, &rdhh);
    current_core->gprh[rD] = rdhh & 0xFFFF;
    ppc_read_effective_half(current_core, EA, &rdlh);
    current_core->gpr[rD] = rdlh & 0xFFFF;
}

/*
 *      evlwhe            Vector Load Word into Two Half Words Even
 *      .785
 */
void ppc_opc_evlwhe(e500_core_t *current_core)
{
    uint32_t rdhh, rdlh;
    int rD, rA;
    uint32_t EA, UIMM;
    PPC_OPC_TEMPL_D_UImm(current_core->current_opc, rD, rA, UIMM);
    if (rA == 0)
    {
        EA = ((UIMM >> 11) & 0x1f) * 4;
    }
    else
    {
        EA = current_core->gpr[rA] + ((UIMM >> 11) & 0x1f) * 4;
    }
    ppc_read_effective_half(current_core, EA, &rdhh);
    current_core->gprh[rD] = rdhh << 16;
    ppc_read_effective_half(current_core, EA + 2, &rdlh);
    current_core->gpr[rD] = rdlh << 16;
}

/*
 *      evlwhex            Vector Load Word into Two Half Words Even Indexed
 *      .784
 */
void ppc_opc_evlwhex(e500_core_t *current_core)
{
    uint32_t rdhh, rdlh;
    int rD, rA, rB;
    uint32_t EA;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    if (rA == 0)
    {
        EA = current_core->gpr[rB];
    }
    else
    {
        EA = current_core->gpr[rA] + current_core->gpr[rB];
    }

    ppc_read_effective_half(current_core, EA, &rdhh);
    current_core->gprh[rD] = rdhh << 16;
    ppc_read_effective_half(current_core, EA + 2, &rdlh);
    current_core->gpr[rD] = rdlh << 16;
}

/*
 *      evlwhos            Vector Load Word into Two Half Words Odd Signed (with sign extension)
 *      .791
 */
void ppc_opc_evlwhos(e500_core_t *current_core)
{
    int rdhh, rdlh;
    int rD, rA;
    uint32_t EA, UIMM;
    PPC_OPC_TEMPL_D_UImm(current_core->current_opc, rD, rA, UIMM);
    if (rA == 0)
    {
        EA = ((UIMM >> 11) & 0x1f) * 4;
    }
    else
    {
        EA = current_core->gpr[rA] + ((UIMM >> 11) & 0x1f) * 4;
    }
    ppc_read_effective_half(current_core, EA, &rdhh);
    current_core->gprh[rD] = (rdhh << 16) >> 16;
    ppc_read_effective_half(current_core, EA + 2, &rdlh);
    current_core->gpr[rD] = (rdlh << 16) >> 16;
}

/*
 *      evlwhosx            Vector Load Word into Two Half Words Odd Signed (with sign extension)
 *      .790
 */
void ppc_opc_evlwhosx(e500_core_t *current_core)
{
    int rdhh, rdlh;
    int rD, rA, rB;
    uint32_t EA;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    if (rA == 0)
    {
        EA = current_core->gpr[rB];
    }
    else
    {
        EA = current_core->gpr[rA] + current_core->gpr[rB];
    }
    ppc_read_effective_half(current_core, EA, &rdhh);
    current_core->gprh[rD] = rdhh << 16 >> 16;
    ppc_read_effective_half(current_core, EA + 2, &rdlh);
    current_core->gpr[rD] = (rdlh << 16) >> 16;
}

/*
 *      evlwhou            Vector Load Word into Two Half Words Odd Unsigned (zero-extended)
 *      .789
 */
void ppc_opc_evlwhou(e500_core_t *current_core)
{
    uint32_t rdhh, rdlh;
    int rD, rA;
    uint32_t EA, UIMM;
    PPC_OPC_TEMPL_D_UImm(current_core->current_opc, rD, rA, UIMM);
    if (rA == 0)
    {
        EA = ((UIMM >> 11) & 0x1f) * 4;
    }
    else
    {
        EA = current_core->gpr[rA] + ((UIMM >> 11) & 0x1f) * 4;
    }
    ppc_read_effective_half(current_core, EA, &rdhh);
    current_core->gprh[rD] = rdhh & 0xFFFF;
    ppc_read_effective_half(current_core, EA + 2, &rdlh);
    current_core->gpr[rD] = rdlh & 0xFFFF;
}

/*
 *      evlwhoux            Vector Load Word into Two Half Words Odd Unsigned Indexed (zero-extended)
 *      .788
 */
void ppc_opc_evlwhoux(e500_core_t *current_core)
{
    uint32_t rdhh, rdlh;
    int rD, rA, rB;
    uint32_t EA;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    if (rA == 0)
    {
        EA = current_core->gpr[rB];
    }
    else
    {
        EA = current_core->gpr[rA] + current_core->gpr[rB];
    }
    ppc_read_effective_half(current_core, EA, &rdhh);
    current_core->gprh[rD] = rdhh & 0xFFFF;
    ppc_read_effective_half(current_core, EA + 2, &rdlh);
    current_core->gpr[rD] = rdlh & 0xFFFF;
}

/*
 *      evlwhsplat            Vector Load Word into Two Half Words and Splat
 *      .797
 */
void ppc_opc_evlwhsplat(e500_core_t *current_core)
{
    uint32_t rdhh, rdhl, rdlh, rdll;
    int rD, rA;
    uint32_t EA, UIMM;

    PPC_OPC_TEMPL_D_UImm(current_core->current_opc, rD, rA, UIMM);
    if (rA == 0)
    {
        EA = (UIMM >> 11) * 4;
    }
    else
    {
        EA = current_core->gpr[rA] + (UIMM >> 11) * 4;
    }

    ppc_read_effective_half(current_core, EA, &rdhh);
    ppc_read_effective_half(current_core, EA, &rdhl);
    current_core->gprh[rD] = (rdhh << 16) + (rdhl & 0x0000ffff);
    ppc_read_effective_half(current_core, EA + 2, &rdlh);
    ppc_read_effective_half(current_core, EA + 2, &rdll);
    current_core->gpr[rD] = (rdlh << 16) + (rdll & 0x0000ffff);
}

/*
 *      evlwhsplatx            Vector Load Word into Two Half Words and Splat Indexed
 *      .796
 */
void ppc_opc_evlwhsplatx(e500_core_t *current_core)
{
    uint32_t rdhh, rdhl, rdlh, rdll;
    int rD, rA, rB;
    uint32_t EA;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    if (rA == 0)
    {
        EA = current_core->gpr[rB];
    }
    else
    {
        EA = current_core->gpr[rA] + current_core->gpr[rB];
    }

    ppc_read_effective_half(current_core, EA, &rdhh);
    ppc_read_effective_half(current_core, EA, &rdhl);
    current_core->gprh[rD] = (rdhh << 16) + (rdhl & 0x0000ffff);
    ppc_read_effective_half(current_core, EA + 2, &rdlh);
    ppc_read_effective_half(current_core, EA + 2, &rdll);
    current_core->gpr[rD] = (rdlh << 16) + (rdll & 0x0000ffff);
}

/*
 *      evlwwsplat            Vector Load Word into Word and Splat
 *      .793
 */
void ppc_opc_evlwwsplat(e500_core_t *current_core)
{
    int rD, rA;
    uint32_t EA, UIMM;

    PPC_OPC_TEMPL_D_UImm(current_core->current_opc, rD, rA, UIMM);
    if (rA == 0)
    {
        EA = (UIMM >> 11) * 4;
    }
    else
    {
        EA = current_core->gpr[rA] + (UIMM >> 11) * 4;
    }

    ppc_read_effective_word(current_core, EA, &current_core->gprh[rD]);
    ppc_read_effective_word(current_core, EA, &current_core->gpr[rD]);
}

/*
 *      evlwwsplatx            Vector Load Word into Word and Splat Indexed
 *      .792
 */
void ppc_opc_evlwwsplatx(e500_core_t *current_core)
{
    int rD, rA, rB;
    uint32_t EA;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    if (rA == 0)
    {
        EA = current_core->gpr[rB];
    }
    else
    {
        EA = current_core->gpr[rA] + current_core->gpr[rB];
    }

    ppc_read_effective_word(current_core, EA, &current_core->gprh[rD]);
    ppc_read_effective_word(current_core, EA, &current_core->gpr[rD]);
}

/*
 *  Vector Merge High
 *  .556
 */
void ppc_opc_evmergehi(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    current_core->gprh[rD] = current_core->gprh[rA];
    current_core->gpr[rD] = current_core->gprh[rB];
}

/*
 *  Vector Merge High/Low
 *  .558
 */
void ppc_opc_evmergehilo(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    uint32_t rah_value = current_core->gprh[rA];
    uint32_t rbl_value = current_core->gpr[rB];

    current_core->gprh[rD] = rah_value;
    current_core->gpr[rD] = rbl_value;
}

/*
 *  Vector Merge Low
 *  .557
 */
void ppc_opc_evmergelo(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    uint32_t ral_value = current_core->gpr[rA];
    uint32_t rbl_value = current_core->gpr[rB];

    current_core->gprh[rD] = ral_value;
    current_core->gpr[rD] = rbl_value;
}

/*
 *  Vector Merge Low/High
 *  .559
 */
void ppc_opc_evmergelohi(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    uint32_t ral_value = current_core->gpr[rA];
    uint32_t rbh_value = current_core->gprh[rB];
    current_core->gprh[rD] = ral_value;
    current_core->gpr[rD] = rbh_value;
}

/*
 *      evmhegsmfaa            Vector Multiply Half Words, Even, Guarded, Signed, Modulo, Fractional and Accumulate
 *      .1323
 */
void ppc_opc_evmhegsmfaa(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp;
    int32_t temph;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    temph = ((int)(current_core->gpr[rB]) >> 16) * ((int)(current_core->gpr[rA]) >> 16);
    temph &= 0x7fffffff;
    temp = (((int64_t)(temph) << 32) >> 32);
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) + (uint32_t)(temp >> 32);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) + (uint32_t)(temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *      evmhegsmfan            Vector Multiply Half Words, Even, Guarded, Signed, Modulo, Fractional and Accumulate Negative
 *      .1451
 */
void ppc_opc_evmhegsmfan(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp;
    int32_t temph;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = ((int)(current_core->gpr[rB]) >> 16) * ((int)(current_core->gpr[rA]) >> 16);
    temph &= 0x7fffffff;
    temp = ((int64_t)(temph) << 32 >> 32);
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) - (uint32_t)(temp >> 32);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) - (uint32_t)(temp);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *      evmhegsmiaa           Vector Multiply Half Words, Even, Guarded, Signed, Modulo, Integer and Accumulate
 *      .1321
 */
void ppc_opc_evmhegsmiaa(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp;
    int32_t temph;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = ((int)(current_core->gpr[rB]) >> 16) * ((int)(current_core->gpr[rA]) >> 16);
    temp = ((int64_t)(temph) << 32 >> 32);
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) + (uint32_t)(temp >> 32);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) + (uint32_t)(temp);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *      evmhegsmian           Vector Multiply Half Words, Even, Guarded, Signed, Modulo, Integer and Accumulate Negative
 *      .1449
 */
void ppc_opc_evmhegsmian(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp;
    int32_t temph;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = ((int)(current_core->gpr[rB]) >> 16) * ((int)(current_core->gpr[rA]) >> 16);
    temp = ((int64_t)(temph) << 32 >> 32);
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) - (uint32_t)(temp >> 32);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) - (uint32_t)(temp);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *      evmhegumiaa           Vector Multiply Half Words, Even, Guarded, Unsigned, Modulo, Integer and Accumulate
 *      .1320
 */
void ppc_opc_evmhegumiaa(e500_core_t *current_core)
{
    int rD, rA, rB;
    uint64_t temp;
    uint32_t temph;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = (current_core->gpr[rB] >> 16) * (current_core->gpr[rA] >> 16);
    temp = (uint64_t)(temph);
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) + (uint32_t)(temp >> 32);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) + (uint32_t)(temp);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *      evmhegumian           Vector Multiply Half Words, Even, Guarded, Unsigned, Modulo, Integer and Accumulate Negative
 *      .1448
 */
void ppc_opc_evmhegumian(e500_core_t *current_core)
{
    int rD, rA, rB;
    uint64_t temp;
    uint32_t temph;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = (current_core->gpr[rB] >> 16) * (current_core->gpr[rA] >> 16);
    temp = (uint64_t)(temph);
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) - (uint32_t)(temp >> 32);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) - (uint32_t)(temp);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *      evmhesmf           Vector Multiply Half Words, Even, Signed, Modulo, Fractional (to Accumulator)
 *      .1035
 */
void ppc_opc_evmhesmf(e500_core_t *current_core)
{
    int rD, rA, rB;
    int32_t temph, templ;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    templ = ((int)(current_core->gpr[rB]) >> 16) * ((int)(current_core->gpr[rA]) >> 16);
    temph = ((int)(current_core->gprh[rB]) >> 16) * ((int)(current_core->gprh[rA]) >> 16);
    temph &= 0x7fffffff;
    templ &= 0x7fffffff;

    current_core->gprh[rD] = (uint32_t)(temph);
    current_core->gpr[rD] = (uint32_t)(templ);
}

/*
 *      evmhesmfa           Vector Multiply Half Words, Even, Signed, Modulo, Fractional (to Accumulator)
 *      .1067
 */
void ppc_opc_evmhesmfa(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp;
    int32_t temph, templ;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    templ = ((int)(current_core->gpr[rB]) >> 16) * ((int)(current_core->gpr[rA]) >> 16);
    temph = ((int)(current_core->gprh[rB]) >> 16) * ((int)(current_core->gprh[rA]) >> 16);
    temph &= 0x7fffffff;
    templ &= 0x7fffffff;

    current_core->gprh[rD] = (uint32_t)(temph);
    current_core->gpr[rD] = (uint32_t)(templ);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *      evmhesmfaaw           Vector Multiply Half Words, Even, Signed, Modulo, Fractional and Accumulate into Words
 *      .1291
 */
void ppc_opc_evmhesmfaaw(e500_core_t *current_core)
{
    int rD, rA, rB;
    int32_t temph;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = ((int)(current_core->gprh[rB]) >> 16) * ((int)(current_core->gprh[rA]) >> 16);
    temph &= 0x7fffffff;
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) + (uint32_t)(temph);

    temph = ((int)(current_core->gpr[rB]) >> 16) * ((int)(current_core->gpr[rA]) >> 16);
    temph &= 0x7fffffff;
    current_core->gpr[rD] = (uint32_t)(current_core->acc) + (uint32_t)(temph);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *      evmhesmfanw           Vector Multiply Half Words, Even, Signed, Modulo, Fractional and Accumulate Negative into Words
 *      .1419
 */
void ppc_opc_evmhesmfanw(e500_core_t *current_core)
{
    int rD, rA, rB;
    int32_t temph;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = ((int)(current_core->gprh[rB]) >> 16) * ((int)(current_core->gprh[rA]) >> 16);
    temph &= 0x7fffffff;
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) - (uint32_t)(temph);

    temph = ((int)(current_core->gpr[rB]) >> 16) * ((int)(current_core->gpr[rA]) >> 16);
    temph &= 0x7fffffff;
    current_core->gpr[rD] = (uint32_t)(current_core->acc) - (uint32_t)(temph);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *      evmhesmi           Vector Multiply Half Words, Even, Signed, Modulo, Integer (to Accumulator)
 *      .1033
 */
void ppc_opc_evmhesmi(e500_core_t *current_core)
{
    int rD, rA, rB;
    int32_t temph, templ;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    templ = ((int)(current_core->gpr[rB]) >> 16) * ((int)(current_core->gpr[rA]) >> 16);
    temph = ((int)(current_core->gprh[rB]) >> 16) * ((int)(current_core->gprh[rA]) >> 16);

    current_core->gprh[rD] = (uint32_t)(temph);
    current_core->gpr[rD] = (uint32_t)(templ);
}

/*
 *      evmhesmia           Vector Multiply Half Words, Even, Signed, Modulo, Integer (to Accumulator)
 *      .1065
 */
void ppc_opc_evmhesmia(e500_core_t *current_core)
{
    int rD, rA, rB;
    int32_t temph, templ;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    templ = ((int)(current_core->gpr[rB]) >> 16) * ((int)(current_core->gpr[rA]) >> 16);
    temph = ((int)(current_core->gprh[rB]) >> 16) * ((int)(current_core->gprh[rA]) >> 16);

    current_core->gprh[rD] = (uint32_t)(temph);
    current_core->gpr[rD] = (uint32_t)(templ);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *      evmhesmiaaw           Vector Multiply Half Words, Even, Signed, Modulo, Integer and Accumulate into Words
 *      .1289
 */
void ppc_opc_evmhesmiaaw(e500_core_t *current_core)
{
    int rD, rA, rB;
    int32_t temph;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = ((int)(current_core->gprh[rB]) >> 16) * ((int)(current_core->gprh[rA]) >> 16);
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) + (uint32_t)(temph);

    temph = ((int)(current_core->gpr[rB]) >> 16) * ((int)(current_core->gpr[rA]) >> 16);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) + (uint32_t)(temph);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *      evmhesmianw           Vector Multiply Half Words, Even, Signed, Modulo, Integer and Accumulate Negative into Words
 *      .1417
 */
void ppc_opc_evmhesmianw(e500_core_t *current_core)
{
    int rD, rA, rB;
    int32_t temph;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = ((int)(current_core->gprh[rB]) >> 16) * ((int)(current_core->gprh[rA]) >> 16);
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) - (uint32_t)(temph);

    temph = ((int)(current_core->gpr[rB]) >> 16) * ((int)(current_core->gpr[rA]) >> 16);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) - (uint32_t)(temph);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *      evmhessf           Vector Multiply Half Words, Even, Signed, Saturate, Fractional (to Accumulator)
 *      .1027
 */
void ppc_opc_evmhessf(e500_core_t *current_core)
{
    int rD, rA, rB;
    int32_t temph, movh, movl;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = ((int)(current_core->gprh[rB]) >> 16) * ((int)(current_core->gprh[rA]) >> 16);
    temph &= 0x7fffffff;
    if ((current_core->gprh[rA] >> 16 == 0x8000) & (current_core->gprh[rB] >> 16 == 0x8000))
    {
        current_core->gprh[rD] = 0x7FFFFFFF;
        movh = 1;
    }
    else
    {
        current_core->gprh[rD] = temph;
        movh = 0;
    }

    temph = ((int)(current_core->gpr[rB]) >> 16) * ((int)(current_core->gpr[rA]) >> 16);
    temph &= 0x7fffffff;
    if ((current_core->gpr[rA] >> 16 == 0x8000) & (current_core->gpr[rB] >> 16 == 0x8000))
    {
        current_core->gpr[rD] = 0x7FFFFFFF;
        movl = 1;
    }
    else
    {
        current_core->gpr[rD] = temph;
        movl = 0;
    }

    current_core->spefscr.OVH = movh;
    current_core->spefscr.OV = movl;
    current_core->spefscr.SOVH |= movh;
    current_core->spefscr.SOV |= movl;
}

/*
 *      evmhessfa           Vector Multiply Half Words, Even, Signed, Saturate, Fractional (to Accumulator)
 *      .1059
 */
void ppc_opc_evmhessfa(e500_core_t *current_core)
{
    int rD, rA, rB;
    int32_t temph, movh, movl;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = ((int)(current_core->gprh[rB]) >> 16) * ((int)(current_core->gprh[rA]) >> 16);
    temph &= 0x7fffffff;
    if ((current_core->gprh[rA] >> 16 == 0x8000) & (current_core->gprh[rB] >> 16 == 0x8000))
    {
        current_core->gprh[rD] = 0x7FFFFFFF;
        movh = 1;
    }
    else
    {
        current_core->gprh[rD] = temph;
        movh = 0;
    }

    temph = ((int)(current_core->gpr[rB]) >> 16) * ((int)(current_core->gpr[rA]) >> 16);
    temph &= 0x7fffffff;
    if ((current_core->gpr[rA] >> 16 == 0x8000) & (current_core->gpr[rB] >> 16 == 0x8000))
    {
        current_core->gpr[rD] = 0x7FFFFFFF;
        movl = 1;
    }
    else
    {
        current_core->gpr[rD] = temph;
        movl = 0;
    }
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->spefscr.OVH = movh;
    current_core->spefscr.OV = movl;
    current_core->spefscr.SOVH |= movh;
    current_core->spefscr.SOV |= movl;
}

/*
 *      evmhessfaaw           Vector Multiply Half Words, Even, Signed, Saturate, Fractional and Accumulate into Words
 *      .1283
 */
void ppc_opc_evmhessfaaw(e500_core_t *current_core)
{
    int rD, rA, rB;
    int temp_31;
    int64_t temp;
    int32_t temph, movh, movl, ovh, ovl;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = ((int)(current_core->gprh[rB]) >> 16) * ((int)(current_core->gprh[rA]) >> 16);
    temph &= 0x7fffffff;
    if ((current_core->gprh[rA] >> 16 == 0x8000) & (current_core->gprh[rB] >> 16 == 0x8000))
    {
        temph = 0x7FFFFFFF;
        movh = 1;
    }
    else
    {
        movh = 0;
    }
    temp = (current_core->acc >> 32) + temph;
    ovh = (((temp << 31) >> 63) & 0x1) ^ (((temp << 32) >> 63) & 0x1);
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gprh[rD] = SATURATE(ovh, temp_31, 0x80000000, 0x7FFFFFFF, temp);

    temph = ((int)(current_core->gpr[rB]) >> 16) * ((int)(current_core->gpr[rA]) >> 16);
    temph &= 0x7fffffff;
    if ((current_core->gpr[rA] >> 16 == 0x8000) & (current_core->gpr[rB] >> 16 == 0x8000))
    {
        temph = 0x7FFFFFFF;
        movl = 1;
    }
    else
    {
        movl = 0;
    }

    temp = ((current_core->acc << 32) >> 32) + temph;
    ovl = (((temp << 31) >> 63) & 0x1) ^ (((temp << 32) >> 63) & 0x1);
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gpr[rD] = SATURATE(ovl, temp_31, 0x80000000, 0x7FFFFFFF, temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->spefscr.OVH = movh;
    current_core->spefscr.OV = movl;
    current_core->spefscr.SOVH |= ovh | movh;
    current_core->spefscr.SOV |= ovl | movl;
}

/*
 *      evmhessfanw           Vector Multiply Half Words, Even, Signed, Saturate, Fractional and Accumulate Negative into Words
 *      .1411
 */
void ppc_opc_evmhessfanw(e500_core_t *current_core)
{
    int rD, rA, rB;
    int temp_31;
    int64_t temp;
    int32_t temph, movh, movl, ovh, ovl;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = ((int)(current_core->gprh[rB]) >> 16) * ((int)(current_core->gprh[rA]) >> 16);
    temph &= 0x7fffffff;
    if ((current_core->gprh[rA] >> 16 == 0x8000) & (current_core->gprh[rB] >> 16 == 0x8000))
    {
        temph = 0x7FFFFFFF;
        movh = 1;
    }
    else
    {
        movh = 0;
    }
    temp = (current_core->acc >> 32) - temph;
    ovh = (((temp << 31) >> 63) & 0x1) ^ (((temp << 32) >> 63) & 0x1);
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gprh[rD] = SATURATE(ovh, temp_31, 0x80000000, 0x7FFFFFFF, temp);

    temph = ((int)(current_core->gpr[rB]) >> 16) * ((int)(current_core->gpr[rA]) >> 16);
    temph &= 0x7fffffff;
    if ((current_core->gpr[rA] >> 16 == 0x8000) & (current_core->gpr[rB] >> 16 == 0x8000))
    {
        temph = 0x7FFFFFFF;
        movl = 1;
    }
    else
    {
        movl = 0;
    }

    temp = ((current_core->acc << 32) >> 32) - temph;
    ovl = (((temp << 31) >> 63) & 0x1) ^ (((temp << 32) >> 63) & 0x1);
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gpr[rD] = SATURATE(ovl, temp_31, 0x80000000, 0x7FFFFFFF, temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->spefscr.OVH = movh;
    current_core->spefscr.OV = movl;
    current_core->spefscr.SOVH |= ovh | movh;
    current_core->spefscr.SOV |= ovl | movl;
}

/*
 *      evmhessiaaw           Vector Multiply Half Words, Even, Signed, Saturate, Integer and Accumulate into Words
 *      .1281
 */
void ppc_opc_evmhessiaaw(e500_core_t *current_core)
{
    int rD, rA, rB;
    int temp_31;
    int64_t temp;
    int32_t temph, ovh, ovl;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = ((int)(current_core->gprh[rB]) >> 16) * ((int)(current_core->gprh[rA]) >> 16);
    temp = (current_core->acc >> 32) + temph;
    ovh = (((temp << 31) >> 63) & 0x1) ^ (((temp << 32) >> 63) & 0x1);
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gprh[rD] = SATURATE(ovh, temp_31, 0x80000000, 0x7FFFFFFF, temp);

    temph = ((int)(current_core->gpr[rB]) >> 16) * ((int)(current_core->gpr[rA]) >> 16);
    temp = ((current_core->acc << 32) >> 32) + temph;
    ovl = (((temp << 31) >> 63) & 0x1) ^ (((temp << 32) >> 63) & 0x1);
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gpr[rD] = SATURATE(ovl, temp_31, 0x80000000, 0x7FFFFFFF, temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->spefscr.OVH = ovh;
    current_core->spefscr.OV = ovl;
    current_core->spefscr.SOVH |= ovh;
    current_core->spefscr.SOV |= ovl;
}

/*
 *      evmhessianw           Vector Multiply Half Words, Even, Signed, Saturate, Integer and Accumulate Negative into Words
 *      .1409
 */
void ppc_opc_evmhessianw(e500_core_t *current_core)
{
    int rD, rA, rB;
    int temp_31;
    int64_t temp;
    int32_t temph, ovh, ovl;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = ((int)(current_core->gprh[rB]) >> 16) * ((int)(current_core->gprh[rA]) >> 16);
    temp = (current_core->acc >> 32) - temph;
    ovh = (((temp << 31) >> 63) & 0x1) ^ (((temp << 32) >> 63) & 0x1);
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gprh[rD] = SATURATE(ovh, temp_31, 0x80000000, 0x7FFFFFFF, temp);

    temph = ((int)(current_core->gpr[rB]) >> 16) * ((int)(current_core->gpr[rA]) >> 16);
    temp = ((current_core->acc << 32) >> 32) - temph;
    ovl = (((temp << 31) >> 63) & 0x1) ^ (((temp << 32) >> 63) & 0x1);
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gpr[rD] = SATURATE(ovl, temp_31, 0x80000000, 0x7FFFFFFF, temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->spefscr.OVH = ovh;
    current_core->spefscr.OV = ovl;
    current_core->spefscr.SOVH |= ovh;
    current_core->spefscr.SOV |= ovl;
}

/*
 *      evmheumi           Vector Multiply Half Words, Even, Unsigned, Modulo, Integer (to Accumulator)
 *      .1032
 */
void ppc_opc_evmheumi(e500_core_t *current_core)
{
    int rD, rA, rB;
    uint32_t temph, templ;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    templ = (current_core->gpr[rB] >> 16) * (current_core->gpr[rA] >> 16);
    temph = (current_core->gprh[rB] >> 16) * (current_core->gprh[rA] >> 16);

    current_core->gprh[rD] = temph;
    current_core->gpr[rD] = templ;
}

/*
 *      evmheumia           Vector Multiply Half Words, Even, Unsigned, Modulo, Integer (to Accumulator)
 *      .1064
 */
void ppc_opc_evmheumia(e500_core_t *current_core)
{
    int rD, rA, rB;
    uint32_t temph, templ;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    templ = (current_core->gpr[rB] >> 16) * (current_core->gpr[rA] >> 16);
    temph = (current_core->gprh[rB] >> 16) * (current_core->gprh[rA] >> 16);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->gprh[rD] = temph;
    current_core->gpr[rD] = templ;
}

/*
 *      evmheumiaaw           Vector Multiply Half Words, Even, Unsigned, Modulo, Integer and Accumulate into Words
 *      .1288
 */
void ppc_opc_evmheumiaaw(e500_core_t *current_core)
{
    int rD, rA, rB;
    uint32_t temph;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = (current_core->gprh[rB] >> 16) * (current_core->gprh[rA] >> 16);
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) + temph;

    temph = (current_core->gpr[rB] >> 16) * (current_core->gpr[rA] >> 16);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) + temph;

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *      evmheumianw           Vector Multiply Half Words, Even, Unsigned, Modulo, Integer and Accumulate Negative into Words
 *      .1416
 */
void ppc_opc_evmheumianw(e500_core_t *current_core)
{
    int rD, rA, rB;
    uint32_t temph;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = (current_core->gprh[rB] >> 16) * (current_core->gprh[rA] >> 16);
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) - temph;
    temph = (current_core->gpr[rB] >> 16) * (current_core->gpr[rA] >> 16);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) - temph;
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *      evmheusiaaw           Vector Multiply Half Words, Even, Unsigned, Saturate, Integer and Accumulate into Words
 *      .1280
 */
void ppc_opc_evmheusiaaw(e500_core_t *current_core)
{
    int rD, rA, rB;
    int temp_31;
    uint64_t temp;
    uint32_t temph, ovh, ovl;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = (current_core->gprh[rB] >> 16) * (current_core->gprh[rA] >> 16);
    temp = ((current_core->acc >> 32) & 0xFFFFffff) + temph;
    ovh = (((temp << 31) >> 63) & 0x1);
    current_core->gprh[rD] = SATURATE(ovh, 0, 0xFFFFFFFF, 0xFFFFFFFF, temp);

    temph = (current_core->gpr[rB] >> 16) * (current_core->gpr[rA] >> 16);
    temp = (current_core->acc & 0xFFFFffff) + temph;
    ovl = (((temp << 31) >> 63) & 0x1);
    current_core->gpr[rD] = SATURATE(ovl, 0, 0xFFFFFFFF, 0xFFFFFFFF, temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->spefscr.OVH = ovh;
    current_core->spefscr.OV = ovl;
    current_core->spefscr.SOVH |= ovh;
    current_core->spefscr.SOV |= ovl;
}

/*
 *      evmheusianw           Vector Multiply Half Words, Even, Unsigned, Saturate, Integer and Accumulate Negative into Words
 *      .1408
 */
void ppc_opc_evmheusianw(e500_core_t *current_core)
{
    int rD, rA, rB;
    int temp_31;
    uint64_t temp;
    uint32_t temph, ovh, ovl;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = (current_core->gprh[rB] >> 16) * (current_core->gprh[rA] >> 16);
    temp = ((current_core->acc >> 32) & 0xFFFFffff) - temph;
    ovh = ((temp << 31 >> 63) & 0x1);
    current_core->gprh[rD] = SATURATE(ovh, 0, 0x00000000, 0x00000000, temp);

    temph = (current_core->gpr[rB] >> 16) * (current_core->gpr[rA] >> 16);
    temp = (current_core->acc & 0xFFFFffff) - temph;
    ovl = ((temp << 31 >> 63) & 0x1);
    current_core->gpr[rD] = SATURATE(ovl, 0, 0x00000000, 0x00000000, temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->spefscr.OVH = ovh;
    current_core->spefscr.OV = ovl;
    current_core->spefscr.SOVH |= ovh;
    current_core->spefscr.SOV |= ovl;
}

/*
 *      evmhogsmfaa           Vector Multiply Half Words, Odd, Guarded, Signed, Modulo, Fractional and Accumulate
 *      .1327
 */
void ppc_opc_evmhogsmfaa(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp;
    int32_t temph;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    temph = ((int)(current_core->gpr[rB]) << 16 >> 16) * ((int)(current_core->gpr[rA]) << 16 >> 16);
    temph &= 0x7fffffff;
    temp = ((int64_t)(temph) << 32 >> 32);
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) + (uint32_t)(temp >> 32);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) + (uint32_t)(temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *      evmhogsmfan           Vector Multiply Half Words, Odd, Guarded, Signed, Modulo, Fractional and Accumulate Negative
 *      .1455
 */
void ppc_opc_evmhogsmfan(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp;
    int32_t temph;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    temph = (((int)(current_core->gpr[rB]) << 16) >> 16) * (((int)(current_core->gpr[rA]) << 16) >> 16);
    temph &= 0x7fffffff;
    temp = ((int64_t)(temph) << 32 >> 32);
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) - (uint32_t)(temp >> 32);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) - (uint32_t)(temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *      evmhogsmiaa           Vector Multiply Half Words, Odd, Guarded, Signed, Modulo, Integer, and Accumulate
 *      .1325
 */
void ppc_opc_evmhogsmiaa(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp;
    int32_t temph;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = ((int)(current_core->gpr[rB]) << 16 >> 16) * ((int)(current_core->gpr[rA]) << 16 >> 16);
    temp = ((int64_t)(temph) << 32 >> 32);
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) + (uint32_t)(temp >> 32);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) + (uint32_t)(temp);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *      evmhogsmian           Vector Multiply Half Words, Odd, Guarded, Signed, Modulo, Integer and Accumulate Negative
 *      .1453
 */
void ppc_opc_evmhogsmian(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp;
    int32_t temph;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = ((int)(current_core->gpr[rB]) << 16 >> 16) * ((int)(current_core->gpr[rA]) << 16 >> 16);
    temp = ((int64_t)(temph) << 32 >> 32);
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) - (uint32_t)(temp >> 32);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) - (uint32_t)(temp);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *      evmhogumiaa           Vector Multiply Half Words, Odd, Guarded, Unsigned, Modulo, Integer and Accumulate
 *      .1324
 */
void ppc_opc_evmhogumiaa(e500_core_t *current_core)
{
    int rD, rA, rB;
    uint64_t temp;
    uint32_t temph;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = (current_core->gpr[rB] & 0xFFFF) * (current_core->gpr[rA] & 0xFFFF);
    temp = (uint64_t)(temph);
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) + (uint32_t)(temp >> 32);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) + (uint32_t)(temp);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *      evmhogumian           Vector Multiply Half Words, Odd, Guarded, Unsigned, Modulo, Integer and Accumulate Negative
 *      .1452
 */
void ppc_opc_evmhogumian(e500_core_t *current_core)
{
    int rD, rA, rB;
    uint64_t temp;
    uint32_t temph;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = (current_core->gpr[rB] & 0xFFFF) * (current_core->gpr[rA] & 0xFFFF);
    temp = (uint64_t)(temph);
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) - (uint32_t)(temp >> 32);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) - (uint32_t)(temp);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *     evmhosmf           Vector Multiply Half Words, Odd, Signed, Modulo, Fractional (to Accumulator)
 *      .1039
 */
void ppc_opc_evmhosmf(e500_core_t *current_core)
{
    int rD, rA, rB;
    int32_t temph, templ;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    templ = ((int)(current_core->gpr[rB]) << 16 >> 16) * ((int)(current_core->gpr[rA]) << 16 >> 16);
    temph = ((int)(current_core->gprh[rB]) << 16 >> 16) * ((int)(current_core->gprh[rA]) << 16 >> 16);
    temph &= 0x7fffffff;
    templ &= 0x7fffffff;

    current_core->gprh[rD] = (uint32_t)(temph);
    current_core->gpr[rD] = (uint32_t)(templ);
}

/*
 *     evmhosmfa           Vector Multiply Half Words, Odd, Signed, Modulo, Fractional (to Accumulator)
 *      .1071
 */
void ppc_opc_evmhosmfa(e500_core_t *current_core)
{
    int rD, rA, rB;
    int32_t temph, templ;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    templ = ((int)(current_core->gpr[rB]) << 16 >> 16) * ((int)(current_core->gpr[rA]) << 16 >> 16);
    temph = ((int)(current_core->gprh[rB]) << 16 >> 16) * ((int)(current_core->gprh[rA]) << 16 >> 16);
    temph &= 0x7fffffff;
    templ &= 0x7fffffff;

    current_core->gprh[rD] = (uint32_t)(temph);
    current_core->gpr[rD] = (uint32_t)(templ);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *     evmhosmfaaw           Vector Multiply Half Words, Odd, Signed, Modulo, Fractional and Accumulate into Words
 *      .1295
 */
void ppc_opc_evmhosmfaaw(e500_core_t *current_core)
{
    int rD, rA, rB;
    int32_t temph;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = ((int)(current_core->gprh[rB]) << 16 >> 16) * ((int)(current_core->gprh[rA]) << 16 >> 16);
    temph &= 0x7fffffff;
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) + (uint32_t)(temph);

    temph = ((int)(current_core->gpr[rB]) << 16 >> 16) * ((int)(current_core->gpr[rA]) << 16 >> 16);
    temph &= 0x7fffffff;
    current_core->gpr[rD] = (uint32_t)(current_core->acc) + (uint32_t)(temph);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *     evmhosmfanw           Vector Multiply Half Words, Odd, Signed, Modulo, Fractional and Accumulate Negative into Words
 *      .1423
 */
void ppc_opc_evmhosmfanw(e500_core_t *current_core)
{
    int rD, rA, rB;
    int32_t temph;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = ((int)(current_core->gprh[rB]) << 16 >> 16) * ((int)(current_core->gprh[rA]) << 16 >> 16);
    temph &= 0x7fffffff;
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) - (uint32_t)(temph);

    temph = ((int)(current_core->gpr[rB]) << 16 >> 16) * ((int)(current_core->gpr[rA]) << 16 >> 16);
    temph &= 0x7fffffff;
    current_core->gpr[rD] = (uint32_t)(current_core->acc) - (uint32_t)(temph);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *     evmhosmi           Vector Multiply Half Words, Odd, Signed, Modulo, Fractional and Accumulate Negative into Words
 *      .1037
 */
void ppc_opc_evmhosmi(e500_core_t *current_core)
{
    int rD, rA, rB;
    int32_t temph, templ;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    templ = ((int)(current_core->gpr[rB]) << 16 >> 16) * ((int)(current_core->gpr[rA]) << 16 >> 16);
    temph = ((int)(current_core->gprh[rB]) << 16 >> 16) * ((int)(current_core->gprh[rA]) << 16 >> 16);

    current_core->gprh[rD] = (uint32_t)(temph);
    current_core->gpr[rD] = (uint32_t)(templ);
}

/*
 *     evmhosmia           Vector Multiply Half Words, Odd, Signed, Modulo, Integer (to Accumulator)
 *      .1069
 */
void ppc_opc_evmhosmia(e500_core_t *current_core)
{
    int rD, rA, rB;
    int32_t temph, templ;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    templ = ((int)(current_core->gpr[rB]) << 16 >> 16) * ((int)(current_core->gpr[rA]) << 16 >> 16);
    temph = ((int)(current_core->gprh[rB]) << 16 >> 16) * ((int)(current_core->gprh[rA]) << 16 >> 16);

    current_core->gprh[rD] = (uint32_t)(temph);
    current_core->gpr[rD] = (uint32_t)(templ);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *     evmhosmiaaw           Vector Multiply Half Words, Odd, Signed, Modulo, Integer and Accumulate into Words
 *      .1293
 */
void ppc_opc_evmhosmiaaw(e500_core_t *current_core)
{
    int rD, rA, rB;
    int32_t temph;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = ((int)(current_core->gprh[rB]) << 16 >> 16) * ((int)(current_core->gprh[rA]) << 16 >> 16);
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) + (uint32_t)(temph);

    temph = ((int)(current_core->gpr[rB]) << 16 >> 16) * ((int)(current_core->gpr[rA]) << 16 >> 16);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) + (uint32_t)(temph);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *     evmhosmianw           Vector Multiply Half Words, Odd, Signed, Modulo, Integer and Accumulate Negative into Words
 *      .1421
 */
void ppc_opc_evmhosmianw(e500_core_t *current_core)
{
    int rD, rA, rB;
    int32_t temph;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = ((int)(current_core->gprh[rB]) << 16 >> 16) * ((int)(current_core->gprh[rA]) << 16 >> 16);
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) - (uint32_t)(temph);

    temph = ((int)(current_core->gpr[rB]) << 16 >> 16) * ((int)(current_core->gpr[rA]) << 16 >> 16);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) - (uint32_t)(temph);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *     evmhossf          Vector Multiply Half Words, Odd, Signed, Saturate, Fractional (to Accumulator)
 *      .1031
 */
void ppc_opc_evmhossf(e500_core_t *current_core)
{
    int rD, rA, rB;
    int32_t temph, movh, movl;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = ((int)(current_core->gprh[rB]) << 16 >> 16) * ((int)(current_core->gprh[rA]) << 16 >> 16);
    temph &= 0x7fffffff;
    if (((current_core->gprh[rA] & 0xFFFF) == 0x8000) & ((current_core->gprh[rB] & 0xFFFF) == 0x8000))
    {
        current_core->gprh[rD] = 0x7FFFFFFF;
        movh = 1;
    }
    else
    {
        current_core->gprh[rD] = temph;
        movh = 0;
    }

    temph = ((int)(current_core->gpr[rB]) << 16 >> 16) * ((int)(current_core->gpr[rA]) << 16 >> 16);
    temph &= 0x7fffffff;
    if (((current_core->gpr[rA] & 0xFFFF) == 0x8000) & ((current_core->gpr[rB] & 0xFFFF) == 0x8000))
    {
        current_core->gpr[rD] = 0x7FFFFFFF;
        movl = 1;
    }
    else
    {
        current_core->gpr[rD] = temph;
        movl = 0;
    }

    current_core->spefscr.OVH = movh;
    current_core->spefscr.OV = movl;
    current_core->spefscr.SOVH |= movh;
    current_core->spefscr.SOV |= movl;
}

/*
 *     evmhossfa          Vector Multiply Half Words, Odd, Signed, Saturate, Fractional (to Accumulator)
 *      .1063
 */
void ppc_opc_evmhossfa(e500_core_t *current_core)
{
    int rD, rA, rB;
    int32_t temph, movh, movl;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = ((int)(current_core->gprh[rB]) << 16 >> 16) * ((int)(current_core->gprh[rA]) << 16 >> 16);
    temph &= 0x7fffffff;
    if (((current_core->gprh[rA] & 0xFFFF) == 0x8000) & ((current_core->gprh[rB] & 0xFFFF) == 0x8000))
    {
        current_core->gprh[rD] = 0x7FFFFFFF;
        movh = 1;
    }
    else
    {
        current_core->gprh[rD] = temph;
        movh = 0;
    }

    temph = ((int)(current_core->gpr[rB]) << 16 >> 16) * ((int)(current_core->gpr[rA]) << 16 >> 16);
    temph &= 0x7fffffff;
    if (((current_core->gpr[rA] & 0xFFFF) == 0x8000) & ((current_core->gpr[rB] & 0xFFFF) == 0x8000))
    {
        current_core->gpr[rD] = 0x7FFFFFFF;
        movl = 1;
    }
    else
    {
        current_core->gpr[rD] = temph;
        movl = 0;
    }

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->spefscr.OVH = movh;
    current_core->spefscr.OV = movl;
    current_core->spefscr.SOVH |= movh;
    current_core->spefscr.SOV |= movl;
}

/*
 *     evmhossfaaw          Vector Multiply Half Words, Odd, Signed, Saturate, Fractional and Accumulate into Words
 *      .1287
 */
void ppc_opc_evmhossfaaw(e500_core_t *current_core)
{
    int rD, rA, rB;
    int temp_31;
    int64_t temp;
    int32_t temph, movh, movl, ovh, ovl;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = ((int)(current_core->gprh[rB]) << 16 >> 16) * ((int)(current_core->gprh[rA]) << 16 >> 16);
    temph &= 0x7fffffff;
    if (((current_core->gprh[rA] & 0xFFFF) == 0x8000) & ((current_core->gprh[rB] & 0xFFFF) == 0x8000))
    {
        temph = 0x7FFFFFFF;
        movh = 1;
    }
    else
    {
        movh = 0;
    }
    temp = (current_core->acc >> 32) + temph;
    ovh = (((temp << 31) >> 63) & 0x1) ^ (((temp << 32) >> 63) & 0x1);
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gprh[rD] = SATURATE(ovh, temp_31, 0x80000000, 0x7FFFFFFF, temp);

    temph = ((int)(current_core->gpr[rB]) << 16 >> 16) * ((int)(current_core->gpr[rA]) << 16 >> 16);
    temph &= 0x7fffffff;
    if (((current_core->gpr[rA] & 0xFFFF) == 0x8000) & ((current_core->gpr[rB] & 0xFFFF) == 0x8000))
    {
        temph = 0x7FFFFFFF;
        movl = 1;
    }
    else
    {
        movl = 0;
    }

    temp = ((current_core->acc << 32) >> 32) + temph;
    ovl = (((temp << 31) >> 63) & 0x1) ^ (((temp << 32) >> 63) & 0x1);
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gpr[rD] = SATURATE(ovl, temp_31, 0x80000000, 0x7FFFFFFF, temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->spefscr.OVH = movh;
    current_core->spefscr.OV = movl;
    current_core->spefscr.SOVH |= ovh | movh;
    current_core->spefscr.SOV |= ovl | movl;
}

/*
 *     evmhossfanw          Vector Multiply Half Words, Odd, Signed, Saturate, Fractional and Accumulate Negative into Words
 *      .1415
 */
void ppc_opc_evmhossfanw(e500_core_t *current_core)
{
    int rD, rA, rB;
    int temp_31;
    int64_t temp;
    int32_t temph, movh, movl, ovh, ovl;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = ((int)(current_core->gprh[rB]) << 16 >> 16) * ((int)(current_core->gprh[rA]) << 16 >> 16);
    temph &= 0x7fffffff;
    if (((current_core->gprh[rA] & 0xFFFF) == 0x8000) & ((current_core->gprh[rB] & 0xFFFF) == 0x8000))
    {
        temph = 0x7FFFFFFF;
        movh = 1;
    }
    else
    {
        movh = 0;
    }
    temp = (current_core->acc >> 32) - temph;
    ovh = (((temp << 31) >> 63) & 0x1) ^ (((temp << 32) >> 63) & 0x1);
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gprh[rD] = SATURATE(ovh, temp_31, 0x80000000, 0x7FFFFFFF, temp);

    temph = ((int)(current_core->gpr[rB]) << 16 >> 16) * ((int)(current_core->gpr[rA]) << 16 >> 16);
    temph &= 0x7fffffff;
    if (((current_core->gpr[rA] & 0xFFFF) == 0x8000) & ((current_core->gpr[rB] & 0xFFFF) == 0x8000))
    {
        temph = 0x7FFFFFFF;
        movl = 1;
    }
    else
    {
        movl = 0;
    }

    temp = ((current_core->acc << 32) >> 32) - temph;
    ovl = (((temp << 31) >> 63) & 0x1) ^ (((temp << 32) >> 63) & 0x1);
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gpr[rD] = SATURATE(ovl, temp_31, 0x80000000, 0x7FFFFFFF, temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->spefscr.OVH = movh;
    current_core->spefscr.OV = movl;
    current_core->spefscr.SOVH |= ovh | movh;
    current_core->spefscr.SOV |= ovl | movl;
}

/*
 *     evmhossiaaw          Vector Multiply Half Words, Odd, Signed, Saturate, Integer and Accumulate into Words
 *      .1285
 */
void ppc_opc_evmhossiaaw(e500_core_t *current_core)
{
    int rD, rA, rB;
    int temp_31;
    int64_t temp;
    int32_t temph, ovh, ovl;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = ((int)(current_core->gprh[rB]) << 16 >> 16) * ((int)(current_core->gprh[rA]) << 16 >> 16);
    temp = (current_core->acc >> 32) + temph;
    ovh = (((temp << 31) >> 63) & 0x1) ^ (((temp << 32) >> 63) & 0x1);
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gprh[rD] = SATURATE(ovh, temp_31, 0x80000000, 0x7FFFFFFF, temp);

    temph = ((int)(current_core->gpr[rB]) << 16 >> 16) * ((int)(current_core->gpr[rA]) << 16 >> 16);
    temp = ((current_core->acc << 32) >> 32) + temph;
    ovl = (((temp << 31) >> 63) & 0x1) ^ (((temp << 32) >> 63) & 0x1);
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gpr[rD] = SATURATE(ovl, temp_31, 0x80000000, 0x7FFFFFFF, temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->spefscr.OVH = ovh;
    current_core->spefscr.OV = ovl;
    current_core->spefscr.SOVH |= ovh;
    current_core->spefscr.SOV |= ovl;
}

/*
 *    evmhossianw          Vector Multiply Half Words, Odd, Signed, Saturate, Integer and Accumulate Negative into Words
 *      .1413
 */
void ppc_opc_evmhossianw(e500_core_t *current_core)
{
    int rD, rA, rB;
    int temp_31;
    int64_t temp;
    int32_t temph, ovh, ovl;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = ((int)(current_core->gprh[rB]) << 16 >> 16) * ((int)(current_core->gprh[rA]) << 16 >> 16);
    temp = (current_core->acc >> 32) - temph;
    ovh = (((temp << 31) >> 63) & 0x1) ^ (((temp << 32) >> 63) & 0x1);
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gprh[rD] = SATURATE(ovh, temp_31, 0x80000000, 0x7FFFFFFF, temp);

    temph = ((int)(current_core->gpr[rB]) << 16 >> 16) * ((int)(current_core->gpr[rA]) << 16 >> 16);
    temp = ((current_core->acc << 32) >> 32) - temph;
    ovl = (((temp << 31) >> 63) & 0x1) ^ (((temp << 32) >> 63) & 0x1);
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gpr[rD] = SATURATE(ovl, temp_31, 0x80000000, 0x7FFFFFFF, temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->spefscr.OVH = ovh;
    current_core->spefscr.OV = ovl;
    current_core->spefscr.SOVH |= ovh;
    current_core->spefscr.SOV |= ovl;
}

/*
 *    evmhoumi          Vector Multiply Half Words, Odd, Unsigned, Modulo, Integer (to Accumulator)
 *      .1036
 */
void ppc_opc_evmhoumi(e500_core_t *current_core)
{
    int rD, rA, rB;
    uint32_t temph, templ;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    templ = (current_core->gpr[rB] & 0xFFFF) * (current_core->gpr[rA] & 0xFFFF);
    temph = (current_core->gprh[rB] & 0xFFFF) * (current_core->gprh[rA] & 0xFFFF);

    current_core->gprh[rD] = temph;
    current_core->gpr[rD] = templ;
}

/*
 *    evmhoumia          Vector Multiply Half Words, Odd, Unsigned, Modulo, Integer (to Accumulator)
 *      .1068
 */
void ppc_opc_evmhoumia(e500_core_t *current_core)
{
    int rD, rA, rB;
    uint32_t temph, templ;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    templ = (current_core->gpr[rB] & 0xFFFF) * (current_core->gpr[rA] & 0xFFFF);
    temph = (current_core->gprh[rB] & 0xFFFF) * (current_core->gprh[rA] & 0xFFFF);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
    current_core->gprh[rD] = temph;
    current_core->gpr[rD] = templ;
}

/*
 *    evmhoumiaaw          Vector Multiply Half Words, Odd, Unsigned, Modulo, Integer and Accumulate into Words
 *      .1292
 */
void ppc_opc_evmhoumiaaw(e500_core_t *current_core)
{
    int rD, rA, rB;
    uint32_t temph;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = (current_core->gprh[rB] & 0xFFFF) * (current_core->gprh[rA] & 0xFFFF);
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) + temph;

    temph = (current_core->gpr[rB] & 0xFFFF) * (current_core->gpr[rA] & 0xFFFF);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) + temph;

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *    evmhoumianw          Vector Multiply Half Words, Odd, Unsigned, Modulo, Integer and Accumulate Negative into Words
 *      .1420
 */
void ppc_opc_evmhoumianw(e500_core_t *current_core)
{
    int rD, rA, rB;
    uint32_t temph;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = (current_core->gprh[rB] & 0xFFFF) * (current_core->gprh[rA] & 0xFFFF);
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) - temph;

    temph = (current_core->gpr[rB] & 0xFFFF) * (current_core->gpr[rA] & 0xFFFF);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) - temph;

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *    evmhousiaaw          Vector Multiply Half Words, Odd, Unsigned, Saturate, Integer and Accumulate into Words
 *      .1284
 */
void ppc_opc_evmhousiaaw(e500_core_t *current_core)
{
    int rD, rA, rB;
    int temp_31;
    uint64_t temp;
    uint32_t temph, ovh, ovl;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = (current_core->gprh[rB] & 0xFFFF) * (current_core->gprh[rA] & 0xFFFF);
    temp = ((current_core->acc >> 32) & 0xFFFFffff) + temph;
    ovh = (((temp << 31) >> 63) & 0x1);
    current_core->gprh[rD] = SATURATE(ovh, 0, 0xFFFFFFFF, 0xFFFFFFFF, temp);

    temph = (current_core->gpr[rB] & 0xFFFF) * (current_core->gpr[rA] & 0xFFFF);
    temp = (current_core->acc & 0xFFFFffff) + temph;
    ovl = (((temp << 31) >> 63) & 0x1);
    current_core->gpr[rD] = SATURATE(ovl, 0, 0xFFFFFFFF, 0xFFFFFFFF, temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->spefscr.OVH = ovh;
    current_core->spefscr.OV = ovl;
    current_core->spefscr.SOVH |= ovh;
    current_core->spefscr.SOV |= ovl;
}

/*
 *    evmhousianw          Vector Multiply Half Words, Odd, Unsigned, Saturate, Integer and Accumulate Negative into Words
 *      .1412
 */
void ppc_opc_evmhousianw(e500_core_t *current_core)
{
    int rD, rA, rB;
    int temp_31;
    uint64_t temp;
    uint32_t temph, ovh, ovl;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temph = (current_core->gprh[rB] & 0xFFFF) * (current_core->gprh[rA] & 0xFFFF);
    temp = ((current_core->acc >> 32) & 0xFFFFffff) - temph;
    ovh = (((temp << 31) >> 63) & 0x1);
    current_core->gprh[rD] = SATURATE(ovh, 0, 0xFFFFFFFF, 0xFFFFFFFF, temp);

    temph = (current_core->gpr[rB] & 0xFFFF) * (current_core->gpr[rA] & 0xFFFF);
    temp = (current_core->acc & 0xFFFFffff) - temph;
    ovl = (((temp << 31) >> 63) & 0x1);
    current_core->gpr[rD] = SATURATE(ovl, 0, 0xFFFFFFFF, 0xFFFFFFFF, temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->spefscr.OVH = ovh;
    current_core->spefscr.OV = ovl;
    current_core->spefscr.SOVH |= ovh;
    current_core->spefscr.SOV |= ovl;
}

/*
 *      Initialize Accumulator
 *      .1220
 */
void ppc_opc_evmra(e500_core_t *current_core)
{
    int rD, rA, rB;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    PPC_OPC_ASSERT(rD == 0 && rB == 0);

    // return data
    current_core->acc = ((int64_t)(current_core->gprh[rA]) << 32) | current_core->gpr[rA];
    current_core->gprh[rD] = current_core->gprh[rA];
    current_core->gpr[rD] = current_core->gpr[rA];
}

/*
 *    evmwhsmf          Vector Multiply Word High Signed, Modulo, Fractional (to Accumulator)
 *      .1103
 */
void ppc_opc_evmwhsmf(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temp = (int64_t)(current_core->gprh[rB]) * (int64_t)(current_core->gprh[rA]);
    temp &= 0x7fffffffffffffffLL;
    current_core->gprh[rD] = (uint32_t)(temp >> 32);

    temp = (int64_t)(current_core->gpr[rB]) * (int64_t)(current_core->gpr[rA]);
    temp &= 0x7fffffffffffffffLL;
    current_core->gpr[rD] = (uint32_t)(temp >> 32);
}

/*
 *    evmwhsmfa          Vector Multiply Word High Signed, Modulo, Fractional (to Accumulator)
 *      .1135
 */
void ppc_opc_evmwhsmfa(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temp = (int64_t)(current_core->gprh[rB]) * (int64_t)(current_core->gprh[rA]);
    temp &= 0x7fffffffffffffffLL;
    current_core->gprh[rD] = (uint32_t)(temp >> 32);

    temp = (int64_t)(current_core->gpr[rB]) * (int64_t)(current_core->gpr[rA]);
    temp &= 0x7fffffffffffffffLL;
    current_core->gpr[rD] = (uint32_t)(temp >> 32);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *    evmwhsmi          Vector Multiply Word High Signed, Modulo, Integer (to Accumulator)
 *      .1101
 */
void ppc_opc_evmwhsmi(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temp = (int64_t)(current_core->gprh[rB]) * (int64_t)(current_core->gprh[rA]);
    current_core->gprh[rD] = (uint32_t)(temp >> 32);

    temp = (int64_t)(current_core->gpr[rB]) * (int64_t)(current_core->gpr[rA]);
    current_core->gpr[rD] = (uint32_t)(temp >> 32);
}

/*
 *    evmwhsmia          Vector Multiply Word High Signed, Modulo, Integer (to Accumulator)
 *      .1133
 */
void ppc_opc_evmwhsmia(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temp = (int64_t)(current_core->gprh[rB]) * (int64_t)(current_core->gprh[rA]);
    current_core->gprh[rD] = (uint32_t)(temp >> 32);

    temp = (int64_t)(current_core->gpr[rB]) * (int64_t)(current_core->gpr[rA]);
    current_core->gpr[rD] = (uint32_t)(temp >> 32);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *    evmwhssf         Vector Multiply Word High Signed, Saturate, Fractional (to Accumulator)
 *      .1095
 */
void ppc_opc_evmwhssf(e500_core_t *current_core)
{
    int rD, rA, rB;
    int32_t movh, movl;
    int64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temp = (int64_t)(current_core->gprh[rB]) * (int64_t)(current_core->gprh[rA]);

    temp &= 0x7fffffffffffffffLL;
    if ((current_core->gprh[rA] == 0x80000000) & (current_core->gprh[rB] == 0x80000000))
    {
        current_core->gprh[rD] = 0x7FFFFFFF;
        movh = 1;
    }
    else
    {
        current_core->gprh[rD] = temp >> 32;
        movh = 0;
    }

    temp = (int64_t)(current_core->gpr[rB]) * (int64_t)(current_core->gpr[rA]);
    temp &= 0x7fffffffffffffffLL;
    if ((current_core->gpr[rA] == 0x80000000) & (current_core->gpr[rB] == 0x80000000))
    {
        current_core->gpr[rD] = 0x7FFFFFFF;
        movl = 1;
    }
    else
    {
        current_core->gpr[rD] = temp >> 32;
        movl = 0;
    }

    current_core->spefscr.OVH = movh;
    current_core->spefscr.OV = movl;
    current_core->spefscr.SOVH |= movh;
    current_core->spefscr.SOV |= movl;
}

/*
 *    evmwhssfa         Vector Multiply Word High Signed, Saturate, Fractional (to Accumulator)
 *      .1127
 */
void ppc_opc_evmwhssfa(e500_core_t *current_core)
{
    int rD, rA, rB;
    int32_t movh, movl;
    int64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temp = (int64_t)(current_core->gprh[rB]) * (int64_t)(current_core->gprh[rA]);
    temp &= 0x7fffffffffffffffLL;
    if ((current_core->gprh[rA] == 0x80000000) & (current_core->gprh[rB] == 0x80000000))
    {
        current_core->gprh[rD] = 0x7FFFFFFF;
        movh = 1;
    }
    else
    {
        current_core->gprh[rD] = temp >> 32;
        movh = 0;
    }

    temp = (int64_t)(current_core->gpr[rB]) * (int64_t)(current_core->gpr[rA]);
    temp &= 0x7fffffffffffffffLL;
    if ((current_core->gpr[rA] == 0x80000000) & (current_core->gpr[rB] == 0x80000000))
    {
        current_core->gpr[rD] = 0x7FFFFFFF;
        movl = 1;
    }
    else
    {
        current_core->gpr[rD] = temp >> 32;
        movl = 0;
    }
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->spefscr.OVH = movh;
    current_core->spefscr.OV = movl;
    current_core->spefscr.SOVH |= movh;
    current_core->spefscr.SOV |= movl;
}

/*
 *    evmwhumi         Vector Multiply Word High Unsigned, Modulo, Integer (to Accumulator)
 *      .1100
 */
void ppc_opc_evmwhumi(e500_core_t *current_core)
{
    int rD, rA, rB;
    uint64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temp = (uint64_t)(current_core->gprh[rB]) * (uint64_t)(current_core->gprh[rA]);
    current_core->gprh[rD] = (uint32_t)(temp >> 32);

    temp = (uint64_t)(current_core->gpr[rB]) * (uint64_t)(current_core->gpr[rA]);
    current_core->gpr[rD] = (uint32_t)(temp >> 32);
}

/*
 *    evmwhumia         Vector Multiply Word High Unsigned, Modulo, Integer (to Accumulator)
 *      .1132
 */
void ppc_opc_evmwhumia(e500_core_t *current_core)
{
    int rD, rA, rB;
    uint64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temp = (uint64_t)(current_core->gprh[rB]) * (uint64_t)(current_core->gprh[rA]);
    current_core->gprh[rD] = (uint32_t)(temp >> 32);

    temp = (uint64_t)(current_core->gpr[rB]) * (uint64_t)(current_core->gpr[rA]);
    current_core->gpr[rD] = (uint32_t)(temp >> 32);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *    evmwlsmiaaw         Vector Multiply Word Low Signed, Modulo, Integer and Accumulate in Words
 *      .1353
 */
void ppc_opc_evmwlsmiaaw(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temp = (int64_t)(current_core->gprh[rB]) * (int64_t)(current_core->gprh[rA]);
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) + (uint32_t)(temp);

    temp = (int64_t)(current_core->gpr[rB]) * (int64_t)(current_core->gpr[rA]);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) + (uint32_t)(temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *    evmwlsmianw         Vector Multiply Word Low Signed, Modulo, Integer and Accumulate Negative in Words
 *      .1481
 */
void ppc_opc_evmwlsmianw(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temp = (int64_t)(current_core->gprh[rB]) * (int64_t)(current_core->gprh[rA]);
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) - (uint32_t)(temp);

    temp = (int64_t)(current_core->gpr[rB]) * (int64_t)(current_core->gpr[rA]);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) - (uint32_t)(temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *    evmwlssiaaw         Vector Multiply Word Low Signed, Saturate, Integer and Accumulate in Words
 *      .1345
 */
void ppc_opc_evmwlssiaaw(e500_core_t *current_core)
{
    int rD, rA, rB;
    int temp_31;
    int64_t temp;
    int32_t ovh, ovl;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temp = (int64_t)(current_core->gprh[rB]) * (int64_t)(current_core->gprh[rA]);
    temp = (current_core->acc >> 32) + (temp << 32 >> 32);

    ovh = (((temp << 31) >> 63) & 0x1) ^ (((temp << 32) >> 63) & 0x1);
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gprh[rD] = SATURATE(ovh, temp_31, 0x80000000, 0x7FFFFFFF, temp);

    temp = (int64_t)(current_core->gpr[rB]) * (int64_t)(current_core->gpr[rA]);
    temp = (current_core->acc << 32 >> 32) + (temp << 32 >> 32);

    ovl = (((temp << 31) >> 63) & 0x1) ^ (((temp << 32) >> 63) & 0x1);
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gpr[rD] = SATURATE(ovl, temp_31, 0x80000000, 0x7FFFFFFF, temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->spefscr.OVH = ovh;
    current_core->spefscr.OV = ovl;
    current_core->spefscr.SOVH |= ovh;
    current_core->spefscr.SOV |= ovl;
}

/*
 *    evmwlssianw         Vector Multiply Word Low Signed, Saturate, Integer and Accumulate Negative in Words
 *      .1473
 */
void ppc_opc_evmwlssianw(e500_core_t *current_core)
{
    int rD, rA, rB;
    int temp_31;
    int64_t temp;
    int32_t ovh, ovl;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temp = (int64_t)(current_core->gprh[rB]) * (int64_t)(current_core->gprh[rA]);
    temp = (current_core->acc >> 32) - (temp << 32 >> 32);

    ovh = (((temp << 31) >> 63) & 0x1) ^ (((temp << 32) >> 63) & 0x1);
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gprh[rD] = SATURATE(ovh, temp_31, 0x80000000, 0x7FFFFFFF, temp);

    temp = (int64_t)(current_core->gpr[rB]) * (int64_t)(current_core->gpr[rA]);
    temp = (current_core->acc << 32 >> 32) - (temp << 32 >> 32);

    ovl = (((temp << 31) >> 63) & 0x1) ^ (((temp << 32) >> 63) & 0x1);
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gpr[rD] = SATURATE(ovl, temp_31, 0x80000000, 0x7FFFFFFF, temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->spefscr.OVH = ovh;
    current_core->spefscr.OV = ovl;
    current_core->spefscr.SOVH |= ovh;
    current_core->spefscr.SOV |= ovl;
}

/*
 *    evmwlumia         Vector Multiply Word Low Unsigned, Modulo, Integer
 *      .1128
 */
void ppc_opc_evmwlumia(e500_core_t *current_core)
{
    int rD, rA, rB;
    uint64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temp = (uint64_t)(current_core->gprh[rB]) * (uint64_t)(current_core->gprh[rA]);
    current_core->gprh[rD] = temp;

    temp = (uint64_t)(current_core->gpr[rB]) * (uint64_t)(current_core->gpr[rA]);
    current_core->gpr[rD] = temp;

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *    evmwlumiaaw         ector Multiply Word Low Unsigned, Modulo, Integer and Accumulate in Words
 *      .1352
 */
void ppc_opc_evmwlumiaaw(e500_core_t *current_core)
{
    int rD, rA, rB;
    uint64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temp = (uint64_t)(current_core->gprh[rB]) * (uint64_t)(current_core->gprh[rA]);
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) + (uint32_t)(temp);

    temp = (uint64_t)(current_core->gpr[rB]) * (uint64_t)(current_core->gpr[rA]);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) + (uint32_t)(temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *    evmwlumianw         Vector Multiply Word Low Unsigned, Modulo, Integer and Accumulate Negative in Words
 *      .1480
 */
void ppc_opc_evmwlumianw(e500_core_t *current_core)
{
    int rD, rA, rB;
    uint64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temp = current_core->gprh[rB] * current_core->gprh[rA];
    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) - (uint32_t)(temp);

    temp = current_core->gpr[rB] * current_core->gpr[rA];
    current_core->gpr[rD] = (uint32_t)(current_core->acc) - (uint32_t)(temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *    evmwlusiaaw         Vector Multiply Word Low Unsigned, Saturate, Integer and Accumulate in Words
 *      .1344
 */
void ppc_opc_evmwlusiaaw(e500_core_t *current_core)
{
    int rD, rA, rB;
    uint64_t temp;
    int32_t ovh, ovl;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temp = (uint64_t)(current_core->gprh[rB]) * (uint64_t)(current_core->gprh[rA]);
    temp = ((current_core->acc >> 32) & 0xFFFFffff) + (temp & 0xFFFFffff);

    ovh = ((temp << 31) >> 63) & 0x1;
    current_core->gprh[rD] = SATURATE(ovh, 0, 0xFFFFFFFF, 0xFFFFFFFF, temp);

    temp = (uint64_t)(current_core->gpr[rB]) * (uint64_t)(current_core->gpr[rA]);
    temp = (current_core->acc & 0xFFFFffff) + (temp & 0xFFFFffff);

    ovl = ((temp << 31) >> 63) & 0x1;
    current_core->gpr[rD] = SATURATE(ovl, 0, 0xFFFFFFFF, 0xFFFFFFFF, temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->spefscr.OVH = ovh;
    current_core->spefscr.OV = ovl;
    current_core->spefscr.SOVH |= ovh;
    current_core->spefscr.SOV |= ovl;
}

/*
 *    evmwlusianw         Vector Multiply Word Low Unsigned, Saturate, Integer and Accumulate Negative in Words
 *      .1472
 */
void ppc_opc_evmwlusianw(e500_core_t *current_core)
{
    int rD, rA, rB;
    uint64_t temp;
    int32_t ovh, ovl;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temp = (uint64_t)(current_core->gprh[rB]) * (uint64_t)(current_core->gprh[rA]);
    temp = ((current_core->acc >> 32) & 0xFFFFffff) - (temp & 0xFFFFffff);

    ovh = ((temp << 31) >> 63) & 0x1;
    current_core->gprh[rD] = SATURATE(ovh, 0, 0x00000000, 0x00000000, temp);

    temp = (uint64_t)(current_core->gpr[rB]) * (uint64_t)(current_core->gpr[rA]);
    temp = (current_core->acc & 0xFFFFffff) - (temp & 0xFFFFffff);

    ovl = ((temp << 31) >> 63) & 0x1;
    current_core->gpr[rD] = SATURATE(ovl, 0, 0x00000000, 0x00000000, temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->spefscr.OVH = ovh;
    current_core->spefscr.OV = ovl;
    current_core->spefscr.SOVH |= ovh;
    current_core->spefscr.SOV |= ovl;
}

/*
 *    evmwsmf         Vector Multiply Word Signed, Modulo, Fractional (to Accumulator)
 *      .1115
 */
void ppc_opc_evmwsmf(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temp = (int64_t)(current_core->gpr[rB]) * (int64_t)(current_core->gpr[rA]);
    temp &= 0x7fffffffffffffffLL;
    current_core->gprh[rD] = (uint32_t)(temp >> 32);
    current_core->gpr[rD] = (uint32_t)(temp);
}

/*
 *    evmwsmfa         Vector Multiply Word Signed, Modulo, Fractional (to Accumulator)
 *      .1147
 */
void ppc_opc_evmwsmfa(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    temp = (int64_t)(current_core->gpr[rB]) * (int64_t)(current_core->gpr[rA]);
    temp &= 0x7fffffffffffffffLL;
    current_core->gprh[rD] = (uint32_t)(temp >> 32);
    current_core->gpr[rD] = (uint32_t)(temp);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *    evmwsmfaa         Vector Multiply Word Signed, Modulo, Fractional and Accumulate
 *      .1371
 */
void ppc_opc_evmwsmfaa(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    temp = (int64_t)(current_core->gpr[rB]) * (int64_t)(current_core->gpr[rA]);
    temp &= 0x7fffffffffffffffLL;

    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) + (uint32_t)(temp >> 32);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) + (uint32_t)(temp);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *    evmwsmfan         Vector Multiply Word Signed, Modulo, Fractional and Accumulate Negative
 *      .1499
 */
void ppc_opc_evmwsmfan(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    temp = (int64_t)(current_core->gpr[rB]) * (int64_t)(current_core->gpr[rA]);
    temp &= 0x7fffffffffffffffLL;

    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) - (uint32_t)(temp >> 32);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) - (uint32_t)(temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *    evmwsmi         Vector Multiply Word Signed, Modulo, Integer (to Accumulator)
 *      .1113
 */
void ppc_opc_evmwsmi(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    temp = (int64_t)(current_core->gpr[rB]) * (int64_t)(current_core->gpr[rA]);

    current_core->gprh[rD] = (uint32_t)(temp >> 32);
    current_core->gpr[rD] = (uint32_t)(temp);
}

/*
 *    evmwsmia         Vector Multiply Word Signed, Modulo, Integer (to Accumulator)
 *      .1145
 */
void ppc_opc_evmwsmia(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    temp = (int64_t)(current_core->gpr[rB]) * (int64_t)(current_core->gpr[rA]);

    current_core->gprh[rD] = (uint32_t)(temp >> 32);
    current_core->gpr[rD] = (uint32_t)(temp);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *    evmwsmiaa         Vector Multiply Word Signed, Modulo, Integer and Accumulate
 *      .1369
 */
void ppc_opc_evmwsmiaa(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    temp = (int64_t)(current_core->gpr[rB]) * (int64_t)(current_core->gpr[rA]);

    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) + (uint32_t)(temp >> 32);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) + (uint32_t)(temp);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *    evmwsmian         Vector Multiply Word Signed, Modulo, Integer and Accumulate Negative
 *      .1497
 */
void ppc_opc_evmwsmian(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    temp = (int64_t)(current_core->gpr[rB]) * (int64_t)(current_core->gpr[rA]);

    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) - (uint32_t)(temp >> 32);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) - (uint32_t)(temp);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *    evmwssf         Vector Multiply Word Signed, Saturate, Fractional (to Accumulator)
 *      .1107
 */
void ppc_opc_evmwssf(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp;
    int mov;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temp = (int64_t)(current_core->gpr[rB]) * (int64_t)(current_core->gpr[rA]);
    temp &= 0x7fffffffffffffffLL;
    if ((current_core->gpr[rA] == 0x80000000) & (current_core->gpr[rB] == 0x80000000))
    {
        current_core->gprh[rD] = 0x7FFFFFFF;
        current_core->gpr[rD] = 0xFFFFFFFF;
        mov = 1;
    }
    else
    {
        current_core->gprh[rD] = (uint32_t)(temp >> 32);
        current_core->gpr[rD] = (uint32_t)(temp);
        mov = 0;
    }

    current_core->spefscr.OVH = 0;
    current_core->spefscr.OV = mov;
    current_core->spefscr.SOV |= mov;
}

/*
 *    evmwssfa         Vector Multiply Word Signed, Saturate, Fractional (to Accumulator)
 *      .1139
 */
void ppc_opc_evmwssfa(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp;
    int mov;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temp = (int64_t)(current_core->gpr[rB]) * (int64_t)(current_core->gpr[rA]);
    temp &= 0x7fffffffffffffffLL;
    if ((current_core->gpr[rA] == 0x80000000) & (current_core->gpr[rB] == 0x80000000))
    {
        current_core->gprh[rD] = 0x7FFFFFFF;
        current_core->gpr[rD] = 0xFFFFFFFF;
        mov = 1;
    }
    else
    {
        current_core->gprh[rD] = (uint32_t)(temp >> 32);
        current_core->gpr[rD] = (uint32_t)(temp);
        mov = 0;
    }

    current_core->spefscr.OVH = 0;
    current_core->spefscr.OV = mov;
    current_core->spefscr.SOV |= mov;

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *    evmwssfaa         Vector Multiply Word Signed, Saturate, Fractional and Accumulate
 *      .1363
 */
void ppc_opc_evmwssfaa(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp, tem;
    int64_t mov, flag, ov;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temp = (int64_t)(current_core->gpr[rB]) * (int64_t)(current_core->gpr[rA]);
    temp &= 0x7fffffffffffffffLL;
    if ((current_core->gpr[rA] == 0x80000000) & (current_core->gpr[rB] == 0x80000000))
    {
        temp = 0x7FFFFFFFFFFFFFFFLL;
        mov = 1;
    }
    else
    {
        mov = 0;
    }

    tem = current_core->acc + temp;
    if ((current_core->acc > 0) & (tem < 0))
    {
        flag = 1;
    }
    else if (current_core->acc < 0)
    {
        flag = 1;
    }
    else
    {
        flag = 0;
    }

    ov = flag ^ ((temp >> 63) & 0x1);
    current_core->gprh[rD] = (uint32_t)(tem >> 32);
    current_core->gpr[rD] = (uint32_t)(tem);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->spefscr.OVH = 0;
    current_core->spefscr.OV = mov;
    current_core->spefscr.SOV |= ov | mov;
}

/*
 *    evmwssfan         Vector Multiply Word Signed, Saturate, Fractional and Accumulate Negative
 *      .1491
 */
void ppc_opc_evmwssfan(e500_core_t *current_core)
{
    int rD, rA, rB;
    int64_t temp, tem;
    int64_t mov, flag, ov;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    temp = (int64_t)(current_core->gpr[rB]) * (int64_t)(current_core->gpr[rA]);
    temp &= 0x7fffffffffffffffLL;
    if ((current_core->gpr[rA] == 0x80000000) & (current_core->gpr[rB] == 0x80000000))
    {
        temp = 0x7FFFFFFFFFFFFFFFLL;
        mov = 1;
    }
    else
    {
        mov = 0;
    }

    tem = current_core->acc - temp;
    if ((current_core->acc < 0) & (tem > 0))
    {
        flag = 0;
    }
    else if (current_core->acc > 0)
    {
        flag = 0;
    }
    else
    {
        flag = 1;
    }

    ov = flag ^ ((temp >> 63) & 0x1);
    current_core->gprh[rD] = (uint32_t)(tem >> 32);
    current_core->gpr[rD] = (uint32_t)(tem);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->spefscr.OVH = 0;
    current_core->spefscr.OV = mov;
    current_core->spefscr.SOV |= ov | mov;
}

/*
 *    evmwumi         Vector Multiply Word Unsigned, Modulo, Integer (to Accumulator)
 *      .1112
 */
void ppc_opc_evmwumi(e500_core_t *current_core)
{
    int rD, rA, rB;
    uint64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    temp = (uint64_t)(current_core->gpr[rB]) * (uint64_t)(current_core->gpr[rA]);

    current_core->gprh[rD] = (uint32_t)(temp >> 32);
    current_core->gpr[rD] = (uint32_t)(temp);
}

/*
 *    evmwumia         Vector Multiply Word Unsigned, Modulo, Integer (to Accumulator)
 *      .1144
 */
void ppc_opc_evmwumia(e500_core_t *current_core)
{
    int rD, rA, rB;
    uint64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    temp = (uint64_t)(current_core->gpr[rB]) * (uint64_t)(current_core->gpr[rA]);

    current_core->gprh[rD] = (uint32_t)(temp >> 32);
    current_core->gpr[rD] = (uint32_t)(temp);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *      Vector Multiply Word Unsigned, Modulo, Integer and Accumulate
 *      .1368
 */
void ppc_opc_evmwumiaa(e500_core_t *current_core)
{
    int rD, rA, rB;
    uint64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    temp = (uint64_t)(current_core->gpr[rB]) * (uint64_t)(current_core->gpr[rA]);

    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) + (uint32_t)(temp >> 32);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) + (uint32_t)(temp);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *    evmwumian         Vector Multiply Word Unsigned, Modulo, Integer and Accumulate Negative
 *      .1496
 */
void ppc_opc_evmwumian(e500_core_t *current_core)
{
    int rD, rA, rB;
    uint64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);
    temp = (uint64_t)(current_core->gpr[rB]) * (uint64_t)(current_core->gpr[rA]);

    current_core->gprh[rD] = (uint32_t)(current_core->acc >> 32) - (uint32_t)(temp >> 32);
    current_core->gpr[rD] = (uint32_t)(current_core->acc) - (uint32_t)(temp);
    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];
}

/*
 *    evnand         Vector NAND
 *      .542
 */
void ppc_opc_evnand(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    current_core->gprh[rD] = ~(current_core->gprh[rA] & current_core->gprh[rB]);
    current_core->gpr[rD] = ~(current_core->gpr[rA] & current_core->gpr[rB]);
}

/*
 *    evneg         Vector Negate
 *      .521
 */
void ppc_opc_evneg(e500_core_t *current_core)
{
    int rD, rA;
    PPC_OPC_TEMPL_XI(current_core->current_opc, rD, rA);

    current_core->gprh[rD] = -current_core->gprh[rA];
    current_core->gpr[rD] = -current_core->gpr[rA];
}

/*
 *    evnor         Vector NOR
 *      .536
 */
void ppc_opc_evnor(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    current_core->gpr[rD] = ~(current_core->gpr[rA] | current_core->gpr[rB]);
    current_core->gprh[rD] = ~(current_core->gprh[rA] | current_core->gprh[rB]);
}

/*
 *    evor        Vector OR
 *      .535
 */
void ppc_opc_evor(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    current_core->gprh[rD] = current_core->gprh[rA] | current_core->gprh[rB];
    current_core->gpr[rD] = current_core->gpr[rA] | current_core->gpr[rB];
}

/*
 *    evorc        Vector OR with Complement
 *      .539
 */
void ppc_opc_evorc(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    current_core->gprh[rD] = current_core->gprh[rA] | ~current_core->gprh[rB];
    current_core->gpr[rD] = current_core->gpr[rA] | ~current_core->gpr[rB];
}

/*
 *    evrlw        Vector Rotate Left Word
 *      .552
 */
void ppc_opc_evrlw(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    current_core->gprh[rD] = ppc_word_rotl(current_core->gprh[rA], current_core->gprh[rB]);
    current_core->gpr[rD] = ppc_word_rotl(current_core->gpr[rA], current_core->gpr[rB]);
}

/*
 *    evrlwi        Vector Rotate Left Word Immediate
 *      .554
 */
void ppc_opc_evrlwi(e500_core_t *current_core)
{
    int rD, rA, UIMM;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, UIMM);

    current_core->gprh[rD] = ppc_word_rotl(current_core->gprh[rA], UIMM);
    current_core->gpr[rD] = ppc_word_rotl(current_core->gpr[rA], UIMM);
}

/*
 *    evrndw        Vector Round Word
 *      .524
 */
void ppc_opc_evrndw(e500_core_t *current_core)
{
    int rD, rA;
    PPC_OPC_TEMPL_XI(current_core->current_opc, rD, rA);

    current_core->gprh[rD] = (current_core->gprh[rA] + 0x00008000) & 0xFFFF0000;
    current_core->gpr[rD] = (current_core->gpr[rA] + 0x00008000) & 0xFFFF0000;
}

/*
 *    evsel        Vector Select
 *      .632-639
 */
void ppc_opc_evsel(e500_core_t *current_core)
{
    int rD, rA, rB, crS;
    PPC_OPC_TEMPL_XT(current_core->current_opc, rD, rA, rB, crS);

    int cr = ppc_opc_get_cr(current_core->cr, crS);
    if ((cr & 0x8) == 0x8)
    {
        current_core->gprh[rD] = current_core->gprh[rA];
    }
    else
    {
        current_core->gprh[rD] = current_core->gprh[rB];
    }
    if ((cr & 0x4) == 0x4)
    {
        current_core->gpr[rD] = current_core->gpr[rA];
    }
    else
    {
        current_core->gpr[rD] = current_core->gpr[rB];
    }
}

/*
 *    evslw        Vector Shift Left Word
 *      .548
 */
void ppc_opc_evslw(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    int gprhB = current_core->gprh[rB] & 0x3F;
    int gprB = current_core->gpr[rB] & 0x3F;
    current_core->gprh[rD] = (uint64)current_core->gprh[rA] << gprhB;
    current_core->gpr[rD] = (uint64)current_core->gpr[rA] << gprB;
}

/*
 *    evslwi        Vector Shift Left Word Immediate
 *      .550
 */
void ppc_opc_evslwi(e500_core_t *current_core)
{
    int rD, rA, UIMM;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, UIMM);

    current_core->gprh[rD] = current_core->gprh[rA] << UIMM;
    current_core->gpr[rD] = current_core->gpr[rA] << UIMM;
}

/*
 *    evsplatfi        Vector Splat Fractional Immediate
 *      .555
 */
void ppc_opc_evsplatfi(e500_core_t *current_core)
{
    int rD, SIMM;
    PPC_OPC_TEMPL_XI(current_core->current_opc, rD, SIMM);

    current_core->gprh[rD] = SIMM << 27;
    current_core->gpr[rD] = SIMM << 27;
}

/*
 *    evsplati        Vector Splat Immediate
 *      .553
 */
void ppc_opc_evsplati(e500_core_t *current_core)
{
    int rD, SIMM;
    PPC_OPC_TEMPL_XI(current_core->current_opc, rD, SIMM);

    current_core->gprh[rD] = (sint32)SIMM << 27 >> 27;
    current_core->gpr[rD] = (sint32)SIMM << 27 >> 27;
}

/*
 *    evsrwis        Vector Shift Right Word Immediate Signed
 *      .547
 */
void ppc_opc_evsrwis(e500_core_t *current_core)
{
    int rD, rA, UIMM;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, UIMM);

    current_core->gprh[rD] = (sint32)current_core->gprh[rA] >> UIMM;
    current_core->gpr[rD] = (sint32)current_core->gpr[rA] >> UIMM;
}

/*
 *    evsrwiu        Vector Shift Right Word Immediate Unsigned
 *      .546
 */
void ppc_opc_evsrwiu(e500_core_t *current_core)
{
    int rD, rA, UIMM;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, UIMM);

    current_core->gprh[rD] = current_core->gprh[rA] >> UIMM;
    current_core->gpr[rD] = current_core->gpr[rA] >> UIMM;
}

/*
*evsrws        Vector Shift Right Word Signed
*.545
*/
void ppc_opc_evsrws(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    int gprhB = current_core->gprh[rB] & 0x3F;
    int gprB = current_core->gpr[rB] & 0x3F;
    current_core->gprh[rD] = (sint64)(sint32)current_core->gprh[rA] >> gprhB;
    current_core->gpr[rD] = (sint64)(sint32)current_core->gpr[rA] >> gprB;
}

/*
 *    evsrwu        Vector Shift Right Word Unsigned
 *      .544
 */
void ppc_opc_evsrwu(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    int gprhB = current_core->gprh[rB] & 0x3F;
    int gprB = current_core->gpr[rB] & 0x3F;
    current_core->gprh[rD] = (uint64)current_core->gprh[rA] >> gprhB;
    current_core->gpr[rD] = (uint64)current_core->gpr[rA] >> gprB;
}

/*
 *  Vector store doubleword of doubleword
 *  .801
 */
void ppc_opc_evstdd(e500_core_t *current_core)
{
    int rS, rA, UIMM;
    PPC_OPC_TEMPL_X(current_core->current_opc, rS, rA, UIMM);

    uint32 EA = current_core->gpr[rA] + UIMM * 8;
    ppc_write_effective_word(current_core, EA, current_core->gprh[rS]);
    ppc_write_effective_word(current_core, EA + 4, current_core->gpr[rS]);
}

/*
 *    evstddx        Vector Store Double of Double Indexed
 *      .800
 */
void ppc_opc_evstddx(e500_core_t *current_core)
{
    int rS, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rS, rA, rB);

    uint32 EA = current_core->gpr[rA] + current_core->gpr[rB];
    ppc_write_effective_word(current_core, EA, current_core->gprh[rS]);
    ppc_write_effective_word(current_core, EA + 4, current_core->gpr[rS]);
}

/*
*evstdh        Vector Store Double of Four Half Words
*.805
*/
void ppc_opc_evstdh(e500_core_t *current_core)
{
    int rS, rA, UIMM;
    PPC_OPC_TEMPL_X(current_core->current_opc, rS, rA, UIMM);

    uint32 EA = current_core->gpr[rA] + UIMM * 8;
    ppc_write_effective_half(current_core, EA, (uint16)(current_core->gprh[rS] >> 16));
    ppc_write_effective_half(current_core, EA + 2, (uint16)current_core->gprh[rS]);
    ppc_write_effective_half(current_core, EA + 4, (uint16)(current_core->gpr[rS] >> 16));
    ppc_write_effective_half(current_core, EA + 6, (uint16)current_core->gpr[rS]);
}

/*
 *    evstdhx        Vector Store Double of Four Half Words Indexed
*.804
*/
void ppc_opc_evstdhx(e500_core_t *current_core)
{
    int rS, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rS, rA, rB);

    uint32 EA = current_core->gpr[rA] + current_core->gpr[rB];
    ppc_write_effective_half(current_core, EA, (uint16)(current_core->gprh[rS] >> 16));
    ppc_write_effective_half(current_core, EA + 2, (uint16)current_core->gprh[rS]);
    ppc_write_effective_half(current_core, EA + 4, (uint16)(current_core->gpr[rS] >> 16));
    ppc_write_effective_half(current_core, EA + 6, (uint16)current_core->gpr[rS]);
}

/*
 *    evstdw        Vector Store Double of Two Words
 *      .803
*/
void ppc_opc_evstdw(e500_core_t *current_core)
{
    int rS, rA, UIMM;
    PPC_OPC_TEMPL_X(current_core->current_opc, rS, rA, UIMM);

    uint32 EA = current_core->gpr[rA] + UIMM * 8;
    ppc_write_effective_word(current_core, EA, current_core->gprh[rS]);
    ppc_write_effective_word(current_core, EA + 4, current_core->gpr[rS]);
}

/*
 *    evstdwx        Vector Store Double of Two Words Indexed
 *      .802
*/
void ppc_opc_evstdwx(e500_core_t *current_core)
{
    int rS, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rS, rA, rB);

    uint32 EA = current_core->gpr[rA] + current_core->gpr[rB];
    ppc_write_effective_word(current_core, EA, current_core->gprh[rS]);
    ppc_write_effective_word(current_core, EA + 4, current_core->gpr[rS]);
}

/*
 *    evstwhe        Vector Store Word of Two Half Words from Even
 *      .817
 */
void ppc_opc_evstwhe(e500_core_t *current_core)
{
    int rS, rA, UIMM;
    PPC_OPC_TEMPL_X(current_core->current_opc, rS, rA, UIMM);

    uint32 EA = current_core->gpr[rA] + UIMM * 4;
    ppc_write_effective_half(current_core, EA, (uint16)(current_core->gprh[rS] >> 16));
    ppc_write_effective_half(current_core, EA + 2, (uint16)(current_core->gpr[rS] >> 16));
}

/*
 *    evstwhex        Vector Store Word of Two Half Words from Even Indexed
 *      .816
 */
void ppc_opc_evstwhex(e500_core_t *current_core)
{
    int rS, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rS, rA, rB);

    uint32 EA = current_core->gpr[rA] + current_core->gpr[rB];
    ppc_write_effective_half(current_core, EA, (uint16)(current_core->gprh[rS] >> 16));
    ppc_write_effective_half(current_core, EA + 2, (uint16)(current_core->gpr[rS] >> 16));
}

/*
 *    evstwho        Vector Store Word of Two Half Words from Odd
 *      .821
 */
void ppc_opc_evstwho(e500_core_t *current_core)
{
    int rS, rA, UIMM;
    PPC_OPC_TEMPL_X(current_core->current_opc, rS, rA, UIMM);

    uint32 EA = current_core->gpr[rA] + UIMM * 4;
    ppc_write_effective_half(current_core, EA, (uint16)current_core->gprh[rS]);
    ppc_write_effective_half(current_core, EA + 2, (uint16)current_core->gpr[rS]);
}

/*
 *    evstwhox        Vector Store Word of Two Half Words from Odd Indexed
 *      .820
 */
void ppc_opc_evstwhox(e500_core_t *current_core)
{
    int rS, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rS, rA, rB);

    uint32 EA = current_core->gpr[rA] + current_core->gpr[rB];
    ppc_write_effective_half(current_core, EA, (uint16)current_core->gprh[rS]);
    ppc_write_effective_half(current_core, EA + 2, (uint16)current_core->gpr[rS]);
}

/*
 *      Vector Store Word of Word from Even
 *      .825
 */
void ppc_opc_evstwwe(e500_core_t *current_core)
{
    int rS, rA, UIMM;
    PPC_OPC_TEMPL_X(current_core->current_opc, rS, rA, UIMM);

    uint32 EA = current_core->gpr[rA] + UIMM * 4;
    ppc_write_effective_word(current_core, EA, current_core->gprh[rS]);
}

/*
 *    evstwwex        Vector Store Word of Word from Even Indexed
 *      .824
 */
void ppc_opc_evstwwex(e500_core_t *current_core)
{
    int rS, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rS, rA, rB);

    uint32 EA = current_core->gpr[rA] + current_core->gpr[rB];
    ppc_write_effective_word(current_core, EA, current_core->gprh[rS]);
}

/*
 *    evstwwo        Vector Store Word of Word from Odd
 *      .829
 */
void ppc_opc_evstwwo(e500_core_t *current_core)
{
    int rS, rA, UIMM;
    PPC_OPC_TEMPL_X(current_core->current_opc, rS, rA, UIMM);

    uint32 EA = current_core->gpr[rA] + UIMM * 4;
    ppc_write_effective_word(current_core, EA, current_core->gpr[rS]);
}

/*
 *    evstwwox        Vector Store Word of Word from Odd Indexed
 *      .828
 */
void ppc_opc_evstwwox(e500_core_t *current_core)
{
    int rS, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rS, rA, rB);

    uint32 EA = current_core->gpr[rA] + current_core->gpr[rB];
    ppc_write_effective_word(current_core, EA, current_core->gpr[rS]);
}

/*
 *    evsubfsmiaaw         Vector Subtract Signed, Modulo, Integer to Accumulator Word
 *      .1227
 */
void ppc_opc_evsubfsmiaaw(e500_core_t *current_core)
{
    int rD, rA;
    PPC_OPC_TEMPL_XI(current_core->current_opc, rD, rA);

    current_core->gprh[rD] = (sint32)(current_core->acc >> 32) - (sint32)current_core->gprh[rA];
    current_core->gpr[rD] = (sint32)current_core->acc - (sint32)current_core->gpr[rA];
    current_core->acc = ((sint64)current_core->gprh[rD] << 32) | current_core->gpr[rD];
}

/*
 *    evsubfssiaaw         Vector Subtract Signed, Saturate, Integer to Accumulator Word
 *      .1219
 */
void ppc_opc_evsubfssiaaw(e500_core_t *current_core)
{
    int rD, rA, n, ovh, ovl;
    int temp_31;
    int64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, n);
    temp = (current_core->acc >> 32) - (((int64_t)(current_core->gprh[rA]) << 32) >> 32);
    ovh = (((temp << 31) >> 63) & 0x1) ^ (((temp << 32) >> 63) & 0x1);
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gprh[rD] = SATURATE(ovh, temp_31, 0x00000000, 0x00000000, temp);

    temp = (((current_core->acc & 0xFFFFffff) << 32) >> 32) - (((int64_t)(current_core->gpr[rA]) << 32) >> 32);
    ovl = (((temp << 31) >> 63) & 0x1) ^ (((temp << 32) >> 63) & 0x1);
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gpr[rD] = SATURATE(ovl, temp_31, 0x00000000, 0x00000000, temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->spefscr.OVH = ovh;
    current_core->spefscr.OV = ovl;
    current_core->spefscr.SOVH = current_core->spefscr.SOVH | ovh;
    current_core->spefscr.SOV = current_core->spefscr.SOV | ovl;
}

/*
 *    evsubfumiaaw         Vector Subtract Unsigned, Modulo, Integer to Accumulator Word
 *      .1226
 */
void ppc_opc_evsubfumiaaw(e500_core_t *current_core)
{
    int rD, rA;
    PPC_OPC_TEMPL_XI(current_core->current_opc, rD, rA);

    current_core->gprh[rD] = (uint32)(current_core->acc >> 32) - current_core->gprh[rA];
    current_core->gpr[rD] = (uint32)current_core->acc - current_core->gpr[rA];
    current_core->acc = ((sint64)current_core->gprh[rD] << 32) | current_core->gpr[rD];
}

/*
 *    evsubfusiaaw         Vector Subtract Unsigned, Saturate, Integer to Accumulator Word
 *      .1218
 */
void ppc_opc_evsubfusiaaw(e500_core_t *current_core)
{
    int rD, rA, n, ovh, ovl;
    int temp_31;
    uint64_t temp;

    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, n);
    temp = ((current_core->acc >> 32) & 0xFFFFffff) - (uint64_t)(current_core->gprh[rA]);
    ovh = ((temp << 31) >> 63) & 0x1;
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gprh[rD] = SATURATE(ovh, temp_31, 0x00000000, 0x00000000, temp);

    temp = (((current_core->acc & 0xFFFFffff) << 32) >> 32) - (((int64_t)(current_core->gpr[rA]) << 32) >> 32);
    ovl = ((temp << 31) >> 63) & 0x1;
    temp_31 = ((temp << 31) >> 63) & 0x1;
    current_core->gpr[rD] = SATURATE(ovl, temp_31, 0x00000000, 0x00000000, temp);

    current_core->acc = ((int64_t)(current_core->gprh[rD]) << 32) | current_core->gpr[rD];

    current_core->spefscr.OVH = ovh;
    current_core->spefscr.OV = ovl;
    current_core->spefscr.SOVH = current_core->spefscr.SOVH | ovh;
    current_core->spefscr.SOV = current_core->spefscr.SOV | ovl;
}

/*
 *    evsubfw         Vector Subtract from Word
 *      .516
 */
void ppc_opc_evsubfw(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    current_core->gprh[rD] = current_core->gprh[rB] - current_core->gprh[rA];
    current_core->gpr[rD] = current_core->gpr[rB] - current_core->gpr[rA];
}

/*
 *    evsubifw         Vector Subtract Immediate from Word
 *      .518
 */
void ppc_opc_evsubifw(e500_core_t *current_core)
{
    int rD, UIMM, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, UIMM, rB);

    current_core->gprh[rD] = current_core->gprh[rB] - UIMM;
    current_core->gpr[rD] = current_core->gpr[rB] - UIMM;
}

/*
 *      Vector XOR
 *      .534
 */
void ppc_opc_evxor(e500_core_t *current_core)
{
    int rD, rA, rB;
    PPC_OPC_TEMPL_X(current_core->current_opc, rD, rA, rB);

    current_core->gprh[rD] = current_core->gprh[rA] ^ current_core->gprh[rB];
    current_core->gpr[rD] = current_core->gpr[rA] ^ current_core->gpr[rB];
}
