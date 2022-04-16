/*  thumbemu.c -- Thumb instruction emulation.
 * Copyright (C) 1996, Cygnus Software Technologies Ltd.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

/* We can provide simple Thumb simulation by decoding the Thumb
 * instruction into its corresponding ARM instruction, and using the
 * existing ARM simulator.  */

#ifndef MODET                           /* required for the Thumb instruction support */
#if 1
#error "MODET needs to be defined for the Thumb world to work"
#else
#define MODET (1)
#endif
#endif

#include "armdefs.h"
#include "armemu.h"
#include "armos.h"
#include "skyeye_pref.h"
#include <skyeye_log.h>
#include <math.h>

#define ILLEGAL_DECODE	printf("In %s.Line:%d.Instruction:0x%08x Decode Error!Current PC = 0x%08x\n", __func__, __LINE__, instr, state->Reg[15]-4);
#define SignExtend(Data, n)	((Data & (1 << (n - 1))) ? (Data | (0xFFFFFFFF << n)) : Data)

bool_t getCondResult(ARMul_State * state, ARMword cond)
{
    bool_t doit = False;

    switch (cond)
    {
        case EQ:
            doit = ZFLAG;
            break;
        case NE:
            doit = !ZFLAG;
            break;
        case CS:
            doit = CFLAG;
            break;
        case VS:
            doit = VFLAG;
            break;
        case VC:
            doit = !VFLAG;
            break;
        case MI:
            doit = NFLAG;
            break;
        case PL:
            doit = !NFLAG;
            break;
        case CC:
            doit = !CFLAG;
            break;
        case HI:
            doit = (CFLAG && !ZFLAG);
            break;
        case LS:
            doit = (!CFLAG || ZFLAG);
            break;
        case GE:
            doit = ((!NFLAG && !VFLAG) || (NFLAG && VFLAG));
            break;
        case LT:
            doit = ((NFLAG && !VFLAG) || (!NFLAG && VFLAG));
            break;
        case GT:
            doit = ((!NFLAG && !VFLAG && !ZFLAG) || (NFLAG && VFLAG && !ZFLAG));
            break;
        case LE:
            doit = ((NFLAG && !VFLAG) || (!NFLAG && VFLAG)) || ZFLAG;
            break;
        default:
            printf("In %s.Line:%d Not implement cond = %x\n", __func__, __LINE__, cond);
            break;
    }

    return doit;
}

/* If-Then makes up to four following instructions (the IT block) conditional */
static void check_it_block_insn_exec(ARMul_State * state, ARMword instr)
{
    ARMword cond = state->condexec_cond;
    ARMword mask = state->condexec_mask;
    bool_t doit;
    bool_t mask3 = ((mask >> 3) & 0x1) == (cond & 0x1);
    bool_t mask2 = ((mask >> 2) & 0x1) == (cond & 0x1);
    bool_t mask1 = ((mask >> 1) & 0x1) == (cond & 0x1);

    doit = getCondResult(state, cond);

    switch (mask)
    {
        case 0x8:
            //Only condition First insn
            if (doit)
            {
                state->it_block.isExec[0] = True;
            }
            state->it_block.insnNum = 1;
            break;
        case 0x4:
        case 0xc:                      //T or E
            if (mask3 && doit)
            {
                state->it_block.isExec[0] = True;
                state->it_block.isExec[1] = True;
            } else if (!mask3 && !doit)
            {
                state->it_block.isExec[0] = False;
                state->it_block.isExec[1] = True;
            }

            state->it_block.insnNum = 2;
            break;
        case 0x2:
        case 0x6:
        case 0xe:
            if (mask2 && mask3 && doit) //T T
            {
                state->it_block.isExec[0] = True;
                state->it_block.isExec[1] = True;
                state->it_block.isExec[2] = True;
            } else if (!mask3 && mask2) //E T
            {
                if (doit)
                {
                    state->it_block.isExec[0] = True;
                    state->it_block.isExec[2] = True;
                } else
                {
                    state->it_block.isExec[1] = True;
                }
            } else if (mask3 && !mask2) //T E
            {
                if (doit)
                {
                    state->it_block.isExec[0] = True;
                    state->it_block.isExec[1] = True;
                } else
                {
                    state->it_block.isExec[2] = True;
                }
            } else if (!mask3 && !mask2)        //E E
            {
                if (!doit)
                {
                    state->it_block.isExec[0] = True;
                    state->it_block.isExec[1] = True;
                    state->it_block.isExec[2] = True;
                }
            }
            state->it_block.insnNum = 3;
            break;
        default:
            if (mask3 && mask2 && mask1 && doit)        //T T T
            {
                state->it_block.isExec[0] = True;
                state->it_block.isExec[1] = True;
                state->it_block.isExec[2] = True;
                state->it_block.isExec[3] = True;
            } else if (!mask3 && mask2 && mask1)        //E T T
            {
                if (doit)
                {
                    state->it_block.isExec[0] = True;
                    state->it_block.isExec[2] = True;
                    state->it_block.isExec[3] = True;
                } else
                    state->it_block.isExec[1] = True;
            } else if (mask3 && !mask2 && mask1)        //T E T
            {
                if (doit)
                {
                    state->it_block.isExec[0] = True;
                    state->it_block.isExec[1] = True;
                    state->it_block.isExec[3] = True;
                } else
                    state->it_block.isExec[2] = True;
            } else if (!mask3 && !mask2 && mask1)       //E E T
            {
                if (doit)
                {
                    state->it_block.isExec[0] = True;
                    state->it_block.isExec[3] = True;
                } else
                {
                    state->it_block.isExec[1] = True;
                    state->it_block.isExec[2] = True;
                }
            } else if (mask3 && mask2 && !mask1)        //T T E
            {
                if (doit)
                {
                    state->it_block.isExec[0] = True;
                    state->it_block.isExec[1] = True;
                    state->it_block.isExec[2] = True;
                } else
                {
                    state->it_block.isExec[3] = True;
                }
            } else if (!mask3 && mask2 && !mask1)       //E T E
            {
                if (doit)
                {
                    state->it_block.isExec[0] = True;
                    state->it_block.isExec[2] = True;
                } else
                {
                    state->it_block.isExec[3] = True;
                    state->it_block.isExec[1] = True;
                }
            } else if (mask3 && !mask2 && !mask1)       //T E E
            {
                if (doit)
                {
                    state->it_block.isExec[0] = True;
                    state->it_block.isExec[1] = True;
                } else
                {
                    state->it_block.isExec[2] = True;
                    state->it_block.isExec[3] = True;
                }
            } else if (!mask3 && !mask2 && !mask1)      //E E E
            {
                if (doit)
                {
                    state->it_block.isExec[0] = True;
                } else
                {
                    state->it_block.isExec[1] = True;
                    state->it_block.isExec[2] = True;
                    state->it_block.isExec[3] = True;
                }
            } else
            {
                ILLEGAL_DECODE;
                break;
            }
            state->it_block.insnNum = 4;
            break;
    }
}

static void WritePCBranch(ARMul_State * state, ARMword src)
{
    if (src & 1)
    {
        /* Thumb bit.  */
        SETT;
        state->Reg[15] = src & 0xfffffffe;
    } else
    {
        CLEART;
        state->Reg[15] = src;
    }
    state->Cpsr = ARMul_GetCPSR(state);
    FLUSHPIPE;
}

static inline int32_t sextract32(uint32_t value, int start, int length)
{
    /* Note that this implementation relies on right shift of signed
     * integers being an arithmetic shift.
     */
    assert(start >= 0 && length > 0 && length <= 32 - start);

    return ((int32_t) (value << (32 - length - start))) >> (32 - length);
}

static inline uint32_t extract32(uint32_t value, int start, int length)
{
    /* Note that this implementation relies on right shift of signed
     * integers being an arithmetic shift.
     */
    assert(start >= 0 && length > 0 && length <= 32 - start);

    return ((uint32_t) (value << (32 - length - start))) >> (32 - length);
}

static inline ARMword sar_shift(uint32_t data, int shift)
{
    ARMword result;

    ((data >> 31) == 1 ? (result = (0xffffffff >> shift << shift) | (data >> shift)) : (result = data >> shift));

    return result;
}

/* Set N and Z flags from result.  */
static inline void gen_logic_cc(ARMul_State * state, ARMword result)
{
    ASSIGNZ(result == 0);
    ASSIGNN(NEG(result));
}

static void shifter_out_imm(ARMul_State * state, ARMword var, int shift)
{
    if (shift == 0)
    {
        state->CFlag = var & 1;
    } else
    {
        state->CFlag = (var >> shift);
        if (shift != 31)
        {
            state->CFlag &= 1;
        }
    }
}
static inline ARMword gen_rbit(ARMword tmp)
{

}

static inline ARMword gen_bswap32(ARMword tmp)
{

}

static inline ARMword gen_rev16(ARMword tmp)
{

}

static inline ARMword gen_revsh(ARMword tmp)
{

}

static inline ARMword gen_clzi(ARMword tmp, int shift)
{
    int i;
    for (i = 31; i >= 0; i--)
    {
        if ((tmp >> i) & 0x1)
            break;
    }

    return (shift - i - 1);
}

static inline ARMword gen_crc32c(ARMword tmp, ARMword tmp2, ARMword tmp3)
{

}

static inline ARMword gen_crc32(ARMword tmp, ARMword tmp2, ARMword tmp3)
{

}

static inline ARMword gen_swap_half(ARMword tmp)
{
    return (tmp >> 16) | (tmp << 16);
}

static inline void gen_smul_dual(ARMword * s1, ARMword * s2)
{
    int32_t tmp1, tmp2, result;

    tmp1 = (*s1 << 16) >> 16;
    tmp2 = (*s2 << 16) >> 16;

    result = tmp1 * tmp2;
    *s1 = sar_shift(*s1, 16);
    *s2 = sar_shift(*s2, 16);

    *s2 = (*s1) * (*s2);
    *s1 = result;
}

static inline uint64_t gen_mulu(ARMword s1, ARMword s2)
{
    uint64_t tmp1, tmp2, result;

    tmp1 = (uint64_t) s1;
    tmp2 = (uint64_t) s2;

    result = tmp1 * tmp2;

    return result;
}

static inline int64_t gen_muls(ARMword s1, ARMword s2)
{
    int64_t tmp1, tmp2, result;

    tmp1 = (int64_t) s1;
    tmp2 = (int64_t) s2;

    result = tmp1 * tmp2;

    return result;
}

/* load and add a 64-bit value from a register pair.  */
static uint64_t gen_addq(ARMul_State * state, uint64_t val, int rlow, int rhigh)
{
    uint64_t result;
    uint32_t tmpl;
    uint32_t tmph;

    /* Load 64-bit value rd:rn.  */
    tmpl = state->Reg[rlow];
    tmph = state->Reg[rhigh];

    result = tmpl | ((uint64_t)tmph << 32);

    result += val;

    return result;
}

/* load a 32-bit value from a register and perform a 64-bit accumulate.  */
static uint64_t gen_addq_lo(ARMul_State * state, uint64_t val, int rlow)
{
    uint64_t result;
    uint32_t tmp;

    /* Load value and extend to 64 bits.  */
    tmp = state->Reg[rlow];
    result = (uint64_t) tmp;

    result += val;

    return result;
}

static inline ARMword gen_usad8(ARMword tmp, ARMword tmp2)
{
    ARMword absdiff1, absdiff2, absdiff3, absdiff4, result;

    absdiff1 = abs((tmp & 0xff) - (tmp2 & 0xff));
    absdiff2 = abs((tmp >> 8 & 0xff) - (tmp2 >> 8 & 0xff));
    absdiff3 = abs((tmp >> 16 & 0xff) - (tmp2 >> 16 & 0xff));
    absdiff4 = abs((tmp >> 24 & 0xff) - (tmp2 >> 24 & 0xff));

    result = absdiff1 + absdiff2 + absdiff3 + absdiff4;

    return result;
}

static ARMword gen_rotl(ARMword var, ARMword shift)
{
    ARMword result, tmp, tmp2;

    if (shift == 0)
        result = var;
    else
    {
        tmp = var << shift;
        tmp2 = var >> (32 - shift);
        result = tmp | tmp2;
    }

    return result;
}

static ARMword gen_rotr(ARMword var, ARMword shift)
{
    ARMword result, tmp, tmp2;

    if (shift == 0)
        result = var;
    else
    {
        tmp = var >> shift;
        tmp2 = var << (32 - shift);
        result = tmp | tmp2;
    }

    return result;
}

static ARMword gen_add16(ARMword t0, ARMword t1)
{
    ARMword tmp;

    tmp = t0 ^ t1;
    tmp &= 0x8000;
    t0 &= ~0x8000;
    t1 &= ~0x8000;

    t0 += t1;
    t0 ^= tmp;

    return t0;
}

/* Shift by immediate.  Includes special handling for shift == 0.  */
static inline ARMword gen_shift_imm(ARMul_State * state, ARMword var, int shiftop, int shift, int flags)
{
    ARMword result, tmp;

    switch (shiftop)
    {
        case 0:                        /* LSL */
            if (shift != 0)
            {
                if (flags)
                    shifter_out_imm(state, var, 32 - shift);
                result = var << shift;
            } else
            {
                result = var;
            }
            break;
        case 1:                        /* LSR */
            if (shift == 0)
            {
                if (flags)
                {
                    state->CFlag = var >> 31;
                }
                result = 0;
            } else
            {
                if (flags)
                    shifter_out_imm(state, var, shift - 1);

                result = var >> shift;
            }
            break;
        case 2:                        /* ASR */
            if (shift == 0)
                shift = 32;
            if (flags)
                shifter_out_imm(state, var, shift - 1);
            if (shift == 32)
                shift = 31;
            result = sar_shift(var, shift);
            break;
        case 3:                        /* ROR/RRX */
            if (shift != 0)
            {
                if (flags)
                    shifter_out_imm(state, var, shift - 1);

                if (shift == 0)
                    result = var;
                else
                    result = gen_rotl(var, 32 - shift);
            } else
            {
                tmp = state->CFlag << 31;
                if (flags)
                    shifter_out_imm(state, var, 0);
                var = var >> 1;
                result = var | tmp;
            }
            break;
        default:
            break;
    }

    return result;
}

static inline ARMword gen_shift_reg(ARMul_State * state, ARMword var, int shiftop, int shift, int flags)
{
    ARMword result = 0;

    if (flags)
    {
        printf("In %s.Line:%d.Not implement!Current PC = 0x%08x\n", __func__, __LINE__, state->Reg[15]);
    } else
    {
        switch (shiftop)
        {
            case 0:
                result = var << shift;
                break;
            case 1:
                result = var >> shift;
                break;
            case 2:
                result = sar_shift(var, shift);
                break;
            case 3:
                shift &= 0x1f;
                result = gen_rotr(var, shift);
                break;
        }
    }

    return result;
}

static void gen_load_exclusive(ARMul_State * state, int rt, int rt2, ARMword addr, int size)
{
    ARMword tmp;

    switch (size)
    {
        case 0:
            tmp = ARMul_LoadByte(state, addr) & 0xFF;
            break;
        case 1:
            tmp = ARMul_LoadHalfWord(state, addr) & 0xFFFF;
            break;
        case 2:
            tmp = ARMul_LoadWordS(state, addr);
            break;
        default:
            printf("In %s.Line:%d.size = %d is Error!Current PC = 0x%08x\n", __func__, __LINE__, size, state->Reg[15]);
            return;
    }

    state->Reg[rt] = tmp;
}

static void gen_store_exclusive(ARMul_State * state, int rd, int rt, int rt2, ARMword addr, int size)
{
    ARMword tmp;

    tmp = state->Reg[rt];

    switch (size)
    {
        case 0:
            ARMul_StoreByte(state, addr, tmp & 0xFF);
            break;
        case 1:
            ARMul_StoreHalfWord(state, addr, tmp & 0xFFFF);
            break;
        case 2:
            ARMul_StoreWordS(state, addr, tmp);
            break;
        default:
            printf("In %s.Line:%d.size = %d is Error!Current PC = 0x%08x\n", __func__, __LINE__, size, state->Reg[15]);
            return;
    }

    state->Reg[rd] = 0;
}

static inline ARMword gen_sxth(ARMword data)
{
    return (int32_t) (data << 16) >> 16;
}

static inline ARMword gen_sxtb(ARMword data)
{
    return (int32_t) (data << 24) >> 24;
}

static inline ARMword gen_uxth(ARMword data)
{
    return (data & 0xFFFF);
}

static inline ARMword gen_uxtb(ARMword data)
{
    return (data & 0xFF);
}

static ARMword sub_cc(ARMul_State * state, ARMword t0, ARMword t1, ARMword cc)
{
    uint32_t result;

    if (cc)
    {
        result = t0 - t1 - !CFLAG;
    } else
        result = t0 - t1;

    if ((t0 >= t1) || ((t0 | t1) >> 31))
    {
        ARMul_SubCarry(state, t0, t1, result);
        ARMul_SubOverflow(state, t0, t1, result);
    } else
    {
        CLEARC;
        CLEARV;
    }

    ARMul_NegZero(state, result);

    return result;
}

static ARMword add_cc(ARMul_State * state, ARMword t0, ARMword t1, ARMword cc)
{
    uint32_t result;

    if (cc)
        result = t0 + t1 + state->CFlag;
    else
        result = t0 + t1;

    ASSIGNZ(result == 0);

    if ((t0 | t1) >> 30)
    {
        ASSIGNN(NEG(result));
        ARMul_AddOverflow(state, t0, t1, result);
        ARMul_AddCarry(state, t0, t1, result);
    } else
    {
        CLEARN;
        CLEARC;
        CLEARV;
    }

    return result;
}

static ARMword gen_deposit(ARMword arg1, ARMword arg2, ARMword ofs, ARMword len)
{
    ARMword mask;
    ARMword t1 = 0;
    ARMword ret;

    if (len == 32)
    {
        ret = arg2;
        return ret;
    }

    mask = (1u << len) - 1;

    if (ofs + len < 32)
    {
        t1 = arg2 & mask;
        t1 <<= ofs;
    } else
    {
        t1 = arg2 << ofs;
    }

    ret = arg1 & (~(mask << ofs));
    ret = ret | t1;

    return ret;
}

/* Generate code for a Thumb-2 data processing operation.  If CONDS is nonzero
 * then set condition code flags based on the result of the operation.
 * If SHIFTER_OUT is nonzero then set the carry flag for logical operations
 * to the high bit of T1.
 * Returns zero if the opcode is valid.  */

static int thumb2_data_op_proc(ARMul_State * state, int op, int conds, ARMword shifter_out, ARMword * ret, ARMword t1)
{
    int logic_cc;
    ARMword t0 = *ret;

    logic_cc = 0;
    switch (op)
    {
        case 0:                        /* and */
            t0 = t0 & t1;
            logic_cc = conds;
            break;
        case 1:                        /* bic */
            t1 = ~t1;
            t0 = t0 & t1;
            logic_cc = conds;
            break;
        case 2:                        /* orr */
            t0 = t0 | t1;
            logic_cc = conds;
            break;
        case 3:                        /* orn */
            t1 = ~t1;
            t0 = t0 | t1;
            logic_cc = conds;
            break;
        case 4:                        /* eor */
            t0 = t0 ^ t1;
            logic_cc = conds;
            break;
        case 8:                        /* add */
            if (conds)
            {
                //add carry
                t0 = add_cc(state, t0, t1, 0);
            } else
                t0 += t1;
            break;
        case 10:                       /* adc */
            if (conds)
                t0 = add_cc(state, t0, t1, 1);
            else
                t0 += t1 + state->CFlag;
            break;
        case 11:                       /* sbc */
            if (conds)
                t0 = sub_cc(state, t0, t1, 1);
            else
                t0 -= t1 + state->CFlag;
            break;
        case 13:                       /* sub */
            if (conds)
                t0 = sub_cc(state, t0, t1, 0);
            else
                t0 -= t1;
            break;
        case 14:                       /* rsb */
            if (conds)
                t0 = sub_cc(state, t1, t0, 0);
            else
                t0 = t1 - t0;
            break;
        default:                       /* 5, 6, 7, 9, 12, 15. */
            return 1;
    }
    if (logic_cc)
    {
        gen_logic_cc(state, t0);
        if (shifter_out)
            state->CFlag = t1 >> 31;
    }

    *ret = t0;
    return 0;
}

static tdstate thumb_push_pop(ARMul_State * state, ARMword instr)
{
    tdstate valid = t_decoded;
    uint32_t addr, offset, tmp;
    int i;

    /* push/pop */
    addr = state->Reg[13];
    if (instr & (1 << 8))
        offset = 4;
    else
        offset = 0;

    for (i = 0; i < 8; i++)
    {
        if (instr & (1 << i))
            offset += 4;
    }

    if ((instr & (1 << 11)) == 0)
    {
        addr = addr - offset;
    }

    for (i = 0; i < 8; i++)
    {
        if (instr & (1 << i))
        {
            if (instr & (1 << 11))
            {
                /* pop */
                tmp = ARMul_LoadWordS(state, addr);
                state->Reg[i] = tmp;
                //printf("POP reg_id = %d address = %x, Reg = %x\n", i, addr, tmp);
            } else
            {
                /* push */
                tmp = state->Reg[i];
                ARMul_StoreWordS(state, addr, tmp);
                //printf("PUSH reg_id = %d address = %x, Reg = %x\n", i, addr, tmp);
            }
            /* advance to the next address.  */
            addr += 4;
        }
    }

    if (instr & (1 << 8))
    {
        if (instr & (1 << 11))
        {
            /* pop pc */
            tmp = 0;
            tmp = ARMul_LoadWordS(state, addr);
            //printf("POP reg_id = %d address = %x, Reg = %x\n", 15, addr, tmp);
            /* don't set the pc until the rest of the instruction
             * has completed */
        } else
        {
            /* push lr */
            tmp = state->Reg[14];
            ARMul_StoreWordS(state, addr, tmp);
            //printf("PUSH reg_id = %d address = %x, Reg = %x\n", 14, addr, tmp);
        }
        addr += 4;
    }

    if ((instr & (1 << 11)) == 0)
    {
        addr -= offset;
    }
    /* write back the new stack pointer */
    state->Reg[13] = addr;
    /* set the new PC value */
    if ((instr & 0x0900) == 0x0900)
    {
        //state->Reg[15] = tmp & 0x3FFFFFFE;
        WritePCBranch(state, tmp);
        valid = t_branch;
    }

    return valid;
}

static bool_t thumb_czb(ARMul_State * state, ARMword instr, ARMword pc)
{
    uint32_t rm, tmp, op, offset, val;

    rm = instr & 7;
    op = (instr >> 11) & 1;
    tmp = state->Reg[rm];

    if ((tmp == 0) != op)
    {
        offset = ((instr & 0xf8) >> 2) | (instr & 0x200) >> 3;
        val = pc + 4;
        val += offset;

        state->Reg[15] = val;
        return True;
    }

    return False;
}

/* Signed saturation.  */
static inline uint32_t do_ssat(ARMul_State *state, int32_t val, int shift)
{
    int32_t top;
    uint32_t mask;

    top = val >> shift;
    mask = (1u << shift) - 1;
    if (top > 0)
    {
        /*CPSR Q bit set to 1(Cumulative saturation bit)*/
        state->Cpsr |= (1 << 27);
        return mask;
    } else if (top < -1)
    {
        /*CPSR Q bit set to 1(Cumulative saturation bit)*/
        state->Cpsr |= (1 << 27);
        return ~mask;
    }
    return val;
}

/* Unsigned saturation.  */
static inline uint32_t do_usat(ARMul_State *state, int32_t val, int shift)
{
    uint32_t max;

    max = (1u << shift) - 1;
    if (val < 0)
    {
        /*CPSR Q bit set to 1(Cumulative saturation bit)*/
        state->Cpsr |= (1 << 27);
        return 0;
    } else if (val > max)
    {
        /*CPSR Q bit set to 1(Cumulative saturation bit)*/
        state->Cpsr |= (1 << 27);
        return max;
    }
    return val;
}

static inline uint32_t do_ssat16(ARMul_State *state, int32_t x, int shift)
{
    uint32_t res;

    res = (uint16_t)do_ssat(state, (int16_t)x, shift);
    res |= do_ssat(state, ((int32_t)x) >> 16, shift) << 16;
    return res;
}

static inline uint32_t do_usat16(ARMul_State *state, int32_t x, int shift)
{
    uint32_t res;

    res = (uint16_t)do_usat(state, (int16_t)x, shift);
    res |= do_usat(state, ((int32_t)x) >> 16, shift) << 16;
    return res;
}

static ARMword do_sat_instr(ARMul_State *state, ARMword instr)
{
    int op = (instr >> 21) & 7;
    int Rn = (instr >> 16) & 0xf;
    int Rd = (instr >> 8) & 0xf;
    int sat_imm = instr & 0x1f;

    int shift = ((instr >> 6) & 3) | ((instr >> 10) & 0x1c); //imm2 | imm3<<2
    uint32_t tmp, tmp2;

    if (shift)
    {
        if (op & 1)
        {
            tmp = state->Reg[Rn] >> shift;
        } else
        {
            tmp = state->Reg[Rn] << shift;
        }
    }

    tmp2 = sat_imm;
    if (op & 4)
    {
        /* Unsigned.  */
        if ((op & 1) && shift == 0)
        {
            tmp = do_usat16(state, tmp, tmp2);
        } else
        {
            tmp = do_usat(state, tmp, tmp2);
        }
    } else
    {
        /* Signed.  */
        if ((op & 1) && shift == 0)
        {
            tmp = do_ssat16(state, tmp, tmp2);
        } else
        {
            tmp = do_ssat(state, tmp, tmp2);
        }
    }

    return tmp;
}

static ARMword get_dest_data(ARMul_State * state, ARMword instr, ARMword rd, ARMword rn)
{
    ARMword op, imm, shift, tmp;
    ARMword dest = 0;

    op = (instr >> 21) & 7;
    imm = instr & 0x1f;
    shift = ((instr >> 6) & 3) | ((instr >> 10) & 0x1c);

    if (rn == 15)
    {
        tmp = 0;
    } else
    {
        tmp = state->Reg[rn];
    }

    switch (op)
    {
        case 2:                        /* Signed bitfield extract.  */
            imm++;
            if (shift + imm > 32)
                ILLEGAL_DECODE;
            if (imm < 32)
            {
                dest = sextract32(tmp, shift, imm);
            }
            break;
        case 6:                        /* Unsigned bitfield extract.  */
            imm++;
            if (shift + imm > 32)
                ILLEGAL_DECODE;
            if (imm < 32)
            {
                dest = extract32(tmp, shift, imm);
            }
            break;
        case 3:                        /* Bitfield insert/clear.  */
            if (imm < shift)
                ILLEGAL_DECODE;
            imm = imm + 1 - shift;
            if (imm != 32)
            {
                ARMword tmp2 = state->Reg[rd];

                dest = gen_deposit(tmp2, tmp, shift, imm);
            }
            break;
        default:                       /* Saturate.  */
            dest = do_sat_instr(state, instr);
            break;
    }

    return dest;

}
static void thumb2_data_process_imm(ARMul_State * state, ARMword instr, ARMword pc)
{
    ARMword op, imm, shift, tmp, offset, dest;
    ARMword rd, rn;

    rn = (instr >> 16) & 0xf;
    rd = (instr >> 8) & 0xf;

    if (instr & (1 << 25))
    {
        /* plain binary immediate */
        if (instr & (1 << 24))
        {
            if (instr & (1 << 20))
                ILLEGAL_DECODE;
            /* Bitfield/Saturate.  */
            dest = get_dest_data(state, instr, rd, rn);

            state->Reg[rd] = dest;
        } else
        {
            imm = ((instr & 0x04000000) >> 15) | ((instr & 0x7000) >> 4) | (instr & 0xff);
            if (instr & (1 << 22))
            {
                /* 16-bit immediate.  */
                imm |= (instr >> 4) & 0xf000;
                if (instr & (1 << 23))
                {
                    /* movt */
                    tmp = state->Reg[rd];
                    tmp = (tmp & 0xFFFF) | (imm << 16);
                } else
                {
                    /* movw */
                    tmp = imm;
                }
            } else
            {
                /* Add/sub 12-bit immediate.  */
                if (rn == 15)
                {
                    offset = pc & ~(uint32_t) 3;
                    if (instr & (1 << 23))
                        offset -= imm;
                    else
                        offset += imm;
                    tmp = offset;
                } else
                {
                    tmp = state->Reg[rn];
                    if (instr & (1 << 23))
                    {
                        tmp = tmp - imm;
                    } else
                    {
                        tmp = tmp + imm;
                    }
                }
            }
            state->Reg[rd] = tmp;
        }
    } else
    {
        int shifter_out = 0;
        int imm3 = BITS(12, 14);

        /* modified 12-bit immediate.  */
        shift = ((instr & 0x04000000) >> 23) | ((instr & 0x7000) >> 12);
        imm = (instr & 0xff);
        switch (shift)
        {
            case 0:                    /* XY */
                /* Nothing to do.  */
                break;
            case 1:                    /* 00XY00XY */
                imm |= imm << 16;
                break;
            case 2:                    /* XY00XY00 */
                imm |= imm << 16;
                imm <<= 8;
                break;
            case 3:                    /* XYXYXYXY */
                imm |= imm << 16;
                imm |= imm << 8;
                break;
            default:                   /* Rotated constant.  */
                shift = (shift << 1) | (imm >> 7);
                imm |= 0x80;
                imm = imm << (32 - shift);
                shifter_out = 1;
                break;
        }
        if (rn == 15)
        {
            tmp = 0;
        } else
        {
            tmp = state->Reg[rn];
        }
        op = (instr >> 21) & 0xf;
        if (thumb2_data_op_proc(state, op, (instr & (1 << 20)) != 0, shifter_out, &tmp, imm))
            ILLEGAL_DECODE;

        if (rd != 15)
        {
            state->Reg[rd] = tmp;
        }
    }
}

static void thumb2_uncond_branch_proc(ARMul_State * state, ARMword instr)
{
    /* signextend(hw1[10:0]) -> offset[:12].  */
    ARMword offset = ((int32_t) instr << 5) >> 9 & ~(int32_t) 0xfff;

    /* hw1[10:0] -> offset[11:1].  */
    offset |= (instr & 0x7ff) << 1;
    /* (~hw2[13, 11] ^ offset[24]) -> offset[23,22]
     * offset[24:22] already have the same value because of the
     * sign extension above.  */
    offset ^= ((~instr) & (1 << 13)) << 10;
    offset ^= ((~instr) & (1 << 11)) << 11;

    if (instr & (1 << 14))
    {
        /* Branch and link.  */
        state->Reg[14] = state->Reg[15] | 1;
    }

    offset += state->Reg[15];
    if (instr & (1 << 12))
    {
        /* b/bl */
        state->Reg[15] = offset;
    } else
    {
        /* blx */
        offset &= ~(uint32_t) 2;
        state->Reg[15] = offset;
    }

    return;
}

static void thumb2_cond_branch_proc(ARMul_State * state, ARMword instr, ARMword pc)
{
    ARMword op, offset;

    op = (instr >> 22) & 0xf;
    bool_t doit = getCondResult(state, op);

    /* offset[11:1] = instr[10:0] */
    offset = (instr & 0x7ff) << 1;
    /* offset[17:12] = instr[21:16].  */
    offset |= (instr & 0x003f0000) >> 4;
    /* offset[31:20] = instr[26].  */
    offset |= ((int32_t) ((instr << 5) & 0x80000000)) >> 11;
    /* offset[18] = instr[13].  */
    offset |= (instr & (1 << 13)) << 5;
    /* offset[19] = instr[11].  */
    offset |= (instr & (1 << 11)) << 8;

    /* jump to the offset */
    if (doit)
        state->Reg[15] += offset;

    return;
}

void thumb2_msr_cpsr(ARMul_State * state, ARMword instr)
{
    ARMword first_instr = instr >> 16;
    ARMword second_instr = instr & 0xffff;
    ARMword reg_n = (first_instr & 0xf);
    ARMword sysm = second_instr & 0xff;

    if ((sysm >> 3) == 0)
    {
        printf("%s The thumb2 MSR instruction is not implemented!\n", __FUNCTION__);
    } else if ((sysm >> 3) == 1)
    {
        if ((sysm & 0x7) == 0)
        {
            state->MSP = state->Reg[reg_n] & 0xfffffffc;
        } else if ((sysm & 0x7) == 1)
        {
            state->PSP = state->Reg[reg_n] & 0xfffffffc;
        }
    } else if ((sysm >> 3) == 2)
    {
        if ((sysm & 0x7) == 0)
        {
            state->PRIMASK = state->Reg[reg_n] & 0x1;
        } else if ((sysm & 0x7) == 4)
        {
            if (((state->Reg[reg_n] & 0x3) >> 1) == 0)
            {
                state->Reg[13] = state->MSP;
            } else if (((state->Reg[reg_n] & 0x3) >> 1) == 1)
            {
                state->Reg[13] = state->PSP;
            }
        }
    }
}

void thumb2_msr_spsr(ARMul_State * state, ARMword instr)
{
    ARMword first_instr = instr >> 16;
    ARMword second_instr = instr & 0xffff;
    ARMword rd = (second_instr >> 8) & 0xf;

    state->Reg[rd] = 0;
    ARMword sysm = first_instr & 0xff;

    if ((sysm >> 3) == 0 || (sysm >> 3) == 1)
    {
        printf("%s Line:%d. The thumb2 MSR instruction is not implemented!\n", __FUNCTION__, __LINE__);
    } else if ((sysm >> 3) == 2)
    {
        if ((sysm & 0x3) == 0)
        {
            state->PRIMASK = state->Reg[rd];
        } else
        {
            printf("%s Line:%d. The thumb2 MSR instruction is not implemented!\n", __FUNCTION__, __LINE__);
        }
    }
}

void thumb2_mrs_spsr(ARMul_State * state, ARMword instr)
{
    ARMword first_instr = instr >> 16;
    ARMword second_instr = instr & 0xffff;
    ARMword rd = (second_instr >> 8) & 0xf;

    state->Reg[rd] = 0;
    ARMword sysm = first_instr & 0xff;

    if ((sysm >> 3) == 0 || (sysm >> 3) == 1)
    {
        printf("%s Line:%d. The thumb2 MRS instruction is not implemented!\n", __FUNCTION__, __LINE__);
    } else if ((sysm >> 3) == 2)
    {
        if ((sysm & 0x3) == 0)
        {
            state->Reg[rd] = state->PRIMASK;
        } else
        {
            printf("%s Line:%d. The thumb2 MRS instruction is not implemented!\n", __FUNCTION__, __LINE__);
        }
    }
}

static tdstate thumb2_misc_control_proc(ARMul_State * state, ARMword instr)
{
    ARMword op, rd, rn;
    ARMword tmp, addr;
    tdstate valid = t_t2_decoded;

    rn = (instr >> 16) & 0xf;
    rd = (instr >> 8) & 0xf;

    if (instr & (1 << 13))
        ILLEGAL_DECODE;

    if (instr & (1 << 26))
    {
        if (!(instr & (1 << 20)))
        {
            /* Hypervisor call (v7) */
            int imm16 = BITS(16, 19) << 12 | BITS(0, 11);

            ILLEGAL_DECODE;
            //gen_hvc(s, imm16);
        } else
        {
            /* Secure monitor call (v6+) */
            ILLEGAL_DECODE;
            //gen_smc(s);
        }
    } else
    {
        op = (instr >> 20) & 7;
        switch (op)
        {
            case 0:                    /* msr cpsr.  */
                thumb2_msr_cpsr(state, instr);
                break;
            case 1:                    /* msr spsr.  */
                thumb2_msr_spsr(state, instr);
                break;
            case 2:                    /* cps, nop-hint.  */
                ILLEGAL_DECODE;
                break;
            case 3:                    /* Special control operations.  */
                op = (instr >> 4) & 0xf;
                switch (op)
                {
                    case 2:            /* clrex */
                    case 4:            /* dsb */
                    case 5:            /* dmb */
                    case 6:            /* isb */
                        /*not implement */
                        break;
                    default:
                        ILLEGAL_DECODE;
                }
                break;
            case 4:                    /* bxj */
                /* Trivial implementation equivalent to bx.
                 * This instruction doesn't exist at all for M-profile.
                 */
                addr = state->Reg[rn];
                WritePCBranch(state, addr);
                valid = t_branch;
                break;
            case 5:                    /* Exception return.  */
                if (rn != 14 || rd != 15)
                {
                    ILLEGAL_DECODE;
                }
                ILLEGAL_DECODE;
                break;
            case 6:                    /* MRS */
                thumb2_mrs_spsr(state, instr);
                break;
            case 7:                    /* MRS */
                ILLEGAL_DECODE;
                break;
            default:
                ILLEGAL_DECODE;
                break;
        }
    }

    return valid;
}

static tdstate thumb2_ldst_single_proc(ARMul_State * state, ARMword instr)
{
    ARMword op, rn, rs, rd, rm;
    ARMword imm, shift, addr, tmp;
    tdstate ret = t_t2_decoded;
    int postinc = 0;
    int writeback = 0;

    op = ((instr >> 21) & 3) | ((instr >> 22) & 4);
    rn = (instr >> 16) & 0xf;
    rs = (instr >> 12) & 0xf;

    if ((instr & 0x01100000) == 0x01000000)
    {
        ILLEGAL_DECODE;
        return;
    }

    if (rs == 15)
    {
        if (!(instr & (1 << 20)))
        {
            ILLEGAL_DECODE;
            return;
        }
        if (op != 2)
        {
            /* Byte or halfword load space with dest == r15 : memory hints.
             * Catch them early so we don't emit pointless addressing code.
             * This space is a mix of:
             *  PLD/PLDW/PLI,  which we implement as NOPs (note that unlike
             *     the ARM encodings, PLDW space doesn't UNDEF for non-v7MP
             *     cores)
             *  unallocated hints, which must be treated as NOPs
             *  UNPREDICTABLE space, which we NOP or UNDEF depending on
             *     which is easiest for the decoding logic
             *  Some space which must UNDEF
             */
            int op1 = (instr >> 23) & 3;
            int op2 = (instr >> 6) & 0x3f;

            if (op & 2)
            {
                ILLEGAL_DECODE;
                return;
            }
            if (rn == 15)
            {
                /* UNPREDICTABLE, unallocated hint or
                 * PLD/PLDW/PLI (literal)
                 */
                ILLEGAL_DECODE;
                return;
            }
            if (op1 & 1)
            {
                ILLEGAL_DECODE;
                return;                 /* PLD/PLDW/PLI or unallocated hint */
            }
            if ((op2 == 0) || ((op2 & 0x3c) == 0x30))
            {
                ILLEGAL_DECODE;
                return;                 /* PLD/PLDW/PLI or unallocated hint */
            }
            /* UNDEF space, or an UNPREDICTABLE */
            ILLEGAL_DECODE;
            return;
        }
    }

    if (rn == 15)
    {
        addr = 0;
        /* PC relative.  */
        /* pc has already been incremented by 4.  */
        imm = state->Reg[15] & 0xfffffffc;
        if (instr & (1 << 23))
            imm += instr & 0xfff;
        else
            imm -= instr & 0xfff;
        addr = imm;
    } else
    {
        addr = state->Reg[rn];
        if (instr & (1 << 23))
        {
            /* Positive offset. imm12 */
            imm = instr & 0xfff;
            addr += imm;
        } else
        {
            /* imm8 */
            imm = instr & 0xff;

            switch ((instr >> 8) & 0xf)
            {
                    /* 1 P U W */
                case 0x0:              /* Shifted Register.  */
                    shift = (instr >> 4) & 0xf;
                    if (shift > 3)
                    {
                        ILLEGAL_DECODE;
                        return;
                    }
                    tmp = state->Reg[rn];
                    if (shift)
                        tmp <<= shift;
                    addr += tmp;
                    break;
                case 0xc:              /* Negative offset.  */
                    addr -= imm;
                    break;
                case 0xe:              /* User privilege.  */
                    addr += imm;
                    break;
                case 0x9:              /* Post-decrement.  */
                    imm = -imm;
                    /* Fall through.  */
                case 0xb:              /* Post-increment.  */
                    postinc = 1;
                    writeback = 1;
                    break;
                case 0xd:              /* Pre-decrement.  */
                    imm = -imm;
                    /* Fall through.  */
                case 0xf:              /* Pre-increment.  */
                    addr += imm;
                    writeback = 1;
                    break;
                default:
                    ILLEGAL_DECODE;
                    return;
            }
        }
    }

    if (instr & (1 << 20))
    {
        /* Load.  */
        tmp = 0;
        switch (op)
        {
            case 0:
                //LDRB
                tmp = ARMul_LoadByte(state, addr) & 0xFF;
                break;
            case 4:
                //LDRSB
                tmp = ARMul_LoadByte(state, addr) & 0xFF;
                tmp = SignExtend(tmp, 8);
                break;
            case 1:
                //LDRH
                tmp = ARMul_LoadHalfWord(state, addr) & 0xFFFF;
                break;
            case 5:
                //LDRSH
                tmp = ARMul_LoadHalfWord(state, addr) & 0xFFFF;
                tmp = SignExtend(tmp, 16);
                break;
            case 2:
                //LDR
                tmp = ARMul_LoadWordN(state, addr);
                break;
            default:
                ILLEGAL_DECODE;
                return;
        }
        if (rs == 15)
        {
            state->Reg[15] = tmp & (~1);
            FLUSHPIPE;
            ret = t_branch;
        } else
        {
            state->Reg[rs] = tmp;
        }
    } else
    {
        /* Store.  */
        tmp = state->Reg[rs];
        switch (op)
        {
            case 0:
                ARMul_StoreByte(state, addr, tmp & 0xFF);
                break;
            case 1:
                ARMul_StoreHalfWord(state, addr, tmp & 0xFFFF);
                break;
            case 2:
                ARMul_StoreWordN(state, addr, tmp);
                break;
            default:
                ILLEGAL_DECODE;
                return;
        }
    }
    if (postinc)
        addr += imm;

    if (writeback)
    {
        state->Reg[rn] = addr;
    }
    return ret;
}

static tdstate thumb2_ldst_dual_porc(ARMul_State * state, ARMword instr, ARMword pc)
{
    ARMword op, rn, rs, rd, rm;
    ARMword addr, offset, tmp;
    tdstate ret = t_t2_decoded;

    rm = (instr >> 0) & 0xf;
    rn = (instr >> 16) & 0xf;
    rs = (instr >> 12) & 0xf;
    rd = (instr >> 8) & 0xf;

    if (instr & 0x1200000)
    {
        /* 0b1110_1000_x11x_xxxx_xxxx_xxxx_xxxx_xxxx
         *  - load/store dual (post-indexed)
         * 0b1111_1001_x10x_xxxx_xxxx_xxxx_xxxx_xxxx
         *  - load/store dual (literal and immediate)
         * 0b1111_1001_x11x_xxxx_xxxx_xxxx_xxxx_xxxx
         *  - load/store dual (pre-indexed)
         */
        bool_t wback = BITS(21, 21);

        if (rn == 15)
        {
            if (instr & (1 << 21))
            {
                ILLEGAL_DECODE;
                return ret;
            }
            addr = pc & (~3);
        } else
        {
            addr = state->Reg[rn];
        }

        offset = (instr & 0xff) * 4;

        if ((instr & (1 << 23)) == 0)
        {
            offset = -offset;
        }

        if (instr & (1 << 24))
        {
            addr += offset;
            offset = 0;
        }

        if (instr & (1 << 20))
        {
            /* ldrd */
            tmp = ARMul_LoadWordS(state, addr);
            state->Reg[rs] = tmp;
            addr += 4;
            tmp = ARMul_LoadWordS(state, addr);
            state->Reg[rd] = tmp;
        } else
        {
            /* strd */
            tmp = state->Reg[rs];
            ARMul_StoreWordS(state, addr, tmp);
            addr += 4;
            tmp = state->Reg[rd];
            ARMul_StoreWordS(state, addr, tmp);
        }

        if (wback)
        {
            addr = addr + offset - 4;
            state->Reg[rn] = addr;
        }
    } else if ((instr & (1 << 23)) == 0)
    {
        /* 0b1110_1000_010x_xxxx_xxxx_xxxx_xxxx_xxxx
         * - load/store exclusive word
         * - TT (v8M only)
         */
        if (rs == 15)
        {
            ILLEGAL_DECODE;
            return ret;
        }

        addr = state->Reg[rn];
        addr += ((instr & 0xff) << 2);

        if (instr & (1 << 20))
        {
            gen_load_exclusive(state, rs, 15, addr, 2);
        } else
        {
            gen_store_exclusive(state, rd, rs, 15, addr, 2);
        }
    } else if ((instr & (7 << 5)) == 0)
    {
        /* Table Branch.  */
        if (instr & (1 << 4))
        {
            /* tbh */
            addr = state->Reg[rn] + (state->Reg[rm] << 1);
            tmp = ARMul_LoadHalfWord(state, addr);
        } else
        {                               /* tbb */
            addr = state->Reg[rn] + state->Reg[rm];
            tmp = ARMul_LoadByte(state, addr);
        }
        tmp <<= 1;
        tmp += state->Reg[15];
        state->Reg[15] = tmp & (~1);
        FLUSHPIPE;
        ret = t_branch;
    } else
    {
        bool_t is_lasr = False;
        bool_t is_ld = BITS(20, 20);
        int op2 = (instr >> 6) & 0x3;

        op = (instr >> 4) & 0x3;
        switch (op2)
        {
            case 0:
                ILLEGAL_DECODE;
                return ret;
            case 1:
                /* Load/store exclusive byte/halfword/doubleword */
                if (op == 2)
                {
                    ILLEGAL_DECODE;
                    return ret;
                }
            case 2:
                /* Load-acquire/store-release */
                if (op == 3)
                {
                    ILLEGAL_DECODE;
                    return ret;
                }
                /* Fall through */
            case 3:
                /* Load-acquire/store-release exclusive */
                is_lasr = True;
                break;
        }

        addr = state->Reg[rn];
        if (!(op2 & 1))
        {
            if (is_ld)
            {
                switch (op)
                {
                    case 0:            /* ldab */
                        tmp = ARMul_LoadByte(state, addr) & 0xFF;
                        break;
                    case 1:            /* ldah */
                        tmp = ARMul_LoadHalfWord(state, addr) & 0xFFFF;
                        break;
                    case 2:            /* lda */
                        tmp = ARMul_LoadWordS(state, addr);
                        break;
                    default:
                        ILLEGAL_DECODE;
                        return ret;
                }
                state->Reg[rs] = tmp;
            } else
            {
                tmp = state->Reg[rs];
                switch (op)
                {
                    case 0:            /* stlb */
                        ARMul_StoreByte(state, addr, tmp & 0xFF);
                        break;
                    case 1:            /* stlh */
                        ARMul_StoreHalfWord(state, addr, tmp & 0xFFFF);
                        break;
                    case 2:            /* stl */
                        ARMul_StoreWordS(state, addr, tmp);
                        break;
                    default:
                        ILLEGAL_DECODE;
                        return ret;
                }
            }
        } else if (is_ld)
        {
            gen_load_exclusive(state, rs, rd, addr, op);
        } else
        {
            gen_store_exclusive(state, rm, rs, rd, addr, op);
        }
    }

    return ret;
}

static tdstate thumb2_ldst_mult_porc(ARMul_State * state, ARMword instr)
{
    ARMword rn, rs, rd, rm;
    ARMword addr, offset, tmp, tmp2;
    tdstate ret = t_t2_decoded;

    rn = (instr >> 16) & 0xf;
    rs = (instr >> 12) & 0xf;
    rd = (instr >> 8) & 0xf;
    rm = instr & 0xf;

    /* Load/store multiple, RFE, SRS.  */
    if (((instr >> 23) & 1) == ((instr >> 24) & 1))
    {
        /* RFE, SRS: not available in user mode or on M profile */
        sky_pref_t *pref = get_skyeye_pref();

        if (pref->user_mode_sim)
        {
            ILLEGAL_DECODE;
            return ret;
        }
        if (instr & (1 << 20))
        {
            /* rfe */
            addr = state->Reg[rn];
            if ((instr & (1 << 24)) == 0)
                addr -= 8;
            /* Load PC into tmp and CPSR into tmp2.  */
            tmp = ARMul_LoadWordS(state, addr);
            addr += 4;
            tmp2 = ARMul_LoadWordS(state, addr);
            if (instr & (1 << 21))
            {
                /* Base writeback.  */
                if (instr & (1 << 24))
                {
                    addr += 4;
                } else
                {
                    addr -= 4;
                }
                state->Reg[rn] = addr;
            }
            state->Reg[15] = tmp;
            state->Cpsr = tmp2;
            FLUSHPIPE;
            ret = t_branch;
        } else
        {
            /* srs */
            ILLEGAL_DECODE;
            return ret;
        }
    } else
    {
        int i, loaded_base = 0;
        uint32_t loaded_var;
        bool_t wback = BITS(21, 21);

        /* Load/store multiple.  */
        addr = state->Reg[rn];
        offset = 0;
        for (i = 0; i < 16; i++)
        {
            if (instr & (1 << i))
                offset += 4;
        }

        if (instr & (1 << 24))
        {
            addr -= offset;
        }

        for (i = 0; i < 16; i++)
        {
            if ((instr & (1 << i)) == 0)
                continue;
            if (instr & (1 << 20))
            {
                /* Load.  */
                tmp = ARMul_LoadWordS(state, addr);
                if (i == 15)
                {
                    WritePCBranch(state, tmp);
                    ret = t_branch;
                } else if (i == rn)
                {
                    loaded_var = tmp;
                    loaded_base = 1;
                } else
                {
                    state->Reg[i] = tmp;
                }
            } else
            {
                /* Store.  */
                tmp = state->Reg[i];
                ARMul_StoreWordS(state, addr, tmp);
            }
            addr += 4;
        }

        if (loaded_base)
        {
            state->Reg[rn] = loaded_var;
        }

        if (wback)
        {
            /* Base register writeback.  */
            if (instr & (1 << 24))
            {
                addr -= offset;
            }
            /* Fault if writeback register is in register list.  */
            if (instr & (1 << rn))
            {
                ILLEGAL_DECODE;
                return ret;
            }
            state->Reg[rn] = addr;
        }
    }

    return ret;
}

static void thumb2_data_process_register(ARMul_State * state, ARMword instr)
{
    ARMword op, rn, rs, rd, rm;
    ARMword tmp, tmp2, shift, shiftop, conds, logic_cc;

    rn = (instr >> 16) & 0xf;
    rs = (instr >> 12) & 0xf;
    rd = (instr >> 8) & 0xf;
    rm = instr & 0xf;

    op = (instr >> 21) & 0xf;

    if (op == 6)
    {
        /* Halfword pack.  */
        tmp = state->Reg[rn];
        tmp2 = state->Reg[rm];
        shift = ((instr >> 10) & 0x1c) | ((instr >> 6) & 0x3);

        if (instr & (1 << 5))
        {
            /* pkhtb */
            if (shift == 0)
                shift = 31;
            tmp2 = (tmp2 >> shift) & 0xffff;
            tmp &= 0xffff0000;
        } else
        {
            /* pkhbt */
            if (shift)
                tmp2 <<= shift;
            tmp &= 0xffff;
            tmp2 &= 0xffff0000;
        }
        tmp |= tmp2;
        state->Reg[rd] = tmp;
    } else
    {
        /* Data processing register constant shift.  */
        if (rn == 15)
        {
            tmp = 0;
        } else
        {
            tmp = state->Reg[rn];
        }
        tmp2 = state->Reg[rm];

        shiftop = (instr >> 4) & 3;
        shift = ((instr >> 6) & 3) | ((instr >> 10) & 0x1c);
        conds = (instr & (1 << 20)) != 0;
        logic_cc = (conds && (op < 8));
        tmp2 = gen_shift_imm(state, tmp2, shiftop, shift, logic_cc);
        if (thumb2_data_op_proc(state, op, conds, 0, &tmp, tmp2))
        {
            ILLEGAL_DECODE;
            return;
        }

        if (rd != 15)
        {
            state->Reg[rd] = tmp;
        }
    }
}

static void thumb2_other_data_process(ARMul_State * state, ARMword instr, ARMword pc)
{
    ARMword op, rn, rd, rm;
    ARMword tmp, tmp2, tmp3;

    rn = (instr >> 16) & 0xf;
    rd = (instr >> 8) & 0xf;
    rm = instr & 0xf;

    op = ((instr >> 17) & 0x38) | ((instr >> 4) & 7);
    if (op < 4)
    {
        /* Saturating add/subtract.  */
        ILLEGAL_DECODE;
        return;
    } else
    {
        switch (op)
        {
            case 0x0a:                 /* rbit */
            case 0x08:                 /* rev */
            case 0x09:                 /* rev16 */
            case 0x0b:                 /* revsh */
            case 0x18:                 /* clz */
                break;
            case 0x10:                 /* sel */
            case 0x20:                 /* crc32/crc32c */
            case 0x21:
            case 0x22:
            case 0x28:
            case 0x29:
            case 0x2a:
            default:
                ILLEGAL_DECODE;
                return;
        }
        tmp = state->Reg[rn];
        switch (op)
        {
            case 0x0a:                 /* rbit */
                tmp = gen_rbit(tmp);
                ILLEGAL_DECODE;
                break;
            case 0x08:                 /* rev */
                tmp = gen_bswap32(tmp);
                ILLEGAL_DECODE;
                break;
            case 0x09:                 /* rev16 */
                tmp = gen_rev16(tmp);
                ILLEGAL_DECODE;
                break;
            case 0x0b:                 /* revsh */
                tmp = gen_revsh(tmp);
                ILLEGAL_DECODE;
                break;
            case 0x10:                 /* sel */
                ILLEGAL_DECODE;
                return;
            case 0x18:                 /* clz */
                tmp = gen_clzi(tmp, 32);
                break;
            case 0x20:
            case 0x21:
            case 0x22:
            case 0x28:
            case 0x29:
            case 0x2a:
                {
                    /* crc32/crc32c */
                    uint32_t sz = op & 0x3;
                    uint32_t c = op & 0x8;

                    tmp2 = state->Reg[rm];
                    tmp3 = 1 << sz;

                    if (sz == 0)
                    {
                        tmp2 &= 0xff;
                    } else if (sz == 1)
                    {
                        tmp2 &= 0xffff;
                    }

                    if (c)
                    {
                        tmp = gen_crc32c(tmp, tmp2, tmp3);
                    } else
                    {
                        tmp = gen_crc32(tmp, tmp2, tmp3);
                    }
                    ILLEGAL_DECODE;
                    break;
                }
            default:
                ILLEGAL_DECODE;
                return;
        }
    }
    state->Reg[rd] = tmp;
}

static void thumb2_mul32_proc(ARMul_State * state, ARMword instr)
{
    ARMword op, rn, rs, rd, rm;
    ARMword tmp, tmp2;

    rn = (instr >> 16) & 0xf;
    rs = (instr >> 12) & 0xf;
    rd = (instr >> 8) & 0xf;
    rm = instr & 0xf;

    op = (instr >> 4) & 0xf;
    tmp = state->Reg[rn];
    tmp2 = state->Reg[rm];

    switch ((instr >> 20) & 7)
    {
        case 0:                        /* 32 x 32 -> 32 */
            tmp = tmp * tmp2;
            if (rs != 15)
            {
                tmp2 = state->Reg[rs];
                if (op)
                    tmp = tmp2 - tmp;
                else
                    tmp = tmp - tmp2;
            }
            break;
        case 7:                        /* Unsigned sum of absolute differences.  */
            tmp = gen_usad8(tmp, tmp2);
            if (rs != 15)
            {
                tmp2 = state->Reg[rs];
                tmp += tmp2;
            }
            break;
        case 1:                        /* 16 x 16 -> 32 */
        case 2:                        /* Dual multiply add.  */
        case 3:                        /* 32 * 16 -> 32msb */
        case 4:                        /* Dual multiply subtract.  */
        case 5:
        case 6:                        /* 32 * 32 -> 32msb (SMMUL, SMMLA, SMMLS) */
            ILLEGAL_DECODE;
            return;
    }

    state->Reg[rd] = tmp;
}

static void thumb2_div_proc(ARMul_State * state, ARMword instr)
{
    ARMword op, rn, rd, rm;

    rn = (instr >> 16) & 0xf;
    rd = (instr >> 8) & 0xf;
    rm = instr & 0xf;

    op = (instr >> 4) & 0xf;

    if (op & 0x20)
    {
        int32_t tmp, tmp2;

        tmp = state->Reg[rn];
        tmp2 = state->Reg[rm];
        tmp = tmp / tmp2;
        state->Reg[rd] = tmp;
    } else
    {
        uint32_t tmp, tmp2;

        tmp = state->Reg[rn];
        tmp2 = state->Reg[rm];
        if (tmp2 == 0)
        {
            skyeye_log(Error_log, __FUNCTION__, "Line:%d Divisor is zero at PC = 0x%x!\n", __LINE__, state->realPC);
            state->Reg[rd] = 0xffffffff;
        } else
        {
            tmp = tmp / tmp2;
            state->Reg[rd] = tmp;
        }
    }
}

static void thumb2_dual_mul_proc(ARMul_State * state, ARMword instr)
{
    ARMword op, rn, rs, rd, rm;
    ARMword tmp, tmp2;

    rn = (instr >> 16) & 0xf;
    rd = (instr >> 8) & 0xf;
    rs = (instr >> 12) & 0xf;
    rm = instr & 0xf;

    op = (instr >> 4) & 0xf;
    tmp = state->Reg[rn];
    tmp2 = state->Reg[rm];

    if (op & 1)
        tmp2 = gen_swap_half(tmp2);

    gen_smul_dual(&tmp, &tmp2);

    if (op & 0x10)
    {
        tmp -= tmp2;
    } else
    {
        tmp += tmp2;
    }

    uint64_t tmp64 = (uint64_t) tmp;

    tmp64 = gen_addq(state, tmp64, rs, rd);
    state->Reg[rs] = tmp64 & 0xffffffff;
    state->Reg[rd] = tmp64 >> 32;
}

static void thumb2_misc_data_process(ARMul_State * state, ARMword instr, ARMword pc)
{
    ARMword op, rn, rs, rd, rm;
    ARMword tmp, tmp2, shift, shiftop, conds, logic_cc;
    uint64_t tmp64;

    rn = (instr >> 16) & 0xf;
    rs = (instr >> 12) & 0xf;
    rd = (instr >> 8) & 0xf;
    rm = instr & 0xf;

    op = ((instr >> 22) & 6) | ((instr >> 7) & 1);
    if (op < 4 && (instr & 0xf000) != 0xf000)
    {
        ILLEGAL_DECODE;
        return;
    }
    switch (op)
    {
        case 0:                        /* Register controlled shift.  */
            tmp = state->Reg[rn];
            tmp2 = state->Reg[rm];
            if ((instr & 0x70) != 0)
            {
                ILLEGAL_DECODE;
                return;
            }
            op = (instr >> 21) & 3;
            logic_cc = (instr & (1 << 20)) != 0;
            tmp = gen_shift_reg(state, tmp, op, tmp2, logic_cc);
            if (logic_cc)
                gen_logic_cc(state, tmp);

            state->Reg[rd] = tmp;
            break;
        case 1:                        /* Sign/zero extend.  */
            op = (instr >> 20) & 7;
            switch (op)
            {
                case 0:                /* SXTAH, SXTH */
                case 1:                /* UXTAH, UXTH */
                case 4:                /* SXTAB, SXTB */
                case 5:                /* UXTAB, UXTB */
                    break;
                case 2:                /* SXTAB16, SXTB16 */
                case 3:                /* UXTAB16, UXTB16 */
                default:
                    ILLEGAL_DECODE;
                    return;
            }

            tmp = state->Reg[rm];
            shift = (instr >> 4) & 3;
            /* ??? In many cases it's not necessary to do a
             * rotate, a shift is sufficient.  */
            if (shift != 0)
                tmp = gen_rotr(tmp, shift * 8);

            op = (instr >> 20) & 7;
            switch (op)
            {
                case 0:
                    gen_sxth(tmp);
                    break;
                case 1:
                    gen_uxth(tmp);
                    break;
                case 4:
                    gen_sxtb(tmp);
                    break;
                case 5:
                    gen_uxtb(tmp);
                    break;
                default:
                    ILLEGAL_DECODE;
                    return;
            }
            if (rn != 15)
            {
                tmp2 = state->Reg[rn];
                if ((op >> 1) == 1)
                {
                    tmp = gen_add16(tmp, tmp2);
                } else
                {
                    tmp += tmp2;
                }
            }
            state->Reg[rd] = tmp;
            break;
        case 2:                        /* SIMD add/subtract.  */
            op = (instr >> 20) & 7;
            shift = (instr >> 4) & 7;
            if ((op & 3) == 3 || (shift & 3) == 3)
            {
                ILLEGAL_DECODE;
                return;
            }
            tmp = state->Reg[rn];
            tmp2 = state->Reg[rm];
            //gen_thumb2_parallel_addsub(op, shift, tmp, tmp2);
            state->Reg[rd] = tmp;
            break;
        case 3:                        /* Other data processing.  */
            thumb2_other_data_process(state, instr, pc);
            break;
        case 4:
        case 5:                        /* 32-bit multiply.  Sum of absolute differences.  */
            thumb2_mul32_proc(state, instr);
            break;
        case 6:
        case 7:                        /* 64-bit multiply, Divide.  */
            op = ((instr >> 4) & 0xf) | ((instr >> 16) & 0x70);
            tmp = state->Reg[rn];
            tmp2 = state->Reg[rm];
            if ((op & 0x50) == 0x10)
            {
                /* sdiv, udiv */
                thumb2_div_proc(state, instr);
            } else if ((op & 0xe) == 0xc)
            {
                /* Dual multiply accumulate long.  */
                thumb2_dual_mul_proc(state, instr);
            } else
            {
                if (op & 0x20)
                {
                    /* Unsigned 64-bit multiply  */
                    tmp64 = gen_mulu(tmp, tmp2);
                } else
                {
                    if (op & 8)
                    {
                        /* smlalxy */
                        ILLEGAL_DECODE;
                        return;
                    } else
                    {
                        /* Signed 64-bit multiply  */
                        tmp64 = gen_muls(tmp, tmp2);
                    }
                }
                if (op & 4)
                {
                    /* umaal */
                    tmp64 = gen_addq_lo(state, tmp64, rs);
                    tmp64 = gen_addq_lo(state, tmp64, rd);
                } else if (op & 0x40)
                {
                    /* 64-bit accumulate.  */
                    tmp64 = gen_addq(state, tmp64, rs, rd);
                }
                state->Reg[rs] = tmp64 & 0xffffffff;
                state->Reg[rd] = tmp64 >> 32;
            }
            break;
        default:
            ILLEGAL_DECODE;
            break;
    }

    return;
}

bool_t is_armv6m_insn(ARMul_State * state, ARMword instr, ARMword * new_instr)
{
    /*
     * ARMv6-M supports a limited subset of Thumb2 instructions.
     * Other Thumb1 architectures allow only 32-bit
     * combined BL/BLX prefix and suffix.
     */
    if (state->is_v7 == 0 && state->is_v6m)
    {
        int i;
        static const uint32_t armv6m_insn[] = { 0xf3808000 /* msr */ ,
            0xf3b08040 /* dsb */ ,
            0xf3b08050 /* dmb */ ,
            0xf3b08060 /* isb */ ,
            0xf3e08000 /* mrs */ ,
            0xf000d000 /* bl */
        };
        static const uint32_t armv6m_mask[] = { 0xffe0d000,
            0xffF0d0f0,
            0xffF0d0f0,
            0xffF0d0f0,
            0xffe0d000,
            0xf800d000
        };

        for (i = 0; i < sizeof (armv6m_insn) / sizeof (uint32_t); i++)
        {
            if ((instr & armv6m_mask[i]) == armv6m_insn[i])
            {
                *new_instr = armv6m_insn[i];
                return True;
            }
        }
    }

    return False;
}

static void thumb2_vfp_insn_proc(ARMul_State * state, ARMword instr, ARMword pc)
{
    uint32_t op, lhs, temp;

    op = BITS(20, 27);

    switch (op)
    {
        case 0xd2:
        case 0xd6:
            /* Store , WriteBack , Pre Dec.  */
            lhs = LHS - LSCOff;
            state->Base = lhs;
            ARMul_STC(state, instr, lhs);
            break;
        case 0xd0:
        case 0xd4:
            /* Store , No WriteBack , Pre Dec.  */
            ARMul_STC(state, instr, LHS - LSCOff);
            break;
        case 0xd1:
        case 0xd5:
            /* Load , No WriteBack , Pre Dec.  */
            ARMul_LDC(state, instr, LHS - LSCOff);
            break;
        case 0xd3:
        case 0xd7:
            /* Load , WriteBack , Pre Dec.  */
            lhs = LHS - LSCOff;
            state->Base = lhs;
            ARMul_LDC(state, instr, lhs);
            break;
        case 0xd8:
        case 0xdc:
            /* Store , No WriteBack , Pre Inc.  */
            ARMul_STC(state, instr, LHS + LSCOff);
            break;
        case 0xd9:
        case 0xdd:
            /* Load , No WriteBack , Pre Inc.  */
            ARMul_LDC(state, instr, LHS + LSCOff);
            break;
        case 0xda:
        case 0xde:
            /* Store , WriteBack , Pre Inc.  */
            lhs = LHS + LSCOff;
            state->Base = lhs;
            ARMul_STC(state, instr, lhs);
            break;
        case 0xdb:
        case 0xdf:
            /* Load , WriteBack , Pre Inc.  */
            lhs = LHS + LSCOff;
            state->Base = lhs;
            ARMul_LDC(state, instr, lhs);
            break;
        case 0xcb:
        case 0xcf:
            /* Load , WriteBack , Post Inc.  */
            lhs = LHS;
            state->Base = lhs + LSCOff;
            ARMul_LDC(state, instr, LHS);
            break;
            /* Co-Processor Register Transfers (MRC) and Data Ops.  */
        case 0xe1:
        case 0xe3:
        case 0xe5:
        case 0xe7:
        case 0xe9:
        case 0xeb:
        case 0xed:
        case 0xef:
            if (BIT(4))
            {
                /* MRC */
                temp = ARMul_MRC(state, instr);
                if (DESTReg == 15)
                {
                    ASSIGNN((temp & NBIT) != 0);
                    ASSIGNZ((temp & ZBIT) != 0);
                    ASSIGNC((temp & CBIT) != 0);
                    ASSIGNV((temp & VBIT) != 0);
                } else
                    DEST = temp;
            } else
                /* CDP Part 2.  */
                ARMul_CDP(state, instr);
            break;
        case 0xe0:
        case 0xe2:
        case 0xe4:
        case 0xe6:
        case 0xe8:
        case 0xea:
        case 0xec:
        case 0xee:
            if (BIT(4))
            {
                /* MCR.  */
                if (DESTReg == 15)
                {
                    UNDEF_MCRPC;
#ifdef MODE32
                    ARMul_MCR(state, instr, state->Reg[15] + isize);
#else
                    ARMul_MCR(state, instr, ECC | ER15INT | EMODE | ((state->Reg[15] + isize) & R15PCBITS));
#endif
                } else
                    ARMul_MCR(state, instr, DEST);
            } else
                /* CDP Part 1.  */
                ARMul_CDP(state, instr);
            break;
        case 0xc4:
            if (state->is_v5)
            {
                /* Reading from R15 is UNPREDICTABLE.  */
                if (BITS(12, 15) == 15 || BITS(16, 19) == 15)
                    ARMul_UndefInstr(state, instr);
                else if (state->is_XScale)
                {
                    /* Only opcode 0 is supported.  */
                    if (BITS(4, 7) != 0x00)
                        ARMul_UndefInstr(state, instr);
                    /* Only coporcessor 0 is supported.  */
                    else if (CPNum != 0x00)
                        ARMul_UndefInstr(state, instr);
                    /* Only accumulator 0 is supported.  */
                    else if (BITS(0, 3) != 0x00)
                        ARMul_UndefInstr(state, instr);
                    else
                    {
                        /* XScale MAR insn.  Move two registers into accumulator.  */
                        state->Accumulator = state->Reg[BITS(12, 15)];
                        state->Accumulator += (ARMdword) state->Reg[BITS(16, 19)] << 32;
                    }
                } else
                {
                    /* MCRR, ARMv5TE and up */
                    ARMul_MCRR(state, instr, DEST, state->Reg[LHSReg]);
                }
                break;
            }
        case 0xc5:
            if (state->is_v5)
            {
                /* Writes to R15 are UNPREDICATABLE.  */
                if (DESTReg == 15 || LHSReg == 15)
                    ARMul_UndefInstr(state, instr);
                else if (state->is_XScale)
                {
                    /* Only opcode 0 is supported.  */
                    if (BITS(4, 7) != 0x00)
                        ARMul_UndefInstr(state, instr);
                    /* Only coprocessor 0 is supported.  */
                    else if (CPNum != 0x00)
                        ARMul_UndefInstr(state, instr);
                    /* Only accumulator 0 is supported.  */
                    else if (BITS(0, 3) != 0x00)
                        ARMul_UndefInstr(state, instr);
                    else
                    {
                        /* XScale MRA insn.  Move accumulator into two registers.  */
                        ARMword t1 = (state->Accumulator >> 32) & 255;

                        if (t1 & 128)
                            t1 -= 256;

                        state->Reg[BITS(12, 15)] = state->Accumulator;
                        state->Reg[BITS(16, 19)] = t1;
                        break;
                    }
                } else
                {
                    /* MRRC, ARMv5TE and up */
                    ARMul_MRRC(state, instr, &DEST, &(state->Reg[LHSReg]));
                }
                break;
            }
        default:
            ILLEGAL_DECODE;
            return;
    }
}

static void thumb2_coproc_insn_proc(ARMul_State * state, ARMword instr, ARMword pc)
{
    uint32_t op, temp;

    op = BITS(20, 27);

    switch (op)
    {
        default:
            ILLEGAL_DECODE;
            return;
    }

}

tdstate ARMul_Thumb2Decode(state, pc, instr, new_instr)
    ARMul_State *state;
    ARMword pc;
    ARMword instr;
    ARMword *new_instr;
{
    tdstate valid = t_t2_decoded;
    ARMword op;

    if (is_armv6m_insn(state, instr, new_instr))
    {
        valid = t_decoded;
        goto success_end;
    }

    op = (instr >> 25) & 0xf;

    switch (op)
    {
        case 4:
            if (instr & (1 << 22))
            {
                valid = thumb2_ldst_dual_porc(state, instr, pc);
            } else
            {
                valid = thumb2_ldst_mult_porc(state, instr);
            }
            break;
        case 5:
            thumb2_data_process_register(state, instr);
            break;
        case 13:
            thumb2_misc_data_process(state, instr, pc);
            break;
        case 6:
        case 7:
        case 14:
        case 15:
            /* Coprocessor.  */
            if ((instr & 0xfe000a00) == 0xfc000800)
            {
                ILLEGAL_DECODE;
                return valid;
            } else if ((instr & 0xff000a00) == 0xfe000800)
            {
                /* The Thumb2 and ARM encodings are identical.  */
                ILLEGAL_DECODE;
                return valid;
            } else if (((instr >> 24) & 3) == 3)
            {
                /* Translate into the equivalent ARM encoding.  */
                *new_instr = (instr & 0xe2ffffff) | ((instr & (1 << 28)) >> 4) | (1 << 28);
                valid = t_decoded;
                //ILLEGAL_DECODE;
                return valid;
            } else if (((instr >> 8) & 0xe) == 10)
            {
                thumb2_vfp_insn_proc(state, instr, pc);
                return valid;
            } else
            {
                if (instr & (1 << 28))
                {
                    ILLEGAL_DECODE;
                    return valid;
                }
                thumb2_coproc_insn_proc(state, instr, pc);
                return valid;
            }
            break;
        case 8:
        case 9:
        case 10:
        case 11:
            if (instr & (1 << 15))
            {
                /* Branches, misc control.  */
                if (instr & 0x5000)
                {
                    /* Unconditional branch.  */
                    thumb2_uncond_branch_proc(state, instr);
                    FLUSHPIPE;
                    valid = t_branch;
                } else if (((instr >> 23) & 7) == 7)
                {
                    /* Misc control */
                    valid = thumb2_misc_control_proc(state, instr);
                } else
                {
                    /* Conditional branch.  */
                    thumb2_cond_branch_proc(state, instr, pc);
                    FLUSHPIPE;
                    valid = t_branch;
                }
            } else
            {
                /* Data processing immediate.  */
                thumb2_data_process_imm(state, instr, pc);
            }
            break;
        case 12:
            /* Load/store single data item. */
            valid = thumb2_ldst_single_proc(state, instr);
            break;
        default:
            ILLEGAL_DECODE;
            return valid;
    }

  success_end:
    return valid;
}

/* Decode a 16bit Thumb instruction.  The instruction is in the low
 * 16-bits of the tinstr field, with the following Thumb instruction
 * held in the high 16-bits.  Passing in two Thumb instructions allows
 * easier simulation of the special dual BL instruction.  */

tdstate ARMul_ThumbDecode(state, pc, tinstr, ainstr)
    ARMul_State *state;
    ARMword pc;
    ARMword tinstr;
    ARMword *ainstr;
{
    tdstate valid = t_decoded;          /* default assumes a valid instruction */
    ARMword next_instr;

    if (state->bigendSig)
    {
        next_instr = tinstr & 0xFFFF;
        tinstr >>= 16;
    } else
    {
        next_instr = tinstr >> 16;
        tinstr &= 0xFFFF;
    }

#if 1                                   /* debugging to catch non updates */
    *ainstr = 0xDEADC0DE;
#endif

    switch ((tinstr & 0xF800) >> 11)
    {
        case 0:                        /* LSL */
        case 1:                        /* LSR */
        case 2:                        /* ASR */
            /* Format 1 */
            *ainstr = 0xE1B00000        /* base opcode */
                | ((tinstr & 0x1800) >> (11 - 5))       /* shift type */
                | ((tinstr & 0x07C0) << (7 - 6))        /* imm5 */
                | ((tinstr & 0x0038) >> 3)      /* Rs */
                | ((tinstr & 0x0007) << 12);    /* Rd */
            break;
        case 3:                        /* ADD/SUB */
            /* Format 2 */
            {
                ARMword subset[4] = {
                    0xE0900000,         /* ADDS Rd,Rs,Rn    */
                    0xE0500000,         /* SUBS Rd,Rs,Rn    */
                    0xE2900000,         /* ADDS Rd,Rs,#imm3 */
                    0xE2500000          /* SUBS Rd,Rs,#imm3 */
                };
                /* It is quicker indexing into a table, than performing switch
                 * or conditionals: */
                *ainstr = subset[(tinstr & 0x0600) >> 9]        /* base opcode */
                    | ((tinstr & 0x01C0) >> 6)  /* Rn or imm3 */
                    | ((tinstr & 0x0038) << (16 - 3))   /* Rs */
                    | ((tinstr & 0x0007) << (12 - 0));  /* Rd */
            }
            break;
        case 4:                        /* MOV */
        case 5:                        /* CMP */
        case 6:                        /* ADD */
        case 7:                        /* SUB */
            /* Format 3 */
            {
                ARMword subset[4] = {
                    0xE3B00000,         /* MOVS Rd,#imm8    */
                    0xE3500000,         /* CMP  Rd,#imm8    */
                    0xE2900000,         /* ADDS Rd,Rd,#imm8 */
                    0xE2500000,         /* SUBS Rd,Rd,#imm8 */
                };
                *ainstr = subset[(tinstr & 0x1800) >> 11]       /* base opcode */
                    | ((tinstr & 0x00FF) >> 0)  /* imm8 */
                    | ((tinstr & 0x0700) << (16 - 8))   /* Rn */
                    | ((tinstr & 0x0700) << (12 - 8));  /* Rd */
            }
            break;
        case 8:                        /* Arithmetic and high register transfers */
            /* TODO: Since the subsets for both Format 4 and Format 5
             * instructions are made up of different ARM encodings, we could
             * save the following conditional, and just have one large
             * subset. */
            if (tinstr)
                if ((tinstr & (1 << 10)) == 0)
                {
                    /* Format 4 */
                    struct
                    {
                        ARMword opcode;
                        enum
                        { t_norm, t_shift, t_neg, t_mul }
                        otype;
                    }
                    subset[16] =
                    {
                        {
                        0xE0100000, t_norm},    /* ANDS Rd,Rd,Rs     */
                        {
                        0xE0300000, t_norm},    /* EORS Rd,Rd,Rs     */
                        {
                        0xE1B00010, t_shift},   /* MOVS Rd,Rd,LSL Rs */
                        {
                        0xE1B00030, t_shift},   /* MOVS Rd,Rd,LSR Rs */
                        {
                        0xE1B00050, t_shift},   /* MOVS Rd,Rd,ASR Rs */
                        {
                        0xE0B00000, t_norm},    /* ADCS Rd,Rd,Rs     */
                        {
                        0xE0D00000, t_norm},    /* SBCS Rd,Rd,Rs     */
                        {
                        0xE1B00070, t_shift},   /* MOVS Rd,Rd,ROR Rs */
                        {
                        0xE1100000, t_norm},    /* TST  Rd,Rs        */
                        {
                        0xE2700000, t_neg},     /* RSBS Rd,Rs,#0     */
                        {
                        0xE1500000, t_norm},    /* CMP  Rd,Rs        */
                        {
                        0xE1700000, t_norm},    /* CMN  Rd,Rs        */
                        {
                        0xE1900000, t_norm},    /* ORRS Rd,Rd,Rs     */
                        {
                        0xE0100090, t_mul},     /* MULS Rd,Rd,Rs     */
                        {
                        0xE1D00000, t_norm},    /* BICS Rd,Rd,Rs     */
                        {
                        0xE1F00000, t_norm}     /* MVNS Rd,Rs        */
                    };
                    *ainstr = subset[(tinstr & 0x03C0) >> 6].opcode;    /* base */
                    switch (subset[(tinstr & 0x03C0) >> 6].otype)
                    {
                        case t_norm:
                            *ainstr |= ((tinstr & 0x0007) << 16)        /* Rn */
                                | ((tinstr & 0x0007) << 12)     /* Rd */
                                | ((tinstr & 0x0038) >> 3);     /* Rs */
                            break;
                        case t_shift:
                            *ainstr |= ((tinstr & 0x0007) << 12)        /* Rd */
                                | ((tinstr & 0x0007) >> 0)      /* Rm */
                                | ((tinstr & 0x0038) << (8 - 3));       /* Rs */
                            break;
                        case t_neg:
                            *ainstr |= ((tinstr & 0x0007) << 12)        /* Rd */
                                | ((tinstr & 0x0038) << (16 - 3));      /* Rn */
                            break;
                        case t_mul:
                            *ainstr |= ((tinstr & 0x0007) << 16)        /* Rd */
                                | ((tinstr & 0x0007) << 8)      /* Rs */
                                | ((tinstr & 0x0038) >> 3);     /* Rm */
                            break;
                    }
                } else
                {
                    /* Format 5 */
                    ARMword Rd = ((tinstr & 0x0007) >> 0);
                    ARMword Rs = ((tinstr & 0x0038) >> 3);

                    if (tinstr & (1 << 7))
                        Rd += 8;
                    if (tinstr & (1 << 6))
                        Rs += 8;
                    //printf("in %s,(tinstr & (1 << 10)) == 1, (tinstr & 0x03C0) >> 6=0x%x\n", __FUNCTION__, (tinstr & 0x03C0) >> 6);
                    switch ((tinstr & 0x03C0) >> 6)
                    {
                        case 0x0:      /* ADD{S}<c> <Rd>, <Rn>, <Rm>*/
                        case 0x1:      /* ADD Rd,Rd,Hs */
                        case 0x2:      /* ADD Hd,Hd,Rs */
                        case 0x3:      /* ADD Hd,Hd,Hs */
                            *ainstr = 0xE0800000        /* base */
                                | (Rd << 16)    /* Rn */
                                | (Rd << 12)    /* Rd */
                                | (Rs << 0);    /* Rm */
                            break;
                        case 0x5:      /* CMP Rd,Hs */
                        case 0x6:      /* CMP Hd,Rs */
                        case 0x7:      /* CMP Hd,Hs */
                            *ainstr = 0xE1500000        /* base */
                                | (Rd << 16)    /* Rn */
                                | (Rd << 12)    /* Rd */
                                | (Rs << 0);    /* Rm */
                            break;
                        case 0x8:
                            //printf("add new encode for tinstr=0x%x, Rd=%d, Rs=%d\n", tinstr, Rd, Rs);
                        case 0x9:      /* MOV Rd,Hs */
                        case 0xA:      /* MOV Hd,Rs */
                        case 0xB:      /* MOV Hd,Hs */
                            *ainstr = 0xE1A00000        /* base */
                                | (Rd << 16)    /* Rn */
                                | (Rd << 12)    /* Rd */
                                | (Rs << 0);    /* Rm */
                            break;
                        case 0xC:      /* BX Rs */
                        case 0xD:      /* BX Hs */
                            *ainstr = 0xE12FFF10        /* base */
                                | ((tinstr & 0x0078) >> 3);     /* Rd */
                            break;
                        //case 0x0:      /* UNDEFINED */
                        case 0x4:      /* UNDEFINED */
                            //case 0x8:     /* UNDEFINED */
                            valid = t_undefined;
                            break;
                        case 0xE:      /* BLX */
                        case 0xF:      /* BLX */
                            if (state->is_v5)
                            {
                                *ainstr = 0xE1200030    /* base */
                                    | (Rs << 0);        /* Rm */
                            } else
                            {
                                valid = t_undefined;
                            }
                            break;
                    }
                }
            break;
        case 9:                        /* LDR Rd,[PC,#imm8] */
            /* Format 6 */
            *ainstr = 0xE59F0000        /* base */
                | ((tinstr & 0x0700) << (12 - 8))       /* Rd */
                | ((tinstr & 0x00FF) << (2 - 0));       /* off8 */
            break;
        case 10:
        case 11:
            /* TODO: Format 7 and Format 8 perform the same ARM encoding, so
             * the following could be merged into a single subset, saving on
             * the following boolean: */
            if ((tinstr & (1 << 9)) == 0)
            {
                /* Format 7 */
                ARMword subset[4] = {
                    0xE7800000,         /* STR  Rd,[Rb,Ro] */
                    0xE7C00000,         /* STRB Rd,[Rb,Ro] */
                    0xE7900000,         /* LDR  Rd,[Rb,Ro] */
                    0xE7D00000          /* LDRB Rd,[Rb,Ro] */
                };
                *ainstr = subset[(tinstr & 0x0C00) >> 10]       /* base */
                    | ((tinstr & 0x0007) << (12 - 0))   /* Rd */
                    | ((tinstr & 0x0038) << (16 - 3))   /* Rb */
                    | ((tinstr & 0x01C0) >> 6); /* Ro */
            } else
            {
                /* Format 8 */
                ARMword subset[4] = {
                    0xE18000B0,         /* STRH  Rd,[Rb,Ro] */
                    0xE19000D0,         /* LDRSB Rd,[Rb,Ro] */
                    0xE19000B0,         /* LDRH  Rd,[Rb,Ro] */
                    0xE19000F0          /* LDRSH Rd,[Rb,Ro] */
                };
                *ainstr = subset[(tinstr & 0x0C00) >> 10]       /* base */
                    | ((tinstr & 0x0007) << (12 - 0))   /* Rd */
                    | ((tinstr & 0x0038) << (16 - 3))   /* Rb */
                    | ((tinstr & 0x01C0) >> 6); /* Ro */
            }
            break;
        case 12:                       /* STR Rd,[Rb,#imm5] */
        case 13:                       /* LDR Rd,[Rb,#imm5] */
        case 14:                       /* STRB Rd,[Rb,#imm5] */
        case 15:                       /* LDRB Rd,[Rb,#imm5] */
            /* Format 9 */
            {
                ARMword subset[4] = {
                    0xE5800000,         /* STR  Rd,[Rb,#imm5] */
                    0xE5900000,         /* LDR  Rd,[Rb,#imm5] */
                    0xE5C00000,         /* STRB Rd,[Rb,#imm5] */
                    0xE5D00000          /* LDRB Rd,[Rb,#imm5] */
                };
                /* The offset range defends on whether we are transferring a
                 * byte or word value: */
                *ainstr = subset[(tinstr & 0x1800) >> 11]       /* base */
                    | ((tinstr & 0x0007) << (12 - 0))   /* Rd */
                    | ((tinstr & 0x0038) << (16 - 3))   /* Rb */
                    | ((tinstr & 0x07C0) >> (6 - ((tinstr & (1 << 12)) ? 0 : 2)));      /* off5 */
            }
            break;
        case 16:                       /* STRH Rd,[Rb,#imm5] */
        case 17:                       /* LDRH Rd,[Rb,#imm5] */
            /* Format 10 */
            *ainstr = ((tinstr & (1 << 11))     /* base */
                       ? 0xE1D000B0     /* LDRH */
                       : 0xE1C000B0)    /* STRH */
                | ((tinstr & 0x0007) << (12 - 0))       /* Rd */
                | ((tinstr & 0x0038) << (16 - 3))       /* Rb */
                | ((tinstr & 0x01C0) >> (6 - 1))        /* off5, low nibble */
                | ((tinstr & 0x0600) >> (9 - 8));       /* off5, high nibble */
            break;
        case 18:                       /* STR Rd,[SP,#imm8] */
        case 19:                       /* LDR Rd,[SP,#imm8] */
            /* Format 11 */
            *ainstr = ((tinstr & (1 << 11))     /* base */
                       ? 0xE59D0000     /* LDR */
                       : 0xE58D0000)    /* STR */
                | ((tinstr & 0x0700) << (12 - 8))       /* Rd */
                | ((tinstr & 0x00FF) << 2);     /* off8 */
            break;
        case 20:                       /* ADD Rd,PC,#imm8 */
        case 21:                       /* ADD Rd,SP,#imm8 */
            /* Format 12 */
            if ((tinstr & (1 << 11)) == 0)
            {
                /* NOTE: The PC value used here should by word aligned */
                /* We encode shift-left-by-2 in the rotate immediate field,
                 * so no shift of off8 is needed.  */
                *ainstr = 0xE28F0F00    /* base */
                    | ((tinstr & 0x0700) << (12 - 8))   /* Rd */
                    | (tinstr & 0x00FF);        /* off8 */
            } else
            {
                /* We encode shift-left-by-2 in the rotate immediate field,
                 * so no shift of off8 is needed.  */
                *ainstr = 0xE28D0F00    /* base */
                    | ((tinstr & 0x0700) << (12 - 8))   /* Rd */
                    | (tinstr & 0x00FF);        /* off8 */
            }
            break;
        case 22:
        case 23:
            if ((tinstr & 0xFFC0) == 0xB2c0)
            {                           /* UXTB */
                int rd = tinstr & 0x7;
                int rm = (tinstr >> 3) & 0x7;

                *ainstr = 0xe6ef0000 | (rd << 12) | (0x7 << 4) | rm;

            } else if ((tinstr & 0xFFC0) == 0xB280)
            {                           /* UXTH */
                int rd = tinstr & 0x7;
                int rm = (tinstr >> 3) & 0x7;

                *ainstr = 0xe6ff0000 | (rd << 12) | (0x7 << 4) | rm;
            } else if ((tinstr & 0x0F00) == 0x0000)
            {
                /* Format 13 */
                /* NOTE: The instruction contains a shift left of 2
                 * equivalent (implemented as ROR #30): */
                *ainstr = ((tinstr & (1 << 7))  /* base */
                           ? 0xE24DDF00 /* SUB */
                           : 0xE28DDF00)        /* ADD */
                    | (tinstr & 0x007F);        /* off7 */
            } else
            {
                int op = (tinstr >> 8) & 0xf;
                uint32_t tmp, val, rd, rm;

                switch (op)
                {
                    case 0:            /* adjust stack pointer */
                        tmp = state->Reg[13];
                        val = (tinstr & 0x7f) * 4;
                        if (tinstr & (1 << 7))
                            val = -(int32_t) val;
                        tmp += val;
                        state->Reg[13] = tmp;
                        *ainstr = 0;
                        break;
                    case 2:            /* sign/zero extend.  */
                        rd = tinstr & 7;
                        rm = (tinstr >> 3) & 7;
                        tmp = state->Reg[rm];
                        switch ((tinstr >> 6) & 3)
                        {
                            case 0:
                                tmp = gen_sxth(tmp);
                                break;
                            case 1:
                                tmp = gen_sxtb(tmp);
                                break;
                            case 2:
                                tmp = gen_uxth(tmp);
                                break;
                            case 3:
                                tmp = gen_uxtb(tmp);
                                break;
                        }
                        state->Reg[rd] = tmp;
                        *ainstr = 0;
                        break;
                    case 4:
                    case 5:
                    case 0xc:
                    case 0xd:
                        //push and pop instruction
                        valid = thumb_push_pop(state, tinstr);
                        if (valid == t_branch)
                        {
                            FLUSHPIPE;
                        }

                        *ainstr = 0;
                        break;
                    case 1:
                    case 3:
                    case 9:
                    case 11:           /* czb */
                        //czb instruction
                        {
                            if (True == thumb_czb(state, tinstr, pc))
                            {
                                //go to branch , return t_branch
                                valid = t_branch;
                                FLUSHPIPE;
                            } else
                            {
                                *ainstr = 0;
                            }
                        }
                        break;
                    case 6:
                        {
                            int im = (tinstr & 0x10) >> 4;

                            state->PRIMASK = im;
                            *ainstr = 0x0;
                        }
                        break;
                    case 14:
                        {
                            /*FIXME bkpt instruction.This value is ignored by the processor, but can be used by a
debugger to store more information about the breakpoint*/
                            //need implement BKPTInstrDebugEvent
                            break;
                        }
                    case 15:
                        /* If Then.  */
                        if ((tinstr & 0xf) == 0)
                        {
                            //nop instruction
                        } else
                        {
                            state->condexec_cond = (tinstr >> 4) & 0xf;
                            state->condexec_mask = tinstr & 0xf;
                            state->inITblock = True;
                            check_it_block_insn_exec(state, tinstr);
                        }
                        *ainstr = 0;
                        break;
                    default:
                        printf("In %s, Line:%d Not Implement op = %d pc = %x!\n", __func__, __LINE__, op, pc);
                        valid = t_undefined;
                        break;
                }
            }
            break;
        case 24:                       /* STMIA */
        case 25:                       /* LDMIA */
            /* Format 15 */
            *ainstr = ((tinstr & (1 << 11))     /* base */
                       ? 0xE8B00000     /* LDMIA */
                       : 0xE8A00000)    /* STMIA */
                | ((tinstr & 0x0700) << (16 - 8))       /* Rb */
                | (tinstr & 0x00FF);    /* mask8 */
            break;
        case 26:                       /* Bcc */
        case 27:                       /* Bcc/SWI */
            if ((tinstr & 0x0F00) == 0x0F00)
            {
                if (tinstr == (ARMul_ABORTWORD & 0xffff) && state->AbortAddr == pc)
                {
                    *ainstr = ARMul_ABORTWORD;
                    break;
                }
                /* Format 17 : SWI */
                *ainstr = 0xEF000000;
                /* Breakpoint must be handled specially.  */
                if ((tinstr & 0x00FF) == 0x18)
                    *ainstr |= ((tinstr & 0x00FF) << 16);
                /* New breakpoint value.  See gdb/arm-tdep.c  */
                else if ((tinstr & 0x00FF) == 0xFE)
                    *ainstr |= SWI_Breakpoint;
                else
                    *ainstr |= (tinstr & 0x00FF);
            } else if ((tinstr & 0x0F00) != 0x0E00)
            {
                /* Format 16 */
                bool_t doit = FALSE;
                int cond = (tinstr & 0x0F00) >> 8;

                doit = getCondResult(state, cond);
                /* TODO: Since we are doing a switch here, we could just add
                 * the SWI and undefined instruction checks into this
                 * switch to same on a couple of conditionals: */

                if (doit)
                {
                    state->Reg[15] = (pc + 4 + (((tinstr & 0x7F) << 1) | ((tinstr & (1 << 7)) ? 0xFFFFFF00 : 0)));
                    FLUSHPIPE;
                }
                valid = t_branch;
            } else                      /* UNDEFINED : cc=1110(AL) uses different format */
                valid = t_undefined;
            break;
        case 28:                       /* B */
            /* Format 18 */
            state->Reg[15] = (pc + 4 + (((tinstr & 0x3FF) << 1) | ((tinstr & (1 << 10)) ? 0xFFFFF800 : 0)));
            FLUSHPIPE;
            valid = t_branch;
            break;
        case 29:
            if (tinstr & 0x1)
                valid = t_undefined;
            else
            {
                /* BLX 1 for armv5t and above */
                ARMword tmp = (pc + 2);

                state->Reg[15] = (state->Reg[14] + ((tinstr & 0x07FF) << 1)) & 0xFFFFFFFC;
                state->Reg[14] = (tmp | 1);
                CLEART;
                DBG("In %s, After  BLX(1),LR=0x%x,PC=0x%x, offset=0x%x\n", __FUNCTION__, state->Reg[14], state->Reg[15],
                    (tinstr & 0x7FF) << 1);
                valid = t_branch;
                FLUSHPIPE;
            }
            break;
        case 30:                       /* BL instruction 1 */
            /* Format 19 */
            /* There is no single ARM instruction equivalent for this Thumb
             * instruction. To keep the simulation simple (from the user
             * perspective) we check if the following instruction is the
             * second half of this BL, and if it is we simulate it
             * immediately.  */
            if (tinstr == 0xf3bf)
            {                           /*DSB ISB */
                *ainstr = 0x0;
                state->pc = pc + 2;
                FLUSHPIPE;
                valid = t_branch;       /* in-case we don't have the 2nd half */
            } else if (tinstr == 0xf3ef)
            {                           /* MRS */
                ARMword rd = (next_instr >> 8) & 0xf;

                state->Reg[rd] = 0;
                ARMword sysm = next_instr & 0xff;

                if ((sysm >> 3) == 0)
                {
                    printf("%s The MRS instruction is not implemented fully\n", __FUNCTION__);
                } else if ((sysm >> 3) == 1)
                {
                    printf("%s The MRS instruction is not implemented fully\n", __FUNCTION__);
                } else if ((sysm >> 3) == 2)
                {
                    if ((sysm & 0x3) == 0)
                    {
                        state->Reg[rd] |= state->PRIMASK;
                    } else if ((sysm & 0x3) == 0x4)
                    {
                        printf("%s The MRS instruction is not implemented fully\n", __FUNCTION__);
                    }
                }
                *ainstr = 0x0;
                state->pc = pc + 2;
                FLUSHPIPE;
                valid = t_branch;       /* in-case we don't have the 2nd half */
            } else if ((tinstr & 0xfff0) == 0xf380)
            {
                /*MSR*/ ARMword reg_n = (tinstr & 0xf);
                //if(next_instr == 0){
                uint32_t data = 0;

                state->space.read(state->space.conf_obj, pc, &data, 4);
                next_instr = data >> 16;
                //}
                ARMword sysm = next_instr & 0xff;

                if ((sysm >> 3) == 0)
                {
                    printf("%s The MSR instruction is not implemented fully\n", __FUNCTION__);
                } else if ((sysm >> 3) == 1)
                {
                    if ((sysm & 0x7) == 0)
                    {
                        state->MSP = state->Reg[reg_n] & 0xfffffffc;
                    } else if ((sysm & 0x7) == 1)
                    {
                        uint32_t data = 0;

                        state->PSP = state->Reg[reg_n] & 0xfffffffc;
                        state->space.read(state->space.conf_obj, state->PSP, &data, 4);
                        // printf("psp = 0x%x, pc = 0x%x data=0x%x\n",state->PSP,state->Reg[13],data);
                    }
                } else if ((sysm >> 3) == 2)
                {
                    if ((sysm & 0x7) == 0)
                    {
                        state->PRIMASK = state->Reg[reg_n] & 0x1;
                    } else if ((sysm & 0x7) == 4)
                    {
                        if (((state->Reg[reg_n] & 0x3) >> 1) == 0)
                        {
                            state->Reg[13] = state->MSP;
                        } else if (((state->Reg[reg_n] & 0x3) >> 1) == 1)
                        {
                            state->Reg[13] = state->PSP;
                        }
                    }
                }
                //printf("rn = 0x%x, tinstr = 0x%x, sysm = 0x%x ,next_instr = 0x%x pc = 0x%x\n",reg_n,tinstr,sysm,next_instr,pc);
                *ainstr = 0x0;
                state->pc = pc + 2;
                FLUSHPIPE;
                valid = t_branch;       /* in-case we don't have the 2nd half */
            } else
            {
#if 1
                state->Reg[14] = state->Reg[15] + (((tinstr & 0x07FF) << 12) | ((tinstr & (1 << 10)) ? 0xFF800000 : 0));
                valid = t_branch;       /* in-case we don't have the 2nd half */
                //tinstr = next_instr;  /* move the instruction down */
                //if (((tinstr & 0xF800) >> 11) != 31)
                //      break;  /* exit, since not correct instruction */
                /* else we fall through to process the second half of the BL */
                //pc += 2;      /* point the pc at the 2nd half */
                state->Reg[15] = pc + 2;
                FLUSHPIPE;
#endif
                valid = t_branch;       /* in-case we don't have the 2nd half */
            }
            break;
        case 31:                       /* BL instruction 2 */
            /* Format 19 */
            /* There is no single ARM instruction equivalent for this
             * instruction. Also, it should only ever be matched with the
             * fmt19 "BL instruction 1" instruction. However, we do allow
             * the simulation of it on its own, with undefined results if
             * r14 is not suitably initialised.  */
            {
                ARMword tmp = (pc + 2);

                state->Reg[15] = (state->Reg[14] + ((tinstr & 0x07FF) << 1));
                state->Reg[14] = (tmp | 1);
                valid = t_branch;
                FLUSHPIPE;
            }
            break;
        default:
            break;
    }
    return valid;
}
