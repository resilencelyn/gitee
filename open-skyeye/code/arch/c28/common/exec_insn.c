/* Copyright (C) 
 * 2013 - Michael.Kang blackfin.kang@gmail.com
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

/**
* @file exec_insn.c
* @brief The instruction interpreter
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-07-31
*/

#include "c28_cpu.h"
#include "c28_decode.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "skyeye_types.h"
#include "skyeye_bus.h"
#include "skyeye_uart_ops.h"
#include "skyeye_interface.h"

#define DEBUG
#include <skyeye_log.h>

#ifdef DBG
#undef DBG
#define DBG
#endif

/* address mode */
#if 0
******************************* direct Addressing Modes(DP)
    00 III III 6 bit set loc = 14
        *******************************
        Stack Addressing Modes(SP)
    01 III III * −SP[6 bit] set loc = 13
    10 111 101 * SP++ set loc = 13
    10 111 110 * −−SP set loc = 13
    *******************************
    28 x Indirect Addressing Modes(XAR0 to XAR7)
    10 000 AAA * XARn++ set loc = 0 - 7
    10 001 AAA * −−XARn set loc = 0 - 7
    10 010 AAA * +XARn[AR0] set loc = 0 - 7
    10 011 AAA * +XARn[AR1] set loc = 0 - 7
    11 III AAA * +XARn[3 bit] set loc = 0 - 7
    *******************************
    Circular Indirect Addressing Modes(XAR6, XAR1)
    10 111 111 * AR6 % ++set loc = 6
    *******************************
    32 - Bit Register Addressing Modes(XAR0 to XAR7, ACC, P, XT)
    10 100 AAA XARn set loc = 0 - 7
    10 101 001 ACC set loc = 9
    10 101 011 P set loc = 11
    10 101 100 XT set loc = 12
    *******************************
    16 - Bit Register Addressing Modes(AR0 to AR7, AH, AL, PH, PL, TH, SP)
    10 100 AAA ARn set loc = 0 - 7
    10 101 000 AH set loc = 8
    10 101 001 AL set loc = 9
    10 101 010 PH set loc = 10 10 101 011 PL set loc = 11 10 101 100 TH set loc = 12 10 101 101 SP set loc = 13
#endif
#define NOT_TEST_INSTRUCTION printf("In %s, This instruction is first tested.\n", __func__)

int decode_instr(uint32_t insn, int32_t * idx, ISEITEM * table, int table_len);

static int exec_mov32_RaH_loc32_cond(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int loc = 0;
    int loc32 = TRUE;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);
    volatile uint32_t *reg = NULL;
    int cond = BITS(16, 19);
    int Ra = BITS(8, 10);
    int mem32_data;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        mem32_data = c28_memory_read(core, core->data_addr) | (c28_memory_read(core, core->data_addr + 1) << 16);
    } else if ((mode_2 == 1) || (mode_2 == 0x2 && mode_3 == 0x7))
    {
        mem32_data = c28_memory_read(core, core->sp_count) | (c28_memory_read(core, core->sp_count + 1) << 16);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        mem32_data = c28_memory_read(core, core->data_addr) | (c28_memory_read(core, core->data_addr + 1) << 16);
    } else
        mem32_data = *reg;

    if (check_float_insn_flag_cond(core, cond) == TRUE)
        core->fp_regs[Ra].Ra = mem32_data;

    if (cond == UNCF)
    {
        /* Negative Floating-point conditions */
        CLEAR_BIT(core->stf, ZF);
        if (core->fp_regs[Ra].Ra & (0x1 << 31))
        {
            SET_BIT(core->stf, NF);
            SET_BIT(core->stf, NI);
        } else
        {
            CLEAR_BIT(core->stf, NF);
            CLEAR_BIT(core->stf, NI);
        }
        if ((core->fp_regs[Ra].Ra & (0xff << 23)) == 0) /* RaH[30 : 23] */
        {
            SET_BIT(core->stf, ZF);
            CLEAR_BIT(core->stf, NF);
        }
        CLEAR_BIT(core->stf, ZI);

        if (core->fp_regs[Ra].Ra == 0)
            SET_BIT(core->stf, ZI);
    }
    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_mov32_RaH_RbH_cond(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int cond = BITS(16, 19);
    int Ra = BITS(0, 2);
    int Rb = BITS(3, 5);

    if (check_float_insn_flag_cond(core, cond) == TRUE)
        core->fp_regs[Ra].Ra = core->fp_regs[Rb].Ra;

    if (cond == UNCF)
    {
        /* Negative Floating-point conditions */
        CLEAR_BIT(core->stf, ZF);
        if (core->fp_regs[Ra].Ra & (0x1 << 31))
        {
            SET_BIT(core->stf, NF);
            SET_BIT(core->stf, NI);
        } else
        {
            CLEAR_BIT(core->stf, NF);
            CLEAR_BIT(core->stf, NI);
        }
        if ((core->fp_regs[Ra].Ra & (0xff << 23)) == 0) /* RaH[30 : 23] */
        {
            SET_BIT(core->stf, ZF);
            CLEAR_BIT(core->stf, NF);
        }
        CLEAR_BIT(core->stf, ZI);

        if (core->fp_regs[Ra].Ra == 0)
            SET_BIT(core->stf, ZI);
    }
    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_addf32_RaH_RbH_RcH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int Ra = BITS(16, 18);
    int Rb = BITS(19, 21);
    int Rc = BITS(22, 24);

    s_precision va_rs1, va_rs2, va_rd;

    va_rs1.data = fpu_get_single(core, Rb);
    va_rs2.data = fpu_get_single(core, Rc);

    va_rd.value = va_rs1.value + va_rs2.value;

    fpu_put_single_bit32(core, va_rd.data, Ra);

    /* Overflow conditions */
    if (va_rd.data == FLOAT_POSITIVE_OVERFLOW)
    {
        SET_BIT(core->stf, LVF);
    }
    /* Underflow conditions */
    if (va_rd.data == NEGATIVE_UNDERFLOW)
    {
        SET_BIT(core->stf, LUF);
    }
    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_mov32_loc32_RaH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int loc = 0;
    int loc32 = TRUE;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);
    volatile uint32_t *reg = NULL;
    int Ra = BITS(8, 10);
    int mem32_data = core->fp_regs[Ra].Ra;
    int mem32_low_data = core->fp_regs[Ra].Ral;
    int mem32_high_data = core->fp_regs[Ra].Rah;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr, mem32_low_data);
        c28_memory_write(core, core->data_addr + 1, mem32_high_data);
    } else if ((mode_2 == 1) || (mode_2 == 0x2 && mode_3 == 0x7))
    {
        c28_memory_write(core, core->sp_count, mem32_low_data);
        c28_memory_write(core, core->sp_count + 1, mem32_high_data);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        c28_memory_write(core, core->data_addr, mem32_low_data);
        c28_memory_write(core, core->data_addr + 1, mem32_high_data);
    } else
        *reg = mem32_data;

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_subf32_RaH_RbH_RcH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int Ra = BITS(16, 18);
    int Rb = BITS(19, 21);
    int Rc = BITS(22, 24);

    s_precision va_rs1, va_rs2, va_rd;

    va_rs1.data = fpu_get_single(core, Rb);
    va_rs2.data = fpu_get_single(core, Rc);

    va_rd.value = va_rs1.value - va_rs2.value;

    fpu_put_single_bit32(core, va_rd.data, Ra);

    /* Overflow conditions */
    if (va_rd.data == FLOAT_POSITIVE_OVERFLOW)
    {
        SET_BIT(core->stf, LVF);
    }
    /* Underflow conditions */
    if (va_rd.data == NEGATIVE_UNDERFLOW)
    {
        SET_BIT(core->stf, LUF);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_subf32_RaH_imm16_RbH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int Ra = BITS(0, 2);
    int Rb = BITS(3, 5);
    int imm16 = BITS(6, 21);

    s_precision va_rs1, va_rs2, va_rd;

    va_rs1.data = imm16 << 16;
    va_rs2.data = fpu_get_single(core, Rb);

    va_rd.value = va_rs1.value - va_rs2.value;
    fpu_put_single_bit32(core, va_rd.data, Ra);

    /* Overflow conditions */
    if (va_rd.data == FLOAT_POSITIVE_OVERFLOW)
    {
        SET_BIT(core->stf, LVF);
    }
    /* Underflow conditions */
    if (va_rd.data == NEGATIVE_UNDERFLOW)
    {
        SET_BIT(core->stf, LUF);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_mpyf32_RaH_RbH_RcH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int Ra = BITS(16, 18);
    int Rb = BITS(19, 21);
    int Rc = BITS(22, 24);
    int sign_flag;

    s_precision va_rs1, va_rs2, va_rd;

    va_rs1.data = fpu_get_single(core, Rb);
    va_rs2.data = fpu_get_single(core, Rc);

    va_rd.value = va_rs1.value * va_rs2.value;

    sign_flag = ((va_rs1.data >> 31) & 0x1) ^ ((va_rs2.data >> 31) & 0x1);
    if (((va_rd.data >> 31) & 0x1) && (sign_flag == 0))
    {
        va_rd.data = FLOAT_POSITIVE_OVERFLOW;
    } else if (va_rd.data > 0xff800000)
    {
        va_rd.data = 0xff800000;
    }
    fpu_put_single_bit32(core, va_rd.data, Ra);

    /* Overflow conditions */
    if (va_rd.data == FLOAT_POSITIVE_OVERFLOW)
    {
        SET_BIT(core->stf, LVF);
    }
    /* Underflow conditions */
    if (va_rd.data == NEGATIVE_UNDERFLOW)
    {
        SET_BIT(core->stf, LUF);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_zero_RaH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int Ra = BITS(0, 2);

    core->fp_regs[Ra].Ra = 0;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_f32toi16_RaH_RbH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int Ra = BITS(16, 18);
    int Rb = BITS(19, 21);
    int sign_ext = 0;

    s_precision va_rs, va_rd;

    va_rs.data = fpu_get_single(core, Rb);
    if ((va_rs.data >> 31) & 0x1)
        sign_ext = 1;

    va_rd.data = (int) va_rs.value;

    if ((va_rd.data > 0x7fff) && (((va_rd.data >> 31) & 0x1) == 0))
    {
        va_rd.data = 0x7fff;
        fpu_put_single_bit16(core, va_rd.data, Ra);
    } else
    {
        if ((((va_rd.data >> 16) & 0x1) == 0) && ((va_rd.data >> 31) & 0x1))
            va_rd.data = 0x8000;
        fpu_put_single_bit16(core, va_rd.data, Ra);
    }

    if (sign_ext)
    {
        core->fp_regs[Ra].Rah = 0xffff;
    } else
        core->fp_regs[Ra].Rah = 0;

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_f32toui16_RaH_RbH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int Ra = BITS(16, 18);
    int Rb = BITS(19, 21);

    s_precision va_rs, va_rd;

    va_rs.data = fpu_get_single(core, Rb);

    va_rd.data = (int) va_rs.value;
    if ((va_rd.data >> 31) & 0x1)
    {
        va_rd.data = 0;
    }
    fpu_put_single_bit16(core, va_rd.data, Ra);

    core->fp_regs[Ra].Rah = 0;

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_f32toi32_RaH_RbH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int Ra = BITS(16, 18);
    int Rb = BITS(19, 21);

    s_precision va_rs, va_rd;

    va_rs.data = fpu_get_single(core, Rb);

    va_rd.data = (int) va_rs.value;

    if (va_rd.data == 0x80000000)
    {
        if (va_rs.data & (0x1 << 31))
        {
            va_rd.data = 0x80000000;
        } else
            va_rd.data = 0x7FFFFFFF;
    }
    fpu_put_single_bit32(core, va_rd.data, Ra);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_f32toui32_RaH_RbH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int Ra = BITS(16, 18);
    int Rb = BITS(19, 21);

    s_precision va_rs, va_rd;

    va_rs.data = fpu_get_single(core, Rb);

    va_rd.data = (unsigned int) va_rs.value;

    if (va_rd.data == 0x80000000)
    {
        if (va_rs.data & (0x1 << 31))
        {
            va_rd.data = 0x80000000;
        } else
            va_rd.data = 0x7FFFFFFF;
    }
    fpu_put_single_bit32(core, va_rd.data, Ra);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_i16tof32_RaH_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int loc = 0;
    int loc32 = TRUE;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);
    volatile uint16_t *reg = NULL;
    uint16_t mem16_data;
    int Ra = BITS(8, 10);

    s_precision va_rd;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        mem16_data = c28_memory_read(core, core->data_addr);
    } else if ((mode_2 == 1) || (mode_2 == 0x2 && mode_3 == 0x7))
    {
        mem16_data = c28_memory_read(core, core->sp_count);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        mem16_data = c28_memory_read(core, core->data_addr);
    } else
        mem16_data = *reg;

    if ((mem16_data >> 15) & 0x1)
    {
        mem16_data = 0xffff - mem16_data + 1;
        va_rd.value = (float) mem16_data;
        va_rd.data = va_rd.data | (0x1 << 31);
        fpu_put_single_bit32(core, va_rd.data, Ra);
    } else
    {
        va_rd.value = (float) mem16_data;
        fpu_put_single_bit32(core, va_rd.data, Ra);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_moviz_RaH_imm16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int Ra = BITS(0, 2);
    int imm16 = BITS(3, 18);

    core->fp_regs[Ra].Rah = imm16;
    core->fp_regs[Ra].Ral = 0;

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_movxi_RaH_imm16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int Ra = BITS(0, 2);
    int imm16 = BITS(3, 18);

    core->fp_regs[Ra].Ral = imm16;

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_mpyf32_RaH_imm16_RbH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int Ra = BITS(0, 2);
    int Rb = BITS(3, 5);
    int imm16 = BITS(6, 21);

    s_precision va_rs1, va_rs2, va_rd;

    va_rs1.data = fpu_get_single(core, Rb);
    va_rs2.data = imm16 << 16;
    va_rd.value = va_rs1.value * va_rs2.value;

    fpu_put_single_bit32(core, va_rd.data, Ra);

    /* Overflow conditions */
    if (va_rd.data == FLOAT_POSITIVE_OVERFLOW)
    {
        SET_BIT(core->stf, LVF);
    }
    /* Underflow conditions */
    if (va_rd.data == NEGATIVE_UNDERFLOW)
    {
        SET_BIT(core->stf, LUF);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_absf32_RaH_RbH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int Ra = BITS(0, 2);
    int Rb = BITS(3, 5);

    if ((core->fp_regs[Rb].Ra >> 31) & 0x1)
    {
        core->fp_regs[Ra].Ra = core->fp_regs[Rb].Ra & (~(0x1 << 31));
    } else
        core->fp_regs[Ra].Ra = core->fp_regs[Rb].Ra;

    CLEAR_BIT(core->stf, ZF);
    CLEAR_BIT(core->stf, NF);

    if (((core->fp_regs[Ra].Ra >> 23) & 0xff) == 0)     /* RaH[30 : 23] */
        SET_BIT(core->stf, ZF);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_cmpf32_RaH_zero(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int Ra = BITS(0, 2);
    double result = 0;

    s_precision va_rs;

    va_rs.data = fpu_get_single(core, Ra);
    result = va_rs.value - 0.000000;

    if (result < 0.0000000001 && result > -0.0000000001)
        result = 0;

    if (result == 0)
    {
        SET_BIT(core->stf, ZF);
        CLEAR_BIT(core->stf, NF);
    } else if (result > 0)
    {
        CLEAR_BIT(core->stf, ZF);
        CLEAR_BIT(core->stf, NF);
    } else if (result < 0)
    {
        SET_BIT(core->stf, NF);
        CLEAR_BIT(core->stf, ZF);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}

static int exec_movst0_flag(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int flag = BITS(0, 7);
    int lvf = bit(core->stf, LVF_BIT);
    int luf = bit(core->stf, LUF_BIT);
    int nf = bit(core->stf, NF_BIT);
    int ni = bit(core->stf, NI_BIT);
    int zf = bit(core->stf, ZF_BIT);
    int zi = bit(core->stf, ZI_BIT);
    int tf = bit(core->stf, TF_BIT);

    if (flag & LVF)
    {
        if (lvf)
        {
            SET(core->st0, V_BIT);
        } else
            CLEAR(core->st0, V_BIT);
    }

    if (flag & LVF)
    {
        if (luf)
        {
            SET(core->st0, V_BIT);
        } else
            CLEAR(core->st0, V_BIT);
    }

    if (flag & NF)
    {
        if (nf)
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);
    }

    if (flag & ZF)
    {
        if (ni)
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);
    }
    if (flag & NI)
    {
        if (zf)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);
    }
    if (flag & ZI)
    {
        if (zi)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);
    }
    if (flag & TEST_FLAG)
    {
        if (tf)
        {
            SET(core->st0, C_BIT);
            SET(core->st0, TC_BIT);
        } else
        {
            CLEAR(core->st0, C_BIT);
            CLEAR(core->st0, TC_BIT);
        }
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}

static int exec_addf32_RaH_imm16_RbH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int Ra = BITS(0, 2);
    int Rb = BITS(3, 5);
    int imm16 = BITS(6, 21);

    s_precision va_rs1, va_rs2, va_rd;

    va_rs1.data = fpu_get_single(core, Rb);
    va_rs2.data = imm16 << 16;

    va_rd.value = va_rs1.value + va_rs2.value;

    fpu_put_single_bit32(core, va_rd.data, Ra);

    /* Overflow conditions */
    if (va_rd.data == FLOAT_POSITIVE_OVERFLOW)
    {
        SET_BIT(core->stf, LVF);
    }
    /* Underflow conditions */
    if (va_rd.data == NEGATIVE_UNDERFLOW)
    {
        SET_BIT(core->stf, LUF);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_i32tof32_RaH_RbH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int Ra = BITS(0, 2);
    int Rb = BITS(3, 5);
    s_precision va_rs, va_rd;

    va_rs.data = fpu_get_single(core, Rb);
    if ((va_rs.data >> 31) & 0x1)
    {
        va_rs.data = 0xffffffff - va_rs.data + 1;
        va_rd.value = (float) va_rs.data;
        va_rd.data = va_rd.data | (0x1 << 31);
        fpu_put_single_bit32(core, va_rd.data, Ra);
    } else
    {
        va_rd.value = (float) va_rs.data;
        fpu_put_single_bit32(core, va_rd.data, Ra);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_ui32tof32_RaH_RbH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int Ra = BITS(0, 2);
    int Rb = BITS(3, 5);
    s_precision va_rs, va_rd;

    va_rs.data = fpu_get_single(core, Rb);
    va_rd.value = (float) va_rs.data;
    fpu_put_single_bit32(core, va_rd.data, Ra);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_i32tof32_RaH_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int loc = 0;
    int loc32 = TRUE;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);
    volatile int *reg = NULL;
    uint32_t mem32_data;
    int Ra = BITS(8, 10);

    s_precision va_rd;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        mem32_data = c28_memory_read(core, core->data_addr) | (c28_memory_read(core, core->data_addr + 1) << 16);
    } else if ((mode_2 == 1) || (mode_2 == 0x2 && mode_3 == 0x7))
    {
        mem32_data = c28_memory_read(core, core->sp_count) | (c28_memory_read(core, core->sp_count + 1) << 16);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        mem32_data = c28_memory_read(core, core->data_addr) | (c28_memory_read(core, core->data_addr + 1) << 16);
    } else
        mem32_data = *reg;

    if ((mem32_data >> 31) & 0x1)
    {
        mem32_data = 0xffffffff - mem32_data + 1;
        va_rd.value = (float) mem32_data;
        va_rd.data = va_rd.data | (0x1 << 31);
        fpu_put_single_bit32(core, va_rd.data, Ra);
    } else
    {
        va_rd.value = (float) mem32_data;
        fpu_put_single_bit32(core, va_rd.data, Ra);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_mpyf32_RaH_RbH_RcH_subf32_RdH_ReH_RfH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int Ra = BITS(0, 2);
    int Rb = BITS(3, 5);
    int Rc = BITS(6, 8);
    int Rd = BITS(9, 11);
    int Re = BITS(12, 14);
    int Rf = BITS(15, 17);

    s_precision va_rs1, va_rs2, va_rd1;
    s_precision va_rs3, va_rs4, va_rd2;

    va_rs1.data = fpu_get_single(core, Rb);
    va_rs2.data = fpu_get_single(core, Rc);
    va_rs3.data = fpu_get_single(core, Re);
    va_rs4.data = fpu_get_single(core, Rf);

    va_rd1.value = va_rs1.value * va_rs2.value;
    fpu_put_single_bit32(core, va_rd1.data, Ra);

    va_rd2.value = va_rs3.value - va_rs4.value;
    fpu_put_single_bit32(core, va_rd2.data, Rd);

    /* Overflow conditions */
    if (va_rd1.data == FLOAT_POSITIVE_OVERFLOW)
    {
        SET_BIT(core->stf, LVF);
    }
    if (va_rd2.data == FLOAT_POSITIVE_OVERFLOW)
    {
        SET_BIT(core->stf, LVF);
    }
    /* Underflow conditions */
    if (va_rd1.data == NEGATIVE_UNDERFLOW)
    {
        SET_BIT(core->stf, LUF);
    }
    if (va_rd2.data == NEGATIVE_UNDERFLOW)
    {
        SET_BIT(core->stf, LUF);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_mpyf32_RaH_RbH_RcH_addf32_RdH_ReH_RfH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int Ra = BITS(0, 2);
    int Rb = BITS(3, 5);
    int Rc = BITS(6, 8);
    int Rd = BITS(9, 11);
    int Re = BITS(12, 14);
    int Rf = BITS(15, 17);

    s_precision va_rs1, va_rs2, va_rd1;
    s_precision va_rs3, va_rs4, va_rd2;

    va_rs1.data = fpu_get_single(core, Rb);
    va_rs2.data = fpu_get_single(core, Rc);
    va_rs3.data = fpu_get_single(core, Re);
    va_rs4.data = fpu_get_single(core, Rf);

    va_rd1.value = va_rs1.value * va_rs2.value;

    fpu_put_single_bit32(core, va_rd1.data, Ra);

    va_rd2.value = va_rs3.value + va_rs4.value;

    fpu_put_single_bit32(core, va_rd2.data, Rd);

    /* Overflow conditions */
    if (va_rd1.data == FLOAT_POSITIVE_OVERFLOW)
    {
        SET_BIT(core->stf, LVF);
    }
    if (va_rd2.data == FLOAT_POSITIVE_OVERFLOW)
    {
        SET_BIT(core->stf, LVF);
    }
    /* Underflow conditions */
    if (va_rd1.data == NEGATIVE_UNDERFLOW)
    {
        SET_BIT(core->stf, LUF);
    }
    if (va_rd2.data == NEGATIVE_UNDERFLOW)
    {
        SET_BIT(core->stf, LUF);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_negf32_RaH_RbH_cond(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int cond = BITS(16, 19);
    int Ra = BITS(0, 2);
    int Rb = BITS(3, 5);
    s_precision va_rs, va_rd;

    va_rs.data = fpu_get_single(core, Rb);

    if (check_float_insn_flag_cond(core, cond) == TRUE)
    {
        va_rd.value = -va_rs.value;
        fpu_put_single_bit32(core, va_rd.data, Ra);
        if ((core->fp_regs[Ra].Ra >> 31) & 0x1)
            SET_BIT(core->stf, NF);
        else
            CLEAR_BIT(core->stf, NF);
    } else
    {
        va_rd.value = va_rs.value;
        fpu_put_single_bit32(core, va_rd.data, Ra);
    }

    /*
     * if (va_rd.data == 0)
     SET_BIT(core->stf, ZF);
     */
    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_addf32_RdH_ReH_RfH_mov32_RaH_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int loc = 0;
    int loc32 = TRUE;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);
    volatile uint32_t *reg = NULL;
    int Ra = BITS(8, 10);
    int Rd = BITS(11, 13);
    int Re = BITS(14, 16);
    int Rf = BITS(17, 19);
    int mem32_data;

    s_precision va_rs1, va_rs2, va_rd;

    va_rs1.data = fpu_get_single(core, Re);
    va_rs2.data = fpu_get_single(core, Rf);
    va_rd.value = va_rs1.value + va_rs2.value;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        mem32_data = c28_memory_read(core, core->data_addr) | (c28_memory_read(core, core->data_addr + 1) << 16);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        mem32_data = c28_memory_read(core, core->sp_count) | (c28_memory_read(core, core->sp_count + 1) << 16);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        mem32_data = c28_memory_read(core, core->data_addr) | (c28_memory_read(core, core->data_addr + 1) << 16);
    } else
        mem32_data = *reg;

    fpu_put_single_bit32(core, va_rd.data, Rd);

    core->fp_regs[Ra].Ra = mem32_data;

    /* Overflow conditions */
    if (va_rd.data == FLOAT_POSITIVE_OVERFLOW)
    {
        SET_BIT(core->stf, LVF);
    }
    /* Underflow conditions */
    if (va_rd.data == NEGATIVE_UNDERFLOW)
    {
        SET_BIT(core->stf, LUF);
    }

    CLEAR_BIT(core->stf, ZF);
    if (core->fp_regs[Ra].Ra & (0x1 << 31))
    {
        SET_BIT(core->stf, NF);
        SET_BIT(core->stf, NI);
    } else
    {
        CLEAR_BIT(core->stf, NF);
        CLEAR_BIT(core->stf, NI);
    }
    if ((core->fp_regs[Ra].Ra & (0xff << 23)) == 0)     /* RaH[30 : 23] */
    {
        SET_BIT(core->stf, ZF);
        CLEAR_BIT(core->stf, NF);
    }
    CLEAR_BIT(core->stf, ZI);

    if (core->fp_regs[Ra].Ra == 0)
        SET_BIT(core->stf, ZI);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_addf32_RdH_ReH_RfH_mov32_loc32_RaH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int loc = 0;
    int loc32 = TRUE;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);
    volatile uint32_t *reg = NULL;
    int Ra = BITS(8, 10);
    int Rd = BITS(11, 13);
    int Re = BITS(14, 16);
    int Rf = BITS(17, 19);
    int mem32_low_data = core->fp_regs[Ra].Ral;
    int mem32_high_data = core->fp_regs[Ra].Rah;
    int mem32_data = core->fp_regs[Ra].Ra;

    s_precision va_rs1, va_rs2, va_rd;

    va_rs1.data = fpu_get_single(core, Re);
    va_rs2.data = fpu_get_single(core, Rf);
    va_rd.value = va_rs1.value + va_rs2.value;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr, mem32_low_data);
        c28_memory_write(core, core->data_addr + 1, mem32_high_data);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count, mem32_low_data);
        c28_memory_write(core, core->sp_count + 1, mem32_high_data);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        c28_memory_write(core, core->data_addr, mem32_low_data);
        c28_memory_write(core, core->data_addr + 1, mem32_high_data);
    } else
        *reg = mem32_data;

    fpu_put_single_bit32(core, va_rd.data, Rd);

    /* Overflow conditions */
    if (va_rd.data == FLOAT_POSITIVE_OVERFLOW)
    {
        SET_BIT(core->stf, LVF);
    }
    /* Underflow conditions */
    if (va_rd.data == NEGATIVE_UNDERFLOW)
    {
        SET_BIT(core->stf, LUF);
    }
    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_subf32_RdH_ReH_RfH_mov32_RaH_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int loc = 0;
    int loc32 = TRUE;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);
    volatile uint32_t *reg = NULL;
    int Ra = BITS(8, 10);
    int Rd = BITS(11, 13);
    int Re = BITS(14, 16);
    int Rf = BITS(17, 19);
    int mem32_data;

    s_precision va_rs1, va_rs2, va_rd;

    va_rs1.data = fpu_get_single(core, Re);
    va_rs2.data = fpu_get_single(core, Rf);
    va_rd.value = va_rs1.value - va_rs2.value;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        mem32_data = c28_memory_read(core, core->data_addr) | (c28_memory_read(core, core->data_addr + 1) << 16);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        mem32_data = c28_memory_read(core, core->sp_count) | (c28_memory_read(core, core->sp_count + 1) << 16);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        mem32_data = c28_memory_read(core, core->data_addr) | (c28_memory_read(core, core->data_addr + 1) << 16);
    } else
        mem32_data = *reg;

    fpu_put_single_bit32(core, va_rd.data, Rd);

    core->fp_regs[Ra].Ra = mem32_data;

    /* Overflow conditions */
    if (va_rd.data == FLOAT_POSITIVE_OVERFLOW)
    {
        SET_BIT(core->stf, LVF);
    }
    /* Underflow conditions */
    if (va_rd.data == NEGATIVE_UNDERFLOW)
    {
        SET_BIT(core->stf, LUF);
    }

    CLEAR_BIT(core->stf, ZF);
    if (core->fp_regs[Ra].Ra & (0x1 << 31))
    {
        SET_BIT(core->stf, NF);
        SET_BIT(core->stf, NI);
    } else
    {
        CLEAR_BIT(core->stf, NF);
        CLEAR_BIT(core->stf, NI);
    }
    if ((core->fp_regs[Ra].Ra & (0xff << 23)) == 0)     /* RaH[30 : 23] */
    {
        SET_BIT(core->stf, ZF);
        CLEAR_BIT(core->stf, NF);
    }
    CLEAR_BIT(core->stf, ZI);

    if (core->fp_regs[Ra].Ra == 0)
        SET_BIT(core->stf, ZI);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}

static int exec_subf32_RdH_ReH_RfH_mov32_loc32_RaH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int loc = 0;
    int loc32 = TRUE;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);
    volatile uint32_t *reg = NULL;
    int Ra = BITS(8, 10);
    int Rd = BITS(11, 13);
    int Re = BITS(14, 16);
    int Rf = BITS(17, 19);
    int mem32_low_data = core->fp_regs[Ra].Ral;
    int mem32_high_data = core->fp_regs[Ra].Rah;
    int mem32_data = core->fp_regs[Ra].Ra;

    s_precision va_rs1, va_rs2, va_rd;

    va_rs1.data = fpu_get_single(core, Re);
    va_rs2.data = fpu_get_single(core, Rf);
    va_rd.value = va_rs1.value - va_rs2.value;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr, mem32_low_data);
        c28_memory_write(core, core->data_addr + 1, mem32_high_data);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count, mem32_low_data);
        c28_memory_write(core, core->sp_count + 1, mem32_high_data);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        c28_memory_write(core, core->data_addr, mem32_low_data);
        c28_memory_write(core, core->data_addr + 1, mem32_high_data);
    } else
        *reg = mem32_data;

    fpu_put_single_bit32(core, va_rd.data, Rd);

    /* Overflow conditions */
    if (va_rd.data == FLOAT_POSITIVE_OVERFLOW)
    {
        SET_BIT(core->stf, LVF);
    }
    /* Underflow conditions */
    if (va_rd.data == NEGATIVE_UNDERFLOW)
    {
        SET_BIT(core->stf, LUF);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_mov32_acc_or_xar_RaH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    int Ra = BITS(16, 23);

    switch (Ra)
    {
        case 0x12:
            Ra = 0;
            break;
        case 0x16:
            Ra = 1;
            break;
        case 0x22:
            Ra = 4;
            break;
        case 0x26:
            Ra = 5;
            break;
        case 0x2a:
            Ra = 6;
            break;
        case 0x2e:
            Ra = 7;
            break;
        default:
            printf("In %s, %d, Decode Error Ra 0x%x at PC:%x\n", __func__, __LINE__, Ra, core->pc);
            break;
    }
    loc = check_insn_address_mode(core, insn, fields, loc32);
    if (loc == 9)
    {
        core->acc = core->fp_regs[Ra].Ra;
    } else if (loc < 8)
    {
        core->xar[loc].xar = core->fp_regs[Ra].Ra;
    } else if (loc == 11)
    {
        core->p = core->fp_regs[Ra].Ra;
    } else if (loc == 12)
    {
        core->xt = core->fp_regs[Ra].Ra;
    }

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_mov32_RaH_acc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    int Ra = BITS(16, 23);

    switch (Ra)
    {
        case 0x12:
            Ra = 0;
            break;
        case 0x16:
            Ra = 1;
            break;
        case 0x22:
            Ra = 4;
            break;
        case 0x26:
            Ra = 5;
            break;
        case 0x2a:
            Ra = 6;
            break;
        case 0x2e:
            Ra = 7;
            break;
        default:
            printf("In %s, %d, Decode Error Ra 0x%x at PC:%x\n", __func__, __LINE__, Ra, core->pc);
            break;
    }

    loc = check_insn_address_mode(core, insn, fields, loc32);
    if (loc == 9)
    {
        core->fp_regs[Ra].Ra = core->acc;
    } else if (loc <= 7)
    {
        core->fp_regs[Ra].Ra = core->xar[loc].xar;
    } else if (loc == 11)
    {
        core->fp_regs[Ra].Ra = core->p;
    } else if (loc == 12)
    {
        core->fp_regs[Ra].Ra = core->xt;
    } else
    {
        printf("In %s, %d, Decode Error Ra 0x%x at PC:%x\n", __func__, __LINE__, Ra, core->pc);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_mpyf32_RdH_ReH_RfH_mov32_RaH_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int loc = 0;
    int loc32 = TRUE;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);
    volatile uint32_t *reg = NULL;
    int Ra = BITS(8, 10);
    int Rd = BITS(11, 13);
    int Re = BITS(14, 16);
    int Rf = BITS(17, 19);
    int mem32_data;

    s_precision va_rs1, va_rs2, va_rd;

    va_rs1.data = fpu_get_single(core, Re);
    va_rs2.data = fpu_get_single(core, Rf);
    va_rd.value = va_rs1.value * va_rs2.value;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        mem32_data = c28_memory_read(core, core->data_addr) | (c28_memory_read(core, core->data_addr + 1) << 16);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        mem32_data = c28_memory_read(core, core->sp_count) | (c28_memory_read(core, core->sp_count + 1) << 16);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        mem32_data = c28_memory_read(core, core->data_addr) | (c28_memory_read(core, core->data_addr + 1) << 16);
    } else
        mem32_data = *reg;

    fpu_put_single_bit32(core, va_rd.data, Rd);

    core->fp_regs[Ra].Ra = mem32_data;

    /* Overflow conditions */
    if (va_rd.data == FLOAT_POSITIVE_OVERFLOW)
    {
        SET_BIT(core->stf, LVF);
    }
    /* Underflow conditions */
    if (va_rd.data == NEGATIVE_UNDERFLOW)
    {
        SET_BIT(core->stf, LUF);
    }

    CLEAR_BIT(core->stf, ZF);
    if (core->fp_regs[Ra].Ra & (0x1 << 31))
    {
        SET_BIT(core->stf, NF);
        SET_BIT(core->stf, NI);
    } else
    {
        CLEAR_BIT(core->stf, NF);
        CLEAR_BIT(core->stf, NI);
    }
    if ((core->fp_regs[Ra].Ra & (0xff << 23)) == 0)     /* RaH[30 : 23] */
    {
        SET_BIT(core->stf, ZF);
        CLEAR_BIT(core->stf, NF);
    }
    CLEAR_BIT(core->stf, ZI);

    if (core->fp_regs[Ra].Ra == 0)
        SET_BIT(core->stf, ZI);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_mpyf32_RdH_ReH_RfH_mov32_loc32_RaH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int loc = 0;
    int loc32 = TRUE;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);
    volatile uint32_t *reg = NULL;
    int Ra = BITS(8, 10);
    int Rd = BITS(11, 13);
    int Re = BITS(14, 16);
    int Rf = BITS(17, 19);
    int mem32_data;

    mem32_data = fpu_get_single(core, Ra);
    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr, mem32_data & 0xffff);
        c28_memory_write(core, core->data_addr + 1, (mem32_data & 0xffff0000) >> 16);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count, mem32_data & 0xffff);
        c28_memory_write(core, core->sp_count + 1, (mem32_data & 0xffff0000) >> 16);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        c28_memory_write(core, core->data_addr, mem32_data & 0xffff);
        c28_memory_write(core, core->data_addr + 1, (mem32_data & 0xffff0000) >> 16);
    } else
        *reg = mem32_data;

    s_precision va_rs1, va_rs2, va_rd;

    va_rs1.data = fpu_get_single(core, Re);
    va_rs2.data = fpu_get_single(core, Rf);
    va_rd.value = va_rs1.value * va_rs2.value;

    fpu_put_single_bit32(core, va_rd.data, Rd);
    /* Overflow conditions */
    if (va_rd.data == FLOAT_POSITIVE_OVERFLOW)
    {
        SET_BIT(core->stf, LVF);
    }
    /* Underflow conditions */
    if (va_rd.data == NEGATIVE_UNDERFLOW)
    {
        SET_BIT(core->stf, LUF);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_eisqrtf32_RaH_RbH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int Ra = BITS(0, 2);
    int Rb = BITS(3, 5);
    uint16_t data_low;
    uint16_t data_high;
    s_precision va_rs, va_rd;
    float Estimate;

    va_rs.data = fpu_get_single(core, Rb);
    Estimate = 1 / sqrt(va_rs.value);

    va_rd.value = Estimate;
    data_low = va_rd.data & 0xffff;
    data_high = (va_rd.data & 0xffff0000) >> 16;
    if ((va_rd.data >> 15) & 0x1)
    {
        data_low = 0;
        data_high += 1;
        va_rd.data = data_low | (data_high << 16);
    }
    fpu_put_single_bit32(core, va_rd.data, Ra);

    /* Overflow conditions */
    if (va_rd.data == FLOAT_POSITIVE_OVERFLOW)
    {
        SET_BIT(core->stf, LVF);
    }
    /* Underflow conditions */
    if (va_rd.data == NEGATIVE_UNDERFLOW)
    {
        SET_BIT(core->stf, LUF);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}

// Repeat A Block of Code 
static int exec_rptb_lable_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int loc = 0;
    int loc32 = TRUE;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);
    int block_lable = BITS(16, 22);
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        core->rb_bit.RC = c28_memory_read(core, core->data_addr);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        core->rb_bit.RC = c28_memory_read(core, core->sp_count);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        core->rb_bit.RC = c28_memory_read(core, core->data_addr);
    } else
        core->rb_bit.RC = *reg;

    core->insn_length = 2;
    core->rb_bit.RA = 1;
    core->rb_bit.RSIZE = block_lable - core->insn_length;
    core->rb_bit.RE = core->pc + block_lable;
    core->rb_ret = core->pc + block_lable;
    core->pc += 2;
    return 0;
}
static int exec_fracf32_RaH_RbH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int Ra = BITS(0, 2);
    int Rb = BITS(3, 5);

    s_precision va_rs, va_rd;
    int temp;

    va_rs.data = fpu_get_single(core, Rb);
    temp = (int) va_rs.value;

    va_rd.value = va_rs.value - temp;

    fpu_put_single_bit32(core, va_rd.data, Ra);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_einvf32_RaH_RbH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int Ra = BITS(0, 2);
    int Rb = BITS(3, 5);
    uint16_t data_low;
    uint16_t data_high;

    s_precision va_rs, va_rd;
    float Estimate;

    va_rs.data = fpu_get_single(core, Rb);
    Estimate = 1 / va_rs.value;

    va_rd.value = Estimate;
    data_low = va_rd.data & 0xffff;
    data_high = (va_rd.data & 0xffff0000) >> 16;
    if ((va_rd.data >> 15) & 0x1)
    {
        data_low = 0;
        data_high += 1;
        va_rd.data = data_low | (data_high << 16);
    }
    fpu_put_single_bit32(core, va_rd.data, Ra);

    /* Overflow conditions */
    if (va_rd.data == FLOAT_POSITIVE_OVERFLOW)
    {
        SET_BIT(core->stf, LVF);
    }
    /* Underflow conditions */
    if (va_rd.data == NEGATIVE_UNDERFLOW)
    {
        SET_BIT(core->stf, LUF);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_cmpf32_RaH_RbH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int Ra = BITS(16, 18);
    int Rb = BITS(19, 21);
    double result = 0;

    s_precision va_rs1, va_rs2;

    va_rs1.data = fpu_get_single(core, Ra);
    va_rs2.data = fpu_get_single(core, Rb);
    result = va_rs1.value - va_rs2.value;

    if (result < 0.0000000001 && result > -0.0000000001)
        result = 0;

    if (result == 0)
    {
        SET_BIT(core->stf, ZF);
        CLEAR_BIT(core->stf, NF);
    } else if (result > 0)
    {
        CLEAR_BIT(core->stf, ZF);
        CLEAR_BIT(core->stf, NF);
    } else if (result < 0)
    {
        SET_BIT(core->stf, NF);
        CLEAR_BIT(core->stf, ZF);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}

static int exec_minf32_RaH_RbH_mov32_RcH_RdH(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int Ra = BITS(0, 2);
    int Rb = BITS(3, 5);
    int Rc = BITS(6, 8);
    int Rd = BITS(9, 11);
    double result = 0;

    s_precision va_rs1, va_rd1;
    s_precision va_rs2, va_rd2;

    va_rs1.data = fpu_get_single(core, Rb);
    va_rd1.data = fpu_get_single(core, Ra);
    va_rs2.data = fpu_get_single(core, Rd);

    result = va_rd1.value - va_rs1.value;
    if (result < 0.0000000001 && result > -0.0000000001)
        result = 0;
    if (result > 0)
    {
        va_rd1.value = va_rs1.value;
        va_rd2.value = va_rs2.value;
        fpu_put_single_bit32(core, va_rd1.data, Ra);
        fpu_put_single_bit32(core, va_rd2.data, Rc);
    }

    if (result == 0)
    {
        SET_BIT(core->stf, ZF);
        CLEAR_BIT(core->stf, NF);
    } else if (result > 0)
    {
        CLEAR_BIT(core->stf, ZF);
        CLEAR_BIT(core->stf, NF);
    } else if (result < 0)
    {
        SET_BIT(core->stf, NF);
        CLEAR_BIT(core->stf, ZF);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_testtf_cond(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int cond = BITS(0, 3);

    if (check_float_insn_flag_cond(core, cond) == TRUE)
        SET_BIT(core->stf, TF);
    else
        CLEAR_BIT(core->stf, TF);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_setflg(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int flag = BITS(11, 21);
    int value = BITS(0, 10);

    switch (flag)
    {
        case 0x1:
        case 0x2:
        case 0x4:
        case 0x8:
        case 0x10:
        case 0x20:
        case 0x40:
        case 0x200:
        case 0x600:
            SET_BIT(core->stf, (value & flag));
            break;
        default:
            printf("In %s, Get Error flag %x\n", __func__, flag);
            break;
    }
    core->insn_length = 2;
    core->pc += 2;
    return 0;
}

static int exec_push_rb(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    c28_memory_write(core, core->sp, core->rbl);
    c28_memory_write(core, core->sp + 1, core->rbh);
    core->sp += 2;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_pop_rb(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    core->sp -= 2;
    core->rbl = c28_memory_read(core, core->sp);
    core->rbh = c28_memory_read(core, core->sp + 1);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_mov32_mem32_stf(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int loc = 0;
    int loc32 = TRUE;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);
    volatile uint32_t *reg = NULL;

    int mem32_data = core->stf;

    int mem32_low_data = core->stf & 0xffff;
    int mem32_high_data = (core->stf & 0xffff0000) >> 16;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr, mem32_low_data);
        c28_memory_write(core, core->data_addr + 1, mem32_high_data);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count, mem32_low_data);
        c28_memory_write(core, core->sp_count + 1, mem32_high_data);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        c28_memory_write(core, core->data_addr, mem32_low_data);
        c28_memory_write(core, core->data_addr + 1, mem32_high_data);
    } else
        *reg = mem32_data;

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}

static int exec_mov32_stf_mem32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int loc = 0;
    int loc32 = TRUE;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);
    volatile uint32_t *reg = NULL;

    int mem32_data = 0;
    int mem32_low_data = 0;
    int mem32_high_data = 0;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if (mode_2 == 0)
    {
        mem32_low_data = c28_memory_read(core, core->data_addr);
        mem32_high_data = c28_memory_read(core, core->data_addr + 1);
        core->stf = mem32_low_data & 0xffff | (mem32_high_data & 0xffff << 16);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        mem32_low_data = c28_memory_read(core, core->sp_count);
        mem32_high_data = c28_memory_read(core, core->sp_count + 1);
        core->stf = mem32_low_data & 0xffff | (mem32_high_data & 0xffff << 16);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        c28_memory_read(core, core->data_addr);
        c28_memory_read(core, core->data_addr + 1);
        core->stf = mem32_low_data & 0xffff | (mem32_high_data & 0xffff << 16);
    } else
        core->stf = *reg;

    core->insn_length = 2;
    core->pc += 2;
}

static int exec_cmpf32_RaH_imm16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int Ra = BITS(0, 2);
    int imm16 = BITS(3, 18);
    double result = 0;

    s_precision va_rs, va_imm;

    va_rs.data = fpu_get_single(core, Ra);

    va_imm.data = imm16 << 16;
    result = va_rs.value - va_imm.value;

    if (result < 0.0000000001 && result > -0.0000000001)
        result = 0;

    if (result == 0)
    {
        SET_BIT(core->stf, ZF);
        CLEAR_BIT(core->stf, NF);
    } else if (result > 0)
    {
        CLEAR_BIT(core->stf, ZF);
        CLEAR_BIT(core->stf, NF);
    } else if (result < 0)
    {
        SET_BIT(core->stf, NF);
        CLEAR_BIT(core->stf, ZF);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_subr_loc16_ax(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int ax = fields->a8;
    uint16_t data = 0, y;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    int x = bit(*reg, 15);

    if (ax == 0)
    {
        if (mode_2 == 0)
        {
            data = c28_memory_read(core, core->data_addr);
            y = bit(data, 15);
            data = core->al - data;
            c28_memory_write(core, core->data_addr, data);
        } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
        {
            data = c28_memory_read(core, core->sp_count);
            y = bit(data, 15);
            data = core->al - data;
            c28_memory_write(core, core->sp_count, data);
        } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
        {
            data = c28_memory_read(core, core->data_addr);
            y = bit(data, 15);
            data = core->al - data;
            c28_memory_write(core, core->data_addr, data);
        } else
        {
            y = bit(*reg, 15);
            data = core->al - *reg;
            *reg = data;
        }
    } else
    {
        if (mode_2 == 0)
        {
            data = c28_memory_read(core, core->data_addr);
            y = bit(data, 15);
            data = core->ah - data;
            c28_memory_write(core, core->data_addr, data);
        } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
        {
            data = c28_memory_read(core, core->sp_count);
            y = bit(data, 15);
            data = core->ah - data;
            c28_memory_write(core, core->data_addr, data);
        } else if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
        {
            data = c28_memory_read(core, core->data_addr);
            y = bit(data, 15);
            data = core->ah - data;
            c28_memory_write(core, core->data_addr, data);
        } else
        {
            y = bit(*reg, 15);
            data = core->ah - *reg;
            *reg = data;
        }
    }

    int z = bit(data, 15);

    /* Zero conditions */
    if (data == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (data & (0x1 << 15))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Carry conditions */
    if ((x && y && z) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);

    /* Overflow conditions */
    if (data == NEGATIVE_OVERFLOW_BIT16)
        SET(core->st0, V_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_mov_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int imm16 = fields->imm16;
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr, imm16);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count, imm16);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        c28_memory_write(core, core->data_addr, imm16);
    } else
    {
        if (loc == 8)
        {
            core->ah = imm16;
            /* Zero conditions */
            if (core->ah == 0)
            {
                SET(core->st0, Z_BIT);
            } else
                CLEAR(core->st0, Z_BIT);

            /* Negative conditions */
            if (core->ah & (0x1 << 15))
            {
                SET(core->st0, N_BIT);
            } else
                CLEAR(core->st0, N_BIT);
        } else if (loc == 9)
        {
            core->al = imm16;
            /* Zero conditions */
            if (core->al == 0)
            {
                SET(core->st0, Z_BIT);
            } else
                CLEAR(core->st0, Z_BIT);

            /* Negative conditions */
            if (core->al & (0x1 << 15))
            {
                SET(core->st0, N_BIT);
            } else
                CLEAR(core->st0, N_BIT);
        } else if (loc == 10)
        {
            core->ph = imm16;
        } else if (loc == 11)
        {
            core->pl = imm16;
        } else if (loc == 12)
        {
            core->th = imm16;
        } else
            *reg = imm16;
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_movl_p_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        int addr_1 = c28_memory_read(core, core->data_addr);
        int addr_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (addr_2 << 16) | addr_1;
        core->p = *reg;
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        int data1 = c28_memory_read(core, core->sp_count);
        int data2 = c28_memory_read(core, core->sp_count + 1);

        *reg = (data2 << 16) | data1;
        core->p = *reg;
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr + 1);
        *reg = (*reg << 16) | c28_memory_read(core, core->data_addr);
        core->p = *reg;
    } else
        core->p = *reg;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_lsl_ax_t(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int ax = BITS(0, 0);
    int shift = core->th & 0xf;
    int carry;

    if (ax == 0)
    {
        carry = core->al & (0x1 << (0x10 - shift));
        core->al = core->al << shift;
        /* Negative conditions */
        if (core->al & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Zero conditions */
        if (core->al == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);
        /* Carry conditions */
        if (shift == 0)
        {
            CLEAR(core->st0, C_BIT);
        } else
        {
            if (carry)
            {
                SET(core->st0, C_BIT);
            } else
                CLEAR(core->st0, C_BIT);
        }
    } else
    {
        carry = core->ah & (0x1 << (0x10 - shift));
        core->ah = core->ah << shift;
        /* Negative conditions */
        if (core->ah & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Zero conditions */
        if (core->ah == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);
        /* Carry conditions */
        if (shift == 0)
        {
            CLEAR(core->st0, C_BIT);
        } else
        {
            if (carry)
            {
                SET(core->st0, C_BIT);
            } else
                CLEAR(core->st0, C_BIT);
        }
    }
    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_lsr_ax_t(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int ax = BITS(0, 0);
    int shift = core->th & 0xf;
    int carry;

    if (ax == 0)
    {
        carry = core->al & (0x1 << shift);
        core->al = core->al >> shift;
        /* Negative conditions */
        if (core->al & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Zero conditions */
        if (core->al == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);
        /* Carry conditions */
        if (shift == 0)
        {
            CLEAR(core->st0, C_BIT);
        } else
        {
            if (carry)
            {
                SET(core->st0, C_BIT);
            } else
                CLEAR(core->st0, C_BIT);
        }
    } else
    {
        carry = core->ah & (0x1 << shift);
        core->ah = core->ah >> shift;
        /* Negative conditions */
        if (core->ah & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Zero conditions */
        if (core->ah == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);
        /* Carry conditions */
        if (shift == 0)
        {
            CLEAR(core->st0, C_BIT);
        } else
        {
            if (carry)
            {
                SET(core->st0, C_BIT);
            } else
                CLEAR(core->st0, C_BIT);
        }
    }
    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_spm_pm(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int mode = fields->mode0_3;

    switch (mode)
    {
        case 8:
            core->st0 |= 0xfe7f;
            break;
        case 9:
            core->st0 |= (0x1 << 7);
            break;
        case 0xa:
            core->st0 |= (0x1 << 8);
            break;
        case 0xb:
            core->st0 |= (0x3 < 7);
            break;
        case 0xc:
            core->st0 |= (0x1 << 9);
            break;
        case 0xd:
            core->st0 |= (0x5 << 7);
            break;
        case 0xe:
            core->st0 |= (0x6 << 7);
            break;
        case 0xf:
            core->st0 |= (0x7 << 7);
            break;
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_c28obj(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    SET(core->st1, OBJMODE_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_setc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    check_insn_status_cond(core, insn, fields);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_clrc_amode(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    CLEAR(core->st1, AMODE_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_c28map(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    SET(core->st1, M0M1MAP_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_clrc_mode(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    check_insn_status_cond(core, insn, fields);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_clrc_ovc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    check_insn_status_cond(core, insn, fields);

    // OVC = st0 & 0xFC00 >> 10;
    core->st0 = core->st0 & 0x3FF;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_movw_dp(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int imm16 = fields->imm16;

    core->dp = imm16;

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_asp(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if ((core->sp & 0x1) == 1)
    {
        core->sp += 1;
        SET(core->st1, SPA_BIT);
    } else
        CLEAR(core->st1, SPA_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_eallow(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    SET(core->st1, EALLOW_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_addb_acc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int imm8 = fields->cond0_7;
    int x = bit(core->acc, 31);

    core->acc = core->acc + imm8;
    int z = bit(core->acc, 31);

    judge_flags_modes(core, insn);
    /* Carry conditions */
    if (x && (z == 0))
    {
        SET(core->st0, C_BIT);
    } else
        CLEAR(core->st0, C_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_sb(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int cond = fields->cond8_11;

    check_insn_flag_cond(core, insn, fields);

    if (cond == 0xa || cond == 0xb)
    {
        CLEAR(core->st0, V_BIT);
    }

    return 0;
}
static int exec_movl_loc32_p(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr, core->pl);
        c28_memory_write(core, core->data_addr + 1, core->ph);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count, core->pl);
        c28_memory_write(core, core->sp_count + 1, core->ph);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        c28_memory_write(core, core->data_addr, core->pl);
        c28_memory_write(core, core->data_addr + 1, core->ph);
    } else
        *reg = core->p;

    if (loc == 9)
    {
        /* Zero conditions */
        if (*reg == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (*reg & (0x1 << 31))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);
    }
    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_movl_xar_imm22(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op = fields->op8_15;
    int mode = fields->mode6_7;
    int imm22 = fields->bit6 << 16 | fields->imm16;

    if (op == 0x8d)
    {
        core->xar[mode].xar = imm22;
    } else if (op == 0x8f)
    {
        if (mode == 0)
        {
            core->xar[4].xar = imm22;
        } else if (mode == 1)
        {
            core->xar[5].xar = imm22;
        }
    } else if (op == 0x76)
    {
        if (mode == 2)
        {
            core->xar[6].xar = imm22;
        } else if (mode == 3)
        {
            core->xar[7].xar = imm22;
        }
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}

static int exec_pread_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int value;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile uint16_t *reg = NULL;

    value = c28_memory_read(core, core->xar[7].xar);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr, value);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count, value);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        c28_memory_write(core, core->data_addr, value);
    } else
        *reg = value;

    if (loc < 8)
    {
        uint16_t data = c28_memory_read(core, core->xar[7].xar + core->pce1);

        c28_memory_write(core, core->xar[loc].xar - 1, data);
        core->pce1 += 1;
    }

    if ((loc == 8) || (loc == 9))
    {
        /* Zero conditions */
        if (*reg == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (*reg & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);
    }
    if (core->rptc == 0)
    {
        core->pce1 = 0;
        core->insn_length = 1;
        core->pc += 1;
        return 0;
    } else
    {
        while (core->rptc)
        {
            core->rptc--;
            exec_pread_loc16(core, insn, fields);
        }
    }
}
static int exec_movz_arn(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;
    int op1 = fields->op11_15;
    int op2 = fields->op8_15;
    int mode = fields->mode8_10;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    if (op1 == 0xb)
    {
        core->xar[mode].ar = *reg;
        core->xar[mode].arh = 0;
    } else if (op2 == 0x88)
    {
        core->xar[6].ar = *reg;
        core->xar[6].arh = 0;
    } else if (op2 == 0x80)
    {
        core->xar[7].ar = *reg;
        core->xar[7].arh = 0;
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_addb_or_subb_xar(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op = fields->b;
    int mode = fields->mode8_10;
    int imm7 = fields->offset0_6;

    if (op == 0)
    {
        core->xar[mode].xar += imm7;
    } else
        core->xar[mode].xar -= imm7;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_banz(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int mode = fields->mode0_2;
    int imm16 = fields->imm16;

    imm16 = SIGN_EXTEND(imm16, 16);
    if (core->xar[mode].ar != 0)
    {
        core->pc += imm16;
        core->xar[mode].ar -= 1;
    } else
    {
        core->xar[mode].ar -= 1;
        core->pc += 2;
    }
    core->insn_length = 2;
    return 0;
}
static int exec_movb_ax(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op = fields->a8;
    int imm8 = fields->offset0_7;

    if (op == 0)
    {
        core->al = imm8;
        if (core->al == 0)
            SET(core->st0, Z_BIT);
        else
            CLEAR(core->st0, Z_BIT);
    } else
    {
        core->ah = imm8;
        if (core->ah == 0)
            SET(core->st0, Z_BIT);
        else
            CLEAR(core->st0, Z_BIT);
    }

    CLEAR(core->st0, N_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_movl_loc32_acc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr, core->al);
        c28_memory_write(core, core->data_addr + 1, core->ah);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count, core->al);
        c28_memory_write(core, core->sp_count + 1, core->ah);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        c28_memory_write(core, core->data_addr, core->al);
        c28_memory_write(core, core->data_addr + 1, core->ah);
    } else
        *reg = core->acc;

    if (loc == 9)
    {
        /* Zero conditions */
        if (*reg == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (*reg & (0x1 << 31))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);
    }
    core->insn_length = 1;
    core->pc += 1;
    return 0;
}

/*   rpt instruction
 * Repeat the next instruction. An internal repeat counter (RPTC) is loaded with
 * a value N that is either the specified #8bit constant value or the content of
 * the location pointed to by the “loc16” addressing mode. After the instruction
 * that follows the RPT is executed once, it is repeated N times; that is, the
 * instruction following the RPT executes N + 1 times.
 */
static int exec_rpt(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op = fields->op8_15;
    int imm8 = fields->offset0_7;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;

    if (op == 0xf6)
    {
        core->rptc = imm8;
    } else
    {
        loc = check_insn_address_mode(core, insn, fields, loc32);
        reg = get_register(core, insn, loc);
        core->rptc = *reg;
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}

static int exec_movl_acc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    volatile int *reg = NULL;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if (mode_2 == 0)
    {
        int addr_1 = c28_memory_read(core, core->data_addr);
        int addr_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (addr_2 << 16) | addr_1;
    }

    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        int data1 = c28_memory_read(core, core->sp_count);
        int data2 = c28_memory_read(core, core->sp_count + 1);

        *reg = (data2 << 16) | data1;
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg = *reg | (c28_memory_read(core, core->data_addr + 1) << 16);
    }

    core->acc = *reg;

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}

static int exec_add_acc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op1 = fields->op8_15;
    int op2 = fields->op0_15;
    int shift = fields->shift24_27;
    int mode_1, mode_2, mode_3;
    int loc = 0;
    int loc32 = FALSE;
    int y = 0;
    volatile uint16_t *reg = NULL;

    if (op2 == 0x5604)
    {
        insn = insn >> 16;
        mode_1 = BITS(0, 7);
        mode_2 = BITS(6, 7);
        mode_3 = BITS(3, 5);
    } else
    {
        mode_1 = BITS(0, 7);
        mode_2 = BITS(6, 7);
        mode_3 = BITS(3, 5);
    }

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }

    int x = bit(core->acc, 31);

    if (op1 == 0x81)
    {
        core->acc += *reg;
        y = bit(*reg, 31);
    } else if (op1 == 0x05)
    {
        core->acc += (*reg << 16);
        y = bit(*reg << 16, 31);
    } else if (op2 == 0x5604)
    {
        core->acc += (*reg) << shift;
        y = bit(*reg << shift, 31);
    }
    int z = bit(core->acc, 31);

    judge_flags_modes(core, insn);
    /* Carry conditions */
    //if ((x && y && !z) || (!x && !y && z)){
    if ((x || y) && !z)
    {
        SET(core->st0, C_BIT);
    } else
        CLEAR(core->st0, C_BIT);

    if ((op1 == 0x81) || (op1 == 0x05) || (op2 == 0x5604))
    {
        if (core->rptc == 0)
        {
            if (op2 == 0x5604)
            {
                core->insn_length = 2;
                core->pc += 2;
            } else
            {
                core->insn_length = 1;
                core->pc += 1;
            }
        } else
        {
            while (core->rptc--)
            {
                return 0;
            }
        }
    } else
    {
        core->pc += 1;
        core->insn_length = 1;
    }
    return 0;
}
static int exec_add_acc_loc16_t(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int a = fields->a8;
    int mode_1 = fields->cond0_7;
    int mode_2 = BITS(22, 23);
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    int sxm = (core->st0 & 0x1);
    volatile uint16_t *reg = NULL;
    int temp = insn & 0xffff;

    insn = (insn >> 16) & 0xffff;
    insn |= (temp << 16) & 0xffff0000;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }

    int shift = (core->xt >> 16) & 0xffff;

    if (sxm)
    {
        core->acc += (*reg << shift) | ((*reg & (0x1 << 31)) << 31);
    } else
    {
        core->acc += *reg << shift;
    }
    core->pc += 2;
    core->insn_length = 2;

}
static int exec_addl_acc_p_pm(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int pm = (core->st0 >> PM_BIT) & 0x7;
    int x = bit(core->acc, 31);
    int y = 0;

    switch (pm)
    {
        case 0:
            //left shift 1
            core->acc += (core->p) << 1;
            y = bit((core->p) << 1, 31);
            break;
        case 1:
            //No shift
            core->acc += core->p;
            y = bit(core->p, 31);
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            //Right shift by (pm - 1)
            core->acc += (core->p) >> (pm - 1);
            y = bit(((core->p) >> (pm - 1)), 31);
            break;
        default:
            printf("In %s, %d, Not implement this instruction at pm = %x\n", __func__, __LINE__, pm);
            break;
    }
    judge_flags_modes(core, insn);
    // Z N
    if (core->acc >> 31)
    {
        SET(core->st0, Z_BIT);
        SET(core->st0, N_BIT);
    } else
    {
        CLEAR(core->st0, Z_BIT);
        CLEAR(core->st0, N_BIT);
    }
    //C
    int z = bit(core->acc, 31);

    /* Carry conditions */
    if ((x || y) && !z)
    {
        SET(core->st0, C_BIT);
    } else
        CLEAR(core->st0, C_BIT);
    core->pc += 1;
    core->insn_length = 1;
}
static int exec_adrk_8bit(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int imm8 = fields->offset0_7;
    int ARP = (core->st1 >> 13) & 0x7;

    core->xar[ARP].xar += imm8;
    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_and_acc_16bit_shift(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op = BITS(12, 15);
    int imm16 = fields->imm16;
    int shift_value = fields->shift0_3;

    if (op == 0x3)
    {
        core->acc &= imm16 << shift_value;
    } else
    {
        core->acc &= imm16 << 16;
    }
    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_c27map(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    CLEAR(core->st1, M0M1MAP_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_c27obj(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    CLEAR(core->st1, OBJMODE_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_clrc_xf(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    core->st1 &= ~(0x1 << 12);
    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_cmpl_acc_p_pm(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int result;
    int pm = (core->st0 >> PM_BIT) & 0x7;
    int p_pm = 0;

    switch (pm)
    {
        case 0:
            //left shift 1
            p_pm = (core->p) << 1;
            break;
        case 1:
            //No shift
            p_pm = core->p;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            //Right shift by (pm - 1)
            p_pm = (core->p) >> (pm - 1);
            break;
        default:
            printf("In %s, %d, Not implement this instruction at pm = %x\n", __func__, __LINE__, pm);
            break;
    }

    result = core->acc - p_pm;
    int x = bit(core->acc, 31);
    int y = bit(p_pm, 31);
    int z = bit(result, 31);

    /* Zero conditions */
    if (result == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (z)
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Carry conditions */
    if ((x && y && !z && (result != 0)) || (!x && !y && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_lpaddr(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    //set amode in st1[8]
    core->st1 |= 1 << 8;
    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_cmpr(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op = BITS(0, 3);
    int ARP = (core->st1 >> 13) & 0x7;

    if (op == 0xd)
    {                                   //0
        if (core->xar[0].xar == core->xar[ARP].xar)
        {
            SET(core->st0, TC_BIT);
        } else
        {
            CLEAR(core->st0, TC_BIT);
        }
    } else if (op == 0x9)
    {                                   //1
        if (core->xar[0].xar > core->xar[ARP].xar)
        {
            SET(core->st0, TC_BIT);
        } else
        {
            CLEAR(core->st0, TC_BIT);
        }
    } else if (op == 0x8)
    {                                   //2
        if (core->xar[0].xar > core->xar[ARP].xar)
        {
            SET(core->st0, TC_BIT);
        } else
        {
            CLEAR(core->st0, TC_BIT);
        }
    } else if (op == 0xc)
    {                                   //3
        if (core->xar[0].xar != core->xar[ARP].xar)
        {
            SET(core->st0, TC_BIT);
        } else
        {
            CLEAR(core->st0, TC_BIT);
        }
    } else
    {
        printf("op error in instr: %s\n", __FUNCTION__);
    }
    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
int ReverseBits(int num)
{
    int ret = 0;
    int i;

    for (i = 0; i < 16; ++i)
    {
        ret <<= 1;
        ret |= num & 1;
        num >>= 1;
    }
    return ret;
}
static int exec_flip_ax(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op = BITS(0, 3);

    if (op == 0)
    {                                   //AL
        core->al = ReverseBits(core->al);
    } else if (op == 1)
    {                                   //AH
        core->ah = ReverseBits(core->ah);
    } else
    {
        printf("op error in instr: %s\n", __FUNCTION__);
    }
    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_imacl_p_loc32_xar7(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op = BITS(24, 31);
    int loc = 0;
    int loc32 = TRUE;
    int result;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int *reg = NULL;
    int pm = (core->st0 >> PM_BIT) & 0x7;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    int x = bit(core->acc, 31);

    core->acc += core->p;
    int z = bit(core->acc, 31);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg |= c28_memory_read(core, core->sp_count + 1) << 16;
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }

    if (op == 0xc7)
    {                                   //*xar7
        result = (*reg) * c28_memory_read(core, core->xar[7].xar);
    } else if (op == 0x87)
    {                                   //*xar7++
        result = (*reg) * c28_memory_read(core, ((core->xar[7].xar) + 1));
    } else
    {
        printf("op error in instr: %s\n", __FUNCTION__);
    }
    switch (pm)
    {
        case 0x1:                      // no shift
            core->p = result;
            break;
        default:
            printf("In %s, %d, Not implement this instruction at pm = %x\n", __func__, __LINE__, pm);
            break;
    }
    if (core->acc == 0)
        SET(core->st0, Z_BIT);
    else
        CLEAR(core->st0, Z_BIT);
    if (core->acc >> 31)
        SET(core->st0, N_BIT);
    else
        CLEAR(core->st0, N_BIT);
    judge_flags_modes(core, insn);
    /* Carry conditions */
    if (x && (z == 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_impyal_p_xt_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    int result;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int *reg = NULL;
    int pm = (core->st0 >> PM_BIT) & 0x7;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    int x = bit(core->acc, 31);

    core->acc += core->p;
    int z = bit(core->acc, 31);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg |= c28_memory_read(core, core->sp_count + 1) << 16;
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }

    result = (core->xt) * (*reg);
    switch (pm)
    {
        case 0x1:                      // no shift
            core->p = result;
            break;
        default:
            printf("In %s, %d, Not implement this instruction at pm = %x\n", __func__, __LINE__, pm);
            break;
    }

    if (core->acc == 0)
        SET(core->st0, Z_BIT);
    else
        CLEAR(core->st0, Z_BIT);
    if (core->acc >> 31)
        SET(core->st0, N_BIT);
    else
        CLEAR(core->st0, N_BIT);
    judge_flags_modes(core, insn);
    /* Carry conditions */
    if (x && (z == 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_impysl_p_xt_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    int result;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int *reg = NULL;
    int pm = (core->st0 >> PM_BIT) & 0x7;

    insn = insn >> 16;
    mode_2 = BITS(6, 7);
    mode_3 = BITS(3, 5);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    int x = bit(core->acc, 31);

    core->acc -= core->p;
    int z = bit(core->acc, 31);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg |= c28_memory_read(core, core->sp_count + 1) << 16;
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }

    result = (core->xt) * (*reg);
    switch (pm)
    {
        case 0x1:                      // no shift
            core->p = result;
            break;
        default:
            printf("In %s, %d, Not implement this instruction at pm = %x\n", __func__, __LINE__, pm);
            break;
    }

    if (core->acc == 0)
        SET(core->st0, Z_BIT);
    else
        CLEAR(core->st0, Z_BIT);
    if (core->acc >> 31)
        SET(core->st0, N_BIT);
    else
        CLEAR(core->st0, N_BIT);
    judge_flags_modes(core, insn);
    /* Carry conditions */
    if (x && (z == 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_loopnz_loc16_imm16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;
    int imm16 = fields->imm16;
    int cond = 0;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    cond = (*reg) & (imm16);
    while (cond != 0)
    {
        ;                               //Loop while not zero
    }
    if (cond >> 15)
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);
    if (cond == 0)
    {
        SET(core->st0, Z_BIT);
        core->st1 |= 1 << 5;            // LOOP bit 
    } else
    {
        CLEAR(core->st0, Z_BIT);
        core->st1 &= ~(1 << 5);         // LOOP bit
    }
    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_loopz_loc16_imm16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;
    int imm16 = fields->imm16;
    int cond = 0;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    cond = (*reg) & (imm16);
    while (cond == 0)
    {
        ;                               //Loop while not zero
    }
    if (cond >> 15)
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);
    if (cond == 0)
    {
        SET(core->st0, Z_BIT);
        core->st1 |= 1 << 5;            // LOOP bit 
    } else
    {
        CLEAR(core->st0, Z_BIT);
        core->st1 &= ~(1 << 5);         // LOOP bit
    }
    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_lret(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{

    core->sp -= 1;
    int sp_data = c28_memory_read(core, core->sp);
    int result = (sp_data << 16) & 0x3fffff;

    core->sp -= 1;
    sp_data = c28_memory_read(core, core->sp);
    result |= sp_data & 0xffff;

    core->insn_length = 1;
    core->pc = result & 0x3fffff;

    return 0;
}
static int exec_lrete(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{

    core->sp -= 1;
    int sp_data = c28_memory_read(core, core->sp);
    int result = (sp_data << 16) & 0x3fffff;

    core->sp -= 1;
    sp_data = c28_memory_read(core, core->sp);
    result |= sp_data & 0xffff;
    core->st1 &= ~1;                    // INTM = 0
    core->insn_length = 1;
    core->pc = result & 0x3fffff;

    return 0;
}
static int exec_lsr64_acc_p_shift(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int shift = BITS(0, 3) + 1;
    uint64_t temp64_value;
    int carry = core->acc & (0x1 << (0x20 - shift));

    temp64_value = ((((uint64_t) core->acc) << 32) | core->p) >> shift;
    core->acc = temp64_value >> 32;
    core->p = temp64_value & 0xFFFFFFFF;

    /* Zero conditions */
    if (temp64_value == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Carry conditions */
    if (carry == 1)
    {
        SET(core->st0, C_BIT);
    } else
        CLEAR(core->st0, C_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_mac_p_loc16_xar7(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int xar7 = 0;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;

    insn = insn >> 16;
    mode_2 = BITS(6, 7);
    mode_3 = BITS(3, 5);
    int op = BITS(12, 15);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if (op == 0xc)
    {                                   //*xar7
        xar7 = c28_memory_read(core, core->xar[7].xar);
    } else if (op == 0x8)
    {                                   //*xar7++
        xar7 = c28_memory_read(core, ((core->xar[7].xar) + 1));
    } else
    {
        printf("op error in instr: %s\n", __FUNCTION__);
    }
    int pm = (core->st0 >> PM_BIT) & 0x7;
    int p_pm = 0;

    switch (pm)
    {
        case 0:
            p_pm = (core->p << 1) & 0xFFFFFFFF;
            break;
            //left shift 1
        case 1:
            p_pm = (core->p) & 0xFFFFFFFF;
            break;
            //No shift
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            p_pm = (core->p >> (pm - 1)) & 0xFFFFFFFF;
            break;
            //Right shift by (pm - 1)
        default:
            printf("In %s, %d, Not implement this instruction at pm = %x\n", __func__, __LINE__, pm);
            break;
    }
    core->acc += p_pm;
    core->th = *reg;
    core->p = (*reg) * xar7;

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_mov_dp_imm10(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int imm10 = fields->offset0_9;

    int dp_bit15_10 = core->dp >> 10;

    core->dp = imm10 & 0x3ff;
    core->dp |= dp_bit15_10;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_mov_loc16_ier(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile uint16_t *reg = NULL;
    int data;

    data = core->ier;
    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr, data);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count, data);
    } else if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        c28_memory_write(core, core->data_addr, data);
    } else
    {
        *reg = data;
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_mov_loc16_ovc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = insn >> 16;
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;

    mode_2 = BITS(6, 7);
    int mode_3 = fields->mode3_5;
    volatile uint16_t *reg = NULL;
    int data = 0;
    int ovc = (core->st0 & 0xfc00);

    data = ovc << 10;
    data = data & (~0x3ff);
    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr, data);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count, data);
    } else if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        c28_memory_write(core, core->data_addr, data);
    } else
    {
        *reg = data;
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_mov_tl_0(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    core->xt &= ~0xf;
    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_mov_xarn_pc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int n = BITS(0, 2);

    core->xar[n].xar = core->pc;
    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_mova_t_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    int pm = (core->st0 >> PM_BIT) & 0x7;
    int p_pm = 0;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    core->xt |= (*reg << 16);
    switch (pm)
    {
        case 0:
            //left shift 1
            p_pm = (core->p) << 1;
            break;
        case 1:
            //No shift
            p_pm = core->p;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            //Right shift by (pm - 1)
            p_pm = (core->p) >> (pm - 1);
            break;
        default:
            printf("In %s, %d, Not implement this instruction at pm = %x\n", __func__, __LINE__, pm);
            break;
    }
    int x = bit(core->acc, 31);

    core->acc += p_pm;
    int y = bit(p_pm, 31);
    int z = bit(core->acc, 31);

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (z)
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Carry conditions */
    if ((x && y && !z && (core->acc != 0)) || (!x && !y && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_movl_acc_p_pm(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int pm = (core->st0 >> PM_BIT) & 0x7;
    int p_pm = 0;

    switch (pm)
    {
        case 0:
            //left shift 1
            p_pm = (core->p) << 1;
            break;
        case 1:
            //No shift
            p_pm = core->p;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            //Right shift by (pm - 1)
            p_pm = (core->p) >> (pm - 1);
            break;
        default:
            printf("In %s, %d, Not implement this instruction at pm = %x\n", __func__, __LINE__, pm);
            break;
    }
    core->acc = p_pm;
    if (core->acc > 31)
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_movl_loc32_acc_cond(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int *reg = NULL;
    int cond = 0;
    int condition = BITS(24, 27);

    switch (condition)
    {
        case 0:                        //NEQ Z=0
            if (((core->st0 >> 4) & 0x1) == 0)
            {
                cond = 1;
            }
            break;
        case 1:                        //EQ Z=1
            if ((core->st0 >> 4) & 0x1)
                cond = 1;
            break;
        case 2:                        //GT Z=0 N=0
            if ((core->st0 >> 4) & 0x2 == 0)
                cond = 1;
            break;
        case 3:                        //GEQ N=0
            if ((core->st0 >> 5) & 0x1 == 0)
                cond = 1;
            break;
        case 4:                        //LT N=1
            if ((core->st0 >> 5) & 0x1)
                cond = 1;
            break;
        case 5:                        //LEQ Z=1 or N=1
            if (((core->st0 >> 4) & 0x1) || ((core->st0 >> 5) & 0x1))
                cond = 1;
            break;
        case 6:                        //HI C=1 and Z=0
            if (((core->st0 >> 3) & 0x1) && ((core->st0 >> 4) & 0x1 == 0))
                cond = 1;
            break;
        default:
            skyeye_log(Error_log, __func__, "not implement the condition for this instruction.\n");
            break;
    }

    if (cond)
    {
        insn = insn >> 16;
        mode_2 = BITS(6, 7);
        mode_3 = BITS(3, 5);
        loc = check_insn_address_mode(core, insn, fields, loc32);
        reg = get_register(core, insn, loc);
        if (mode_2 == 0)
        {
            c28_memory_write(core, core->data_addr, core->al);
            c28_memory_write(core, core->data_addr + 1, core->ah);
        } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
        {
            c28_memory_write(core, core->sp_count, core->al);
            c28_memory_write(core, core->sp_count + 1, core->ah);
        } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
        {
            c28_memory_write(core, core->data_addr, core->al);
            c28_memory_write(core, core->data_addr + 1, core->ah);
        } else
            *reg = core->acc;
    }
    if (loc == 9)
    {
        /* Zero conditions */
        if (*reg == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (*reg & (0x1 << 31))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);
    }
    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_movp_t_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    int pm = (core->st0 >> PM_BIT) & 0x7;
    int p_pm = 0;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    core->xt |= (*reg << 16);
    switch (pm)
    {
        case 0:
            //left shift 1
            p_pm = (core->p) << 1;
            break;
        case 1:
            //No shift
            p_pm = core->p;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            //Right shift by (pm - 1)
            p_pm = (core->p) >> (pm - 1);
            break;
        default:
            printf("In %s, %d, Not implement this instruction at pm = %x\n", __func__, __LINE__, pm);
            break;
    }
    int x = bit(core->acc, 31);

    core->acc = p_pm;
    int y = bit(p_pm, 31);
    int z = bit(core->acc, 31);

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (z)
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_movu_ovc_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;

    insn = insn >> 16;
    loc = check_insn_address_mode(core, insn, fields, loc32);

    reg = get_register(core, insn, loc);
    mode_2 = BITS(6, 7);
    mode_3 = BITS(3, 5);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    core->st0 = core->st0 & (~(0x3f << 10)) | (((*reg) & 0x1f) << 10);
    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_mpy_acc_loc16_imm16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;
    int imm16 = fields->imm16;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    core->tl = *reg;
    int x = bit(core->acc, 31);

    core->acc = (core->tl) * imm16;
    int y = bit(*reg, 31);
    int z = bit(core->acc, 31);

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (z)
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Carry conditions */
    if ((x && y && !z && (core->acc != 0)) || (!x && !y && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);
    judge_flags_modes(core, insn);
    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_mpya_p_loc16_imm16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int pm = (core->st0 >> PM_BIT) & 0x7;
    int p_pm = 0;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;
    int imm16 = fields->imm16;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    core->tl = *reg;
    switch (pm)
    {
        case 0:
            //left shift 1
            p_pm = (core->p) << 1;
            break;
        case 1:
            //No shift
            p_pm = core->p;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            //Right shift by (pm - 1)
            p_pm = (core->p) >> (pm - 1);
            break;
        default:
            printf("In %s, %d, Not implement this instruction at pm = %x\n", __func__, __LINE__, pm);
            break;
    }
    core->p = (core->tl) * imm16;
    int x = bit(core->acc, 31);

    core->acc += p_pm;
    int y = bit(*reg, 31);
    int z = bit(core->acc, 31);

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (z)
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Carry conditions */
    if ((x && y && !z && (core->acc != 0)) || (!x && !y && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);
    judge_flags_modes(core, insn);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_mpya_p_t_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int pm = (core->st0 >> PM_BIT) & 0x7;
    int p_pm = 0;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    switch (pm)
    {
        case 0:
            //left shift 1
            p_pm = (core->p) << 1;
            break;
        case 1:
            //No shift
            p_pm = core->p;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            //Right shift by (pm - 1)
            p_pm = (core->p) >> (pm - 1);
            break;
        default:
            printf("In %s, %d, Not implement this instruction at pm = %x\n", __func__, __LINE__, pm);
            break;
    }
    core->p = (core->tl) * (*reg);
    int x = bit(core->acc, 31);

    core->acc += p_pm;
    int y = bit(*reg, 31);
    int z = bit(core->acc, 31);

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (z)
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Carry conditions */
    if ((x && y && !z && (core->acc != 0)) || (!x && !y && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);
    judge_flags_modes(core, insn);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_mpys_p_t_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int pm = (core->st0 >> PM_BIT) & 0x7;
    int p_pm = 0;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    switch (pm)
    {
        case 0:
            //left shift 1
            p_pm = (core->p) << 1;
            break;
        case 1:
            //No shift
            p_pm = core->p;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            //Right shift by (pm - 1)
            p_pm = (core->p) >> (pm - 1);
            break;
        default:
            printf("In %s, %d, Not implement this instruction at pm = %x\n", __func__, __LINE__, pm);
            break;
    }
    core->p = (core->tl) * (*reg);
    int x = bit(core->acc, 31);

    core->acc -= p_pm;
    int y = bit(*reg, 31);
    int z = bit(core->acc, 31);

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (z)
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Carry conditions */
    if ((x && y && !z && (core->acc != 0)) || (!x && !y && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);
    judge_flags_modes(core, insn);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_or_acc_imm16_shift(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = ((insn & 0xffff) << 16) | (insn >> 16);
    int op = BITS(28, 31);
    int imm16 = fields->imm16;

    if (op == 0x3)
    {
        int shift_value = fields->shift0_3;

        core->acc |= imm16 << shift_value;
    } else
    {
        core->acc |= imm16 << 16;
    }
    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_pop_dp_st1(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    core->sp -= 2;
    core->st1 = c28_memory_read(core, core->sp);
    core->dp = c28_memory_read(core, core->sp + 1);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_pop_ifr(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    core->sp -= 1;
    core->ifr = c28_memory_read(core, core->sp);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_pop_rpc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    core->sp -= 2;
    core->rpc = c28_memory_read(core, core->sp);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_pop_st1(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    core->sp -= 1;
    core->st1 = c28_memory_read(core, core->sp);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_pop_t_st0(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    core->sp -= 2;
    core->th = c28_memory_read(core, core->sp);
    core->st0 = c28_memory_read(core, core->sp + 1);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_push_dbgier(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    c28_memory_write(core, core->sp, core->dbgier);
    core->sp += 1;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_push_dp_st1(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    c28_memory_write(core, core->sp, core->st1);
    c28_memory_write(core, core->sp + 1, core->dp);
    core->sp += 2;
    core->insn_length = 1;

    core->pc += 1;
    return 0;
}
static int exec_push_ifr(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    c28_memory_write(core, core->sp, core->ifr);
    core->sp += 1;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_push_rpc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    c28_memory_write(core, core->sp, core->rpc);
    core->sp += 2;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_push_st1(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    c28_memory_write(core, core->sp, core->st1);
    core->sp += 1;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_push_t_st0(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    c28_memory_write(core, core->sp, core->st0);
    c28_memory_write(core, core->sp + 1, core->th);
    core->sp += 2;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_pwrite_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int value;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    c28_memory_write(core, core->xar[7].xar, *reg);

    if ((loc == 8) || (loc == 9))
    {
        /* Zero conditions */
        if (*reg == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (*reg & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);
    }
    if (core->rptc == 0)
    {
        core->pce1 = 0;
        core->insn_length = 1;
        core->pc += 1;
        return 0;
    } else
    {
        while (core->rptc)
        {
            core->rptc--;
            exec_pwrite_loc16(core, insn, fields);
        }
    }
}
static int exec_qmacl_p_loc32_xar7(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op = BITS(28, 31);
    int pm = (core->st0 >> PM_BIT) & 0x7;
    int p_pm = 0;
    int xar7 = 0;

    switch (pm)
    {
        case 0:
            //left shift 1
            p_pm = (core->p) << 1;
            break;
        case 1:
            //No shift
            p_pm = core->p;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            //Right shift by (pm - 1)
            p_pm = (core->p) >> (pm - 1);
            break;
        default:
            printf("In %s, %d, Not implement this instruction at pm = %x\n", __func__, __LINE__, pm);
            break;
    }
    if (op == 0xc)
    {                                   //*xar7
        xar7 = c28_memory_read(core, core->xar[7].xar);
    } else if (op == 0x8)
    {                                   //*xar7++
        xar7 = c28_memory_read(core, ((core->xar[7].xar) + 1));
    } else
        skyeye_log(Error_log, __func__, "decode error opcode for this instruction.\n");
    int x = bit(core->acc, 31);

    core->acc += p_pm;
    uint64_t result = core->th * xar7;

    core->p = result >> 32;
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);
    if (core->acc >> 31)
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    int z = bit(core->acc, 31);

    judge_flags_modes(core, insn);
    /* Carry conditions */
    if (x && (z == 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_qmpyal_p_xt_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int pm = (core->st0 >> PM_BIT) & 0x7;
    int p_pm = 0;
    int loc = 0;
    int loc32 = TRUE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int *reg = NULL;

    insn = (insn >> 16);
    mode_2 = BITS(6, 7);
    mode_3 = BITS(3, 5);
    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg |= c28_memory_read(core, core->sp_count + 1) << 16;
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }
    switch (pm)
    {
        case 0:
            //left shift 1
            p_pm = (core->p) << 1;
            break;
        case 1:
            //No shift
            p_pm = core->p;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            //Right shift by (pm - 1)
            p_pm = (core->p) >> (pm - 1);
            break;
        default:
            printf("In %s, %d, Not implement this instruction at pm = %x\n", __func__, __LINE__, pm);
            break;
    }
    int x = bit(core->acc, 31);

    core->acc += p_pm;
    uint64_t result = core->th * (*reg);

    core->p = result >> 32;
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);
    if (core->acc >> 31)
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    int z = bit(core->acc, 31);

    judge_flags_modes(core, insn);
    /* Carry conditions */
    if (x && (z == 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_qmpyl_p_xt_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg |= c28_memory_read(core, core->sp_count + 1) << 16;
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }
    int x = bit(core->acc, 31);
    uint64_t result = core->xt * (*reg);

    core->p = result >> 32;
    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_qmpysl_p_xt_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int pm = (core->st0 >> PM_BIT) & 0x7;
    int p_pm = 0;
    int loc = 0;
    int loc32 = TRUE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int *reg = NULL;

    insn = (insn >> 16);
    mode_2 = BITS(6, 7);
    mode_3 = BITS(3, 5);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg |= c28_memory_read(core, core->sp_count + 1) << 16;
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }
    switch (pm)
    {
        case 0:
            //left shift 1
            p_pm = (core->p) << 1;
            break;
        case 1:
            //No shift
            p_pm = core->p;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            //Right shift by (pm - 1)
            p_pm = (core->p) >> (pm - 1);
            break;
        default:
            printf("In %s, %d, Not implement this instruction at pm = %x\n", __func__, __LINE__, pm);
            break;
    }
    int x = bit(core->acc, 31);

    core->acc -= p_pm;
    uint64_t result = core->th * (*reg);

    core->p = result >> 32;
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);
    if (core->acc >> 31)
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    int z = bit(core->acc, 31);

    judge_flags_modes(core, insn);
    /* Carry conditions */
    if (x && (z == 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_qmpyxul_p_xt_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    uint64_t result;
    volatile int *reg = NULL;

    insn = (insn >> 16);
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        int addr_1 = c28_memory_read(core, core->data_addr);
        int addr_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (addr_2 << 16) | addr_1;
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg |= c28_memory_read(core, core->sp_count + 1) << 16;
    }
    if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }

    result = (int) core->xt * (*reg);
    core->p = result >> 32;

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_rol_acc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int c = (core->st0 >> 3) & 0x1;

    core->st0 = core->st0 & (~(0x1 << 3)) | (core->acc >> 31) << 3;
    core->acc = core->acc << 1;
    core->acc = core->acc & (~0x1) | c;
    if (core->acc >> 31)
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);
    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_ror_acc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int c = (core->st0 >> 3) & 0x1;

    core->st0 = core->st0 & (~(0x1 << 3)) | (core->acc & 0x1) << 3;
    core->acc = core->acc >> 1;
    core->acc = core->acc & (~(0x1 << 31)) | c << 31;
    if (core->acc >> 31)
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);
    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_sbbu_acc_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int c = (core->st0 >> 3) & 0x1;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;

    /*   insn = (insn >> 16);
     * mode_2 = BITS(6, 7);
     * mode_3 = BITS(3, 5);
     */
    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    int x = bit(core->acc, 31);

    core->acc = core->acc - (*reg) - ((~c) & 0x1);
    int z = bit(core->acc, 31);

    /* Carry conditions */
    if (x && (z == 0) && (core->acc != 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);
    judge_flags_modes(core, insn);
    if (core->acc == 0)
        SET(core->st0, Z_BIT);
    else
        CLEAR(core->st0, Z_BIT);
    if (core->acc >> 31)
        SET(core->st0, N_BIT);
    else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_setc_xf(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    core->st1 |= 0x1 << 12;
    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_sqra_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int pm = (core->st0 >> PM_BIT) & 0x7;
    int p_pm = 0;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;

    insn = (insn >> 16);
    mode_2 = BITS(6, 7);
    mode_3 = BITS(3, 5);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    switch (pm)
    {
        case 0:
            //left shift 1
            p_pm = (core->p) << 1;
            break;
        case 1:
            //No shift
            p_pm = core->p;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            //Right shift by (pm - 1)
            p_pm = (core->p) >> (pm - 1);
            break;
        default:
            printf("In %s, %d, Not implement this instruction at pm = %x\n", __func__, __LINE__, pm);
            break;
    }
    core->th = *reg;
    core->p = (core->th) * (*reg);
    int x = bit(core->acc, 31);

    core->acc += p_pm;
    int z = bit(core->acc, 31);

    /* Carry conditions */
    if (x && (z == 0) && (core->acc != 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);
    judge_flags_modes(core, insn);
    if (core->acc == 0)
        SET(core->st0, Z_BIT);
    else
        CLEAR(core->st0, Z_BIT);
    if (core->acc >> 31)
        SET(core->st0, N_BIT);
    else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_sqrs_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int pm = (core->st0 >> PM_BIT) & 0x7;
    int p_pm = 0;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;

    insn = (insn >> 16);
    mode_2 = BITS(6, 7);
    mode_3 = BITS(3, 5);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    switch (pm)
    {
        case 0:
            //left shift 1
            p_pm = (core->p) << 1;
            break;
        case 1:
            //No shift
            p_pm = core->p;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            //Right shift by (pm - 1)
            p_pm = (core->p) >> (pm - 1);
            break;
        default:
            printf("In %s, %d, Not implement this instruction at pm = %x\n", __func__, __LINE__, pm);
            break;
    }
    core->th = *reg;
    core->p = (core->th) * (*reg);
    int x = bit(core->acc, 31);

    core->acc -= p_pm;
    int z = bit(core->acc, 31);

    /* Carry conditions */
    if (x && (z == 0) && (core->acc != 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);
    judge_flags_modes(core, insn);
    if (core->acc == 0)
        SET(core->st0, Z_BIT);
    else
        CLEAR(core->st0, Z_BIT);
    if (core->acc >> 31)
        SET(core->st0, N_BIT);
    else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_sub_acc_loc16_t(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;
    int t_shift = core->th & 0xf;
    int loc16_shift = 0;
    int sxm = core->st0 & 0x1;

    insn = (insn >> 16);
    mode_2 = BITS(6, 7);
    mode_3 = BITS(3, 5);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    loc16_shift = *reg;
    if (sxm)
    {
        loc16_shift = (loc16_shift << t_shift) | ((loc16_shift & (0x1 << 31)) << 31);
    } else
    {
        loc16_shift = loc16_shift << t_shift;
    }
    int x = bit(core->acc, 31);

    core->acc -= loc16_shift;
    int z = bit(core->acc, 31);

    /* Carry conditions */
    if (x && (z == 0) && (core->acc != 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);
    judge_flags_modes(core, insn);
    if (core->acc == 0)
        SET(core->st0, Z_BIT);
    else
        CLEAR(core->st0, Z_BIT);
    if (core->acc >> 31)
        SET(core->st0, N_BIT);
    else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_subl_p_pm(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int pm = (core->st0 >> PM_BIT) & 0x7;
    int p_pm = 0;

    switch (pm)
    {
        case 0:
            //left shift 1
            p_pm = (core->p) << 1;
            break;
        case 1:
            //No shift
            p_pm = core->p;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            //Right shift by (pm - 1)
            p_pm = (core->p) >> (pm - 1);
            break;
        default:
            printf("In %s, %d, Not implement this instruction at pm = %x\n", __func__, __LINE__, pm);
            break;
    }
    int x = bit(core->acc, 31);

    core->acc -= p_pm;
    int z = bit(core->acc, 31);

    /* Carry conditions */
    if (x && (z == 0) && (core->acc != 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);
    judge_flags_modes(core, insn);
    if (core->acc == 0)
        SET(core->st0, Z_BIT);
    else
        CLEAR(core->st0, Z_BIT);
    if (core->acc >> 31)
        SET(core->st0, N_BIT);
    else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_subrl_loc32_acc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    int result;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int *reg = NULL;

    insn = (insn >> 16);
    mode_2 = BITS(6, 7);
    mode_3 = BITS(3, 5);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
        c28_memory_write(core, core->data_addr, core->acc - (*reg));
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg |= c28_memory_read(core, core->sp_count + 1) << 16;
        c28_memory_write(core, core->sp_count, core->acc - (*reg));
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
        c28_memory_write(core, core->data_addr, core->acc - (*reg));
    } else
    {
        core->xar[loc].xar = core->acc - (*reg);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_tbit_loc16_t(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int bit = 15 - (core->th) & 0xf;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }

    core->st0 = core->st0 & (~(0x1 << 2)) | (*reg >> bit) & 0x1;
    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_tclr_loc16_bit16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;

    insn = (insn >> 16);
    mode_2 = BITS(6, 7);
    mode_3 = BITS(3, 5);
    int bit16 = BITS(8, 11);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    core->st0 = (core->st0) & (~(0x1 << 2)) | ((*reg >> bit16) & 0x1 << 2);

    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        c28_memory_write(core, core->data_addr, (*reg) & (~(0x1 << bit16)));
    }
    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr, (*reg) & (~(0x1 << bit16)));
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count, (*reg) & (~(0x1 << bit16)));
    } else
    {
        core->xar[loc].ar = (*reg) & (~(0x1 << bit16));

    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_tset_loc16_bit16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;

    insn = (insn >> 16);
    mode_2 = BITS(6, 7);
    mode_3 = BITS(3, 5);
    int bit16 = BITS(8, 11);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    core->st0 = (core->st0) & (~(0x1 << 2)) | (*reg >> bit16) & 0x1;

    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        c28_memory_write(core, core->data_addr, (*reg) | (0x1 << bit16));
    }
    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr, (*reg) | (0x1 << bit16));
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count, (*reg) | (0x1 << bit16));
    } else
    {
        core->xar[loc].ar = (*reg) | (0x1 << bit16);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_xcall_al(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int temp = 0;

    temp = (core->pc + 1) & 0x3fffff;
    c28_memory_write(core, core->sp, temp & 0xffff);
    core->sp += 1;
    core->pc = (0x3f << 16) | (core->al);

    core->insn_length = 1;
    return 0;
}
static int exec_xb_al(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    core->insn_length = 1;
    core->pc = (0x3f << 16) | core->al;
    return 0;
}
static int exec_xor_acc_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    core->acc ^= (*reg) & 0xffff;
    if (core->acc == 0)
        SET(core->st0, Z_BIT);
    else
        CLEAR(core->st0, Z_BIT);
    if (core->acc >> 31)
        SET(core->st0, N_BIT);
    else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_xor_acc_imm16_0_16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    //  insn = ((insn & 0xffff) << 16) | (insn >> 16);
    int bit16 = BITS(16, 32);
    int op = BITS(4, 15);
    int shift = BITS(0, 3);

    if (op == 0x3e2)
    {
        core->acc ^= bit16 << shift;
    } else if (op == 0x565)
    {
        core->acc ^= bit16 << 16;
    } else
        skyeye_log(Error_log, __func__, "decode error opcode for this instruction.\n");
    if (core->acc >> 31)
        SET(core->st0, N_BIT);
    else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_xorb_ax_imm8(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int a = BITS(8, 11);
    int imm8 = BITS(0, 7);

    if (a == 0)
    {                                   //al
        core->al ^= imm8 & 0xff;
        if (core->al >> 15)
            SET(core->st0, N_BIT);
        else
            CLEAR(core->st0, N_BIT);
    } else
    {                                   //ah
        core->ah ^= imm8 & 0xff;
        if (core->ah >> 15)
            SET(core->st0, N_BIT);
        else
            CLEAR(core->st0, N_BIT);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_xpread_loc16_ax(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile uint16_t *reg = NULL;
    int addr = 0x3f << 16 | core->al & 0xfff;
    int data = c28_memory_read(core, addr);

    insn = insn >> 16;
    mode_2 = BITS(6, 7);
    mode_3 = BITS(3, 5);
    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr, data);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count, data);
    } else if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        c28_memory_write(core, core->data_addr, data);
    } else
    {
        *reg = data;
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_xpwrite_ax_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int addr = 0x3f << 16 | core->al & 0xfff;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    c28_memory_write(core, addr, *reg);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_xretc_cond(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int condition = BITS(0, 3);
    int cond = 0;

    switch (condition)
    {
        case 0:                        //NEQ Z=0
            if ((core->st0 >> 4) & 0x1 == 0)
                cond = 1;
            break;
        case 1:                        //EQ Z=1
            if ((core->st0 >> 4) & 0x1)
                cond = 1;
            break;
        case 2:                        //GT Z=0 N=0
            if ((core->st0 >> 4) & 0x2 == 0)
                cond = 1;
            break;
        case 3:                        //GEQ N=0
            if ((core->st0 >> 5) & 0x1 == 0)
                cond = 1;
            break;
        case 4:                        //LT N=1
            if ((core->st0 >> 5) & 0x1)
                cond = 1;
            break;
        case 5:                        //LEQ Z=1 or N=1
            if (((core->st0 >> 4) & 0x1) || ((core->st0 >> 5) & 0x1))
                cond = 1;
            break;
        case 6:                        //HI C=1 and Z=0
            if (((core->st0 >> 3) & 0x1) && ((core->st0 >> 4) & 0x1 == 0))
                cond = 1;
            break;
        default:
            skyeye_log(Error_log, __func__, "not implement the condition for this instruction.\n");
            break;
    }
    if (cond)
    {
        core->sp -= 1;
        int sp_data = c28_memory_read(core, core->sp);

        core->pc = (0x3f << 16) | (sp_data & 0xffff);
    } else
    {
        core->pc += 1;
    }

    core->insn_length = 1;
    return 0;
}
static int exec_zalr_cond(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;

    insn = insn >> 16;
    mode_2 = BITS(6, 7);
    mode_3 = BITS(3, 5);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    core->ah = *reg;
    core->al = 0x8000;
    if (core->acc == 0)
        SET(core->st0, Z_BIT);
    else
        CLEAR(core->st0, Z_BIT);
    if (core->acc >> 31)
        SET(core->st0, N_BIT);
    else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_bar(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = ((insn & 0xffff) << 16) | (insn >> 16);
    int op = BITS(22, 23);
    int n = BITS(19, 21);
    int m = BITS(16, 18);
    int arn = core->xar[n].ar;
    int arm = core->xar[m].ar;
    int cond = 0;
    int offset = BITS(0, 15);

    if (op == 2)
    {
        if (arn == arm)
            cond = 1;
    } else if (op == 3)
    {
        if (arn != arm)
            cond = 1;
    } else
    {
        skyeye_log(Error_log, __func__, "decode error opcode for this instruction.\n");
    }
    if (cond)
        core->pc += offset;
    else
        core->pc += 2;

    core->insn_length = 2;
    return 0;
}
static int exec_dmac(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op = BITS(28, 31);
    int xar7 = 0;
    int loc = 0;
    int loc32 = TRUE;
    int result;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int *reg = NULL;

    insn = (insn >> 16);
    mode_1 = BITS(0, 7);
    mode_2 = BITS(6, 7);
    mode_3 = BITS(3, 5);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg |= c28_memory_read(core, core->sp_count + 1) << 16;
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }
    core->xt = *reg;
    if (op == 0xc)
    {                                   //*xar7
        xar7 = c28_memory_read(core, core->xar[7].xar);
    } else if (op == 0x8)
    {                                   //*xar7++
        xar7 = c28_memory_read(core, core->xar[7].xar + 1);
    } else
    {
        skyeye_log(Error_log, __func__, "decode error opcode for this instruction.\n");
    }
    int x = bit(core->acc, 31);
    int z = 0;
    int result_msw = 0;
    int result_lsw = 0;

    result_msw = ((core->xt >> 16) & 0xffff) * ((xar7 >> 16) & 0xffff);
    result_lsw = ((core->xt) & 0xffff) * ((xar7) & 0xffff);
    int pm = (core->st0 >> PM_BIT) & 0x7;

    switch (pm)
    {
        case 0:
            //left shift 1
            core->acc += result_msw << 1;
            core->p += result_lsw << 1;
            z = bit(core->acc, 31);
            break;
        case 1:
            //No shift
            core->acc += result_msw;
            core->p += result_lsw;
            z = bit(core->acc, 31);
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            //Right shift by (pm - 1)
            core->acc += result_msw >> (pm - 1);
            core->p += result_lsw >> (pm - 1);
            z = bit(core->acc, 31);
            break;
        default:
            printf("In %s, %d, Not implement this instruction at pm = %x\n", __func__, __LINE__, pm);
            break;
    }
    if (core->acc == 0)
        SET(core->st0, Z_BIT);
    else
        CLEAR(core->st0, Z_BIT);
    if (core->acc >> 31)
        SET(core->st0, N_BIT);
    else
        CLEAR(core->st0, N_BIT);
    judge_flags_modes(core, insn);
    /* Carry conditions */
    if (x && (z == 0) && (core->acc != 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_dmov_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;
    int addr = 0;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        addr = core->data_addr;
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
        addr = core->data_addr;
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        addr = core->sp_count;
    }
    c28_memory_write(core, addr + 1, *reg);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_mac_p_loc16_0_pma(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int pm = (core->st0 >> PM_BIT) & 0x7;
    int p_pm = 0;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;
    int pma = BITS(16, 31);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    switch (pm)
    {
        case 0:
            //left shift 1
            p_pm = (core->p) << 1;
            break;
        case 1:
            //No shift
            p_pm = core->p;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            //Right shift by (pm - 1)
            p_pm = (core->p) >> (pm - 1);
            break;
        default:
            printf("In %s, %d, Not implement this instruction at pm = %x\n", __func__, __LINE__, pm);
            break;
    }
    int x = bit(core->acc, 31);

    core->acc += p_pm;
    core->p = (*reg) * c28_memory_read(core, pma);

    int z = bit(core->acc, 31);

    /* Carry conditions */
    if (x && (z == 0) && (core->acc != 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);
    judge_flags_modes(core, insn);
    if (core->acc == 0)
        SET(core->st0, Z_BIT);
    else
        CLEAR(core->st0, Z_BIT);
    if (core->acc >> 31)
        SET(core->st0, N_BIT);
    else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_mov_pm_ax(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op = BITS(0, 0);

    //int pm = (core->st0 >> PM_BIT) & 0x7;
    int ax = 0;

    if (op)
    {                                   // ah
        ax = (core->ah) & 0x7;
    } else
    {
        ax = (core->al) & 0x7;
    }
    core->st0 = (core->st0) & (~(0x7 << 7)) | (ax << 7);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_mov_loc16_0_bi16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile uint16_t *reg = NULL;
    int addr = BITS(16, 31);
    int data = c28_memory_read(core, addr);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr, data);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count, data);
    } else if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        c28_memory_write(core, core->data_addr, data);
    } else
    {
        *reg = data;
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_movad_t_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int pm = (core->st0 >> PM_BIT) & 0x7;
    int p_pm = 0;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;
    int pma = BITS(16, 31);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    core->th = *reg;
    int data = core->th;

    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr + 1, data);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count + 1, data);
    } else if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        c28_memory_write(core, core->data_addr + 1, data);
    } else
    {
        *reg = data;
    }
    switch (pm)
    {
        case 0:
            //left shift 1
            p_pm = (core->p) << 1;
            break;
        case 1:
            //No shift
            p_pm = core->p;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            //Right shift by (pm - 1)
            p_pm = (core->p) >> (pm - 1);
            break;
        default:
            printf("In %s, %d, Not implement this instruction at pm = %x\n", __func__, __LINE__, pm);
            break;
    }
    int x = bit(core->acc, 31);

    core->acc += p_pm;

    int z = bit(core->acc, 31);

    /* Carry conditions */
    if (x && (z == 0) && (core->acc != 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);
    judge_flags_modes(core, insn);
    if (core->acc == 0)
        SET(core->st0, Z_BIT);
    else
        CLEAR(core->st0, Z_BIT);
    if (core->acc >> 31)
        SET(core->st0, N_BIT);
    else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_movdl_xt_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    int result;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg |= c28_memory_read(core, core->sp_count + 1) << 16;
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }
    core->xt = *reg;
    int data = core->xt;

    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr + 2, data);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count + 2, data);
    } else if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        c28_memory_write(core, core->data_addr + 2, data);
    } else
    {
        *reg = data;
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_norm_acc_ind(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    core->acc = core->acc << 1;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_xb_pma_arpn(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int pma = BITS(16, 31);

    core->pc = (0x3f << 16) | pma;
    int n = BITS(0, 2);

    core->st1 = core->st1 & (~(0x7 << 13)) | (n << 13);

    core->insn_length = 1;
    return 0;
}
static int exec_xb_pma_cond(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int condition = BITS(0, 3);
    int cond = 0;
    int pma = BITS(16, 31);

    switch (condition)
    {
        case 0:                        //NEQ Z=0
            if (((core->st0 >> 4) & 0x1) == 0)
                cond = 1;
            break;
        case 1:                        //EQ Z=1
            if ((core->st0 >> 4) & 0x1)
                cond = 1;
            break;
        case 2:                        //GT Z=0 N=0
            if (((core->st0 >> 4) & 0x2) == 0)
                cond = 1;
            break;
        case 3:                        //GEQ N=0
            if (((core->st0 >> 5) & 0x1) == 0)
                cond = 1;
            break;
        case 4:                        //LT N=1
            if ((core->st0 >> 5) & 0x1)
                cond = 1;
            break;
        case 5:                        //LEQ Z=1 or N=1
            if (((core->st0 >> 4) & 0x1) || ((core->st0 >> 5) & 0x1))
                cond = 1;
            break;
        case 6:                        //HI C=1 and Z=0
            if (((core->st0 >> 3) & 0x1) && ((core->st0 >> 4) & 0x1 == 0))
                cond = 1;
            break;
        default:
            skyeye_log(Error_log, __func__, "not implement the condition for this instruction.\n");
            break;
    }
    if (cond)
    {
        core->pc = (0x3f << 16) | pma;
    } else
    {
        core->pc += 2;
    }

    core->insn_length = 2;
    return 0;
}
static int exec_xbanz_pma(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int ARP = (core->st1 >> 13) & 0x7;
    int pma = BITS(16, 31);

    if (core->xar[ARP].ar)
        core->pc = (0x3f << 16) | pma;
    else
        core->pc += 2;
    core->insn_length = 2;
    return 0;
}
static int exec_xcall_pma_cond(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int cond = 0;
    int condition = BITS(0, 3);

    switch (condition)
    {
        case 0:                        //NEQ Z=0
            if (((core->st0 >> 4) & 0x1) == 0)
            {
                cond = 1;
            }
            break;
        case 1:                        //EQ Z=1
            if ((core->st0 >> 4) & 0x1)
                cond = 1;
            break;
        case 2:                        //GT Z=0 N=0
            if ((core->st0 >> 4) & 0x2 == 0)
                cond = 1;
            break;
        case 3:                        //GEQ N=0
            if ((core->st0 >> 5) & 0x1 == 0)
                cond = 1;
            break;
        case 4:                        //LT N=1
            if ((core->st0 >> 5) & 0x1)
                cond = 1;
            break;
        case 5:                        //LEQ Z=1 or N=1
            if (((core->st0 >> 4) & 0x1) || ((core->st0 >> 5) & 0x1))
                cond = 1;
            break;
        case 6:                        //HI C=1 and Z=0
            if (((core->st0 >> 3) & 0x1) && ((core->st0 >> 4) & 0x1 == 0))
                cond = 1;
            break;
        default:
            skyeye_log(Error_log, __func__, "not implement the condition for this instruction.\n");
            break;
    }
    if (cond)
    {
        int temp = (core->pc + 1) & 0x3fffff;

        c28_memory_write(core, core->sp, temp & 0xffff);
        core->sp += 1;
        int pma = BITS(16, 31);

        core->pc = (0x3f << 16) | pma;
    } else
    {
        core->pc += 2;
    }

    core->insn_length = 2;
    return 0;
}
static int exec_xcall_pma_arpn(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int temp = (core->pc + 1) & 0x3fffff;

    c28_memory_write(core, core->sp, temp & 0xffff);
    core->sp += 1;
    int pma = BITS(16, 31);

    core->pc = (0x3f << 16) | pma;
    int n = BITS(0, 2);

    core->st1 = core->st1 & (~(0x7 << 13)) | (n << 13);

    core->insn_length = 2;
    return 0;
}
static int exec_xmac_p_loc16_pma(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int pm = (core->st0 >> PM_BIT) & 0x7;
    int p_pm = 0;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;
    int pma = BITS(16, 31);
    int pma_data = c28_memory_read(core, (0x3f << 16) | pma);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    switch (pm)
    {
        case 0:
            //left shift 1
            p_pm = (core->p) << 1;
            break;
        case 1:
            //No shift
            p_pm = core->p;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            //Right shift by (pm - 1)
            p_pm = (core->p) >> (pm - 1);
            break;
        default:
            printf("In %s, %d, Not implement this instruction at pm = %x\n", __func__, __LINE__, pm);
            break;
    }
    int x = bit(core->acc, 31);

    core->acc += p_pm;
    core->th = *reg;
    core->p = core->th * pma_data;

    int z = bit(core->acc, 31);

    /* Carry conditions */
    if (x && (z == 0) && (core->acc != 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);
    judge_flags_modes(core, insn);
    if (core->acc == 0)
        SET(core->st0, Z_BIT);
    else
        CLEAR(core->st0, Z_BIT);
    if (core->acc >> 31)
        SET(core->st0, N_BIT);
    else
        CLEAR(core->st0, N_BIT);

    core->pc += 2;
    core->insn_length = 2;
    return 0;
}
static int exec_in_loc16_pa(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;
    int pa = BITS(16, 31);
    int data = c28_memory_read(core, pa);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr + 1, data);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count + 1, data);
    } else if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        c28_memory_write(core, core->data_addr + 1, data);
    } else
    {
        *reg = data;
    }

    core->pc += 2;
    core->insn_length = 2;
    return 0;
}
static int exec_intr(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op = BITS(4, 15);
    int int_num = BITS(0, 3);

    if (op == 0x1)
    {
        int i = int_num;

        core->pc += 1;
        core->pie_group_num = i;
        if (i >= 12 && i < 16)
        {
            RW_WRLOCK(core->ifr_lock);
            core->ifr |= (1 << i);
            core->pie_group_num = i;
            core->signal_type = Non_Periph_Type;
            RW_UNLOCK(core->ifr_lock);
        } else
        {
            printf("In %s, c28x cpu interrupt group num is not defined.\n", __func__);
        }
    } else
    {
        printf("non't implement the op: %d of %s instruction 0x%x at 0x%x\n", op, __FUNCTION__, insn, core->pc);
    }
    core->insn_length = 1;
    return 0;
}
static int exec_trap(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int int_num = BITS(0, 4);
    int i = int_num;
    int isr_addr = 0;
    int isr_base_addr = PIE_VECTOR_BASE_ADDR;   //PieVectTable Base Address.

    isr_addr = isr_base_addr + i * 2;   //interrupt ISR handler address offset from Pie Vector Table base address (3fffc0).
    core->pc = c28_memory_read(core, isr_addr) | (c28_memory_read(core, isr_addr + 1) << 16);
    core->insn_length = 1;
    return 0;
}
static int exec_aborti(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    printf("warning: the insn %s implemention is empty %s\n", __FUNCTION__);
    core->pc = 1;
    core->insn_length = 1;
    return 0;
}
static int exec_iack(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    printf("warning: the insn %s implemention is empty %s\n", __FUNCTION__);
    core->pc = 2;
    core->insn_length = 2;
    return 0;
}
static int exec_out_pa_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int result;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int imm16 = fields->imm16;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    int pa = BITS(16, 31);

    c28_memory_write(core, pa, *reg);

    core->pc += 2;
    core->insn_length = 2;
    return 0;
}
static int exec_xmacd(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int pm = (core->st0 >> PM_BIT) & 0x7;
    int p_pm = 0;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;
    int pma = BITS(16, 31);
    int pma_data = c28_memory_read(core, (0x3f << 16) | pma);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    switch (pm)
    {
        case 0:
            //left shift 1
            p_pm = (core->p) << 1;
            break;
        case 1:
            //No shift
            p_pm = core->p;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            //Right shift by (pm - 1)
            p_pm = (core->p) >> (pm - 1);
            break;
        default:
            printf("In %s, %d, Not implement this instruction at pm = %x\n", __func__, __LINE__, pm);
            break;
    }
    int x = bit(core->acc, 31);

    core->acc += p_pm;
    core->th = *reg;
    core->p = core->th * pma_data;
    int data = core->th;

    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr + 1, data);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count + 1, data);
    } else if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        c28_memory_write(core, core->data_addr + 1, data);
    } else
    {
        *reg = data;
    }

    int z = bit(core->acc, 31);

    /* Carry conditions */
    if (x && (z == 0) && (core->acc != 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);
    judge_flags_modes(core, insn);
    if (core->acc == 0)
        SET(core->st0, Z_BIT);
    else
        CLEAR(core->st0, Z_BIT);
    if (core->acc >> 31)
        SET(core->st0, N_BIT);
    else
        CLEAR(core->st0, N_BIT);

    core->pc += 2;
    core->insn_length = 2;
    return 0;
}
static int exec_xpread_loc16_pma(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int pma = BITS(16, 31);
    int pma_data = c28_memory_read(core, (0x3f << 16) | pma);
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;
    int data = pma_data;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr, data);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count, data);
    } else if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        c28_memory_write(core, core->data_addr, data);
    } else
    {
        *reg = data;
    }

    core->pc += 2;
    core->insn_length = 2;
    return 0;
}
static int exec_neg_ax(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int a = fields->a0;

    if (a == 0)
    {
        if (core->al == 0x8000)
        {
            core->al = 0x8000;
            SET(core->st0, V_BIT);
        } else
        {
            core->al = -(core->al);
            if (core->al == 0)
            {
                SET(core->st0, C_BIT);
            } else
                CLEAR(core->st0, C_BIT);
        }
        /* Negative conditions */
        if (core->al & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Zero conditions */
        if (core->al == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);
    } else
    {
        if (core->ah == 0x8000)
        {
            core->ah = 0x8000;
            SET(core->st0, V_BIT);
        } else
        {
            core->ah = -(core->ah);
            if (core->ah == 0)
            {
                SET(core->st0, C_BIT);
            } else
                CLEAR(core->st0, C_BIT);
        }
        /* Negative conditions */
        if (core->ah & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Zero conditions */
        if (core->ah == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_edis(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    CLEAR(core->st1, EALLOW_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_lcr(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op1 = fields->op3_15;
    int op2 = fields->op6_15;
    int mode = fields->mode0_2;

    int imm22 = fields->bit6 << 16 | fields->imm16;

    int sp_data = (core->rpc & 0xffff);

    c28_memory_write(core, core->sp, sp_data);
    core->sp += 1;

    sp_data = (core->rpc & 0x3f0000) >> 16;
    c28_memory_write(core, core->sp, sp_data);
    core->sp += 1;

    if (op2 == 0x1d9)
    {
        core->rpc = core->pc + 2;
        core->insn_length = 2;
        core->pc = imm22;
    } else
    {
        core->rpc = core->pc + 1;
        core->insn_length = 1;
        core->pc = core->xar[mode].xar;
    }

    return 0;
}
static int exec_cmpl_acc_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    int result;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg |= c28_memory_read(core, core->sp_count + 1) << 16;
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }
    result = core->acc - *reg;
    int x = bit(core->acc, 31);
    int y = bit(*reg, 31);
    int z = bit(result, 31);

    /* Zero conditions */
    if (result == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (z)
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Carry conditions */
    if ((x && y && !z && (result != 0)) || (!x && !y && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_sbf(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op = fields->op8_15;
    int offset = fields->offset0_7;
    int z = core->st0 & (0x1 << 4);
    int tc = core->st0 & (0x1 << 2);

    if (offset & (0x1 << 7))
    {
        offset = -(0xff - offset + 1);
    }
    switch (op)
    {
        case 0xec:
            if (z)
            {
                core->pc += offset;
            } else
            {
                core->pc += 1;
            }
            break;
        case 0xed:
            if (z == 0)
            {
                core->pc += offset;
            } else
            {
                core->pc += 1;
            }
            break;
        case 0xee:
            if (tc)
            {
                core->pc += offset;
            } else
            {
                core->pc += 1;
            }
            break;
        case 0xef:
            if (tc == 0)
            {
                core->pc += offset;
            } else
            {
                core->pc += 1;
            }
            break;
        default:
            break;
    }
    core->insn_length = 1;
    return 0;
}
static int exec_movl_xar_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op = fields->op8_15;
    int op1 = fields->op12_15;
    int op2 = fields->cond8_11;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = TRUE;
    volatile int *reg = NULL;

    int objmode = (core->st1 >> 9) & 0x1;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        int addr_1 = c28_memory_read(core, core->data_addr);
        int addr_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (addr_2 << 16) | addr_1;
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg |= c28_memory_read(core, core->sp_count + 1) << 16;
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }
    if (op1 == 0x8)
    {
        switch (op2)
        {
            case 0xe:
                core->xar[0].xar = *reg;
                break;
            case 0xb:
                core->xar[1].xar = *reg;
                break;
            case 0x6:
                core->xar[2].xar = *reg;
                break;
            case 0x2:
                core->xar[3].xar = *reg;
                break;
            case 0xa:
                core->xar[4].xar = *reg;
                break;
            case 0x3:
                core->xar[5].xar = *reg;
                break;
            default:
                break;
        }
    } else if (op1 == 0xc)
    {
        if (op2 == 0x4)
        {
            core->xar[6].xar = *reg;
        } else
            core->xar[7].xar = *reg;
    } else
    {
        skyeye_log(Error_log, __func__, "decode error opcode for this instruction.\n");
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_mov_ax_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int a = fields->a8;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }

    if (a == 0)
    {
        core->al = *reg;

        /* N conditions */
        if (core->al & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);
        /* Z conditions */
        if (core->al == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);
    } else
    {
        core->ah = *reg;

        /* N conditions */
        if (core->ah & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);
        /* Z conditions */
        if (core->ah == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_addb_or_subb_sp(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int imm7 = fields->offset0_6;
    int b = fields->b;

    if (b == 0)
        core->sp += imm7;
    else
        core->sp -= imm7;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}

static int exec_sub_acc_loc16_shift(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int op1 = fields->op12_15;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int shift = fields->shift24_27;
    int sxm = (core->st0 & 0x1);
    volatile uint16_t *reg = NULL;

    /*
     * if (sxm){
     * shift = SIGN_EXTEND(shift, 4);       
     * }
     */
    int x = bit(core->acc, 31);

    if ((op1 == 0xa) || (op1 == 0x0))
    {
        loc = check_insn_address_mode(core, insn, fields, loc32);
        reg = get_register(core, insn, loc);

        if (mode_2 == 0)
        {
            int addr_1 = c28_memory_read(core, core->data_addr);
            int addr_2 = c28_memory_read(core, core->data_addr + 1);

            *reg = (addr_2 << 16) | addr_1;
        }
        if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
        {
            *reg = c28_memory_read(core, core->sp_count);
            *reg |= c28_memory_read(core, core->sp_count + 1) << 16;
        }
        if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
        {
            *reg = c28_memory_read(core, core->data_addr);
            *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
        }
        switch (op1)
        {
            case 0xa:
                core->acc -= *reg;
                break;
            case 0x0:
                core->acc -= (*reg << shift);
                break;
        }
    } else if (op1 == 0x5)
    {
        insn = (insn >> 16);
        mode_1 = BITS(0, 7);
        mode_2 = BITS(6, 7);
        mode_3 = BITS(3, 5);
        loc = check_insn_address_mode(core, insn, fields, loc32);
        reg = get_register(core, insn, loc);

        if (mode_2 == 0)
        {
            int addr_1 = c28_memory_read(core, core->data_addr);
            int addr_2 = c28_memory_read(core, core->data_addr + 1);

            *reg = (addr_2 << 16) | addr_1;
        }
        if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
        {
            *reg = c28_memory_read(core, core->sp_count);
            *reg |= c28_memory_read(core, core->sp_count + 1) << 16;
        }
        if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
        {
            *reg = c28_memory_read(core, core->data_addr);
            *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
        }
        core->acc -= (*reg << shift);
    }

    int z = bit(core->acc, 31);

    judge_flags_modes(core, insn);

    /* Carry conditions */
    if (x && (z == 0) && (core->acc != 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);

    if (core->rptc == 0)
    {
        core->insn_length = 1;
        core->pc += 1;
        return 0;
    } else
        while (core->rptc)
        {
            core->rptc--;
            return 0;
        }
}
static int exec_movl_loc32_xar(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    int op1 = fields->op12_15;
    int op2 = fields->cond8_11;
    volatile int *reg = NULL;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (op1 == 0x3)
    {
        if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
        {
            c28_memory_write(core, core->sp_count, core->xar[0].ar);
            c28_memory_write(core, core->sp_count + 1, core->xar[0].arh);
        } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
        {
            c28_memory_write(core, core->data_addr, core->xar[0].ar);
            c28_memory_write(core, core->data_addr + 1, core->xar[0].arh);
        } else if (mode_2 == 0)
        {
            c28_memory_write(core, core->data_addr, core->xar[0].ar);
            c28_memory_write(core, core->data_addr + 1, core->xar[0].arh);
        } else
            *reg = core->xar[0].xar;
    } else if (op1 == 0xb)
    {
        if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
        {
            c28_memory_write(core, core->sp_count, core->xar[1].ar);
            c28_memory_write(core, core->sp_count + 1, core->xar[1].arh);
        } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
        {
            c28_memory_write(core, core->data_addr, core->xar[1].ar);
            c28_memory_write(core, core->data_addr + 1, core->xar[1].arh);
        } else if (mode_2 == 0)
        {
            c28_memory_write(core, core->data_addr, core->xar[1].ar);
            c28_memory_write(core, core->data_addr + 1, core->xar[1].arh);
        } else
            *reg = core->xar[1].xar;
    } else if (op1 == 0xc)
    {
        if (op2 == 0x2)
        {
            if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
            {
                c28_memory_write(core, core->sp_count, core->xar[6].ar);
                c28_memory_write(core, core->sp_count + 1, core->xar[6].arh);
            } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
            {
                c28_memory_write(core, core->data_addr, core->xar[6].ar);
                c28_memory_write(core, core->data_addr + 1, core->xar[6].arh);
            } else if (mode_2 == 0)
            {
                c28_memory_write(core, core->data_addr, core->xar[6].ar);
                c28_memory_write(core, core->data_addr + 1, core->xar[6].arh);
            } else
                *reg = core->xar[6].xar;
        } else
        {
            if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
            {
                c28_memory_write(core, core->sp_count, core->xar[7].ar);
                c28_memory_write(core, core->sp_count + 1, core->xar[7].arh);
            } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
            {
                c28_memory_write(core, core->data_addr, core->xar[7].ar);
                c28_memory_write(core, core->data_addr + 1, core->xar[7].arh);
            } else if (mode_2 == 0)
            {
                c28_memory_write(core, core->data_addr, core->xar[7].ar);
                c28_memory_write(core, core->data_addr + 1, core->xar[7].arh);
            } else
                *reg = core->xar[7].xar;
        }
    } else if (op1 == 0xa)
    {
        switch (op2)
        {
            case 0xa:
                if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
                {
                    c28_memory_write(core, core->sp_count, core->xar[2].ar);
                    c28_memory_write(core, core->sp_count + 1, core->xar[2].arh);
                } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
                {
                    c28_memory_write(core, core->data_addr, core->xar[2].ar);
                    c28_memory_write(core, core->data_addr + 1, core->xar[2].arh);
                } else if (mode_2 == 0)
                {
                    c28_memory_write(core, core->data_addr, core->xar[2].ar);
                    c28_memory_write(core, core->data_addr + 1, core->xar[2].arh);
                } else
                    *reg = core->xar[2].xar;
                break;
            case 0x2:
                if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
                {
                    c28_memory_write(core, core->sp_count, core->xar[3].ar);
                    c28_memory_write(core, core->sp_count + 1, core->xar[3].arh);
                } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
                {
                    c28_memory_write(core, core->data_addr, core->xar[3].ar);
                    c28_memory_write(core, core->data_addr + 1, core->xar[3].arh);
                } else if (mode_2 == 0)
                {
                    c28_memory_write(core, core->data_addr, core->xar[3].ar);
                    c28_memory_write(core, core->data_addr + 1, core->xar[3].arh);
                } else
                    *reg = core->xar[3].xar;
                break;
            case 0x8:
                if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
                {
                    c28_memory_write(core, core->sp_count, core->xar[4].ar);
                    c28_memory_write(core, core->sp_count + 1, core->xar[4].arh);
                } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
                {
                    c28_memory_write(core, core->data_addr, core->xar[4].ar);
                    c28_memory_write(core, core->data_addr + 1, core->xar[4].arh);
                } else if (mode_2 == 0)
                {
                    c28_memory_write(core, core->data_addr, core->xar[4].ar);
                    c28_memory_write(core, core->data_addr + 1, core->xar[4].arh);
                } else
                    *reg = core->xar[4].xar;
                break;
            case 0x0:
                if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
                {
                    c28_memory_write(core, core->sp_count, core->xar[5].ar);
                    c28_memory_write(core, core->sp_count + 1, core->xar[5].arh);
                } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
                {
                    c28_memory_write(core, core->data_addr, core->xar[5].ar);
                    c28_memory_write(core, core->data_addr + 1, core->xar[5].arh);
                } else if (mode_2 == 0)
                {
                    c28_memory_write(core, core->data_addr, core->xar[5].ar);
                    c28_memory_write(core, core->data_addr + 1, core->xar[5].arh);
                } else
                    *reg = core->xar[5].xar;
                break;
            case 0xb:
                if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
                {
                    c28_memory_write(core, core->sp_count, core->tl);
                    c28_memory_write(core, core->sp_count + 1, core->th);
                } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
                {
                    c28_memory_write(core, core->data_addr, core->tl);
                    c28_memory_write(core, core->data_addr + 1, core->th);
                } else if (mode_2 == 0)
                {
                    c28_memory_write(core, core->data_addr, core->tl);
                    c28_memory_write(core, core->data_addr + 1, core->th);
                } else
                    *reg = core->xt;

                break;
            default:
                skyeye_log(Error_log, __func__, "It's error opcode for this instruction.\n");
                break;
        }
    } else
    {
        skyeye_log(Error_log, __func__, "decode error opcode for this instruction.\n");
    }
    if (loc == 9)
    {
        /* Zero conditions */
        if (core->acc == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (core->acc & (0x1 << 31))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_lretr(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    core->pc = core->rpc & 0x3fffff;

    core->sp -= 1;
    int sp_data = c28_memory_read(core, core->sp);

    core->rpc = (sp_data << 16) & 0x3fffff;

    core->sp -= 1;
    sp_data = c28_memory_read(core, core->sp);
    core->rpc |= sp_data;

    core->insn_length = 1;

    return 0;
}
static int exec_cmp_loc16_16sg(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int result;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int imm16 = fields->imm16;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    result = *reg - imm16;
    int x = bit(*reg, 15);
    int y = bit(imm16, 15);
    int z = bit(result, 31);

    /* Zero conditions */
    if (result == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (bit(result, 15))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);
    //Fix bug
    if ((!x && z) || (x && y && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_movb_acc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int imm8 = fields->offset0_7;

    core->acc = imm8;
    int z = bit(core->acc, 31);

    /* Negative conditions */
    if (z)
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);
    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_subb_acc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int imm8 = fields->offset0_7;
    int x = bit(core->acc, 31);

    core->acc -= imm8;
    int z = bit(core->acc, 31);

    judge_flags_modes(core, insn);
    /* Carry conditions */
    if (x && (z == 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_mov_loc16_imm(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr, 0);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count, 0);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        c28_memory_write(core, core->data_addr, 0);
    } else
        *reg = 0;

    if (loc == 8)
    {
        /* Negative conditions */
        if (core->ah & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);
        /* Zero conditions */
        if (core->ah == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);
    } else if (loc == 9)
    {
        /* Negative conditions */
        if (core->al & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);
        /* Zero conditions */
        if (core->al == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_addl_acc_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int *reg = NULL;
    int x = bit(core->acc, 31);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        int addr_1 = c28_memory_read(core, core->data_addr);
        int addr_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (addr_2 << 16) | addr_1;
        core->acc += *reg;
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        int data1 = c28_memory_read(core, core->sp_count);
        int data2 = c28_memory_read(core, core->sp_count + 1);

        *reg = (data2 << 16) | data1;
        core->acc += *reg;
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
        core->acc += *reg;
    } else
        core->acc += *reg;

    int y = bit(*reg, 31);
    int z = bit(core->acc, 31);

    judge_flags_modes(core, insn);
    /* Carry conditions */
    if ((x || y) && (z == 0))
    {
        SET(core->st0, C_BIT);
    } else
        CLEAR(core->st0, C_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_sub_ax_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int a8 = fields->a8;
    int result;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    int xl = bit(core->al, 15);
    int xh = bit(core->ah, 15);
    int y = bit(*reg, 15);

    if (a8 == 0)
    {
        core->al -= *reg;
        int zl = bit(core->al, 15);

        /* Zero conditions */
        if (core->al == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (zl)
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Carry conditions */
        if ((xl && y && zl) || (!xl && zl))
        {
            CLEAR(core->st0, C_BIT);
        } else
            SET(core->st0, C_BIT);

        /* Overflow conditions */
        if (core->al == NEGATIVE_OVERFLOW_BIT16)
            SET(core->st0, V_BIT);
    } else
    {
        core->ah -= *reg;
        int zh = bit(core->ah, 15);

        /* Zero conditions */
        if (core->ah == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (zh)
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Carry conditions */
        if ((xh && y && zh) || (!xh && zh))
        {
            CLEAR(core->st0, C_BIT);
        } else
            SET(core->st0, C_BIT);

        /* Overflow conditions */
        if (core->ah == NEGATIVE_OVERFLOW_BIT16)
            SET(core->st0, V_BIT);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_cmpb_ax_imm8(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int a8 = fields->a8;
    int imm8 = fields->offset0_7;
    int result, z;
    int xl = bit(core->al, 15);
    int xh = bit(core->ah, 15);

    if (a8 == 0)
    {
        result = core->al - imm8;
        z = bit(result, 15);

        /* Zero conditions */
        if (result == 0)
        {
            SET(core->st0, Z_BIT);
        } else
        {
            CLEAR(core->st0, Z_BIT);
        }
        /* Negative conditions */
        if (z)
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Carry conditions */
        if (xl && (z == 0) || (!xl && z))
        {
            CLEAR(core->st0, C_BIT);
        } else
            SET(core->st0, C_BIT);
    } else
    {
        result = core->ah - imm8;
        z = bit(result, 15);

        /* Zero conditions */
        if (result == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (z)
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Carry conditions */
        if (xh && (z == 0) || (!xh && z))
        {
            CLEAR(core->st0, C_BIT);
        } else
            SET(core->st0, C_BIT);
    }

    int z1 = core->st0 & (0x1 << 4);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_movb_loc16_cond(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int imm8 = fields->offset24_31;
    int cond = fields->mode0_3;
    int cond_flag = 0;
    volatile uint16_t *reg = NULL;

    int c = core->st0 & (0x1 << 3);
    int z = core->st0 & (0x1 << 4);
    int n = core->st0 & (0x1 << 5);
    int v = core->st0 & (0x1 << 6);
    int tc = core->st0 & (0x1 << 2);

    insn = insn >> 16;
    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);

    switch (cond)
    {
        case 0x0:
            if (z == 0)
            {
                cond_flag = 1;
            }
            break;
        case 0x1:
            if (z)
            {
                cond_flag = 1;
            }
            break;
        case 0x2:
            if (z == 0 && n == 0)
            {
                cond_flag = 1;
            }
            break;
        case 0x3:
            if (n == 0)
            {
                cond_flag = 1;
            }
            break;
        case 0x4:
            if (n)
            {
                cond_flag = 1;
            }
            break;
        case 0x5:
            if (z || n)
            {
                cond_flag = 1;
            }
            break;
        case 0x6:
            if (z == 0 && c)
            {
                cond_flag = 1;
            }
            break;
        case 0x7:
            if (c)
            {
                cond_flag = 1;
            }
            break;
        case 0x8:
            if (c == 0)
            {
                cond_flag = 1;
            }
            break;
        case 0x9:
            if (z || c == 0)
            {
                cond_flag = 1;
            }
            break;
        case 0xa:
            if (v == 0)
            {
                cond_flag = 1;
            }
            break;
        case 0xb:
            if (v)
            {
                cond_flag = 1;
                CLEAR(core->st0, V_BIT);
            }
            break;
        case 0xc:
            if (tc == 0)
            {
                cond_flag = 1;
            }
            break;
        case 0xd:
            if (tc)
            {
                cond_flag = 1;
            }
            break;
        case 0xe:
            if (imm8 == 0)
            {
                cond_flag = 1;
            }
            break;
        case 0xf:
            cond_flag = 1;
            break;
        default:
            break;
    }
    if (cond_flag == 1)
    {
        if (mode_2 == 0)
        {
            c28_memory_write(core, core->data_addr, imm8);
        } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
        {
            c28_memory_write(core, core->sp_count, imm8);
        } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
        {
            c28_memory_write(core, core->data_addr, imm8);
        } else
            *reg = imm8;

        if (loc == 8 || loc == 9)
        {
            /* Zero conditions */
            if (*reg == 0)
            {
                SET(core->st0, Z_BIT);
            } else
                CLEAR(core->st0, Z_BIT);

            /* Negative conditions */
            if (*reg & (0x1 << 15))
            {
                SET(core->st0, N_BIT);
            } else
                CLEAR(core->st0, N_BIT);
        }
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_mov_loc16_ax(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int a8 = fields->a8;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (a8 == 0)
    {
        if (mode_2 == 0)
        {
            c28_memory_write(core, core->data_addr, core->al);
        } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
        {
            c28_memory_write(core, core->sp_count, core->al);
        } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
        {
            c28_memory_write(core, core->data_addr, core->al);
        } else
        {
            *reg = core->al;
        }

        if (loc == 8 || loc == 9)
        {
            /* Zero conditions */
            if (*reg == 0)
            {
                SET(core->st0, Z_BIT);
            } else
                CLEAR(core->st0, Z_BIT);

            /* Negative conditions */
            if (*reg & (0x1 << 15))
            {
                SET(core->st0, N_BIT);
            } else
                CLEAR(core->st0, N_BIT);
        }
    } else
    {
        if (mode_2 == 0)
        {
            c28_memory_write(core, core->data_addr, core->ah);
        } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
        {
            c28_memory_write(core, core->sp_count, core->ah);
        } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
        {
            c28_memory_write(core, core->data_addr, core->ah);
        } else
        {
            *reg = core->ah;
        }
        if (loc == 8 || loc == 9)
        {
            /* Zero conditions */
            if (*reg == 0)
            {
                SET(core->st0, Z_BIT);
            } else
                CLEAR(core->st0, Z_BIT);

            /* Negative conditions */
            if (*reg & (0x1 << 15))
            {
                SET(core->st0, N_BIT);
            } else
                CLEAR(core->st0, N_BIT);
        }
    }

    if (core->rptc == 0)
    {
        core->insn_length = 1;
        core->pc += 1;
        return 0;
    } else
        while (core->rptc--)
        {
            return 0;
        }
}
static int exec_add_ax_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int a8 = fields->a8;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    int xl = bit(core->al, 15);
    int xh = bit(core->ah, 15);

    if (a8 == 0)
    {
        core->al += *reg;
        int zl = bit(core->al, 15);

        /* Zero conditions */
        if (core->al == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (core->al & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Carry conditions */
        if (xl && (zl == 0) || (!xl && zl))
        {
            SET(core->st0, C_BIT);
        } else
            CLEAR(core->st0, C_BIT);

        /* Overflow conditions */
        if (core->al == NEGATIVE_OVERFLOW_BIT16)
            SET(core->st0, V_BIT);
    } else
    {
        core->ah += *reg;
        int zh = bit(core->ah, 15);

        /* Zero conditions */
        if (core->ah == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (core->ah & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Carry conditions */
        if (xh && (zh == 0) || (!xh && zh))
        {
            SET(core->st0, C_BIT);
        } else
            CLEAR(core->st0, C_BIT);

        /* Overflow conditions */
        if (core->ah == NEGATIVE_OVERFLOW_BIT16)
            SET(core->st0, V_BIT);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_mov_t_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    core->th = *reg;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_mpy_acc_t_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    core->acc = (int16_t) core->th * (int16_t) (*reg);

    if (core->al > 0x7fff)
        core->acc |= 0xffff0000;

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_movb_xar(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op1 = fields->op11_15;
    int op2 = fields->op8_15;
    int mode = fields->mode8_10;
    int imm8 = fields->offset0_7;

    if (op1 == 0x1a)
    {
        core->xar[mode].xar = imm8;
    } else if (op2 == 0xbe)
    {
        core->xar[6].xar = imm8;
    } else if (op2 == 0xb6)
    {
        core->xar[7].xar = imm8;
    } else
    {
        skyeye_log(Error_log, __func__, "opcode error instructions 0x%x at 0x%x\n", __func__, insn, core->pc);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_mov_p(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile uint16_t *reg = NULL;
    int op = fields->op8_15;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    if (op == 0x27)
    {
        core->pl = *reg;
    } else if (op == 0x2f)
    {
        core->ph = *reg;
    } else
    {
        skyeye_log(Error_log, __func__, "opcode error for 0x%x at 0x%x\n", insn, core->pc);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_lsr_ax_shift(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int a4 = fields->a4;
    int shift = fields->mode0_3 + 1;

    //int c = BITS(shift, shift);
    int carry;

    if (a4 == 0)
    {
        carry = core->al & (0x1 << (shift - 1));
        core->al = (core->al >> shift);
        /* Zero conditions */
        if (core->al == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (core->al & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Carry conditions */
        if (carry == 0)
        {
            CLEAR(core->st0, C_BIT);
        } else
            SET(core->st0, C_BIT);
    } else
    {
        carry = core->ah & (0x1 << (shift - 1));
        core->ah = (core->ah >> shift);
        /* Zero conditions */
        if (core->ah == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (core->ah & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Carry conditions */
        if (carry == 0)
        {
            CLEAR(core->st0, C_BIT);
        } else
            SET(core->st0, C_BIT);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_addu_acc_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile uint16_t *reg = NULL;
    int x = bit(core->acc, 31);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    core->acc += *reg;

    int z = bit(core->acc, 31);

    judge_flags_modes(core, insn);
    /* Carry conditions */
    if (x && (z == 0))
    {
        SET(core->st0, C_BIT);
    } else
        CLEAR(core->st0, C_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_and_ax_loc16_imm16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int a8 = fields->a8;
    int imm16 = fields->imm16;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    if (a8 == 0)
    {
        core->al = *reg & imm16;

        /* Negative conditions */
        if (core->al & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Zero conditions */
        if (core->al == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);
    } else
    {
        core->ah = *reg & imm16;

        /* Negative conditions */
        if (core->ah & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Zero conditions */
        if (core->ah == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_andb_ax_imm8(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int a8 = fields->a8;
    int imm8 = fields->offset0_7;

    if (a8 == 0)
    {
        core->al &= imm8;

        /* Negative conditions */
        if (core->al & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Zero conditions */
        if (core->al == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);
    } else
    {
        core->ah &= imm8;

        /* Negative conditions */
        if (core->ah & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Zero conditions */
        if (core->ah == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_addb_ax_imm8(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int a8 = fields->a8;
    int imm8 = fields->offset0_7;

    imm8 = SIGN_EXTEND(imm8, 8);
    int xl = bit(core->al, 15);
    int xh = bit(core->ah, 15);

    if (a8 == 0)
    {
        core->al += imm8;
        int zl = bit(core->al, 15);

        /* Zero conditions */
        if (core->al == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (zl)
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Carry conditions */
        if (xl && (zl == 0) || (!xl && zl))
        {
            if (imm8 & NEGATIVE_OVERFLOW_BIT16)
                CLEAR(core->st0, C_BIT);
            else
                SET(core->st0, C_BIT);
        } else
        {
            if (imm8 & NEGATIVE_OVERFLOW_BIT16)
                SET(core->st0, C_BIT);
            else
                CLEAR(core->st0, C_BIT);
        }
        /* Overflow conditions */
        if (core->al == NEGATIVE_OVERFLOW_BIT16)
            SET(core->st0, V_BIT);
    } else
    {
        core->ah += imm8;

        int zh = bit(core->ah, 15);

        /* Zero conditions */
        if (core->ah == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (zh)
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Carry conditions */
        if (xh && (zh == 0) || (!xh && zh))
        {
            SET(core->st0, C_BIT);
        } else
            CLEAR(core->st0, C_BIT);

        /* Overflow conditions */
        if (core->ah == NEGATIVE_OVERFLOW_BIT16)
            SET(core->st0, V_BIT);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_cmp_ax_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int a8 = fields->a8;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;
    int xl = bit(core->al, 15);
    int xh = bit(core->ah, 15);
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int result;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    if (a8 == 0)
    {
        result = core->al - *reg;
        int zl = bit(result, 15);

        /* Negative conditions */
        if (result & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Zero conditions */
        if (result == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Carry conditions */
        if (xl && (zl == 0) || (!xl && zl))
        {
            CLEAR(core->st0, C_BIT);
        } else
            SET(core->st0, C_BIT);
    } else
    {
        result = core->ah - *reg;
        int zh = bit(result, 15);

        /* Negative conditions */
        if (result & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Zero conditions */
        if (result == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Carry conditions */
        if (xh && (zh == 0) || (!xh && zh))
        {
            CLEAR(core->st0, C_BIT);
        } else
            SET(core->st0, C_BIT);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_mov_acc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op1 = fields->op8_15;
    int op2 = fields->op0_15;
    int op3 = fields->op12_15;
    int loc32 = FALSE;
    int loc = 0;
    int shift = fields->shift24_27;
    int shift1 = BITS(8, 11);
    volatile uint16_t *reg = NULL;
    int mode_1, mode_2, mode_3;

    if (op2 == 0x5603)
    {
        insn = ((insn & 0xffff) << 16) | (insn >> 16);
        mode_1 = BITS(0, 7);
        mode_2 = BITS(6, 7);
        mode_3 = BITS(3, 5);
    } else
    {
        mode_1 = BITS(0, 7);
        mode_2 = BITS(6, 7);
        mode_3 = BITS(3, 5);
    }
    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }

    if (op1 == 0x85)
    {
        if (*reg & (0x1 << 15))
            core->acc = *reg | 0xffff0000;
        else
            core->acc = *reg;

        core->insn_length = 1;
        core->pc += 1;
    } else if (op1 == 0x25)
    {
        if (*reg & (0x1 << 15))
            core->acc = (*reg | 0xffff0000) << 16;
        else
            core->acc = *reg << 16;

        core->insn_length = 1;
        core->pc += 1;
    } else if (op2 == 0x5603)
    {
        if (*reg & (0x1 << 15))
            core->acc = (*reg | 0xffff0000) << shift;
        else
            core->acc = *reg << shift;

        core->insn_length = 2;
        core->pc += 2;
    } else if (op3 == 0xe)
    {
        if (core->st1 & OBJMODE == 0)
        {
            core->acc = *reg << shift1;
            core->insn_length = 1;
            core->pc += 1;
        } else
        {
            printf("In %s, decode instruction %x error.\n", __func__, insn);
            skyeye_exit(1);
        }
    }

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    return 0;
}
static int exec_addl_lc_acc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    volatile int *reg = NULL;

    insn = (insn >> 16);

    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    int x = bit(core->acc, 31);

    if (mode_2 == 0)
    {
        int addr_1 = c28_memory_read(core, core->data_addr);
        int addr_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (addr_2 << 16) | addr_1;
        *reg += core->acc;
        c28_memory_write(core, core->data_addr, *reg & 0xffff);
        c28_memory_write(core, core->data_addr + 1, (*reg & 0xffff0000) >> 16);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        int data1 = c28_memory_read(core, core->sp_count);
        int data2 = c28_memory_read(core, core->sp_count + 1);

        *reg = (data2 << 16) | data1;
        *reg += core->acc;
        c28_memory_write(core, core->sp_count, *reg & 0xffff);
        c28_memory_write(core, core->sp_count + 1, (*reg & 0xffff0000) >> 16);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr) | (c28_memory_read(core, core->data_addr + 1) << 16);
        *reg += core->acc;
        c28_memory_write(core, core->data_addr, *reg);
    } else
        *reg += core->acc;

    int z = bit(core->acc, 31);

    judge_flags_modes(core, insn);
    /* Zero conditions */
    if (*reg == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (*reg & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);
    /* Carry conditions */
    if (x && (z == 0))
    {
        SET(core->st0, C_BIT);
    } else
        CLEAR(core->st0, C_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_mov_ier_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    core->ier = *reg;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}

static int exec_nop(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (core->rptc == 0)
    {
        core->insn_length = 1;
        core->pc += 1;
        return 0;
    } else
        while (core->rptc--)
        {
            return 0;
        }
}

static int exec_add_lc_16sg(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    uint16_t imm16 = fields->imm16;
    int loc = 0;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    int x = bit(*reg, 15);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg += imm16;
        c28_memory_write(core, core->data_addr, *reg);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg += imm16;
        c28_memory_write(core, core->sp_count, *reg);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg += imm16;
        c28_memory_write(core, core->data_addr, *reg);
    } else
        *reg += imm16;

    int y = bit(*reg, 15);
    int z = bit(imm16, 15);

    /* Zero conditions */
    if (*reg == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (y)
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Carry conditions */
    //      if (x && (z == 0) || (!x && z)){
    if ((x && y && !z) || (!x && !y && z))
    {
        SET(core->st0, C_BIT);
    } else
        CLEAR(core->st0, C_BIT);

    /* Overflow conditions */
    if (y == NEGATIVE_OVERFLOW_BIT16)
        SET(core->st0, V_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_mov_16bit_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    uint16_t imm16 = fields->imm16;
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    c28_memory_write(core, imm16, *reg);

    if (core->rptc == 0)
    {
        core->insn_length = 2;
        core->pc += 2;
        return 0;
    } else
        while (core->rptc--)
        {
            return 0;
        }
}
static int exec_sfr_acc_16bit(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int sxm = core->st0 & 0x1;
    int shift = fields->mode0_3 + 1;
    int carry;

    int x = core->acc;

    carry = core->acc & (0x1 << (shift - 1));
    if (sxm)
    {
        core->acc = (core->acc >> shift) | ((core->acc & (0x1 << 31)) << 31);
    } else
    {
        core->acc = core->acc >> shift;
    }
    int z = bit(core->acc, 31);

    /* Negative conditions */
    if (z)
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);
    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);
    /* Carry conditions */
    if (carry)
    {
        SET(core->st0, C_BIT);
    } else
        CLEAR(core->st0, C_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_dec(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int x;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
        x = bit(*reg, 15);
        *reg -= 1;
        c28_memory_write(core, core->data_addr, *reg);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        x = bit(*reg, 15);
        *reg -= 1;
        c28_memory_write(core, core->sp_count, *reg);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        x = bit(*reg, 15);
        *reg -= 1;
        c28_memory_write(core, core->data_addr, *reg);
    } else
    {
        x = bit(*reg, 15);
        *reg -= 1;
    }
    int z = bit(*reg, 15);

    /* Zero conditions */
    if (*reg == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (z)
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Carry conditions */
    if (x && (z == 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);

    /* Overflow conditions */
    if (*reg == NEGATIVE_OVERFLOW_BIT16)
        SET(core->st0, V_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_movl_xt_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int data_1, data_2;
    volatile int *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        data_1 = c28_memory_read(core, core->data_addr);
        data_2 = c28_memory_read(core, core->data_addr + 1);
        *reg = (data_2 << 16) | data_1;
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        data_1 = c28_memory_read(core, core->sp_count);
        data_2 = c28_memory_read(core, core->sp_count + 1);
        *reg = (data_2 << 16) | data_1;
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        data_1 = c28_memory_read(core, core->data_addr);
        data_2 = c28_memory_read(core, core->data_addr + 1);
        *reg = (data_2 << 16) | data_1;
    }

    core->xt = *reg;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_mov_loc16_arn(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int mode = fields->mode8_10;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr, core->xar[mode].ar);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count, core->xar[mode].ar);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        c28_memory_write(core, core->data_addr, core->xar[mode].ar);
    } else
        *reg = core->xar[mode].ar;

    if ((loc == 8) || (loc == 9))
    {
        /* Zero conditions */
        if (*reg == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (*reg & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_b(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int cond = fields->mode0_3;
    int offset = fields->imm16;
    int c = core->st0 & (0x1 << 3);
    int z = core->st0 & (0x1 << 4);
    int n = core->st0 & (0x1 << 5);
    int v = core->st0 & (0x1 << 6);
    int tc = core->st0 & (0x1 << 2);

    if (offset & (0x1 << 15))
    {
        offset = -(0xffff - offset + 1);
    }

    switch (cond)
    {
        case NEQ:
            if (z == 0)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case EQ:
            if (z)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case GT:
            if (z == 0 && n == 0)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case GEQ:
            if (n == 0)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case LT:
            if (n)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case LEQ:
            if (z || n)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case HI:
            if (z == 0 && c)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case HIS_C:
            if (c)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case LO_NC:
            if (c == 0)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case LOS:
            if (z || (c == 0))
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case NOV:
            if (v == 0)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case OV:
            if (v)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case NTC:
            if (tc == 0)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case TC_C:
            if (tc)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case NBIO:
            if (offset == 0)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case UNC:
            core->pc += offset;
            break;
        default:
            break;
    }

    if (cond == 0xa || cond == 0xb)
    {
        CLEAR(core->st0, V_BIT);
    }
    core->insn_length = 2;
    return 0;
}
static int exec_movz_dp(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int imm10 = fields->offset0_9;

    core->dp = imm10;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}

static int exec_mov_acc_16bit_shift(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int shift = fields->shift0_3;
    int imm16 = fields->imm16;
    int sxm = core->st0 & 0x1;

    if (sxm)
    {
        if ((imm16 >> 15) & 0x1)
        {
            core->acc = (imm16 << shift);
            core->acc |= sign_extend(shift);
        } else
        {
            core->acc = (imm16 << shift);
        }
    } else
    {
        core->acc = (imm16 << shift);
    }
    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}

static int exec_test_acc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}

static int exec_lsl_acc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int shift = fields->shift0_3 + 1;
    int carry;

    carry = core->acc & (0x1 << (0x20 - shift));
    core->acc = core->acc << shift;
    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
    {
        CLEAR(core->st0, N_BIT);
    }
    /* Carry conditions */
    if (carry)
    {
        SET(core->st0, C_BIT);
    } else
        CLEAR(core->st0, C_BIT);

    if (core->rptc == 0)
    {
        core->insn_length = 1;
        core->pc += 1;
        return 0;
    } else
        while (core->rptc)
        {
            core->rptc--;
            return 0;
        }
}
static int exec_push_st0(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    c28_memory_write(core, core->sp, core->st0);
    core->sp += 1;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_pop_st0(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    core->sp -= 1;
    core->st0 = c28_memory_read(core, core->sp);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}

static int exec_abs_acc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (core->acc == NEGATIVE_OVERFLOW)
    {
        SET(core->st0, V_BIT);
        if (core->st0 & 0x2)
            core->acc = POSITIVE_OVERFLOW;
        else
            core->acc = NEGATIVE_OVERFLOW;
    } else
    {
        if (core->acc & (0x1 << 31))
        {
            core->acc = (-core->acc);
        }
    }

    judge_flags_modes(core, insn);
    /* Carry conditions */
    CLEAR(core->st0, C_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}

static int exec_xor_ax_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int a8 = fields->a8;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int loc = 0;
    int loc32 = FALSE;
    volatile int *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    if (a8 == 0)
    {
        core->al ^= *reg;
        /* Zero conditions */
        if (core->al == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (core->al & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);
    } else
    {
        core->ah ^= *reg;
        /* Zero conditions */
        if (core->ah == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (core->ah & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}

static int exec_and_loc16_16sg(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int imm16 = fields->imm16;
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg &= imm16;
        c28_memory_write(core, core->data_addr, *reg);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg &= imm16;
        c28_memory_write(core, core->sp_count, *reg);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg &= imm16;
        c28_memory_write(core, core->data_addr, *reg);
    } else
        *reg &= imm16;

    /* Negative conditions */
    if (*reg & (0x1 << 15))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Zero conditions */
    if (*reg == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}

static int exec_sat_acc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int ovc = (core->st0 & 0xfc00);

    if ((ovc > 0) && (ovc & (0x1 << 16)))
    {
        core->acc = 0x80000000;
        SET(core->st0, V_BIT);
    } else if ((ovc > 0) && (ovc & (0x1 << 16) == 0))
    {
        core->acc = 0x7FFFFFFF;
        SET(core->st0, V_BIT);
    } else if (ovc == 0)
    {
        CLEAR(core->st0, V_BIT);
    }

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Carry conditions */
    CLEAR(core->st0, C_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_movl_p_acc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    core->p = core->acc;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_asr_ax_shift(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int a4 = fields->a4;
    int shift = fields->shift0_3 + 1;
    int cl = bit(core->al, 0);
    int ch = bit(core->ah, 0);

    if (a4 == 0)
    {
        core->al = core->al >> shift;
        /* Zero conditions */
        if (core->al == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (core->al & (0x1 << 31))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Carry conditions */
        if (cl == 1)
        {
            SET(core->st0, C_BIT);
        } else
            CLEAR(core->st0, C_BIT);
    } else
    {
        core->ah = core->ah >> shift;
        /* Zero conditions */
        if (core->ah == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (core->ah & (0x1 << 31))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Carry conditions */
        if (ch == 1)
        {
            SET(core->st0, C_BIT);
        } else
            CLEAR(core->st0, C_BIT);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_subbl_acc_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int *reg = NULL;

    insn = (insn >> 16);
    mode_2 = BITS(6, 7);
    mode_3 = BITS(3, 5);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    int x = bit(core->acc, 31);
    int c_flags = bit(core->st0, C_BIT);

    // ACC = ACC - [loc32] - ~C;
    if (mode_2 == 0)
    {
        int addr_1 = c28_memory_read(core, core->data_addr);
        int addr_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (addr_2 << 16) | addr_1;
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg |= c28_memory_read(core, core->sp_count + 1) << 16;
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        int addr_1 = c28_memory_read(core, core->data_addr);
        int addr_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (addr_2 << 16) | addr_1;
    }

    core->acc -= *reg;
    core->acc -= (~c_flags) & 0x1;

    int z = bit(core->acc, 31);

    judge_flags_modes(core, insn);

    /* Carry conditions */
    if (x && (z == 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
    {
        SET(core->st0, C_BIT);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_subl_loc32_acc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    volatile int *reg = NULL;

    int new_insn = insn >> 16;

    loc = check_insn_address_mode(core, new_insn, fields, loc32);
    reg = get_register(core, insn, loc);
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);
    int x = bit(*reg, 31);

    if (mode_2 == 0)
    {
        int addr_1 = c28_memory_read(core, core->data_addr);
        int addr_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (addr_2 << 16) | addr_1;
        *reg -= core->acc;
        c28_memory_write(core, core->data_addr, *reg & 0xffff);
        c28_memory_write(core, core->data_addr + 1, (*reg & 0xffff0000) >> 16);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg |= c28_memory_read(core, core->sp_count + 1) << 16;
        *reg -= core->acc;
        c28_memory_write(core, core->sp_count, *reg & 0xffff);
        c28_memory_write(core, core->sp_count + 1, (*reg & 0xffff0000) >> 16);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        int addr_1 = c28_memory_read(core, core->data_addr);
        int addr_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (addr_2 << 16) | addr_1;
        *reg -= core->acc;
        c28_memory_write(core, core->data_addr, (*reg & 0xffff));
        c28_memory_write(core, core->data_addr + 1, (*reg & 0xffff0000) >> 16);
    } else
        *reg -= core->acc;

    int z = bit(*reg, 31);

    judge_flags_modes(core, insn);

    /* Negative conditions */
    if (*reg & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Carry conditions */
    if (x && (z == 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_subl_acc_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    int x = bit(core->acc, 31);

    if (mode_2 == 0)
    {
        int addr_1 = c28_memory_read(core, core->data_addr);
        int addr_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (addr_2 << 16) | addr_1;
        core->acc -= *reg;
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg |= c28_memory_read(core, core->sp_count + 1) << 16;
        core->acc -= *reg;
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        int addr_1 = c28_memory_read(core, core->data_addr);
        int addr_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (addr_2 << 16) | addr_1;
        core->acc -= *reg;
    } else
        core->acc -= *reg;

    int z = bit(core->acc, 31);

    judge_flags_modes(core, insn);

    /* Carry conditions */
    if (x && (z == 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_ffc_xar7_22bit(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int imm22 = fields->bit6 << 16 | fields->imm16;

    core->xar[7].xar = (core->pc + 2) & 0x3fffff;
    core->insn_length = 2;
    core->pc = imm22;

    return 0;
}
static int exec_subcu_acc_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int temp;
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    temp = (core->acc << 1) - (*reg << 16);
    int x = bit(temp, 31);

    if (temp < 0)
    {
        core->acc = core->acc << 1;
    } else
        core->acc = temp + 1;

    int z = bit(core->acc, 31);

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Carry conditions */
    if (x && (z == 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);

    if (core->rptc == 0)
    {
        core->insn_length = 1;
        core->pc += 1;
        return 0;
    } else
    {
        while (core->rptc)
        {
            core->rptc--;
            return 0;
        }
    }
}
static int exec_tbit_loc16_16bit(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int imm16 = fields->cond8_11;
    int loc = 0;
    int loc32 = FALSE;
    volatile int *reg = NULL;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    int tc = (*reg >> imm16) & 1;

    if (tc)
    {
        SET(core->st0, TC_BIT);
    } else
        CLEAR(core->st0, TC_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_lb_xar(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{

    core->insn_length = 1;
    core->pc = core->xar[7].xar & 0x3fffff;
    return 0;
}
static int exec_inc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    volatile uint16_t *reg = NULL;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int x = 0;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
        x = bit(*reg, 15);
        *reg += 1;
        c28_memory_write(core, core->data_addr, *reg);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        x = bit(*reg, 15);
        *reg += 1;
        c28_memory_write(core, core->sp_count, *reg);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        x = bit(*reg, 15);
        *reg += 1;
        c28_memory_write(core, core->data_addr, *reg);
    } else
    {
        x = bit(*reg, 15);
        *reg += 1;
    }

    int z = bit(*reg, 15);

    /* Zero conditions */
    if (*reg == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (*reg & (0x1 << 15))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Carry conditions */
    if (x && (z == 0) || (!x && z))
    {
        SET(core->st0, C_BIT);
    } else
        CLEAR(core->st0, C_BIT);

    /* Overflow conditions */
    if (*reg == NEGATIVE_OVERFLOW)
        SET(core->st0, V_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_lsl_ax_16bit(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int a4 = fields->a4;
    int shift = fields->shift0_3 + 1;
    int carry;

    if (a4 == 0)
    {
        carry = core->al & (0x1 << (0x10 - shift));
        core->al = core->al << shift;
        /* Zero conditions */
        if (core->al == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (core->al & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Carry conditions */
        if (carry)
        {
            SET(core->st0, C_BIT);
        } else
            CLEAR(core->st0, C_BIT);
    } else
    {
        carry = core->ah & (0x1 << (0x10 - shift));
        core->ah = core->ah << shift;
        /* Zero conditions */
        if (core->ah == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (core->ah & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Carry conditions */
        if (carry)
        {
            SET(core->st0, C_BIT);
        } else
            CLEAR(core->st0, C_BIT);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_or_ax_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int a8 = fields->a8;
    int loc = 0;
    int loc32 = FALSE;
    volatile int *reg = NULL;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    int xl = bit(core->al, 15);
    int xh = bit(core->ah, 15);

    if (a8 == 0)
    {
        core->al |= *reg;
        int zl = bit(core->al, 15);

        /* Zero conditions */
        if (core->al == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (core->al & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Carry conditions */
        if (xl && (zl == 0))
        {
            SET(core->st0, C_BIT);
        } else
            CLEAR(core->st0, C_BIT);

        /* Overflow conditions */
        if (core->al == NEGATIVE_OVERFLOW_BIT16)
            SET(core->st0, V_BIT);
    } else
    {
        core->ah |= *reg;
        int zh = bit(core->ah, 15);

        /* Zero conditions */
        if (core->ah == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (core->ah & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Carry conditions */
        if (xh && (zh == 0))
        {
            SET(core->st0, C_BIT);
        } else
            CLEAR(core->st0, C_BIT);

        /* Overflow conditions */
        if (core->ah == NEGATIVE_OVERFLOW_BIT16)
            SET(core->st0, V_BIT);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_orb_ax_8bit(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int a8 = fields->a8;
    int imm8 = fields->offset0_7;

    if (a8 == 0)
    {
        core->al |= imm8;
        int zl = bit(core->al, 15);

        /* Zero conditions */
        if (core->al == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (zl)
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);
    } else
    {
        core->ah |= imm8;
        int zh = bit(core->ah, 15);

        /* Zero conditions */
        if (core->ah == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (zh)
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);
    }
    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_or_loc16_16bit(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int imm16 = fields->imm16;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg |= imm16;
        c28_memory_write(core, core->sp_count, *reg);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= imm16;
        c28_memory_write(core, core->data_addr, *reg);
    } else if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= imm16;
        c28_memory_write(core, core->data_addr, *reg);
    } else
        *reg |= imm16;

    /* Zero conditions */
    if (*reg == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (*reg & (0x1 << 15))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_bf(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int cond = fields->mode0_3;
    int offset = fields->imm16;
    int c = core->st0 & (0x1 << 3);
    int z = core->st0 & (0x1 << 4);
    int n = core->st0 & (0x1 << 5);
    int v = core->st0 & (0x1 << 6);
    int tc = core->st0 & (0x1 << 2);

    if (offset & (0x1 << 15))
    {
        offset = -(0xffff - offset + 1);
    }

    switch (cond)
    {
        case NEQ:
            if (z == 0)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case EQ:
            if (z)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case GT:
            if (z == 0 && n == 0)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case GEQ:
            if (n == 0)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case LT:
            if (n)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case LEQ:
            if (z || n)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case HI:
            if (z == 0 && c)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case HIS_C:
            if (c)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case LO_NC:
            if (c == 0)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case LOS:
            if (z || (c == 0))
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case NOV:
            if (v == 0)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case OV:
            if (v)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case NTC:
            if (tc == 0)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case TC_C:
            if (tc)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case NBIO:
            if (offset == 0)
            {
                core->pc += offset;
            } else
            {
                core->pc += 2;
            }
            break;
        case UNC:
            core->pc += offset;
            break;
        default:
            break;
    }

    if (cond == 0xa || cond == 0xb)
    {
        CLEAR(core->st0, V_BIT);
    }

    core->insn_length = 2;
    return 0;
}
static int exec_and_ifr(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int imm16 = fields->imm16;

    core->ifr &= imm16;

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_and_loc16_ax(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int a8 = fields->a8;
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
        if (a8 == 0)
        {
            *reg &= core->al;
        } else
            *reg &= core->ah;

        c28_memory_write(core, core->data_addr, *reg);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        if (a8 == 0)
        {
            *reg &= core->al;
        } else
            *reg &= core->ah;

        c28_memory_write(core, core->sp_count, *reg);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        if (a8 == 0)
        {
            *reg &= core->al;
        } else
            *reg &= core->ah;

        c28_memory_write(core, core->data_addr, *reg);
    } else
    {
        if (a8 == 0)
        {
            *reg &= core->al;
        } else
            *reg &= core->ah;
    }

    /* Zero conditions */
    if (*reg == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (*reg & (0x1 << 15))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_movu_acc_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    core->al = *reg;
    core->ah = 0;

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    CLEAR(core->st0, N_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_movu_loc16_ovc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int ax = fields->a8;
    volatile uint16_t *reg = NULL;

    int ovc = (core->st0 & 0xfc00) >> 10;
    int data = ovc;

    insn = (insn >> 16);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    /*
     * if (mode_2 == 0) {
     * *reg = c28_memory_read(core, core->data_addr);
     * }
     * 
     * if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7))) {
     * *reg = c28_memory_read(core, core->sp_count);
     * }
     * 
     * if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))) {
     * *reg = c28_memory_read(core, core->data_addr);
     * }
     * 
     * *reg  = ovc;
     * *reg |= ovc;
     */
    mode_2 = BITS(6, 7);
    mode_3 = BITS(3, 5);
    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr, data);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count, data);
    } else if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        c28_memory_write(core, core->data_addr, data);
    } else
    {
        *reg = data;
    }

    /* Negative conditions */
    if ((ax == *reg) && (ax & 0x8000))
    {
        SET(core->st0, N_BIT);
    } else
    {
        CLEAR(core->st0, N_BIT);
    }

    /* Zero conditions */
    if ((ax == *reg) && (ax == 0))
    {
        SET(core->st0, Z_BIT);
    } else
    {
        CLEAR(core->st0, Z_BIT);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_lsll_acc_t(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int shift;
    int carry;

    shift = core->th & 0x1f;
    carry = core->acc & (0x1 << (0x20 - shift));
    core->acc = core->acc << shift;

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);
    /* Carry conditions */
    if (shift == 0)
    {
        CLEAR(core->st0, C_BIT);
    } else
    {
        if (carry)
        {
            SET(core->st0, C_BIT);
        } else
            CLEAR(core->st0, C_BIT);
    }
    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_lsrl_acc_t(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int shift;

    shift = core->th & 0x1f;
    int carry = core->acc & (0x1 << (shift - 1));

    core->acc = core->acc >> shift;

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Carry conditions */
    if (shift == 0)
    {
        CLEAR(core->st0, C_BIT);
    } else
    {
        if (carry == 1)
        {
            SET(core->st0, C_BIT);
        } else
            CLEAR(core->st0, C_BIT);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_impyl_acc_xt_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    uint64_t result;
    volatile int *reg = NULL;

    insn = (insn >> 16);
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        int addr_1 = c28_memory_read(core, core->data_addr);
        int addr_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (addr_2 << 16) | addr_1;
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg |= c28_memory_read(core, core->sp_count + 1) << 16;
    }
    if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }

    result = core->xt * (*reg);
    core->acc = result & 0xffffffff;

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_or_loc16_ax(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int a8 = fields->a8;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        if (a8 == 0)
        {
            *reg |= core->al;
            c28_memory_write(core, core->sp_count, *reg);
        } else
        {
            *reg |= core->ah;
            c28_memory_write(core, core->sp_count, *reg);
        }
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        if (a8 == 0)
        {
            *reg |= core->al;
            c28_memory_write(core, core->data_addr, *reg);
        } else
        {
            *reg |= core->ah;
            c28_memory_write(core, core->data_addr, *reg);
        }
    } else if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
        if (a8 == 0)
        {
            *reg |= core->al;
            c28_memory_write(core, core->data_addr, *reg);
        } else
        {
            *reg |= core->ah;
            c28_memory_write(core, core->data_addr, *reg);
        }
    } else
    {
        if (a8 == 0)
        {
            *reg |= core->al;
        } else
        {
            *reg |= core->ah;
        }
    }

    int z = bit(*reg, 15);

    /* Zero conditions */
    if (*reg == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (z)
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_sub_acc_16bit(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int shift = BITS(0, 3);
    int imm16 = fields->imm16;
    int x = bit(core->acc, 31);

    core->acc -= (imm16 << shift);
    int z = bit(core->acc, 31);

    judge_flags_modes(core, insn);

    /* Carry conditions */
    if (x && (z == 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_neg_acc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int ovm = core->st0 & (0x1 << 1);

    if (core->acc == 0x80000000)
    {
        SET(core->st0, V_BIT);
        if (ovm == 1)
        {
            core->acc = 0x7FFFFFFF;
        } else
            core->acc = 0x80000000;
    } else
        core->acc = -(core->acc);
    judge_flags_modes(core, insn);

    /* Carry conditions */
    if (core->acc == 0)
    {
        SET(core->st0, C_BIT);
    } else
        CLEAR(core->st0, C_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_mpyb_acc_t_imm8(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    unsigned int imm8 = fields->offset0_7;

    core->acc = core->th * imm8;

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_mpy_p_t_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    core->p = core->th * (*reg);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_mpyu_acc_t_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    core->acc = core->th * (*reg);
    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_asrl_acc_t(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int shift;
    int right_bit;
    int off_value = 0;
    int off_num;

    shift = core->th & 0x1f;
    right_bit = bit(core->acc, 31);
    if (right_bit)
    {
        for (off_num = 0; off_num < shift; off_num++)
            off_value |= 0x1 << (31 - off_num);
    }
    core->acc = (core->acc >> shift) | off_value;
    int carry = (core->acc >> shift) & 0x1;

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Carry conditions */
    if (shift == 0)
    {
        CLEAR(core->st0, C_BIT);
    } else
    {
        if (carry == 1)
        {
            SET(core->st0, C_BIT);
        } else
            CLEAR(core->st0, C_BIT);
    }
    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_norm_acc_xar(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int n = BITS(0, 2);
    int flag = BITS(3, 3);

    if (core->acc != 0)
    {
        if (((core->acc & (0x1 << 31)) ^ (core->acc & (0x1 << 30))) == 0)
        {
            core->acc = core->acc << 1;
            CLEAR(core->st0, TC_BIT);
            if (flag == 1)
            {
                core->xar[n].xar += 1;
            } else
                core->xar[n].xar -= 1;
        } else
            SET(core->st0, TC_BIT);
    } else
        SET(core->st0, TC_BIT);

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    if (core->rptc == 0)
    {
        core->insn_length = 1;
        core->pc += 1;
        return 0;
    } else
        while (core->rptc)
        {
            core->rptc--;
            exec_norm_acc_xar(core, insn, fields);
        }
}
static int exec_mpyu_p_t_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    core->p = core->th * (*reg);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_addcu_acc_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int carry;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    carry = bit(core->st0, 3);
    int x = bit(core->acc, 31);

    core->acc = core->acc + (*reg) + carry;
    int z = bit(core->acc, 31);

    judge_flags_modes(core, insn);
    /* Carry conditions */
    if (x && (z == 0) || (!x && z))
    {
        SET(core->st0, C_BIT);
    } else
        CLEAR(core->st0, C_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_xor_loc16_16bit(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int imm16 = fields->imm16;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg ^= imm16;
        c28_memory_write(core, core->data_addr, *reg);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg ^= imm16;
        c28_memory_write(core, core->sp_count, *reg);
    } else if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg ^= imm16;
        c28_memory_write(core, core->data_addr, *reg);
    } else
        *reg ^= imm16;
    /* Zero conditions */
    if (*reg == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (*reg & (0x1 << 15))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_subu_acc_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int imm16 = fields->imm16;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    int x = bit(core->acc, 31);

    core->acc -= *reg;
    int z = bit(core->acc, 31);

    judge_flags_modes(core, insn);
    /* Carry conditions */
    if (x && (z == 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);

    if (core->rptc == 0)
    {
        core->insn_length = 1;
        core->pc += 1;
        return 0;
    } else
        while (core->rptc)
        {
            core->rptc--;
            return 0;
        }
}
static int exec_mov_loc16_acc_shift(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int op1 = BITS(8, 15);
    int op2 = BITS(11, 15);
    int op3 = BITS(0, 15);
    int mode_1;
    int mode_2;
    int mode_3;
    int shift;
    int data;
    volatile uint16_t *reg = NULL;

    if (op3 == 0x562d)
    {
        insn = insn >> 16;
        mode_1 = BITS(0, 7);
        mode_2 = BITS(6, 7);
        mode_3 = BITS(3, 5);
        shift = BITS(8, 10) + 1;
    } else
    {
        mode_1 = fields->cond0_7;
        mode_2 = fields->mode6_7;
        mode_3 = fields->mode3_5;
        if (op1 == 0xb1)
        {
            shift = 1;
        } else
            shift = BITS(8, 10) + 1;
    }
    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        data = core->acc << shift & 0xffff;
        c28_memory_write(core, core->data_addr, data);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        data = core->acc << shift & 0xffff;
        c28_memory_write(core, core->sp_count, data);
    } else if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        data = core->acc << shift & 0xffff;
        c28_memory_write(core, core->data_addr, data);
    } else
    {
        *reg = core->acc << shift & 0xffff;
    }
    if (loc == 9 || loc == 8)
    {
        /* Zero conditions */
        if (*reg == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (*reg & (0x1 << 16))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);
    }

    if (op2 == 0x17)
    {
        core->insn_length = 1;
        core->pc += 1;
        return 0;
    } else if (op3 == 0x562d)
    {
        if (core->rptc == 0)
        {
            core->insn_length = 2;
            core->pc += 2;
            return 0;
        } else
            while (core->rptc)
            {
                core->rptc--;
                return 0;
            }
    } else if (op1 == 0xb1)
    {
        if (core->rptc == 0)
        {
            core->insn_length = 1;
            core->pc += 1;
            return 0;
        } else
            while (core->rptc)
            {
                core->rptc--;
                return 0;
            }
    } else
    {
        printf("In %s, decode error instruction 0x%x at 0x%x\n", __FUNCTION__, insn, core->pc);
    }
}
static int exec_mov_loc16_p(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile uint16_t *reg = NULL;
    int shift, data;

    shift = (core->st0 & 0x380) >> 7;
    if (shift == 0)
    {
        data = core->p << 1;
    } else if (shift == 1)
    {
        data = core->p;
    } else
    {
        data = core->p >> (shift - 1);
    }
    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr, data);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count, data);
    } else if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        c28_memory_write(core, core->data_addr, data);
    } else
    {
        *reg = data;
    }
    if (core->rptc == 0)
    {
        core->insn_length = 1;
        core->pc += 1;
        return 0;
    } else
        while (core->rptc)
        {
            core->rptc--;
            return 0;
        }
}

static int exec_mov_loc16_t(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile uint16_t *reg = NULL;
    int shift, data;

    shift = (core->st0 & 0x380) >> 7;
    data = core->xt >> 16;
    /*
     * if (shift == 0) {
     * data = core->th << 1;
     * } else if (shift == 1) {
     * data = core->th;
     * } else  {
     * data = core->th >> (shift - 1);
     * }
     */
    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr, data);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count, data);
    } else if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        c28_memory_write(core, core->data_addr, data);
    } else
    {
        *reg = data;
    }

    if (core->rptc == 0)
    {
        core->insn_length = 1;
        core->pc += 1;
        return 0;
    } else
    {
        while (core->rptc)
        {
            core->rptc--;
            return 0;
        }
    }
}
static int exec_sub_loc16_ax(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int ax = fields->a8;
    uint16_t data;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    int x = bit(*reg, 15);

    if (ax == 0)
    {
        if (mode_2 == 0)
        {
            data = c28_memory_read(core, core->data_addr);
            data = data - core->al;
            c28_memory_write(core, core->data_addr, data);
        } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
        {
            data = c28_memory_read(core, core->sp_count);
            data = data - core->al;
            c28_memory_write(core, core->sp_count, data);
        } else if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
        {
            data = c28_memory_read(core, core->data_addr);
            data = data - core->al;
            c28_memory_write(core, core->data_addr, data);
        } else
        {
            data = *reg - core->al;
            *reg = data;
        }
    } else
    {
        if (mode_2 == 0)
        {
            data = c28_memory_read(core, core->data_addr);
            data = data - core->ah;
            c28_memory_write(core, core->data_addr, data);
        } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
        {
            data = c28_memory_read(core, core->sp_count);
            data = data - core->ah;
            c28_memory_write(core, core->sp_count, data);
        } else if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
        {
            data = c28_memory_read(core, core->data_addr);
            data = data - core->ah;
            c28_memory_write(core, core->data_addr, data);
        } else
        {
            data = *reg - core->ah;
            *reg = data;
        }
    }
    int z = bit(data, 15);

    /* Zero conditions */
    if (data == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (data & (0x1 << 15))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Carry conditions */
    if (x && (z == 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_sfr_acc_t(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int shift;
    int carry;

    shift = core->th & 0xf;

    carry = (core->acc >> (shift - 1)) & 0x1;
    core->acc = core->acc >> shift;

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);
    /* Carry conditions */
    if (shift == 0)
    {
        CLEAR(core->st0, C_BIT);
    } else
    {
        if (carry)
        {
            SET(core->st0, C_BIT);
        } else
            CLEAR(core->st0, C_BIT);
    }
    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_not_ax(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int ax = 0;

    ax = BITS(0, 0);

    if (ax == 0)
    {
        core->al ^= 0xffff;
        /* Negative conditions */
        if (core->al & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Zero conditions */
        if (core->al == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);
    } else
    {
        core->ah ^= 0xffff;
        /* Negative conditions */
        if (core->ah & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Zero conditions */
        if (core->ah == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);
    }
    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_add_loc16_ax(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int ax = fields->a8;
    uint16_t data;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (ax == 0)
    {
        if (mode_2 == 0)
        {
            data = c28_memory_read(core, core->data_addr);
            data = data + core->al;
            c28_memory_write(core, core->data_addr, data);
        } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
        {
            data = c28_memory_read(core, core->sp_count);
            data = data + core->al;
            c28_memory_write(core, core->sp_count, data);
        } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
        {
            data = c28_memory_read(core, core->data_addr);
            data = data + core->al;
            c28_memory_write(core, core->data_addr, data);
        } else
        {
            data = *reg + core->al;
            *reg = data;
        }
    } else
    {
        if (mode_2 == 0)
        {
            data = c28_memory_read(core, core->data_addr);
            data = data + core->ah;
            c28_memory_write(core, core->data_addr, data);
        } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
        {
            data = c28_memory_read(core, core->sp_count);
            data = data + core->ah;
            c28_memory_write(core, core->sp_count, data);
        } else if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
        {
            data = c28_memory_read(core, core->data_addr);
            data = data + core->ah;
            c28_memory_write(core, core->data_addr, data);
        } else
        {
            data = *reg + core->ah;
            *reg = data;
        }
    }

    /* Zero conditions */
    if (data == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (data & (0x1 << 15))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_xor_loc16_ax(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int ax = fields->a8;
    uint16_t data;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    int x = bit(*reg, 15);

    if (ax == 0)
    {
        if (mode_2 == 0)
        {
            data = c28_memory_read(core, core->data_addr);
            data = data + core->al;
            c28_memory_write(core, core->data_addr, data);
        } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
        {
            data = c28_memory_read(core, core->sp_count);
            data = data + core->al;
            c28_memory_write(core, core->sp_count, data);
        } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
        {
            data = c28_memory_read(core, core->data_addr);
            data = data + core->al;
            c28_memory_write(core, core->data_addr, data);
        } else
        {
            data = *reg + core->al;
            *reg = data;
        }
    } else
    {
        if (mode_2 == 0)
        {
            data = c28_memory_read(core, core->data_addr);
            data = data + core->ah;
            c28_memory_write(core, core->data_addr, data);
        } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
        {
            data = c28_memory_read(core, core->sp_count);
            data = data + core->ah;
            c28_memory_write(core, core->sp_count, data);
        } else if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
        {
            data = c28_memory_read(core, core->data_addr);
            data = data + core->ah;
            c28_memory_write(core, core->data_addr, data);
        } else
        {
            data = *reg + core->ah;
            *reg = data;
        }
    }

    int z = bit(data, 15);

    /* Zero conditions */
    if (data == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (data & (0x1 << 15))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Carry conditions */
    if (x && (z == 0) || (!x && z))
    {
        SET(core->st0, C_BIT);
    } else
        CLEAR(core->st0, C_BIT);

    /* Overflow conditions */
    if (data == NEGATIVE_OVERFLOW_BIT16)
        SET(core->st0, V_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_abstc_acc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int tc = bit(core->st0, TC_BIT);

    if (core->acc == NEGATIVE_OVERFLOW)
    {
        SET(core->st0, V_BIT);
        if (core->st0 & OVM)
            core->acc = POSITIVE_OVERFLOW;
        else
            core->acc = NEGATIVE_OVERFLOW;
        if (tc)
            CLEAR(core->st0, TC_BIT);
        else
            SET(core->st0, TC_BIT);
    } else
    {
        if (core->acc & (0x1 << 31))
        {
            core->acc = (-core->acc);
            if (tc)
                CLEAR(core->st0, TC_BIT);
            else
                SET(core->st0, TC_BIT);
        }
    }
    judge_flags_modes(core, insn);

    /* Carry conditions */
    CLEAR(core->st0, C_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_subcul_acc_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int temp;
    int loc = 0;
    int loc32 = TRUE;
    int data1, data2, data;
    int p_flag = bit(core->p, 31);
    volatile int *reg = NULL;

    insn = insn >> 16;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        data1 = c28_memory_read(core, core->data_addr);
        data2 = c28_memory_read(core, core->data_addr + 1);
        data = (data2 << 16) | data1;
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        data1 = c28_memory_read(core, core->sp_count);
        data2 = c28_memory_read(core, core->sp_count + 1);
        data = (data2 << 16) | data1;
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        data1 = c28_memory_read(core, core->data_addr);
        data2 = c28_memory_read(core, core->data_addr + 1);
        data = (data2 << 16) | data1;
    } else
        data = *reg;

    temp = (core->acc << 1) + p_flag - data;
    int x = bit(temp, 31);

    if (temp < 0)
    {
        core->acc = (core->acc << 1) + p_flag;
        core->p = core->p << 1;
    } else
    {
        core->acc = temp;
        core->p = (core->p << 1) + 1;
    }
    int z = bit(core->acc, 31);

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Carry conditions */
    if (x && (z == 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);

    if (core->rptc == 0)
    {
        core->insn_length = 2;
        core->pc += 2;
        return 0;
    } else
        while (core->rptc)
        {
            core->rptc--;
            core->insn_length = 2;
            return 0;
        }
}
static int exec_negtc_acc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int tc = bit(core->st0, TC_BIT);
    int ovm = bit(core->st0, OVM_BIT);

    if (tc)
    {
        if (core->acc == 0x80000000)
        {
            SET(core->st0, V_BIT);
            if (ovm == 1)
            {
                core->acc = 0x7FFFFFFF;
            } else
                core->acc = 0x80000000;
        } else
            core->acc = -(core->acc);

        if (core->acc == 0)
        {
            SET(core->st0, C_BIT);
        } else
            CLEAR(core->st0, C_BIT);

        judge_flags_modes(core, insn);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_mpyb_p_t_imm8(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    unsigned int imm8 = fields->offset0_7;

    core->p = core->th * imm8;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_mpy_p_loc16_imm16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int imm16 = fields->imm16;
    volatile int *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    core->p = (*reg) * imm16;

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_and_ier(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int imm16 = fields->imm16;

    core->ier &= imm16;

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_push_arh1_arh0(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    c28_memory_write(core, core->sp, core->xar[0].arh);
    c28_memory_write(core, core->sp + 1, core->xar[1].arh);
    core->sp += 2;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_pop_arh1_arh0(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    core->sp -= 2;
    core->xar[0].arh = c28_memory_read(core, core->sp);
    core->xar[1].arh = c28_memory_read(core, core->sp + 1);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_nasp(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if ((core->st1 >> SPA_BIT) & 0x1)
    {
        core->sp -= 1;
        CLEAR(core->st1, SPA_BIT);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_iret(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int idle;

    core->sp -= 2;
    core->pc = c28_memory_read(core, core->sp) | (c28_memory_read(core, core->sp + 1) << 16);

    core->sp -= 2;
    core->ier = c28_memory_read(core, core->sp);
    core->dbgstat = c28_memory_read(core, core->sp + 1);

    core->sp -= 2;
    core->st1 = c28_memory_read(core, core->sp);
    core->dp = c28_memory_read(core, core->sp + 1);

    core->sp -= 2;
    core->xar[0].ar = c28_memory_read(core, core->sp);
    core->xar[1].ar = c28_memory_read(core, core->sp + 1);

    core->sp -= 2;
    core->pl = c28_memory_read(core, core->sp);
    core->ph = c28_memory_read(core, core->sp + 1);

    core->sp -= 2;
    core->al = c28_memory_read(core, core->sp);
    core->ah = c28_memory_read(core, core->sp + 1);

    core->sp -= 2;
    core->st0 = c28_memory_read(core, core->sp);
    core->th = c28_memory_read(core, core->sp + 1);

    core->sp -= 1;
    core->rptc = core->rptc_ret;
    core->insn_length = 0;

    idle = bit(core->st1, IDLESTAT_BIT);
    if (idle)
    {
        CLEAR(core->st1, IDLESTAT_BIT);
        core->pc += 1;
    }

    return 0;
}
static int exec_estop0(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    core->pc += 1;
    core->insn_length = 1;

    return 0;
}

static int exec_estop1(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    core->pc += 1;
    core->insn_length = 1;

    return 0;
}

static int exec_or_ier(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int imm16 = fields->imm16;

    core->ier |= imm16;

    core->pc += 2;
    core->insn_length = 2;

    return 0;
}

static int exec_or_ifr(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int imm16 = fields->imm16;

    core->ifr |= imm16;

    core->pc += 2;
    core->insn_length = 2;

    return 0;
}
static int exec_movb_ax_LSB_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int op = fields->a8;
    uint8_t mem8_data;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if (mode_2 == 0x3)
    {
        if (mode_3 % 2 == 0)
        {
            mem8_data = c28_memory_read_byte_hl(core, core->data_addr, LSB_ADDR) & 0xff;
            if (op == 0)
                core->al_LSB = mem8_data;
            else
                core->ah_LSB = mem8_data;
        } else
        {
            mem8_data = c28_memory_read_byte_hl(core, core->data_addr, MSB_ADDR) & 0xff;
            if (op == 0)
                core->al_LSB = mem8_data;
            else
                core->ah_LSB = mem8_data;
        }
        if (op == 0)
        {
            /* Zero conditions */
            if (core->al == 0)
            {
                SET(core->st0, Z_BIT);
            } else
                CLEAR(core->st0, Z_BIT);

            /* Negative conditions */
            if (core->al & (0x1 << 15))
            {
                SET(core->st0, N_BIT);
            } else
                CLEAR(core->st0, N_BIT);
        } else
        {
            /* Zero conditions */
            if (core->ah == 0)
            {
                SET(core->st0, Z_BIT);
            } else
                CLEAR(core->st0, Z_BIT);

            /* Negative conditions */
            if (core->ah & (0x1 << 15))
            {
                SET(core->st0, N_BIT);
            } else
                CLEAR(core->st0, N_BIT);
        }
    } else
    {
        if (mode_2 == 0)
        {
            mem8_data = c28_memory_read_byte_hl(core, core->data_addr, LSB_ADDR) & 0xff;
        } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
        {
            mem8_data = c28_memory_read_byte_hl(core, core->sp_count, LSB_ADDR) & 0xff;
        } else if ((mode_2 == 0x2) && (mode_3 < 0x4))
        {
            mem8_data = c28_memory_read_byte_hl(core, core->data_addr, LSB_ADDR) & 0xff;
        } else
        {
            mem8_data = *reg & 0xff;
        }
        if (op == 0)
        {
            core->al_LSB = mem8_data;
            core->al_MSB = 0;
            /* Zero conditions */
            if (core->al == 0)
            {
                SET(core->st0, Z_BIT);
            } else
                CLEAR(core->st0, Z_BIT);

            /* Negative conditions */
            if (core->al & (0x1 << 15))
            {
                SET(core->st0, N_BIT);
            } else
                CLEAR(core->st0, N_BIT);
        } else
        {
            core->ah_LSB = mem8_data;
            core->ah_MSB = 0;
            /* Zero conditions */
            if (core->ah == 0)
            {
                SET(core->st0, Z_BIT);
            } else
                CLEAR(core->st0, Z_BIT);

            /* Negative conditions */
            if (core->ah & (0x1 << 15))
            {
                SET(core->st0, N_BIT);
            } else
                CLEAR(core->st0, N_BIT);
        }
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_lsr64_acc_p_t_shift(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int shift;
    int carry;
    uint64_t temp64_value;

    shift = BITS(0, 3) + 1;
    carry = (core->p >> shift) & 0x1;

    temp64_value = ((((uint64_t) core->acc) << 32) | core->p) >> shift;
    core->acc = temp64_value >> 32;
    core->p = temp64_value & 0xFFFFFFFF;

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
    {
        CLEAR(core->st0, N_BIT);
    }

    /* Zero conditions */
    if (core->acc == 0 || core->p == 0)
    {
        SET(core->st0, Z_BIT);
    } else
    {
        CLEAR(core->st0, Z_BIT);
    }
    /* Carry conditions */
    if (shift == 0)
    {
        CLEAR(core->st0, C_BIT);
    } else
    {
        if (carry)
        {
            SET(core->st0, C_BIT);
        } else
        {
            CLEAR(core->st0, C_BIT);
        }
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_lsr64_acc_p_t(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int shift;
    int carry;
    uint64_t temp64_value;

    shift = core->th & 0x3f;
    carry = core->p & (0x1 << shift);

    temp64_value = ((((uint64_t) core->acc) << 32) | core->p) >> shift;
    core->acc = temp64_value >> 32;
    core->p = temp64_value & 0xFFFFFFFF;

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Zero conditions */
    if (core->acc == 0 || core->p == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);
    /* Carry conditions */
    if (shift == 0)
    {
        CLEAR(core->st0, C_BIT);
    } else
    {
        if (carry)
        {
            SET(core->st0, C_BIT);
        } else
            CLEAR(core->st0, C_BIT);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_neg64_acc_p(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int ovm = core->st0 & (0x1 << 1);

    if (core->acc == 0x80000000 && core->p == 0x0)
    {
        SET(core->st0, V_BIT);
        if (ovm == 1)
        {
            core->acc = 0x7FFFFFFF;
            core->p = 0xFFFFFFFF;
        } else
        {
            core->acc = 0x80000000;
            core->p = 0;
        }
    } else
    {
        core->acc = -core->acc;
        core->p = -core->p;
    }

    if (core->acc == 0 && core->p == 0)
    {
        SET(core->st0, C_BIT);
        SET(core->st0, Z_BIT);
    } else
    {
        CLEAR(core->st0, C_BIT);
        CLEAR(core->st0, Z_BIT);
    }
    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_cmp64_acc_p(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int v_flag, n_flag;

    v_flag = core->st0 & V;
    n_flag = (core->acc >> 31) & 0x1;

    if (v_flag && n_flag)
    {
        CLEAR(core->st0, N_BIT);
    } else
        SET(core->st0, N_BIT);

    if (v_flag && n_flag == 0)
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    if (core->acc == 0x80000000 && core->p == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    CLEAR(core->st0, V_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_mov_ovc_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);
    uint16_t mem16_data;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }

    mem16_data = *reg & 0xfc00;

    core->st0 = core->st0 & 0x3ff | mem16_data;

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_sat64_acc_p(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int ovc = (core->st0 & 0xfc00) >> 10;

    if (ovc > 0)
    {
        core->acc = 0x7FFFFFFF;
        core->p = 0xFFFFFFFF;
        SET(core->st0, V_BIT);
    } else if (ovc < 0)
    {
        core->acc = 0x80000000;
        core->p = 0x0;
        SET(core->st0, V_BIT);
    } else
    {
        CLEAR(core->st0, V_BIT);
    }

    CLEAR(core->st0, C_BIT);

    /* Zero conditions */
    if (core->acc == 0 && core->p == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc >> 31 & 0x1)
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    core->st0 = core->st0 & 0x3ff;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_zapa(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    core->acc = 0;
    core->p = 0;
    core->st0 = core->st0 & 0x3ff;
    SET(core->st0, N_BIT);
    CLEAR(core->st0, Z_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_addul_p_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int loc = 0;
    int loc32 = TRUE;
    int result;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);
    volatile int *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    int x = bit(core->p, 31);

    if (mode_2 == 0)
    {
        int addr_1 = c28_memory_read(core, core->data_addr);
        int addr_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (addr_2 << 16) | addr_1;
        core->p += *reg;
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        int data1 = c28_memory_read(core, core->sp_count);
        int data2 = c28_memory_read(core, core->sp_count + 1);

        *reg = (data2 << 16) | data1;
        core->p += *reg;
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr + 1);
        *reg = (*reg << 16) | c28_memory_read(core, core->data_addr);
        core->p += *reg;
    } else
        core->p += *reg;

    int z = bit(core->p, 31);

    /* Carry conditions */
    if (x && (z == 0) || (!x && z))
    {
        SET(core->st0, C_BIT);
    } else
        CLEAR(core->st0, C_BIT);

    /* Zero conditions */
    if (core->p == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->p & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Overflow conditions */
    if (core->p == NEGATIVE_OVERFLOW)
        SET(core->st0, V_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}

static int exec_addul_acc_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    insn = (insn >> 16) | ((insn & 0xffff) << 16);
    int loc = 0;
    int loc32 = TRUE;
    int result;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);
    volatile int *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    int x = bit(core->acc, 31);

    if (mode_2 == 0)
    {
        int addr_1 = c28_memory_read(core, core->data_addr);
        int addr_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (addr_2 << 16) | addr_1;
        core->acc += *reg;
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        int data1 = c28_memory_read(core, core->sp_count);
        int data2 = c28_memory_read(core, core->sp_count + 1);

        *reg = (data2 << 16) | data1;
        core->acc += *reg;
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr + 1);
        *reg = (*reg << 16) | c28_memory_read(core, core->data_addr);
        core->acc += *reg;
    } else
    {
        core->acc += *reg;
    }

    int z = bit(core->acc, 31);

    /* Carry conditions */
    if (x && (z == 0) || (!x && z))
    {
        SET(core->st0, C_BIT);
    } else
    {
        CLEAR(core->st0, C_BIT);
    }

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
    {
        CLEAR(core->st0, Z_BIT);
    }

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
    {
        CLEAR(core->st0, N_BIT);
    }

    /* Overflow conditions */
    if (core->acc == NEGATIVE_OVERFLOW)
    {
        SET(core->st0, V_BIT);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}

static int exec_movb_loc16_ax_LSB(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int op = fields->a8;
    uint8_t mem8_data;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0x3)
    {
        if (mode_3 % 2 == 0)
        {
            if (op == 0)
                mem8_data = core->al_LSB & 0xff;
            else
                mem8_data = core->ah_LSB & 0xff;
            c28_memory_write_byte_hl(core, core->data_addr, mem8_data, LSB_ADDR);
        } else
        {
            if (op == 0)
                mem8_data = core->al_LSB & 0xff;
            else
                mem8_data = core->ah_LSB & 0xff;

            c28_memory_write_byte_hl(core, core->data_addr, mem8_data, MSB_ADDR);
        }
        if (op == 0)
        {
            /* Zero conditions */
            if (core->al == 0)
            {
                SET(core->st0, Z_BIT);
            } else
                CLEAR(core->st0, Z_BIT);

            /* Negative conditions */
            if (core->al & (0x1 << 15))
            {
                SET(core->st0, N_BIT);
            } else
                CLEAR(core->st0, N_BIT);
        } else
        {
            /* Zero conditions */
            if (core->ah == 0)
            {
                SET(core->st0, Z_BIT);
            } else
                CLEAR(core->st0, Z_BIT);

            /* Negative conditions */
            if (core->ah & (0x1 << 15))
            {
                SET(core->st0, N_BIT);
            } else
                CLEAR(core->st0, N_BIT);
        }
    } else
    {
        if (mode_2 == 0)
        {
            if (op == 0)
            {
                mem8_data = core->al_LSB & 0xff;
            } else
            {
                mem8_data = core->ah_LSB & 0xff;
            }
            c28_memory_write_byte_hl(core, core->data_addr, mem8_data, LSB_ADDR);
        } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
        {
            if (op == 0)
            {
                mem8_data = core->al_LSB & 0xff;
            } else
            {
                mem8_data = core->ah_LSB & 0xff;
            }
            c28_memory_write_byte_hl(core, core->sp_count, mem8_data, LSB_ADDR);
        } else if ((mode_2 == 0x2) && (mode_3 < 0x4))
        {
            if (op == 0)
            {
                mem8_data = core->al_LSB & 0xff;
            } else
            {
                mem8_data = core->ah_LSB & 0xff;
            }
            c28_memory_write_byte_hl(core, core->data_addr, mem8_data, LSB_ADDR);
        } else
        {
            if (op == 0)
            {
                mem8_data = core->al_LSB & 0xff;
            } else
            {
                mem8_data = core->ah_LSB & 0xff;
            }
            *reg = mem8_data;
        }
        if (op == 0)
        {
            /* Zero conditions */
            if (core->al == 0)
            {
                SET(core->st0, Z_BIT);
            } else
                CLEAR(core->st0, Z_BIT);

            /* Negative conditions */
            if (core->al & (0x1 << 15))
            {
                SET(core->st0, N_BIT);
            } else
                CLEAR(core->st0, N_BIT);
        } else
        {
            /* Zero conditions */
            if (core->ah == 0)
            {
                SET(core->st0, Z_BIT);
            } else
                CLEAR(core->st0, Z_BIT);

            /* Negative conditions */
            if (core->ah & (0x1 << 15))
            {
                SET(core->st0, N_BIT);
            } else
                CLEAR(core->st0, N_BIT);
        }
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_movb_ax_MSB_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int op = fields->a8;
    uint8_t mem8_data;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0x3)
    {
        if (mode_3 % 2 == 0)
        {
            mem8_data = c28_memory_read_byte_hl(core, core->data_addr, LSB_ADDR) & 0xff;
            if (op == 0)
                core->al_MSB = mem8_data;
            else
                core->ah_MSB = mem8_data;
        } else
        {
            mem8_data = c28_memory_read_byte_hl(core, core->data_addr, MSB_ADDR) & 0xff;
            if (op == 0)
                core->al_MSB = mem8_data;
            else
                core->ah_MSB = mem8_data;
        }
        if (op == 0)
        {
            /* Zero conditions */
            if (core->al == 0)
            {
                SET(core->st0, Z_BIT);
            } else
                CLEAR(core->st0, Z_BIT);

            /* Negative conditions */
            if (core->al & (0x1 << 15))
            {
                SET(core->st0, N_BIT);
            } else
                CLEAR(core->st0, N_BIT);
        } else
        {
            /* Zero conditions */
            if (core->ah == 0)
            {
                SET(core->st0, Z_BIT);
            } else
                CLEAR(core->st0, Z_BIT);

            /* Negative conditions */
            if (core->ah & (0x1 << 15))
            {
                SET(core->st0, N_BIT);
            } else
                CLEAR(core->st0, N_BIT);
        }
    } else
    {
        if (mode_2 == 0)
        {
            mem8_data = c28_memory_read_byte_hl(core, core->data_addr, LSB_ADDR) & 0xff;
        } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
        {
            mem8_data = c28_memory_read_byte_hl(core, core->sp_count, LSB_ADDR) & 0xff;
        } else if ((mode_2 == 0x2) && (mode_3 < 0x4))
        {
            mem8_data = c28_memory_read_byte_hl(core, core->data_addr, LSB_ADDR) & 0xff;
        } else
        {
            mem8_data = *reg & 0xff;
        }

        if (op == 0)
        {
            core->al_MSB = mem8_data;
        } else
        {
            core->ah_MSB = mem8_data;
        }
        if (op == 0)
        {
            /* Zero conditions */
            if (core->al == 0)
            {
                SET(core->st0, Z_BIT);
            } else
                CLEAR(core->st0, Z_BIT);

            /* Negative conditions */
            if (core->al & (0x1 << 15))
            {
                SET(core->st0, N_BIT);
            } else
                CLEAR(core->st0, N_BIT);
        } else
        {
            /* Zero conditions */
            if (core->ah == 0)
            {
                SET(core->st0, Z_BIT);
            } else
                CLEAR(core->st0, Z_BIT);

            /* Negative conditions */
            if (core->ah & (0x1 << 15))
            {
                SET(core->st0, N_BIT);
            } else
                CLEAR(core->st0, N_BIT);
        }
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_movb_loc16_ax_MSB(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int op = fields->a8;
    uint8_t mem8_data;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0x3)
    {
        if (mode_3 % 2 == 0)
        {
            if (op == 0)
                mem8_data = core->al_MSB & 0xff;
            else
                mem8_data = core->ah_MSB & 0xff;

            c28_memory_write_byte_hl(core, core->data_addr, mem8_data, LSB_ADDR);
        } else
        {
            if (op == 0)
                mem8_data = core->al_MSB & 0xff;
            else
                mem8_data = core->ah_MSB & 0xff;

            c28_memory_write_byte_hl(core, core->data_addr, mem8_data, LSB_ADDR);
        }
        if (op == 0)
        {
            /* Zero conditions */
            if (core->al == 0)
            {
                SET(core->st0, Z_BIT);
            } else
                CLEAR(core->st0, Z_BIT);

            /* Negative conditions */
            if (core->al & (0x1 << 15))
            {
                SET(core->st0, N_BIT);
            } else
                CLEAR(core->st0, N_BIT);
        } else
        {
            /* Zero conditions */
            if (core->ah == 0)
            {
                SET(core->st0, Z_BIT);
            } else
                CLEAR(core->st0, Z_BIT);

            /* Negative conditions */
            if (core->ah & (0x1 << 15))
            {
                SET(core->st0, N_BIT);
            } else
                CLEAR(core->st0, N_BIT);
        }
    } else
    {
        if (mode_2 == 0)
        {
            if (op == 0)
            {
                mem8_data = core->al_MSB & 0xff;
            } else
            {
                mem8_data = core->ah_MSB & 0xff;
            }
            c28_memory_write_byte_hl(core, core->data_addr, mem8_data, LSB_ADDR);
        } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
        {
            if (op == 0)
            {
                mem8_data = core->al_MSB & 0xff;
            } else
            {
                mem8_data = core->ah_MSB & 0xff;
            }
            c28_memory_write_byte_hl(core, core->sp_count, mem8_data, LSB_ADDR);
        } else if ((mode_2 == 0x2) && (mode_3 < 0x4))
        {
            if (op == 0)
            {
                mem8_data = core->al_MSB & 0xff;
            } else
            {
                mem8_data = core->ah_MSB & 0xff;
            }
            c28_memory_write_byte_hl(core, core->data_addr, mem8_data, LSB_ADDR);
        } else
        {
            if (op == 0)
            {
                mem8_data = core->al_MSB & 0xff;
            } else
            {
                mem8_data = core->ah_MSB & 0xff;
            }
            *reg = mem8_data;
        }
        if (op == 0)
        {
            /* Zero conditions */
            if (core->al == 0)
            {
                SET(core->st0, Z_BIT);
            } else
                CLEAR(core->st0, Z_BIT);

            /* Negative conditions */
            if (core->al & (0x1 << 15))
            {
                SET(core->st0, N_BIT);
            } else
                CLEAR(core->st0, N_BIT);
        } else
        {
            /* Zero conditions */
            if (core->ah == 0)
            {
                SET(core->st0, Z_BIT);
            } else
                CLEAR(core->st0, Z_BIT);

            /* Negative conditions */
            if (core->ah & (0x1 << 15))
            {
                SET(core->st0, N_BIT);
            } else
                CLEAR(core->st0, N_BIT);
        }
    }
    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_mpyxu_acc_t_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    core->acc = core->th * (*reg);

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}

static int exec_mpyxu_p_t_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    core->p = core->th * (*reg);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}

static int exec_lb_22bit(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    uint16_t pc_low = BITS(16, 31);
    uint8_t pc_high = BITS(0, 5);

    core->pc = pc_low | (pc_high << 16);
    core->insn_length = 2;
    return 0;
}

static int exec_and_acc_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    core->acc &= *reg;
    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    CLEAR(core->st0, N_BIT);

    if (core->rptc == 0)
    {
        core->insn_length = 1;
        core->pc += 1;
        return 0;
    } else
    {
        while (core->rptc)
        {
            core->rptc--;
            return 0;
        }
    }
}

static int exec_add_acc_bit16_shift(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int shift = BITS(0, 3);
    int bit16 = BITS(16, 31);
    int sxm = core->st0 & 0x1;

    //if sxm == 1, sign extension else zero extension

    if (sxm)
    {
        if ((bit16 >> 15) & 0x1)
        {
            core->acc += (bit16 << shift);
            core->acc |= sign_extend(shift);
        } else
        {
            core->acc += (bit16 << shift);
        }
    } else
    {
        core->acc += (bit16 << shift);
    }
    judge_flags_modes(core, insn);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_not_acc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    core->acc = core->acc ^ 0xFFFFFFFF;

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 1;
    core->pc += 1;
}
static int exec_movx_tl_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;

    insn = insn >> 16;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }

    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    core->tl = *reg;
    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_asr64_acc_p_shift(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int shift = BITS(0, 3);
    int64_t temp64_value;
    int carry = (core->p >> shift) & 0x1;

    temp64_value = ((((uint64_t) core->acc) << 32) | core->p) >> (shift + 1);
    core->acc = temp64_value >> 32;
    core->p = temp64_value & 0xFFFFFFFF;

    /* Zero conditions */
    if (temp64_value == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Carry conditions */
    if (carry == 1)
    {
        SET(core->st0, C_BIT);
    } else
        CLEAR(core->st0, C_BIT);

    core->insn_length = 1;
    core->pc += 1;
}
static int exec_asr64_acc_p_t(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int shift;
    int carry;

    // 算术右位移(有符号数的右位移)
    int64_t temp64_value;

    shift = core->th & 0x3F;            // T(5:0) register
    carry = (core->p >> shift) & 0x1;

    temp64_value = ((((uint64_t) core->acc) << 32) | core->p) >> shift;
    core->acc = temp64_value >> 32;
    core->p = temp64_value & 0xFFFFFFFF;

    /* Zero conditions */
    if (temp64_value == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Carry conditions */
    if (shift == 0)
        CLEAR(core->st0, C_BIT);
    else
    {
        if (carry == 1)
        {
            SET(core->st0, C_BIT);
        } else
            CLEAR(core->st0, C_BIT);
    }

    core->insn_length = 1;
    core->pc += 1;
}

static int exec_impyl_p_xt_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    uint64_t result;
    int pm = (core->st0 >> PM_BIT) & 0x7;
    volatile int *reg = NULL;

    insn = (insn >> 16);
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        int addr_1 = c28_memory_read(core, core->data_addr);
        int addr_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (addr_2 << 16) | addr_1;
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg |= c28_memory_read(core, core->sp_count + 1) << 16;
    }
    if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }

    result = core->xt * (*reg);
    switch (pm)
    {
        case 0:
            core->p = (result << 1) & 0xFFFFFFFF;
            break;
            //left shift 1
        case 1:
            core->p = result & 0xFFFFFFFF;
            break;
            //No shift
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            core->p = (result >> (pm - 1)) & 0xFFFFFFFF;
            break;
            //Right shift by (pm - 1)
        default:
            printf("In %s, %d, Not implement this instruction at pm = %x\n", __func__, __LINE__, pm);
            break;
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_qmpyul_p_xt_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    uint64_t result;
    volatile int *reg = NULL;

    insn = (insn >> 16);
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        int addr_1 = c28_memory_read(core, core->data_addr);
        int addr_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (addr_2 << 16) | addr_1;
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg |= c28_memory_read(core, core->sp_count + 1) << 16;
    }
    if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }

    result = core->xt * (*reg);
    core->p = result >> 32;

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_impyxul_p_xt_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    uint64_t result;
    int pm = (core->st0 >> PM_BIT) & 0x7;
    volatile int *reg = NULL;

    insn = (insn >> 16);
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        int addr_1 = c28_memory_read(core, core->data_addr);
        int addr_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (addr_2 << 16) | addr_1;
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg |= c28_memory_read(core, core->sp_count + 1) << 16;
    }
    if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }

    result = core->xt * (*reg);
    switch (pm)
    {
        case 0:
            core->p = (result << 1) & 0xFFFFFFFF;
            break;
            //left shift 1
        case 1:
            core->p = result & 0xFFFFFFFF;
            break;
            //No shift
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            core->p = (result >> (pm - 1)) & 0xFFFFFFFF;
            break;
            //Right shift by (pm - 1)
        default:
            printf("In %s, %d, Not implement this instruction at pm = %x\n", __func__, __LINE__, pm);
            break;
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_subul_acc_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    volatile int *reg = NULL;

    insn = (insn >> 16);
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        int addr_1 = c28_memory_read(core, core->data_addr);
        int addr_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (addr_2 << 16) | addr_1;
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg |= c28_memory_read(core, core->sp_count + 1) << 16;
    }
    if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }

    int x = bit(core->acc, 31);

    core->acc -= *reg;
    int z = bit(core->acc, 31);

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
    {
        CLEAR(core->st0, Z_BIT);
    }

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
    {
        CLEAR(core->st0, N_BIT);
    }

    /* Carry conditions */
    if (x && (z == 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
    {
        SET(core->st0, C_BIT);
    }

    /* Overflow conditions */
    if (core->acc == NEGATIVE_OVERFLOW_BIT16)
    {
        SET(core->st0, V_BIT);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_subul_p_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    volatile int *reg = NULL;

    insn = (insn >> 16);
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        int addr_1 = c28_memory_read(core, core->data_addr);
        int addr_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (addr_2 << 16) | addr_1;
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg |= c28_memory_read(core, core->sp_count + 1) << 16;
    }
    if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }

    int x = bit(core->p, 31);

    core->p = core->p - *reg;
    int z = bit(core->p, 31);

    /* Zero conditions */
    if (core->p == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->p & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Carry conditions */
    if (x && (z == 0) || (!x && z))
    {
        CLEAR(core->st0, C_BIT);
    } else
        SET(core->st0, C_BIT);

    /* Overflow conditions */
    if (core->p == NEGATIVE_OVERFLOW_BIT16)
        SET(core->st0, V_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_csb_acc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int sign_num = 0, i;

    for (i = 0; i < 32; i++)
    {
        if ((core->acc >> i) & 0x1)
        {
            sign_num += 1;
        }
    }
    core->th = sign_num - 1;

    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
        SET(core->st0, TC_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_qmpyl_acc_xt_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    int64_t result;
    volatile int *reg = NULL;

    insn = (insn >> 16);
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if (mode_2 == 0)
    {
        int data_1 = c28_memory_read(core, core->data_addr);
        int data_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (data_2 << 16) | data_1;
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
        *reg |= c28_memory_read(core, core->sp_count + 1) << 16;
    }
    if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }

    result = *reg * core->xt;
    core->acc = result >> 32;
    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_lsl64_acc_p_shift(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int shift = BITS(0, 3);
    uint64_t temp64_value;
    int carry = core->acc & (0x1 << (0x20 - shift));

    temp64_value = ((((uint64_t) core->acc) << 32) | core->p) << shift;
    core->acc = temp64_value >> 32;
    core->p = temp64_value & 0xFFFFFFFF;

    /* Zero conditions */
    if (temp64_value == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);

    /* Carry conditions */
    if (carry == 1)
    {
        SET(core->st0, C_BIT);
    } else
        CLEAR(core->st0, C_BIT);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_lsl64_acc_p_t(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int shift = core->th & 0x3f;
    int carry = core->acc & (0x1 << (0x20 - shift));
    uint64_t temp64_value;

    temp64_value = ((((uint64_t) core->acc) << 32) | core->p) << shift;
    core->acc = temp64_value >> 32;
    core->p = temp64_value & 0xFFFFFFFF;

    /* Zero conditions */
    if (temp64_value == 0)
    {
        SET(core->st0, Z_BIT);
    } else
    {
        CLEAR(core->st0, Z_BIT);
    }

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
    {
        CLEAR(core->st0, N_BIT);
    }

    /* Carry conditions */
    if (carry == 1)
    {
        SET(core->st0, C_BIT);
    } else
    {
        CLEAR(core->st0, C_BIT);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}

static int exec_addcl_acc_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    int c = (core->st0 >> C_BIT) & 0x1;
    int x = core->acc;
    volatile int *reg = NULL;

    insn = insn >> 16;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        int addr_1 = c28_memory_read(core, core->data_addr);
        int addr_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (addr_2 << 16) | addr_1;
        core->acc += *reg + c;
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        int data1 = c28_memory_read(core, core->sp_count);
        int data2 = c28_memory_read(core, core->sp_count + 1);

        *reg = (data2 << 16) | data1;
        core->acc += *reg + c;
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
        core->acc += *reg + c;
    } else
        core->acc += *reg + c;
    int z = core->acc;

    judge_flags_modes(core, insn);
    /* Carry conditions */
    if ((x & (0x1 << 31)) && ((z & (0x1 << 31)) == 0))
    {
        SET(core->st0, C_BIT);
    } else
        CLEAR(core->st0, C_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_minl_acc_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    volatile int *reg = NULL;
    int x, y, z;

    insn = insn >> 16;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        int addr_1 = c28_memory_read(core, core->data_addr);
        int addr_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (addr_2 << 16) | addr_1;
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        int data1 = c28_memory_read(core, core->sp_count);
        int data2 = c28_memory_read(core, core->sp_count + 1);

        *reg = (data2 << 16) | data1;
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }

    x = (core->acc >> 31) & 0x1;
    y = (*reg >> 31) & 0x1;
    z = ((core->acc - *reg) >> 31) & 0x1;
    if (core->acc > *reg)
        core->acc = *reg;

    /* Zero conditions */
    if (core->acc == *reg)
        SET(core->st0, Z_BIT);
    else
        CLEAR(core->st0, Z_BIT);
    /* Negative conditions */
    if (core->acc < *reg)
        SET(core->st0, N_BIT);
    else
        CLEAR(core->st0, N_BIT);
    /* Carry conditions */
    if ((x && y && z) || (!x && z))
        SET(core->st0, C_BIT);
    else
        CLEAR(core->st0, C_BIT);

    /* Overflow conditions */
    if (core->acc > *reg)
        SET(core->st0, V_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_maxl_acc_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    volatile int *reg = NULL;
    int x, y, z;

    insn = insn >> 16;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        int addr_1 = c28_memory_read(core, core->data_addr);
        int addr_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (addr_2 << 16) | addr_1;
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        int data1 = c28_memory_read(core, core->sp_count);
        int data2 = c28_memory_read(core, core->sp_count + 1);

        *reg = (data2 << 16) | data1;
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }

    x = (core->acc >> 31) & 0x1;
    y = (*reg >> 31) & 0x1;
    z = ((core->acc - *reg) >> 31) & 0x1;
    if (core->acc < *reg)
        core->acc = *reg;

    /* Zero conditions */
    if (core->acc == *reg)
        SET(core->st0, Z_BIT);
    else
        CLEAR(core->st0, Z_BIT);
    /* Negative conditions */
    if (core->acc < *reg)
        SET(core->st0, N_BIT);
    else
        CLEAR(core->st0, N_BIT);
    /* Carry conditions */
    if ((x && y && z) || (!x && z))
        SET(core->st0, C_BIT);
    else
        CLEAR(core->st0, C_BIT);

    /* Overflow conditions */
    if (core->acc < *reg)
        SET(core->st0, V_BIT);

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_mincul_p_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    int n = (core->st0 >> N_BIT) & 0x1;
    int z = (core->st0 >> Z_BIT) & 0x1;
    volatile int *reg = NULL;

    insn = insn >> 16;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        int addr_1 = c28_memory_read(core, core->data_addr);
        int addr_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (addr_2 << 16) | addr_1;
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        int data1 = c28_memory_read(core, core->sp_count);
        int data2 = c28_memory_read(core, core->sp_count + 1);

        *reg = (data2 << 16) | data1;
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }

    if (n == 0 && z == 0)
        core->p = *reg;

    if (n == 0 && z == 1 && (core->p > *reg))
    {
        SET(core->st0, V_BIT);
        core->p = *reg;
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_maxcul_p_loc32(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    int n = (core->st0 >> N_BIT) & 0x1;
    int z = (core->st0 >> Z_BIT) & 0x1;
    volatile int *reg = NULL;

    insn = insn >> 16;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        int addr_1 = c28_memory_read(core, core->data_addr);
        int addr_2 = c28_memory_read(core, core->data_addr + 1);

        *reg = (addr_2 << 16) | addr_1;
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        int data1 = c28_memory_read(core, core->sp_count);
        int data2 = c28_memory_read(core, core->sp_count + 1);

        *reg = (data2 << 16) | data1;
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
        *reg |= c28_memory_read(core, core->data_addr + 1) << 16;
    }

    if (n == 1 && z == 0)
        core->p = *reg;

    if (n == 0 && z == 1 && (core->p < *reg))
    {
        SET(core->st0, V_BIT);
        core->p = *reg;
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_movh_loc16_acc_shift_1(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    int result;
    volatile uint16_t *reg = NULL;

    result = core->acc >> (16 - 1);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr, result);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count, result);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        c28_memory_write(core, core->data_addr, result);
    }

    if (loc == 9 || loc == 8)
    {
        /* Zero conditions */
        if (result == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (result & (0x1 << 31))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_movh_loc16_acc_shift_2(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = TRUE;
    int result;
    volatile int *reg = NULL;

    insn = insn >> 16;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);
    int shift = BITS(8, 10);

    result = core->acc >> (16 - shift);
    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr, result);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count, result);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        c28_memory_write(core, core->data_addr, result);
    }

    if (loc == 9 || loc == 8)
    {
        /* Zero conditions */
        if (result == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (result & (0x1 << 31))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);
    }

    core->insn_length = 2;
    core->pc += 2;
    return 0;
}
static int exec_movh_loc16_p(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int pm = (core->st0 >> PM_BIT) & 0x7;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    unsigned short result;
    volatile uint16_t *reg = NULL;

    result = (core->p << pm) >> 16;
    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr, result);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count, result);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        c28_memory_write(core, core->data_addr, result);
    }

    if (loc == 9 || loc == 8)
    {
        /* Zero conditions */
        if (result == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (result & (0x1 << 31))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);
    }

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_idle(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    SET(core->st1, IDLESTAT_BIT);
}
static int exec_lc_22bit(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int temp;
    uint16_t pc_low = BITS(16, 31);
    uint8_t pc_high = BITS(0, 5);

    temp = core->pc + 2;
    c28_memory_write(core, core->sp, temp & 0xffff);

    core->sp += 1;
    c28_memory_write(core, core->sp, (temp & 0x3f0000) >> 16);

    core->sp += 1;
    core->pc = pc_low | (pc_high << 16);

    core->insn_length = 2;
    //NOT_TEST_INSTRUCTION;
    return 0;
}
static int exec_lc_xar(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int temp;

    temp = core->pc + 1;
    c28_memory_write(core, core->sp, temp & 0xffff);

    core->sp += 1;
    c28_memory_write(core, core->sp, (temp & 0x3f0000) >> 16);

    core->sp += 1;
    core->pc = core->xar[7].xar & 0x3fffff;

    core->insn_length = 1;
    NOT_TEST_INSTRUCTION;
    return 0;
}
static int exec_and_ax_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int a8 = fields->a8;
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = fields->cond0_7;
    int mode_2 = fields->mode6_7;
    int mode_3 = fields->mode3_5;
    volatile int *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    if (a8 == 0)
    {
        core->al &= *reg;
        /* Zero conditions */
        if (core->al == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (core->al & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);
    } else
    {
        core->ah &= *reg;
        /* Zero conditions */
        if (core->ah == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (core->ah & (0x1 << 15))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);
    }

    core->insn_length = 1;
    core->pc += 1;
    //      NOT_TEST_INSTRUCTION;
    return 0;
}
static int exec_asr_ax_t(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int ax = BITS(0, 0);
    int shift = core->th & 0xf;
    int carry;

    if (ax == 0)
    {
        carry = core->al & (0x1 << shift);
        core->al = core->al >> shift;
        /* Zero conditions */
        if (core->al == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (core->al & (0x1 << 31))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Carry conditions */
        if (shift == 0)
        {
            CLEAR(core->st0, C_BIT);
        } else
        {
            if (carry)
            {
                SET(core->st0, C_BIT);
            } else
                CLEAR(core->st0, C_BIT);
        }
    } else
    {
        carry = core->ah & (0x1 << shift);
        core->ah = core->ah >> shift;
        /* Zero conditions */
        if (core->ah == 0)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (core->ah & (0x1 << 31))
        {
            SET(core->st0, N_BIT);
        } else
            CLEAR(core->st0, N_BIT);

        /* Carry conditions */
        if (shift == 0)
        {
            CLEAR(core->st0, C_BIT);
        } else
        {
            if (carry)
            {
                SET(core->st0, C_BIT);
            } else
                CLEAR(core->st0, C_BIT);
        }
    }

    core->insn_length = 1;
    core->pc += 1;
    NOT_TEST_INSTRUCTION;
    return 0;
}
static int exec_max_ax_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int ax = BITS(0, 0);
    int loc = 0;
    int loc32 = FALSE;

    insn = insn >> 16;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    if (ax == 0)
    {
        if (core->al < *reg)
            core->al = *reg;
        /* Zero conditions */
        if (*reg == core->al)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        /* Overflow conditions */
        if (core->al < *reg)
        {
            SET(core->st0, N_BIT);
            SET(core->st0, V_BIT);
        } else
        {
            CLEAR(core->st0, N_BIT);
        }
    } else
    {
        if (core->ah < *reg)
            core->ah = *reg;
        /* Zero conditions */
        if (*reg == core->ah)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        /* Overflow conditions */
        if (core->ah < *reg)
        {
            SET(core->st0, N_BIT);
            SET(core->st0, V_BIT);
        } else
        {
            CLEAR(core->st0, N_BIT);
        }
    }

    core->insn_length = 2;
    core->pc += 2;
    NOT_TEST_INSTRUCTION;
    return 0;
}
static int exec_min_ax_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int ax = BITS(0, 0);
    int loc = 0;
    int loc32 = FALSE;

    insn = insn >> 16;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }

    if (ax == 0)
    {
        if (core->al > *reg)
            core->al = *reg;
        /* Zero conditions */
        if (*reg == core->al)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (core->al < *reg)
        {
            SET(core->st0, N_BIT);
        } else
        {
            CLEAR(core->st0, N_BIT);
        }
        /* Overflow conditions */
        if (core->al > *reg)
            SET(core->st0, V_BIT);
    } else
    {
        if (core->ah > *reg)
            core->ah = *reg;
        /* Zero conditions */
        if (*reg == core->ah)
        {
            SET(core->st0, Z_BIT);
        } else
            CLEAR(core->st0, Z_BIT);

        /* Negative conditions */
        if (core->ah < *reg)
        {
            SET(core->st0, N_BIT);
        } else
        {
            CLEAR(core->st0, N_BIT);
        }

        /* Overflow conditions */
        if (core->ah > *reg)
            SET(core->st0, V_BIT);
    }

    core->insn_length = 2;
    core->pc += 2;
    NOT_TEST_INSTRUCTION;
    return 0;
}
static int exec_mov_loc16_ax_cond(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int ax = BITS(0, 0);
    int loc32 = FALSE;
    int cond = BITS(24, 27);
    int cond_flag = 0;
    volatile int *reg = NULL;

    int c = core->st0 & (0x1 << 3);
    int z = core->st0 & (0x1 << 4);
    int n = core->st0 & (0x1 << 5);
    int v = core->st0 & (0x1 << 6);
    int tc = core->st0 & (0x1 << 2);

    insn = insn >> 16;
    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);

    switch (cond)
    {
        case 0x0:
            if (z == 0)
            {
                cond_flag = 1;
            }
            break;
        case 0x1:
            if (z)
            {
                cond_flag = 1;
            }
            break;
        case 0x2:
            if (z == 0 && n == 0)
            {
                cond_flag = 1;
            }
            break;
        case 0x3:
            if (n == 0)
            {
                cond_flag = 1;
            }
            break;
        case 0x4:
            if (n)
            {
                cond_flag = 1;
            }
            break;
        case 0x5:
            if (z || n)
            {
                cond_flag = 1;
            }
            break;
        case 0x6:
            if (z == 0 && c)
            {
                cond_flag = 1;
            }
            break;
        case 0x7:
            if (c)
            {
                cond_flag = 1;
            }
            break;
        case 0x8:
            if (c == 0)
            {
                cond_flag = 1;
            }
            break;
        case 0x9:
            if (z || c == 0)
            {
                cond_flag = 1;
            }
            break;
        case 0xa:
            if (v == 0)
            {
                cond_flag = 1;
            }
            break;
        case 0xb:
            if (v)
            {
                cond_flag = 1;
                CLEAR(core->st0, V_BIT);
            }
            break;
        case 0xc:
            if (tc == 0)
            {
                cond_flag = 1;
            }
            break;
        case 0xd:
            if (tc)
            {
                cond_flag = 1;
            }
            break;
        case 0xe:
        case 0xf:
            cond_flag = 1;
            break;
        default:
            break;
    }
    if (cond_flag == 1)
    {
        if (ax == 0)
        {
            if (mode_2 == 0)
            {
                c28_memory_write(core, core->data_addr, core->al);
            } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
            {
                c28_memory_write(core, core->sp_count, core->al);
            } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
            {
                c28_memory_write(core, core->data_addr, core->al);
            } else
                *reg = core->al;
        } else
        {
            if (mode_2 == 0)
            {
                c28_memory_write(core, core->data_addr, core->ah);
            } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
            {
                c28_memory_write(core, core->sp_count, core->ah);
            } else if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
            {
                c28_memory_write(core, core->data_addr, core->ah);
            } else
                *reg = core->ah;
        }

        if (loc == 8 || loc == 9)
        {
            /* Zero conditions */
            if (*reg == 0)
            {
                SET(core->st0, Z_BIT);
            } else
                CLEAR(core->st0, Z_BIT);

            /* Negative conditions */
            if (*reg & (0x1 << 15))
            {
                SET(core->st0, N_BIT);
            } else
                CLEAR(core->st0, N_BIT);
        }
    }

    core->insn_length = 2;
    core->pc += 2;
    NOT_TEST_INSTRUCTION;
    return 0;
}
static int exec_pop_arn_arn(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op = BITS(0, 3);
    int Arn = 0;

    if (op == 7)
    {
        Arn = 0;
    } else if (op == 5)
    {
        Arn = 2;
    } else if (op == 6)
    {
        Arn = 4;
    } else
    {
        printf("In %s, Decode instruction 0x%x error.\n", __FUNCTION__, insn);
    }

    core->sp -= 2;
    core->xar[Arn].ar = c28_memory_read(core, core->sp);
    core->xar[Arn + 1].ar = c28_memory_read(core, core->sp + 1);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_push_arn_arn(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op = BITS(0, 3);
    int Arn = 0;

    if (op == 0xd)
    {
        Arn = 0;
    } else if (op == 0xf)
    {
        Arn = 2;
    } else if (op == 0xc)
    {
        Arn = 4;
    } else
    {
        printf("In %s, Decode instruction 0x%x error.\n", __FUNCTION__, insn);
    }

    c28_memory_write(core, core->sp, core->xar[Arn].ar);
    c28_memory_write(core, core->sp + 1, core->xar[Arn + 1].ar);
    core->sp += 2;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_push_acc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    c28_memory_write(core, core->sp, core->al);
    c28_memory_write(core, core->sp + 1, core->ah);
    core->sp += 2;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_push_dp(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    c28_memory_write(core, core->sp, core->dp);
    core->sp += 1;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_push_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;

    //insn = insn >> 16;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);
    volatile uint16_t *reg = NULL;

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        *reg = c28_memory_read(core, core->sp_count);
    }
    if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4)))
    {
        *reg = c28_memory_read(core, core->data_addr);
    }
    // [SP] = [loc16]
    // SP  += 1
    c28_memory_write(core, core->sp, *reg);
    core->sp += 1;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_push_p(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    c28_memory_write(core, core->sp, core->pl);
    c28_memory_write(core, core->sp + 1, core->ph);
    core->sp += 2;

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_pop_acc(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    core->sp -= 2;

    core->al = c28_memory_read(core, core->sp);
    core->ah = c28_memory_read(core, core->sp + 1);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_pop_dp(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    core->sp -= 1;
    core->dp = c28_memory_read(core, core->sp);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_pop_loc16(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int loc = 0;
    int loc32 = FALSE;
    int mode_1 = BITS(0, 7);
    int mode_2 = BITS(6, 7);
    int mode_3 = BITS(3, 5);
    volatile uint16_t *reg = NULL;
    int op = BITS(0, 1);

    if (op == 5)
    {
        insn = (insn >> 16);
        mode_2 = BITS(6, 7);
        mode_3 = BITS(3, 5);
    }
    core->sp -= 1;
    int data = c28_memory_read(core, core->sp);

    loc = check_insn_address_mode(core, insn, fields, loc32);
    reg = get_register(core, insn, loc);

    if (mode_2 == 0)
    {
        c28_memory_write(core, core->data_addr, data);
    } else if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7)))
    {
        c28_memory_write(core, core->sp_count, data);
    } else if ((loc < 8) && ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))))
    {
        c28_memory_write(core, core->data_addr, data);
    } else
    {
        *reg = data;
    }
    /*
     * loc = check_insn_address_mode(core, insn, fields, loc32);
     * reg = get_register(core, insn, loc);
     * 
     * if (mode_2 == 0){
     * *reg = c28_memory_read(core, core->data_addr);
     * }
     * if ((mode_2 == 1) || ((mode_2 == 0x2) && (mode_3 == 0x7))){
     * *reg = c28_memory_read(core, core->sp_count);
     * }
     * if ((mode_2 == 0x3) || ((mode_2 == 0x2) && (mode_3 < 0x4))){
     * *reg = c28_memory_read(core, core->data_addr);
     * }
     * 
     * // SP -= 1
     * // [loc16] = [SP]
     * 
     * core->sp -= 1;
     * *reg = c28_memory_read(core, core->sp);
     */

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_pop_p(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    core->sp -= 2;
    core->pl = c28_memory_read(core, core->sp);
    core->ph = c28_memory_read(core, core->sp + 1);

    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
static int exec_pop_dbgier(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    core->sp -= 1;
    core->dbgier = c28_memory_read(core, core->sp);
    core->insn_length = 1;
    core->pc += 1;
    return 0;
}
const ISEITEM insn32_decode_table[] = {
#if 0
    {"aborti", 1, 2, 0, 15, 0x1},

    {"add_sh", 1, 2, 4, 15, 0xff1},
    {"add_lc", 1, 2, 0, 15, 0x5623},

    {"addcl_acc", 1, 2, 0, 15, 0x5640},
    {"addl_acc_pm", 1, 2, 0, 15, 0x10ac},
    {"addul_acc_lc", 1, 2, 0, 15, 0x5653},

    {"adrk", 1, 2, 8, 15, 0xfc},

    {"and_acc", 1, 2, 4, 15, 0x3e0},
    {"and_acc", 1, 2, 4, 15, 0x560},
    {"and_lc_ax", 1, 2, 9, 15, 0x60},
    {"and_ax_lc", 1, 2, 9, 15, 0x67},

    {"asr64_acc_p_t", 1, 2, 0, 15, 0x562c},

    {"bar", 1, 2, 6, 15, 0x23e},
    {"bar", 1, 2, 6, 15, 0x23f},

    {"c27map", 1, 2, 0, 15, 0x563f},
    {"c27obj", 1, 2, 0, 15, 0x5636},
    {"c28addr", 1, 2, 0, 15, 0x5616},   //is an alias for clrc_amode 

    {"clrc_m0m1map", 1, 2, 0, 15, 0x563f},
    {"clrc_objmode", 1, 2, 0, 15, 0x5636},
    {"clrc_ovc", 1, 2, 0, 15, 0x565c},
    {"clrc_xf", 1, 2, 0, 15, 0x561b},

    {"cmp64_acc_p", 1, 2, 0, 15, 0x565e},

    {"cmpl_acc_p_pm", 1, 2, 0, 15, 0xff59},

    {"cmpr", 1, 2, 4, 15, 0x561},       //cmpr cond 

    {"dint", 1, 2, 0, 15, 0x3b10},

    {"dmac", 1, 2, 0, 15, 0x564b},

    {"dmov", 1, 2, 8, 15, 0xa5},

    {"eint", 1, 2, 0, 15, 0x2910},

    {"flip", 1, 2, 1, 15, 0x2b38},
    {"iack", 1, 2, 0, 15, 0x763f},

    {"imacl", 1, 2, 0, 15, 0x564d},

    {"impyal", 1, 2, 0, 15, 0x564c},
    {"impyl_p_xt", 1, 2, 0, 15, 0x5605},
    {"impysl", 1, 2, 0, 15, 0x5643},

    {"in", 1, 2, 8, 15, 0xb4},

    {"intr_1", 1, 2, 4, 15, 0x1},
    {"intr_2", 1, 2, 4, 15, 0x761},

    {"subb_xar", 2, 2, 11, 15, 0x1b, 7, 7, 1},  //subb xarn #7b 
#endif
    /*Float instruction */
    {"mov32_RaH_loc32_cond", 2, 2, 4, 15, 0xe2a, 27, 31, 0},
    {"mov32_RaH_RbH_cond", 2, 2, 4, 15, 0xe6c, 22, 31, 0},
    {"addf32_RaH_RbH_RcH", 2, 2, 0, 15, 0xe710, 25, 31, 0},
    {"mov32_loc32_RaH", 2, 2, 0, 15, 0xe203, 27, 31, 0},
    {"subf32_RaH_RbH_RcH", 2, 2, 0, 15, 0xe720, 25, 31, 0},
    {"subf32_RaH_imm16_RbH", 1, 2, 6, 15, 0x3a3},
    {"mpyf32_RaH_RbH_RcH", 2, 2, 0, 15, 0xe700, 25, 31, 0},
    {"zero_RaH", 1, 2, 3, 15, 0x1cb2},
    {"f32toi16_RaH_RbH", 2, 2, 0, 15, 0xe68c, 22, 31, 0},
    {"f32toui16_RaH_RbH", 2, 2, 0, 15, 0xe68e, 22, 31, 0},
    {"f32toi32_RaH_RbH", 2, 2, 0, 15, 0xe688, 22, 31, 0},
    {"f32toui32_RaH_RbH", 2, 2, 0, 15, 0xe68a, 22, 31, 0},
    {"i16tof32_RaH_loc16", 2, 2, 0, 15, 0xe2c8, 27, 31, 0},
    {"moviz_RaH_imm16", 1, 2, 3, 15, 0x1d00},
    {"movxi_RaH_imm16", 1, 2, 3, 15, 0x1d01},
    {"mpyf32_RaH_imm16_RbH", 1, 2, 6, 15, 0x3a1},
    {"absf32_RaH_RbH", 2, 2, 0, 15, 0xe695, 22, 31, 0},
    {"cmpf32_RaH_zero", 1, 2, 3, 15, 0x1cb4},
    {"movst0_flag", 1, 2, 8, 15, 0xad},
    {"addf32_RaH_imm16_RbH", 1, 2, 6, 15, 0x3a2},
    {"i32tof32_RaH_RbH", 2, 2, 0, 15, 0xe689, 22, 31, 0},
    {"ui32tof32_RaH_RbH", 2, 2, 0, 15, 0xe68b, 22, 31, 0},
    {"i32tof32_RaH_loc32", 2, 2, 0, 15, 0xe288, 27, 31, 0},
    {"mpyf32_RaH_RbH_RcH_subf32_RdH_ReH_RfH", 1, 2, 2, 15, 0x39d4},
    {"mpyf32_RaH_RbH_RcH_addf32_RdH_ReH_RfH", 1, 2, 2, 15, 0x39d0},
    {"negf32_RaH_RbH_cond", 2, 2, 4, 15, 0xe6a, 22, 31, 0},
    {"addf32_RdH_ReH_RfH_mov32_RaH_loc32", 1, 2, 4, 15, 0xe31},
    {"addf32_RdH_ReH_RfH_mov32_loc32_RaH", 1, 2, 4, 15, 0xe01},
    {"subf32_RdH_ReH_RfH_mov32_RaH_loc32", 1, 2, 4, 15, 0xe32},
    {"subf32_RdH_ReH_RfH_mov32_loc32_RaH", 1, 2, 4, 15, 0xe02},
    {"mov32_acc_or_xar_RaH", 1, 2, 8, 15, 0xbf},
    {"mov32_RaH_acc", 1, 2, 8, 15, 0xbd},
    {"mpyf32_RdH_ReH_RfH_mov32_RaH_loc32", 1, 2, 4, 15, 0xe30},
    {"mpyf32_RdH_ReH_RfH_mov32_loc32_RaH", 1, 2, 4, 15, 0xe00},
    {"eisqrtf32_RaH_RbH", 2, 2, 0, 15, 0xe692, 22, 31, 0},
    {"rptb_lable_loc16", 2, 2, 7, 15, 0x16a, 24, 31, 0},
    {"fracf32_RaH_RbH", 2, 2, 0, 15, 0xe6f1, 22, 31, 0},
    {"einvf32_RaH_RbH", 2, 2, 0, 15, 0xe693, 22, 31, 0},
    {"cmpf32_RaH_RbH", 2, 2, 0, 15, 0xe694, 22, 31, 0},
    {"minf32_RaH_RbH_mov32_RcH_RdH", 2, 2, 0, 15, 0xe69d, 27, 31, 0},
    {"testtf_cond", 1, 2, 4, 15, 0xe58},
    {"setflg", 1, 2, 6, 15, 0x398},     //cond 
    {"push_rb", 1, 2, 0, 15, 0xfff0},
    {"pop_rb", 1, 2, 0, 15, 0xfff1},
    {"mov32_mem32_stf", 1, 2, 0, 15, 0xe200, 24, 31, 0},
    {"MOV32 STF,mem32", 2, 2, 0, 15, 0xe280, 24, 31, 0},
    {"cmpf32_RaH_imm16", 1, 2, 3, 15, 0x1d02},

    /* Integer instruction */
    {"subr_loc16_ax", 1, 2, 9, 15, 0x75},
    {"mov_loc16", 1, 2, 8, 15, 0x28},   //mov loc16 #16bit 
    {"movl_p_loc32", 1, 2, 8, 15, 0xa3},
    {"lsl_ax_t", 1, 2, 1, 15, 0x7fb3},
    {"lsr_ax_t", 1, 2, 1, 15, 0x7fb1},
    {"spm_pm", 1, 2, 4, 15, 0xff6},
    {"c28obj", 1, 2, 0, 15, 0x561f},
    {"setc", 1, 2, 8, 15, 0x3b},        //cond 
    {"clrc_amode", 1, 2, 0, 15, 0x5616},
    {"c28map", 1, 2, 0, 15, 0x561a},
    {"clrc_mode", 1, 2, 8, 15, 0x29},   //clrc cond 
    {"clrc_ovc", 1, 2, 0, 15, 0x565c},
    {"movw_dp", 1, 2, 0, 15, 0x761f},
    {"asp", 1, 2, 0, 15, 0x761b},
    {"eallow", 1, 2, 0, 15, 0x7622},
    {"addb_acc", 1, 2, 8, 15, 0x09},
    {"sb", 1, 2, 12, 15, 0x6},          //cond
    {"movl_loc32_p", 1, 2, 8, 15, 0xa9},        //mov acc ,shift = <1-15>
    {"bar", 1, 2, 7, 15, 0x11f},
    {"movl_xar_imm22", 1, 2, 8, 15, 0x8d},      //XAR0-3
    {"movl_xar_imm22", 1, 2, 8, 15, 0x8f},      //XAR4-5
    {"movl_xar_imm22", 1, 2, 6, 15, 0x1da},     //XAR6
    {"movl_xar_imm22", 1, 2, 6, 15, 0x1db},     //XAR7
    {"pread_loc16", 1, 2, 8, 15, 0x24}, //cond
    {"movz_arn", 1, 2, 11, 15, 0xb},    //movz ar0-5 (8,10) 
    {"movz_arn", 1, 2, 8, 15, 0x88},    //movz ar6  
    {"movz_arn", 1, 2, 8, 15, 0x80},    //movz ar7  
    {"addb_or_subb_xar", 1, 2, 11, 15, 0x1b},
    {"banz", 1, 2, 3, 15, 0x1},
    {"movb_ax", 1, 2, 9, 15, 0x4d},     //movb ax #8bit (8, 8) 0:1
    {"movl_loc32_acc", 1, 2, 8, 15, 0x1e},      //movl loc32 acc  
    {"rpt", 1, 2, 8, 15, 0xf6},         //rpt #8bit
    {"rpt", 1, 2, 8, 15, 0xf7},         //rpt loc16 
    {"movl_acc", 1, 2, 8, 15, 0x06},    //movl acc loc32 
    {"add_acc", 1, 2, 8, 15, 0x81},
    {"add_acc", 1, 2, 0, 15, 0x5604},
    {"add_acc", 1, 2, 8, 15, 0x05},
    {"neg_ax", 1, 2, 1, 15, 0x7fae},    //(0,0) 1: AH, 0: AL
    {"edis", 1, 2, 0, 15, 0x761a},
    {"lcr", 1, 2, 3, 15, 0x7cc},        //lcr *xarn
    {"lcr", 1, 2, 6, 15, 0x1d9},        //lcr #22bit 
    {"cmpl_acc_loc32", 1, 2, 8, 15, 0xf},
    {"sbf", 1, 2, 8, 15, 0xec},         //sbf EQ 
    {"sbf", 1, 2, 8, 15, 0xed},         //sbf NEQ 
    {"sbf", 1, 2, 8, 15, 0xee},         //sbf TC 
    {"sbf", 1, 2, 8, 15, 0xef},         //sbf NTC 
    {"movl_xar_loc32", 1, 2, 8, 15, 0x8e},      //XAR0
    {"movl_xar_loc32", 1, 2, 8, 15, 0x8b},      //XAR1
    {"movl_xar_loc32", 1, 2, 8, 15, 0x86},      //XAR2
    {"movl_xar_loc32", 1, 2, 8, 15, 0x82},      //XAR3
    {"movl_xar_loc32", 1, 2, 8, 15, 0x8a},      //XAR4
    {"movl_xar_loc32", 1, 2, 8, 15, 0x83},      //XAR5
    {"movl_xar_loc32", 1, 2, 8, 15, 0xc4},      //XAR6
    {"movl_xar_loc32", 1, 2, 8, 15, 0xc5},      //XAR7
    {"mov_ax_loc16", 1, 2, 9, 15, 0x49},        //mov ax loc16 
    {"addb_or_subb_sp", 1, 2, 8, 15, 0xfe},
    {"sub_acc_loc16_shift", 1, 2, 8, 15, 0xae}, //shift = 0
    {"sub_acc_loc16_shift", 2, 2, 0, 15, 0x5600, 28, 31, 0x0},  //shift = 1-15
    {"sub_acc_loc16_shift", 1, 2, 8, 15, 0x04}, //shift = 16
    {"dmov_loc16", 1, 2, 8, 15, 0xa5},
    {"movad_t_loc16", 1, 2, 8, 15, 0xa7},
    {"movdl_xt_loc32", 1, 2, 8, 15, 0xa6},
    {"xmacd", 1, 2, 8, 15, 0xa4},
    {"xpread_loc16_pma", 1, 2, 8, 15, 0xac},
    {"movl_loc32_xar", 1, 2, 8, 15, 0x3a},      //XAR0
    {"movl_loc32_xar", 1, 2, 8, 15, 0xb2},      //XAR1
    {"movl_loc32_xar", 1, 2, 12, 15, 0xa},      //XAR2-5
    {"movl_loc32_xar", 1, 2, 8, 15, 0xc2},      //XAR6
    {"movl_loc32_xar", 1, 2, 8, 15, 0xc3},      //XAR7
    {"lretr", 1, 2, 0, 15, 0x6},        //lretr
    {"cmp_loc16_16sg", 1, 2, 8, 15, 0x1b},
    {"movb_acc", 1, 2, 8, 15, 0x02},    //movb acc #8bit 
    {"subb_acc", 1, 2, 8, 15, 0x19},    //subb acc #8bit 
    {"mov_loc16_imm", 1, 2, 8, 15, 0x2b},       //mov loc16 #0 
    {"addl_acc_loc32", 1, 2, 8, 15, 0x07},
    {"sub_ax_loc16", 1, 2, 9, 15, 0x4f},        //sub ax (8, 8)=0:AL or 1:AH
    {"cmpb_ax_imm8", 1, 2, 9, 15, 0x29},
    {"movb_loc16_cond", 1, 2, 4, 15, 0x56b},    //mov loc16, #8bit, cond = NEQ
    {"mov_loc16_ax", 1, 2, 9, 15, 0x4b},
    {"add_ax_loc16", 1, 2, 9, 15, 0x4a},
    {"mov_t_loc16", 1, 2, 8, 15, 0x2d},
    {"mpy_acc_t_loc16", 1, 2, 8, 15, 0x12},
    {"movb_xar", 1, 2, 11, 15, 0x1a},   //mov xar0-5 , #8bit
    {"movb_xar", 1, 2, 8, 15, 0xbe},    //mov xar6 , #8bit
    {"movb_xar", 1, 2, 8, 15, 0xb6},    //mov xar7 , #8bit
    {"mov_p", 1, 2, 8, 15, 0x27},
    {"mov_p", 1, 2, 8, 15, 0x2f},
    {"lsr_ax_shift", 1, 2, 5, 15, 0x7fe},
    {"addu_acc_loc16", 1, 2, 8, 15, 0x0d},
    {"and_ax_loc16_imm16", 1, 2, 9, 15, 0x66},
    {"andb_ax_imm8", 1, 2, 9, 15, 0x48},
    {"addb_ax_imm8", 1, 2, 9, 15, 0x4e},
    {"cmp_ax_loc16", 1, 2, 9, 15, 0x2a},
    {"mov_acc", 1, 2, 0, 15, 0x5603},   //mov acc ,shift = <1-15>
    {"mov_acc", 1, 2, 8, 15, 0x85},     //mov acc ,shift = <0>
    {"mov_acc", 1, 2, 8, 15, 0x25},     //mov acc ,shift = <16>
    {"mov_acc", 1, 2, 12, 15, 0xe},     //mov acc ,shift = <1-15>
    {"addl_lc_acc", 1, 2, 0, 15, 0x5601},
    {"mov_ier_loc16", 1, 2, 8, 15, 0x23},       //mov acc ,shift = <16>
    {"nop", 1, 2, 8, 15, 0x77},         //mov acc ,shift = <16>
    {"add_lc_16sg", 1, 2, 8, 15, 0x08},
    {"mov_16bit_loc16", 1, 2, 8, 15, 0xf4},
    {"sfr_acc_16bit", 1, 2, 4, 15, 0xff4},
    {"dec", 1, 2, 8, 15, 0xb},
    {"movl_xt_loc32", 1, 2, 8, 15, 0x87},
    {"mov_loc16_arn", 1, 2, 11, 15, 0xf},       //mov loc16 #16bit 
    {"b", 1, 2, 4, 15, 0xffe},
    {"movz_dp", 1, 2, 10, 15, 0x2e},    //movz dp 10bit 
    {"mov_acc_16bit_shift", 1, 2, 4, 15, 0xff2},        //mov acc ,#16bit, shift = <16>
    {"test_acc", 1, 2, 0, 15, 0xff58},
    {"lsl_acc", 1, 2, 4, 15, 0xff3},
    {"push_st0", 1, 2, 0, 15, 0x7618},
    {"pop_st0", 1, 2, 0, 15, 0x7613},
    {"abs_acc", 1, 2, 0, 15, 0xff56},
    {"xor_ax_loc16", 1, 2, 9, 15, 0x38},
    {"and_loc16_16sg", 1, 2, 8, 15, 0x18},
    {"sat_acc", 1, 2, 0, 15, 0xff57},
    {"movl_p_acc", 1, 2, 0, 15, 0xff5a},        //mov acc ,shift = <1-15>
    {"asr_ax_shift", 1, 2, 5, 15, 0x7fd},
    {"subbl_acc_loc32", 1, 2, 0, 15, 0x5654},
    {"subl_loc32_acc", 1, 2, 0, 15, 0x5641},
    {"subl_acc_loc32", 1, 2, 8, 15, 0x3},
    {"ffc_xar7_22bit", 1, 2, 6, 15, 0x3},
    {"subcu_acc_loc16", 1, 2, 8, 15, 0x1f},
    {"tbit_loc16_16bit", 1, 2, 12, 15, 0x4},
    {"lb_xar", 1, 2, 0, 15, 0x7620},
    {"inc", 1, 2, 8, 15, 0xa},
    {"lsl_ax_16bit", 1, 2, 5, 15, 0x7fc},
    {"or_ax_loc16", 1, 2, 9, 15, 0x65},
    {"orb_ax_8bit", 1, 2, 9, 15, 0x28},
    {"or_loc16_16bit", 1, 2, 8, 15, 0x1a},
    {"bf", 1, 2, 4, 15, 0x56c},
    {"and_ifr", 1, 2, 0, 15, 0x762f},
    {"and_loc16_ax", 1, 2, 9, 15, 0x60},
    {"movu_acc_loc16", 1, 2, 8, 15, 0xe},
    {"movu_loc16_ovc", 1, 2, 0, 15, 0x5628, 24, 31, 0},
    {"lsll_acc_t", 1, 2, 0, 15, 0x563b},
    {"lsrl_acc_t", 1, 2, 0, 15, 0x5622},
    {"impyl_acc_xt_loc32", 1, 2, 0, 15, 0x5644},
    {"or_loc16_ax", 1, 2, 9, 15, 0x4c},
    {"sub_acc_16bit", 1, 2, 4, 15, 0xff0},
    {"neg_acc", 1, 2, 0, 15, 0xff54},   //(0,0) 1: AH, 0: AL
    {"mpyb_acc_t_imm8", 1, 2, 8, 15, 0x35},
    {"mpy_p_t_loc16", 1, 2, 8, 15, 0x33},
    {"mpyu_acc_t_loc16", 1, 2, 8, 15, 0x36},
    {"asrl_acc_t", 1, 2, 0, 15, 0x5610},
    {"norm_acc_xar", 1, 2, 3, 15, 0x1fef},
    {"norm_acc_xar", 1, 2, 3, 15, 0x1fee},
    {"mpyu_p_t_loc16", 1, 2, 8, 15, 0x37},
    {"addcu_acc_loc16", 1, 2, 8, 15, 0x0c},
    {"xor_loc16_16bit", 1, 2, 8, 15, 0x1c},
    {"subu_acc_loc16", 1, 2, 8, 15, 0x01},
    {"out_pa_loc16", 1, 2, 8, 15, 0xbc},
    {"uout_pa_loc16", 1, 2, 8, 15, 0xb0},
    {"mov_loc16_acc_shift", 1, 2, 0, 15, 0x562d},
    {"mov_loc16_acc_shift", 1, 2, 8, 15, 0xb1},
    {"mov_loc16_acc_shift", 1, 2, 11, 15, 0x17},
    {"mov_loc16_p", 1, 2, 8, 15, 0x3f},
    {"mov_loc16_t", 1, 2, 8, 15, 0x21},
    {"sub_loc16_ax", 1, 2, 9, 15, 0x3a},
    {"sfr_acc_t", 1, 2, 0, 15, 0xff51},
    {"not_ax", 1, 2, 1, 15, 0x7faf},
    {"add_loc16_ax", 1, 2, 9, 15, 0x39},
    {"xor_loc16_ax", 1, 2, 9, 15, 0x79},
    {"abstc", 1, 2, 0, 15, 0x565f},
    {"subcul_acc_loc32", 2, 2, 0, 15, 0x5617, 24, 31, 0},
    {"negtc_acc", 1, 2, 0, 15, 0x5632},
    {"mpyb_p_t_imm8", 1, 2, 8, 15, 0x31},
    {"mpy_p_loc16_imm16", 1, 2, 8, 15, 0x8c},
    {"and_ier", 1, 2, 0, 15, 0x7626},
    {"push_arh1_arh0", 1, 2, 0, 15, 0x5},
    {"pop_arh1_arh0", 1, 2, 0, 15, 0x3},
    {"nasp", 1, 2, 0, 15, 0x7617},
    {"iret", 1, 2, 0, 15, 0x7602},
    {"estop0", 1, 2, 0, 15, 0x7625},
    {"estop1", 1, 2, 0, 15, 0x7624},
    {"or_ier", 1, 2, 0, 15, 0x7623},
    {"or_ifr", 1, 2, 0, 15, 0x7627},
    {"movb_ax_LSB_loc16", 1, 2, 9, 15, 0x63},   //movb ax #8bit (8, 8) 0:1
    {"lsr64_acc_p_t_shift", 1, 2, 4, 15, 0x569},
    {"lsr64_acc_p_t", 1, 2, 0, 15, 0x565b},
    {"neg64_acc_p", 1, 2, 0, 15, 0x5658},
    {"cmp64_acc_p", 1, 2, 0, 15, 0x565e},
    {"mov_ovc_loc16", 2, 2, 0, 15, 0x5602, 24, 31, 0x0},
    {"sat64_acc_p", 1, 2, 0, 15, 0x563e},
    {"zapa", 1, 2, 0, 15, 0x5633},
    {"addul_p_loc32", 2, 2, 0, 15, 0x5657, 24, 31, 0x0},
    {"addul_acc_loc32", 2, 2, 0, 15, 0x5653, 24, 31, 0x0},
    {"movb_loc16_ax_LSB", 1, 2, 9, 15, 0x1e},
    {"movb_ax_MSB_loc16", 1, 2, 9, 15, 0x1c},
    {"movb_loc16_ax_MSB", 1, 2, 9, 15, 0x64},
    {"mpyxu_acc_t_loc16", 1, 2, 8, 15, 0x30},
    {"mpyxu_p_t_loc16", 1, 2, 8, 15, 0x32},
    {"lb_22bit", 1, 2, 6, 15, 0x1},
    {"and_acc_loc16", 1, 2, 8, 15, 0x89},
    /*untest instructions */
    {"add_acc_bit16_shift", 1, 2, 4, 15, 0xff1},
    {"not_acc", 1, 2, 0, 15, 0xff55},
    {"movx_tl_loc16", 1, 2, 0, 15, 0x5621},
    {"asr64_acc_p_shift", 1, 2, 4, 15, 0x568},
    {"asr64_acc_p_t", 1, 2, 0, 15, 0x562c},
    {"impyl_p_xt_loc32", 2, 2, 0, 15, 0x5605, 24, 31, 0x0},
    {"qmpyul_p_xt_loc32", 2, 2, 0, 15, 0x5647, 24, 31, 0x0},
    {"impyxul_p_xt_loc32", 2, 2, 0, 15, 0x5665, 24, 31, 0x0},
    {"subul_acc_loc32", 2, 2, 0, 15, 0x5655, 24, 31, 0x0},
    {"subul_p_loc32", 2, 2, 0, 15, 0x565d, 24, 31, 0x0},
    {"csb_acc", 1, 2, 0, 15, 0x5635},
    {"qmpyl_acc_xt_loc32", 2, 2, 0, 15, 0x5663, 28, 31, 0x0},
    {"lsl64_acc_p_shift", 1, 2, 4, 15, 0x56a},
    {"lsl64_acc_p_t", 1, 2, 0, 15, 0x5652},
    {"addcl_acc_loc32", 1, 2, 0, 15, 0x5640},
    {"minl_acc_loc32", 2, 2, 0, 15, 0x5650, 24, 31, 0x0},
    {"maxl_acc_loc32", 2, 2, 0, 15, 0x5661, 24, 31, 0x0},
    {"mincul_p_loc32", 1, 2, 0, 15, 0x5659},
    {"maxcul_p_loc32", 2, 2, 0, 15, 0x5651, 24, 31, 0x0},
    {"movh_loc16_acc_shift_1", 1, 2, 8, 15, 0xb3},
    {"movh_loc16_acc_shift_2", 2, 2, 0, 15, 0x562f, 27, 31, 0x0},
    {"movh_loc16_p", 1, 2, 8, 15, 0x57},
    {"idle", 1, 2, 0, 15, 0x7621},

    {"lc_22bit", 1, 2, 6, 15, 0x2},     //lcr *xarn
    {"lc_xar", 1, 2, 0, 15, 0x7604},    //lcr #22bit 
    {"and_ax_loc16", 1, 2, 9, 15, 0x67},
    {"asr_ax_t", 1, 2, 1, 15, 0x7fb2},
    {"max_ax_loc16", 2, 2, 1, 15, 0x2b39, 24, 31, 0},
    {"min_ax_loc16", 2, 2, 1, 15, 0x2b3a, 24, 31, 0},
    {"mov_loc16_ax_cond", 2, 2, 1, 15, 0x2b15, 28, 31, 0},
    {"pop_arn_arn", 1, 2, 0, 15, 0x7607},
    {"pop_arn_arn", 1, 2, 0, 15, 0x7605},
    {"pop_arn_arn", 1, 2, 0, 15, 0x7606},
    {"push_arn_arn", 1, 2, 0, 15, 0x760d},
    {"push_arn_arn", 1, 2, 0, 15, 0x760f},
    {"push_arn_arn", 1, 2, 0, 15, 0x760c},
    {"push_acc", 1, 2, 0, 15, 0x1ebd},
    {"push_dp", 1, 2, 0, 15, 0x760b},
    {"push_loc16", 1, 2, 8, 15, 0x22},
    {"push_p", 1, 2, 0, 15, 0x761d},
    {"pop_acc", 1, 2, 0, 15, 0x6be},
    {"pop_dp", 1, 2, 0, 15, 0x7603},
    {"pop_loc16", 1, 2, 8, 15, 0x2a},
    {"pop_p", 1, 2, 0, 15, 0x7611},
    {"pop_dbgier", 1, 2, 0, 15, 0x7612},
    {"add_acc_loc16_t", 1, 2, 0, 15, 0x5623},
    {"addl_acc_p_pm", 1, 2, 0, 15, 0x10ac},
    {"adrk_8bit", 1, 2, 8, 15, 0xfc},
    {"and_acc_bit16_shift", 1, 2, 4, 15, 0x3e0},
    {"and_acc_bit16_shift", 1, 2, 0, 15, 0x5608},
    {"c27map", 1, 2, 0, 15, 0x563f},
    {"c27obj", 1, 2, 0, 15, 0x5636},
    {"clrc_xf", 1, 2, 0, 15, 0x561b},
    {"cmpl_acc_p_pm", 1, 2, 0, 15, 0xff59},
    {"lpaddr", 1, 2, 0, 15, 0x561e},    //cmpr cond 
    {"sqra_loc16", 1, 2, 0, 15, 0x5615},
    {"sqrs_loc16", 1, 2, 0, 15, 0x5611},
    {"xb_al", 1, 2, 0, 15, 0x5614},
    {"zalr_acc_loc16", 1, 2, 0, 15, 0x5613},
    {"cmpr", 1, 2, 4, 15, 0x561},       //cmpr cond 
    {"flip_ax", 1, 2, 4, 15, 0x567},
    {"imacl_p_loc32_xar7", 1, 2, 0, 15, 0x564d},
    {"impyal_p_xt_loc32", 1, 2, 0, 15, 0x564c},
    {"impysl_p_xt_loc32", 1, 2, 0, 15, 0x5643},
    {"loopnz_loc16_imm16", 1, 2, 8, 15, 0x2e},
    {"loopz_loc16_imm16", 1, 2, 8, 15, 0x2c},
    {"lret", 1, 2, 0, 15, 0x7614},
    {"lrete", 1, 2, 0, 15, 0x7610},
    {"lsl64_acc_p_shift", 1, 2, 4, 15, 0x569},
    {"mac_p_loc16_xar7", 1, 2, 0, 15, 0x5607},
    {"mov_dp_imm10", 1, 2, 10, 15, 0x3e},
    {"mov_loc16_ier", 1, 2, 8, 15, 0x20},
    {"mov_loc16_ovc", 1, 2, 0, 15, 0x5629},
    {"mov_tl_0", 1, 2, 0, 15, 0x5656},
    {"mov_xarn_pc", 1, 2, 3, 15, 0x7cb},
    {"mova_t_loc16", 1, 2, 8, 15, 0x10},
    {"movl_acc_p_pm", 1, 2, 0, 15, 0x16ac},
    {"movl_loc32_acc_cond", 1, 2, 0, 15, 0x5648},
    {"movp_t_loc16", 1, 2, 8, 15, 0x16},
    {"movu_ovc_loc16", 1, 2, 0, 15, 0x5662},
    {"mpy_acc_loc16_imm16", 1, 2, 8, 15, 0x34},
    {"mpya_p_loc16_imm16", 1, 2, 8, 15, 0x15},
    {"mpya_p_t_loc16", 1, 2, 8, 15, 0x17},
    {"mpys_p_t_loc16", 1, 2, 8, 15, 0x13},
    {"or_acc_imm16_shift", 1, 2, 4, 15, 0x3e1},
    {"or_acc_imm16_shift", 1, 2, 0, 15, 0x564a},
    {"pop_dp_st1", 1, 2, 0, 15, 0x7601},
    {"pop_ifr", 1, 2, 0, 15, 0x2},
    {"pop_rpc", 1, 2, 0, 15, 0x7},
    {"pop_st1", 1, 2, 0, 15, 0x7600},
    {"pop_t_st0", 1, 2, 0, 15, 0x7615},
    {"push_dbgier", 1, 2, 0, 15, 0x760e},
    {"push_dp_st1", 1, 2, 0, 15, 0x7609},
    {"push_ifr", 1, 2, 0, 15, 0x760a},
    {"push_rpc", 1, 2, 0, 15, 0x4},
    {"push_st1", 1, 2, 0, 15, 0x7608},
    {"push_t_st0", 1, 2, 0, 15, 0x7619},
    {"pwrite_loc16", 1, 2, 8, 15, 0x26},
    {"qmacl_p_loc32_xar7", 1, 2, 0, 15, 0x564f},
    {"qmpyal_p_xt_loc32", 1, 2, 0, 15, 0x5646},
    {"qmpyl_p_xt_loc32", 1, 2, 0, 15, 0x5667},
    {"qmpysl_p_xt_loc32", 1, 2, 0, 15, 0x5645},
    {"qmpyxul_p_xt_loc32", 1, 2, 0, 15, 0x5642},
    {"rol_acc", 1, 2, 0, 15, 0xff53},
    {"ror_acc", 1, 2, 0, 15, 0xff52},
    {"sbbu_acc_loc16", 1, 2, 8, 15, 0x1d},
    {"setc_xf", 1, 2, 0, 15, 0x5626},
    {"sub_acc_loc16_t", 1, 2, 0, 15, 0x5627},
    {"subl_acc_p_pm", 1, 2, 0, 15, 0x11ac},
    {"subrl_loc32_acc", 1, 2, 0, 15, 0x5649},
    {"tbit_loc16_t", 1, 2, 0, 15, 0x5625},
    {"tclr_loc16_bit16", 1, 2, 0, 15, 0x5609},
    {"tset_loc16_bit16", 1, 2, 0, 15, 0x560d},
    {"xcall_al", 1, 2, 0, 15, 0x5634},
    {"xor_acc_loc16", 1, 2, 8, 15, 0xb7},
    {"xor_acc_imm16_0_16", 1, 2, 4, 15, 0x3e2},
    {"xor_acc_imm16_0_16", 1, 2, 0, 15, 0x564e},
    {"mov_loc16_0_bi16", 1, 2, 8, 15, 0xf5},
    {"xorb_ax_imm8", 1, 2, 12, 15, 0xf},
    {"xpread_loc16_ax", 1, 2, 0, 15, 0x563c},
    {"xpwrite_ax_loc16", 1, 2, 0, 15, 0x563d},
    {"xretc_cond", 1, 2, 4, 15, 0x56f},
    {"dmac", 1, 2, 0, 15, 0x564b},
    {"mac_p_loc16_0_pma", 1, 2, 8, 15, 0x14},
    {"mov_pm_ax", 1, 2, 4, 15, 0x563},
    {"norm_acc_ind", 1, 2, 0, 15, 0x5624},
    {"xb_pma_arpn", 1, 2, 4, 15, 0x3e7},
    {"xb_pma_cond", 1, 2, 4, 15, 0x56d},
    {"xbanz_pma", 1, 2, 0, 15, 0x560c},
    {"xcall_pma_arpn", 1, 2, 4, 15, 0x3e6},
    {"xcall_pma_cond", 1, 2, 4, 15, 0x56e},
    {"xmac_p_loc16_pma", 1, 2, 8, 15, 0x84},
    {"in_loc16_pa", 1, 2, 8, 15, 0xb4},
    {"intr", 1, 2, 4, 15, 0x1},
    {"intr", 1, 2, 0, 15, 0x7616},
    {"intr", 1, 2, 0, 15, 0x761c},
    {"trap", 1, 2, 5, 15, 0x1},
    {"aborti", 1, 2, 0, 15, 0x1},
    {"iack", 1, 2, 0, 15, 0x763f},

};

insn_action_t insn_action[] = {
#if 0
    exec_aborti,
    exec_abstc,

    exec_add_sh,
    exec_add_lc,

    exec_addl_acc_pm,
    exec_addul_acc_lc,

    exec_adrk,

    exec_and_acc,
    exec_and_acc,
    exec_and_lc_ax,
    exec_asr1_acc_t,

    exec_bar,
    exec_bar,

    exec_c27map,
    exec_c27obj,
    exec_c28addr,

    exec_clrc_m0m1map,
    exec_clrc_objmode,
    exec_clrc_ovc,
    exec_clrc_xf,

    exec_cmp64_acc_p,
    exec_cmpl_acc_p_pm,

    exec_cmpr,

    exec_dint,

    exec_dmac,
    exec_dmov,

    exec_dint,

    exec_ffc,
    exec_flip,
    exec_iack,
    exec_idle,

    exec_imacl,

    exec_impyal,
    exec_impyl_p_xt,
    exec_impysl,
    exec_impyxul,

    exec_in,

    exec_intr_1,
    exec_intr_2,

    exec_subb_xar,
#endif
    /* Float instruction */
    exec_mov32_RaH_loc32_cond,
    exec_mov32_RaH_RbH_cond,
    exec_addf32_RaH_RbH_RcH,
    exec_mov32_loc32_RaH,
    exec_subf32_RaH_RbH_RcH,
    exec_subf32_RaH_imm16_RbH,
    exec_mpyf32_RaH_RbH_RcH,
    exec_zero_RaH,
    exec_f32toi16_RaH_RbH,
    exec_f32toui16_RaH_RbH,
    exec_f32toi32_RaH_RbH,
    exec_f32toui32_RaH_RbH,
    exec_i16tof32_RaH_loc16,
    exec_moviz_RaH_imm16,
    exec_movxi_RaH_imm16,
    exec_mpyf32_RaH_imm16_RbH,
    exec_absf32_RaH_RbH,
    exec_cmpf32_RaH_zero,
    exec_movst0_flag,
    exec_addf32_RaH_imm16_RbH,
    exec_i32tof32_RaH_RbH,
    exec_ui32tof32_RaH_RbH,
    exec_i32tof32_RaH_loc32,
    exec_mpyf32_RaH_RbH_RcH_subf32_RdH_ReH_RfH,
    exec_mpyf32_RaH_RbH_RcH_addf32_RdH_ReH_RfH,
    exec_negf32_RaH_RbH_cond,
    exec_addf32_RdH_ReH_RfH_mov32_RaH_loc32,
    exec_addf32_RdH_ReH_RfH_mov32_loc32_RaH,
    exec_subf32_RdH_ReH_RfH_mov32_RaH_loc32,
    exec_subf32_RdH_ReH_RfH_mov32_loc32_RaH,
    exec_mov32_acc_or_xar_RaH,
    exec_mov32_RaH_acc,
    exec_mpyf32_RdH_ReH_RfH_mov32_RaH_loc32,
    exec_mpyf32_RdH_ReH_RfH_mov32_loc32_RaH,
    exec_eisqrtf32_RaH_RbH,
    exec_rptb_lable_loc16,
    exec_fracf32_RaH_RbH,
    exec_einvf32_RaH_RbH,
    exec_cmpf32_RaH_RbH,
    exec_minf32_RaH_RbH_mov32_RcH_RdH,
    exec_testtf_cond,
    exec_setflg,
    exec_push_rb,
    exec_pop_rb,
    exec_mov32_mem32_stf,
    exec_mov32_stf_mem32,
    exec_cmpf32_RaH_imm16,

    /* Interger instruction */
    exec_subr_loc16_ax,
    exec_mov_loc16,
    exec_movl_p_loc32,
    exec_lsl_ax_t,
    exec_lsr_ax_t,
    exec_spm_pm,
    exec_c28obj,
    exec_setc,
    exec_clrc_amode,
    exec_c28map,
    exec_clrc_mode,
    exec_clrc_ovc,
    exec_movw_dp,
    exec_asp,
    exec_eallow,
    exec_addb_acc,
    exec_sb,
    exec_movl_loc32_p,
    exec_bar,
    exec_movl_xar_imm22,
    exec_movl_xar_imm22,
    exec_movl_xar_imm22,
    exec_movl_xar_imm22,
    exec_pread_loc16,
    exec_movz_arn,
    exec_movz_arn,
    exec_movz_arn,
    exec_addb_or_subb_xar,
    exec_banz,
    exec_movb_ax,
    exec_movl_loc32_acc,
    exec_rpt,
    exec_rpt,
    exec_movl_acc,
    exec_add_acc,
    exec_add_acc,
    exec_add_acc,
    exec_neg_ax,
    exec_edis,
    exec_lcr,
    exec_lcr,
    exec_cmpl_acc_loc32,
    exec_sbf,
    exec_sbf,
    exec_sbf,
    exec_sbf,
    exec_movl_xar_loc32,
    exec_movl_xar_loc32,
    exec_movl_xar_loc32,
    exec_movl_xar_loc32,
    exec_movl_xar_loc32,
    exec_movl_xar_loc32,
    exec_movl_xar_loc32,
    exec_movl_xar_loc32,
    exec_mov_ax_loc16,
    exec_addb_or_subb_sp,
    exec_sub_acc_loc16_shift,
    exec_sub_acc_loc16_shift,
    exec_sub_acc_loc16_shift,
    exec_dmov_loc16,
    exec_movad_t_loc16,
    exec_movdl_xt_loc32,
    exec_xmacd,
    exec_xpread_loc16_pma,
    exec_movl_loc32_xar,
    exec_movl_loc32_xar,
    exec_movl_loc32_xar,
    exec_movl_loc32_xar,
    exec_movl_loc32_xar,
    exec_lretr,
    exec_cmp_loc16_16sg,
    exec_movb_acc,
    exec_subb_acc,
    exec_mov_loc16_imm,
    exec_addl_acc_loc32,
    exec_sub_ax_loc16,
    exec_cmpb_ax_imm8,
    exec_movb_loc16_cond,
    exec_mov_loc16_ax,
    exec_add_ax_loc16,
    exec_mov_t_loc16,
    exec_mpy_acc_t_loc16,
    exec_movb_xar,
    exec_movb_xar,
    exec_movb_xar,
    exec_mov_p,
    exec_mov_p,
    exec_lsr_ax_shift,
    exec_addu_acc_loc16,
    exec_and_ax_loc16_imm16,
    exec_andb_ax_imm8,
    exec_addb_ax_imm8,
    exec_cmp_ax_loc16,
    exec_mov_acc,
    exec_mov_acc,
    exec_mov_acc,
    exec_mov_acc,
    exec_addl_lc_acc,
    exec_mov_ier_loc16,
    exec_nop,
    exec_add_lc_16sg,
    exec_mov_16bit_loc16,
    exec_sfr_acc_16bit,
    exec_dec,
    exec_movl_xt_loc32,
    exec_mov_loc16_arn,
    exec_b,
    exec_movz_dp,
    exec_mov_acc_16bit_shift,
    exec_test_acc,
    exec_lsl_acc,
    exec_push_st0,
    exec_pop_st0,
    exec_abs_acc,
    exec_xor_ax_loc16,
    exec_and_loc16_16sg,
    exec_sat_acc,
    exec_movl_p_acc,
    exec_asr_ax_shift,
    exec_subbl_acc_loc32,
    exec_subl_loc32_acc,
    exec_subl_acc_loc32,
    exec_ffc_xar7_22bit,
    exec_subcu_acc_loc16,
    exec_tbit_loc16_16bit,
    exec_lb_xar,
    exec_inc,
    exec_lsl_ax_16bit,
    exec_or_ax_loc16,
    exec_orb_ax_8bit,
    exec_or_loc16_16bit,
    exec_bf,
    exec_and_ifr,
    exec_and_loc16_ax,
    exec_movu_acc_loc16,
    exec_movu_loc16_ovc,
    exec_lsll_acc_t,
    exec_lsrl_acc_t,
    exec_impyl_acc_xt_loc32,
    exec_or_loc16_ax,
    exec_sub_acc_16bit,
    exec_neg_acc,
    exec_mpyb_acc_t_imm8,
    exec_mpy_p_t_loc16,
    exec_mpyu_acc_t_loc16,
    exec_asrl_acc_t,
    exec_norm_acc_xar,
    exec_norm_acc_xar,
    exec_mpyu_p_t_loc16,
    exec_addcu_acc_loc16,
    exec_xor_loc16_16bit,
    exec_subu_acc_loc16,
    exec_out_pa_loc16,
    exec_out_pa_loc16,
    exec_mov_loc16_acc_shift,
    exec_mov_loc16_acc_shift,
    exec_mov_loc16_acc_shift,
    exec_mov_loc16_p,
    exec_mov_loc16_t,
    exec_sub_loc16_ax,
    exec_sfr_acc_t,
    exec_not_ax,
    exec_add_loc16_ax,
    exec_xor_loc16_ax,
    exec_abstc_acc,
    exec_subcul_acc_loc32,
    exec_negtc_acc,
    exec_mpyb_p_t_imm8,
    exec_mpy_p_loc16_imm16,
    exec_and_ier,
    exec_push_arh1_arh0,
    exec_pop_arh1_arh0,
    exec_nasp,
    exec_iret,
    exec_estop0,
    exec_estop1,
    exec_or_ier,
    exec_or_ifr,
    exec_movb_ax_LSB_loc16,
    exec_lsr64_acc_p_t_shift,
    exec_lsr64_acc_p_t,
    exec_neg64_acc_p,
    exec_cmp64_acc_p,
    exec_mov_ovc_loc16,
    exec_sat64_acc_p,
    exec_zapa,
    exec_addul_p_loc32,
    exec_addul_acc_loc32,
    exec_movb_loc16_ax_LSB,
    exec_movb_ax_MSB_loc16,
    exec_movb_loc16_ax_MSB,
    exec_mpyxu_acc_t_loc16,
    exec_mpyxu_p_t_loc16,
    exec_lb_22bit,
    exec_and_acc_loc16,
    /*untested instructions */
    exec_add_acc_bit16_shift,
    exec_not_acc,
    exec_movx_tl_loc16,
    exec_asr64_acc_p_shift,
    exec_asr64_acc_p_t,
    exec_impyl_p_xt_loc32,
    exec_qmpyul_p_xt_loc32,
    exec_impyxul_p_xt_loc32,
    exec_subul_acc_loc32,
    exec_subul_p_loc32,
    exec_csb_acc,
    exec_qmpyl_acc_xt_loc32,
    exec_lsl64_acc_p_shift,
    exec_lsl64_acc_p_t,
    exec_addcl_acc_loc32,
    exec_minl_acc_loc32,
    exec_maxl_acc_loc32,
    exec_mincul_p_loc32,
    exec_maxcul_p_loc32,
    exec_movh_loc16_acc_shift_1,
    exec_movh_loc16_acc_shift_2,
    exec_movh_loc16_p,
    exec_idle,

    /*untest instructions */
    exec_lc_22bit,
    exec_lc_xar,
    exec_and_ax_loc16,
    exec_asr_ax_t,
    exec_max_ax_loc16,
    exec_min_ax_loc16,
    exec_mov_loc16_ax_cond,
    exec_pop_arn_arn,
    exec_pop_arn_arn,
    exec_pop_arn_arn,
    exec_push_arn_arn,
    exec_push_arn_arn,
    exec_push_arn_arn,
    exec_push_acc,
    exec_push_dp,
    exec_push_loc16,
    exec_push_p,
    exec_pop_acc,
    exec_pop_dp,
    exec_pop_loc16,
    exec_pop_p,
    exec_pop_dbgier,
    exec_add_acc_loc16_t,
    exec_addl_acc_p_pm,
    exec_adrk_8bit,
    exec_and_acc_16bit_shift,
    exec_and_acc_16bit_shift,
    exec_c27map,
    exec_c27obj,
    exec_clrc_xf,
    exec_cmpl_acc_p_pm,
    exec_lpaddr,
    exec_sqra_loc16,
    exec_sqrs_loc16,
    exec_xb_al,
    exec_zalr_cond,
    exec_cmpr,
    exec_flip_ax,
    exec_imacl_p_loc32_xar7,
    exec_impyal_p_xt_loc32,
    exec_impysl_p_xt_loc32,
    exec_loopnz_loc16_imm16,
    exec_loopz_loc16_imm16,
    exec_lret,
    exec_lrete,
    exec_lsr64_acc_p_shift,
    exec_mac_p_loc16_xar7,
    exec_mov_dp_imm10,
    exec_mov_loc16_ier,
    exec_mov_loc16_ovc,
    exec_mov_tl_0,
    exec_mov_xarn_pc,
    exec_mova_t_loc16,
    exec_movl_acc_p_pm,
    exec_movl_loc32_acc_cond,
    exec_movp_t_loc16,
    exec_movu_ovc_loc16,
    exec_mpy_acc_loc16_imm16,
    exec_mpya_p_loc16_imm16,
    exec_mpya_p_t_loc16,
    exec_mpys_p_t_loc16,
    exec_or_acc_imm16_shift,
    exec_or_acc_imm16_shift,
    exec_pop_dp_st1,
    exec_pop_ifr,
    exec_pop_rpc,
    exec_pop_st1,
    exec_pop_t_st0,
    exec_push_dbgier,
    exec_push_dp_st1,
    exec_push_ifr,
    exec_push_rpc,
    exec_push_st1,
    exec_push_t_st0,
    exec_pwrite_loc16,
    exec_qmacl_p_loc32_xar7,
    exec_qmpyal_p_xt_loc32,
    exec_qmpyl_p_xt_loc32,
    exec_qmpysl_p_xt_loc32,
    exec_qmpyxul_p_xt_loc32,
    exec_rol_acc,
    exec_ror_acc,
    exec_sbbu_acc_loc16,
    exec_setc_xf,
    exec_sub_acc_loc16_t,
    exec_subl_p_pm,
    exec_subrl_loc32_acc,
    exec_tbit_loc16_t,
    exec_tclr_loc16_bit16,
    exec_tset_loc16_bit16,
    exec_xcall_al,
    exec_xor_acc_loc16,
    exec_xor_acc_imm16_0_16,
    exec_xor_acc_imm16_0_16,
    exec_mov_loc16_0_bi16,
    exec_xorb_ax_imm8,
    exec_xpread_loc16_ax,
    exec_xpwrite_ax_loc16,
    exec_xretc_cond,
    exec_dmac,
    exec_mac_p_loc16_0_pma,
    exec_mov_pm_ax,
    exec_norm_acc_ind,
    exec_xb_pma_arpn,
    exec_xb_pma_cond,
    exec_xbanz_pma,
    exec_xcall_pma_arpn,
    exec_xcall_pma_cond,
    exec_xmac_p_loc16_pma,
    exec_in_loc16_pa,
    exec_intr,
    exec_intr,
    exec_intr,
    exec_trap,
    exec_aborti,
    exec_iack,

    NULL,
};

uint32_t exec_insn_cache(c28_core_t * core, insn_cache_t * cache)
{
    uint32_t insn = cache->insn;

#if 0
    printf("PC :0x%x, acc :%x, xt :%x, p :%x, st0 :%x, st1 :%x, sp :%x\n", core->pc, core->acc, core->xt, core->p,
           core->st0, core->st1, core->sp);
    printf("xar0 :0x%x, xar1 :%x, xar2 :%x, xar3 :%x\n", core->xar[0].xar, core->xar[1].xar, core->xar[2].xar,
           core->xar[3].xar);
    printf("xar4 :%x, xar5 :%x, xar6 :%x, xar7 :%x\n", core->xar[4].xar, core->xar[5].xar, core->xar[6].xar,
           core->xar[7].xar);
    printf("R0 :%x, R1 :%x, R2 :%x, R3 :%x\n", core->fp_regs[0].Ra, core->fp_regs[1].Ra, core->fp_regs[2].Ra,
           core->fp_regs[3].Ra);
    printf("#####R4 :%x, R5 :%x, R6 :%x, R7 :%x, stf :%x\n", core->fp_regs[4].Ra, core->fp_regs[5].Ra,
           core->fp_regs[6].Ra, core->fp_regs[7].Ra, core->stf);
#endif
    cache->action(core, insn, &(cache->fields));
    core->insn_num++;
    return 0;
}

insn_action_t get_insn_action(c28_core_t * core, uint32_t insn)
{
    int index;
    insn_action_t ret = NULL;

    if ((ret =
         decode_instr(insn, &index, insn32_decode_table,
                      sizeof (insn32_decode_table) / sizeof (ISEITEM))) == DECODE_SUCCESS)
    {
        DBG("In %s, instr=0x%x, index=0x%x\n", __FUNCTION__, insn, index);
        return insn_action[index];
    } else
    {
        //skyeye_log(Error_log, "decode error instruction for 0x%x at 0x%x\n",insn, core->pc);
        printf("Can't implement the instruction 0x%x at 0x%x\n", insn, core->pc);
    }
    return ret;
}

int trans_opcode_fields(uint32_t insn, opcode_fields_t * fields)
{
    fields->op0_15 = BITS(0, 15);
    fields->op1_15 = BITS(1, 15);
    fields->op3_15 = BITS(3, 15);
    fields->op4_15 = BITS(4, 15);
    fields->op5_15 = BITS(5, 15);
    fields->op6_15 = BITS(6, 15);
    fields->op8_15 = BITS(8, 15);
    fields->op9_15 = BITS(9, 15);
    fields->op10_15 = BITS(10, 15);
    fields->op11_15 = BITS(11, 15);
    fields->op12_15 = BITS(12, 15);

    /* address modes */
    fields->mode0_2 = BITS(0, 2);
    fields->mode3_5 = BITS(3, 5);
    fields->mode6_7 = BITS(6, 7);
    fields->mode0_3 = BITS(0, 3);
    fields->mode4_7 = BITS(4, 7);
    fields->mode8_10 = BITS(8, 10);
    fields->bit6 = BITS(0, 5);

    /* 16bit immediate value */
    fields->imm16 = BITS(16, 31);

    /* conditions */
    fields->cond8_11 = BITS(8, 11);
    fields->cond0_7 = BITS(0, 7);

    /* 8bit immediate offset value */
    fields->offset0_7 = BITS(0, 7);
    fields->offset24_31 = BITS(24, 31);

    /* 7bit immediate offset value */
    fields->offset0_6 = BITS(0, 6);

    /* 10bit immediate offset value */
    fields->offset0_9 = BITS(0, 9);

    fields->shift0_3 = BITS(0, 3);
    fields->shift8_11 = BITS(8, 11);
    fields->shift24_27 = BITS(24, 27);

    /* bit8 : AL or AH */
    fields->a8 = BITS(8, 8);
    /* bit0 : AL or AH */
    fields->a0 = BITS(0, 0);
    /* bit4 : AL or AH */
    fields->a4 = BITS(4, 4);
    /* bit7 : ADD or SUB */
    fields->b = BITS(7, 7);

    return 0;
}

int decode_instr(uint32_t insn, int32_t * idx, ISEITEM * table, int table_len)
{
    int n = 0;
    int base = 0;
    int ret = DECODE_FAILURE;
    int i = 0;

    //int instr_slots = sizeof(insn_decode_table)/sizeof(ISEITEM);
    int instr_slots = table_len;

    for (i = 0; i < instr_slots; i++)
    {
        ISEITEM *slot = &table[i];

        n = slot->attribute_value;

        /* if not content, we ignore */
        if (n == 0)
            continue;
        base = 0;
        while (n)
        {
            if (BITS(slot->content[base], slot->content[base + 1]) != slot->content[base + 2])
            {
                break;
            }
            base += 3;
            n--;
        }
        //All conditions is satisfied.
        if (n == 0)
            ret = DECODE_SUCCESS;

        if (ret == DECODE_SUCCESS)
        {
            *idx = i;
            return ret;
        }
    }
    return ret;
}
