/*
 *      PearPC
 *      ppc_tools.h
 *
 *      Copyright (C) 2003 Sebastian Biallas (sb@biallas.net)
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License version 2 as
 *      published by the Free Software Foundation.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __PPC_TOOLS_H__
#define __PPC_TOOLS_H__

#include "skyeye_types.h"
#include "ppc_fpu.h"
static inline bool_t ppc_carry_3(uint32 a, uint32 b, uint32 c)
{
    if ((a + b) < a)
    {
        return True;
    }
    if ((a + b + c) < c)
    {
        return True;
    }
    return False;
}

static inline uint32 ppc_word_rotl(uint32 data, int n)
{
    n &= 0x1f;
    return (data << n) | (data >> (32 - n));
}

static inline uint64 ppc_opc_get_reg_64(uint32 reg_high, uint32 reg_low)
{
    return ((uint64)reg_high << 32) | reg_low;
}

static inline uint32 ppc_opc_get_reg_64_high(uint64 data)
{
    return (uint32)(data >> 32);
}

static inline uint32 ppc_opc_get_reg_64_low(uint64 data)
{
    return (uint32)data;
}

static inline uint32 ppc_opc_get_cr(uint32 reg, int n)
{
    return (reg >> (7 - n) * 4) & 0xF;
}

static inline uint32 ppc_opc_get_bit(uint32 reg, int n)
{
    return (reg >> (31 - n)) & 1;
}

static inline uint64 ppc_opc_get_bit_64(uint64 reg, int n)
{
    return (reg >> (63 - n)) & 1;
}

static inline uint32 ppc_opc_get_bits(uint32 reg, int m, int n)
{
	return (reg >> (31 - n)) & (0xFFFFFFFF >> (31 - (n - m)));
}

static inline uint64 ppc_opc_get_bits_64(uint64 reg, int m, int n)
{
	return (reg >> (63 - n)) & (0xFFFFFFFFFFFFFFFFLL >> (63 - (n - m)));
}

#define ppc_opc_set_cr(reg, n, data)                      \
    {                                                     \
        (reg) &= ~(~(~0U << 4) << (7 - (n)) * 4);         \
        (reg) |= ((data) & ~(~0U << 4)) << (7 - (n)) * 4; \
    }

#define ppc_opc_set_bit(reg, n, data)                                          \
    {                                                                          \
        (data) & 1 ? ((reg) |= (1 << 31 - (n))) : ((reg) &= ~(1 << 31 - (n))); \
    }

#define ppc_opc_set_bit_64(reg, n, data)                                           \
    {                                                                              \
        (data) & 1 ? ((reg) |= (1LL << 63 - (n))) : ((reg) &= ~(1LL << 63 - (n))); \
    }

#define ppc_opc_set_bits(reg, m, n, data)                        \
    {                                                            \
        (reg) &= ~(~(~0U << (n) - (m) + 1) << 31 - (n));         \
        (reg) |= ((data) & ~(~0U << (n) - (m) + 1)) << 31 - (n); \
    }

#define ppc_opc_set_bits_64(reg, m, n, data)                       \
    {                                                              \
        (reg) &= ~(~(~0ULL << (n) - (m) + 1) << 63 - (n));         \
        (reg) |= ((data) & ~(~0ULL << (n) - (m) + 1)) << 63 - (n); \
    }

void ppc_fpu_single_to_double(ppc_single * s, ppc_double * d);
uint32 ppc_fpu_pack_single(e500_core_t *current_core, ppc_double * d, uint32 * res);
void ppc_fpu_unpack_single(ppc_single * res, uint32 d);
uint32 ppc_fpu_pack_double(e500_core_t *current_core, ppc_double * d, uint64 * res);
void ppc_fpu_unpack_double(ppc_double * res, uint64 d);
int ppc_fpu_normalize(ppc_double * d);
int ppc_fpu_normalize_single(ppc_single * s);
int ppc_fpu_normalize_quadro(ppc_quadro * d);
uint32 ppc_fpu_pack_double_as_single(e500_core_t *current_core, ppc_double * d, uint64 * res);
uint32 ppc_fpu_double_to_int(e500_core_t *current_core, ppc_double * d);
#endif
