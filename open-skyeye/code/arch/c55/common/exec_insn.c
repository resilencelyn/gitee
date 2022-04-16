/* Copyright (C) 
 *2013 - Michael.Kang blackfin.kang@gmail.com
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

#include "c55_cpu.h"
#include "c55_decode.h"
#include <stdio.h>
#include <stdlib.h>
#include "skyeye_types.h"
#include "skyeye_bus.h"
#include "skyeye_uart_ops.h"
#include "skyeye_interface.h"
#include <skyeye_swapendian.h>

#define DEBUG
#include <skyeye_log.h>

#ifdef DBG
#undef DBG
#define DBG
#endif

#define NOT_IMP printf("In %s:%d, not implement at 0x%x\n", __FUNCTION__, __LINE__, core->pc);skyeye_exit(-1)

static int exec_xccpart_cond_mov_k4_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                        opcode_fields_t * fields)
{
    int src = BITS(16, 18);
    int fs = BITS(19, 19);
    int cond = BITS(20, 22);
    int cond_flag = check_cond_l4(core, cond, src, fs);

    if (cond_flag == 1)
    {
        mov_k4_dst(core, insn, fields);
    }

    core->pc += 4;
    return 0;
}

static int exec_xccpart_cond_mov_lmem_acx(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                          opcode_fields_t * fields)
{
    int src = BITS(8, 10);
    int fs = BITS(11, 11);
    int dst = BITS(28, 29);
    int cond = BITS(20, 22);
    int addr_mode = BITS(32, 32);
    int mem32_val = 0;
    int cond_flag = check_cond_l4(core, cond, src, fs);

    if (cond_flag == 1)
    {
        //      mem32_val = read_imm_val_from_lmem(core, insn);
        if (addr_mode)
            mem32_val = read_val_from_indirect_lmem_address(core, insn);
        else
            mem32_val = read_val_from_direct_lmem_address(core, insn);

        core->ac[dst] = mem32_val;
    }

    core->pc += 5;
    return 0;
}

static int exec_xccpart_cond_neg_src_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                         opcode_fields_t * fields)
{
    int src = BITS(16, 18);
    int fs = BITS(19, 19);
    int cond = BITS(20, 22);
    int cond_flag = check_cond_l4(core, cond, src, fs);

    if (cond_flag == 1)
        neg_src_dst(core, insn, fields);

    core->pc += 4;
    return 0;
}
static int exec_xccpart_cond_add_k4_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                        opcode_fields_t * fields)
{
    int src = BITS(16, 18);
    int fs = BITS(19, 19);
    int cond = BITS(20, 22);
    int cond_flag = check_cond_l4(core, cond, src, fs);

    if (cond_flag == 1)
        add_k4_dst(core, insn, fields);

    core->pc += 4;
    return 0;
}
static int exec_xccpart_cond_sfts_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                      opcode_fields_t * fields)
{
    int src = BITS(16, 18);
    int fs = BITS(19, 19);
    int cond = BITS(20, 22);
    int m = BITS(36, 36);
    int cond_flag = check_cond_l4(core, cond, src, fs);

    if (cond_flag == 1)
    {
        if (m == 0)
            sfts_dst_right(core, insn, fields);
        else
            sfts_dst_left(core, insn, fields);
    }

    core->pc += 4;
    return 0;
}
static int exec_xccpart_cond_mov_k16_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                         opcode_fields_t * fields)
{
    int src = BITS(0, 2);
    int fs = BITS(3, 3);
    int cond = BITS(4, 6);

    int cond_flag = check_cond_l4(core, cond, src, fs);

    if (cond_flag == 1)
        mov_k16_dst(core, insn, fields);

    core->pc += 6;
    return 0;
}
static int exec_xccpart_cond_sftl_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                      opcode_fields_t * fields)
{
    int src = BITS(16, 18);
    int fs = BITS(19, 19);
    int cond = BITS(20, 22);
    int shift_flag = BITS(32, 34);
    int cond_flag = check_cond_l4(core, cond, src, fs);

    if (cond_flag == 1)
    {
        if (shift_flag == 0)
            sftl_dst_left(core, insn, fields);
        else
            sftl_dst_right(core, insn, fields);
    }

    core->pc += 4;
    return 0;
}
static int exec_xccpart_cond_sftl_acx_shift_acy(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                                opcode_fields_t * fields)
{
    int src = BITS(8, 10);
    int fs = BITS(11, 11);
    int cond = BITS(12, 14);

    int cond_flag = check_cond_l4(core, cond, src, fs);

    if (cond_flag == 1)
        sftl_acx_shift_acy(core, insn, fields);

    core->pc += 5;
    return 0;
}
static int exec_xccpart_cond_sub_k4_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                        opcode_fields_t * fields)
{
    int src = BITS(16, 18);
    int fs = BITS(19, 19);
    int cond = BITS(20, 22);
    int cond_flag = check_cond_l4(core, cond, src, fs);

    if (cond_flag == 1)
        sub_k4_dst(core, insn, fields);

    core->pc += 4;
    return 0;
}
static int exec_xccpart_cond_mov_k16_shift_acx(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                               opcode_fields_t * fields)
{
    int src = BITS(0, 2);
    int fs = BITS(3, 3);
    int cond = BITS(4, 6);

    int cond_flag = check_cond_l4(core, cond, src, fs);

    if (cond_flag == 1)
        mov_k16_acx(core, insn, fields);

    core->pc += 6;
    return 0;
}

static int exec_xccpart_cond_mov_smem_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                          opcode_fields_t * fields)
{
    int src = BITS(16, 18);
    int fs = BITS(19, 19);
    int cond = BITS(20, 22);
    int fd = BITS(43, 43);
    int dst = BITS(40, 42);
    int addr_mode = BITS(32, 32);
    uint16_t mem16_val = 0;

    if (addr_mode)
        mem16_val = read_val_from_indirect_smem_address(core, insn);
    else
        mem16_val = read_val_from_direct_smem_address(core, insn);

    //      uint16_t mem16_val = read_smem_to_register(core, insn);
    int cond_flag = check_cond_l4(core, cond, src, fs);

    if (cond_flag == 1)
    {
        if (fd == 1)
        {
            core->xar[dst].ar = mem16_val;
        } else
        {
            if (dst < 4)
            {
                core->ac[dst] = mem16_val;
            } else
            {
                core->t[dst - 4] = mem16_val;
            }
        }
    }

    core->pc += 4;
    //      core->pc += 2;
    return 0;
}
static int exec_xcc_cond_mov_lmem_acx(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                      opcode_fields_t * fields)
{
    int src = BITS(8, 10);
    int fs = BITS(11, 11);
    int cond = BITS(12, 14);

    int cond_flag = check_cond_l4(core, cond, src, fs);

    if (cond_flag == 1)
        mov_lmem_acx(core, insn, fields);

    core->pc += 5;
    return 0;
}
static int exec_xcc_cond_mov_src_smem(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                      opcode_fields_t * fields)
{
    int src = BITS(16, 18);
    int fs = BITS(19, 19);
    int cond = BITS(20, 22);
    int mov_fs = BITS(43, 43);
    int mov_src = BITS(40, 42);
    int addr_mode = BITS(32, 32);
    uint16_t mem16_val = 0;
    int cond_flag = check_cond_l4(core, cond, src, fs);

    if (cond_flag == 1)
    {
        if (mov_fs == 1)
        {
            mem16_val = core->xar[mov_src].ar;
        } else
        {
            if (mov_src < 4)
            {
                mem16_val = core->ac[mov_src];
            } else
            {
                mem16_val = core->t[mov_src - 4];
            }
        }
        if (addr_mode)
            write_val_to_indirect_smem_address(core, insn, mem16_val);
        else
            write_val_to_direct_smem_address(core, insn, mem16_val);
    }

    core->pc += 4;
    return 0;
}

static int exec_add_acx_shift_acy(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int shift = fields->shift;
    int acx = BITS(36, 37);
    int acy = BITS(38, 39);
    int m40 = bit(core->st1_55, M40);

    core->pc += 3;
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }

    core->ac[acy] = core->ac[acy] + (core->ac[acx] << shift);
    int x32 = (core->ac[acy] >> 32) & 0x1;
    int x40 = (core->ac[acy] >> 40) & 0x1;

    if (m40)
    {
        if (x40)
            SET(core->st0_55, CARRY);
        else
            CLEAR(core->st0_55, CARRY);
    } else
    {
        if (x32)
            SET(core->st0_55, CARRY);
        else
            CLEAR(core->st0_55, CARRY);
    }

    while (core->rptc)
    {
        core->rptc--;
        return 0;
    }
    return 0;
}
static int exec_amar_smem_xadst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    uint16_t data;
    int addr = fields->smem;
    int src = BITS(37, 39);
    int op = BITS(32, 47);
    int flag = bit(core->st2_55, 14);
    int *xdst = NULL;

    if (op == 0xec31)
    {
        xdst = get_xadst_register(core, insn, fields);
        if (flag)
        {
            *xdst = addr;
        } else
            SET(core->st2_55, 14);
        core->pc += 6;
    } else
    {
        xdst = get_xadst_register(core, insn, fields);
        if (flag)
        {
            *xdst = core->xar[src].ar;
        } else
            SET(core->st2_55, 14);

        core->pc += 3;
    }

    return 0;
}
static int exec_and_k16_abs_smem(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k16 = fields->k16;
    int lsmem = BITS(0, 15);
    uint16_t mem16_val = 0;

    mem16_val = c55_memory_read(core, lsmem);
    c55_memory_write(core, lsmem, k16 & mem16_val);

    core->pc += 6;
    return 0;
}
static int exec_and_k16_smem(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k16 = fields->k16;
    int mmap_op = BITS(8, 15);
    int smem = fields->addr;
    uint16_t mem16_val = 0;

    if (mmap_op == 0x98)
    {                                   //mmap(smem)
        switch (smem)
        {
            case AC0_ADDR:
                mem16_val = core->ac[0] & 0xFFFF;
                core->ac[0] = mem16_val & k16;
                break;
            case AC0G_ADDR:
                mem16_val = (core->ac[0] >> 32) & 0xFF;
                core->ac[0] = mem16_val & k16;
                break;
            case AC1_ADDR:
                mem16_val = core->ac[1] & 0xFFFF;
                core->ac[1] = mem16_val & k16;
                break;
            case AC1G_ADDR:
                mem16_val = (core->ac[1] >> 32) & 0xFF;
                core->ac[1] = mem16_val & k16;
            case AC2_ADDR:
                mem16_val = core->ac[2] & 0xFFFF;
                core->ac[2] = mem16_val & k16;
                break;
            case AC2G_ADDR:
                mem16_val = (core->ac[2] >> 32) & 0xFF;
                core->ac[2] = mem16_val & k16;
                break;
            case AC3_ADDR:
                mem16_val = core->ac[3] & 0xFFFF;
                core->ac[3] = mem16_val & k16;
                break;
            case AC3G_ADDR:
                mem16_val = (core->ac[3] >> 32) & 0xFF;
                core->ac[3] = mem16_val & k16;
                break;
            case ST0_ADDR:
                mem16_val = core->st0_55;
                core->st0_55 = mem16_val & k16;
                break;
            case ST1_ADDR:
                mem16_val = core->st1_55;
                core->st1_55 = mem16_val & k16;
                break;
            case ST2_ADDR:
                mem16_val = core->st2_55;
                core->st2_55 = mem16_val & k16;
                break;
            case ST3_ADDR:
                mem16_val = core->st3_55;
                core->st3_55 = mem16_val & k16;
                break;
            case DPH_ADDR:
                mem16_val = core->dph;
                core->dph = mem16_val & k16;
                break;
              defalt:
                if (smem >= AR0_ADDR && smem <= AR7_ADDR)
                {
                    mem16_val = core->xar[smem - AR0_ADDR].ar;
                    core->xar[smem - AR0_ADDR].ar = mem16_val & k16;
                    break;
                } else
                {
                    printf("In %s, not decode this instruction correct at pc = 0x%x\n", __func__, core->pc);
                    skyeye_exit(-1);
                }
        }
        c55_memory_write(core, smem, mem16_val & k16);
        core->pc += 5;
    } else
    {
        mem16_val = c55_memory_read(core, (smem + core->sp));
        c55_memory_write(core, (smem + core->sp), mem16_val & k16);
        core->pc += 4;
    }

    return 0;
}
static int exec_or_k16_smem(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    uint16_t k16 = fields->k16;
    int smem = fields->addr;
    int mm_st = BITS(32, 39);
    int mmap_op = BITS(8, 15);
    int addr = BITS(37, 39);
    int op = BITS(33, 35);
    int op1 = BITS(32, 32);
    int op2 = BITS(36, 36);
    int lsmem = BITS(0, 15);
    int16_t mem_val;

    if (mmap_op == 0x98)
    {                                   //mmap(smem)
        switch (smem)
        {
            case AC0_ADDR:
                mem_val = core->ac[0] & 0xFFFF;
                core->ac[0] = mem_val | k16;
                break;
            case AC1_ADDR:
                mem_val = core->ac[1] & 0xFFFF;
                core->ac[1] = mem_val | k16;
                break;
            case ST0_ADDR:
                mem_val = core->st0_55;
                core->st0_55 = mem_val | k16;
                break;
            case ST1_ADDR:
                mem_val = core->st1_55;
                core->st1_55 = mem_val | k16;
                break;
            case ST2_ADDR:
                mem_val = core->st2_55;
                core->st2_55 = mem_val | k16;
                break;
            case ST3_ADDR:
                mem_val = core->st3_55;
                core->st3_55 = mem_val | k16;
                break;
            case DPH_ADDR:
                mem_val = core->dph;
                core->dph = mem_val | k16;
                break;
              defalt:
                if (smem >= AR0_ADDR && smem <= AR7_ADDR)
                {
                    mem_val = core->xar[smem - AR0_ADDR].ar;
                    core->xar[smem - AR0_ADDR].ar = mem_val | k16;
                    break;
                } else
                {
                    printf("In %s, not decode this instruction correct at pc = 0x%x\n", __func__, core->pc);
                    skyeye_exit(-1);
                }
        }
        c55_memory_write(core, smem, mem_val | k16);
        core->pc += 5;
    } else
    {
        if (op1 == 0)
        {
            mem_val = c55_memory_read(core, (smem + core->sp));
            mem_val |= k16;
            c55_memory_write(core, (smem + core->sp), mem_val);

            core->pc += 4;
        } else
        {
            if (op == 0)
            {
                if (op2 == 0)
                {
                    mem_val = c55_memory_read(core, core->xar[addr].ar);
                    mem_val |= k16;
                    c55_memory_write(core, core->xar[addr].ar, mem_val);

                    core->pc += 4;
                } else
                {
                    mem_val = c55_memory_read(core, lsmem);
                    mem_val |= k16;
                    c55_memory_write(core, lsmem, mem_val);

                    core->pc += 6;
                }
            } else if (op == 0x1)
            {
                mem_val = c55_memory_read(core, core->xar[addr].ar);
                mem_val |= k16;
                c55_memory_write(core, core->xar[addr].ar, mem_val);

                core->xar[addr].ar += 1;
                core->pc += 4;
            } else if (op == 0x6)
            {
                if ((lsmem >> 15) & 0x1)
                {
                    lsmem = (lsmem - 0xFFFF) - 1;
                }
                mem_val = c55_memory_read(core, (lsmem + core->xar[addr].ar));
                mem_val |= k16;
                c55_memory_write(core, (lsmem + core->xar[addr].ar), mem_val);

                core->pc += 6;
            } else if (op == 0x7)
            {
                mem_val = c55_memory_read(core, (lsmem + core->xar[addr].ar));
                mem_val |= k16;
                c55_memory_write(core, (lsmem + core->xar[addr].ar), mem_val);

                core->xar[addr].ar += 1;
                core->pc += 6;
            } else
            {
                printf("In %s, %d, decode error insn %lx\n", __FUNCTION__, __LINE__, insn);
                skyeye_exit(-1);
            }
        }
    }

    return 0;
}
static int exec_bclr_or_bset_k4_stx(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                    opcode_fields_t * fields)
{
    int stx_55 = BITS(32, 35);
    int k4 = fields->k4;

    if (stx_55 % 2 == 0)
    {
        if (stx_55 == 0x0)
        {
            CLEAR(core->st0_55, k4);
            c55_memory_write(core, ST0_ADDR, core->st0_55);
        } else if (stx_55 == 0x2)
        {
            CLEAR(core->st1_55, k4);
            c55_memory_write(core, ST1_ADDR, core->st1_55);
        } else if (stx_55 == 0x4)
        {
            CLEAR(core->st2_55, k4);
            c55_memory_write(core, ST2_ADDR, core->st2_55);
        } else if (stx_55 == 0x6)
            CLEAR(core->st3_55, k4);
        c55_memory_write(core, ST3_ADDR, core->st3_55);
    } else
    {
        if (stx_55 == 0x1)
        {
            SET(core->st0_55, k4);
            c55_memory_write(core, ST0_ADDR, core->st0_55);
        } else if (stx_55 == 0x3)
        {
            SET(core->st1_55, k4);
            c55_memory_write(core, ST1_ADDR, core->st1_55);
        } else if (stx_55 == 0x5)
        {
            SET(core->st2_55, k4);
            c55_memory_write(core, ST2_ADDR, core->st2_55);
        } else if (stx_55 == 0x7)
            SET(core->st3_55, k4);
        c55_memory_write(core, ST3_ADDR, core->st3_55);
    }

    core->pc += 2;
    return 0;
}
static int exec_call_p24(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int p24 = fields->p24;

    core->sp -= 1;
    core->reta = core->pc + 4;
    c55_memory_write(core, core->sp, core->reta & 0xffff);
    core->ssp -= 1;
    c55_memory_write(core, core->ssp, (core->reta >> 16) & 0xff);

    core->pc = p24;
    return 0;
}

/* repeat instruction */
static int exec_aadd_k8_sp(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k8 = fields->k8;

    k8 = SIGN_EXTEND(k8, 8);
    while (core->rptc)
    {
        core->rptc--;
        return 0;
    }
    core->sp += k8;
    core->pc += 2;

    return 0;
}
static int exec_aadd_p8_tax(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    unsigned char p8 = BITS(32, 39);
    int dst = BITS(28, 30);
    int fd = BITS(31, 31);

    if (fd == 1)
    {
        core->xar[dst].ar += p8;
    } else
    {
        core->t[dst - 4] += p8;
    }
    while (core->rptc)
    {
        core->rptc--;
        return 0;
    }

    core->pc += 3;
    return 0;
}

/* repeat instruction */
static int exec_not_src_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int fs = BITS(39, 39);
    int fd = BITS(35, 35);
    int src = fields->src;
    int dst = fields->dst;

    if (fs == 1)
    {
        if (fd == 1)
        {
            core->xar[dst].ar = bit16_reverse(core->xar[src].ar);
        } else
        {
            if (dst < 4)
            {
                core->ac[dst] = bit16_reverse(core->xar[src].ar);
            } else
            {
                core->t[dst - 4] = bit16_reverse(core->xar[src].ar);
            }
        }
    } else
    {
        if (fd == 1)
        {
            if (src < 4)
            {
                core->xar[dst].ar = bit16_reverse(core->ac[src]);
            } else
            {
                core->xar[dst].ar = bit16_reverse(core->t[src - 4]);
            }
        } else
        {
            if (dst < 4)
            {
                if (src < 4)
                {
                    core->ac[dst] = 0xFFFFFFFFFF - core->ac[src];
                } else
                {
                    core->ac[dst] = bit16_reverse(core->t[src - 4]);
                }
            } else
            {
                if (src < 4)
                {
                    core->t[dst - 4] = bit16_reverse(core->ac[src]);
                } else
                {
                    core->t[dst - 4] = bit16_reverse(core->t[src - 4]);
                }
            }
        }
    }

    while (core->rptc)
    {
        core->rptc--;
        return 0;
    }
    core->pc += 2;
    return 0;
}
static int exec_bcc_l8_cond(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int cond = BITS(36, 38);
    int src = BITS(32, 34);
    int fs = BITS(35, 35);
    int l8 = fields->l8;
    int cond_flag = check_cond_l4(core, cond, src, fs);

    if (cond_flag == 1)
    {
        if ((l8 >> 7) & 0x1)
            core->pc -= (0xFF - l8 + 1);
        else
            core->pc += l8;
    }

    core->pc += 3;
    return 0;
}
static int exec_mov_smem_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int fd = fields->f1;
    int dst = fields->dst1;
    int parallel_op = BITS(24, 31);
    int addr_mode = BITS(32, 32);
    uint16_t mem16_val = 0;

    if (addr_mode)
        mem16_val = read_val_from_indirect_smem_address(core, insn);
    else
        mem16_val = read_val_from_direct_smem_address(core, insn);

    core->pc += 2;

    /*instruction : a961_99 : MOV port(*AR3), AR1 */
    if (parallel_op == 0x99)
    {
        core->pc += 1;
    } else
    {
        if (No_exp != check_parallel_insn(conf_obj, core))
        {
            printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
            skyeye_exit(-1);
        }
    }

    if (fd == 1)
    {
        core->xar[dst].ar = mem16_val;
    } else
    {
        if (dst < 4)
        {
            core->ac[dst] = mem16_val;
        } else
        {
            core->t[dst - 4] = mem16_val;
        }
    }

    return 0;
}
static int exec_bcc_l4_cond(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int l4 = BITS(39, 42);
    int cond = BITS(36, 38);
    int src = BITS(32, 34);
    int fs = BITS(35, 35);
    int cond_flag = check_cond_l4(core, cond, src, fs);

    if (cond_flag == 1)
        core->pc += l4;

    core->pc += 2;
    return 0;
}
static int exec_mov_abs_lmem_acx(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int dst = BITS(28, 29);
    int lmem = BITS(8, 23);
    int flag = bit(core->st2_55, 14);

    int mem32_val = c55_memory_read(core, lmem);

    core->pc += 5;

    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }

    if (flag)
    {
        core->ac[dst] = mem32_val;
    } else
        SET(core->st2_55, 14);

    return 0;
}
static int exec_mov_lmem_acx(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int dst = BITS(28, 29);
    int flag = bit(core->st2_55, 14);
    int addr_mode = BITS(32, 32);
    uint32_t mem32_val = 0;

    //      mem32_val = read_lmem_data(core, insn);
    if (addr_mode)
        mem32_val = read_val_from_indirect_lmem_address(core, insn);
    else
        mem32_val = read_val_from_direct_lmem_address(core, insn);

    core->pc += 3;
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }

    if (core->exec_enable)
    {
        if (flag)
        {
            core->ac[dst] = mem32_val;
        } else
            SET(core->st2_55, 14);
    } else
    {
        core->exec_enable = 1;
        SET(core->st2_55, 14);
    }
    return 0;
}
static int exec_ret(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    core->pc += 2;

    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }
    core->pc = c55_memory_read(core, core->sp);
    core->pc |= c55_memory_read(core, core->ssp) << 16;

    core->sp += 1;
    core->ssp += 1;

    return 0;
}
static int exec_mov_k16_acx(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int dst = BITS(20, 21);
    int k16 = BITS(24, 39);

    core->pc += 4;
    //check_next_insn is parallel_insn 
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }

    core->ac[dst] = k16 << 16;

    return 0;
}
static int exec_or_k16_src_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int fs = fields->fs;
    int fd = fields->fd;
    int fsrc = fields->fsrc;
    int fdst = fields->fdst;
    int k16 = BITS(24, 39);
    int flag = 1;

    //check_next_insn 

    insn = insn & 0xFFFF;
    int insn_op = BITS(7, 15);

    if (insn_op == 0x13d)
    {
        int cond = BITS(4, 6);
        int src = BITS(0, 2);
        int fs = BITS(3, 3);

        int cond_flag = check_cond_l4(core, cond, src, fs);

        core->pc += 2;

        if (cond_flag == 0)
            flag = 0;
    }

    if (flag)
    {
        if (fd == 0)
        {
            if (fdst < 4)
            {
                if (fs == 0)
                {
                    if (fsrc < 4)
                    {
                        core->ac[fdst] = (core->ac[fsrc] | k16);
                    } else
                    {
                        core->ac[fdst] = (core->t[fsrc - 4] | k16);
                    }
                } else
                    core->ac[fdst] = (core->xar[fsrc].ar | k16);
            } else
            {
                if (fs == 0)
                {
                    if (fsrc < 4)
                    {
                        core->t[fdst - 4] = (core->ac[fsrc] | k16);
                    } else
                    {
                        core->t[fdst - 4] = (core->t[fsrc - 4] | k16);
                    }
                } else
                    core->t[fdst - 4] = (core->xar[fsrc].ar | k16);
            }
        } else
        {
            if (fs == 0)
            {
                if (fsrc < 4)
                {
                    core->xar[fdst].ar = (core->ac[fsrc] | k16);
                } else
                {
                    core->xar[fdst].ar = (core->t[fsrc - 4] | k16);
                }
            } else
                core->xar[fdst].ar = (core->xar[fsrc].ar | k16);
        }
    }
    core->pc += 4;

    return 0;
}

static int exec_cmp_src_dst_tcx(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int x = BITS(24, 24);
    int cc = BITS(34, 35);
    int u = BITS(26, 26);
    int fdst = BITS(28, 31);
    int fsrc = BITS(36, 39);
    int set_flag = 0, tcx = 0, ac_flag = 0;
    uint32_t temp_src, temp_dst;

    switch (fsrc)
    {
        case 0:
        case 1:
        case 2:
        case 3:
            temp_src = core->ac[fsrc];
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            temp_src = core->t[fsrc - 4];
            break;
        default:
            temp_src = core->xar[fsrc - 8].ar;
            break;
    }
    switch (fdst)
    {
        case 0:
        case 1:
        case 2:
        case 3:
            temp_dst = core->ac[fdst];
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            temp_dst = core->t[fdst - 4];
            break;
        default:
            temp_dst = core->xar[fdst - 8].ar;
            break;
    }
    if (fsrc > 3 || fdst > 3)
    {
        temp_src = temp_src & 0xFFFF;
        temp_dst = temp_dst & 0xFFFF;
    } else
        ac_flag = 1;

    if (x == 0)
    {
        tcx = TC1;
    } else
        tcx = TC2;

    switch (cc)
    {
        case 0:
            //src = dst
            if (u)
            {
                if (temp_src == temp_dst)
                    set_flag = 1;
            } else
            {
                if (ac_flag)
                {
                    if ((int64_t) temp_src == (int64_t) temp_dst)
                        set_flag = 1;
                } else
                {
                    if ((int16_t) temp_src == (int16_t) temp_dst)
                        set_flag = 1;
                }
            }
            break;
        case 1:
            //src < dst
            if (u)
            {
                if (temp_src < temp_dst)
                    set_flag = 1;
            } else
            {
                if (ac_flag)
                {
                    int result = (int32_t) (core->ac[fsrc] - core->ac[fdst]);

                    if (((result >> 31) & 0x1))
                        set_flag = 1;
                } else
                {
                    if ((int16_t) temp_src < (int16_t) temp_dst)
                        set_flag = 1;
                }
            }
            break;
        case 2:
            //src >= dst
            if (u)
            {
                if (temp_src >= temp_dst)
                    set_flag = 1;
            } else
            {
                if (ac_flag)
                {
                    int result = (int32_t) (core->ac[fsrc] - core->ac[fdst]);

                    if (((result >> 31) & 0x1) == 0 || result == 0)
                        set_flag = 1;
                } else
                {
                    if ((int16_t) temp_src >= (int16_t) temp_dst)
                        set_flag = 1;
                }
            }
            break;
        case 3:
            //src != dst
            if (u)
            {
                if (ac_flag)
                {
                    if (core->ac[fsrc] != core->ac[fdst])
                        set_flag = 1;
                } else
                {
                    if (temp_src != temp_dst)
                        set_flag = 1;
                }
            } else
            {
                if (ac_flag)
                {
                    if ((int64_t) core->ac[fsrc] != (int64_t) core->ac[fdst])
                        set_flag = 1;
                } else
                {
                    if ((int16_t) temp_src != (int16_t) temp_dst)
                        set_flag = 1;
                }
            }
            break;
    }

    if (set_flag)
    {
        SET(core->st0_55, tcx);
    } else
        CLEAR(core->st0_55, tcx);

    core->pc += 3;
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }

    return 0;
}
static int exec_nop(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    core->pc += 1;
    return 0;
}
static int exec_mov_k4_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    mov_k4_dst(core, insn, fields);

    core->pc += 2;
    return 0;
}
static int exec_mov_negk4_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    core->pc += 2;
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }

    mov_negk4_dst(core, insn, fields);

    return 0;
}
static int exec_xcc_cond(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int cond = BITS(36, 38);
    int src = BITS(32, 34);
    int fs = BITS(35, 35);

    int cond_flag = check_cond_l4(core, cond, src, fs);

    if (cond_flag == 0)
    {
        CLEAR(core->st2_55, 14);
        core->exec_enable = 0;
    }

    core->pc += 2;
    return 0;
}
static int exec_mov_acx_lmem(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int acx = fields->xdst;
    int dst = BITS(28, 29);
    int addr_mode = BITS(32, 32);
    int mem32_val = core->ac[dst];

    //      write_lmem_frome_register(core, insn, mem32_val);
    if (addr_mode)
        write_val_to_indirect_lmem_address(core, insn, mem32_val);
    else
        write_val_to_direct_lmem_address(core, insn, mem32_val);

    core->pc += 3;
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }
    return 0;
}
static int exec_xor_k16_smem(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k16 = fields->k16;
    int smem = fields->addr;
    int smem_data;

    smem_data = c55_memory_read(core, (smem + core->sp));
    smem_data = smem_data ^ k16;
    c55_memory_write(core, (smem + core->sp), smem_data);

    core->pc += 4;
    return 0;
}
static int exec_bclr_baddr_src(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int src = BITS(28, 30);
    int fs = BITS(31, 31);
    int baddr = BITS(33, 39);

    if (fs == 1)
    {
        core->xar[src].ar &= ~(0x1 << baddr);
    } else
    {
        if (src < 4)
        {
            core->ac[src] &= (~(0x1 << baddr) | 0xff00000000);
        } else
        {
            core->t[src - 4] &= ~(0x1 << baddr);
        }

    }
    while (core->rptc)
    {
        core->rptc--;
        return 0;
    }

    core->pc += 3;
    return 0;
}
static int exec_or_k8_src_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int fs = BITS(27, 27);
    int fd = fields->x;
    int fsrc = BITS(24, 26);
    int fdst = BITS(28, 30);
    int k8 = fields->k8;
    int flag = 1;

    //check_next_insn 

    insn = (insn >> 8) & 0xFFFF;
    int insn_op = BITS(7, 15);

    if (insn_op == 0x13d)
    {
        int cond = BITS(4, 6);
        int src = BITS(0, 2);
        int fs = BITS(3, 3);

        int cond_flag = check_cond_l4(core, cond, src, fs);

        core->pc += 2;

        if (cond_flag == 0)
            flag = 0;
    }

    core->pc += 3;
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }

    if (flag)
    {
        if (fd == 0)
        {
            if (fdst < 4)
            {
                if (fs == 0)
                {
                    if (fsrc < 4)
                    {
                        core->ac[fdst] = (core->ac[fsrc] | k8);
                    } else
                    {
                        core->ac[fdst] = (core->t[fsrc - 4] | k8);
                    }
                } else
                    core->ac[fdst] = (core->xar[fsrc].ar | k8);
            } else
            {
                if (fs == 0)
                {
                    if (fsrc < 4)
                    {
                        core->t[fdst - 4] = (core->ac[fsrc] | k8);
                    } else
                    {
                        core->t[fdst - 4] = (core->t[fsrc - 4] | k8);
                    }
                } else
                    core->t[fdst - 4] = (core->xar[fsrc].ar | k8);
            }
        } else
        {
            if (fs == 0)
            {
                if (fsrc < 4)
                {
                    core->xar[fdst].ar = (core->ac[fsrc] | k8);
                } else
                {
                    core->xar[fdst].ar = (core->t[fsrc - 4] | k8);
                }
            } else
                core->xar[fdst].ar = (core->xar[fsrc].ar | k8);
        }
    }
    return 0;
}
static int exec_bcc_l16_cond(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int cond = BITS(36, 38);
    int src = BITS(32, 34);
    int fs = BITS(35, 35);
    int l16 = fields->l16;
    int cond_flag = check_cond_l4(core, cond, src, fs);

    if (cond_flag == 1)
    {
        if ((l16 >> 15) & 0x1)
        {
            l16 = 0xffff - l16 + 1;
            core->pc -= l16;
        } else
            core->pc += l16;
    }

    core->pc = core->pc + 4;
    return 0;
}
static int exec_psh_src(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int src = fields->src;
    int fs = fields->f;

    core->pc += 2;
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }
    core->sp -= 1;
    if (fs == 0)
    {
        if (src < 4)
            c55_memory_write(core, core->sp, core->ac[src] & 0xffff);
        else
            c55_memory_write(core, core->sp, core->t[src - 4]);
    } else
    {
        c55_memory_write(core, core->sp, core->xar[src].ar);
    }

    return 0;
}
static int exec_pop_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int dst = fields->src;
    int fd = fields->f;
    int eq = fields->eq;

    core->pc += 2;
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }
    if (fd == 0)
    {
        if (dst < 4)
            core->ac[dst] = (core->ac[dst] & 0xFFFFFF0000) | c55_memory_read(core, core->sp);
        else
            core->t[dst - 4] = c55_memory_read(core, core->sp);
    } else
    {
        core->xar[dst].ar = c55_memory_read(core, core->sp);
    }
    core->sp += 1;

    return 0;
}
static int exec_xor_src_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int fsrc = BITS(36, 38);
    int fdst = BITS(32, 34);
    int fs = BITS(39, 39);
    int fd = BITS(35, 35);

    if (fd == 1)
    {
        if (fs == 1)
            core->xar[fdst].ar ^= core->xar[fsrc].ar;
        else
        {
            if (fsrc > 3)
            {
                core->xar[fdst].ar ^= core->t[fsrc - 4];
            } else
                core->xar[fdst].ar ^= core->ac[fsrc];
        }
    } else
    {
        if (fdst > 3)
        {
            if (fs == 1)
            {
                core->t[fdst - 4] ^= core->xar[fsrc].ar;
            } else
            {
                if (fsrc > 3)
                {
                    core->t[fdst - 4] ^= core->t[fsrc - 4];
                } else
                    core->t[fdst - 4] ^= core->ac[fsrc];
            }
        } else
        {
            if (fs == 1)
            {
                core->ac[fdst] ^= core->xar[fsrc].ar;
            } else
            {
                if (fsrc > 3)
                {
                    core->ac[fdst] ^= core->t[fsrc - 4];
                } else
                    core->ac[fdst] ^= core->ac[fsrc];
            }
        }
    }

    core->pc += 2;
    return 0;
}
static int exec_and_src_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int fsrc = BITS(36, 38);
    int fdst = BITS(32, 34);
    int fs = BITS(39, 39);
    int fd = BITS(35, 35);
    uint16_t xar_src = core->xar[fsrc].ar;
    uint16_t t_src = core->t[fsrc - 4];
    uint64_t ac_src = core->ac[fsrc];

    core->pc += 2;

    //next instruction
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }

    if (fd == 1)
    {
        if (fs == 1)
            core->xar[fdst].ar &= xar_src;
        else
        {
            if (fsrc > 3)
            {
                core->xar[fdst].ar &= t_src;
            } else
                core->xar[fdst].ar &= ac_src;
        }
    } else
    {
        if (fdst > 3)
        {
            if (fs == 1)
            {
                core->t[fdst - 4] &= xar_src;
            } else
            {
                if (fsrc > 3)
                {
                    core->t[fdst - 4] &= t_src;
                } else
                    core->t[fdst - 4] &= ac_src;
            }
        } else
        {
            if (fs == 1)
            {
                core->ac[fdst] &= xar_src;
            } else
            {
                if (fsrc > 3)
                {
                    core->ac[fdst] &= t_src;
                } else
                    core->ac[fdst] &= ac_src;
            }
        }
    }

    return 0;
}
static int exec_b_l16(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int l16 = fields->bk16;

    core->pc += 3;

    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }
    core->pc += l16;
    return 0;
}
static int exec_mov_src_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int src = fields->src;
    int dst = fields->dst;
    int fs = BITS(39, 39);
    int fd = BITS(35, 35);
    int64_t result = 0;
    uint16_t xar_src = core->xar[src].ar;
    uint64_t ac_src = core->ac[src];
    uint16_t t_src = core->t[src - 4];

    core->pc += 2;
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }

    if (fs == 1)
    {
        if (fd == 1)
        {
            core->xar[dst].ar = xar_src;
        } else
        {
            if (dst < 4)
            {
                if ((core->xar[src].ar >> 15) & 0x1)
                    result = 0xFFFFFF0000 | xar_src;
                else
                    result = (int64_t) xar_src;
                core->ac[dst] = result;
            } else
                core->t[dst - 4] = xar_src;
        }
    } else
    {
        if (fd == 1)
        {
            if (src < 4)
            {
                core->xar[dst].ar = ac_src;
            } else
                core->xar[dst].ar = t_src;
        } else
        {
            if (dst < 4)
            {
                if (src < 4)
                {
                    core->ac[dst] = ac_src;
                } else
                {
                    if ((t_src >> 15) & 0x1)
                        result = 0xFFFFFF0000 | t_src;
                    else
                        result = (int64_t) t_src;
                    core->ac[dst] = result;
                }
            } else
            {
                if (src < 4)
                {
                    core->t[dst - 4] = ac_src;
                } else
                    core->t[dst - 4] = t_src;
            }
        }
    }
    //Overflow conditions
    /*
     * if (fd == 0 && dst < 4)
     * {
     * if ((core->ac[dst] >> 32) & 0xFF){
     * switch(dst){
     * case 0:
     * SET(core->st0_55, ACOV0);
     * break;
     * case 1:
     * SET(core->st0_55, ACOV1);
     * break;
     * case 2:
     * SET(core->st0_55, ACOV2);
     * break;
     * case 3:
     * SET(core->st0_55, ACOV3);
     * break;
     * default:
     * printf("It's error in %s, pc :%x\n", __FUNCTION__, core->pc);
     * skyeye_exit(-1);
     * }
     * }
     * }
     */
    while (core->rptc)
    {
        core->rptc--;
        return 0;
    }
    return 0;
}
static int exec_mpym_smem_tx_acy(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    //uint16_t mem16_val = 0;
    int src = BITS(24, 25);
    int dst = BITS(28, 29);
    int u_flag = BITS(27, 27);
    int m40 = bit(core->st1_55, M40);
    int addr_mode = BITS(32, 32);
    int set_flag = 0;
    uint16_t mem16_val = 0;
    uint64_t result;

    //mem16_val = read_uns_smem_data(core, insn);
    if (addr_mode)
        mem16_val = read_val_from_indirect_smem_address(core, insn);
    else
        mem16_val = read_val_from_direct_smem_address(core, insn);

    if (u_flag)
    {
        result = core->t[src] * mem16_val;
    } else
        result = (int16_t) core->t[src] * mem16_val;

    /* Overflow conditions */
    if (m40 == 0)
    {
        if ((result >> 32) & 0x1)
        {
            set_flag = 1;
        }
    } else
    {
        if ((result >> 40) & 0x1)
        {
            set_flag = 1;
        }
    }
    if (set_flag)
    {
        switch (dst)
        {
            case 0:
                SET(core->st0_55, ACOV0);
                break;
            case 1:
                SET(core->st0_55, ACOV1);
                break;
            case 2:
                SET(core->st0_55, ACOV2);
                break;
            case 3:
                SET(core->st0_55, ACOV3);
                break;
            default:
                printf("It's error in %s, pc:%x\n", __FUNCTION__, core->pc);
                skyeye_exit(-1);
        }
    }

    if (u_flag)
        core->ac[dst] = result & 0xFFFFFFFF;
    else
        core->ac[dst] = result & 0xFFFFFFFFFF;

    core->pc += 3;
    return 0;
}
static int exec_mpym_smem_acx_acy(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int smem = fields->addr;
    int acx = fields->acx;
    int acy = fields->acy;
    int addr_mode = BITS(32, 32);
    uint16_t mem16_val = 0;

    //mem16_val = read_uns_smem_data(core, insn);
    if (addr_mode)
        mem16_val = read_val_from_indirect_smem_address(core, insn);
    else
        mem16_val = read_val_from_direct_smem_address(core, insn);

    int result = (core->ac[acx] >> 16) * mem16_val;

    core->ac[acy] = result;

    core->pc += 3;
    return 0;
}
static int exec_mac_acy_tx_acx_acy(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int tsrc = BITS(34, 35);
    int asrc = BITS(36, 37);
    int adst = BITS(38, 39);

    core->ac[adst] = (core->ac[adst] >> 16) * core->t[tsrc] + core->ac[asrc];
    //need Overflow_check
    while (core->rptc)
    {
        core->rptc--;
        return;
    }

    core->pc += 2;
    return 0;
}
static int exec_mov_smem_shift_acx(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int shift = fields->shift;
    int acx = BITS(20, 21);
    int mem16_val = 0;
    int addr_mode = BITS(32, 32);
    int parallel_op = BITS(8, 15);

    //      mem16_val = read_imm_val_from_smem(core, insn);
    if (addr_mode)
        mem16_val = read_val_from_indirect_smem_address(core, insn);
    else
        mem16_val = read_val_from_direct_smem_address(core, insn);

    core->pc += 4;

    if (parallel_op == 0x99)
    {
        core->pc += 1;
    } else
    {
        if (No_exp != check_parallel_insn(conf_obj, core))
        {
            printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
            skyeye_exit(-1);
        }
    }
    core->ac[acx] = mem16_val << shift;

    return 0;
}
static int exec_sub_k4_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int dst = fields->dst;
    int fd = fields->m1;
    int k4 = fields->k4;
    int m40 = bit(core->st1_55, M40);

    if (fd == 1)
    {
        core->xar[dst].ar -= k4;
    } else
    {
        if (dst < 4)
        {
            int x = bit(core->ac[dst], 31);
            int y = bit(k4, 31);

            core->ac[dst] -= k4;
            int z = bit(core->ac[dst], 31);

            /* Carry conditions */
            if (m40 == 0)
            {
                if ((x && y && z == 0) || (!x && !y && z) || (!x && y && z == 0) || (x && !y && z == 0))
                    CLEAR(core->st0_55, CARRY);
                else
                {
                    SET(core->st0_55, CARRY);
                    /* Overflow conditions */
                    switch (dst)
                    {
                        case 0:
                            SET(core->st0_55, ACOV0);
                            break;
                        case 1:
                            SET(core->st0_55, ACOV1);
                            break;
                        case 2:
                            SET(core->st0_55, ACOV2);
                            break;
                        case 3:
                            SET(core->st0_55, ACOV3);
                            break;
                        default:
                            printf("It's error in %s, pc:%x\n", __FUNCTION__, core->pc);
                            skyeye_exit(-1);
                    }
                }
            }
        } else
            core->t[dst - 4] -= k4;
    }
    core->pc += 2;
    return 0;
}
static int exec_mov_smem_acx(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int dst = BITS(28, 29);
    int addr_mode = BITS(32, 32);
    uint16_t mem16_val = 0;

    if (addr_mode)
        mem16_val = read_val_from_indirect_smem_address(core, insn);
    else
        mem16_val = read_val_from_direct_smem_address(core, insn);

    core->ac[dst] = mem16_val & 0xFFFF;
    core->pc += 3;
    //core->ac[dst] = read_uns_smem_data(core, insn) & 0xFFFF;

    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }
    return 0;
}
static int exec_mov_tax_reg(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int fs = fields->f;
    int fsrc = fields->src;
    int fdst = BITS(32, 35);
    int *reg = NULL;

    if (fdst == 0xe)
    {
        reg = &core->brc0;
    } else if (fdst == 0xd)
    {
        reg = &core->brc1;
    } else if (fdst == 0xa)
    {
        reg = &core->cdp;
    } else if (fdst == 0xc)
    {
        reg = &core->csr;
    } else if (fdst == 0x8)
    {
        reg = &core->sp;
    } else if (fdst == 0x9)
    {
        reg = &core->ssp;
    }

    if (fs == 1)
    {
        *reg = core->xar[fsrc].ar;
    } else
    {
        if (fsrc > 3)
        {
            *reg = core->t[fsrc - 4];
        } else
            *reg = core->ac[fsrc];
    }

    core->pc += 2;
    return 0;
}
static int exec_or_acx_shift_acy(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int shift = BITS(24, 29);
    int fsrc = BITS(36, 37);
    int fdst = BITS(38, 39);
    int m40 = bit(core->st1_55, M40);
    uint64_t temp_src = core->ac[fsrc];

    core->pc += 3;
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }

    if (shift <= 32)
    {
        core->ac[fdst] |= (temp_src << shift);
    } else
        core->ac[fdst] |= (temp_src >> (0x40 - shift));

    if (m40 == 1)
    {
        core->ac[fdst] &= 0xFFFFFFFFFF;
    } else
    {
        core->ac[fdst] &= 0xFFFFFFFF;
    }
    return 0;
}
static int exec_and_k8_src_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k8 = fields->k8;
    int fs = BITS(27, 27);
    int fd = fields->x;
    int fsrc = fields->acx;
    int fdst = fields->acy;

    if (fd == 1)
    {
        if (fs == 1)
            core->xar[fdst].ar = core->xar[fsrc].ar & k8;
        else
        {
            if (fsrc > 3)
            {
                core->xar[fdst].ar = core->t[fsrc - 4] & k8;
            } else
                core->xar[fdst].ar = core->ac[fsrc] & k8;
        }
    } else
    {
        if (fdst > 3)
        {
            if (fs == 1)
            {
                core->t[fdst - 4] = core->xar[fsrc].ar & k8;
            } else
            {
                if (fsrc > 3)
                {
                    core->t[fdst - 4] = core->t[fsrc - 4] & k8;
                } else
                    core->t[fdst - 4] = core->ac[fsrc] & k8;
            }
        } else
        {
            if (fs == 1)
            {
                core->ac[fdst] = core->xar[fsrc].ar & k8;
            } else
            {
                if (fsrc > 3)
                {
                    core->ac[fdst] = core->t[fsrc - 4] & k8;
                } else
                    core->ac[fdst] = core->ac[fsrc] & k8;
            }
        }
    }

    core->pc += 3;
    return 0;
}
static int exec_mov_xsrc_xdst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int fs = fields->f;
    int fd = fields->m1;
    int xsrc = fields->src;
    int xdst = fields->dst;
    int src_reg = 0;

    if (fs == 0)
    {
        switch (xsrc)
        {
            case 0:
            case 1:
            case 2:
            case 3:
                src_reg = core->ac[xsrc];
                break;
            case 4:
                src_reg = core->xsp;
                break;
            case 5:
                src_reg = core->xssp;
                break;
            case 6:
                src_reg = core->xdp;
                break;
            case 7:
                src_reg = core->xcdp;
                break;
            default:
                printf("Decode error instruction at %s, pc:%x\n", __FUNCTION__, core->pc);
                skyeye_exit(-1);
        }
    } else
    {
        src_reg = core->xar[xsrc].xar;
    }
    if (fd == 0)
    {
        switch (xdst)
        {
            case 0:
            case 1:
            case 2:
            case 3:
                core->ac[xdst] = src_reg;
                break;
            case 4:
                core->xsp = src_reg;
                break;
            case 5:
                core->xssp = src_reg;
                break;
            case 6:
                core->xdp = src_reg;
                break;
            case 7:
                core->xcdp = src_reg;
                break;
            default:
                printf("Decode error instruction at %s, pc:%x\n", __FUNCTION__, core->pc);
                skyeye_exit(-1);
        }
    } else
    {
        core->xar[xdst].xar = src_reg;
    }

    core->pc += 2;
    return 0;
}
static int exec_rpt_csr(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    core->rptc = core->csr;

    core->pc += 2;
    return 0;
}

int16_t read_xmem_data(int xmem, int src, c55_core_t * core)
{
    int16_t ret_data;

    if (xmem == 0x0)
    {
        ret_data = c55_memory_read(core, core->xar[src].ar);
    } else if (xmem == 0x1)
    {
        ret_data = c55_memory_read(core, core->xar[src].ar);
        core->xar[src].ar += 1;
    } else if (xmem == 0x7)
    {
        ret_data = c55_memory_read(core, (core->t[0] + core->xar[src].ar));
    } else
    {
        printf("In %s, read xmem error.\n", __FUNCTION__);
        skyeye_exit(-1);
    }
    return ret_data;
}
void write_ymem_data(int ymem, int dst, c55_core_t * core, int16_t xmem_data)
{
    if (ymem == 0x0)
    {
        c55_memory_write(core, core->xar[dst].ar, xmem_data);
    } else if (ymem == 0x1)
    {
        c55_memory_write(core, core->xar[dst].ar, xmem_data);
        core->xar[dst].ar += 1;
    } else if (ymem == 0x7)
    {
        c55_memory_write(core, (core->t[0] + core->xar[dst].ar), xmem_data);
    } else
    {
        printf("In %s, write xmem error.\n", __FUNCTION__);
        skyeye_exit(-1);
    }
    return;
}
static int exec_mov_xmem_ymem(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int src = fields->lmem;
    int dst = fields->ymem;
    int xmem = BITS(34, 36);
    int ymem = BITS(28, 30);
    int parallel_op = BITS(16, 23);
    int16_t xmem_data, ymem_data;

    xmem_data = read_xmem_data(xmem, src, core);
    write_ymem_data(ymem, dst, core, xmem_data);

    while (core->rptc)
    {
        core->rptc--;
        return 0;
    }
    if (parallel_op == 0x9a)
        core->pc += 1;
    core->pc += 3;
    return 0;
}

uint32_t read_xlmem_data(int xmem, int src, c55_core_t * core)
{
    uint32_t ret_data = 0;
    uint16_t ret_upper, ret_lower;

    if (xmem == 0x0)
    {
        ret_upper = c55_memory_read(core, core->xar[src].ar);
        ret_lower = c55_memory_read(core, (core->xar[src].ar + 1));
    } else if (xmem == 0x1)
    {
        ret_upper = c55_memory_read(core, core->xar[src].ar);
        ret_lower = c55_memory_read(core, (core->xar[src].ar + 1));
        core->xar[src].ar += 2;
    } else if (xmem == 0x7)
    {
        ret_upper = c55_memory_read(core, (core->t[0] + core->xar[src].ar));
        ret_lower = c55_memory_read(core, (core->t[0] + core->xar[src].ar + 1));
    } else
    {
        printf("In %s, read xmem error.\n", __FUNCTION__);
        skyeye_exit(-1);
    }
    ret_data = (ret_upper << 16) | ret_lower;
    return ret_data;
}
void write_ylmem_data(int ymem, int dst, c55_core_t * core, uint32_t xmem_data)
{
    if (ymem == 0x0)
    {
        c55_memory_write(core, core->xar[dst].ar, xmem_data >> 16);
        c55_memory_write(core, (core->xar[dst].ar + 1), xmem_data & 0xFFFF);
    } else if (ymem == 0x1)
    {
        c55_memory_write(core, core->xar[dst].ar, xmem_data >> 16);
        c55_memory_write(core, (core->xar[dst].ar + 1), xmem_data & 0xFFFF);
        core->xar[dst].ar += 2;
    } else if (ymem == 0x7)
    {
        c55_memory_write(core, (core->t[0] + core->xar[dst].ar), xmem_data >> 16);
        c55_memory_write(core, (core->t[0] + core->xar[dst].ar + 1), xmem_data & 0xFFFF);
    } else
    {
        printf("In %s, write xmem error.\n", __FUNCTION__);
        skyeye_exit(-1);
    }
    return;
}
static int exec_mov_dblxmem_dblymem(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                    opcode_fields_t * fields)
{
    int src = BITS(37, 39);
    int dst = BITS(31, 33);
    int xmem = BITS(34, 36);
    int ymem = BITS(28, 30);
    uint32_t xmem_data = 0;

    xmem_data = read_xlmem_data(xmem, src, core);
    write_ylmem_data(ymem, dst, core, xmem_data);

    while (core->rptc)
    {
        core->rptc--;
        return 0;
    }
    core->pc += 3;

    return 0;
}
static int exec_b_l7(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int l7 = BITS(32, 39);
    int m = BITS(36, 38);

    core->pc += 2;
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }

    if (m > 3)
    {
        l7 = 0x80 - l7;
        core->pc -= l7;
    } else
    {
        core->pc += l7;
    }
    return 0;
}
static int exec_sfts_acx_shift_acy(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int fsrc = BITS(36, 37);
    int fdst = BITS(38, 39);
    int m40 = bit(core->st1_55, M40);
    int sxmd = bit(core->st1_55, SXMD);
    int shiftw = fields->shift;
    uint64_t src_val = core->ac[fsrc];

    if (m40 == 0)
        src_val = src_val & 0xFFFFFFFF;

    if (shiftw <= 32)
        core->ac[fdst] = src_val << shiftw;
    else
        core->ac[fdst] = src_val >> (0x40 - shiftw);

    if (m40 == 0)
    {
        /*Overflow conditions */
        if ((core->ac[fdst] >> 32) & 0xff)
        {
            switch (fdst)
            {
                case 0:
                    SET(core->st0_55, ACOV0);
                    break;
                case 1:
                    SET(core->st0_55, ACOV1);
                    break;
                case 2:
                    SET(core->st0_55, ACOV2);
                    break;
                case 3:
                    SET(core->st0_55, ACOV3);
                    break;
                default:
                    printf("Decode error instruction at %s, pc:%x\n", __FUNCTION__, core->pc);
                    skyeye_exit(-1);
            }
        }
    }
    core->pc += 3;
    return 0;
}
static int exec_add_src_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int fsrc = fields->src;
    int fdst = fields->dst;
    int fd = BITS(35, 35);
    int fs = BITS(39, 39);

    if (fd == 1)
    {
        if (fs == 1)
            core->xar[fdst].ar += core->xar[fsrc].ar;
        else
        {
            if (fsrc > 3)
            {
                core->xar[fdst].ar += core->t[fsrc - 4];
            } else
                core->xar[fdst].ar += core->ac[fsrc];
        }
    } else
    {
        if (fdst > 3)
        {
            if (fs == 1)
            {
                core->t[fdst - 4] += core->xar[fsrc].ar;
            } else
            {
                if (fsrc > 3)
                {
                    core->t[fdst - 4] += core->t[fsrc - 4];
                } else
                    core->t[fdst - 4] += core->ac[fsrc];
            }
        } else
        {
            if (fs == 1)
            {
                core->ac[fdst] += core->xar[fsrc].ar;
            } else
            {
                if (fsrc > 3)
                {
                    core->ac[fdst] += core->t[fsrc - 4];
                } else
                {
                    int x = bit(core->ac[fsrc], 31);
                    int y = bit(core->ac[fdst], 31);

                    core->ac[fdst] += core->ac[fsrc];
                    int z = bit(core->ac[fdst], 31);

                    if (bit(core->st1_55, M40) == 0)
                    {
                        /*Carry conditions */
                        if ((x && y && z == 0) || (!x && !y && z) || (!x && y && z == 0) || (x && !y && z == 0))
                        {
                            SET(core->st0_55, CARRY);
                        } else
                            CLEAR(core->st0_55, CARRY);
                    }
                }
            }
        }
    }
    core->pc += 2;
    return 0;
}

static int exec_mov_k16_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k16 = BITS(24, 39);
    int eq = fields->fd;
    int dst = BITS(20, 22);

    core->pc += 4;
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }
    if (core->exec_enable)
    {
        if (eq == 0)
        {
            if (dst < 4)
            {
                if ((k16 >> 15) & 0x1)
                    core->ac[dst] = k16 | 0xFFFFFF0000;
                else
                    core->ac[dst] = k16;
            } else
            {
                core->t[dst - 4] = k16;
            }
        } else
            core->xar[dst].ar = k16;
    } else
    {
        core->exec_enable = 1;
        SET(core->st2_55, 14);
    }
    return 0;
}

static int exec_sub_src_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int src = BITS(36, 38);
    int dst = BITS(32, 34);
    int fs = BITS(39, 39);
    int fd = BITS(35, 35);
    int flag = 1;

    //check_next_insn is xccpart cond  

    insn = (insn >> 16) & 0xFFFF;
    int insn_op = BITS(7, 15);

    if (insn_op == 0x13d)
    {
        int cond = BITS(4, 6);
        int src = BITS(0, 2);
        int fs = BITS(3, 3);

        int cond_flag = check_cond_l4(core, cond, src, fs);

        core->pc += 2;
        if (cond_flag == 0)
            flag = 0;
    }

    core->pc += 2;
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }

    if (flag)
    {
        if (fs == 1)
        {
            if (fd == 1)
            {
                core->xar[dst].ar -= core->xar[src].ar;
            } else
            {
                if (dst < 4)
                {
                    //      if ((core->ac[dst] & 0xFFFF) <= core->xar[src].ar)
                    if (bit((core->ac[dst] - core->xar[src].ar), 31) == 0 || core->ac[dst] == core->xar[src].ar)
                        SET(core->st0_55, CARRY);
                    else
                        CLEAR(core->st0_55, CARRY);

                    core->ac[dst] -= core->xar[src].ar;
                } else
                    core->t[dst - 4] -= core->xar[src].ar;
            }
        } else
        {
            if (fd == 1)
            {
                if (src < 4)
                {
                    core->xar[dst].ar -= core->ac[src];
                } else
                    core->xar[dst].ar -= core->t[src - 4];
            } else
            {
                if (dst < 4)
                {
                    if (src < 4)
                    {
                        if (bit((core->ac[dst] - core->ac[src]), 31) == 0 || core->ac[dst] == core->ac[src])
                        {
                            SET(core->st0_55, CARRY);
                        } else
                        {
                            CLEAR(core->st0_55, CARRY);
                        }

                        core->ac[dst] -= core->ac[src];
                    } else
                    {
                        if (bit((core->ac[dst] - core->t[src - 4]), 31) == 0 || core->ac[dst] == core->t[src - 4])
                            SET(core->st0_55, CARRY);
                        else
                            CLEAR(core->st0_55, CARRY);

                        core->ac[dst] -= core->t[src - 4];
                    }
                } else
                {
                    if (src < 4)
                    {
                        core->t[dst - 4] -= core->ac[src];
                    } else
                        core->t[dst - 4] -= core->t[src - 4];
                }
            }
        }
    }

    return 0;
}
static int exec_neg_src_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int src = fields->src;
    int dst = fields->dst;
    int fs = fields->f;
    int fd = fields->m1;

    if (fs == 1)
    {
        if (fd == 1)
        {
            core->xar[dst].ar = -core->xar[src].ar;
        } else
        {
            if (dst < 4)
            {
                core->ac[dst] = -core->xar[src].ar;
                if (core->xar[src].ar == 0)
                {
                    SET(core->st0_55, CARRY);
                } else
                    CLEAR(core->st0_55, CARRY);
            } else
                core->t[dst - 4] = -core->xar[src].ar;
        }
    } else
    {
        if (fd == 1)
        {
            if (src < 4)
            {
                core->xar[dst].ar = -core->ac[src];
                if (core->ac[src] == 0)
                {
                    SET(core->st0_55, CARRY);
                } else
                    CLEAR(core->st0_55, CARRY);
            } else
            {
                core->xar[dst].ar = -core->t[src - 4];
            }
        } else
        {
            if (dst < 4)
            {
                if (src < 4)
                {
                    core->ac[dst] = -core->ac[src];
                    if (core->ac[src] == 0)
                    {
                        SET(core->st0_55, CARRY);
                    } else
                        CLEAR(core->st0_55, CARRY);
                } else
                {
                    core->ac[dst] = -core->t[src - 4];
                    if (core->t[src - 4] == 0)
                    {
                        SET(core->st0_55, CARRY);
                    } else
                        CLEAR(core->st0_55, CARRY);
                }
            } else
            {
                if (src < 4)
                {
                    core->t[dst - 4] = -core->ac[src];
                    if (core->ac[src] == 0)
                    {
                        SET(core->st0_55, CARRY);
                    } else
                        CLEAR(core->st0_55, CARRY);
                } else
                {
                    core->t[dst - 4] = -core->t[src - 4];
                }
            }
        }
    }

    core->pc += 2;
    return 0;
}
static int exec_sfts_acx_tx_acy(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int fsrc = BITS(36, 37);
    int fdst = BITS(38, 39);
    int m40 = bit(core->st1_55, M40);
    int sxmd = bit(core->st1_55, SXMD);
    int src = BITS(34, 35);
    int shiftw;

    shiftw = core->t[src];
    if (shiftw < 0x7fff)
        core->ac[fdst] = core->ac[fsrc] << shiftw;
    else
        core->ac[fdst] = core->ac[fsrc] >> (-shiftw);

    /*Overflow conditions */
    if (m40 == 0)
    {
        /*Overflow conditions */
        if ((core->ac[fdst] >> 32) & 0xff)
        {
            switch (fdst)
            {
                case 0:
                    SET(core->st0_55, ACOV0);
                    break;
                case 1:
                    SET(core->st0_55, ACOV1);
                    break;
                case 2:
                    SET(core->st0_55, ACOV2);
                    break;
                case 3:
                    SET(core->st0_55, ACOV3);
                    break;
                default:
                    printf("It's error in %s, pc:%x\n", __FUNCTION__, core->pc);
                    skyeye_exit(-1);
            }
        }
    }
    //      Overflow_check(core, insn, fdst);
    while (core->rptc)
    {
        core->rptc--;
        return 0;
    }

    core->pc += 2;
    return 0;
}
static int exec_sftl_acx_tx_acy(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int fsrc = BITS(36, 37);
    int fdst = BITS(38, 39);
    int m40 = bit(core->st1_55, M40);
    int sxmd = bit(core->st1_55, SXMD);
    int src = BITS(34, 35);
    int shiftw;

    shiftw = core->t[src];
    if ((shiftw >> 15) & 0x1)
    {
        shiftw = (0xFFFF - shiftw + 1);
        /* Carry conditions */
        if ((core->ac[fsrc] >> (shiftw - 1)) & 0x1)
            SET(core->st0_55, CARRY);
        else
            CLEAR(core->st0_55, CARRY);
        core->ac[fdst] = core->ac[fsrc] >> shiftw;
    } else
    {
        if ((core->ac[fsrc] >> (32 - shiftw)) & 0x1)
            SET(core->st0_55, CARRY);
        else
            CLEAR(core->st0_55, CARRY);
        core->ac[fdst] = core->ac[fsrc] << shiftw;
    }

    if (m40 == 0)
    {
        /*Overflow conditions */
        if ((core->ac[fdst] >> 32) & 0xff)
        {
            switch (fdst)
            {
                case 0:
                    SET(core->st0_55, ACOV0);
                    break;
                case 1:
                    SET(core->st0_55, ACOV1);
                    break;
                case 2:
                    SET(core->st0_55, ACOV2);
                    break;
                case 3:
                    SET(core->st0_55, ACOV3);
                    break;
                default:
                    printf("It's error in %s, pc:%x\n", __FUNCTION__, core->pc);
                    skyeye_exit(-1);
            }
        }
    }

    if (m40 == 1)
    {
        core->ac[fdst] &= 0xFFFFFFFFFF;
    } else
    {
        core->ac[fdst] &= 0xFFFFFFFF;
    }

    while (core->rptc)
    {
        core->rptc--;
        return 0;
    }
    core->pc += 2;
    return 0;
}
static int exec_sftl_dst_left(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int fdst = BITS(36, 38);
    int fd = BITS(39, 39);
    int m40 = bit(core->st1_55, M40);
    int x, y = 0;

    core->pc += 2;
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }

    if (fd == 0)
    {
        if (fdst < 4)
        {
            core->ac[fdst] = core->ac[fdst] << 1;
            if (m40 == 1)
            {
                /*Carry conditions */
                if ((core->ac[fdst] >> 40) & 0x1)
                {
                    SET(core->st0_55, CARRY);
                } else
                    CLEAR(core->st0_55, CARRY);
            } else
            {
                /*Carry conditions */
                if ((core->ac[fdst] >> 32) & 0x1)
                {
                    SET(core->st0_55, CARRY);
                } else
                    CLEAR(core->st0_55, CARRY);
            }
            /*Overflow conditions */
        } else
        {
            x = bit(core->t[fdst], 15);
            core->t[fdst - 4] = core->t[fdst] << 1;
            y = bit(core->t[fdst], 15);
            if ((!x && y) || (x && !y))
                SET(core->st0_55, CARRY);
            else
                CLEAR(core->st0_55, CARRY);
        }
    } else
    {
        x = bit(core->xar[fdst].ar, 15);
        core->xar[fdst].ar = core->xar[fdst].ar << 1;
        y = bit(core->xar[fdst].ar, 15);
        if ((!x && y) || (x && !y))
            SET(core->st0_55, CARRY);
        else
            CLEAR(core->st0_55, CARRY);
    }
    while (core->rptc)
    {
        core->rptc--;
        return 0;
    }

    return 0;
}
static int exec_sftl_dst_right(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int fdst = BITS(36, 38);
    int fd = BITS(39, 39);

    if (fd == 0)
    {
        if (fdst < 4)
        {
            /*Carry conditions */
            if (core->ac[fdst] & 0x1)
            {
                SET(core->st0_55, CARRY);
            } else
                CLEAR(core->st0_55, CARRY);

            core->ac[fdst] = (core->ac[fdst] & 0xFFFFFFFF) >> 1;
            /*Overflow conditions */
        } else
        {
            /*Carry conditions */
            if (core->t[fdst - 4] & 0x1)
            {
                SET(core->st0_55, CARRY);
            } else
                CLEAR(core->st0_55, CARRY);

            core->t[fdst - 4] = core->t[fdst] >> 1;
        }
    } else
    {
        /*Carry conditions */
        if (core->xar[fdst].ar & 0x1)
        {
            SET(core->st0_55, CARRY);
        } else
            CLEAR(core->st0_55, CARRY);

        core->xar[fdst].ar = core->xar[fdst].ar >> 1;
    }

    core->pc += 2;
    return 0;
}
static int exec_or_src_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int src = fields->src;
    int dst = fields->dst;
    int fs = fields->f;
    int fd = fields->m1;
    uint16_t xar_src = core->xar[src].ar;
    uint64_t ac_src = core->ac[src];
    uint16_t t_src = core->t[src - 4];

    core->pc += 2;
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }

    if (fs == 1)
    {
        if (fd == 1)
        {
            core->xar[dst].ar |= xar_src;
        } else
        {
            if (dst < 4)
            {
                core->ac[dst] |= xar_src;
            } else
                core->t[dst - 4] |= xar_src;
        }
    } else
    {
        if (fd == 1)
        {
            if (src < 4)
            {
                core->xar[dst].ar |= ac_src;
            } else
                core->xar[dst].ar |= t_src;
        } else
        {
            if (dst < 4)
            {
                if (src < 4)
                {
                    core->ac[dst] |= ac_src;
                } else
                    core->ac[dst] |= t_src;
            } else
            {
                if (src < 4)
                {
                    core->t[dst - 4] |= ac_src;
                } else
                    core->t[dst - 4] |= t_src;
            }
        }
    }

    return 0;
}
static int exec_add_k4_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int dst = BITS(32, 34);
    int fd = BITS(35, 35);
    int k4 = BITS(36, 39);

    if (fd == 1)
    {
        core->xar[dst].ar += k4;
    } else
    {
        if (dst < 4)
        {
            int x = bit(core->ac[dst], 31);

            core->ac[dst] += k4;
            int y = bit(core->ac[dst], 31);

            /* Carry conditions */
            if ((!x && y) || (x && !y))
            {
                SET(core->st0_55, CARRY);
            } else
            {
                CLEAR(core->st0_55, CARRY);
            }
        } else
            core->t[dst - 4] += k4;
    }
    core->pc += 2;
    return 0;
}
static int exec_sfts_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int dst = fields->dst;
    int fd = fields->m1;
    int m = BITS(36, 36);

    if (fd == 1)
    {
        if (m == 0)
            core->xar[dst].ar = (core->xar[dst].ar >> 1);
        else
            core->xar[dst].ar = (core->xar[dst].ar << 1);
    } else
    {
        if (dst < 4)
        {
            if (m == 0)
                core->ac[dst] = (core->ac[dst] >> 1);
            else
                core->ac[dst] = (core->ac[dst] << 1);
        } else
        {
            if (m == 0)
                core->t[dst - 4] = (core->t[dst - 4] >> 1);
            else
                core->t[dst - 4] = (core->t[dst - 4] << 1);
        }
    }

    core->pc += 2;
    return 0;
}
static int exec_sftl_acx_shift_acy(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int fsrc = BITS(36, 37);
    int fdst = BITS(38, 39);
    int m40 = bit(core->st1_55, M40);
    int sxmd = bit(core->st1_55, SXMD);
    int shiftw = fields->shift;
    uint32_t src32_val = core->ac[fsrc];
    uint64_t src40_val = core->ac[fsrc];

    core->pc += 3;
    //next instruction
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }

    if (shiftw <= 32)
    {
        if (m40)
        {
            if ((src40_val >> (40 - shiftw)) & 0x1)
                SET(core->st0_55, CARRY);
            else
                CLEAR(core->st0_55, CARRY);

            core->ac[fdst] = src40_val << shiftw;
        } else
        {
            if ((src32_val >> (32 - shiftw)) & 0x1)
                SET(core->st0_55, CARRY);
            else
                CLEAR(core->st0_55, CARRY);

            core->ac[fdst] = src32_val << shiftw;
        }
    } else
    {
        if (m40)
        {
            if (src40_val & (0x1 << (0x40 - shiftw - 1)))
                SET(core->st0_55, CARRY);
            else
                CLEAR(core->st0_55, CARRY);

            core->ac[fdst] = src40_val >> (0x40 - shiftw);
        } else
        {
            if (src32_val & (0x1 << (0x40 - shiftw - 1)))
                SET(core->st0_55, CARRY);
            else
                CLEAR(core->st0_55, CARRY);

            core->ac[fdst] = src32_val >> (0x40 - shiftw);
        }
    }
    if (shiftw == 0)
    {
        CLEAR(core->st0_55, CARRY);
    }

    if (m40 == 1)
    {
        core->ac[fdst] &= 0xFFFFFFFFFF;
    } else
    {
        core->ac[fdst] &= 0xFFFFFFFF;
    }

    return 0;
}
static int exec_psh_src1_src2(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int src1 = fields->src;
    int src2 = fields->dst;
    int fs = fields->f;
    int fd = fields->m1;

    core->sp -= 2;
    if (fs == 0)
    {
        if (src1 < 4)
        {
            c55_memory_write(core, core->sp, core->ac[src1] & 0xffff);
        } else
        {
            c55_memory_write(core, core->sp, core->t[src1 - 4]);
        }
    } else
    {
        c55_memory_write(core, core->sp, core->xar[src1].ar);
    }

    if (fd == 0)
    {
        if (src2 < 4)
        {
            c55_memory_write(core, (core->sp + 1), core->ac[src2] & 0xffff);
        } else
        {
            c55_memory_write(core, (core->sp + 1), core->t[src2 - 4]);
        }
    } else
    {
        c55_memory_write(core, (core->sp + 1), core->xar[src2].ar);
    }

    core->pc += 2;
    return 0;
}
static int exec_sub_k16_src_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k16 = fields->bk16;
    int src = fields->fdst;
    int dst = fields->fsrc;
    int fs = fields->fs;
    int fd = fields->fd;

    if (fs == 1)
    {
        if (fd == 1)
        {
            core->xar[dst].ar = core->xar[src].ar - k16;;
        } else
        {
            if (dst < 4)
            {
                core->ac[dst] = core->xar[src].ar - k16;;
            } else
                core->t[dst - 4] = core->xar[src].ar - k16;
        }
    } else
    {
        if (fd == 1)
        {
            if (src < 4)
            {
                core->xar[dst].ar = core->ac[src] - k16;
            } else
                core->xar[dst].ar = core->t[src - 4] - k16;
        } else
        {
            if (dst < 4)
            {
                if (src < 4)
                {
                    core->ac[dst] = core->ac[src] - k16;
                } else
                    core->ac[dst] = core->t[src - 4] - k16;
            } else
            {
                if (src < 4)
                {
                    core->t[dst - 4] = core->ac[src] - k16;
                } else
                {
                    if (core->t[src - 4] < k16)
                    {
                        core->t[dst - 4] = k16 - core->t[src - 4] - 1;
                    } else
                        core->t[dst - 4] = core->t[src - 4] - k16;
                }
            }
        }
    }

    core->pc += 4;
    return 0;
}
static int exec_and_k16_src_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k16 = fields->bk16;
    int src = BITS(16, 18);
    int dst = BITS(20, 22);
    int fs = BITS(19, 19);
    int fd = BITS(23, 23);
    int rptc_addr = core->pc;

    core->pc += 4;
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }

    if (fs == 1)
    {
        if (fd == 1)
        {
            core->xar[dst].ar = core->xar[src].ar & k16;;
        } else
        {
            if (dst < 4)
            {
                core->ac[dst] = core->xar[src].ar & k16;;
            } else
                core->t[dst - 4] = core->xar[src].ar & k16;
        }
    } else
    {
        if (fd == 1)
        {
            if (src < 4)
            {
                core->xar[dst].ar = core->ac[src] & k16;
            } else
                core->xar[dst].ar = core->t[src - 4] & k16;
        } else
        {
            if (dst < 4)
            {
                if (src < 4)
                {
                    core->ac[dst] = core->ac[src] & k16;
                } else
                    core->ac[dst] = core->t[src - 4] & k16;
            } else
            {
                if (src < 4)
                {
                    core->t[dst - 4] = core->ac[src] & k16;
                } else
                    core->t[dst - 4] = core->t[src - 4] & k16;
            }
        }
    }
    while (core->rptc)
    {
        core->rptc--;
        core->pc = rptc_addr;
        return 0;
    }

    return 0;
}
static int exec_add_k16_src_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k16 = fields->bk16;
    int src = fields->fdst;
    int dst = fields->fsrc;
    int fs = fields->fs;
    int fd = fields->fd;
    int m40 = bit(core->st1_55, M40);

    if (fs == 1)
    {
        if (fd == 1)
        {
            core->xar[dst].ar = core->xar[src].ar + k16;;
        } else
        {
            if (dst < 4)
            {
                core->ac[dst] = core->xar[src].ar + k16;;
            } else
                core->t[dst - 4] = core->xar[src].ar + k16;
        }
    } else
    {
        if (fd == 1)
        {
            if (src < 4)
            {
                core->xar[dst].ar = core->ac[src] + k16;
            } else
                core->xar[dst].ar = core->t[src - 4] + k16;
        } else
        {
            if (dst < 4)
            {
                if (src < 4)
                {
                    int x = bit(core->ac[src], 31);
                    int y = bit(k16, 31);

                    core->ac[dst] = core->ac[src] + k16;
                    int z = bit(core->ac[dst], 31);

                    /* Carry conditions */
                    if (m40 == 0)
                    {
                        //if ((x && y && z == 0) || (!x && !y && z)){
                        if ((x && y && z == 0) || (!x && !y && z) || (!x && y && z == 0) || (x && !y && z == 0))
                        {
                            SET(core->st0_55, CARRY);
                            /* Overflow conditions */
                            switch (dst)
                            {
                                case 0:
                                    SET(core->st0_55, ACOV0);
                                    break;
                                case 1:
                                    SET(core->st0_55, ACOV1);
                                    break;
                                case 2:
                                    SET(core->st0_55, ACOV2);
                                    break;
                                case 3:
                                    SET(core->st0_55, ACOV3);
                                    break;
                                default:
                                    printf("It's error in %s, pc:%x\n", __FUNCTION__, core->pc);
                                    skyeye_exit(-1);
                            }
                        } else
                            CLEAR(core->st0_55, CARRY);
                    }
                } else
                    core->ac[dst] = core->t[src - 4] + k16;
            } else
            {
                if (src < 4)
                {
                    core->t[dst - 4] = core->ac[src] + k16;
                } else
                    core->t[dst - 4] = core->t[src - 4] + k16;
            }
        }
    }

    while (core->rptc)
    {
        core->rptc--;
        return 0;
    }

    core->pc += 4;
    return 0;
}
static int exec_mov_src_smem(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int src = BITS(40, 42);
    int fs = BITS(43, 43);
    int addr_mode = BITS(32, 32);
    int parallel_op = BITS(24, 31);
    uint16_t mem16_val = 0;

    if (fs == 1)
    {
        mem16_val = core->xar[src].ar;
    } else
    {
        if (src < 4)
            mem16_val = core->ac[src] & 0xFFFF;
        else
            mem16_val = core->t[src - 4];
    }

    if (addr_mode)
        write_val_to_indirect_smem_address(core, insn, mem16_val);
    else
        write_val_to_direct_smem_address(core, insn, mem16_val);

    if (parallel_op == 0x9a)
    {
        core->pc += 1;
    }

    core->pc += 2;
    return 0;
}
static int exec_pop_dst1_dst2(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int dst1 = fields->src;
    int dst2 = fields->dst;
    int fs = fields->f;
    int fd = fields->m1;

    core->pc += 2;
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }
    if (fs == 0)
    {
        if (dst1 < 4)
        {
            core->ac[dst1] = c55_memory_read(core, core->sp) & 0xffff;
        } else
        {
            core->t[dst1 - 4] = c55_memory_read(core, core->sp);
        }
    } else
    {
        core->xar[dst1].ar = c55_memory_read(core, core->sp);
    }
    core->sp += 1;

    if (fd == 0)
    {
        if (dst2 < 4)
        {
            core->ac[dst2] = c55_memory_read(core, (core->sp)) & 0xffff;
        } else
        {
            core->t[dst2 - 4] = c55_memory_read(core, (core->sp));
        }
    } else
    {
        core->xar[dst2].ar = c55_memory_read(core, (core->sp));
    }
    core->sp += 1;
    return 0;
}
static int exec_call_acx(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int acx = BITS(32, 33);

    core->sp -= 1;
    core->reta = core->pc + 2;
    c55_memory_write(core, core->sp, core->reta & 0xffff);
    core->ssp -= 1;
    c55_memory_write(core, core->ssp, core->reta & 0xff0000);

    int call_addr = core->ac[acx] & 0xffffff;

    core->pc = call_addr;
    return 0;
}
static int exec_mov_k12_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k12 = BITS(28, 39);
    int k7 = BITS(28, 34);
    int op = BITS(24, 27);

    switch (op)
    {
        case 0x0:
            core->dph = k7;
            break;
        case 0x1:
            core->dph = (core->dph & 0x1F) | k7;
            break;
        case 0x2:
            core->dph = (core->dph & 0x60) | k7;
            break;
        case 0x4:
            core->bk03 = k12;
            break;
        case 0x5:
            core->bk47 = k12;
            break;
        case 0x6:
            core->bkc = k12;
            break;
        case 0x8:
            core->csr = k12;
            break;
        case 0x9:
            core->brc0 = k12;
            break;
        case 0xa:
            core->brc1 = k12;
            break;
        case 0xc:
            //SIM_TRIG
            break;
        default:
            printf("In %s, at line %d, Not implement this instruction at op = %x\n", __FUNCTION__, __LINE__, op);
            skyeye_exit(-1);
    }
    core->pc += 3;
    return 0;
}

static int exec_add_lmem_acx_acy(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int fdst = BITS(28, 29);
    int fsrc = BITS(30, 31);
    int lmem = BITS(33, 39);
    int addr_mode = BITS(32, 32);
    int mem32_val = 0;
    uint64_t result;

    /*
     * int data1 = c55_memory_read(core, (lmem + core->sp) );
     * int data2 = c55_memory_read(core, (lmem + core->sp + 1) );
     * int mem32_val = (data1 << 16) | data2;
     */

    if (addr_mode)
        mem32_val = read_val_from_indirect_lmem_address(core, insn);
    else
        mem32_val = read_val_from_direct_lmem_address(core, insn);

    int x = bit(core->ac[fsrc], 31);
    int y = bit(mem32_val, 31);

    result = (core->ac[fsrc] + mem32_val);
    int z = bit(result, 31);

    /* Carry conditions */
    //      if ((x && y && z == 0) || (!x && !y && z) || (!x && y && z == 0) || (x && !y && z == 0)){
    if ((x || y) && z == 0)
    {
        SET(core->st0_55, CARRY);
        /*Overflow conditions */
        switch (fdst)
        {
            case 0:
                SET(core->st0_55, ACOV0);
                break;
            case 1:
                SET(core->st0_55, ACOV1);
                break;
            case 2:
                SET(core->st0_55, ACOV2);
                break;
            case 3:
                SET(core->st0_55, ACOV3);
                break;
            default:
                printf("It's error in %s, pc:%x\n", __FUNCTION__, core->pc);
                skyeye_exit(-1);
        }
    } else
        CLEAR(core->st0_55, CARRY);

    core->ac[fdst] = result & 0xFFFFFFFFFF;

    core->pc += 3;
    return 0;
}
static int exec_sub_lmem_acx_acy(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int fdst = BITS(28, 29);
    int fsrc = BITS(30, 31);
    int lmem = BITS(33, 39);
    int addr_mode = BITS(32, 32);
    int mem32_val = 0;

    /*
     * int data1 = c55_memory_read(core, (lmem + core->sp) );
     * int data2 = c55_memory_read(core, (lmem + core->sp + 1) );
     * int mem32_val = (data1 << 16) | data2;
     */
    if (addr_mode)
        mem32_val = read_val_from_indirect_lmem_address(core, insn);
    else
        mem32_val = read_val_from_direct_lmem_address(core, insn);

    int x = bit(core->ac[fsrc], 31);
    int y = bit(mem32_val, 31);

    core->ac[fdst] = core->ac[fsrc] - mem32_val;
    int z = bit(core->ac[fdst], 31);

    /* Carry conditions */
    //if ((x && y && z == 0) || (!x && !y && z) || (!x && y && z == 0) || (x && !y && z == 0))
    if ((x || y) && z == 0)
        CLEAR(core->st0_55, CARRY);
    else
        SET(core->st0_55, CARRY);

    core->pc += 3;
    return 0;
}
static int exec_mov_k8_abs_smem(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k8 = BITS(24, 31);
    int lsmem = BITS(8, 23);

    c55_memory_write(core, lsmem, k8);

    core->pc += 5;
    return 0;
}
static int exec_mov_k8_smem(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k8 = BITS(24, 31);
    int op = BITS(33, 36);
    int k16 = BITS(8, 23);
    int k23 = BITS(0, 23);
    int addr = BITS(37, 39);
    int parallel_op = BITS(16, 23);
    int addr_mode = BITS(32, 32);

    if (addr_mode && op >= 6 && op <= 8)
    {
        if (op == 6)
        {
            if ((k16 >> 15) & 0x1)
                k16 = (k16 - 0xFFFF) - 1;
            c55_memory_write(core, core->xar[addr].ar + k16, k8);
            core->pc += 2;
        } else if (op == 7)
        {
            core->xar[addr].ar += k16;
            c55_memory_write(core, core->xar[addr].ar + k16, k8);
            core->pc += 2;
        } else
        {
            switch (addr)
            {
                case 0:
                    c55_memory_write(core, k16, k8);
                    core->pc += 2;
                    break;
                case 1:
                    c55_memory_write(core, k23, k8);
                    core->pc += 3;
                    break;
                default:
                    NOT_IMP;
                    break;
            }
        }
    } else
    {
        if (addr_mode)
            write_val_to_indirect_smem_address(core, insn, k8);
        else
            write_val_to_direct_smem_address(core, insn, k8);
    }

    if (parallel_op == 0x9a)
        core->pc += 1;

    core->pc += 3;
    return 0;
}
static int exec_mov_tax_acx(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int src = BITS(36, 38);
    int fs = BITS(39, 39);
    int dst = BITS(32, 33);

    if (fs == 1)
    {
        core->ac[dst] = core->xar[src].ar << 16;
    } else
    {
        if (src < 4)
        {
            core->ac[dst] = core->ac[src] << 16;
        } else
            core->ac[dst] = core->t[src - 4] << 16;
    }

    core->pc += 2;
    return 0;
}
static int exec_mpy_acx_acy(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int src = BITS(36, 37);
    int dst = BITS(38, 39);

    int x = bit(core->ac[src], 31);
    int y = bit(core->ac[dst], 31);

    core->ac[dst] = (core->ac[src] >> 16) * (core->ac[dst] >> 16);
    int z = bit(core->ac[dst], 31);

    /*Overflow conditions */
    if (x && y && z)
    {
        switch (dst)
        {
            case 0:
                SET(core->st0_55, ACOV0);
                break;
            case 1:
                SET(core->st0_55, ACOV1);
                break;
            case 2:
                SET(core->st0_55, ACOV2);
                break;
            case 3:
                SET(core->st0_55, ACOV3);
                break;
            default:
                printf("It's error in %s, pc:%x\n", __FUNCTION__, core->pc);
                skyeye_exit(-1);
        }
    }

    core->pc += 2;
    return 0;
}
static int exec_mov_acx_tax(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int src = BITS(36, 37);
    int dst = BITS(32, 34);
    int fd = BITS(35, 35);

    if (fd == 1)
    {
        core->xar[dst].ar = core->ac[src] >> 16;
    } else
    {
        if (dst < 4)
        {
            core->ac[dst] = core->ac[src] >> 16;
        } else
            core->t[dst - 4] = core->ac[src] >> 16;
    }

    core->pc += 2;
    return 0;
}
static int exec_add_k16_abs_smem(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int lsmem = BITS(0, 15);
    int k16 = BITS(16, 31);
    uint16_t mem16_val = c55_memory_read(core, lsmem);
    int x = bit(k16, 15);
    int y = bit(mem16_val, 15);

    mem16_val += k16;
    int z = bit(mem16_val, 15);

    c55_memory_write(core, lsmem, mem16_val);
    core->pc += 6;
    /* Carry conditions */
    //      if ((x && y && z == 0) || (!x && !y && z) || (!x && y && z == 0) || (x && !y && z == 0))
    if ((x || y) && z == 0)
        SET(core->st0_55, CARRY);
    else
        CLEAR(core->st0_55, CARRY);

    return 0;
}
static int exec_add_k16_smem(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k16 = fields->k16;

    //uint16_t mem16_val = read_imm_val_from_smem(core, insn);
    int addr_mode = BITS(32, 32);
    uint16_t mem16_val = 0;

    if (addr_mode)
        mem16_val = read_val_from_indirect_smem_address(core, insn);
    else
        mem16_val = read_val_from_direct_smem_address(core, insn);

    int x = bit(k16, 15);
    int y = bit(mem16_val, 15);

    mem16_val += k16;
    int z = bit(mem16_val, 15);

    //write_imm_val_to_smem(core, insn, mem16_val);
    if (addr_mode)
        write_val_to_indirect_smem_address(core, insn, mem16_val);
    else
        write_val_to_direct_smem_address(core, insn, mem16_val);

    core->pc += 4;
    /* Carry conditions */
    //      if ((x && y && z == 0) || (!x && !y && z) || (!x && y && z == 0) || (x && !y && z == 0))
    if ((x || y) && z == 0)
        SET(core->st0_55, CARRY);
    else
        CLEAR(core->st0_55, CARRY);

    return 0;
}
static int exec_and_k16_shift_acx_acy(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                      opcode_fields_t * fields)
{
    int16_t k16 = fields->bk16;
    int fsrc = BITS(22, 23);
    int fdst = BITS(20, 21);
    int m40 = bit(core->st1_55, M40);

    if (m40)
        core->ac[fdst] = (core->ac[fsrc] & (k16 << 16)) & 0xFFFFFFFFFF;
    else
        core->ac[fdst] = (core->ac[fsrc] & (k16 << 16)) & 0xFFFFFFFF;

    core->pc += 4;
    return 0;
}
static int exec_mov_sp_tax(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int fdst = BITS(32, 34);
    int fd = BITS(35, 35);

    if (fd == 1)
    {
        core->xar[fdst].ar = core->sp;
    } else
    {
        if (fdst < 4)
        {
            core->ac[fdst] = core->sp;
        } else
            core->t[fdst - 4] = core->sp;
    }

    core->pc += 2;
    return 0;
}
static int exec_rpt_k8(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k8 = BITS(32, 39);

    core->rptc = k8;

    core->pc += 2;
    return 0;
}
static int exec_amar_smem(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int x = BITS(32, 32);
    int src = BITS(37, 39);
    int smem = BITS(33, 39);
    int k16 = BITS(16, 31);
    int op = BITS(33, 35);

    if (x == 1)
    {
        if (op == 0x1)
        {
            core->xar[src].ar += 1;
            core->pc += 2;
        } else if (op == 0x2)
        {
            core->xar[src].ar -= 1;
            core->pc += 2;
        } else if (op == 0x7)
        {
            core->xar[src].ar += k16;
            core->pc += 4;
        } else
        {
            printf("In %s, %d, insn %lx decode error.\n", __FUNCTION__, __LINE__, insn);
            skyeye_exit(-1);
        }
    } else
    {
        printf("In %s, %d, insn %lx decode error.\n", __FUNCTION__, __LINE__, insn);
        skyeye_exit(-1);
    }

    return 0;
}
static int exec_macmk_smem_k8_acx_acy(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                      opcode_fields_t * fields)
{
    int k8 = BITS(24, 31);

    //      int smem = BITS(33, 39);
    int fsrc = BITS(20, 21);
    int fdst = BITS(22, 23);
    int addr_mode = BITS(32, 32);
    uint16_t mem16_val = 0;

    //int mem16_val = c55_memory_read(core, (smem + core->sp) );
    if (addr_mode)
        mem16_val = read_val_from_indirect_smem_address(core, insn);
    else
        mem16_val = read_val_from_direct_smem_address(core, insn);

    printf("mem16_val :%x\n", mem16_val);
    core->ac[fdst] = core->ac[fsrc] + (mem16_val * k8);

    core->pc += 4;
    return 0;
}
static int exec_add_smem_src_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int smem = BITS(33, 39);
    int fsrc = BITS(24, 26);
    int fdst = BITS(28, 30);
    int fs = BITS(27, 27);
    int fd = BITS(31, 31);
    int addr_mode = BITS(32, 32);
    uint16_t data = 0;

    if (addr_mode)
        data = read_val_from_indirect_smem_address(core, insn);
    else
        data = read_val_from_direct_smem_address(core, insn);

    if (fd == 1)
    {
        if (fs == 1)
            core->xar[fdst].ar = core->xar[fsrc].ar + data;
        else
        {
            if (fsrc > 3)
            {
                core->xar[fdst].ar = core->t[fsrc - 4] + data;
            } else
                core->xar[fdst].ar = core->ac[fsrc] + data;
        }
    } else
    {
        if (fdst > 3)
        {
            if (fs == 1)
            {
                core->t[fdst - 4] = core->xar[fsrc].ar + data;
            } else
            {
                if (fsrc > 3)
                {
                    core->t[fdst - 4] = core->t[fsrc - 4] + data;
                } else
                    core->t[fdst - 4] = core->ac[fsrc] + data;
            }
        } else
        {
            if (fs == 1)
            {
                core->ac[fdst] = core->xar[fsrc].ar + data;
            } else
            {
                if (fsrc > 3)
                {
                    core->ac[fdst] = core->t[fsrc - 4] + data;
                } else
                    core->ac[fdst] = core->ac[fsrc] + data;
            }
        }
    }

    core->pc += 3;
    return 0;
}

/*instruction 0x8a6186c0 : MOV *AR3, T2 || MOV AC0, *AR3 */
static int exec_mov_smem_dst_mov_src_smem(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                          opcode_fields_t * fields)
{
    //      uint16_t mem16_val = read_smem_to_register(core, insn);
    int fdst = BITS(24, 26);
    int fd = BITS(27, 27);
    int src = BITS(16, 18);
    int fs = BITS(19, 19);
    int addr_mode = BITS(32, 32);
    uint16_t mem16_val = 0;

    if (addr_mode)
        mem16_val = read_val_from_indirect_smem_address(core, insn);
    else
        mem16_val = read_val_from_direct_smem_address(core, insn);

    if (fd == 1)
    {
        core->xar[fdst].ar = mem16_val;
    } else
    {
        if (fdst > 3)
        {
            core->t[fdst - 4] = mem16_val;
        } else
        {
            core->ac[fdst] = mem16_val;
        }
    }

    if (fs == 1)
    {
        mem16_val = core->xar[src].ar;
        //      write_smem_from_register(core, insn, core->xar[src].ar);
    } else
    {
        if (src < 4)
        {
            mem16_val = core->ac[src];
            //              write_smem_from_register(core, insn, core->ac[src]);
        } else
        {
            mem16_val = core->t[src - 4];
            //              write_smem_from_register(core, insn, core->t[src - 4]);
        }
    }
    if (addr_mode)
        write_val_to_indirect_smem_address(core, insn, mem16_val);
    else
        write_val_to_direct_smem_address(core, insn, mem16_val);

    core->pc += 4;
    return 0;
}

static int exec_mov_smem_dst_aadd_src_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                          opcode_fields_t * fields)
{
    int fsrc = BITS(12, 14);
    int fdst = BITS(13, 15);
    int fd = BITS(15, 15);
    int fs = BITS(27, 27);
    int smem = BITS(33, 39);
    int dst = BITS(24, 26);
    int d = BITS(27, 27);

    if (fd == 1)
    {
        if (fs == 1)
            core->xar[fdst].ar += core->xar[fsrc].ar;
        else
        {
            if (fsrc > 3)
            {
                core->xar[fdst].ar += core->t[fsrc - 4];
            } else
                core->xar[fdst].ar += core->ac[fsrc];
        }
    } else
    {
        if (fdst > 3)
        {
            if (fs == 1)
            {
                core->t[fdst - 4] += core->xar[fsrc].ar;
            } else
            {
                if (fsrc > 3)
                {
                    core->t[fdst - 4] += core->t[fsrc - 4];
                } else
                    core->t[fdst - 4] += core->ac[fsrc];
            }
        } else
        {
            if (fs == 1)
            {
                core->ac[fdst] += core->xar[fsrc].ar;
            } else
            {
                if (fsrc > 3)
                {
                    core->ac[fdst] += core->t[fsrc - 4];
                } else
                    core->ac[fdst] += core->ac[fsrc];
            }
        }
    }

    if (d == 1)
    {
        core->xar[dst].ar = c55_memory_read(core, (smem + core->sp));
    } else
    {
        if (dst > 3)
        {
            core->t[dst - 4] = c55_memory_read(core, (smem + core->sp));
        } else
            core->ac[dst] = c55_memory_read(core, (smem + core->sp));
    }

    core->pc += 5;
    return 0;
}

static int exec_macm_xmem_ymem_acx_acy(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                       opcode_fields_t * fields)
{
    int xmem = BITS(37, 39);
    int ymem = BITS(31, 33);
    int fsrc = BITS(26, 27);
    int fdst = BITS(24, 25);

    int data1 = c55_memory_read(core, core->xar[xmem].ar);
    int data2 = c55_memory_read(core, core->xar[ymem].ar);

    core->ac[fdst] = core->ac[fsrc] + (data1 * data2);
    if (core->ac[fdst] >> 31 & 0x1)
    {
        switch (fdst)
        {
            case 0:
                SET(core->st0_55, ACOV0);
                break;
            case 1:
                SET(core->st0_55, ACOV1);
                break;
            case 2:
                SET(core->st0_55, ACOV2);
                break;
            case 3:
                SET(core->st0_55, ACOV3);
                break;
            default:
                printf("In %s, %d, insn %lx decode error.\n", __FUNCTION__, __LINE__, insn);
                skyeye_exit(-1);
        }
    }

    core->pc += 4;
    return 0;
}
static int exec_xor_k16_src_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k16 = BITS(24, 39);
    int src = BITS(16, 18);
    int dst = BITS(20, 22);
    int fs = BITS(19, 19);
    int fd = BITS(23, 23);

    if (fs == 1)
    {
        if (fd == 1)
        {
            core->xar[dst].ar = core->xar[src].ar ^ k16;;
        } else
        {
            if (dst < 4)
            {
                core->ac[dst] = core->xar[src].ar ^ k16;
            } else
                core->t[dst - 4] = core->xar[src].ar ^ k16;
        }
        if (core->xar[src].ar == 0)
        {
            SET(core->st0_55, CARRY);
        }
    } else
    {
        if (fd == 1)
        {
            if (src < 4)
            {
                core->xar[dst].ar = core->ac[src] ^ k16;
                if (core->ac[src] == 0)
                {
                    SET(core->st0_55, CARRY);
                }
            } else
            {
                core->xar[dst].ar = core->t[src - 4] ^ k16;
                if (core->t[src - 4] == 0)
                {
                    SET(core->st0_55, CARRY);
                }
            }
        } else
        {
            if (dst < 4)
            {
                if (src < 4)
                {
                    core->ac[dst] = core->ac[src] ^ k16;
                    if (core->ac[src] == 0)
                    {
                        SET(core->st0_55, CARRY);
                    }
                } else
                {
                    core->ac[dst] = core->t[src - 4] ^ k16;
                    if (core->t[src - 4] == 0)
                    {
                        SET(core->st0_55, CARRY);
                    }
                }
            } else
            {
                if (src < 4)
                {
                    core->t[dst - 4] = core->ac[src] ^ k16;
                    if (core->ac[src] == 0)
                    {
                        SET(core->st0_55, CARRY);
                    }
                } else
                {
                    core->t[dst - 4] = core->t[src - 4] ^ k16;
                    if (core->t[src - 4] == 0)
                    {
                        SET(core->st0_55, CARRY);
                    }
                }
            }
        }
    }

    core->pc += 4;
    return 0;
}
static int exec_sub_acx_shift_acy(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int fsrc = BITS(36, 37);
    int fdst = BITS(38, 39);
    int shiftw = fields->shift;

    if (shiftw <= 32)
        core->ac[fdst] -= core->ac[fsrc] << shiftw;
    else
        core->ac[fdst] -= core->ac[fsrc] >> (0x40 - shiftw);

    core->pc += 3;
    return 0;
}
static int exec_mov_k16_abs_smem(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int lsmem = BITS(0, 15);
    int k16 = BITS(16, 31);

    c55_memory_write(core, lsmem, k16);

    core->pc += 6;
    return 0;
}
static int exec_mov_k16_smem(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k16 = fields->k16;
    int addr_mode = BITS(32, 32);
    int parallel_op = BITS(8, 15);

    //write_imm_val_to_smem(core, insn, k16);
    if (addr_mode)
        write_val_to_indirect_smem_address(core, insn, k16);
    else
        write_val_to_direct_smem_address(core, insn, k16);

    if (parallel_op == 0x9a)
        core->pc += 1;

    core->pc += 4;
    return 0;
}
static int exec_call_l16(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int l16 = BITS(24, 39);

    core->sp -= 1;
    core->reta = core->pc + 3;
    c55_memory_write(core, core->sp, core->reta & 0xffff);
    core->ssp -= 1;
    c55_memory_write(core, core->ssp, (core->reta >> 16) & 0xff);

    if (l16 >> 15 & 0x1)
    {
        l16 = 0xffff - l16 + 1;
        core->pc -= l16;
    } else
        core->pc += l16;

    core->pc += 3;
    return 0;
}
static int exec_cmp_smem_k16_tcx(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int smem = BITS(33, 39);
    int k16 = BITS(16, 31);
    int x = BITS(40, 40);
    int addr_mode = BITS(32, 32);
    uint16_t mem16_val = 0;
    int tcx = 0;

    if (x == 0)
        tcx = TC1;
    else
        tcx = TC2;

    if (addr_mode)
        mem16_val = read_val_from_indirect_smem_address(core, insn);
    else
        mem16_val = read_val_from_direct_smem_address(core, insn);

    if (mem16_val == k16)
    {
        SET(core->st0_55, tcx);
    } else
        CLEAR(core->st0_55, tcx);

    core->pc += 4;
    return 0;
}

static int exec_subc_smem_acx_acy(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int smem = BITS(33, 39);
    int src = BITS(30, 31);
    int dst = BITS(28, 29);
    int addr_mode = BITS(32, 32);
    int64_t smem_data = 0;

    if (addr_mode)
        smem_data = read_val_from_indirect_smem_address(core, insn);
    else
        smem_data = read_val_from_direct_smem_address(core, insn);

    smem_data = SIGN_EXTEND(smem_data, 40);
    smem_data = smem_data << 15;

    if (core->ac[src] >= smem_data)
    {
        SET(core->st0_55, CARRY);
        core->ac[dst] = ((core->ac[src] - smem_data) << 1) + 1;
    } else
    {
        core->ac[dst] = core->ac[src] << 1;
        CLEAR(core->st0_55, CARRY);
    }
    while (core->rptc)
    {
        core->rptc--;
        return 0;
    }

    core->pc += 3;
    return 0;
}
static int exec_abs_src_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int src = BITS(36, 38);
    int dst = BITS(32, 34);
    int fs = BITS(39, 39);
    int fd = BITS(35, 35);

    if (fs == 1)
    {
        int tmp = core->xar[src].ar;

        if (fd == 1)
        {
            if (tmp < 0)
                tmp = -tmp;
            core->xar[dst].ar = tmp;
        } else
        {
            if (dst < 4)
            {
                core->ac[dst] = tmp;
                if (core->ac[dst] == 0)
                {
                    SET(core->st0_55, CARRY);
                } else
                    CLEAR(core->st0_55, CARRY);
            } else
                core->t[dst - 4] = tmp;
        }
    } else
    {
        if (fd == 1)
        {
            if (src < 4)
            {
                (core->ac[src] < 0) ? (core->xar[dst].ar = -core->ac[src]) : (core->xar[dst].ar = core->ac[src]);
            } else
                (core->t[src - 4] < 0) ? (core->xar[dst].ar = -core->t[src - 4]) : (core->xar[dst].ar =
                                                                                    core->t[src - 4]);
        } else
        {
            if (dst < 4)
            {
                if (src < 4)
                {
                    (core->ac[src] < 0) ? (core->ac[dst] = -core->ac[src]) : (core->ac[dst] = core->ac[src]);
                } else
                    (core->t[src - 4] < 0) ? (core->ac[dst] = -core->t[src - 4]) : (core->ac[dst] = core->t[src - 4]);

                if (core->ac[dst] == 0)
                {
                    SET(core->st0_55, CARRY);
                } else
                    CLEAR(core->st0_55, CARRY);
            } else
            {
                if (src < 4)
                {
                    (core->ac[src] < 0) ? (core->t[dst - 4] = -core->ac[src]) : (core->t[dst - 4] = core->ac[src]);
                } else
                    (core->t[src - 4] < 0) ? (core->t[dst - 4] = -core->t[src - 4]) : (core->t[dst - 4] =
                                                                                       core->t[src - 4]);
            }
        }
    }

    core->pc += 2;
    return 0;
}
static int exec_sub_src_smem_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int smem_addr = BITS(33, 39);
    int src = BITS(24, 26);
    int dst = BITS(28, 30);
    int fs = BITS(27, 27);
    int fd = BITS(31, 31);
    int addr_mode = BITS(32, 32);
    uint16_t smem_data = 0;

    //int16_t smem_data = c55_memory_read(core, (smem_addr + core->sp) );
    if (addr_mode)
        smem_data = read_val_from_indirect_smem_address(core, insn);
    else
        smem_data = read_val_from_direct_smem_address(core, insn);

    if (fs == 1)
    {
        if (fd == 1)
        {
            core->xar[dst].ar = smem_data - core->xar[src].ar;
        } else
        {
            if (dst < 4)
            {
                smem_data = SIGN_EXTEND(smem_data, 40);
                core->ac[dst] = smem_data - core->xar[src].ar;
            } else
            {
                core->t[dst - 4] = smem_data - core->xar[src].ar;
            }
        }
    } else
    {
        if (dst < 4)
        {
            smem_data = SIGN_EXTEND(smem_data, 40);
            if (src < 4)
            {
                core->ac[dst] = smem_data - core->ac[src];
            } else
            {
                core->ac[dst] = smem_data - core->t[src - 4];
            }
        } else
        {
            if (src < 4)
            {
                core->t[dst - 4] = smem_data - core->ac[src];
            } else
            {
                core->t[dst - 4] = smem_data - core->t[src - 4];
            }
        }
    }
    core->pc += 3;
    return 0;
}
static int exec_mov_lmem_tax(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int dst = BITS(28, 30);
    int fd = BITS(31, 31);
    int addr_mode = BITS(32, 32);
    uint32_t mem32_val = 0;

    //int mem32_val = read_lmem_data(core, insn);
    if (addr_mode)
        mem32_val = read_val_from_indirect_lmem_address(core, insn);
    else
        mem32_val = read_val_from_direct_lmem_address(core, insn);

    core->pc += 3;
    //next instruction
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }

    if (fd == 1)
    {
        core->xar[dst].xar = mem32_val & 0x7FFFFF;
    } else
    {
        core->t[dst - 4] = (mem32_val >> 16);
        core->t[dst - 3] = mem32_val & 0xFFFF;
    }
    return 0;
}
static int exec_mov_xsrc_lmem(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int xsrc = BITS(28, 30);
    int fs = BITS(31, 31);
    int addr_mode = BITS(32, 32);
    uint32_t mem32_val = 0;

    if (fs == 0)
    {
        switch (xsrc)
        {
            case 0:
            case 1:
            case 2:
            case 3:
                mem32_val = core->ac[xsrc];
                break;
            case 4:
                mem32_val = core->xsp;
                break;
            case 5:
                mem32_val = core->xssp;
                break;
            case 6:
                mem32_val = core->xdp;
                break;
            case 7:
                mem32_val = core->xcdp;
                break;
            default:
                printf("Decode error instruction at %s, pc:%x\n", __FUNCTION__, core->pc);
                skyeye_exit(-1);
        }
    } else
    {
        mem32_val = core->xar[xsrc].xar;
    }

    if (addr_mode)
        write_val_to_indirect_lmem_address(core, insn, mem32_val);
    else
        write_val_to_direct_lmem_address(core, insn, mem32_val);

    core->pc += 3;
    return 0;
}
static int exec_pshboth_xsrc(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int src = BITS(36, 38);
    int fs = BITS(39, 39);
    int mem32_val;

    core->sp -= 1;
    core->ssp -= 1;

    if (fs == 1)
    {
        c55_memory_write(core, core->sp, core->xar[src].xar >> 16);
        c55_memory_write(core, (core->ssp), core->xar[src].xar & 0xffff);
    } else
    {
        switch (src)
        {
            case 0:
            case 1:
            case 2:
            case 3:
                mem32_val = core->ac[src] & 0xFFFFFFFF;
                break;
            case 4:
                mem32_val = core->xsp;
                break;
            case 5:
                mem32_val = core->xssp;
                break;
            case 6:
                mem32_val = core->xdp;
                break;
            case 7:
                mem32_val = core->xcdp;
                break;
            default:
                break;
        }
        c55_memory_write(core, core->sp, mem32_val >> 16);
        c55_memory_write(core, core->ssp, mem32_val & 0xFFFF);
    }

    core->pc += 2;
    return 0;
}
static int exec_btst_baddr_src_tcx(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int op = BITS(24, 27);
    int src = BITS(28, 30);
    int fs = BITS(31, 31);
    int baddr = BITS(33, 39);
    int bit_val = 0;
    uint16_t xar_src = core->xar[src].ar;
    uint64_t ac_src = core->ac[src];
    uint16_t t_src = core->t[src - 4];

    core->pc += 3;
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }

    if (fs == 1)
    {
        bit_val = (xar_src >> baddr) & 0x1;
    } else
    {
        if (src > 3)
        {
            bit_val = (t_src >> baddr) & 0x1;
        } else
        {
            bit_val = (ac_src >> baddr) & 0x1;
            if (baddr > 39)
                bit_val = 0;
        }
    }

    if (op == 0x8)
    {
        (bit_val == 1) ? SET(core->st0_55, TC1) : CLEAR(core->st0_55, TC1);
    } else if (op == 0x9)
    {
        (bit_val == 1) ? SET(core->st0_55, TC2) : CLEAR(core->st0_55, TC2);
    } else
    {
        printf("In %s, decode error insn %lx\n", __func__, insn);
    }
    while (core->rptc)
    {
        core->rptc--;
        return 0;
    }

    return 0;
}
static int exec_popboth_xdst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int dst = BITS(36, 38);
    int fd = BITS(39, 39);
    int mem32_val = (c55_memory_read(core, core->sp) << 16) | c55_memory_read(core, core->ssp);

    if (fd == 1)
    {
        core->xar[dst].xar = mem32_val;
    } else
    {
        switch (dst)
        {
            case 0:
            case 1:
            case 2:
            case 3:
                core->ac[dst] = (core->ac[dst] & 0xFF00000000) | mem32_val;
                break;
            case 4:
                core->xsp = mem32_val;
                break;
            case 5:
                core->xssp = mem32_val;
                break;
            case 6:
                core->xdp = mem32_val;
                break;
            case 7:
                core->xcdp = mem32_val;
                break;
            default:
                break;
        }
    }

    core->sp += 1;
    core->ssp += 1;
    core->pc += 2;
    return 0;
}
static int exec_btst_k4_smem_tcx(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int baddr = BITS(33, 39);
    int k4 = BITS(28, 31);
    int op = BITS(33, 36);
    int op1 = BITS(24, 25);
    int k16 = BITS(8, 23);
    int k23 = BITS(0, 23);
    int addr = BITS(37, 39);
    int addr_mode = BITS(32, 32);
    uint16_t mem16_val = 0;
    int bit_val = 0;

    if (addr_mode && op >= 6 && op <= 8)
    {
        if (op == 6)
        {
            if ((k16 >> 15) & 0x1)
                k16 = (k16 - 0xFFFF) - 1;
            mem16_val = c55_memory_read(core, core->xar[addr].ar + k16);
            core->pc += 2;
        } else if (op == 7)
        {
            core->xar[addr].ar += k16;
            mem16_val = c55_memory_read(core, core->xar[addr].ar + k16);
            core->pc += 2;
        } else
        {
            switch (addr)
            {
                case 0:
                    mem16_val = c55_memory_read(core, k16);
                    core->pc += 2;
                    break;
                case 1:
                    mem16_val = c55_memory_read(core, k23);
                    core->pc += 3;
                    break;
                default:
                    NOT_IMP;
                    break;
            }
        }
    } else
    {
        if (addr_mode)
            mem16_val = read_val_from_indirect_smem_address(core, insn);
        else
            mem16_val = read_val_from_direct_smem_address(core, insn);
    }
    bit_val = (mem16_val >> k4) & 0x1;

    if (op1 == 0x0)
    {
        (bit_val == 1) ? SET(core->st0_55, TC1) : CLEAR(core->st0_55, TC1);
    } else if (op == 0x1)
    {
        (bit_val == 1) ? SET(core->st0_55, TC2) : CLEAR(core->st0_55, TC2);
    } else
    {
        printf("In %s, decode error insn %lx\n", __func__, insn);
        skyeye_exit(-1);
    }

    while (core->rptc)
    {
        core->rptc--;
        return 0;
    }

    core->pc += 3;
    return 0;
}
static int exec_aadd_tax_tay(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int src = BITS(36, 38);
    int fs = BITS(39, 39);
    int dst = BITS(28, 30);
    int fd = BITS(31, 31);

    if (fs == 1)
    {
        if (fd == 1)
        {
            core->xar[dst].ar += core->xar[src].ar;
        } else
        {
            core->t[dst - 4] += core->xar[src].ar;
        }
    } else
    {
        if (fd == 1)
        {
            core->xar[dst].ar += core->t[src - 4];
        } else
        {
            core->xar[dst].ar += core->t[src - 4];
        }
    }
    while (core->rptc)
    {
        core->rptc--;
        return 0;
    }

    core->pc += 3;
}
static int exec_asub_p8_tax(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int dst = BITS(28, 30);
    int fd = BITS(31, 31);
    unsigned char p8 = BITS(32, 39);

    if (fd == 1)
    {
        core->xar[dst].ar -= p8;
    } else
    {
        core->t[dst - 4] -= p8;
    }

    while (core->rptc)
    {
        core->rptc--;
        return 0;
    }
    core->pc += 3;
}
static int exec_rptb_pmad(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int pmad = BITS(24, 39);

    core->pc += 3;
    //next instruction
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }
    core->rptb_enable = 1;
    core->rsa0 = core->pc;
    core->rea0 = core->pc + pmad;
    return 0;
}
static int exec_rptblocal_pmad(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int pmad = BITS(32, 38);

    core->pc += 2;
    //next instruction
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }
    core->rptb_enable = 1;
    core->rsa0 = core->pc;
    core->rea0 = core->pc + pmad;
    return 0;
}
static int exec_bfxtr_k16_acx_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int16_t k16 = BITS(24, 39);
    int dst = BITS(20, 22);
    int fd = BITS(23, 23);
    int src = BITS(16, 17);
    int result = 0;
    int i = 15;
    int j = 0;

    core->pc += 4;
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }
    //test k16 each bit is 1 
    do
    {
        if ((k16 >> i) & 0x1)
        {
            int bit_val = (core->ac[src] >> i) & 0x1;

            result = SET_BIT(result, bit_val, j);
            j++;
        }
    } while (i--);

    if (fd == 1)
    {
        core->xar[dst].ar = result;
    } else
    {
        if (dst < 4)
        {
            core->ac[dst] = result;
        } else
        {
            core->t[dst - 4] = result;
        }
    }

    return 0;
}
static int exec_mov_smem_dst_aadd_p8_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                         opcode_fields_t * fields)
{
    unsigned char p8 = BITS(0, 7);
    int fd1 = BITS(31, 31);
    int fd2 = BITS(15, 15);
    int fdst1 = BITS(28, 30);
    int fdst2 = BITS(12, 14);
    int addr_mode = BITS(32, 32);
    uint16_t mem16_val = 0;

    //Mov Smem, dst
    //      uint16_t mem16_val = read_uns_smem_data(core, insn);
    if (addr_mode)
        mem16_val = read_val_from_indirect_smem_address(core, insn);
    else
        mem16_val = read_val_from_direct_smem_address(core, insn);

    if (fd2 == 1)
    {
        core->xar[fdst2].ar = mem16_val;
    } else
    {
        if (fdst2 < 4)
        {
            core->ac[fdst2] = mem16_val;
        } else
        {
            core->t[fdst2 - 4] = mem16_val;
        }
    }
    //AADD P8, dst
    if (fd1 == 1)
    {
        core->xar[fdst1].ar += p8;
    } else
    {
        if (fdst1 < 4)
        {
            core->ac[fdst1] += p8;
        } else
            core->t[fdst1 - 4] += p8;
    }

    core->pc += 6;
    return 0;
}
static int exec_or_abs_smem_src_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                    opcode_fields_t * fields)
{
    int fsrc = BITS(24, 26);
    int fdst = BITS(28, 30);
    int fs = BITS(27, 27);
    int fd = BITS(31, 31);
    int lsmem = BITS(8, 23);

    uint16_t mem16_val = c55_memory_read(core, lsmem);

    if (fd == 1)
    {
        if (fs == 1)
            core->xar[fdst].ar = core->xar[fsrc].ar | mem16_val;
        else
        {
            if (fsrc > 3)
            {
                core->xar[fdst].ar = core->t[fsrc - 4] | mem16_val;
            } else
                core->xar[fdst].ar = core->ac[fsrc] | mem16_val;
        }
    } else
    {
        if (fdst > 3)
        {
            if (fs == 1)
            {
                core->t[fdst - 4] = core->xar[fsrc].ar | mem16_val;
            } else
            {
                if (fsrc > 3)
                {
                    core->t[fdst - 4] = core->t[fsrc - 4] | mem16_val;
                } else
                    core->t[fdst - 4] = core->ac[fsrc] | mem16_val;
            }
        } else
        {
            if (fs == 1)
            {
                core->ac[fdst] = core->xar[fsrc].ar | mem16_val;
            } else
            {
                if (fsrc > 3)
                {
                    core->ac[fdst] = core->t[fsrc - 4] | mem16_val;
                } else
                    core->ac[fdst] = core->ac[fsrc] | mem16_val;
            }
        }
    }

    core->pc += 5;
    return 0;
}
static int exec_or_smem_src_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int fsrc = BITS(24, 26);
    int fdst = BITS(28, 30);
    int fs = BITS(27, 27);
    int fd = BITS(31, 31);
    int addr_mode = BITS(32, 32);
    uint16_t mem16_val = 0;

    if (addr_mode)
        mem16_val = read_val_from_indirect_smem_address(core, insn);
    else
        mem16_val = read_val_from_direct_smem_address(core, insn);

    //      uint16_t mem16_val = read_uns_smem_data(core, insn);

    if (fd == 1)
    {
        if (fs == 1)
            core->xar[fdst].ar = core->xar[fsrc].ar | mem16_val;
        else
        {
            if (fsrc > 3)
            {
                core->xar[fdst].ar = core->t[fsrc - 4] | mem16_val;
            } else
                core->xar[fdst].ar = core->ac[fsrc] | mem16_val;
        }
    } else
    {
        if (fdst > 3)
        {
            if (fs == 1)
            {
                core->t[fdst - 4] = core->xar[fsrc].ar | mem16_val;
            } else
            {
                if (fsrc > 3)
                {
                    core->t[fdst - 4] = core->t[fsrc - 4] | mem16_val;
                } else
                    core->t[fdst - 4] = core->ac[fsrc] | mem16_val;
            }
        } else
        {
            if (fs == 1)
            {
                core->ac[fdst] = core->xar[fsrc].ar | mem16_val;
            } else
            {
                if (fsrc > 3)
                {
                    core->ac[fdst] = core->t[fsrc - 4] | mem16_val;
                } else
                    core->ac[fdst] = core->ac[fsrc] | mem16_val;
            }
        }
    }

    core->pc += 3;
    return 0;
}

static int exec_bset_baddr_src(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int src = BITS(28, 30);
    int fs = BITS(31, 31);
    int baddr = BITS(33, 39);

    if (fs == 1)
    {
        core->xar[src].ar |= 0x1 << baddr;
    } else
    {
        if (src < 4)
        {
            core->ac[src] |= 0x1 << baddr;
        } else
        {
            core->t[src - 4] |= 0x1 << baddr;
        }

    }
    while (core->rptc)
    {
        core->rptc--;
        return 0;
    }

    core->pc += 3;
    return 0;
}
static int exec_mov_src_lmem_aadd_p8_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                         opcode_fields_t * fields)
{
    unsigned char p8 = BITS(0, 7);
    int fd1 = BITS(31, 31);
    int fd2 = BITS(15, 15);
    int fdst1 = BITS(28, 30);
    int fdst2 = BITS(12, 14);
    int addr_mode = BITS(32, 32);
    int mem32_val = 0;

    //AADD P8, dst
    if (fd1 == 1)
    {
        core->xar[fdst1].ar += p8;
    } else
    {
        if (fdst1 < 4)
        {
            core->ac[fdst1] += p8;
        } else
            core->t[fdst1 - 4] += p8;
    }
    //Mov src ,lmem
    if (fd2 == 1)
    {
        mem32_val = core->xar[fdst2].xar;
    } else
    {
        if (fdst2 < 4)
        {
            mem32_val = core->ac[fdst2];
        } else
        {
            mem32_val = core->t[fdst2 - 4];
        }
    }

    //write_imm_val_to_lmem(core, insn, mem32_val);
    if (addr_mode)
        write_val_to_indirect_lmem_address(core, insn, mem32_val);
    else
        write_val_to_direct_lmem_address(core, insn, mem32_val);

    core->pc += 6;
    return 0;
}
static int exec_exp_acx_tx(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int src = BITS(36, 37);
    int dst = BITS(28, 29);

    uint64_t src_val = core->ac[src] & 0xFFFFFFFFFF;
    uint16_t dst_val = 0;

    src_val = SIGN_EXTEND(src_val, 40);

    if ((src_val >> 39) & 0x1)
        src_val = 0xFFFFFFFFFF - src_val;

    int i;
    int bit_val;

    for (i = 39; i >= 0; i--)
    {
        bit_val = (src_val >> i) & 0x1;
        if (bit_val == 1)
        {
            dst_val = (39 - (i + 1)) - 8;
            break;
        }
    }

    core->pc += 3;
    //next instruction
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }

    core->t[dst] = dst_val;
    return 0;
}
static int exec_bnot_baddr_src(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int src = BITS(28, 30);
    int fs = BITS(31, 31);
    int op1 = BITS(32, 32);
    int addr = BITS(37, 39);
    int smem = BITS(33, 39);
    int baddr = 0;
    int m40 = bit(core->st1_55, M40);

    if (op1 == 0)
        baddr = smem;
    else
        baddr = core->xar[addr].ar & 0xF;

    if (fs == 1)
    {
        if ((core->xar[src].ar >> baddr) & 0x1)
            core->xar[src].ar &= ~(0x1 << baddr);
        else
            core->xar[src].ar |= (0x1 << baddr);
    } else
    {
        if (src < 4)
        {
            if ((core->ac[src] >> baddr) & 0x1)
            {
                core->ac[src] = (core->ac[src] & ~(0x1 << baddr)) | (core->ac[src] & 0xFF00000000);
            } else
            {
                core->ac[src] = (core->ac[src] | (0x1 << baddr)) & 0xFFFFFFFF;
            }
        } else
        {
            if ((core->t[src - 4] >> baddr) & 0x1)
                core->t[src - 4] &= ~(0x1 << baddr);
            else
                core->t[src - 4] |= (0x1 << baddr);
        }
    }

    while (core->rptc)
    {
        core->rptc--;
        return 0;
    }

    core->pc += 3;
    return 0;
}
static int exec_intr_k5(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k5 = BITS(32, 36);
    int ret_pc = core->pc + 2;

    core->sp -= 1;
    c55_memory_write(core, core->sp, core->st2_55);

    core->ssp -= 1;
    c55_memory_write(core, core->ssp, (core->st0_55 & 0xFE00)); //push st0_55 to ssp

    core->sp -= 1;
    c55_memory_write(core, core->sp, core->st1_55);
    //INTM_BIT is set to 1
    core->st1_55 |= INTM_BIT;

    core->ssp -= 1;
    c55_memory_write(core, core->ssp, core->dbstat);

    core->sp -= 1;
    c55_memory_write(core, core->sp, (ret_pc & 0xFFFF));

    core->ssp -= 1;
    c55_memory_write(core, core->ssp, ((ret_pc >> 16) & 0xFF));

    core->ifr &= ~(0x1 << k5);
    if (k5 >= 16 && k5 <= 23)
        core->pc = core->ivph + k5 * 8; //ISR address
    else
        core->pc = core->ivpd + k5 * 8; //ISR address

    return 0;
}
static int exec_reti(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    uint16_t msb_pc, lsb_pc = 0;

    msb_pc = c55_memory_read(core, core->ssp);
    core->ssp += 1;

    lsb_pc = c55_memory_read(core, core->sp);
    core->sp += 1;

    core->dbstat = c55_memory_read(core, core->ssp);
    core->ssp += 1;

    core->st1_55 = c55_memory_read(core, core->sp);
    core->sp += 1;

    core->st0_55 = c55_memory_read(core, core->ssp) & 0xFE00;   //pop st0_55 to ssp
    core->ssp += 1;

    core->st2_55 = c55_memory_read(core, core->sp);
    core->sp += 1;

    core->intr_flag = 0;
    core->pc = (msb_pc << 16) | lsb_pc;
    return 0;
}
static int exec_mov_k16_ssp(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k16 = BITS(24, 39);

    core->ssp = k16;
    core->pc += 4;
    return 0;
}

/*0xd9119004fa : AND *abs16(#004fah), AC0, AR1 */
static int exec_and_abs_smem_src_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                     opcode_fields_t * fields)
{
    int src = BITS(24, 26);
    int fs = BITS(27, 27);
    int dst = BITS(28, 30);
    int fd = BITS(31, 31);
    int lsmem = BITS(8, 23);

    uint16_t mem16_val = c55_memory_read(core, lsmem);

    if (fd == 1)
    {
        if (fs == 1)
            core->xar[dst].ar = mem16_val & core->xar[src].ar;
        else
        {
            if (src > 3)
                core->xar[dst].ar = mem16_val & core->t[src - 4];
            else
                core->xar[dst].ar = mem16_val & core->ac[src];
        }
    } else
    {
        if (dst > 3)
        {
            if (fs == 1)
                core->t[dst - 4] = mem16_val & core->xar[src].ar;
            else
            {
                if (src > 3)
                    core->t[dst - 4] = mem16_val & core->t[src - 4];
                else
                    core->ac[dst] = mem16_val & core->ac[src];
            }
        } else
        {
            if (fs == 1)
                core->ac[dst] = mem16_val & core->xar[src].ar;
            else
            {
                if (src > 3)
                    core->ac[dst] = mem16_val & core->t[src - 4];
                else
                    core->ac[dst] = mem16_val & core->ac[src];
            }

        }
    }
    core->pc += 5;
    return 0;
}
static int exec_and_smem_src_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int src = BITS(24, 26);
    int fs = BITS(27, 27);
    int dst = BITS(28, 30);
    int fd = BITS(31, 31);
    int parallel_op = BITS(24, 31);
    int addr_mode = BITS(32, 32);
    uint16_t mem16_val = 0;

    //uint16_t mem16_val = read_smem_to_register(core, insn);
    if (addr_mode)
        mem16_val = read_val_from_indirect_smem_address(core, insn);
    else
        mem16_val = read_val_from_direct_smem_address(core, insn);

    if (fd == 1)
    {
        if (fs == 1)
            core->xar[dst].ar = mem16_val & core->xar[src].ar;
        else
        {
            if (src > 3)
                core->xar[dst].ar = mem16_val & core->t[src - 4];
            else
                core->xar[dst].ar = mem16_val & core->ac[src];
        }
    } else
    {
        if (dst > 3)
        {
            if (fs == 1)
                core->t[dst - 4] = mem16_val & core->xar[src].ar;
            else
            {
                if (src > 3)
                    core->t[dst - 4] = mem16_val & core->t[src - 4];
                else
                    core->ac[dst] = mem16_val & core->ac[src];
            }
        } else
        {
            if (fs == 1)
                core->ac[dst] = mem16_val & core->xar[src].ar;
            else
            {
                if (src > 3)
                    core->ac[dst] = mem16_val & core->t[src - 4];
                else
                    core->ac[dst] = mem16_val & core->ac[src];
            }

        }
    }
    if (parallel_op == 0x99)
        core->pc += 1;

    core->pc += 2;
    return 0;
}
static int exec_mack_tx_k8_acx_acy(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k8 = BITS(32, 39);
    int ac_src = BITS(30, 31);
    int ac_dst = BITS(28, 29);
    int tx_src = BITS(26, 27);
    int frct = bit(core->st1_55, FRCT);
    uint64_t ac_src_val = core->ac[ac_src];
    uint16_t tx_src_val = core->t[tx_src];

    core->pc += 3;
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }

    core->ac[ac_dst] = ac_src_val + (tx_src_val * k8);
    //if FRCT = 1, result << 1;
    if (frct)
        core->ac[ac_dst] <<= 1;
    return 0;
}
static int exec_amov_tax_tay(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int src = BITS(36, 38);
    int fs = BITS(39, 39);
    int dst = BITS(28, 30);
    int fd = BITS(31, 31);
    uint16_t xar_src = core->xar[src].ar;
    uint16_t t_src = core->t[src - 4];

    core->pc += 3;
    if (No_exp != check_parallel_insn(conf_obj, core))
    {
        printf("In %s, implement this instruction at pc = 0x%x error.\n", __func__, core->pc);
        skyeye_exit(-1);
    }
    if (fs == 1)
    {
        if (fd == 1)
        {
            core->xar[dst].ar = xar_src;
        } else
        {
            core->t[dst - 4] = xar_src;
        }
    } else
    {
        if (fd == 1)
        {
            core->xar[dst].ar = t_src;
        } else
        {
            core->xar[dst].ar = t_src;
        }
    }
    while (core->rptc)
    {
        core->rptc--;
        return 0;
    }
    return 0;
}
static int exec_mack_tx_k16_acx_acy(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                    opcode_fields_t * fields)
{
    int k16 = BITS(24, 39);
    int ac_src = BITS(22, 23);
    int ac_dst = BITS(20, 21);
    int tx_src = BITS(18, 19);
    int frct = bit(core->st1_55, FRCT);

    core->ac[ac_dst] = core->ac[ac_src] + (core->t[tx_src] * k16);
    //if FRCT = 1, result << 1;
    if (frct)
        core->ac[ac_dst] <<= 1;
    /*Overflow conditions */
    if ((core->ac[ac_dst] >> 32) & 0xFF)
    {
        switch (ac_dst)
        {
            case 0:
                SET(core->st0_55, ACOV0);
                break;
            case 1:
                SET(core->st0_55, ACOV1);
                break;
            case 2:
                SET(core->st0_55, ACOV2);
                break;
            case 3:
                SET(core->st0_55, ACOV3);
                break;
            default:
                printf("In %s, %d, insn %lx decode error.\n", __FUNCTION__, __LINE__, insn);
                skyeye_exit(-1);
        }
    }
    core->pc += 4;
    return 0;
}
static int exec_mov_smem_shift16_acx(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn,
                                     opcode_fields_t * fields)
{
    int dst = BITS(40, 41);

    //uint16_t mem16_val = read_smem_to_register(core, insn);
    int addr_mode = BITS(32, 32);
    uint16_t mem16_val = 0;

    if (addr_mode)
        mem16_val = read_val_from_indirect_smem_address(core, insn);
    else
        mem16_val = read_val_from_direct_smem_address(core, insn);

    core->ac[dst] = ((int64_t) mem16_val) << 16;

    /*Overflow conditions */
    if ((core->ac[dst] >> 32) & 0xFF)
    {
        switch (dst)
        {
            case 0:
                SET(core->st0_55, ACOV0);
                break;
            case 1:
                SET(core->st0_55, ACOV1);
                break;
            case 2:
                SET(core->st0_55, ACOV2);
                break;
            case 3:
                SET(core->st0_55, ACOV3);
                break;
            default:
                printf("In %s, %d, insn %lx decode error.\n", __FUNCTION__, __LINE__, insn);
                skyeye_exit(-1);
        }
    }
    core->pc += 2;
    return 0;
}
static int exec_mpyk_k16_acx_acy(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k16 = BITS(24, 39);
    int src = BITS(22, 23);
    int dst = BITS(20, 21);
    int frct = bit(core->st1_55, FRCT);

    core->ac[dst] = ((core->ac[src] >> 16) & 0xFFFF) * k16;
    //if FRCT = 1, result << 1;
    if (frct)
        core->ac[dst] <<= 1;
    /*Overflow conditions */
    if ((core->ac[dst] >> 32) & 0xFF)
    {
        switch (dst)
        {
            case 0:
                SET(core->st0_55, ACOV0);
                break;
            case 1:
                SET(core->st0_55, ACOV1);
                break;
            case 2:
                SET(core->st0_55, ACOV2);
                break;
            case 3:
                SET(core->st0_55, ACOV3);
                break;
            default:
                printf("In %s, %d, insn %lx decode error.\n", __FUNCTION__, __LINE__, insn);
                skyeye_exit(-1);
        }
    }
    core->pc += 4;
    return 0;
}
static int exec_band_smem_k16_tcx(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k16 = BITS(24, 39);
    int op = BITS(40, 43);
    int tcx = 0;

    //      uint16_t mem16_val = read_smem_to_register(core, insn);
    int addr_mode = BITS(32, 32);
    uint16_t mem16_val = 0;

    if (addr_mode)
        mem16_val = read_val_from_indirect_smem_address(core, insn);
    else
        mem16_val = read_val_from_direct_smem_address(core, insn);

    if (op == 2)
        tcx = TC1;
    else if (op == 3)
        tcx = TC2;
    else
    {
        NOT_IMP;
    }

    if ((mem16_val & k16) == 0)
        CLEAR(core->st0_55, tcx);
    else
        SET(core->st0_55, tcx);

    core->pc += 4;
    return 0;
}
static int exec_psh_smem(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int addr_mode = BITS(32, 32);
    uint16_t mem16_val = 0;

    if (addr_mode)
        mem16_val = read_val_from_indirect_smem_address(core, insn);
    else
        mem16_val = read_val_from_direct_smem_address(core, insn);
    //uint16_t mem16_val = read_smem_to_register(core, insn); 

    core->sp -= 1;
    c55_memory_write(core, core->sp, mem16_val);

    core->pc += 2;
    return 0;
}
static int exec_pop_smem(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int paralled_op = BITS(24, 31);
    int addr_mode = BITS(32, 32);
    uint16_t mem16_val = c55_memory_read(core, core->sp);

    //write_smem_from_register(core, insn, mem16_val);
    if (addr_mode)
        write_val_to_indirect_smem_address(core, insn, mem16_val);
    else
        write_val_to_direct_smem_address(core, insn, mem16_val);

    core->sp += 1;

    core->pc += 2;
    return 0;
}
static int exec_psh_dbl_acx(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int src = BITS(36, 37);

    core->sp -= 2;
    c55_memory_write(core, core->sp, (core->ac[src] >> 16) & 0xFFFF);
    c55_memory_write(core, core->sp + 1, (core->ac[src] & 0xFFFF));

    core->pc += 2;
    return 0;
}
static int exec_pop_dbl_acx(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int src = BITS(36, 37);
    int mem32_val = (c55_memory_read(core, core->sp) << 16) | c55_memory_read(core, core->sp + 1);

    core->ac[src] = (core->ac[src] & 0xFF00000000) | mem32_val;

    core->sp += 2;

    core->pc += 2;
    return 0;
}
static int exec_sub_smem_src_dst(conf_object_t * conf_obj, c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int addr_mode = BITS(32, 32);
    int smem = fields->addr;
    uint16_t mem16_val = 0;
    int fs = BITS(27, 27);
    int src = BITS(24, 26);
    int fd = BITS(31, 31);
    int dst = BITS(28, 30);

    if (addr_mode)
        mem16_val = read_val_from_indirect_smem_address(core, insn);
    else
        mem16_val = read_val_from_direct_smem_address(core, insn);

    if (fs == 1)
    {
        if (fd == 1)
        {
            core->xar[dst].ar = core->xar[src].ar - mem16_val;
        } else
        {
            if (dst < 4)
            {
                core->ac[dst] = core->xar[src].ar - mem16_val;
            } else
            {
                core->t[dst - 4] = core->xar[src].ar - mem16_val;
            }
        }
    } else
    {
        if (fd == 1)
        {
            if (src < 4)
            {
                core->xar[dst].ar = core->ac[src] - mem16_val;
            } else
            {
                core->xar[dst].ar = core->t[src - 4] - mem16_val;
            }
        } else
        {
            if (dst < 4)
            {
                if (src < 4)
                {
                    core->ac[dst] = core->ac[src] - mem16_val;
                } else
                {
                    core->ac[dst] = core->t[src - 4] - mem16_val;
                }
            } else
            {
                if (src < 4)
                {
                    core->t[dst - 4] = core->ac[src] - mem16_val;
                } else
                {
                    core->t[dst - 4] = core->t[src - 4] - mem16_val;
                }
            }
        }
    }

    while (core->rptc)
    {
        core->rptc--;
        return 0;
    }
    core->pc += 3;
}
const ISEITEM insn32_decode_table[] = {
    {"xccpart_cond_mov_k4_dst", 2, 2, {41, 47, 0x1e, 23, 31, 0x13d}, 4, FALSE},
    {"xccpart_cond_mov_lmem_acx", 2, 2, {40, 47, 0xed, 15, 23, 0x13d}, 5, FALSE},
    {"xccpart_cond_neg_src_dst", 2, 2, {41, 47, 0x1a, 23, 31, 0x13d}, 4, FALSE},
    {"xccpart_cond_add_k4_dst", 2, 2, {41, 47, 0x20, 23, 31, 0x13d}, 4, FALSE},
    {"xccpart_cond_sfts_dst", 3, 2, {41, 47, 0x22, 38, 39, 0x1, 23, 31, 0x13d}, 4, FALSE},
    {"xccpart_cond_mov_k16_dst", 2, 2, {40, 47, 0x76, 7, 15, 0x13d}, 6, FALSE},
    {"xccpart_cond_sftl_dst", 2, 2, {41, 47, 0x28, 23, 31, 0x13d}, 4, FALSE},
    {"xccpart_cond_sftl_acx_shift_acy", 3, 2, {41, 47, 0x8, 32, 35, 0x7, 15, 23, 0x13d}, 5, FALSE},
    {"xccpart_cond_sub_k4_dst", 2, 2, {41, 47, 0x21, 23, 31, 0x13d}, 4, FALSE},
    {"xccpart_cond_mov_k16_shift_acx", 3, 2, {40, 47, 0x7a, 17, 19, 0x5, 7, 15, 0x13d}, 6, FALSE},
    {"xccpart_cond_mov_smem_dst", 2, 2, {44, 47, 0xa, 23, 31, 0x13d}, 4, FALSE},
    {"xcc_cond_mov_lmem_acx", 3, 2, {40, 47, 0xed, 25, 27, 0x4, 15, 23, 0x13c}, 5, FALSE},
    {"xcc_cond_mov_src_smem", 2, 2, {44, 47, 0xc, 23, 31, 0x13c}, 4, FALSE},
    //{"ret_or_src_dst", 3, 2, { 41, 47, 0x24, 32, 34, 0x4, 25, 31, 0x15 }, 4, FALSE},
    {"add_acx_shift_acy", 2, 2, {41, 47, 0x08, 32, 35, 0x3}, 3, TRUE},
    {"amar_smem_xadst", 1, 2, {32, 47, 0xec31}, 6, FALSE},
    {"amar_smem_xadst", 2, 2, {40, 47, 0xec, 24, 27, 0xe}, 3, FALSE},
    {"and_k16_abs_smem", 1, 2, {32, 47, 0xf411}, 6, FALSE},
    {"and_k16_smem", 1, 2, {40, 47, 0xf4}, 5, FALSE},
    {"or_k16_smem", 1, 2, {40, 47, 0xf5}, 5, FALSE},
    {"bclr_or_bset_k4_stx", 1, 2, {41, 47, 0x23}, 2, TRUE},
    {"call_p24", 1, 2, {40, 47, 0x6c}, 4, FALSE},
    {"aadd_k8_sp", 1, 2, {41, 47, 0x27}, 2, TRUE},
    {"aadd_p8_tax", 2, 2, {41, 47, 0xa, 24, 27, 0x4}, 3, FALSE},
    {"aadd_p8_tax", 2, 2, {41, 47, 0xa, 24, 27, 0xc}, 3, FALSE},
    {"not_src_dst", 1, 2, {41, 47, 0x1b}, 2, TRUE},
    {"bcc_l8_cond", 1, 2, {41, 47, 0x02}, 3, FALSE},
    {"mov_smem_dst", 1, 2, {44, 47, 0xa}, 2, FALSE},
    {"bcc_l4_cond", 1, 2, {43, 47, 0x0c}, 2, FALSE},
    {"mov_abs_lmem_acx", 2, 2, {32, 47, 0xed11, 25, 27, 0x4}, 5, FALSE},
    {"mov_lmem_acx", 2, 2, {40, 47, 0xed, 25, 27, 0x4}, 6, FALSE},
    {"ret", 2, 2, {41, 47, 0x24, 32, 34, 0x4}, 2, TRUE},
    {"mov_k16_acx", 2, 2, {40, 47, 0x7a, 17, 19, 0x5}, 4, FALSE},
    {"or_k16_src_dst", 1, 2, {40, 47, 0x7e}, 4, FALSE},
    {"cmp_src_dst_tcx", 3, 2, {41, 47, 0x09, 32, 33, 0x0, 24, 24, 0x0}, 3, TRUE},
    {"cmp_src_dst_tcx", 3, 2, {41, 47, 0x09, 32, 33, 0x0, 24, 24, 0x1}, 3, TRUE},
    {"nop", 1, 2, {41, 47, 0x10}, 1, TRUE},
    {"mov_k4_dst", 1, 2, {41, 47, 0x1e}, 2, TRUE},
    {"mov_negk4_dst", 1, 2, {41, 47, 0x1f}, 2, FALSE},
    {"xcc_cond", 1, 2, {39, 47, 0x12c}, 2, FALSE},
    {"xcc_cond", 1, 2, {39, 47, 0x13c}, 2, FALSE},
    {"xcc_cond", 1, 2, {39, 47, 0x13e}, 2, FALSE},
    {"mov_acx_lmem", 3, 2, {40, 47, 0xeb, 24, 24, 0x0, 26, 27, 0x2}, 3, FALSE},
    {"xor_k16_smem", 1, 2, {40, 47, 0xf6}, 4, FALSE},
    {"bclr_baddr_src", 2, 2, {40, 47, 0xec, 25, 27, 0x1}, 3, FALSE},
    {"or_k8_src_dst", 1, 2, {41, 47, 0xd}, 3, TRUE},
    {"bcc_l16_cond", 1, 2, {40, 47, 0x6d}, 4, FALSE},
    {"psh_src", 2, 2, {41, 47, 0x28, 32, 34, 0x6}, 2, TRUE},
    {"pop_dst", 2, 2, {41, 47, 0x28, 32, 34, 0x2}, 2, TRUE},
    {"xor_src_dst", 1, 2, {41, 47, 0x16}, 2, TRUE},
    {"and_src_dst", 1, 2, {41, 47, 0x14}, 2, TRUE},
    {"b_l16", 1, 2, {41, 47, 0x3}, 3, TRUE},
    {"mov_src_dst", 1, 2, {41, 47, 0x11}, 2, TRUE},
    {"mpym_smem_tx_acy", 2, 2, {40, 47, 0xd3, 26, 26, 0x1}, 3, FALSE},
    {"mpym_smem_acx_acy", 2, 2, {40, 47, 0xd3, 26, 27, 0x0}, 3, FALSE},
    {"mac_acy_tx_acx_acy", 2, 2, {41, 47, 0x2c, 33, 33, 1}, 2, TRUE},
    {"mov_smem_shift_acx", 2, 2, {40, 47, 0xf9, 18, 19, 0x2}, 4, FALSE},
    {"sub_k4_dst", 1, 2, {41, 47, 0x21}, 2, TRUE},
    {"mov_smem_acx", 1, 2, {40, 47, 0xdf}, 3, FALSE},
    {"mov_tax_reg", 2, 2, {41, 47, 0x29, 35, 35, 0x1}, 2, TRUE},
    {"or_acx_shift_acy", 2, 2, {41, 47, 0x8, 32, 35, 0x1}, 3, TRUE},
    {"and_k8_src_dst", 1, 2, {41, 47, 0xc}, 3, TRUE},
    {"mov_xsrc_xdst", 1, 2, {40, 47, 0x90}, 2, TRUE},
    {"rpt_csr", 2, 2, {41, 47, 0x24, 32, 34, 0x0}, 2, TRUE},
    {"mov_xmem_ymem", 2, 2, {40, 47, 0x80, 26, 27, 0x1}, 3, FALSE},
    {"mov_dblxmem_ydblmem", 2, 2, {40, 47, 0x80, 26, 27, 0x0}, 3, FALSE},
    {"b_l7", 2, 2, {41, 47, 0x25, 39, 39, 0x0}, 2, TRUE},
    {"sfts_acx_shift_acy", 2, 2, {41, 47, 0x8, 32, 35, 0x5}, 3, TRUE},
    {"add_src_dst", 1, 2, {41, 47, 0x12}, 2, TRUE},
    {"mov_k16_dst", 2, 2, {40, 47, 0x76, 18, 19, 0x2}, 4, TRUE},
    {"sub_src_dst", 1, 2, {41, 47, 0x13}, 2, TRUE},
    {"neg_src_dst", 1, 2, {41, 47, 0x1a}, 2, TRUE},
    {"sfts_acx_tx_acy", 2, 2, {41, 47, 0x2e, 32, 33, 0x1}, 2, TRUE},
    {"sftl_acx_tx_acy", 2, 2, {41, 47, 0x2e, 32, 33, 0x0}, 2, TRUE},
    {"sftl_dst_left", 2, 2, {41, 47, 0x28, 32, 34, 0x0}, 2, TRUE},
    {"sftl_dst_right", 2, 2, {41, 47, 0x28, 32, 34, 0x1}, 2, TRUE},
    {"or_src_dst", 1, 2, {41, 47, 0x15}, 2, TRUE},
    {"add_k4_dst", 1, 2, {41, 47, 0x20}, 2, TRUE},
    {"sfts_dst", 2, 2, {41, 47, 0x22, 38, 39, 0x1}, 2, TRUE},
    {"sftl_acx_shift_acy", 2, 2, {41, 47, 0x8, 32, 35, 0x7}, 3, TRUE},
    {"psh_src1_src2", 1, 2, {41, 47, 0x1c}, 2, TRUE},
    {"sub_k16_src_dst", 1, 2, {40, 47, 0x7c}, 4, FALSE},
    {"and_k16_src_dst", 1, 2, {40, 47, 0x7d}, 4, FALSE},
    {"add_k16_src_dst", 1, 2, {40, 47, 0x7b}, 4, FALSE},
    {"mov_src_smem", 1, 2, {44, 47, 0xc}, 2, FALSE},
    {"pop_dst1_dst2", 1, 2, {41, 47, 0x1d}, 2, TRUE},
    {"call_acx", 1, 2, {40, 47, 0x92}, 2, TRUE},
    {"mov_k12_dst", 1, 2, {41, 47, 0xb}, 3, TRUE},
    {"add_lmem_acx_acy", 2, 2, {40, 47, 0xed, 25, 27, 0x0}, 3, FALSE},
    {"sub_lmem_acx_acy", 2, 2, {40, 47, 0xed, 25, 27, 0x1}, 3, FALSE},
    {"mov_k8_abs_smem", 1, 2, {32, 47, 0xe611}, 5, FALSE},
    {"mov_k8_smem", 1, 2, {40, 47, 0xe6}, 3, FALSE},
    {"mov_tax_acx", 2, 2, {41, 47, 0x29, 34, 35, 0x0}, 2, TRUE},
    {"mpy_acx_acy", 2, 2, {41, 47, 0x2a, 33, 35, 0x3}, 2, TRUE},
    {"mov_acx_tax", 2, 2, {41, 47, 0x22, 38, 39, 0x0}, 2, TRUE},
    {"add_k16_abs_smem", 1, 2, {32, 47, 0xf711}, 4, FALSE},
    {"add_k16_smem", 1, 2, {40, 47, 0xf7}, 4, FALSE},
    {"and_k16_shift_acx_acy", 2, 2, {40, 47, 0x7a, 17, 19, 0x2}, 4, TRUE},
    {"mov_sp_tax", 2, 2, {41, 47, 0x22, 36, 39, 0x8}, 2, TRUE},
    {"rpt_k8", 1, 2, {41, 47, 0x26}, 2, TRUE},
    {"amar_smem", 1, 2, {40, 47, 0xb4}, 2, FALSE},
    {"macmk_smem_k8_acx_acy", 2, 2, {40, 47, 0xf8, 18, 18, 0x1}, 4, FALSE},
    {"add_smem_src_dst", 1, 2, {40, 47, 0xd6}, 4, FALSE},
    {"mov_smem_dst_mov_src_smem", 2, 2, {40, 47, 0x8a, 20, 23, 0xc}, 4, FALSE},
    {"mov_smem_dst_aadd_src_dst", 1, 2, {40, 47, 0x8a}, 5, FALSE},
    {"macm_xmem_ymem_acx_acy", 2, 2, {40, 47, 0x86, 21, 23, 0x1}, 4, FALSE},
    {"xor_k16_src_dst", 1, 2, {40, 47, 0x7f}, 4, FALSE},
    {"sub_acx_shiftw_acy", 2, 2, {41, 47, 0x8, 32, 35, 0x4}, 3, TRUE},
    {"mov_k16_abs_smem", 1, 2, {32, 47, 0xfb11}, 4, FALSE},
    {"mov_k16_smem", 1, 2, {40, 47, 0xfb}, 4, FALSE},
    {"call_l16", 1, 2, {41, 47, 0x4}, 3, TRUE},
    {"cmp_smem_k16_tcx", 1, 2, {40, 47, 0xf0}, 4, FALSE},
    {"cmp_smem_k16_tcx", 1, 2, {40, 47, 0xf1}, 4, FALSE},
    {"subc_smem_acx_acy", 2, 2, {40, 47, 0xde, 24, 27, 0x3}, 3, FALSE},
    {"abs_src_dst", 1, 2, {41, 47, 0x19}, 2, TRUE},
    {"sub_src_smem_dst", 1, 2, {40, 47, 0xd8}, 3, FALSE},
    {"mov_lmem_tax", 2, 2, {40, 47, 0xed, 25, 27, 0x7}, 3, FALSE},
    {"mov_xsrc_lmem", 2, 2, {40, 47, 0xeb, 24, 27, 0x5}, 3, FALSE},
    {"pshboth_xsrc", 2, 2, {41, 47, 0x28, 32, 35, 0x5}, 2, TRUE},
    {"btst_baddr_src_tcx", 2, 2, {40, 47, 0xec, 24, 27, 0x8}, 3, TRUE},
    {"btst_baddr_src_tcx", 2, 2, {40, 47, 0xec, 24, 27, 0x9}, 3, TRUE},
    {"popboth_xdst", 2, 2, {41, 47, 0x28, 32, 35, 0x4}, 2, TRUE},
    {"btst_k4_smem_tcx", 2, 2, {40, 47, 0xdc, 24, 25, 0x0}, 3, FALSE},
    {"btst_k4_smem_tcx", 2, 2, {40, 47, 0xdc, 24, 25, 0x1}, 3, FALSE},
    {"aadd_tax_tay", 2, 2, {41, 47, 0xa, 24, 27, 0x0}, 3, FALSE},
    {"aadd_tax_tay", 2, 2, {41, 47, 0xa, 24, 27, 0x8}, 3, FALSE},
    {"asub_p8_tax", 2, 2, {41, 47, 0xa, 24, 27, 0x6}, 3, FALSE},
    {"asub_p8_tax", 2, 2, {41, 47, 0xa, 24, 27, 0xe}, 3, FALSE},
    {"rptb_pmad", 1, 2, {41, 47, 0x07}, 3, TRUE},
    {"rptblocal_pmad", 1, 2, {41, 47, 0x25}, 2, TRUE},
    {"bfxtr_k16_acx_dst", 2, 2, {40, 47, 0x76, 18, 19, 0x0}, 4, TRUE},
    {"mov_smem_dst_aadd_p8_dst", 2, 2, {40, 47, 0x8d, 17, 23, 0xe}, 6, FALSE},
    {"or_abs_smem_src_dst", 1, 2, {32, 47, 0xda11}, 5, FALSE},
    {"or_smem_src_dst", 1, 2, {40, 47, 0xda}, 3, FALSE},
    {"bset_baddr_src", 2, 2, {40, 47, 0xec, 25, 27, 0x0}, 3, FALSE},
    {"mov_src_lmem_aadd_p8_dst", 2, 2, {40, 47, 0x8e, 17, 23, 0xa}, 6, FALSE},
    {"exp_acx_tx", 2, 2, {41, 47, 0x8, 32, 35, 0x8}, 3, TRUE},
    {"bnot_baddr_src", 2, 2, {40, 47, 0xec, 25, 27, 0x3}, 3, FALSE},
    {"intr_k5", 1, 2, {39, 47, 0x12a}, 2, FALSE},
    {"reti", 2, 2, {41, 47, 0x24, 32, 34, 0x5}, 2, FALSE},
    {"mov_k16_ssp", 2, 2, {40, 47, 0x78, 17, 20, 0x1}, 2, FALSE},
    {"and_abs_smem_src_dst", 1, 2, {32, 47, 0xd911}, 5, FALSE},
    {"and_smem_src_dst", 1, 2, {40, 47, 0xd9}, 3, FALSE},
    {"mack_tx_k8_acx_acy", 2, 2, {41, 47, 0xf, 25, 25, 0x1}, 3, FALSE},
    {"amov_tax_tay", 2, 2, {41, 47, 0xa, 24, 27, 0x1}, 3, TRUE},
    {"amov_tax_tay", 2, 2, {41, 47, 0xa, 24, 27, 0x9}, 3, TRUE},
    {"mack_tx_k16_acx_acy", 2, 2, {40, 47, 0x79, 17, 17, 0x1}, 4, FALSE},
    {"mov_smem_shift16_acx", 1, 2, {42, 47, 0x2c}, 2, FALSE},
    {"mpyk_k16_acx_acy", 2, 2, {40, 47, 0x79, 17, 17, 0x0}, 4, FALSE},
    {"band_smem_k16_tc1", 1, 2, {40, 47, 0xf2}, 4, FALSE},
    {"band_smem_k16_tc2", 1, 2, {40, 47, 0xf3}, 4, FALSE},
    {"psh_smem", 1, 2, {40, 47, 0xb5}, 2, FALSE},
    {"pop_smem", 1, 2, {40, 47, 0xbb}, 2, FALSE},
    {"psh_dbl_acx", 2, 2, {41, 47, 0x28, 32, 34, 0x7}, 2, TRUE},
    {"pop_dbl_acx", 2, 2, {41, 47, 0x28, 32, 34, 0x3}, 2, TRUE},
    {"sub_smem_src_dst", 1, 2, {40, 47, 0xd7}, 3, FALSE},
};

insn_action_t insn_action[] = {
    exec_xccpart_cond_mov_k4_dst,
    exec_xccpart_cond_mov_lmem_acx,
    exec_xccpart_cond_neg_src_dst,
    exec_xccpart_cond_add_k4_dst,
    exec_xccpart_cond_sfts_dst,
    exec_xccpart_cond_mov_k16_dst,
    exec_xccpart_cond_sftl_dst,
    exec_xccpart_cond_sftl_acx_shift_acy,
    exec_xccpart_cond_sub_k4_dst,
    exec_xccpart_cond_mov_k16_shift_acx,
    exec_xccpart_cond_mov_smem_dst,
    exec_xcc_cond_mov_lmem_acx,
    exec_xcc_cond_mov_src_smem,
    //  exec_ret_or_src_dst,
    exec_add_acx_shift_acy,
    exec_amar_smem_xadst,
    exec_amar_smem_xadst,
    exec_and_k16_abs_smem,
    exec_and_k16_smem,
    exec_or_k16_smem,
    exec_bclr_or_bset_k4_stx,
    exec_call_p24,
    exec_aadd_k8_sp,
    exec_aadd_p8_tax,
    exec_aadd_p8_tax,
    exec_not_src_dst,
    exec_bcc_l8_cond,
    exec_mov_smem_dst,
    exec_bcc_l4_cond,
    exec_mov_abs_lmem_acx,
    exec_mov_lmem_acx,
    exec_ret,
    exec_mov_k16_acx,
    exec_or_k16_src_dst,
    exec_cmp_src_dst_tcx,
    exec_cmp_src_dst_tcx,
    exec_nop,
    exec_mov_k4_dst,
    exec_mov_negk4_dst,
    exec_xcc_cond,
    exec_xcc_cond,
    exec_xcc_cond,
    exec_mov_acx_lmem,
    exec_xor_k16_smem,
    exec_bclr_baddr_src,
    exec_or_k8_src_dst,
    exec_bcc_l16_cond,
    exec_psh_src,
    exec_pop_dst,
    exec_xor_src_dst,
    exec_and_src_dst,
    exec_b_l16,
    exec_mov_src_dst,
    exec_mpym_smem_tx_acy,
    exec_mpym_smem_acx_acy,
    exec_mac_acy_tx_acx_acy,
    exec_mov_smem_shift_acx,
    exec_sub_k4_dst,
    exec_mov_smem_acx,
    exec_mov_tax_reg,
    exec_or_acx_shift_acy,
    exec_and_k8_src_dst,
    exec_mov_xsrc_xdst,
    exec_rpt_csr,
    exec_mov_xmem_ymem,
    exec_mov_dblxmem_dblymem,
    exec_b_l7,
    exec_sfts_acx_shift_acy,
    exec_add_src_dst,
    exec_mov_k16_dst,
    exec_sub_src_dst,
    exec_neg_src_dst,
    exec_sfts_acx_tx_acy,
    exec_sftl_acx_tx_acy,
    exec_sftl_dst_left,
    exec_sftl_dst_right,
    exec_or_src_dst,
    exec_add_k4_dst,
    exec_sfts_dst,
    exec_sftl_acx_shift_acy,
    exec_psh_src1_src2,
    exec_sub_k16_src_dst,
    exec_and_k16_src_dst,
    exec_add_k16_src_dst,
    exec_mov_src_smem,
    exec_pop_dst1_dst2,
    exec_call_acx,
    exec_mov_k12_dst,
    exec_add_lmem_acx_acy,
    exec_sub_lmem_acx_acy,
    exec_mov_k8_abs_smem,
    exec_mov_k8_smem,
    exec_mov_tax_acx,
    exec_mpy_acx_acy,
    exec_mov_acx_tax,
    exec_add_k16_abs_smem,
    exec_add_k16_smem,
    exec_and_k16_shift_acx_acy,
    exec_mov_sp_tax,
    exec_rpt_k8,
    exec_amar_smem,
    exec_macmk_smem_k8_acx_acy,
    exec_add_smem_src_dst,
    exec_mov_smem_dst_mov_src_smem,
    exec_mov_smem_dst_aadd_src_dst,
    exec_macm_xmem_ymem_acx_acy,
    exec_xor_k16_src_dst,
    exec_sub_acx_shift_acy,
    exec_mov_k16_abs_smem,
    exec_mov_k16_smem,
    exec_call_l16,
    exec_cmp_smem_k16_tcx,
    exec_cmp_smem_k16_tcx,
    exec_subc_smem_acx_acy,
    exec_abs_src_dst,
    exec_sub_src_smem_dst,
    exec_mov_lmem_tax,
    exec_mov_xsrc_lmem,
    exec_pshboth_xsrc,
    exec_btst_baddr_src_tcx,
    exec_btst_baddr_src_tcx,
    exec_popboth_xdst,
    exec_btst_k4_smem_tcx,
    exec_btst_k4_smem_tcx,
    exec_aadd_tax_tay,
    exec_aadd_tax_tay,
    exec_asub_p8_tax,
    exec_asub_p8_tax,
    exec_rptb_pmad,
    exec_rptblocal_pmad,
    exec_bfxtr_k16_acx_dst,
    exec_mov_smem_dst_aadd_p8_dst,
    exec_or_abs_smem_src_dst,
    exec_or_smem_src_dst,
    exec_bset_baddr_src,
    exec_mov_src_lmem_aadd_p8_dst,
    exec_exp_acx_tx,
    exec_bnot_baddr_src,
    exec_intr_k5,
    exec_reti,
    exec_mov_k16_ssp,
    exec_and_abs_smem_src_dst,
    exec_and_smem_src_dst,
    exec_mack_tx_k8_acx_acy,
    exec_amov_tax_tay,
    exec_amov_tax_tay,
    exec_mack_tx_k16_acx_acy,
    exec_mov_smem_shift16_acx,
    exec_mpyk_k16_acx_acy,
    exec_band_smem_k16_tcx,
    exec_band_smem_k16_tcx,
    exec_psh_smem,
    exec_pop_smem,
    exec_psh_dbl_acx,
    exec_pop_dbl_acx,
    exec_sub_smem_src_dst,
    NULL,
};
uint32_t exec_insn_cache(conf_object_t * conf_obj, c55_core_t * core, insn_cache_t * cache)
{
    uint64_t insn = cache->insn;

    cache->action(conf_obj, core, insn, &(cache->fields));

    core->insn_num++;

    return 0;
}

int decode_instr(c55_core_t * core, uint64_t insn, int32_t * idx, ISEITEM * table, int table_len)
{
    int n = 0;
    int base = 0;
    int ret = DECODE_FAILURE;
    int i = 0;
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
            core->PEB = slot->PEB;
            //              printf("In %s, insn :%llx, action:%s\n", __FUNCTION__, insn, slot->name);
            *idx = i;
            return ret;
        }
    }
    return ret;
}

insn_action_t get_insn_action(c55_core_t * core, uint64_t insn)
{
    int index;
    insn_action_t ret = NULL;

    if ((ret =
         decode_instr(core, insn, &index, insn32_decode_table,
                      sizeof (insn32_decode_table) / sizeof (ISEITEM))) == DECODE_SUCCESS)
    {
        DBG("In %s, instr=0x%x, index=0x%x\n", __FUNCTION__, insn, index);
        //      printf("index :%x, insn_action :%x\n", index, insn_action[index]);
        return insn_action[index];
    } else
    {
        printf("In %s, decode error for 0x%llx at 0x%x\n", __FUNCTION__, insn, core->pc);
        skyeye_exit(-1);
    }
}

int trans_opcode_fields(uint64_t insn, opcode_fields_t * fields)
{
    fields->x = BITS(31, 31);
    fields->x1 = BITS(30, 31);
    fields->x2 = BITS(20, 20);
    fields->f = BITS(39, 39);
    fields->f1 = BITS(43, 43);
    fields->fs = BITS(19, 19);
    fields->fd = BITS(23, 23);
    fields->fdst = BITS(16, 18);
    fields->fsrc = BITS(20, 22);
    fields->c = BITS(34, 35);
    /*ac[x] or ar[x] */
    fields->eq = BITS(40, 40);
    fields->m1 = BITS(35, 35);
    fields->a1 = BITS(33, 33);
    fields->xdst = BITS(28, 30);
    fields->smem = BITS(0, 22);
    fields->lmem = BITS(37, 39);
    fields->ymem = BITS(31, 33);
    fields->addr = BITS(33, 39);
    fields->src = BITS(36, 38);
    fields->acx = BITS(24, 26);
    fields->acy = BITS(28, 30);
    fields->dst = BITS(32, 34);
    fields->dst1 = BITS(40, 42);
    fields->shift = BITS(24, 29);
    /*k16 */
    fields->k16 = BITS(16, 31);
    fields->bk16 = BITS(24, 39);
    /*k4 */
    fields->k4 = BITS(36, 39);
    /*k8 */
    fields->k8 = BITS(32, 39);
    /*p24 */
    fields->p24 = BITS(16, 39);
    /*l8 */
    fields->l8 = BITS(24, 31);
    /*l7 */
    fields->l7 = BITS(32, 38);
    /*l16 */
    fields->l16 = BITS(16, 31);
    /*I4 */
    fields->I4 = BITS(39, 42);
    return 0;
}

int *get_xadst_register(c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int x = fields->x;
    int xdst = fields->xdst;

    if (x == 0)
    {
        switch (xdst)
        {
            case XCDP:
                return &core->xcdp;
                break;
            case XDP:
                return &core->xdp;
                break;
            case XSP:
                return &core->xsp;
                break;
            case XSSP:
                return &core->xssp;
                break;
            default:
                printf("In %s, %d, insn %lx decode error.\n", __FUNCTION__, __LINE__, insn);
                skyeye_exit(-1);
        }
    } else
    {
        return &core->xar[xdst].xar;
    }
}
uint16_t bit16_reverse(uint16_t src)
{
    src = (0xf - (src & 0xf)) | (0xf0 - (src & 0xf0)) | (0xf00 - (src & 0xf00)) | (0xf000 - (src & 0xf000));
    return src;
}

void mov_k4_dst(c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k4 = fields->k4;
    int fd = fields->m1;
    unsigned int dst = BITS(32, 34);

    if (fd == 0)
    {
        if (dst < 4)
        {
            core->ac[dst] = k4;
        } else
        {
            core->t[dst - 4] = k4;
        }
    } else
        core->xar[dst].ar = k4;

    return;
}

void mov_negk4_dst(c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k4 = fields->k4;
    int fd = fields->m1;
    unsigned int dst = BITS(32, 34);

    if (fd == 0)
    {
        if (dst < 4)
        {
            core->ac[dst] = -k4;
        } else
        {
            core->t[dst - 4] = -k4;
        }
    } else
        core->xar[dst].ar = -k4;

    return;
}

void mov_k8_dst(c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k8 = fields->k8;
    int fd = fields->m1;
    int dst = BITS(32, 34);

    if (fd == 0)
    {
        if (dst < 4)
        {
            core->ac[dst] = k8;
        } else
        {
            core->t[dst - 4] = k8;
        }
    } else
        core->xar[dst].ar = k8;

    return;
}

void mov_lmem_acx(c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int dst = BITS(28, 29);
    int addr_mode = BITS(32, 32);
    uint32_t mem32_val = 0;

    if (addr_mode)
        mem32_val = read_val_from_indirect_lmem_address(core, insn);
    else
        mem32_val = read_val_from_direct_lmem_address(core, insn);

    core->ac[dst] = mem32_val;
    /*
     * int x1 = fields->x1;
     * int a1 = fields->a1;
     * int eq = BITS(37, 37);
     * int lmem = fields->lmem;
     * int lmem1 = BITS(33, 39);
     * int dir_addr = fields->dir_addr;
     * uint16_t mem16_low, mem16_upper;
     * 
     * if (eq == 0){
     * if (a1 == 1){
     * mem16_low = c55_memory_read(core, core->xar[lmem].ar + 1);   
     * mem16_upper = c55_memory_read(core, core->xar[lmem].ar);     
     * core->ac[x1] = mem16_upper << 16 | mem16_low;
     * 
     * core->xar[lmem].ar += 2;
     * }else{
     * mem16_upper = c55_memory_read(core, lmem1 );
     * mem16_low = c55_memory_read(core, (lmem1 + 1) );
     * core->ac[x1] = (mem16_upper << 16) | mem16_low;
     * }
     * }else{
     * mem16_upper = c55_memory_read(core, dir_addr );
     * mem16_low = c55_memory_read(core, (dir_addr + 1) );
     * core->ac[x1] = (mem16_upper << 16) | mem16_low;
     * }
     */
    return;
}

void neg_src_dst(c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int src = fields->src;
    int dst = fields->dst;
    int fs = fields->f;
    int fd = fields->m1;

    if (fs == 1)
    {
        if (fd == 1)
        {
            core->xar[dst].ar = -core->xar[src].ar;
        } else
        {
            if (dst < 4)
            {
                core->ac[dst] = -core->xar[src].ar;
            } else
                core->t[dst - 4] = -core->xar[src].ar;
        }
        if (core->xar[src].ar == 0)
        {
            SET(core->st0_55, CARRY);
        }
    } else
    {
        if (fd == 1)
        {
            if (src < 4)
            {
                core->xar[dst].ar = -core->ac[src];
                if (core->ac[src] == 0)
                {
                    SET(core->st0_55, CARRY);
                }
            } else
            {
                core->xar[dst].ar = -core->t[src - 4];
                if (core->t[src - 4] == 0)
                {
                    SET(core->st0_55, CARRY);
                }
            }
        } else
        {
            if (dst < 4)
            {
                if (src < 4)
                {
                    core->ac[dst] = -core->ac[src];
                    if (core->ac[src] == 0)
                    {
                        SET(core->st0_55, CARRY);
                    }
                } else
                {
                    core->ac[dst] = -core->t[src - 4];
                    if (core->t[src - 4] == 0)
                    {
                        SET(core->st0_55, CARRY);
                    }
                }
            } else
            {
                if (src < 4)
                {
                    core->t[dst - 4] = -core->ac[src];
                    if (core->ac[src] == 0)
                    {
                        SET(core->st0_55, CARRY);
                    }
                } else
                {
                    core->t[dst - 4] = -core->t[src - 4];
                    if (core->t[src - 4] == 0)
                    {
                        SET(core->st0_55, CARRY);
                    }
                }
            }
        }
    }

    return;
}

void add_k4_dst(c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int dst = fields->dst;
    int fd = fields->m1;
    int k4 = fields->k4;

    if (fd == 1)
    {
        core->xar[dst].ar += k4;
    } else
    {
        if (dst < 4)
        {
            int x = bit(core->ac[dst], 31);
            int y = bit(k4, 31);

            core->ac[dst] += k4;
            int z = bit(core->ac[dst], 31);

            /* Carry conditions */
            if ((x && y && z == 0) || (!x && !y && z) || (!x && y && z == 0) || (x && !y && z == 0))
                SET(core->st0_55, CARRY);
            else
                CLEAR(core->st0_55, CARRY);
        } else
            core->t[dst - 4] += k4;
    }

    return;
}

void sfts_dst_right(c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int dst = fields->dst;
    int fd = fields->m1;

    if (fd == 1)
    {
        core->xar[dst].ar = (core->xar[dst].ar >> 1);
    } else
    {
        if (dst < 4)
        {
            core->ac[dst] = (core->ac[dst] >> 1);
        } else
            core->t[dst - 4] = (core->t[dst - 4] >> 1);
    }

    return;
}

void sfts_dst_left(c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int dst = fields->dst;
    int fd = fields->m1;

    if (fd == 1)
    {
        core->xar[dst].ar = (core->xar[dst].ar << 1);
    } else
    {
        if (dst < 4)
        {
            core->ac[dst] = (core->ac[dst] << 1);
        } else
            core->t[dst - 4] = (core->t[dst - 4] << 1);
    }

    return;
}

void sftl_dst_right(c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int dst = BITS(36, 38);
    int fd = BITS(39, 39);
    int x, y, z;

    if (fd == 1)
    {
        x = core->xar[dst].ar & 0x1;
        core->xar[dst].ar = (core->xar[dst].ar >> 1) & 0xffff;
        if (x)
        {
            SET(core->st0_55, CARRY);
        } else
            CLEAR(core->st0_55, CARRY);
    } else
    {
        if (dst < 4)
        {
            y = core->ac[dst] & 0x1;
            core->ac[dst] = (core->ac[dst] >> 1) & 0xffffffff;
            if (y)
            {
                SET(core->st0_55, CARRY);
            } else
                CLEAR(core->st0_55, CARRY);
        } else
        {
            z = core->t[dst - 4] & 0x1;
            core->t[dst - 4] = (core->t[dst - 4] >> 1) & 0xffff;
            if (z)
            {
                SET(core->st0_55, CARRY);
            } else
                CLEAR(core->st0_55, CARRY);
        }
    }

    return;
}

void sftl_dst_left(c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int dst = BITS(36, 38);
    int fd = BITS(39, 39);
    int m40 = bit(core->st1_55, M40);
    int x, y, z;

    if (fd == 1)
    {
        x = core->xar[dst].ar & (0x1 << 15);
        core->xar[dst].ar = (core->xar[dst].ar << 1) & 0xffff;
        if (x)
        {
            SET(core->st0_55, CARRY);
        } else
            CLEAR(core->st0_55, CARRY);
    } else
    {
        if (dst < 4)
        {
            if (m40 == 1)
                y = (core->ac[dst] >> 39) & 0x1;
            else
                y = (core->ac[dst] >> 31) & 0x1;
            core->ac[dst] = (core->ac[dst] << 1) & 0xffffffff;
            if (y)
            {
                SET(core->st0_55, CARRY);
            } else
                CLEAR(core->st0_55, CARRY);

        } else
        {
            z = (core->t[dst - 4] >> 15) & 0x1;
            core->t[dst - 4] = (core->t[dst - 4] << 1) & 0xffff;
            if (z)
            {
                SET(core->st0_55, CARRY);
            } else
                CLEAR(core->st0_55, CARRY);
        }
    }

    return;
}

void sftl_acx_shift_acy(c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int dst = BITS(38, 39);
    int src = BITS(36, 37);
    int shiftw = BITS(24, 29);
    int x;

    x = (core->ac[src] >> shiftw) & 0x1;
    if (shiftw <= 32)
        core->ac[dst] = (core->ac[src] << shiftw);
    else
        core->ac[dst] = core->ac[src] >> (0x40 - shiftw);

    if (x == 0)
    {
        CLEAR(core->st0_55, CARRY);
    } else
        SET(core->st0_55, CARRY);

    return;
}

void mov_k16_dst(c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k16 = BITS(24, 39);
    int eq = fields->fd;
    int dst = BITS(20, 22);

    if (eq == 0)
    {
        if (dst < 4)
        {
            core->ac[dst] = k16;
        } else
        {
            core->t[dst - 4] = k16;
        }
    } else
        core->xar[dst].ar = k16;

    return;
}

void sub_k4_dst(c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int k4 = BITS(36, 39);
    int dst = BITS(32, 34);
    int fd = BITS(35, 35);

    if (fd == 1)
    {
        core->xar[dst].ar -= k4;
    } else
    {
        if (dst < 4)
        {
            core->ac[dst] -= k4;
        } else
            core->t[dst - 4] -= k4;
    }

    return;
}

void mov_k16_acx(c55_core_t * core, uint64_t insn, opcode_fields_t * fields)
{
    int x2 = BITS(20, 21);
    int k16 = BITS(24, 39);

    core->ac[x2] = k16 << 16;

    return;
}

void Overflow_check(c55_core_t * core, uint64_t insn, uint32_t fdst)
{
    int m40 = bit(core->st1_55, M40);;

    if (m40 == 0)
    {
        if (core->ac[fdst] >> 32 & 0x1)
            switch (fdst)
            {
                case 0:
                    SET(core->st0_55, ACOV0);
                    break;
                case 1:
                    SET(core->st0_55, ACOV1);
                    break;
                case 2:
                    SET(core->st0_55, ACOV2);
                    break;
                case 3:
                    SET(core->st0_55, ACOV3);
                    break;
                default:
                    printf("In %s, %d, insn %lx decode error.\n", __FUNCTION__, __LINE__, insn);
                    skyeye_exit(-1);
            }
    } else
    {
        if (core->ac[fdst] >> 40 & 0x1)
            switch (fdst)
            {
                case 0:
                    SET(core->st0_55, ACOV0);
                    break;
                case 1:
                    SET(core->st0_55, ACOV1);
                    break;
                case 2:
                    SET(core->st0_55, ACOV2);
                    break;
                case 3:
                    SET(core->st0_55, ACOV3);
                    break;
                default:
                    printf("In %s, %d, insn %lx decode error.\n", __FUNCTION__, __LINE__, insn);
                    skyeye_exit(-1);
            }
    }

    return;
}

uint32_t read_val_from_indirect_lmem_address(c55_core_t * core, uint64_t insn)
{
    int op1 = BITS(33, 36);
    int op2 = BITS(37, 39);
    int addr = BITS(37, 39);
    int k16 = BITS(8, 23);
    int k23 = BITS(0, 23);
    uint16_t mem16_upper = 0;
    uint16_t mem16_low = 0;
    uint32_t mem32_val = 0;
    int arms = bit(core->st2_55, ARMS);

    switch (op1)
    {
        case 0:
            //*ARn
            mem16_upper = c55_memory_read(core, core->xar[addr].ar);
            mem16_low = c55_memory_read(core, (core->xar[addr].ar + 1));
            break;
        case 1:
            //*ARn+
            mem16_upper = c55_memory_read(core, core->xar[addr].ar);
            mem16_low = c55_memory_read(core, (core->xar[addr].ar + 1));
            core->xar[addr].ar += 2;
            break;
        case 2:
            //*ARn-
            mem16_upper = c55_memory_read(core, core->xar[addr].ar);
            mem16_low = c55_memory_read(core, (core->xar[addr].ar + 1));
            core->xar[addr].ar -= 2;
            break;
        case 3:
            //*(ARn+T0)
            mem16_upper = c55_memory_read(core, core->xar[addr].ar);
            mem16_low = c55_memory_read(core, (core->xar[addr].ar + 1));
            core->xar[addr].ar += core->t[0];
            break;
        case 4:
            //*(ARn-T0)
            mem16_upper = c55_memory_read(core, core->xar[addr].ar);
            mem16_low = c55_memory_read(core, (core->xar[addr].ar + 1));
            core->xar[addr].ar -= core->t[0];
            break;
        case 5:
            //*ARn(T0)
            mem16_upper = c55_memory_read(core, core->xar[addr].ar + core->t[0]);
            mem16_low = c55_memory_read(core, (core->xar[addr].ar + core->t[0] + 1));
            break;
        case 6:
            //*ARn(#k16)
            if ((k16 >> 15) & 0x1)
                k16 = (k16 - 0xFFFF) - 1;
            mem16_upper = c55_memory_read(core, core->xar[addr].ar + k16);
            mem16_low = c55_memory_read(core, (core->xar[addr].ar + k16 + 1));
            core->pc += 2;
            break;
        case 7:
            //*+ARn(#k16)
            core->xar[addr].ar += k16;
            mem16_upper = c55_memory_read(core, core->xar[addr].ar + k16);
            mem16_low = c55_memory_read(core, (core->xar[addr].ar + k16 + 1));
            core->pc += 2;
            break;
        case 8:
            switch (op2)
            {
                case 0:
                    //ABS16(k16)
                    mem16_upper = c55_memory_read(core, k16);
                    mem16_low = c55_memory_read(core, k16 + 1);
                    core->pc += 2;
                    break;
                case 1:
                    //*(#k23)
                    mem16_upper = c55_memory_read(core, k23);
                    mem16_low = c55_memory_read(core, k23 + 1);
                    core->pc += 3;
                    break;
                case 2:
                    //port(#k16)
                    break;
                case 3:
                    //*CDP
                    NOT_IMP;
                    break;
                case 4:
                    //*CDP+
                    NOT_IMP;
                    break;
                case 5:
                    //*CDP-
                    NOT_IMP;
                    break;
                case 6:
                    //*CDP(#k16)
                    NOT_IMP;
                    break;
                case 7:
                    NOT_IMP;
                    //*+CDP(#k16)
                    break;
                default:
                    NOT_IMP;
                    break;
            }
            break;
        case 9:
            //(*(ARn + T1), ARMS = 0)
            //*ARn(short(#1), ARMS = 1
            if (arms)
            {
                mem16_upper = c55_memory_read(core, core->xar[addr].ar + 1);
                mem16_low = c55_memory_read(core, (core->xar[addr].ar + 2));
            } else
            {
                mem16_upper = c55_memory_read(core, core->xar[addr].ar);
                mem16_low = c55_memory_read(core, (core->xar[addr].ar + 1));
                core->xar[addr].ar += core->t[1];
            }
            break;
        case 10:
            //(*(ARn - T1), ARMS = 0)
            //*ARn(short(#2), ARMS = 1
            if (arms)
            {
                mem16_upper = c55_memory_read(core, core->xar[addr].ar + 2);
                mem16_low = c55_memory_read(core, (core->xar[addr].ar + 3));
            } else
            {
                mem16_upper = c55_memory_read(core, core->xar[addr].ar);
                mem16_low = c55_memory_read(core, (core->xar[addr].ar + 1));
                core->xar[addr].ar -= core->t[1];
            }
            break;
        case 11:
            //(*ARn(T1), ARMS = 0)
            //*ARn(short(#3), ARMS = 1
            if (arms)
            {
                mem16_upper = c55_memory_read(core, core->xar[addr].ar + 3);
                mem16_low = c55_memory_read(core, (core->xar[addr].ar + 4));
            } else
            {
                mem16_upper = c55_memory_read(core, core->xar[addr].ar + core->t[1]);
                mem16_low = c55_memory_read(core, (core->xar[addr].ar + core->t[1] + 1));
            }
            break;
        case 12:
            //(*+ARn, ARMS = 0)
            //*ARn(short(#4), ARMS = 1
            if (arms)
            {
                mem16_upper = c55_memory_read(core, core->xar[addr].ar + 4);
                mem16_low = c55_memory_read(core, (core->xar[addr].ar + 5));
            } else
            {
                core->xar[addr].ar += 2;
                mem16_upper = c55_memory_read(core, core->xar[addr].ar);
                mem16_low = c55_memory_read(core, (core->xar[addr].ar + 1));
            }
            break;
        case 13:
            //(*-ARn, ARMS = 0)
            //*ARn(short(#5), ARMS = 1
            if (arms)
            {
                mem16_upper = c55_memory_read(core, core->xar[addr].ar + 5);
                mem16_low = c55_memory_read(core, (core->xar[addr].ar + 6));
            } else
            {
                core->xar[addr].ar -= 2;
                mem16_upper = c55_memory_read(core, core->xar[addr].ar);
                mem16_low = c55_memory_read(core, (core->xar[addr].ar + 1));
            }
            break;
        case 14:
            //(*ARn + T0B), ARMS = 0)
            //*ARn(short(#6), ARMS = 1
            if (arms)
            {
                mem16_upper = c55_memory_read(core, core->xar[addr].ar + 6);
                mem16_low = c55_memory_read(core, (core->xar[addr].ar + 7));
            } else
            {
                mem16_upper = c55_memory_read(core, core->xar[addr].ar);
                mem16_low = c55_memory_read(core, (core->xar[addr].ar + 1));
                core->xar[addr].ar += core->t[0];
            }
            break;
        case 15:
            //(*ARn - T0B), ARMS = 0)
            //*ARn(short(#7), ARMS = 1
            if (arms)
            {
                mem16_upper = c55_memory_read(core, core->xar[addr].ar + 7);
                mem16_low = c55_memory_read(core, (core->xar[addr].ar + 8));
            } else
            {
                mem16_upper = c55_memory_read(core, core->xar[addr].ar);
                mem16_low = c55_memory_read(core, (core->xar[addr].ar + 1));
                core->xar[addr].ar -= core->t[0];
            }
            /* When this bit-reverse operand is used, ARn cannot
             * be used as a circular pointer. If ARn is configured in
             * ST2_55 for circular addressing, the corresponding buffer
             * start address register value (BSAxx) is added to ARn, but
             * ARn is not modified so as to remain inside a circular buffer. */
            break;
        default:
            NOT_IMP;
            break;
    }

    mem32_val = (mem16_upper << 16) | mem16_low;
    return mem32_val;
}

uint32_t read_val_from_direct_lmem_address(c55_core_t * core, uint64_t insn)
{
    int smem = BITS(33, 39);
    uint16_t mem16_upper = 0;
    uint16_t mem16_low = 0;
    uint32_t mem32_val = 0;

    mem16_upper = c55_memory_read(core, (smem + core->sp));
    mem16_low = c55_memory_read(core, (smem + core->sp + 1));

    mem32_val = (mem16_upper << 16) | mem16_low;
    return mem32_val;
}

int check_cond_l4(c55_core_t * core, int cond, int src, int fs)
{
    int cond_flag = 0;

    switch (cond)
    {
        case 0:
            if (fs == 0)
            {
                if (src < 4)
                {
                    if ((core->ac[src] & 0xFFFFFFFF) == 0)
                    {
                        cond_flag = 1;
                    }
                } else
                {
                    if (core->t[src - 4] == 0)
                    {
                        cond_flag = 1;
                    }
                }
            } else
            {
                if (core->xar[src].ar == 0)
                {
                    cond_flag = 1;
                }
            }
            break;
        case 1:
            if (fs == 0)
            {
                if (src < 4)
                {
                    if ((core->ac[src] & 0xFFFFFFFF) != 0)
                    {
                        cond_flag = 1;
                    }
                } else
                {
                    if (core->t[src - 4] != 0)
                    {
                        cond_flag = 1;
                    }
                }
            } else
            {
                if (core->xar[src].ar != 0)
                {
                    cond_flag = 1;
                }
            }
            break;
        case 2:
            //src < 0
            if (fs == 0)
            {
                if (src < 4)
                {
                    if (((core->ac[src] >> 31) & 0x1) == 1 && (core->ac[src] != 0))
                    {
                        cond_flag = 1;
                    }
                } else
                {
                    if (((core->t[src - 4] >> 15) & 0x1) == 1 && (core->t[src - 4] != 0))
                    {
                        cond_flag = 1;
                    }
                }
            } else
            {
                if (((core->xar[src].ar >> 15) & 0x1) == 1 && (core->xar[src].ar != 0))
                {
                    cond_flag = 1;
                }
            }
            break;
        case 3:
            // src <= 0
            if (fs == 0)
            {
                if (src < 4)
                {
                    if (((core->ac[src] >> 31) & 0x1) == 1 || (core->ac[src] == 0))
                    {
                        cond_flag = 1;
                    }
                } else
                {
                    if (((core->t[src - 4] >> 15) & 0x1) == 1 || (core->t[src - 4] == 0))
                    {
                        cond_flag = 1;
                    }
                }
            } else
            {
                if (((core->xar[src].ar >> 15) & 0x1) == 1 || (core->xar[src].ar == 0))
                {
                    cond_flag = 1;
                }
            }
            break;
        case 4:
            // src > 0
            if (fs)
            {
                if (((core->xar[src].ar >> 15) & 0x1) == 0 && (core->xar[src].ar != 0))
                {
                    cond_flag = 1;
                }
            } else
            {
                if (src < 4)
                {
                    if (((core->ac[src] >> 31) & 0x1) == 0 && (core->ac[src] != 0))
                    {
                        cond_flag = 1;
                    }
                } else
                {
                    if (((core->t[src - 4] >> 15) & 0x1) == 0 && (core->t[src - 4] != 0))
                    {
                        cond_flag = 1;
                    }
                }
            }
            break;
        case 5:
            // src >= 0
            if (fs)
            {
                if (((core->xar[src].ar >> 15) & 0x1) == 0 || (core->xar[src].ar == 0))
                {
                    cond_flag = 1;
                }
            } else
            {
                if (src < 4)
                {
                    if (((core->ac[src] >> 31) & 0x1) == 0 || (core->ac[src] == 0))
                    {
                        cond_flag = 1;
                    }
                } else
                {
                    if (((core->t[src - 4] >> 15) & 0x1) == 0 || (core->t[src - 4] == 0))
                    {
                        cond_flag = 1;
                    }
                }
            }
            break;
        case 6:
            if (src == 0x4)
            {
                if (bit(core->st0_55, TC1))
                {
                    cond_flag = 1;
                }
            } else if (src == 0x5)
            {
                if (bit(core->st0_55, TC2))
                {
                    cond_flag = 1;
                }
            }
            break;
        case 7:
            if (src == 0x4)
            {
                if (bit(core->st0_55, TC1) == 0)
                {
                    cond_flag = 1;
                }
            } else if (src == 0x5)
            {
                if (bit(core->st0_55, TC2) == 0)
                {
                    cond_flag = 1;
                }
            } else
            {
                printf("In %s, decode error instruction at pc = 0x%x\n", __FUNCTION__, core->pc);
                skyeye_exit(-1);
            }
            break;
        default:
            printf("In %s, instruction is not implement at pc = 0x%x\n", __FUNCTION__, core->pc);
            skyeye_exit(-1);
    }
    return cond_flag;
}

uint16_t read_val_from_indirect_smem_address(c55_core_t * core, uint64_t insn)
{
    int op = BITS(33, 36);
    int addr = BITS(37, 39);
    int k16 = BITS(16, 31);
    int k23 = BITS(8, 31);
    uint16_t mem16_val = 0;
    int arms = bit(core->st2_55, ARMS);

    switch (op)
    {
        case 0:
            //*ARn
            mem16_val = c55_memory_read(core, core->xar[addr].ar);
            break;
        case 1:
            //*ARn+
            mem16_val = c55_memory_read(core, core->xar[addr].ar);
            core->xar[addr].ar += 1;
            break;
        case 2:
            //*ARn-
            mem16_val = c55_memory_read(core, core->xar[addr].ar);
            core->xar[addr].ar -= 1;
            break;
        case 3:
            //*(ARn+T0)
            mem16_val = c55_memory_read(core, core->xar[addr].ar);
            core->xar[addr].ar += core->t[0];
            break;
        case 4:
            //*(ARn-T0)
            mem16_val = c55_memory_read(core, core->xar[addr].ar);
            core->xar[addr].ar -= core->t[0];
            break;
        case 5:
            //*ARn(T0)
            mem16_val = c55_memory_read(core, core->xar[addr].ar + core->t[0]);
            break;
        case 6:
            //*ARn(#k16)
            if ((k16 >> 15) & 0x1)
                k16 = (k16 - 0xFFFF) - 1;
            mem16_val = c55_memory_read(core, core->xar[addr].ar + k16);
            core->pc += 2;
            break;
        case 7:
            //*+ARn(#k16)
            core->xar[addr].ar += k16;
            mem16_val = c55_memory_read(core, (k16 + core->xar[addr].ar));
            core->pc += 2;
            break;
        case 8:
            switch (addr)
            {
                case 0:
                    //ABS16(k16)
                    mem16_val = c55_memory_read(core, k16);
                    core->pc += 2;
                    break;
                case 1:
                    //*(#k23)
                    mem16_val = c55_memory_read(core, k23);
                    core->pc += 3;
                    break;
                case 2:
                    //port(#k16)
                    break;
                case 3:
                    //*CDP
                    NOT_IMP;
                    break;
                case 4:
                    //*CDP+
                    NOT_IMP;
                    break;
                case 5:
                    //*CDP-
                    NOT_IMP;
                    break;
                case 6:
                    //*CDP(#k16)
                    NOT_IMP;
                    break;
                case 7:
                    NOT_IMP;
                    //*+CDP(#k16)
                    break;
                default:
                    NOT_IMP;
                    break;
            }
            break;
        case 9:
            //(*(ARn + T1), ARMS = 0)
            //*ARn(short(#1), ARMS = 1
            if (arms)
                mem16_val = c55_memory_read(core, core->xar[addr].ar + 1);
            else
            {
                mem16_val = c55_memory_read(core, core->xar[addr].ar);
                core->xar[addr].ar += core->t[1];
            }
            break;
        case 10:
            //(*(ARn - T1), ARMS = 0)
            //*ARn(short(#2), ARMS = 1
            if (arms)
                mem16_val = c55_memory_read(core, core->xar[addr].ar + 2);
            else
            {
                mem16_val = c55_memory_read(core, core->xar[addr].ar);
                core->xar[addr].ar -= core->t[1];
            }
            break;
        case 11:
            //(*ARn(T1), ARMS = 0)
            //*ARn(short(#3), ARMS = 1
            if (arms)
                mem16_val = c55_memory_read(core, core->xar[addr].ar + 3);
            else
                mem16_val = c55_memory_read(core, core->xar[addr].ar + core->t[1]);
            break;
        case 12:
            //(*+ARn, ARMS = 0)
            //*ARn(short(#4), ARMS = 1
            if (arms)
                mem16_val = c55_memory_read(core, core->xar[addr].ar + 4);
            else
            {
                core->xar[addr].ar += 1;
                mem16_val = c55_memory_read(core, core->xar[addr].ar);
            }
            break;
        case 13:
            //(*-ARn, ARMS = 0)
            //*ARn(short(#5), ARMS = 1
            if (arms)
                mem16_val = c55_memory_read(core, core->xar[addr].ar + 5);
            else
            {
                core->xar[addr].ar -= 1;
                mem16_val = c55_memory_read(core, core->xar[addr].ar);
            }
            break;
        case 14:
            //(*ARn + T0B), ARMS = 0)
            //*ARn(short(#6), ARMS = 1
            if (arms)
                mem16_val = c55_memory_read(core, core->xar[addr].ar + 6);
            else
            {
                mem16_val = c55_memory_read(core, core->xar[addr].ar);
                core->xar[addr].ar += core->t[0];
            }
            break;
        case 15:
            //(*ARn - T0B), ARMS = 0)
            //*ARn(short(#7), ARMS = 1
            if (arms)
                mem16_val = c55_memory_read(core, core->xar[addr].ar + 7);
            else
            {
                mem16_val = c55_memory_read(core, core->xar[addr].ar);
                core->xar[addr].ar -= core->t[0];
            }
            /* When this bit-reverse operand is used, ARn cannot
             * be used as a circular pointer. If ARn is configured in
             * ST2_55 for circular addressing, the corresponding buffer
             * start address register value (BSAxx) is added to ARn, but
             * ARn is not modified so as to remain inside a circular buffer. */
            break;
        default:
            NOT_IMP;
            break;
    }
    return mem16_val;
}

uint16_t read_val_from_direct_smem_address(c55_core_t * core, uint64_t insn)
{
    int mmap_op = BITS(24, 31);
    int smem = BITS(33, 39);
    uint16_t mem16_val = 0;

    if (mmap_op == 0x98)
    {                                   //mmap(smem)
        switch (smem)
        {
            case AC0_ADDR:
                mem16_val = core->ac[0] & 0xFFFF;
                break;
            case AC1_ADDR:
                mem16_val = core->ac[1] & 0xFFFF;
                break;
            case ST0_ADDR:
                mem16_val = core->st0_55;
                break;
            case ST1_ADDR:
                mem16_val = core->st1_55;
                break;
            case ST2_ADDR:
                mem16_val = core->st2_55;
                break;
            case ST3_ADDR:
                mem16_val = core->st3_55;
                break;
            case DPH_ADDR:
                mem16_val = core->dph;
                break;
            case BKC_ADDR:
                mem16_val = core->bkc;
                break;
            case BK03_ADDR:
                mem16_val = core->bk03;
                break;
            case BK47_ADDR:
                mem16_val = core->bk47;
                break;
            case BRC0_ADDR:
                mem16_val = core->brc0;
                break;
            case RSA0L_ADDR:
                mem16_val = core->rsa0 & 0xFFFF;
                break;
            case RSA0H_ADDR:
                mem16_val = (core->rsa0 >> 16) & 0xFF;
                break;
            case REA0L_ADDR:
                mem16_val = core->rea0 & 0xFFFF;
                break;
            case REA0H_ADDR:
                mem16_val = (core->rea0 >> 16) & 0xFF;
                break;
            case BRS1_ADDR:
                mem16_val = core->brs1;
                break;
            case BRC1_ADDR:
                mem16_val = core->brc1;
                break;
            case RSA1L_ADDR:
                mem16_val = core->rsa1 & 0xFFFF;
                break;
            case RSA1H_ADDR:
                mem16_val = (core->rsa1 >> 16) & 0xFF;
                break;
            case REA1L_ADDR:
                mem16_val = core->rea1 & 0xFFFF;
                break;
            case REA1H_ADDR:
                mem16_val = (core->rea1 >> 16) & 0xFF;
                break;
            case CSR_ADDR:
                mem16_val = core->csr;
                break;
            case RPTC_ADDR:
                mem16_val = core->rptc;
                break;
            case TRN0_ADDR:
                mem16_val = core->trn0;
                break;
            case TRN1_ADDR:
                mem16_val = core->trn1;
                break;
            case BSA01_ADDR:
                mem16_val = core->bsa01;
                break;
            case BSA23_ADDR:
                mem16_val = core->bsa23;
                break;
            case BSA45_ADDR:
                mem16_val = core->bsa45;
                break;
            case BSA67_ADDR:
                mem16_val = core->bsa67;
                break;
            case BSAC_ADDR:
                mem16_val = core->bsac;
                break;
              defalt:
                if (smem >= AR0_ADDR && smem <= AR7_ADDR)
                {
                    mem16_val = core->xar[smem - AR0_ADDR].ar;
                    break;
                } else
                {
                    printf("In %s, not decode this instruction correct at pc = 0x%x\n", __func__, core->pc);
                    skyeye_exit(-1);
                }
        }
        core->pc += 1;
    } else
        mem16_val = c55_memory_read(core, (smem + core->sp));

    return mem16_val;
}

void write_val_to_indirect_lmem_address(c55_core_t * core, uint64_t insn, uint32_t mem32_val)
{
    int op = BITS(33, 36);
    int addr = BITS(37, 39);
    int k16 = BITS(8, 23);
    int k23 = BITS(0, 23);
    int arms = bit(core->st2_55, ARMS);

    switch (op)
    {
        case 0:
            //*ARn
            c55_memory_write(core, core->xar[addr].ar, mem32_val >> 16);
            c55_memory_write(core, core->xar[addr].ar + 1, mem32_val & 0xFFFF);
            break;
        case 1:
            //*ARn+
            c55_memory_write(core, core->xar[addr].ar, mem32_val >> 16);
            c55_memory_write(core, core->xar[addr].ar + 1, mem32_val & 0xFFFF);
            core->xar[addr].ar += 2;
            break;
        case 2:
            //*ARn-
            c55_memory_write(core, core->xar[addr].ar, mem32_val >> 16);
            c55_memory_write(core, core->xar[addr].ar + 1, mem32_val & 0xFFFF);
            core->xar[addr].ar -= 2;
            break;
        case 3:
            //*(ARn+T0)
            c55_memory_write(core, core->xar[addr].ar, mem32_val >> 16);
            c55_memory_write(core, core->xar[addr].ar + 1, mem32_val & 0xFFFF);
            core->xar[addr].ar += core->t[0];
            break;
        case 4:
            //*(ARn-T0)
            c55_memory_write(core, core->xar[addr].ar, mem32_val >> 16);
            c55_memory_write(core, core->xar[addr].ar + 1, mem32_val & 0xFFFF);
            core->xar[addr].ar -= core->t[0];
            break;
        case 5:
            //*ARn(T0)
            c55_memory_write(core, core->xar[addr].ar + core->t[0], mem32_val >> 16);
            c55_memory_write(core, core->xar[addr].ar + core->t[0] + 1, mem32_val & 0xFFFF);
            break;
        case 6:
            //*ARn(#k16)
            if ((k16 >> 15) & 0x1)
                k16 = (k16 - 0xFFFF) - 1;
            c55_memory_write(core, core->xar[addr].ar + k16, mem32_val >> 16);
            c55_memory_write(core, core->xar[addr].ar + k16 + 1, mem32_val & 0xFFFF);
            core->pc += 2;
            break;
        case 7:
            //*+ARn(#k16)
            core->xar[addr].ar += k16;
            c55_memory_write(core, core->xar[addr].ar + k16, mem32_val >> 16);
            c55_memory_write(core, core->xar[addr].ar + k16 + 1, mem32_val & 0xFFFF);
            core->pc += 2;
            break;
        case 8:
            switch (addr)
            {
                case 0:
                    //ABS16(k16)
                    c55_memory_write(core, k16, mem32_val >> 16);
                    c55_memory_write(core, k16 + 1, mem32_val & 0xFFFF);
                    core->pc += 2;
                    break;
                case 1:
                    //*(#k23)
                    c55_memory_write(core, k23, mem32_val >> 16);
                    c55_memory_write(core, k23 + 1, mem32_val & 0xFFFF);
                    core->pc += 3;
                    break;
                case 2:
                    //port(#k16)
                    break;
                case 3:
                    //*CDP
                    NOT_IMP;
                    break;
                case 4:
                    //*CDP+
                    NOT_IMP;
                    break;
                case 5:
                    //*CDP-
                    NOT_IMP;
                    break;
                case 6:
                    //*CDP(#k16)
                    NOT_IMP;
                    break;
                case 7:
                    NOT_IMP;
                    //*+CDP(#k16)
                    break;
                default:
                    NOT_IMP;
                    break;
            }
            break;
        case 9:
            //(*(ARn + T1), ARMS = 0)
            //*ARn(short(#1), ARMS = 1
            if (arms)
            {
                c55_memory_write(core, core->xar[addr].ar + 1, mem32_val >> 16);
                c55_memory_write(core, core->xar[addr].ar + 2, mem32_val & 0xFFFF);
            } else
            {
                c55_memory_write(core, core->xar[addr].ar, mem32_val >> 16);
                c55_memory_write(core, core->xar[addr].ar + 1, mem32_val & 0xFFFF);
                core->xar[addr].ar += core->t[1];
            }
            break;
        case 10:
            //(*(ARn - T1), ARMS = 0)
            //*ARn(short(#2), ARMS = 1
            if (arms)
            {
                c55_memory_write(core, core->xar[addr].ar + 2, mem32_val >> 16);
                c55_memory_write(core, core->xar[addr].ar + 3, mem32_val & 0xFFFF);
            } else
            {
                c55_memory_write(core, core->xar[addr].ar, mem32_val >> 16);
                c55_memory_write(core, core->xar[addr].ar + 1, mem32_val & 0xFFFF);
                core->xar[addr].ar -= core->t[1];
            }
            break;
        case 11:
            //(*ARn(T1), ARMS = 0)
            //*ARn(short(#3), ARMS = 1
            if (arms)
            {
                c55_memory_write(core, core->xar[addr].ar + 3, mem32_val >> 16);
                c55_memory_write(core, core->xar[addr].ar + 4, mem32_val & 0xFFFF);
            } else
            {
                c55_memory_write(core, core->xar[addr].ar, mem32_val >> 16);
                c55_memory_write(core, core->xar[addr].ar + 1, mem32_val & 0xFFFF);
            }
            break;
        case 12:
            //(*+ARn, ARMS = 0)
            //*ARn(short(#4), ARMS = 1
            if (arms)
            {
                c55_memory_write(core, core->xar[addr].ar + 4, mem32_val >> 16);
                c55_memory_write(core, core->xar[addr].ar + 5, mem32_val & 0xFFFF);
            } else
            {
                core->xar[addr].ar += 2;
                c55_memory_write(core, core->xar[addr].ar, mem32_val >> 16);
                c55_memory_write(core, core->xar[addr].ar + 1, mem32_val & 0xFFFF);
            }
            break;
        case 13:
            //(*-ARn, ARMS = 0)
            //*ARn(short(#5), ARMS = 1
            if (arms)
            {
                c55_memory_write(core, core->xar[addr].ar + 5, mem32_val >> 16);
                c55_memory_write(core, core->xar[addr].ar + 6, mem32_val & 0xFFFF);
            } else
            {
                core->xar[addr].ar -= 2;
                c55_memory_write(core, core->xar[addr].ar, mem32_val >> 16);
                c55_memory_write(core, core->xar[addr].ar + 1, mem32_val & 0xFFFF);
            }
            break;
        case 14:
            //(*ARn + T0B), ARMS = 0)
            //*ARn(short(#6), ARMS = 1
            if (arms)
            {
                c55_memory_write(core, core->xar[addr].ar + 6, mem32_val >> 16);
                c55_memory_write(core, core->xar[addr].ar + 7, mem32_val & 0xFFFF);
            } else
            {
                c55_memory_write(core, core->xar[addr].ar, mem32_val >> 16);
                c55_memory_write(core, core->xar[addr].ar + 1, mem32_val & 0xFFFF);
                core->xar[addr].ar += core->t[0];
            }
            break;
        case 15:
            //(*ARn - T0B), ARMS = 0)
            //*ARn(short(#7), ARMS = 1
            if (arms)
            {
                c55_memory_write(core, core->xar[addr].ar + 7, mem32_val >> 16);
                c55_memory_write(core, core->xar[addr].ar + 8, mem32_val & 0xFFFF);
            } else
            {
                c55_memory_write(core, core->xar[addr].ar, mem32_val >> 16);
                c55_memory_write(core, core->xar[addr].ar + 1, mem32_val & 0xFFFF);
                core->xar[addr].ar -= core->t[0];
            }
            /* When this bit-reverse operand is used, ARn cannot
             * be used as a circular pointer. If ARn is configured in
             * ST2_55 for circular addressing, the corresponding buffer
             * start address register value (BSAxx) is added to ARn, but
             * ARn is not modified so as to remain inside a circular buffer. */
            break;
        default:
            NOT_IMP;
            break;
    }
}

void write_val_to_direct_lmem_address(c55_core_t * core, uint64_t insn, uint32_t mem32_val)
{
    int smem = BITS(33, 39);

    c55_memory_write(core, (smem + core->sp), mem32_val >> 16);
    c55_memory_write(core, (smem + core->sp + 1), mem32_val & 0xffff);

}

void write_val_to_indirect_smem_address(c55_core_t * core, uint64_t insn, uint16_t mem16_val)
{
    int op = BITS(33, 36);
    int addr = BITS(37, 39);
    int k16 = BITS(16, 31);
    int k23 = BITS(8, 31);
    int arms = bit(core->st2_55, ARMS);

    switch (op)
    {
        case 0:
            //*ARn
            c55_memory_write(core, core->xar[addr].ar, mem16_val);
            break;
        case 1:
            //*ARn+
            c55_memory_write(core, core->xar[addr].ar, mem16_val);
            core->xar[addr].ar += 1;
            break;
        case 2:
            //*ARn-
            c55_memory_write(core, core->xar[addr].ar, mem16_val);
            core->xar[addr].ar -= 1;
            break;
        case 3:
            //*(ARn+T0)
            c55_memory_write(core, core->xar[addr].ar, mem16_val);
            core->xar[addr].ar += core->t[0];
            break;
        case 4:
            //*(ARn-T0)
            c55_memory_write(core, core->xar[addr].ar, mem16_val);
            core->xar[addr].ar -= core->t[0];
            break;
        case 5:
            //*ARn(T0)
            c55_memory_write(core, core->xar[addr].ar + core->t[0], mem16_val);
            break;
        case 6:
            //*ARn(#k16)
            if ((k16 >> 15) & 0x1)
                k16 = (k16 - 0xFFFF) - 1;
            c55_memory_write(core, core->xar[addr].ar + k16, mem16_val);
            core->pc += 2;
            break;
        case 7:
            //*+ARn(#k16)
            core->xar[addr].ar += k16;
            c55_memory_write(core, core->xar[addr].ar + k16, mem16_val);
            core->pc += 2;
            break;
        case 8:
            switch (addr)
            {
                case 0:
                    //ABS16(k16)
                    c55_memory_write(core, k16, mem16_val);
                    core->pc += 2;
                    break;
                case 1:
                    //*(#k23)
                    c55_memory_write(core, k23, mem16_val);
                    core->pc += 3;
                    break;
                case 2:
                    //port(#k16)
                    break;
                case 3:
                    //*CDP
                    NOT_IMP;
                    break;
                case 4:
                    //*CDP+
                    NOT_IMP;
                    break;
                case 5:
                    //*CDP-
                    NOT_IMP;
                    break;
                case 6:
                    //*CDP(#k16)
                    NOT_IMP;
                    break;
                case 7:
                    NOT_IMP;
                    //*+CDP(#k16)
                    break;
                default:
                    NOT_IMP;
                    break;
            }
            break;
        case 9:
            //(*(ARn + T1), ARMS = 0)
            //*ARn(short(#1), ARMS = 1
            if (arms)
                c55_memory_write(core, core->xar[addr].ar + 1, mem16_val);
            else
            {
                c55_memory_write(core, core->xar[addr].ar, mem16_val);
                core->xar[addr].ar += core->t[1];
            }
            break;
        case 10:
            //(*(ARn - T1), ARMS = 0)
            //*ARn(short(#2), ARMS = 1
            if (arms)
                c55_memory_write(core, core->xar[addr].ar + 2, mem16_val);
            else
            {
                c55_memory_write(core, core->xar[addr].ar, mem16_val);
                core->xar[addr].ar -= core->t[1];
            }
            break;
        case 11:
            //(*ARn(T1), ARMS = 0)
            //*ARn(short(#3), ARMS = 1
            if (arms)
                c55_memory_write(core, core->xar[addr].ar + 3, mem16_val);
            else
                c55_memory_write(core, core->xar[addr].ar, mem16_val);
            break;
        case 12:
            //(*+ARn, ARMS = 0)
            //*ARn(short(#4), ARMS = 1
            if (arms)
                c55_memory_write(core, core->xar[addr].ar + 4, mem16_val);
            else
            {
                core->xar[addr].ar += 1;
                c55_memory_write(core, core->xar[addr].ar, mem16_val);
            }
            break;
        case 13:
            //(*-ARn, ARMS = 0)
            //*ARn(short(#5), ARMS = 1
            if (arms)
                c55_memory_write(core, core->xar[addr].ar + 5, mem16_val);
            else
            {
                core->xar[addr].ar -= 1;
                c55_memory_write(core, core->xar[addr].ar, mem16_val);
            }
            break;
        case 14:
            //(*ARn + T0B), ARMS = 0)
            //*ARn(short(#6), ARMS = 1
            if (arms)
                c55_memory_write(core, core->xar[addr].ar + 6, mem16_val);
            else
            {
                c55_memory_write(core, core->xar[addr].ar, mem16_val);
                core->xar[addr].ar += core->t[0];
            }
            break;
        case 15:
            //(*ARn - T0B), ARMS = 0)
            //*ARn(short(#7), ARMS = 1
            if (arms)
                c55_memory_write(core, core->xar[addr].ar + 7, mem16_val);
            else
            {
                c55_memory_write(core, core->xar[addr].ar, mem16_val);
                core->xar[addr].ar -= core->t[0];
            }
            /* When this bit-reverse operand is used, ARn cannot
             * be used as a circular pointer. If ARn is configured in
             * ST2_55 for circular addressing, the corresponding buffer
             * start address register value (BSAxx) is added to ARn, but
             * ARn is not modified so as to remain inside a circular buffer. */
            break;
        default:
            NOT_IMP;
            break;
    }
}

void write_val_to_direct_smem_address(c55_core_t * core, uint64_t insn, uint16_t mem16_val)
{
    int mmap_op = BITS(24, 31);
    int smem = BITS(33, 39);

    if (mmap_op == 0x98)
    {                                   //mmap(smem)
        switch (smem)
        {
            case AC0_ADDR:
                core->ac[0] = (core->ac[0] & 0xFFFFFF0000) | mem16_val;
                break;
            case AC0G_ADDR:
                core->ac[0] = (core->ac[0] & 0xFFFFFFFF) | ((uint64_t) mem16_val << 32);
                break;
            case AC1_ADDR:
                core->ac[1] = (core->ac[1] & 0xFFFFFF0000) | mem16_val;
                break;
            case AC1G_ADDR:
                core->ac[1] = (core->ac[1] & 0xFFFFFFFF) | ((uint64_t) mem16_val << 32);
                break;
            case AC2_ADDR:
                core->ac[2] = (core->ac[2] & 0xFFFFFF0000) | mem16_val;
                break;
            case AC2G_ADDR:
                core->ac[2] = (core->ac[2] & 0xFFFFFFFF) | ((uint64_t) mem16_val << 32);
                break;
            case AC3_ADDR:
                core->ac[3] = (core->ac[3] & 0xFFFFFF0000) | mem16_val;
                break;
            case AC3G_ADDR:
                core->ac[3] = (core->ac[3] & 0xFFFFFFFF) | ((uint64_t) mem16_val << 32);
                break;
            case ST0_ADDR:
                core->st0_55 = mem16_val;
                break;
            case ST1_ADDR:
                core->st1_55 = mem16_val;
                break;
            case ST2_ADDR:
                core->st2_55 = mem16_val;
                break;
            case ST3_ADDR:
                core->st3_55 = mem16_val;
                break;
            case DPH_ADDR:
                core->dph = mem16_val;
                break;
            case BKC_ADDR:
                core->bkc = mem16_val;
                break;
            case BK03_ADDR:
                core->bk03 = mem16_val;
                break;
            case BK47_ADDR:
                core->bk47 = mem16_val;
                break;
            case BRC0_ADDR:
                core->brc0 = mem16_val;
                break;
            case RSA0L_ADDR:
                core->rsa0 = (core->rsa0 & 0xFFFF0000) | mem16_val;
                break;
            case RSA0H_ADDR:
                core->rsa0 = mem16_val << 16;
                break;
            case REA0L_ADDR:
                core->rea0 = (core->rsa0 & 0xFFFF0000) | mem16_val;
                break;
            case REA0H_ADDR:
                core->rea0 = mem16_val << 16;
                break;
            case BRS1_ADDR:
                core->brs1 = mem16_val;
                break;
            case BRC1_ADDR:
                core->brc1 = mem16_val;
                break;
            case RSA1L_ADDR:
                core->rsa1 = (core->rsa1 & 0xFFFF0000) | mem16_val;
                break;
            case RSA1H_ADDR:
                core->rsa1 = mem16_val << 16;
                break;
            case REA1L_ADDR:
                core->rea1 = (core->rsa1 & 0xFFFF0000) | mem16_val;
                break;
            case REA1H_ADDR:
                core->rea1 = mem16_val << 16;
                break;
            case CSR_ADDR:
                core->csr = mem16_val;
                break;
            case RPTC_ADDR:
                core->rptc = mem16_val;
                break;
            case TRN0_ADDR:
                core->trn0 = mem16_val;
                break;
            case TRN1_ADDR:
                core->trn1 = mem16_val;
                break;
            case BSA01_ADDR:
                core->bsa01 = mem16_val;
                break;
            case BSA23_ADDR:
                core->bsa23 = mem16_val;
                break;
            case BSA45_ADDR:
                core->bsa45 = mem16_val;
                break;
            case BSA67_ADDR:
                core->bsa67 = mem16_val;
                break;
            case BSAC_ADDR:
                core->bsac = mem16_val;
                break;
              defalt:
                if (smem >= AR0_ADDR && smem <= AR7_ADDR)
                {
                    core->xar[smem - AR0_ADDR].ar = mem16_val;
                    break;
                } else
                {
                    printf("In %s, not decode this instruction correct at pc = 0x%x\n", __func__, core->pc);
                    skyeye_exit(-1);
                }
        }
        c55_memory_write(core, smem, mem16_val);
        core->pc += 1;
    } else
    {
        c55_memory_write(core, (smem + core->sp), mem16_val);
    }

}
int c55_memory_read(c55_core_t * core, uint32_t addr)
{
    uint16_t data;

    memory_space_intf *io_memory = core->c55_memory_intf;

    io_memory->read(core->obj, addr, &data, 2);
    return data & 0xFFFF;
}

void c55_memory_write(c55_core_t * core, uint32_t addr, uint16_t buffer)
{
    memory_space_intf *io_memory = core->c55_memory_intf;

    if (io_memory->write(core->obj, addr, &buffer, 2) != 0)
    {
        skyeye_log(Error_log, __func__, "Write %x to physical address %x error!\n", buffer, addr);
    }
    switch (addr)
    {
        case IER_ADDR:
            core->ier = buffer;
            break;
        case IVPD_ADDR:
            core->ivpd = buffer;
            break;
        case IVPH_ADDR:
            core->ivph = buffer;
            break;
    }
    return;
}

exception_t check_parallel_insn(conf_object_t * conf_obj, c55_core_t * core)
{
    int addr = core->pc;
    insn_cache_t *cache = &(core->insn_cache[HASH(addr)]);
    int status;

    uint64_t insn;
    uint32_t insn1;

    cache->addr = addr;
    status = c55x_space_read(conf_obj, addr, &insn, 32 / 8);
    status = c55x_space_read(conf_obj, addr + 4, &insn1, 32 / 8);
    insn = (insn << 16) | (insn1 >> 16);
    cache->insn = insn;
    trans_opcode_fields(insn, &(cache->fields));
    cache->action = get_insn_action(core, insn);
    int peb_flag = BITS(40, 40);

    if (core->PEB == TRUE && peb_flag == 1)
    {
        exec_insn_cache(conf_obj, core, cache);
    }
    if (core->pc == 0)
    {
        skyeye_log(Error_log, __func__, "Get the error pc value\n");
        skyeye_exit(-1);
    }

    return No_exp;
}
