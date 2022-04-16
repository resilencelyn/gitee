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
* @file c6k_decode_16.c
* @brief The decoder of 16 bit instruction
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-07-31
*/

#include "c6k_cpu.h"
#include "c6k_decode.h"
#include <stdio.h>
#include <stdlib.h>
//#include "portable/portable.h"
#include "skyeye_types.h"
#include "skyeye_bus.h"
#include "skyeye_uart_ops.h"
#include "bank_defs.h"

#define DEBUG
#include <skyeye_log.h>

#ifdef DBG
#undef DBG
#define DBG
//#define DBG(fmt, ...) do { fprintf(stderr, fmt, ## __VA_ARGS__); } while (0)
#endif

#define NOT_IMP printf("In %s : line %d, not implement at 0x%x\n", __FUNCTION__, __LINE__, core->pc);exit(-1)
/* 16 - 18 should be DSZ */
const ISEITEM insn16_decode_table[] = {
    {"doff4", 2, 6, {1, 2, 0x2, 10, 10, 0x0}, MIXED_LS_SLOT | 3,},
    {"doff4_dw", 3, 6, {1, 2, 0x2, 9, 10, 0x0, 18, 18, 0x1}, MIXED_LS_SLOT | 3,},
    {"dind", 2, 6, {1, 2, 0x2, 10, 11, 0x1}, MIXED_LS_SLOT | 3,},
    {"dind_dw", 3, 6, {1, 2, 0x2, 9, 11, 0x2, 18, 18, 0x1}, MIXED_LS_SLOT | 3,},
    {"dinc", 3, 6, {1, 2, 0x2, 10, 11, 0x3, 14, 15, 0x0}, MIXED_LS_SLOT | 3,},
    {"dinc_dw", 4, 6, {1, 2, 0x2, 9, 11, 0x6, 14, 15, 0x0, 18, 18, 0x1}, MIXED_LS_SLOT | 3,},
    {"ddec", 3, 6, {1, 2, 0x2, 10, 11, 0x3, 14, 15, 0x1}, MIXED_LS_SLOT | 3,},
    {"ddec_dw", 4, 6, {1, 2, 0x2, 9, 11, 0x6, 14, 15, 0x1, 18, 18, 0x1}, MIXED_LS_SLOT | 3,},
    {"dstk", 3, 6, {1, 2, 0x2, 10, 11, 0x3, 15, 15, 0x1}, MIXED_LS_SLOT | 3,},
    {"dx2op", 2, 6, {1, 6, 0x1b, 10, 10, 0x0}, 0,},
    {"dx5", 2, 6, {1, 6, 0x1b, 10, 10, 0x1}, 0,},
    {"dx5p", 2, 6, {1, 6, 0x3b, 10, 12, 0x3}, 0,},
    {"dx1", 2, 6, {1, 6, 0x3b, 10, 12, 0x6}, 0,},
    {"dpp", 2, 6, {0, 6, 0x77, 11, 11, 0x0}, MIXED_LS_SLOT | 14,},
    {"l3", 2, 6, {1, 3, 0x0, 10, 10, 0x0}, 0,},
    {"l3i", 2, 6, {1, 3, 0x0, 10, 10, 0x1}, 0,},
    {"ltbd", 2, 6, {1, 3, 0x4, 10, 10, 0x0}, 0,},       // not used still
    {"l2c", 2, 6, {1, 3, 0x4, 10, 10, 0x1}, 0,},
    {"lx5", 2, 6, {1, 6, 0x13, 10, 10, 0x1}, 0,},
    {"lx3c", 3, 6, {1, 6, 0x13, 10, 10, 0x0, 12, 12, 0x0}, 0,},
    {"lx1c", 3, 6, {1, 6, 0x13, 10, 10, 0x0, 12, 12, 0x1}, 0,},
    {"lx1", 3, 6, {1, 6, 0x33, 10, 12, 0x6}, 0,},
    {"m3", 1, 6, {1, 4, 0xf}, 1 | SGL_SLOT,},
    {"sbu8", 2, 6, {1, 5, 0x5, 14, 15, 0x3, BR_INSN_IN_UNIT_S}, BNOP_SLOT,},
    {"sbs7", 1, 6, {1, 5, 0x5, BR_INSN_IN_UNIT_S}, BNOP_SLOT,},
    {"scs10", 1, 6, {1, 5, 0xd, BR_INSN_IN_UNIT_S}, CALLP_SLOT,},
    {"sbu8c", 2, 6, {1, 3, 0x5, 5, 5, 0x1, 14, 15, 0x3, BR_INSN_IN_UNIT_S}, BNOP_SLOT,},
    {"sbs7c", 2, 6, {1, 3, 0x5, 5, 5, 0x1, BR_INSN_IN_UNIT_S}, BNOP_SLOT,},
    {"s3", 2, 6, {1, 3, 0x5, 10, 10, 0x0, NONBR_INSN_IN_UNIT_S}, 0,},
    {"s3i", 2, 6, {1, 3, 0x5, 10, 10, 0x1, NONBR_INSN_IN_UNIT_S}, 0,},
    {"smvk8", 1, 6, {1, 4, 0x9}, 0,},
    {"ssh5", 2, 6, {1, 4, 0x1, 10, 10, 0x1}, 0,},
    {"s2sh", 2, 6, {1, 6, 0x31, 10, 10, 0x1}, 0,},
    {"sc5", 2, 6, {1, 4, 0x1, 10, 10, 0x0}, 0,},
    {"s2ext", 2, 6, {1, 6, 0x31, 10, 10, 0x0}, 0,},
    {"sx2op", 2, 6, {1, 6, 0x17, 10, 10, 0x0}, 0,},
    {"sx5", 2, 6, {1, 6, 0x17, 10, 10, 0x1}, 0,},
    {"sx1", 2, 6, {1, 6, 0x37, 10, 12, 0x6}, 0,},
    {"sx1b", 2, 6, {1, 6, 0x37, 11, 12, 0x0}, BNOP_SLOT,},
    {"lsd_mvto", 2, 6, {1, 2, 0x3, 5, 6, 0x0}, 0,},
    {"lsd_mvfr", 2, 6, {1, 2, 0x3, 5, 6, 0x2}, 0,},
    {"lsd_x1c", 3, 6, {1, 2, 0x3, 5, 6, 0x3, 10, 12, 0x2}, 0,},
    {"lsd_x1", 3, 6, {1, 2, 0x3, 5, 6, 0x3, 10, 12, 0x6}, 0,},
    {"uspl", 3, 6, {1, 6, 0x33, 10, 13, 0x3, 15, 15, 0x0}, 0,},
    {"uspldr", 3, 6, {1, 6, 0x33, 10, 13, 0x3, 15, 15, 0x1}, 0,},
    {"uspk", 2, 6, {1, 6, 0x33, 10, 13, 0x7}, 0,},
    {"uspm", 2, 6, {1, 6, 0x33, 10, 13, 0xb}, 0,},
    {"uspm", 2, 6, {1, 6, 0x33, 10, 13, 0xf}, 0,},
    {"unop", 1, 6, {0, 12, 0xc6e}, NOP_SLOT,},
};

static int exec_lsd_x1(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields);
static int exec_s2sh(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields);

int get_func_unit(c6k_core_t * core, uint32_t insn);

int get_func_unit(c6k_core_t * core, uint32_t insn)
{
    if (((insn >> 1) & 0x3) == 0x3 && ((insn >> 5) & 0x3) == 0x2)
    {
        int unit = (insn >> 3) & 0x3;
        int s = BITS(0, 0);

        if (unit == 0)
        {
            return 1 << (L1_UNIT + s);
        } else if (unit == 1)
            return 1 << (S1_UNIT + s);
        else if (unit == 2)
            return 1 << (D1_UNIT + s);
        else
        {
            NOT_IMP;
        }
    } else
    {
        NOT_IMP;
    }
    return 0;
}

enum disp_mode
{
    ACCESS_ONLY,
    POST_INC,
    PRE_DEC,
};

struct ldst
{
    int ptr;
    int ptr_side;
    int src_dst;
    int src_dst_side;
    int offset;
    int dszln;
    enum disp_mode mode;
    opcode_fields_t *fields;
};

static inline generic_address_t get_target_addr(c6k_core_t * core,
                                                generic_address_t addr, enum disp_mode mode, int offset, int reg_no)
{
    generic_address_t addr_ret = addr;
    uint32_t reg_result;

    switch (mode)
    {
        case ACCESS_ONLY:
            addr_ret += offset;
            break;
        case POST_INC:
            reg_result = addr + offset;
            WRITE_BUFFER(core, reg_no, reg_result);
            break;
        case PRE_DEC:
            addr -= offset;
            reg_result = addr;
            WRITE_BUFFER(core, reg_no, reg_result);
            break;
    }
    return addr_ret;
}

static inline void exec_ldsts(c6k_core_t * core, struct ldst *ldstp)
{
    int szl = (ldstp->dszln >> 1) & 0x3;
    int dsz_szl = ldstp->dszln >> 1;
    int base = ldstp->ptr | 0x4;
    int s = ldstp->ptr_side;
    int t = ldstp->src_dst_side;
    opcode_fields_t *fields = ldstp->fields;
    int use_hi_regs = fields->compact_rs;
    int src = ldstp->src_dst + (use_hi_regs ? 16 : 0);
    int32_t offset = ldstp->offset;
    generic_address_t addr = core->gpr[s][base];
    uint64_t mem_result = 0;
    int32_t reg_no = t * GPR_NUM_IN_GROUP + src;
    int32_t sreg_no = s * GPR_NUM_IN_GROUP + base;

    if (szl == 0)
    {                                   //stw
        if (dsz_szl & 0x10)
        {
            int na = ldstp->dszln & 0x1;
            int szln = (szl << 1) | na;

            src &= use_hi_regs ? 0x16 : 0x6;    // src[5..6]
            reg_no = t * GPR_NUM_IN_GROUP + src;

            switch (szln)
            {
                case 0x0:              //stdw
                    addr = get_target_addr(core, addr, ldstp->mode, offset << 3, sreg_no);
                    mem_result = core->gpr[t][src] | ((uint64_t) core->gpr[t][src + 1] << 32);
                    lds_save_result(core, fields, mem_result, reg_no, addr, WR_FLAG, 0, 64);
                    break;
                case 0x1:              //stndw
                    addr = get_target_addr(core, addr, ldstp->mode, offset, sreg_no);
                    mem_result = core->gpr[t][src] | ((uint64_t) core->gpr[t][src + 1] << 32);
                    lds_save_result(core, fields, mem_result, reg_no, addr, WR_FLAG, 0, 64);
                    break;
            }
        } else
        {                               //stw
            addr = get_target_addr(core, addr, ldstp->mode, offset << 2, sreg_no);
            mem_result = core->gpr[t][src];
            lds_save_result(core, fields, mem_result, reg_no, addr, WR_FLAG, 0, 32);
        }
    } else if (szl == 1)
    {
        if (dsz_szl & 0x10)
        {
            int na = ldstp->dszln & 0x1;
            int szln = (szl << 1) | na;

            src &= use_hi_regs ? 0x16 : 0x6;    // src[5..6]
            reg_no = t * GPR_NUM_IN_GROUP + src;

            switch (szln)
            {
                case 0x2:              //lddw
                    addr = get_target_addr(core, addr, ldstp->mode, offset << 3, sreg_no);
                    lds_save_result(core, fields, mem_result, reg_no, addr, RD_FLAG, 0, 64);
                    break;
                case 0x3:              //ldndw
                    addr = get_target_addr(core, addr, ldstp->mode, offset, sreg_no);
                    lds_save_result(core, fields, mem_result, reg_no, addr, RD_FLAG, 0, 64);
                    break;
            }
        } else
        {                               //ldw
            addr = get_target_addr(core, addr, ldstp->mode, offset << 2, sreg_no);
            lds_save_result(core, fields, mem_result, reg_no, addr, RD_FLAG, 0, 32);
        }
    } else
    {
        switch (dsz_szl)
        {
            case 0x2:                  //stb
            case 0x6:                  //stb, FIXME, what's the differences between them
            case 0x16:                 //stb
                addr = get_target_addr(core, addr, ldstp->mode, offset, sreg_no);
                mem_result = core->gpr[t][src];
                lds_save_result(core, fields, mem_result, reg_no, addr, WR_FLAG, 0, 8);
                break;
            case 0x3:                  //ldbu
                addr = get_target_addr(core, addr, ldstp->mode, offset, sreg_no);
                lds_save_result(core, fields, mem_result, reg_no, addr, RD_FLAG, 0, 8);
                break;
            case 0x7:                  //ldb
            case 0x17:                 //ldb
                addr = get_target_addr(core, addr, ldstp->mode, offset, sreg_no);
                lds_save_result(core, fields, mem_result, reg_no, addr, RD_FLAG, 1, 8);
                break;
            case 0xa:                  //sth
            case 0xe:                  //sth
            case 0x1e:                 //sth
                addr = get_target_addr(core, addr, ldstp->mode, offset << 1, sreg_no);
                mem_result = core->gpr[t][src];
                lds_save_result(core, fields, mem_result, reg_no, addr, WR_FLAG, 0, 16);
                break;
            case 0xb:                  //ldhu
                addr = get_target_addr(core, addr, ldstp->mode, offset << 1, sreg_no);
                lds_save_result(core, fields, mem_result, reg_no, addr, RD_FLAG, 0, 16);
                break;
            case 0xf:                  //ldh
            case 0x1f:                 //ldh
                addr = get_target_addr(core, addr, ldstp->mode, offset << 1, sreg_no);
                lds_save_result(core, fields, mem_result, reg_no, addr, RD_FLAG, 1, 16);
                break;
            case 0x12:                 //stw
            case 0x1a:                 //stnw
                addr = get_target_addr(core, addr, ldstp->mode, offset << 2, sreg_no);
                mem_result = core->gpr[t][src];
                lds_save_result(core, fields, mem_result, reg_no, addr, WR_FLAG, 0, 32);
                break;
            case 0x13:                 //ldw
            case 0x1b:                 //ldnw
                addr = get_target_addr(core, addr, ldstp->mode, offset << 2, sreg_no);
                lds_save_result(core, fields, mem_result, reg_no, addr, RD_FLAG, 0, 32);
                break;
        }
    }
}

static int exec_doff4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_D1 << (BITS(0, 0));
    int ld_st = BITS(3, 3);
    int sz = BITS(9, 9);
    int szl = (sz << 1) + ld_st;
    int dsz_szl = (fields->compact_dsz << 2) | szl;
    uint32_t ucst3 = BITS(11, 11);
    uint32_t ucst2_0 = BITS(13, 15);
    uint32_t ucst4 = ucst3 << 3 | ucst2_0;

    struct ldst ldst;

    ldst.fields = fields;
    ldst.mode = ACCESS_ONLY;
    ldst.src_dst = BITS(4, 6);
    ldst.src_dst_side = BITS(12, 12);
    ldst.ptr = BITS(7, 8);
    ldst.ptr_side = BITS(0, 0);
    ldst.offset = ucst4;
    ldst.dszln = (dsz_szl << 1) | BITS(4, 4);

    exec_ldsts(core, &ldst);

    core->pc += 2;
    return 0;
}

static int exec_doff4_dw(c6k_core_t * core, uint32_t insn, opcode_fields_t * unused)
{
    NOT_IMP;
    return 0;
}

static int exec_dind(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_D1 << (BITS(0, 0));
    int ld_st = BITS(3, 3);
    int sz = BITS(9, 9);
    int src1 = BITS(13, 15);
    int szl = (sz << 1) + ld_st;
    int dsz_szl = (fields->compact_dsz << 2) | szl;

    struct ldst ldst;

    ldst.fields = fields;
    ldst.mode = ACCESS_ONLY;
    ldst.src_dst = BITS(4, 6);
    ldst.src_dst_side = BITS(12, 12);
    ldst.ptr = BITS(7, 8);
    ldst.ptr_side = BITS(0, 0);
    ldst.offset = core->gpr[ldst.ptr_side][src1];
    ldst.dszln = (dsz_szl << 1) | BITS(4, 4);

    exec_ldsts(core, &ldst);

    core->pc += 2;
    return 0;
}

static int exec_dind_dw(c6k_core_t * core, uint32_t insn, opcode_fields_t * unused)
{
    NOT_IMP;
    return 0;
}

static int exec_dinc(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_D1 << (BITS(0, 0));
    int ld_st = BITS(3, 3);
    int sz = BITS(9, 9);
    int ucst2 = BITS(13, 13) + 1;
    int szl = (sz << 1) + ld_st;
    int dsz_szl = (fields->compact_dsz << 2) | szl;

    struct ldst ldst;

    ldst.fields = fields;
    ldst.mode = POST_INC;
    ldst.src_dst = BITS(4, 6);
    ldst.src_dst_side = BITS(12, 12);
    ldst.ptr = BITS(7, 8);
    ldst.ptr_side = BITS(0, 0);
    ldst.offset = ucst2;
    ldst.dszln = (dsz_szl << 1) | BITS(4, 4);

    exec_ldsts(core, &ldst);

    core->pc += 2;
    return 0;
}

static int exec_dinc_dw(c6k_core_t * core, uint32_t insn, opcode_fields_t * unused)
{
    NOT_IMP;
    return 0;
}

static int exec_ddec(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_D1 << (BITS(0, 0));
    int ld_st = BITS(3, 3);
    int sz = BITS(9, 9);
    int ucst2 = BITS(13, 13) + 1;
    int szl = (sz << 1) + ld_st;
    int dsz_szl = (fields->compact_dsz << 2) | szl;

    struct ldst ldst;

    ldst.fields = fields;
    ldst.mode = PRE_DEC;
    ldst.src_dst = BITS(4, 6);
    ldst.src_dst_side = BITS(12, 12);
    ldst.ptr = BITS(7, 8);
    ldst.ptr_side = BITS(0, 0);
    ldst.offset = ucst2;
    ldst.dszln = (dsz_szl << 1) | BITS(4, 4);

    exec_ldsts(core, &ldst);

    core->pc += 2;
    return 0;
}

static int exec_ddec_dw(c6k_core_t * core, uint32_t insn, opcode_fields_t * unused)
{
    NOT_IMP;
    return 0;
}

static int exec_dstk(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_D1 << (BITS(0, 0));
    int base = fields->compact_rs ? 16 : 0;
    int ld_st = BITS(3, 3);
    int t = BITS(12, 12);
    int src = BITS(4, 6) + base;
    int ucst5 = BITS(13, 14) | (BITS(7, 9) << 2);

    if (ld_st)
    {
        /* ldw */
        generic_address_t addr = core->gpr[GPR_B][15] + (ucst5 << 2);
        uint32_t result = 0;

        lds_save_result(core, fields, result, t * GPR_NUM_IN_GROUP + src, addr, RD_FLAG, 0, 32);
        DBG("In %s, LDW@0x%x, addr=0x%x, src=%d, ucst5\n", __FUNCTION__, core->pc, addr, src, ucst5);
        DBG("In %s, LDW@0x%x, addr=0x%x, src=%d, ucst5\n", __FUNCTION__, core->pc, addr, src, ucst5);

    } else
    {
        /* stw */
        generic_address_t addr = core->gpr[GPR_B][15] + (ucst5 << 2);

        lds_save_result(core, fields, core->gpr[t][src], t * GPR_NUM_IN_GROUP + src, addr, WR_FLAG, 0, 32);
        DBG("In %s, STW@0x%x, addr=0x%x, src=%d, ucst5\n", __FUNCTION__, core->pc, addr, src, ucst5);
    }
    core->pc += 2;
    return 0;
}
static int exec_dx2op(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_D1 << (BITS(0, 0));
    int s = BITS(0, 0);
    int op = BITS(11, 11);
    int base = fields->compact_rs ? 16 : 0;
    int reg_group = BITS(12, 12) ? (!s) & 0x1 : s;
    int dst = BITS(13, 15) + base;
    int32_t result = (int32_t) core->gpr[s][dst];
    int32_t src2 = (int32_t) core->gpr[reg_group][BITS(7, 9) + base];

    if (op)
    {                                   //sub
        result -= src2;
    } else
    {                                   //add
        result += src2;
    }
    WRITE_BUFFER(core, s * GPR_NUM_IN_GROUP + dst, result);

    core->pc += 2;
    return 0;
}
static int exec_dx5(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_D1 << (BITS(0, 0));
    int ucst2_0 = BITS(13, 15);
    int ucst3_4 = BITS(11, 12);
    int ucst5 = (ucst3_4 << 3) | ucst2_0;
    int dst = BITS(7, 9);
    int s = BITS(0, 0);

    WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, core->gpr[GPR_B][15] + (ucst5 << 2));
    //core->gpr[s][dst] = core->gpr[GPR_B][15] + (ucst5 << 2);
    core->pc += 2;
    return 0;
}

static int exec_dx5p(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_D1 << (BITS(0, 0));
    int op = BITS(7, 7);
    int ucst2_0 = BITS(13, 15);
    int ucst3_4 = BITS(8, 9);
    int ucst5 = (ucst3_4 << 3) | ucst2_0;
    int32_t dst = core->gpr[1][15];

    if (op)
    {                                   //subaw
        dst -= ucst5 << 2;
    } else
    {                                   //addaw
        dst += ucst5 << 2;
    }
    WRITE_BUFFER(core, 15 + 1 * GPR_NUM_IN_GROUP, dst);
    core->pc += 2;
    return 0;
}

static int exec_dx1(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_D1 << (BITS(0, 0));
    int op = BITS(13, 15);
    int src_dst = BITS(7, 9);
    int s = BITS(0, 0);
    int result = 0;

    if (op == 0x0)
    {
        //core->gpr[s][src_dst] = 0;
        WRITE_BUFFER(core, src_dst + s * GPR_NUM_IN_GROUP, 0);
    } else if (op == 0x1)
    {
        //core->gpr[s][src_dst] = 0;
        WRITE_BUFFER(core, src_dst + s * GPR_NUM_IN_GROUP, 1);
    } else if (op == 0x3)
    {
        result = core->gpr[s][src_dst] - 1;
        WRITE_BUFFER(core, src_dst + s * GPR_NUM_IN_GROUP, result);
    } else if (op == 0x7)
    {
        result = core->gpr[s][src_dst] ^ 1;
        WRITE_BUFFER(core, src_dst + s * GPR_NUM_IN_GROUP, result);
    } else
    {
        NOT_IMP;
    }
    core->pc += 2;
    return 0;
}
static int exec_dpp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_D1 << (BITS(0, 0));
    int dw = BITS(15, 15);
    int ld_st = BITS(14, 14);
    int t = BITS(12, 12);
    int src_dst = BITS(7, 10);
    unsigned int ucst0 = BITS(13, 13);
    generic_address_t addr;

    if (ld_st)
    {                                   /* load */
        if (dw)
        {
            /* lddw ++B15 */
            core->gpr[GPR_B][15] = core->gpr[GPR_B][15] + ((ucst0 + 1) << 3);
            addr = core->gpr[GPR_B][15];
            uint64_t result = 0;

            lds_save_result(core, fields, result, t * GPR_NUM_IN_GROUP + src_dst, addr, RD_FLAG, 0, 64);
            DBG("In %s, lddw, addr=0x%x\n", __FUNCTION__, addr);

        } else
        {
            /* ldw ++b15 */
            core->gpr[GPR_B][15] = core->gpr[GPR_B][15] + ((ucst0 + 1) << 2);
            addr = core->gpr[GPR_B][15];
            uint64_t result = 0;

            lds_save_result(core, fields, result, src_dst + t * GPR_NUM_IN_GROUP, addr, RD_FLAG, 0, 32);
        }
    } else
    {
        /* store */
        if (dw)
        {
            /* double word */
            //NOT_IMP;
            /* stdw *B15--[ucst2] */
            addr = core->gpr[GPR_B][15];
            core->gpr[GPR_B][15] = core->gpr[GPR_B][15] - ((ucst0 + 1) << 3);
            uint64_t result = core->gpr[t][src_dst] | ((uint64_t) core->gpr[t][src_dst + 1] << 32);

            lds_save_result(core, fields, result, t * GPR_NUM_IN_GROUP + src_dst, addr, WR_FLAG, 0, 64);
        } else
        {
            //if(core->pc == 0x810ac0){
            if (0)
            {
                DBG("In %s, the instruction is inhibited at 0x%x.\n", __FUNCTION__, core->pc);
            } else
            {
                /* stw *B15--[ucst2] */
                addr = core->gpr[GPR_B][15];
                core->gpr[GPR_B][15] = core->gpr[GPR_B][15] - ((ucst0 + 1) << 2);
                lds_save_result(core, fields, core->gpr[t][src_dst], t * GPR_NUM_IN_GROUP + src_dst, addr, WR_FLAG, 0,
                                32);
                DBG("In %s, addr=0x%x, src_dst=%d, ucst0=%d\n", __FUNCTION__, addr, src_dst, ucst0);
            }
        }
    }
    DBG("In %s, src_dst=%d\n", __FUNCTION__, src_dst);
    //sleep(5);
    core->pc += 2;
    return 0;
}
static int exec_l3(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op = BITS(11, 11);
    int src1 = BITS(13, 15);
    int src2 = BITS(7, 9);
    int x = BITS(12, 12);
    int dst = BITS(4, 6);
    int s = BITS(0, 0);

    fields->exec_unit = UNIT_L1 << s;
    int sat = (core->header >> 14) & 0x1;
    int32_t result;

    if ((op == 1) && (sat == 0))
    {
        /* SUB */
        if (x)
            result = core->gpr[s][src1] - core->gpr[(!s) & 0x1][src2];
        else
            result = core->gpr[s][src1] - core->gpr[s][src2];
        WRITE_BUFFER(core, s * GPR_NUM_IN_GROUP + dst, result);
        DBG("In %s, src1=%d, src2=%d, dst=%d\n", __FUNCTION__, src1, src2, dst);
    } else if ((op == 0) && (sat == 0))
    {
        /* ADD */
        if (x)
            result = core->gpr[s][src1] + core->gpr[(!s) & 0x1][src2];
        else
            result = core->gpr[s][src1] + core->gpr[s][src2];
        WRITE_BUFFER(core, s * GPR_NUM_IN_GROUP + dst, result);
        DBG("In %s, src1=%d, src2=%d, dst=%d\n", __FUNCTION__, src1, src2, dst);
    } else
    {
        NOT_IMP;
    }
    core->pc += 2;
    return 0;
}

static int exec_l3i(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int base = fields->compact_rs ? 16 : 0;
    int sn = BITS(11, 11);
    int cst3 = BITS(13, 15);
    int dst = BITS(4, 6) + base;
    int src2 = BITS(7, 9) + base;
    int x = BITS(12, 12);
    int s = BITS(0, 0);

    fields->exec_unit = UNIT_L1 << s;
    int scst5;
    int32_t result;

    if (sn)
    {
        scst5 = cst3 | 0x18;
        scst5 = SIGN_EXTEND(scst5, 5);
    } else
    {
        if (cst3 == 0)
            scst5 = 8;
        else
            scst5 = cst3;
    }
    if (x)
        result = core->gpr[(!s) & 0x1][src2] + scst5;
    else
        result = core->gpr[s][src2] + scst5;
    WRITE_BUFFER(core, s * GPR_NUM_IN_GROUP + dst, result);
    DBG("In %s, scst5=%d\n", __FUNCTION__, scst5);
    core->pc += 2;
    return 0;
}

static int exec_ltbd(c6k_core_t * core, uint32_t insn, opcode_fields_t * unused)
{
    NOT_IMP;
    return 0;
}
static int exec_l2c(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op = BITS(5, 6) | (BITS(11, 11) << 2);
    int src1 = BITS(13, 15);
    int src2 = BITS(7, 9);
    int dst = BITS(4, 4);
    int s = BITS(0, 0);

    fields->exec_unit = UNIT_L1 << s;
    int x = BITS(12, 12);
    uint32_t result;
    uint32_t dst_regno = dst + s * GPR_NUM_IN_GROUP;

    if (op == 3)
    {
        if (x)
            result = (core->gpr[s][src1] == core->gpr[(!s) & 0x1][src2]);
        else
            result = (core->gpr[s][src1] == core->gpr[s][src2]);
        WRITE_BUFFER(core, dst_regno, result);
    } else if (op == 1)
    {
        if (x)
            result = (core->gpr[s][src1] | core->gpr[(!s) & 0x1][src2]);
        else
            result = (core->gpr[s][src1] | core->gpr[s][src2]);
        WRITE_BUFFER(core, dst_regno, result);

    } else if (op == 0x0)
    {
        if (x)
            result = (core->gpr[s][src1] & core->gpr[(!s) & 0x1][src2]);
        else
            result = (core->gpr[s][src1] & core->gpr[s][src2]);
        WRITE_BUFFER(core, dst_regno, result);
        DBG("In %s, and, src1=%d, src2=%d, dst=%d\n", __FUNCTION__, src1, src2, dst);
    } else if (op == 0x5)
    {
        /* CMPGT , FIXME, sign comparison */
        if (x)
            result = ((int) core->gpr[s][src1] > (int) core->gpr[(!s) & 0x1][src2]);
        else
            result = ((int) core->gpr[s][src1] > (int) core->gpr[s][src2]);
        WRITE_BUFFER(core, dst_regno, result);
    } else if (op == 0x7)
    {
        /* CMPGTU */
        if (x)
            result = (core->gpr[s][src1] > core->gpr[(!s) & 0x1][src2]);
        else
            result = (core->gpr[s][src1] > core->gpr[s][src2]);
        WRITE_BUFFER(core, dst_regno, result);
    } else if (op == 0x6)
    {
        /* CMPLTU */
        if (x)
            result = (core->gpr[s][src1] < core->gpr[(!s) & 0x1][src2]);
        else
            result = (core->gpr[s][src1] < core->gpr[s][src2]);
        WRITE_BUFFER(core, dst_regno, result);
    } else if (op == 0x4)
    {
        if (x)
            result = ((int) core->gpr[s][src1] < (int) core->gpr[(!s) & 0x1][src2]);
        else
            result = ((int) core->gpr[s][src1] < (int) core->gpr[s][src2]);
        WRITE_BUFFER(core, dst_regno, result);
    } else if (op == 0x2)
    {
        if (x)
            result = core->gpr[s][src1] ^ core->gpr[(!s) & 0x1][src2];
        else
            result = core->gpr[s][src1] ^ core->gpr[s][src2];
        WRITE_BUFFER(core, dst_regno, result);
    } else
    {
        NOT_IMP;
    }
    core->pc += 2;
    return 0;
}

static int exec_lx5(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int dst = BITS(7, 9) + (fields->compact_rs ? 16 : 0);
    int scst5 = BITS(13, 15) | (BITS(11, 12) << 3);
    int s = BITS(0, 0);

    fields->exec_unit = UNIT_L1 << s;
    int result = SIGN_EXTEND(scst5, 5);

    WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    DBG("In %s, dst=%d, scst5=%d\n", __FUNCTION__, dst, SIGN_EXTEND(scst5, 5));
    core->pc += 2;
    return 0;
}

static int exec_lx3c(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int ucst3 = BITS(13, 15);
    int dst = BITS(11, 11);
    int src2 = BITS(7, 9);
    int s = BITS(0, 0);

    fields->exec_unit = UNIT_L1 << s;
    int32_t result = (core->gpr[s][src2] == ucst3);

    WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    DBG("In %s, dst=%d, src2=%d\n", __FUNCTION__, dst, src2);
    core->pc += 2;
    return 0;
}

static int exec_lx1c(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op = BITS(14, 15);
    int ucst1 = BITS(13, 13);
    int dst = BITS(11, 11);
    int src2 = BITS(7, 9);
    int s = BITS(0, 0);
    uint32_t result = 0;

    fields->exec_unit = UNIT_L1 << s;
    if (op == 1)
    {                                   //cmpgt
        result = (int32_t) ucst1 > (int32_t) core->gpr[s][src2];
    } else if (op == 0x0)
    {                                   //cmplt
        result = (int32_t) ucst1 < (int32_t) core->gpr[s][src2];
    } else if (op == 0x2)
    {                                   //cmpltu
        result = (uint32_t) ucst1 < core->gpr[s][src2];
    } else if (op == 0x3)
    {                                   //cmpgtu
        result = (uint32_t) ucst1 > core->gpr[s][src2];
    }
    WRITE_BUFFER(core, s * GPR_NUM_IN_GROUP + dst, result);
    core->pc += 2;
    return 0;
}

static int exec_lx1(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op = BITS(13, 15);
    int src_dst = BITS(7, 9);
    int s = BITS(0, 0);

    fields->exec_unit = UNIT_L1 << s;
    if (op == 0x7)
    {
        core->gpr[s][src_dst] = 1 ^ core->gpr[s][src_dst];
    } else if (op == 0x2)
    {
        core->gpr[s][src_dst] = -core->gpr[s][src_dst];
    } else
    {
        NOT_IMP;
    }
    core->pc += 2;
    return 0;
}
static int exec_m3(c6k_core_t * core, uint32_t insn, opcode_fields_t * unused)
{
    NOT_IMP;
    return 0;
}
static int exec_sbu8(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    /* bnop ucst8, 5 */
    fields->exec_unit = UNIT_S1 << BITS(0, 0);
    int uscst8 = BITS(6, 13);

    core->pfc = core->pce1 + (uscst8 << 1);
    REGISTER_BRANCH_EVENT(core, core->pfc);
    ADD_PENDING_NOPS(core, 5);
    DBG("In %s, bnop, pfc = 0x%x, pc = 0x%x\n", __FUNCTION__, core->pfc, core->pc);
    core->pc += 2;
    return 0;
}
static int exec_sbs7(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    /* bnop scst7, N3 */
    fields->exec_unit = UNIT_S1 << BITS(0, 0);
    int n3 = BITS(13, 15);
    int scst7 = BITS(6, 12);
    int offset = (scst7 << 1);

    if ((offset >> 7) & 0x1)
    {
        core->pfc = core->pce1 - (0x100 - offset);
    } else
    {
        core->pfc = core->pce1 + offset;
    }

    REGISTER_BRANCH_EVENT(core, core->pfc);
    ADD_PENDING_NOPS(core, n3);
    DBG("In %s, bnop, pfc = 0x%x, pc = 0x%x, n3=%d, \n", __FUNCTION__, core->pfc, core->pc, n3);
    core->pc += 2;
    return 0;
}

static int exec_scs10(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    /* callp */
    int scst10 = BITS(6, 15);
    int s = BITS(0, 0);

    fields->exec_unit = UNIT_S1 << BITS(0, 0);
    int br = (core->header >> 15) & 0x1;

    if (br)
    {
        /* not callp */
        if (scst10 & 0x200)
        {
            scst10 |= 0xFFFFFC00;
        }
        scst10 = scst10 << 2;
        generic_address_t orig_pc = core->pc;
        generic_address_t addr = core->pc;

        core->pfc = (signed int) core->pce1 + scst10;
        REGISTER_BRANCH_EVENT(core, core->pfc);
        uint32_t layout = (core->header >> 21) & 0x7f;
        uint32_t pbits = core->header & 0x3FFF;
        int i = (addr - core->pce1) / 2;

        addr += 2;
        int pbit = (pbits >> i) & 0x1;

        i++;
        int word;

        while (pbit)
        {
            core_read(core, 32, addr, &word);
            if (((word & 0xe0000000) == 0xe0000000) && ((addr & 0x1f) == 0x1c))
            {
                /* skip the header */
                addr += 4;
                continue;
            }
            if ((addr & 0x1f) == 0)
            {
                uint32_t header;

                core_read(core, 32, addr + 0x1c, &header);
                if ((header >> 28) == 0xe)
                {
                    layout = (header >> 21) & 0x7f;
                    pbits = header & 0x3FFF;
                } else
                    layout = 0;
                i = 0;
            }
            if ((layout >> (i / 2)) & 0x1)
            {
                pbit = (pbits >> i) & 0x1;
                addr += 2;
                if (pbit == 0)
                {
                    break;
                }
                i++;
            } else
            {
                addr += 4;
                pbit = word & 0x1;
                if (pbit == 0)
                {
                    break;
                }
                i += 2;
            }
        }
        // jump over the header
        if ((addr & 0x1c) == 0x1c)
            addr += 4;

        WRITE_BUFFER(core, 3 + s * GPR_NUM_IN_GROUP, addr);
        ADD_PENDING_NOPS(core, 5);
        DBG("In %s, return addr=0x%x, pc=0x%x, i=%d\n", __FUNCTION__, addr, orig_pc, i);
    } else if ((BITS(10, 10) == 1) && (BITS(11, 11) == 0x0))
    {                                   /* s3i */
        /* SHL */
        int cst3 = BITS(13, 15);
        int src2 = BITS(7, 9);
        int dst = BITS(4, 6);
        int s = BITS(0, 0);
        int x = BITS(12, 12);
        int cst5;

        if (cst3 == 0)
            cst5 = 16;
        else if (cst3 == 0x7)
            cst5 = 0x8;
        else
            cst5 = cst3;
        int result;

        if (x)
        {
            result = core->gpr[(!s) & 0x1][src2] << cst5;
        } else
            result = core->gpr[s][src2] << cst5;
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        DBG("In %s, shl\n", __FUNCTION__);
    } else
    {
        NOT_IMP;
    }
    core->pc += 2;

    return 0;
}
static int exec_sbu8c(c6k_core_t * core, uint32_t insn, opcode_fields_t * unused)
{
    NOT_IMP;
    return 0;
}
static int exec_sbs7c(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int s = BITS(0, 0);

    fields->exec_unit = UNIT_S1 << BITS(0, 0);
    int z = BITS(4, 4);
    int sz = (s << 1) | z;
    int n3 = BITS(13, 15);
    int scst7 = BITS(6, 12);
    int A0 = core->gpr[GPR_A][0];
    int B0 = core->gpr[GPR_B][0];
    int br = (core->header >> 15) & 0x1;

    if (br)
    {
        int offset;

        if ((n3 & 0x6) == 0x6)
        {
            offset = BITS(6, 13);
        } else
        {
            /* sign extend */
            offset = (scst7 & 0x40) ? (scst7 | 0xFFFFFF80) : scst7;
        }
        switch (sz)
        {
            case 0:
                if (A0)
                {
                    /* bnop */
                    core->pfc = core->pce1 + (offset << 1);
                    REGISTER_BRANCH_EVENT(core, core->pfc);
                    DBG("In %s, bnop, pfc = 0x%x, pc = 0x%x, n3=%d\n", __FUNCTION__, core->pfc, core->pc, n3);
                }
                break;
            case 1:
                if (!A0)
                {
                    /* bnop */
                    core->pfc = core->pce1 + (offset << 1);
                    REGISTER_BRANCH_EVENT(core, core->pfc);
                    DBG("In %s, bnop, pfc = 0x%x, pc = 0x%x, n3=%d\n", __FUNCTION__, core->pfc, core->pc, n3);

                }
                break;
            case 2:
                if (B0)
                {
                    /* bnop */
                    core->pfc = core->pce1 + (offset << 1);
                    REGISTER_BRANCH_EVENT(core, core->pfc);
                    DBG("In %s, bnop, pfc = 0x%x, pc = 0x%x, n3=%d\n", __FUNCTION__, core->pfc, core->pc, n3);
                }
                break;
            case 3:
                if (!B0)
                {
                    /* bnop */
                    core->pfc = core->pce1 + (offset << 1);
                    REGISTER_BRANCH_EVENT(core, core->pfc);

                }
                break;

            default:
                NOT_IMP;
        }
        ADD_PENDING_NOPS(core, n3);
    } else if ((BITS(10, 10) == 1) && (BITS(11, 11) == 0x0))
    {                                   /* s3i */
        /* SHL */
        int cst3 = BITS(13, 15);
        int src2 = BITS(7, 9);
        int dst = BITS(4, 6);
        int s = BITS(0, 0);
        int x = BITS(12, 12);
        int cst5;

        if (cst3 == 0)
            cst5 = 16;
        else if (cst3 == 0x7)
            cst5 = 0x8;
        else
            cst5 = cst3;
        int result;

        if (x)
        {
            result = core->gpr[(!s) & 0x1][src2] << cst5;
        } else
            result = core->gpr[s][src2] << cst5;

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    } else
    {
        NOT_IMP;
    }
    core->pc += 2;
    return 0;
}

static int exec_s3(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int s = BITS(0, 0);

    fields->exec_unit = UNIT_S1 << BITS(0, 0);
    int base = fields->compact_rs ? 16 : 0;
    int dst = BITS(4, 6) + base;
    int src2 = BITS(7, 9) + base;
    int op = (fields->compact_sat << 1) | BITS(11, 11);
    int reg_group = BITS(12, 12) ? (!s) & 0x1 : s;
    int src1 = BITS(13, 15) + base;
    int32_t result = 0;

    if (op == 0x0)
    {                                   //add
        result = (int32_t) ((int64_t) core->gpr[s][src1] + (int64_t) core->gpr[reg_group][src2]);
    } else if (op == 0x2)
    {                                   //sadd
        int32_t mark = 0x7fffffff;
        int64_t sadd = (int64_t) core->gpr[s][src1] + (int64_t) core->gpr[reg_group][src2];

        if (sadd > mark)
            result = mark;
        else if (sadd < -mark - 1)
            result = -mark - 1;
        else
            result = (int32_t) sadd;

        /* p366: The result is placed in dst. If a saturate occurs,
         * the SAT bit in the control status register
         * (CSR) is set one cycle after dst is written.
         */
    } else if (op & 0x1)
    {                                   //sub
        result = (int32_t) ((int64_t) core->gpr[s][src1] - (int64_t) core->gpr[reg_group][src2]);
    }

    WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);

    core->pc += 2;
    return 0;
}

static int exec_s3i(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int s = BITS(0, 0);

    fields->exec_unit = UNIT_S1 << BITS(0, 0);
    int dst = BITS(4, 6);
    int src2 = BITS(7, 9);
    int op = BITS(11, 11);
    int reg_group = BITS(12, 12) ? (!s) & 0x1 : s;
    int cst3 = BITS(13, 15);
    const int ucst5[8] = { 16, 1, 2, 3, 4, 5, 6, 8 };
    int32_t result = (int32_t) core->gpr[reg_group][src2];

    if (op)
    {                                   //shr
        result >>= ucst5[cst3];
    } else
    {                                   //shl
        result <<= ucst5[cst3];
    }
    WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);

    core->pc += 2;
    return 0;
}

static int exec_smvk8(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int base = fields->compact_rs ? 16 : 0;
    int dst = BITS(7, 9) + base;
    int ucst5_6 = BITS(5, 6);
    int ucst7 = BITS(10, 10);
    int ucst3_4 = BITS(11, 12);
    int ucst0_2 = BITS(13, 15);
    int ucst8 = ucst0_2 | (ucst3_4 << 3) | (ucst5_6 << 5) | (ucst7 << 7);
    int s = BITS(0, 0);

    fields->exec_unit = UNIT_S1 << BITS(0, 0);
    WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, ucst8);
    DBG("In %s, ucst8=0x%x, dst=%d\n", __FUNCTION__, ucst8, dst);
    core->pc += 2;
    return 0;
}

static int exec_ssh5(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op = BITS(5, 6);
    int sat = (core->header >> 14) & 0x1;
    int ucst0_2 = BITS(13, 15);
    int ucst3_4 = BITS(11, 12);
    int ucst5 = ucst0_2 | (ucst3_4 << 3);
    int src_dst = BITS(7, 9);
    int s = BITS(0, 0);

    fields->exec_unit = UNIT_S1 << BITS(0, 0);
    int32_t result;

    if (op == 0x0)
    {                                   //shl
        result = core->gpr[s][src_dst];
        result <<= ucst5;
        WRITE_BUFFER(core, src_dst + s * GPR_NUM_IN_GROUP, result);
    } else if (op == 0x1)
    {                                   //shr
        result = core->gpr[s][src_dst];
        result >>= ucst5;
        WRITE_BUFFER(core, src_dst + s * GPR_NUM_IN_GROUP, result);
    } else if (op == 0x2 && sat == 0x0)
    {                                   //shru
        WRITE_BUFFER(core, src_dst + s * GPR_NUM_IN_GROUP, core->gpr[s][src_dst] >> ucst5);
    } else if (op == 0x2 && sat == 0x1)
    {                                   //sshl
        uint32_t mask = 0xFFFFFFFF >> ucst5;
        uint32_t tst = (core->gpr[s][src_dst] & mask) >> (31 - ucst5);

        result = core->gpr[s][src_dst];
        if (tst == 0x0 || tst == (1 << ucst5 - 1))
        {
            result <<= ucst5;
            // FIXME, ssturate CSR, SSR
        } else if (result > 0)
        {
            result = 0x7FFFFFFF;
            // FIXME, ssturate CSR, SSR
        } else if (result < 0)
        {
            result = 0x80000000;
        }
        WRITE_BUFFER(core, src_dst + s * GPR_NUM_IN_GROUP, result);
    } else
    {                                   /* that's s2sh's business */
        return exec_s2sh(core, insn, fields);
    }

    core->pc += 2;
    return 0;
}

static int exec_s2sh(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op = BITS(11, 12);
    int base = fields->compact_rs ? 16 : 0;
    int src_dst = BITS(7, 9) + base;
    int src1 = BITS(13, 15) + base;
    int s = BITS(0, 0);

    fields->exec_unit = UNIT_S1 << BITS(0, 0);
    uint32_t shift = core->gpr[s][src1];
    int32_t result = (int32_t) core->gpr[s][src_dst];

    if (op == 0x0)
    {                                   //shl
        shift &= 0x3F;
        if (shift > 39)
            shift = 40;
        if (shift >= 32)
            result = 0;
        else
            result <<= shift;
    } else if (op == 0x1)
    {                                   //shr
        shift &= 0x3F;
        if (shift > 39)
            shift = 40;
        if (shift >= 32)
            result = 0;
        else
            result >>= shift;
    } else if (op == 0x2)
    {                                   //shru
        shift &= 0x3F;
        if (shift > 39)
            shift = 40;
        if (shift >= 32)
            result = 0;
        else
            result >>= shift;
    } else if (op == 0x3)
    {                                   //sshl
        uint32_t mask = 0xFFFFFFFF >> shift;
        uint32_t tst = (result & mask) >> (31 - shift);

        if (tst == 0x0 || tst == (1 << shift - 1))
        {
            result <<= shift;
            // FIXME, ssturate CSR, SSR
        } else if (result > 0)
        {
            result = 0x7FFFFFFF;
            // FIXME, ssturate CSR, SSR
        } else if (result < 0)
        {
            result = 0x80000000;
        }
    }

    WRITE_BUFFER(core, src_dst + s * GPR_NUM_IN_GROUP, result);

    core->pc += 2;
    return 0;
}

static int exec_sc5(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op = BITS(5, 6);
    int ucst0_2 = BITS(13, 15);
    int ucst3_4 = BITS(11, 12);
    int src = BITS(7, 9) + (fields->compact_rs ? 16 : 0);
    int s = BITS(0, 0);

    fields->exec_unit = UNIT_S1 << BITS(0, 0);
    int ucst5 = ucst0_2 | (ucst3_4 << 3);

    fields->exec_unit = UNIT_S1 << s;
    int result;

    if (op == 0x2)
    {
        int v = ((0xFFFFFFFF << (ucst5 + 1)) | (0xFFFFFFFF >> (32 - ucst5)));

        result = core->gpr[s][src] & v;
        WRITE_BUFFER(core, src + s * GPR_NUM_IN_GROUP, result);
    } else if (op == 0x1)
    {                                   /* set */
        int32_t v = 0xFFFFFFFF << ucst5;

        v &= 0xFFFFFFFF >> (31 - ucst5);
        result = core->gpr[s][src] | v;
        WRITE_BUFFER(core, src + s * GPR_NUM_IN_GROUP, result);
    } else if (op == 0x3)
    {                                   /* s2ext */
        int op1 = BITS(11, 12);
        int dst = BITS(13, 15);

        if (op1 == 0x1)
        {                               /* ext */
            result = (core->gpr[s][src] << 24) >> 24;
            result = SIGN_EXTEND(result, 31 - 24);
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
            DBG("In %s, ext src=%d, result=0x%x\n", __FUNCTION__, src, core->gpr[s][dst]);
        } else if (op1 == 0x0)
        {
            result = (core->gpr[s][src] << 16) >> 16;
            result = SIGN_EXTEND(result, 31 - 24);
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else if (op1 == 0x2)
        {
            result = (core->gpr[s][src] << 16) >> 16;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else if (op1 == 0x3)
        {
            result = (core->gpr[s][src] << 24) >> 24;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        }
    } else if (op == 0x0)
    {
        /*extu */
        result = (core->gpr[s][src] << ucst5) >> 31;
        WRITE_BUFFER(core, 0 + s * GPR_NUM_IN_GROUP, result);
    } else
    {
        NOT_IMP;
    }
    core->pc += 2;
    return 0;
}
static int exec_s2ext(c6k_core_t * core, uint32_t insn, opcode_fields_t * unused)
{
    NOT_IMP;
    return 0;
}

static int exec_sx2op(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int s = BITS(0, 0);

    fields->exec_unit = UNIT_S1 << BITS(0, 0);
    int x = BITS(12, 12) ? (!s) & 0x1 : s;
    int op = BITS(11, 11);
    int32_t src2 = (int32_t) core->gpr[x][BITS(7, 9)];
    int32_t dst = (int32_t) core->gpr[s][BITS(13, 15)];
    int dst_reg = BITS(13, 15) + s * GPR_NUM_IN_GROUP;

    if (!op)
        dst += src2;
    else
        dst -= src2;

    WRITE_BUFFER(core, dst_reg, dst);

    core->pc += 2;
    return 0;
}

static int exec_sx5(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int base = fields->compact_rs ? 16 : 0;
    int dst = BITS(7, 9) + base;
    int ucst3_4 = BITS(11, 12);
    int ucst0_2 = BITS(13, 15);
    int ucst5 = ucst0_2 | (ucst3_4 << 3);
    int s = BITS(0, 0);

    fields->exec_unit = UNIT_S1 << s;
    /* ADDK */
    int32_t result = core->gpr[s][dst] + ucst5;

    WRITE_BUFFER(core, s * GPR_NUM_IN_GROUP + dst, result);
    core->pc += 2;
    return 0;
}

static int exec_sx1(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op = BITS(13, 15);
    int src = BITS(7, 9);
    int s = BITS(0, 0);

    fields->exec_unit = UNIT_S1 << s;
    if (op == 6)
    {
        core->ilc = core->gpr[s][src];
    } else if (op == 0x0 || op == 0x1 || op == 0x5 || op == 0x7)
    {
        return exec_lsd_x1(core, insn, fields);
    } else if (op == 0x2)
    {
        WRITE_BUFFER(core, src + s * GPR_NUM_IN_GROUP, 0 - core->gpr[s][src]);
    } else if (op == 0x3)
    {
        WRITE_BUFFER(core, src + s * GPR_NUM_IN_GROUP, -1 + core->gpr[s][src]);
    } else
    {
        NOT_IMP;
    }
    core->pc += 2;
    return 0;
}

static int exec_sx1b(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int src2 = BITS(7, 10);
    int n3 = BITS(13, 15);
    int s = BITS(0, 0);

    fields->exec_unit = UNIT_S1 << BITS(0, 0);
    core->pfc = core->gpr[s][src2];
    REGISTER_BRANCH_EVENT(core, core->pfc);
    ADD_PENDING_NOPS(core, n3);
    DBG("In %s, pfc=0x%x\n", __FUNCTION__, core->pfc);

    core->pc += 2;
    return 0;
}
static int exec_lsd_mvto(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int s = BITS(0, 0);
    int unit = BITS(3, 4);
    int dst = BITS(13, 15);
    int srcms = BITS(10, 11);
    int src2 = BITS(7, 9);
    int x = BITS(12, 12);

    DBG("In %s, unit=%d\n", __FUNCTION__, unit);
    if (unit == 0)
    {
        fields->exec_unit = UNIT_L1 << s;
        int src = (srcms << 3) | src2;

        if (x)
            x = ((!s) & 0x1);
        else
            x = s;
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, core->gpr[x][src]);
    } else if (unit == 1)
    {
        fields->exec_unit = UNIT_S1 << s;
        int src = (srcms << 3) | src2;

        if (x)
            x = ((!s) & 0x1);
        else
            x = s;
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, core->gpr[x][src]);
    } else
    {
        fields->exec_unit = UNIT_D1 << s;
        int src = (srcms << 3) | src2;

        if (x)
            x = ((!s) & 0x1);
        else
            x = s;
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, core->gpr[x][src]);
    }
    core->pc += 2;
    return 0;
}
static int exec_lsd_mvfr(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int unit = BITS(3, 4);

    DBG("In %s, unit=%d\n", __FUNCTION__, unit);
    int dstms = BITS(10, 11);
    int dst = BITS(13, 15);
    int base = fields->compact_rs ? 16 : 0;
    int s = BITS(0, 0);
    int src2 = BITS(7, 9) + base;
    int x = BITS(12, 12);

    if (unit == 0x0)
    {
        fields->exec_unit = UNIT_L1 << s;
        dst = (dstms << 3) | dst;
        if (x)
            x = ((!s) & 0x1);
        else
            x = s;
        int result = core->gpr[x][src2];

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        DBG("In %s, s=%d, dst=%d,src2=%d, pc=0x%x\n", __FUNCTION__, s, dst, src2, core->pc);
    } else if (unit == 2)
    {
        fields->exec_unit = UNIT_D1 << s;
        dst = (dstms << 3) | dst;
        if (x)
            x = ((!s) & 0x1);
        else
            x = s;

        int result = core->gpr[x][src2];

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        DBG("In %s, s=%d, dst=%d,src2=%d\n", __FUNCTION__, s, dst, src2);
    } else if (unit == 1)
    {
        fields->exec_unit = UNIT_S1 << s;
        dst = (dstms << 3) | dst;
        if (x)
            x = ((!s) & 0x1);
        else
            x = s;

        int result = core->gpr[x][src2];

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        DBG("In %s, s=%d, dst=%d,src2=%d\n", __FUNCTION__, s, dst, src2);
    } else
    {
        NOT_IMP;
    }
    core->pc += 2;
    return 0;
}
static int exec_lsd_x1c(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int cc = BITS(14, 15);
    int ucst1 = BITS(13, 13);
    int unit = BITS(3, 4);
    int dst = BITS(7, 9);
    int s = BITS(0, 0);

    if (cc == 2)
    {
        if (core->gpr[GPR_B][0])
        {
            fields->exec_unit = ASSIGN_EXEC_UNIT(fields, unit, s);
            WRITE_BUFFER(core, s * GPR_NUM_IN_GROUP + dst, ucst1);
        } else
        {
            /* do nothing */
        }
    } else if (cc == 0)
    {
        if (core->gpr[GPR_A][0])
        {
            fields->exec_unit = ASSIGN_EXEC_UNIT(fields, unit, s);
            WRITE_BUFFER(core, s * GPR_NUM_IN_GROUP + dst, ucst1);
        } else
        {
            /* do nothing */
        }

    } else if (cc == 0x3)
    {
        if (!core->gpr[GPR_B][0])
        {
            fields->exec_unit = ASSIGN_EXEC_UNIT(fields, unit, s);
            WRITE_BUFFER(core, s * GPR_NUM_IN_GROUP + dst, ucst1);
        } else
        {
            /* do nothing */
        }

    } else if (cc = 0x1)
    {
        if (!core->gpr[GPR_A][0])
        {
            fields->exec_unit = ASSIGN_EXEC_UNIT(fields, unit, s);
            WRITE_BUFFER(core, s * GPR_NUM_IN_GROUP + dst, ucst1);
        } else
        {
            /* do nothing */
        }

    } else
    {
        NOT_IMP;
    }
    core->pc += 2;
    return 0;
}

static int exec_lsd_x1(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int op = BITS(13, 15);
    int32_t src = BITS(7, 9);
    int unit = BITS(3, 4);
    int s = BITS(0, 0);
    int dst = core->gpr[s][src];

    switch (op)
    {
        case 0x0:
            fields->exec_unit = ASSIGN_EXEC_UNIT(fields, unit, s);
            WRITE_BUFFER(core, s * GPR_NUM_IN_GROUP + src, 0);
            break;
        case 0x1:
            fields->exec_unit = ASSIGN_EXEC_UNIT(fields, unit, s);
            WRITE_BUFFER(core, s * GPR_NUM_IN_GROUP + src, 1);
            break;
        case 0x2:                      // nevermind, other insns will take care of those
            fields->exec_unit = ASSIGN_EXEC_UNIT(fields, unit, s);
            WRITE_BUFFER(core, s * GPR_NUM_IN_GROUP + src, 0 - core->gpr[s][src]);
            break;
        case 0x3:
        case 0x4:
        case 0x6:
            NOT_IMP;
            break;
        case 0x5:
            fields->exec_unit = ASSIGN_EXEC_UNIT(fields, unit, s);
            WRITE_BUFFER(core, s * GPR_NUM_IN_GROUP + src, 1 + dst);
            break;
            NOT_IMP;
            break;
        case 0x7:
            fields->exec_unit = ASSIGN_EXEC_UNIT(fields, unit, s);
            WRITE_BUFFER(core, s * GPR_NUM_IN_GROUP + src, 1 ^ dst);
            break;
        default:
            NOT_IMP;
            break;
    }

    core->pc += 2;
    return 0;
}

static int exec_uspl(c6k_core_t * core, uint32_t insn, opcode_fields_t * unused)
{
    int ii = BITS(7, 9) + (BITS(14, 14) ? 8 : 0);
    int op = BITS(0, 0);
    uint32_t pbits = core->header & 0x3FFF;

    core->sploop_begin = core->pc;
    core->sploop_ii = ii + 1;
    if (pbits)
        core->sploop_begin_paralleled = 1;

    core->sploop_flag = op ? SPLOOPD : SPLOOP;

    core->pc += 2;

    return 0;
}

static int exec_uspldr(c6k_core_t * core, uint32_t insn, opcode_fields_t * unused)
{
    NOT_IMP;
    return 0;
}
static int exec_uspk(c6k_core_t * core, uint32_t insn, opcode_fields_t * unused)
{
    core->sploop_end = core->pc;
    if (core->sploop_flag == SPLOOP)
        core->ilc = core->ilc == 0 ? 0 : core->ilc - 1;
    DBG("In %s, sploop_end=0x%x, ilc=%d\n", __FUNCTION__, core->sploop_end, core->ilc);
    core->pc += 2;
    return 0;
}
static int exec_uspm(c6k_core_t * core, uint32_t insn, opcode_fields_t * unused)
{
    core->spmask = ASSEMBLE_COMPACT_SPMASK(insn);
    core->spmask_begin = core->pc;

    core->pc += 2;
    return 0;
}
static int exec_unop(c6k_core_t * core, uint32_t insn, opcode_fields_t * unused)
{
    int n3 = BITS(13, 15);

    CYCLE_INC(core, n3);
    core->pc += 2;
    return 0;
}

static insn_action_t insn16_action[] = {
    exec_doff4,
    exec_doff4_dw,
    exec_dind,
    exec_dind_dw,
    exec_dinc,
    exec_dinc_dw,
    exec_ddec,
    exec_ddec_dw,
    exec_dstk,
    exec_dx2op,
    exec_dx5,
    exec_dx5p,
    exec_dx1,
    exec_dpp,
    exec_l3,
    exec_l3i,
    exec_ltbd,
    exec_l2c,
    exec_lx5,
    exec_lx3c,
    exec_lx1c,
    exec_lx1,
    exec_m3,
    exec_sbu8,
    exec_sbs7,
    exec_scs10,
    exec_sbu8c,
    exec_sbs7c,
    exec_s3,
    exec_s3i,
    exec_smvk8,
    exec_ssh5,
    exec_s2sh,
    exec_sc5,
    exec_s2ext,
    exec_sx2op,
    exec_sx5,
    exec_sx1,
    exec_sx1b,
    exec_lsd_mvto,
    exec_lsd_mvfr,
    exec_lsd_x1c,
    exec_lsd_x1,
    exec_uspl,
    exec_uspldr,
    exec_uspk,
    exec_uspm,
    exec_uspm,
    exec_unop,
    NULL,
};

int get_insn16_index(uint32_t insn)
{
    int index;
    int64_t delay_slot;
    int ret = decode_instr(insn, &index, (ISEITEM *) insn16_decode_table,
                           sizeof (insn16_decode_table) / sizeof (ISEITEM),
                           -1, &delay_slot);

    if (ret == DECODE_SUCCESS)
        return index;
    else
    {
        printf("Can't implement the instruction 0x%x ", insn);
        return -1;
    }
}

int get_insn16_number()
{
    return sizeof (insn16_decode_table) / sizeof (ISEITEM);
}

char *get_insn16_name(int index)
{
    if (index < get_insn16_number())
    {
        return insn16_decode_table[index].name;
    } else
        return NULL;
}

int32_t get_insn16_delay_slot(uint32_t insn)
{
    int index, max;

    index = get_insn16_index(insn);
    max = get_insn16_number();
    if (index < 0)
        return -1;
    if (index > max)
        return -1;
    return insn16_decode_table[index].delay_slot;
}

insn_action_t get_insn16_action(c6k_core_t * core, uint32_t insn, int br, int32_t * index, int64_t * delay_slot)
{
    insn_action_t ret = NULL;

    if (decode_instr(insn, index, (ISEITEM *) insn16_decode_table,
                     sizeof (insn16_decode_table) / sizeof (ISEITEM), br, delay_slot) == DECODE_SUCCESS)
    {
        DBG("In %s, instr=0x%x, index=0x%x\n", __FUNCTION__, insn, *index);
        return insn16_action[*index];
    } else
    {
        DBG("In %s, decode error for 0x%x at 0x%x\n", __FUNCTION__, insn, core->pc);
        exit(-1);
    }
    return ret;
}
