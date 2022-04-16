
/*************************************************************************
	> File Name    : armv8_helper.c
	> Author       : jiachao
	> Mail         : jiachao@163.com
	> Created Time : 2019/9/23 11:04:46
************************************************************************/
#include <skyeye_swapendian.h>
#include "armdefs.h"
#include "arch64_helper.h"

/**
 * revbit16 - reverse the bits in a 16-bit value.
 * @x: The value to modify.
 */
inline uint16_t revbit16(uint16_t x)
{
    /* Assign the correct byte position.  */
    x = bswap_half(x);
    /* Assign the correct nibble position.  */
    x = ((x & 0xf0f0) >> 4) | ((x & 0x0f0f) << 4);
    /* Assign the correct bit position.  */
    x = ((x & 0x8888) >> 3) | ((x & 0x4444) >> 1) | ((x & 0x2222) << 1) | ((x & 0x1111) << 3);
    return x;
}

/**
 * revbit32 - reverse the bits in a 32-bit value.
 * @x: The value to modify.
 */
inline uint32_t revbit32(uint32_t x)
{
    /* Assign the correct byte position.  */
    x = bswap_word(x);
    /* Assign the correct nibble position.  */
    x = ((x & 0xf0f0f0f0u) >> 4) | ((x & 0x0f0f0f0fu) << 4);
    /* Assign the correct bit position.  */
    x = ((x & 0x88888888u) >> 3) | ((x & 0x44444444u) >> 1) | ((x & 0x22222222u) << 1) | ((x & 0x11111111u) << 3);
    return x;
}

/**
 * revbit64 - reverse the bits in a 64-bit value.
 * @x: The value to modify.
 */
inline uint64_t revbit64(uint64_t x)
{
    /* Assign the correct byte position.  */
    x = bswap_dword(x);
    /* Assign the correct nibble position.  */
    x = ((x & 0xf0f0f0f0f0f0f0f0ull) >> 4) | ((x & 0x0f0f0f0f0f0f0f0full) << 4);
    /* Assign the correct bit position.  */
    x = ((x & 0x8888888888888888ull) >> 3)
        | ((x & 0x4444444444444444ull) >> 1) | ((x & 0x2222222222222222ull) << 1) | ((x & 0x1111111111111111ull) << 3);
    return x;
}

inline int64_t gen_sar(uint64_t data, int shift)
{
    int64_t result;

    ((data >> 63) == 1 ? (result = (BIT64_MAX_VALUE >> shift << shift) | (data >> shift)) : (result = data >> shift));

    return result;
}

inline int64_t gen_sextract64(uint64_t value, int ofs, int len)
{
    uint64_t result;

    assert(ofs < 64);
    assert(len > 0);
    assert(len <= 64);
    assert(ofs + len <= 64);

    result = value << (64 - len - ofs);

    return gen_sar(result, 64 - len);
}

inline uint32_t extract32(uint32_t value, int start, int length)
{
    assert(start >= 0 && length > 0 && length <= 32 - start);
    return (value >> start) & (~0U >> (32 - length));
}

inline int64_t gen_extract64(uint64_t value, int ofs, int len)
{
    uint64_t result;

    assert(ofs < 64);
    assert(len > 0);
    assert(len <= 64);
    assert(ofs + len <= 64);

    result = value << (64 - len - ofs);

    return result >> (64 - len);
}

uint64_t gen_deposit64(uint64_t rd_val, uint64_t rn_val, int ofs, int len)
{
    uint64_t mask, tmp_val, result;

    assert(ofs < 64);
    assert(len > 0);
    assert(len <= 64);
    assert(ofs + len <= 64);

    if (len == 64)
        return rn_val;

    mask = (1 << len) - 1;

    if (ofs + len < 64)
    {
        tmp_val = rn_val & mask;
        tmp_val <<= ofs;
    } else
        tmp_val = rn_val << ofs;

    result = rd_val & ~(mask << ofs);
    result |= tmp_val;

    return result;
}

uint64_t gen_deposit_z64(uint64_t rn_val, int ofs, int len)
{
    uint64_t result;

    assert(ofs < 64);
    assert(len > 0);
    assert(len <= 64);
    assert(ofs + len <= 64);

    if (ofs + len == 64)
    {
        result = rn_val << ofs;
    } else if (ofs == 0)
    {
        result = rn_val & ((1 << len) - 1);
    } else
    {
        result = rn_val & ((1 << len) - 1);
        result = result << ofs;
    }

    return result;
}

inline int64_t sextract64(uint64_t value, int start, int length)
{
    assert(start >= 0 && length > 0 && length <= 64 - start);
    /* Note that this implementation relies on right shift of signed
     * integers being an arithmetic shift.
     */
    return ((int64_t) (value << (64 - length - start))) >> (64 - length);
}

inline int64_t SignExtend64(uint64_t data, int size)
{
    return (data & (1 << (size - 1))) ? (data | (BIT64_MAX_VALUE << size)) : data;
}

inline uint64_t GetShiftValue(uint64_t Data, int shift)
{
    switch (shift)
    {
        case 0:
#ifdef __WIN32__
            Data &= 0xFFFFFFFFFFFF0000ULL;
#else
            Data &= 0xFFFFFFFFFFFF0000UL;
#endif
            break;
        case 16:
#ifdef __WIN32__
            Data &= 0xFFFFFFFF0000FFFFULL;
#else
            Data &= 0xFFFFFFFF0000FFFFUL;
#endif
            break;
        case 32:
#ifdef __WIN32__
            Data &= 0xFFFF0000FFFFFFFFULL;
#else
            Data &= 0xFFFF0000FFFFFFFFUL;
#endif
            break;
        case 48:
#ifdef __WIN32__
            Data &= 0xFFFFFFFFFFFFULL;
#else
            Data &= 0xFFFFFFFFFFFFUL;
#endif
            break;
        default:
            break;
    }
    return Data;
}

/**
 * clz32 - count leading zeros in a 32-bit value.
 * @val: The value to search
 *
 * Returns 32 if the value is zero.  Note that the GCC builtin is
 * undefined if the value is zero.
 */
inline int clz32(uint32_t val)
{
    return val ? __builtin_clz(val) : 32;
}

/**
 * ctz32 - count trailing zeros in a 32-bit value.
 * @val: The value to search
 *
 * Returns 32 if the value is zero.  Note that the GCC builtin is
 * undefined if the value is zero.
 */
inline int ctz32(uint32_t val)
{
    return val ? __builtin_ctz(val) : 32;
}

/**
 * clz64 - count leading zeros in a 64-bit value
 * @val: The value to search
 *
 * Returns 64 if the value is zero.  Note that the GCC builtin is
 * undefined if the value is zero
 */
inline int clz64(uint64_t val)
{
    return val ? __builtin_clzll(val) : 64;
}

/* Return a value with the bottom len bits set (where 0 < len <= 64) */
inline uint64_t bitmask64(uint32_t length)
{
    assert(length > 0 && length <= 64);
    return ~0ULL >> (64 - length);
}

/**
 * cls32 - count leading redundant sign bits in a 32-bit value.
 * @val: The value to search
 *
 * Returns the number of bits following the sign bit that are equal to it.
 * No special cases; output range is [0-31].
 */
inline int cls32(uint32_t val)
{
    return clz32(val ^ ((int32_t) val >> 1)) - 1;
}

/**
 * cls64 - count leading redundant sign bits in a 64-bit value.
 * @val: The value to search
 *
 * Returns the number of bits following the sign bit that are equal to it.
 * No special cases; output range is [0-63].
 */
inline int cls64(uint64_t val)
{
    return clz64(val ^ ((int64_t) val >> 1)) - 1;
}

/* The input should be a value in the bottom e bits (with higher
 * bits zero); returns that value replicated into every element
 * of size e in a 64 bit integer.
 */
inline uint64_t bitfield_replicate(uint64_t mask, uint32_t e)
{
    assert(e != 0);
    while (e < 64)
    {
        mask |= mask << e;
        e *= 2;
    }
    return mask;
}

inline uint32_t DecodeBitMasks(uint64_t * result, uint32_t n, uint32_t imms, uint32_t immr)
{
    uint64_t mask;
    uint32_t e, levels, len, s, r;

    /* First determine the element size */
    len = 31 - clz32((n << 6) | (~imms & 0x3f));
    if (len < 1)
    {
        /* This is the immn == 0, imms == 0x11111x case */
        return False;
    }

    e = 1 << len;

    levels = e - 1;
    s = imms & levels;
    r = immr & levels;

    if (s == levels)
    {
        /* <length of run - 1> mustn't be all-ones. */
        return False;
    }

    /* Create the value of one element: s+1 set bits rotated
     * * by r within the element (which is e bits wide)
     * */
    mask = bitmask64(s + 1);

    if (r)
    {
        mask = (mask >> r) | (mask << (e - r));
        mask &= bitmask64(e);
    }
    /* then replicate the element over the whole 64 bit value */
    mask = bitfield_replicate(mask, e);
    *result = mask;

    return True;
}

inline uint32_t Poly32Mod2(uint64_t Data, uint32_t poly, int size)
{
    int i = size;
    uint64_t tmp_poly = (uint64_t) poly;

    for (; i > 32; i--)
    {
        if ((Data >> (i - 1)) & 0x1)
        {
            Data &= BIT32_MAX_VALUE << (size - i);
            tmp_poly = tmp_poly << (i - 32 - 1);
            Data = Data ^ tmp_poly;
        }
    }

    return (uint32_t) Data;
}

inline uint32_t ROR32(uint32_t Data, int amount)
{
    uint32_t result;

    assert(amount >= 0);

    if (amount == 0)
        result = Data;
    else
    {
        int m = amount % 32;

        result = Data >> m | Data << (32 - m);
    }

    return result;
}

inline uint64_t ROR64(uint64_t Data, int amount)
{
    uint64_t result;

    assert(amount >= 0);

    if (amount == 0)
        result = Data;
    else
    {
        int m = amount % 64;

        result = Data >> m | Data << (64 - m);
    }

    return result;
}
