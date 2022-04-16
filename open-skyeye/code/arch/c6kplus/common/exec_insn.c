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

#include "c6k_cpu.h"
#include "c6k_decode.h"
#include <stdio.h>
#include <stdlib.h>
#include "skyeye_types.h"
#include "skyeye_bus.h"
#include "skyeye_uart_ops.h"
#include "skyeye_interface.h"
#include <math.h>
#include <sim_control.h>

#define DEBUG
#include <skyeye_log.h>

#ifdef DBG
#undef DBG
//#define DBG(fmt, ...) do { fprintf(stderr, fmt, ## __VA_ARGS__); } while (0)
#define DBG(fmt, ...)
#endif

#if 0
#	define PRINT_ALL_GPR_TO_LOG(core)	print_all_gpr_to_log(core)
#else
#	define PRINT_ALL_GPR_TO_LOG(core)
#endif

static int exec_32b_insn(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields, int64_t * delay_slot);
static void print_all_gpr_to_log(c6k_core_t * core);
static void register_regw_event(c6k_core_t * core, uint32_t value, int regno, int delay_slot);
void check_event(c6k_core_t * core);

#define NOT_IMP_NO_NEED printf("In %s:%d, not implement at 0x%x\n", __FUNCTION__, __LINE__, core->pc);skyeye_exit(-1)
#define NOT_IMP printf("In %s:%d, not implement at 0x%x\n", __FUNCTION__, __LINE__, core->pc);skyeye_exit(-1)
#define NEW_IMP skyeye_log(Warning_log, __FUNCTION__, "In line %d, This instruction is new implemented at 0x%x\n",  __LINE__, core->pc)
//#define NEW_IMP printf( "In line %d, This instruction is new implemented at 0x%x\n",  __LINE__, core->pc)

void bb_add_pending_nops(c6k_core_t * core, int cycles)
{
    if (cycles > core->pending_nops)
        core->pending_nops = cycles;
}
void legacy_add_pending_nops(c6k_core_t * core, int cycles)
{
    if (cycles > core->pending_nops)
        core->pending_nops = cycles;
}

static uint32_t last_pc = 0;
static void print_all_gpr_to_log(c6k_core_t * core)
{
    int i;
    uint32_t result, pc;

    pc = core->pc;
    if (pc == last_pc)
        return;
    if ((pc & 0x1c) == 0x1c)
    {
        core_read(core, 32, pc, &result);
        if ((result & 0xe0000000) == 0xe0000000)
            pc += 4;
    }
    last_pc = pc;

    FILE *fp = fopen("log.txt", "a+");

    fprintf(fp, "######Current PC:%x\n", pc);
    fprintf(fp, "    ILC:%x\n", core->ilc);
    for (i = 0; i < GPR_NUM_IN_GROUP; i++)
        fprintf(fp, "    A%d:%x\n", i, core->gpr[GPR_A][i]);
    for (i = 0; i < GPR_NUM_IN_GROUP; i++)
        fprintf(fp, "    B%d:%x\n", i, core->gpr[GPR_B][i]);

    fclose(fp);
}

int core_read(c6k_core_t * core, int width, generic_address_t addr, uint32_t * result)
{
    if (core->c6k_memory_intf == NULL)
    {
        bus_read(width, addr, result);
    } else
    {
        memory_space_intf *io_memory = core->c6k_memory_intf;

        io_memory->read(core->obj, addr, result, width / 8);

        /* compatiable with bus read */
        if (width == 8)
            *result = (*result) & 0xFF;
        else if (width == 16)
            *result = (*result) & 0xFFFF;

    }

    DBG("in %s, result=0x%08x, width=0x%x, addr = 0x%08x\n", __FUNCTION__, *result, width, addr);
    return 0;
}

int core_write(c6k_core_t * core, int width, generic_address_t addr, uint32_t value)
{
    if (core->c6k_memory_intf == NULL)
    {
        bus_write(width, addr, value);
        DBG("In %s, addr=0x%x, value=0x%x, width=%d\n", __FUNCTION__, addr, value, width);
    } else
    {
        memory_space_intf *io_memory = core->c6k_memory_intf;

        io_memory->write(core->obj, addr, &value, width / 8);
    }
    return 0;
}

static inline int calc_a_index(int creg)
{
    int a = -1;

    if ((creg & 0x3) == 0)
        a = 1;
    else if ((creg & 0x3) == 1)
        a = 2;
    else if ((creg & 0x3) == 2)
        a = 0;
    else                                /* something wrong */
        ;
    return a;
}

static inline int calc_cond(c6k_core_t * core, uint32_t insn)
{
    int cond = insn >> 28;
    int ret;
    int creg = cond >> 1;
    int z = cond & 0x1;

    if (cond == 0 || cond == 0x1 /* c66x use this to extend operators to dw */ )
        return 1;
    else
    {
        if (z)
        {
            if (creg & 0x4)
                ret = core->gpr[GPR_A][calc_a_index(creg)] ? 0 : 1;
            else
            {
                ret = core->gpr[GPR_B][creg - 1] ? 0 : 1;
            }
        } else
        {
            if (creg & 0x4)
                ret = core->gpr[GPR_A][calc_a_index(creg)] ? 1 : 0;
            else
            {
                ret = core->gpr[GPR_B][creg - 1] ? 1 : 0;
            }

        }
    }
    return ret;
}

void write_buffer(c6k_core_t * core, int regno, uint32_t result)
{
    int pos = core->wb_result_pos;

    core->wb_index[pos] = regno;
    core->wb_result[pos] = result;
    core->wb_result_pos++;
    core->wb_flag = 1;
    return;
}

static void write_back(c6k_core_t * core)
{
    uint32_t i = 0;

    if (core->wb_flag)
    {
        for (; i < core->wb_result_pos; i++)
        {
            int pos = core->wb_index[i];

            if (pos > (GPR_NUM_IN_GROUP - 1))
            {
                core->gpr[GPR_B][pos - GPR_NUM_IN_GROUP] = core->wb_result[i];
            } else
            {
                core->gpr[GPR_A][pos] = core->wb_result[i];
            }
            DBG("WriteBack i=%d, gpr=%d, result=0x%x\n", i, pos, core->wb_result[i]);
        }
        /* end of parallel, we should write back result */
        core->wb_result_pos = 0;
        core->wb_flag = 0;
    }
    return;
}

int exec_write_back(c6k_core_t * core, uint32_t unused, opcode_fields_t * unused2)
{
    uint32_t i;

    for (i = 0; i < core->wb_result_pos; i++)
    {
        int pos = core->wb_index[i];

        if (pos > (GPR_NUM_IN_GROUP - 1))
            core->gpr[GPR_B][pos - GPR_NUM_IN_GROUP] = core->wb_result[i];
        else
            core->gpr[GPR_A][pos] = core->wb_result[i];
        DBG("exec_write_back: write %08x back to reg[%d][%d] = %08x ...\n",
            core->wb_result[i], pos / GPR_NUM_IN_GROUP,
            pos % GPR_NUM_IN_GROUP, core->gpr[pos / GPR_NUM_IN_GROUP][pos % GPR_NUM_IN_GROUP]);
    }
    core->wb_result_pos = 0;

    return 0;
}

void legacy_register_branch_event(c6k_core_t * core, generic_address_t target_addr)
{
    int fill_slot_id = (core->curr_slot_id + BRANCH_DELAY_SLOT) % DELAY_SLOT_NUM;

    if ((core->delay_slot_cycle[fill_slot_id] & BRANCH_EVENT_ID) == 0)
    {
        core->delay_slot_cycle[fill_slot_id] |= BRANCH_EVENT_ID;
        DBG("curr_slot_id = 0x%x, legacy_branch , pfc = 0x%x, pc=0x%x, cycle_num=0x%x, fill_slot_id=%d\n",
            core->curr_slot_id, target_addr, core->pc, core->cycle_num + BRANCH_DELAY_SLOT, fill_slot_id);
        core->pfc_branch[fill_slot_id] = target_addr;
    } else
    {
        /* something wrong, two branch triggered at the same cycle */
        SKYEYE_ERR("The delay slot is occupied.\n");
        DBG("In %s, pfc=0x%x\n", __FUNCTION__, core->pfc);
    }
    return;
}

void bb_register_branch_event(c6k_core_t * core, generic_address_t target_addr)
{

    core->br_target = target_addr;

    DBG("core->pc: %08x bb_register_branch_event with target_addr = %08x ..\n", core->pc, target_addr);
    return;
}

void legacy_register_regw_event(c6k_core_t * core, uint32_t value, int regno, int delay_slot)
{
    int fill_slot_id = (core->curr_slot_id + delay_slot) % DELAY_SLOT_NUM;

    core->delay_slot_cycle[fill_slot_id] |= REGW_EVENT_ID;
    core->regw_regno_buf[fill_slot_id][core->regw_buf_pos[fill_slot_id]] = regno;
    core->regw_regval_buf[fill_slot_id][core->regw_buf_pos[fill_slot_id]] = value;

    DBG("In %s, regw event registered, value = 0x%x, pc=0x%x, cycle_num=0x%x, fill_slot_id=%d, curr_slot_id=0x%x\n",
        __FUNCTION__, value, core->pc, core->cycle_num + BRANCH_DELAY_SLOT, fill_slot_id, core->curr_slot_id);

    core->regw_buf_pos[fill_slot_id]++;
    if (core->regw_buf_pos[fill_slot_id] == REGW_BUF_SIZE)
    {                                   /* buf overflow */
        skyeye_exit(-1);
    }

    return;
}

void bb_register_regw_event_l(c6k_core_t * core, uint32_t value, int regno, int unused)
{
    REGW_PUSH_L(core, value, regno);
}

void bb_register_regw_event_h(c6k_core_t * core, uint32_t value, int regno, int unused)
{
    REGW_PUSH_H(core, value, regno);
}

int exec_regw_l(c6k_core_t * core, uint32_t reg_addr, opcode_fields_t * unused)
{
    REGW_POP_L(core, reg_addr);

    return 0;
}

int exec_regw_h(c6k_core_t * core, uint32_t reg_addr, opcode_fields_t * unused)
{
    REGW_POP_H(core, reg_addr);

    return 0;
}

static int check_imm_write(c6k_core_t * core, generic_address_t addr)
{
    //in debug mode, if the delay slot do not access same address,then we can store the value imm
    if (core->debug_mode)
    {
        int i = 0, j = 0;

        for (i = 0; i < MEM_WRITE_DELAY_SLOT; i++)
        {
            int fill_slot_id = (core->curr_slot_id + i) % DELAY_SLOT_NUM;

            //check same addr in memory-read event
            if (core->delay_slot_cycle[fill_slot_id] & MEM_RD_EVENT_ID)
            {
                return 0;
            }
            //check same addr in memory-write event
            if (core->delay_slot_cycle[fill_slot_id] & MEM_WR_EVENT_ID)
            {
                for (j = 0; j < core->mem_wr_buf_pos[fill_slot_id]; j++)
                {
                    if (core->mem_wr_addr[i][j] == addr)
                    {
                        return 0;
                    }
                }
            }
        }
        return 1;
    }

    return 0;
}

void legacy_register_memory_event(c6k_core_t * core, uint64_t result, uint32_t reg_no, generic_address_t addr,
                                  rw_flag_t rw_flag, bool_t sign_ext, uint32_t width)
{
    if (rw_flag == RD_FLAG)
    {
        int fill_slot_id = (core->curr_slot_id + MEM_READ_DELAY_SLOT) % DELAY_SLOT_NUM;
        int buf_pos = core->mem_access_buf_pos[fill_slot_id];

        if (buf_pos < MEM_ACCESS_BUF_SIZE)
        {
            core->delay_slot_cycle[fill_slot_id] |= MEM_RD_EVENT_ID;
            /* memory access insn will be triggered after the specific cycle */
            core->mem_access_result[fill_slot_id][buf_pos] = result;
            uint32_t mem_access_reg = (reg_no & 0xFF) | (rw_flag << 8) | (width << 16) | (sign_ext << 24);

            core->mem_access_reg[fill_slot_id][buf_pos] = mem_access_reg;
            core->mem_access_addr[fill_slot_id][buf_pos] = addr;
            DBG("In %s, register RD, fill_slot_id=%d, core->mem_access_buf_pos=%d, pc=0x%x\n",
                __FUNCTION__, fill_slot_id, buf_pos, core->pc);
            core->mem_access_buf_pos[fill_slot_id]++;
        } else
        {
            SKYEYE_ERR(" RD buffer is full in the slot, pc=0x%x\n", core->pc);
        }
    } else if (rw_flag == WR_FLAG)
    {
        //check_imm_write to write to current delay_slot
        int imm = check_imm_write(core, addr);
        int fill_slot_id = 0;

        if (imm)
            fill_slot_id = (core->curr_slot_id) % DELAY_SLOT_NUM;
        else
            fill_slot_id = (core->curr_slot_id + MEM_WRITE_DELAY_SLOT) % DELAY_SLOT_NUM;

        int buf_pos = core->mem_wr_buf_pos[fill_slot_id];

        if (buf_pos < MEM_ACCESS_BUF_SIZE)
        {
            core->delay_slot_cycle[fill_slot_id] |= MEM_WR_EVENT_ID;
            core->mem_wr_addr[fill_slot_id][buf_pos] = addr;
            core->mem_wr_result[fill_slot_id][buf_pos] = result;
            uint32_t mem_wr_reg = (reg_no & 0xFF) | (rw_flag << 8) | (width << 16) | (sign_ext << 24);

            core->mem_wr_reg[fill_slot_id][buf_pos] = mem_wr_reg;
            core->mem_wr_buf_pos[fill_slot_id]++;

            DBG("In %s, delay_slot_cycle=%d, pfc_branch[%d]=0x%x\n", __FUNCTION__,
                core->delay_slot_cycle[fill_slot_id], fill_slot_id, core->pfc_branch[core->mem_access_slot_id]);
        } else
        {
            SKYEYE_ERR(" WR buffer is full in the slot, pc=0x%x\n", core->pc);
        }
    } else
    {
        DBG("In %s, error when memory event registered, pc=0x%x, cycle_num=0x%x, fill_slot_id=%d\n",
            __FUNCTION__, core->pc, core->cycle_num, fill_slot_id);
        /* something wrong */
        SKYEYE_ERR("The delay slot is occupied.\n");
        DBG("In %s, pfc=0x%x\n", __FUNCTION__, core->pfc);
    }
    return;
}

void bb_register_memory_event(c6k_core_t * core, uint64_t result, uint32_t reg_no, generic_address_t addr,
                              rw_flag_t rw_flag, bool_t sign_ext, uint32_t width)
{
    if (rw_flag == RD_FLAG)
    {
        uint32_t mem_access_reg = (reg_no & 0xFF) | (rw_flag << 8) | (width << 16) | (sign_ext << 24);

        DBG("register_memory_read: pc = 0x%08x, reg_no = %d, width = %d, addr = %08x, memr_buf_pos = %d\n",
            core->pc, reg_no, width, addr, core->memr_buf_pos);
        MEMR_PUSH(core, mem_access_reg, addr);
    } else if (rw_flag == WR_FLAG)
    {
        uint32_t mem_wr_reg = (reg_no & 0xFF) | (rw_flag << 8) | (width << 16) | (sign_ext << 24);

        DBG("register_memory_write: pc = 0x%08x, reg_no = %d, width = %d, "
            "addr = %08x, memw_buf_pos = %d, result = %016lx\n",
            core->pc, reg_no, width, addr, core->memw_buf_pos, result);
        MEMW_PUSH(core, mem_wr_reg, addr, result);
    } else
    {
        /* something wrong */
        SKYEYE_ERR("The delay slot is occupied.\n");
        DBG("In %s, pfc=0x%x\n", __FUNCTION__, core->pfc);
    }

    return;
}

int exec_read_memory(c6k_core_t * core, uint32_t reg_addr, opcode_fields_t * unused)
{
    uint32_t mem_access_reg = 0;
    generic_address_t addr;
    uint32_t reg_no, width, sign_ext, result = 0;

    DBG("exec_read_memory: memr_buf_pos = %d, memr_reg_addr = %08x, memr_addr = %08x ...\n",
        core->memr_buf_pos, core->memr_reg_addr[0], core->memr_addr[0]);

    if (SHOULD_MEMR_POP(core, reg_addr))
    {
        MEMR_POP(core, mem_access_reg, addr);

        reg_no = mem_access_reg & 0xFF;
        width = (mem_access_reg >> 16) & 0xFF;
        sign_ext = (mem_access_reg >> 24) & 0xFF;

        if (width == 64)
        {
            core_read(core, 32, addr, &result);
            core->gpr[reg_no / GPR_NUM_IN_GROUP][reg_no % GPR_NUM_IN_GROUP] = result;
            DBG("exec_read_memoryD: reg_addr = %08x, addr = %08x, core->pc = %08x, "
                "load %08x to reg[%d][%d]\n",
                reg_addr, addr, core->pc, result, reg_no / GPR_NUM_IN_GROUP, reg_no % GPR_NUM_IN_GROUP);
            core_read(core, 32, addr + 4, &result);
            core->gpr[reg_no / GPR_NUM_IN_GROUP][(reg_no % GPR_NUM_IN_GROUP) + 1] = result;
            DBG("exec_read_memoryD: reg_addr = %08x, addr = %08x, core->pc = %08x, "
                "load %08x to reg[%d][%d]\n",
                reg_addr, addr + 4, core->pc, result, reg_no / GPR_NUM_IN_GROUP, (reg_no % GPR_NUM_IN_GROUP) + 1);
        } else
        {
            core_read(core, width, addr, &result);
            if (sign_ext && (width == 8) && (result & 0x80))
                result |= 0xFFFFFF00;
            if (sign_ext && (width == 16) && (result & 0x8000))
                result |= 0xFFFF0000;
            core->gpr[reg_no / GPR_NUM_IN_GROUP][reg_no % GPR_NUM_IN_GROUP] = result;
            DBG("exec_read_memory: core->pc = %08x, load %08x to reg[%d][%d]\n",
                core->pc, result, reg_no / GPR_NUM_IN_GROUP, reg_no % GPR_NUM_IN_GROUP);
        }
    }

    return 0;
}

int exec_write_memory(c6k_core_t * core, uint32_t reg_addr, opcode_fields_t * unused)
{
    generic_address_t addr;
    uint32_t mem_wr_reg, width, reg_no;
    uint64_t result = 0;

    if (SHOULD_MEMW_POP(core, reg_addr))
    {
        MEMW_POP(core, mem_wr_reg, addr, result);
        width = (mem_wr_reg >> 16) & 0xFF;
        reg_no = mem_wr_reg & 0xFF;

        DBG("exec_write_memory: core->pc = %08x, write %016lx to addr@%08x\n", core->pc, result, addr);
        if (width == 64)
        {
            core_write(core, 32, addr, result & 0xFFFFFFFF);
            core_write(core, 32, addr + 4, result >> 32);
        } else
            core_write(core, width, addr, result);
    }

    return 0;
}

void check_event(c6k_core_t * core)
{
    DBG("check_event: pc = %08x, delay_slot_cycle[curr_slot_id] = %08x, curr_slot_id = %d ...\n",
        core->pc, core->delay_slot_cycle[core->curr_slot_id], core->curr_slot_id);
    if (core->delay_slot_cycle[core->curr_slot_id] == 0)
        return;

    if (core->delay_slot_cycle[core->curr_slot_id] & BRANCH_EVENT_ID)
    {
        fetch_context_t *fc = (fetch_context_t *) core->fetch_context;

        /* check branch event */
        if (core->pfc_branch[core->curr_slot_id] != BAD_ADDR)
        {

            DBG("check_event: BRANCH_EVENT_ID triggered: target = %08x, pc = %08x, ENTRY = %08x\n",
                core->pfc_branch[core->curr_slot_id], core->pc, HASH(core->pfc_branch[core->curr_slot_id]));

            core->pc = core->pfc_branch[core->curr_slot_id];
            core->pfc_branch[core->curr_slot_id] = BAD_ADDR;
            core->delay_slot_cycle[core->curr_slot_id] &= ~BRANCH_EVENT_ID;

            /* The branch sometimes will triggered in sploop, so we have to reset the sploop buffer */
            core->sploop_begin = core->sploop_end = core->sploop_outside = BAD_ADDR;
            core->buffer_pos = 0;

            invalidate_bb_cache(core);
        }
    }

    if (core->delay_slot_cycle[core->curr_slot_id] & REGW_EVENT_ID)
    {
        int curr_slot_id = core->curr_slot_id;

        assert(core->regw_buf_pos[curr_slot_id] > 0);
        DBG("In %s, REGW triggered, curr_slot_id=%d, pc=0x%x\n", __FUNCTION__, core->curr_slot_id, core->pc);
        uint32_t i = 0;

        for (; i < core->regw_buf_pos[curr_slot_id]; i++)
        {
            int reg_no = core->regw_regno_buf[curr_slot_id][i];
            int result = core->regw_regval_buf[curr_slot_id][i];

            core->gpr[reg_no / GPR_NUM_IN_GROUP][reg_no % GPR_NUM_IN_GROUP] = result;
        }
        core->delay_slot_cycle[curr_slot_id] &= ~REGW_EVENT_ID;
        core->regw_buf_pos[curr_slot_id] = 0;
    }
    /* check memory access event */

    if (core->delay_slot_cycle[core->curr_slot_id] & MEM_RD_EVENT_ID)
    {
        uint32_t i = 0;

        for (; i < core->mem_access_buf_pos[core->curr_slot_id]; i++)
        {
            uint32_t mem_access_reg = core->mem_access_reg[core->curr_slot_id][i];
            generic_address_t addr = core->mem_access_addr[core->curr_slot_id][i];
            int reg_no = mem_access_reg & 0xFF;
            int width = (mem_access_reg >> 16) & 0xFF;
            int sign_ext = (mem_access_reg >> 24) & 0xFF;
            uint32_t result = 0;

            if (mem_access_reg != 0)
            {
                if (width == 64)
                {
                    core_read(core, 32, addr, &result);
                    core->gpr[reg_no / GPR_NUM_IN_GROUP][reg_no % GPR_NUM_IN_GROUP] = result;
                    core_read(core, 32, addr + 4, &result);
                    core->gpr[reg_no / GPR_NUM_IN_GROUP][(reg_no % GPR_NUM_IN_GROUP) + 1] = result;
                } else
                {
                    core_read(core, width, addr, &result);
                    if (sign_ext && (width == 8) && (result & 0x80))
                        result |= 0xFFFFFF00;
                    if (sign_ext && (width == 16) && (result & 0x8000))
                        result |= 0xFFFF0000;
                    core->gpr[reg_no / GPR_NUM_IN_GROUP][reg_no % GPR_NUM_IN_GROUP] = result;
                }
                core->mem_access_reg[core->curr_slot_id][i] = 0x0;
            }
            DBG("In %s, RD triggered, curr_slot_id=%d, pc=0x%x, reg_no = %d\n",
                __FUNCTION__, core->curr_slot_id, core->pc, reg_no);
        }
        core->mem_access_buf_pos[core->curr_slot_id] = 0;
        core->delay_slot_cycle[core->curr_slot_id] &= ~MEM_RD_EVENT_ID;
    }
    if (core->delay_slot_cycle[core->curr_slot_id] & MEM_WR_EVENT_ID)
    {
        uint32_t i = 0;

        for (; i < core->mem_wr_buf_pos[core->curr_slot_id]; i++)
        {
            generic_address_t addr = core->mem_wr_addr[core->curr_slot_id][i];
            int width = (core->mem_wr_reg[core->curr_slot_id][i] >> 16) & 0xFF;
            int reg_no = core->mem_wr_reg[core->curr_slot_id][i] & 0xFF;

            uint64_t result = core->mem_wr_result[core->curr_slot_id][i];

            if (width == 64)
            {
                core_write(core, 32, addr, result & 0xFFFFFFFF);
                core_write(core, 32, addr + 4, result >> 32);
            } else
                core_write(core, width, addr, result);

            core->mem_wr_reg[core->curr_slot_id][i] = 0x0;
            DBG("In %s, WR triggered, curr_slot_id=%d, pc=0x%x, reg_no = %d\n",
                __FUNCTION__, core->curr_slot_id, core->pc, reg_no);
        }
        core->mem_wr_buf_pos[core->curr_slot_id] = 0;
        core->delay_slot_cycle[core->curr_slot_id] &= ~MEM_WR_EVENT_ID;
        DBG("In %s, WR triggered, curr_slot_id=%d, pc=0x%x\n", __FUNCTION__, core->curr_slot_id, core->pc);
    }

    return;
}

void bb_cycle_inc(c6k_core_t * core, int n)
{
    int i = 0;

    if (n > 0 && core->pending_nops >= n)
    {
        n = core->pending_nops + 1;
        core->pending_nops = 0;
    }
    for (; i < n; i++)
    {
        core->cycle_num++;
    }
}
void legacy_cycle_inc(c6k_core_t * core, int n)
{
    int i = 0;

    if (n > 0 && core->pending_nops >= n)
    {
        n = core->pending_nops + 1;
        core->pending_nops = 0;
    }
    for (; i < n; i++)
    {
        check_event(core);
        core->cycle_num++;
        core->curr_slot_id++;
        core->curr_slot_id %= DELAY_SLOT_NUM;
    }
}

//we check linear mode or amr mode for offset
static inline int calc_offset(c6k_core_t * core, int base, int offset, int y, int align)
{

    int cal_src1;
    int block_size;
    int cal_num;

    if (base >= 4 && base <= 7)
    {
        int amr_mode = check_amr_mode(core, y, base);

        if (amr_mode == BK0)
        {
            cal_num = (core->amr >> 16) & 0x1f;
            block_size = 0x1 << (cal_num + 1);
            cal_src1 = (offset << align) % block_size;
        } else if (amr_mode == BK1)
        {
            cal_num = (core->amr >> 21) & 0x1f;
            block_size = 0x1 << (cal_num + 1);
            cal_src1 = (offset << align) % block_size;
        } else
        {
            cal_src1 = offset << align;
        }
    } else
    {
        cal_src1 = offset << align;
    }

    return cal_src1;
}

#define WORD_ALIGN(a, b) (a << b)
static inline generic_address_t calc_addr(c6k_core_t * core, int base, int offset, int mode, int y, int align)
{
    generic_address_t addr;
    uint32_t result;

    switch (mode)
    {
        case 0:
            //addr = core->gpr[y][base] - WORD_ALIGN(offset, align);
            addr = core->gpr[y][base] - calc_offset(core, base, offset, y, align);
            break;
        case 1:
            addr = core->gpr[y][base] + calc_offset(core, base, offset, y, align);
            break;
        case 4:
            addr = core->gpr[y][base] - calc_offset(core, base, core->gpr[y][offset], y, align);
            break;
        case 5:
            addr = core->gpr[y][base] + calc_offset(core, base, core->gpr[y][offset], y, align);
            break;
        case 8:
            result = core->gpr[y][base] - calc_offset(core, base, offset, y, align);
            addr = result;
            WRITE_BUFFER(core, base + y * GPR_NUM_IN_GROUP, result);
            break;
        case 9:
            result = core->gpr[y][base] + calc_offset(core, base, offset, y, align);
            addr = result;
            WRITE_BUFFER(core, base + y * GPR_NUM_IN_GROUP, result);
            break;
        case 0xa:
            addr = core->gpr[y][base];
            result = core->gpr[y][base] - calc_offset(core, base, offset, y, align);
            WRITE_BUFFER(core, base + y * GPR_NUM_IN_GROUP, result);
            break;
        case 0xb:
            addr = core->gpr[y][base];
            result = core->gpr[y][base] + calc_offset(core, base, offset, y, align);
            WRITE_BUFFER(core, base + y * GPR_NUM_IN_GROUP, result);
            break;
        case 0xc:
            result = core->gpr[y][base] - calc_offset(core, base, core->gpr[y][offset], y, align);
            addr = result;
            WRITE_BUFFER(core, base + y * GPR_NUM_IN_GROUP, result);
            break;
        case 0xd:
            result = core->gpr[y][base] + calc_offset(core, base, core->gpr[y][offset], y, align);
            addr = result;
            WRITE_BUFFER(core, base + y * GPR_NUM_IN_GROUP, result);
            break;
        case 0xe:
            addr = core->gpr[y][base];
            result = core->gpr[y][base] - calc_offset(core, base, core->gpr[y][offset], y, align);
            WRITE_BUFFER(core, base + y * GPR_NUM_IN_GROUP, result);
            break;
        case 0xf:
            addr = core->gpr[y][base];
            result = core->gpr[y][base] + calc_offset(core, base, core->gpr[y][offset], y, align);
            WRITE_BUFFER(core, base + y * GPR_NUM_IN_GROUP, result);
            break;
        default:
            NOT_IMP;
            addr = 0xFFFFFFFF;
            break;
    }

    return addr;
}

int64_t slow32(int64_t src)
{
    int64_t op;

    op = src & 0xFFFFFFFF;
    if (op & 0x80000000)
    {                                   /* Number is negative */
        return 0xffffffff00000000 | op;
    } else
    {
        return op;
    }
}

int32_t shigh16(int32_t src)
{
    int32_t op;

    op = (src >> 16) & 0x0000FFFF;
    if (op & 0x00008000)
    {                                   /* Number is negative */
        return 0xffff0000 | op;
    } else
    {
        return op;
    }
}

int32_t slow16(int32_t src)
{
    int32_t op;

    op = src & 0x0000FFFF;
    if (op & 0x00008000)
    {                                   /* Number is negative */
        return 0xffff0000 | op;
    } else
    {
        return op;
    }
}

static int32_t uhigh16(int32_t src)
{
    int32_t op;

    op = (src >> 16) & 0x0000FFFF;
    return op;
}

static int32_t ulow16(int32_t src)
{
    int32_t op;

    op = src & 0x0000FFFF;
    return op;
}

int32_t shigh8(int32_t src)
{
    int32_t op;

    op = (src >> 8) & 0x000000FF;
    if (op & 0x00000080)
    {                                   /* Number is negative */
        return 0xffffff00 | op;
    } else
    {
        return op;
    }
}

int32_t slow8(int32_t src)
{
    int32_t op;

    op = src & 0x000000FF;
    if (op & 0x00000080)
    {                                   /* Number is negative */
        return 0xffffff00 | op;
    } else
    {
        return op;
    }
}

int32_t sat_jz(int64_t a)
{
    int i;

    // Joe Zbiciak test for saturation 
    i = a;
    if (a >= ((int64_t) 0x80000000))
        i = 0x7FFFFFFF;
    if (a < -((int64_t) 0x80000000))
        i = 0x80000000;
    return i;
}

int64_t sat_64(int64_t a)
{
    int64_t i;

    // Joe Zbiciak test for saturation 
    i = a;
    if (a >= ((int64_t) 0x80000000))
        i = 0x7FFFFFFF;
    if (a < -((int64_t) 0x80000000))
        i = 0x80000000;
    return i;
}

int32_t sat_check32(int64_t a)
{
    int i;

    // Joe Zbiciak test for saturation - just return sat or not 
    i = 0x0;
    if (a >= ((int64_t) 0x80000000))
        i = 0x1;
    if (a < -((int64_t) 0x80000000))
        i = 0x1;
    return i;
}

static int32_t sbyte0(int32_t src)
{
    int32_t op;

    op = src & 0x000000ff;
    if (op & 0x00000080)
    {                                   /* Number is negative */
        return 0xffffff00 | op;
    } else
    {
        return op;
    }
}

static int32_t sbyte1(int32_t src)
{
    int32_t op;

    op = (src & 0x0000ff00) >> 8;
    if (op & 0x00000080)
    {                                   /* Number is negative */
        return 0xffffff00 | op;
    } else
    {
        return op;
    }
}

static int32_t sbyte2(int32_t src)
{
    int32_t op;

    op = (src & 0x00ff0000) >> 16;
    if (op & 0x00000080)
    {                                   /* Number is negative */
        return 0xffffff00 | op;
    } else
    {
        return op;
    }
}

static int32_t sbyte3(int32_t src)
{
    int32_t op;

    op = (src & 0xff000000) >> 24;
    if (op & 0x00000080)
    {                                   /* Number is negative */
        return 0xffffff00 | op;
    } else
    {
        return op;
    }
}

static int32_t ubyte0(int32_t src)
{
    int32_t op;

    op = src & 0x000000ff;
    return op;
}

static int32_t ubyte1(int32_t src)
{
    int32_t op;

    op = (src & 0x0000ff00) >> 8;
    return op;
}

static int32_t ubyte2(int32_t src)
{
    int32_t op;

    op = (src & 0x00ff0000) >> 16;
    return op;
}

static int32_t ubyte3(int32_t src)
{
    int32_t op;

    op = (src >> 24) & 0x000000ff;
    return op;
}

static int32_t snan_chk_sp(int32_t op)
{
    if (((op & 0x7fc00000) == 0x7f800000) && (op & 0x003fffff))
    {
        return 1;
    } else
    {
        return 0;
    }
}

static int32_t qnan_chk_sp(int32_t op)
{
    if ((op & 0x7fc00000) == 0x7fc00000)
    {
        return 1;
    } else
    {
        return 0;
    }
}

static int32_t denorm_chk_sp(int32_t op)
{
    if (!(op & 0x7f800000) && (op & 0x007fffff))
    {
        return 1;
    } else
    {
        return 0;
    }
}

static int32_t extract_exp_sp(int32_t op)
{
    return ((op & 0x7f800000) >> 23);
}

static int32_t extract_man_sp(int32_t op)
{

    return ((op & 0x007fffff) | 0x00800000);
}

static int32_t inf_chk_sp(int32_t op)
{
    if (((op & 0x7f800000) == 0x7f800000) && !(op & 0x007fffff))
    {
        return 1;
    } else
    {
        return 0;
    }
}

static int32_t excp_chk_mpy_sp(int32_t op1, int32_t op2)
{
    int32_t exp1, exp2;

    if (snan_chk_sp(op1))
    {
        return (1);
    }
    if (snan_chk_sp(op2))
    {
        return (1);
    }
    if (qnan_chk_sp(op1))
    {
        return (1);
    }
    if (qnan_chk_sp(op2))
    {
        return (1);
    }

    exp1 = extract_exp_sp(op1);
    exp2 = extract_exp_sp(op2);

    if (inf_chk_sp(op1))
    {
        if (!exp2)
        {
            return (1);
        } else
        {
            return (2);
        }
    }
    if (inf_chk_sp(op2))
    {
        if (!exp1)
        {
            return (1);
        } else
        {
            return (2);
        }
    }
    return (0);
}

static int32_t extract_sign(int32_t op)
{
    if (op & 0x80000000)
        return (1);
    else
        return (0);
}

static int32_t round_ofuf_chk_sp(int32_t sign, int32_t exp, int32_t man, int32_t round, int32_t sticky, int32_t adder,
                                 int rmode1, int rmode2)
{
    int32_t round_mode;
    int32_t overflow;
    int32_t underflow;
    int32_t infinity;
    int32_t inexact;

    if (adder)
        round_mode = rmode1;
    else
        round_mode = rmode2;

    inexact = round || sticky;
    overflow = 0;
    underflow = 0;
    infinity = 0;

    switch (round_mode)
    {
        case 0:                        /* round to nearest */
            if (round && (sticky || (man & 1)))
            {
                man++;
                if (man & 0x01000000)
                {
                    man >>= 1;
                    exp++;
                }
            }
            if (exp > 0x000000fe)
            {                           /* overflow */
                man = 0;
                exp = 0x000000ff;
                overflow = 1;
                infinity = 1;
                inexact = 1;
            }
            if (exp < 1)
            {                           /* underflow */
                man = 0;
                exp = 0;
                underflow = 1;
                inexact = 1;
            }
            break;

        case 1:                        /* round to zero */
            if (exp > 0x000000fe)
            {                           /* overflow */
                man = 0x00ffffff;
                exp = 0x000000fe;
                overflow = 1;
                inexact = 1;
            }
            if (exp < 1)
            {                           /* underflow */
                man = 0;
                exp = 0;
                underflow = 1;
                inexact = 1;
            }
            break;

        case 2:                        /* round to +infinity */
            if (!sign)
            {
                if (round || sticky)
                {
                    man++;
                    if (man & 0x01000000)
                    {
                        man >>= 1;
                        exp++;
                    }
                }
            }
            if (exp > 0x000000fe)
            {                           /* overflow */
                if (sign)
                {
                    man = 0x00ffffff;
                    exp = 0x000000fe;
                } else
                {
                    man = 0;
                    exp = 0x000000ff;
                    infinity = 1;
                }
                overflow = 1;
                inexact = 1;
            }
            if (exp < 1)
            {                           /* underflow */
                if (sign)
                {
                    man = 0;
                    exp = 0;
                } else
                {
                    man = 0;
                    exp = 1;
                }
                underflow = 1;
                inexact = 1;
            }
            break;

        case 3:                        /* round to -infinity */
            if (sign)
            {
                if (round || sticky)
                {
                    man++;
                    if (man & 0x01000000)
                    {
                        man >>= 1;
                        exp++;
                    }
                }
            }
            if (exp > 0x000000fe)
            {                           /* overflow */
                if (sign)
                {
                    man = 0;
                    exp = 0x000000ff;
                    infinity = 1;
                } else
                {
                    man = 0x00ffffff;
                    exp = 0x000000fe;
                }
                overflow = 1;
                inexact = 1;
            }
            if (exp < 1)
            {                           /* underflow */
                if (sign)
                {
                    man = 0;
                    exp = 1;
                } else
                {
                    man = 0;
                    exp = 0;
                }
                underflow = 1;
                inexact = 1;
            }
            break;
    }

    return ((sign << 31) | (exp << 23) | (man & 0x007fffff));
}

static int32_t excp_chk_add_sp(int32_t op1, int32_t op2)
{
    int32_t sign1, sign2;

    /* SNaN checks */
    if (snan_chk_sp(op1))
    {
        return (1);
    }
    if (snan_chk_sp(op2))
    {
        return (1);
    }
    /* QNaN checks */
    if (qnan_chk_sp(op1))
    {
        return (1);
    }
    if (qnan_chk_sp(op2))
    {
        return (1);
    }

    sign1 = extract_sign(op1);
    sign2 = extract_sign(op2);

    /* infinity checks */
    if (inf_chk_sp(op1))
    {
        if (inf_chk_sp(op2) && (sign1 != sign2))
        {
            return (1);
        } else
        {
            return (2 + sign1);
        }
    }

    if (inf_chk_sp(op2))
    {
        return (2 + sign2);
    }

    return (0);
}

static int32_t add_sp(int32_t op1, int32_t op2, int rmode1, int rmode2)
{
    int32_t rc;
    int32_t sign1, sign2;
    int32_t exp1, exp2;
    int32_t round_mode;
    int32_t man1, man2;
    int32_t exp_diff;
    int32_t sticky, round;
    int32_t result_exp;
    int32_t result_man;
    int32_t result_sign;
    int32_t result;

    /* call exception checks */
    rc = excp_chk_add_sp(op1, op2);
    if (rc == 1)
        return (0x7fffffff);            /* QNaN */
    else if (rc == 2)
        return (0x7f800000);            /* +infinity */
    else if (rc == 3)
        return (0xff800000);            /* -infinity */

    sign1 = extract_sign(op1);
    sign2 = extract_sign(op2);

    exp1 = extract_exp_sp(op1);
    exp2 = extract_exp_sp(op2);
    round_mode = rmode1;

    /* zero operand */
    if (!exp1)
    {                                   /* test op1 == zero (or denormal)          */
        if (!exp2)
        {                               /*  both operands are zero                 */
            if (sign1 == sign2)
                return (sign1 ? 0x80000000 : 0);        /*  if same sign, return that sign of zero */
            else
            {                           /*  for different signed zeros             */
                if (round_mode == 3)
                    return (0x80000000);        /*    return -zero if round down           */
                else
                    return (0);         /*    return +zero for other roundings     */
            }
        } else
            return (op2);               /*  return op2 if op1==0 && op2!=0         */
    }
    if (!exp2)
        return (op1);                   /*  return op1 if op2==0 && op1!=0         */

    /* real additions */
    man1 = extract_man_sp(op1) << 3;    /* shift mantissa 3 bits for guard/round/sticky bits */
    man2 = extract_man_sp(op2) << 3;
    if (exp1 > exp2)
    {
        result_exp = exp1;
        result_sign = sign1;
        exp_diff = exp1 - exp2;
        if (exp_diff > 31)
            man2 = 1;                   /* just sticky bit is set */
        else
        {
            sticky = (man2 << (32 - exp_diff)) ? 1 : 0; /* or of bits shifted out */
            man2 = (man2 >> exp_diff) | sticky; /* or sticky into LSB */
        }
        result_man = (sign1 == sign2) ? man1 + man2 : man1 - man2;
    } else if (exp2 > exp1)
    {
        result_exp = exp2;
        result_sign = sign2;
        exp_diff = exp2 - exp1;
        if (exp_diff > 31)
            man1 = 1;
        else
        {
            sticky = (man1 << (32 - exp_diff)) ? 1 : 0;
            man1 = (man1 >> exp_diff) | sticky;
        }
        result_man = (sign1 == sign2) ? man2 + man1 : man2 - man1;
    } else
    {                                   /* exponents are equal */
        if ((man1 == man2) && (sign1 != sign2))
        {
            if (round_mode == 3)
                return (0x80000000);
            else
                return (0);
        } else
        {
            result_exp = exp1;
            result_sign = sign1;
            result_man = (sign1 == sign2) ? man1 + man2 : man1 - man2;
            if (result_man < 0)
            {
                result_man = -result_man;
                result_sign = !result_sign;
            }
        }
    }

    /* normalize result mantissa and extract round bit */
    if (result_man & 0x08000000)
    {                                   /* correct mantissa overflow */
        round = (result_man & 8) ? 1 : 0;
        sticky = (result_man & 7) ? 1 : 0;
        result_man >>= 4;
        result_exp++;
    } else
    {
        if (result_man & 0x07ffffff)
        {                               /* to prevent an infinite while loop */
            while (!(result_man & 0x04000000))
            {
                result_man <<= 1;
                result_exp--;
            }
        }
        round = (result_man & 4) ? 1 : 0;
        sticky = (result_man & 3) ? 1 : 0;
        result_man >>= 3;
    }

    result = round_ofuf_chk_sp(result_sign, result_exp, result_man, round, sticky, 1, round_mode, rmode2);
    return (result);
}

static int exec_abs(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op5_11;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int reg_group = fields->x ? (!s) & 0x1 : s;

        if (op == 0x1a)
        {
            int32_t tmp = core->gpr[reg_group][src2];

            if (tmp < 0)
            {
                //if src2 = -2^31, dst = 2^31 - 1
#define A_SPEC_SRC2	(0x80000000)
#define A_SPEC_DST	(0x7FFFFFFF)
                if (tmp == A_SPEC_SRC2)
                    tmp = A_SPEC_DST;
                else
                {
                    tmp = -tmp;
                }
                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, tmp);
            } else
                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, tmp);
        } else if (op == 0x38)
        {                               // NOTES, operators only slong, the cross may not be used
            int64_t tmp_upper, tmp;
            int64_t pwr = -(int64_t) 2 << 39;

            tmp_upper = ((int64_t) core->gpr[s][src2 + 1] & 0xFF) << 32;
            tmp_upper = EXTEND_SIGN(tmp_upper, 40);
            tmp = (int64_t) core->gpr[s][src2];
            tmp |= tmp_upper;

            if (tmp < 0 && tmp != pwr)
                tmp = -tmp;
            else if (tmp == pwr)
                tmp = -pwr - 1;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, tmp & 0xFFFFFFFF);
            WRITE_BUFFER(core, dst + 1 + s * GPR_NUM_IN_GROUP, tmp >> 32);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_abs2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;

        int16_t tmp_upper = core->gpr[reg_group][src2] >> 16;
        int16_t tmp_low = core->gpr[reg_group][src2] & 0xFFFF;
        int32_t tmp;
        int16_t pwr = (int16_t) 1 << 15;

        if (tmp_upper < 0 && tmp_upper != pwr)
        {
            tmp_upper = -tmp_upper;
        } else if (tmp_upper == pwr)
        {
            tmp_upper = (int16_t) (1 << 15) - 1;
        }

        if (tmp_low < 0 && tmp_low != pwr)
        {
            tmp_low = -tmp_low;
        } else if (tmp_low == pwr)
        {
            tmp_low = (int16_t) (1 << 15) - 1;
        }

        tmp = (tmp_upper << 16) | tmp_low;
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, tmp);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}

static int exec_absdp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_S1 << fields->s;
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        double result;

        GET_DW_DOUBLE(core->gpr[s][src2 - 1], core->gpr[s][src2], dp) result = fabs(dp);
        CVT_DW_TO_WORDS(result, res_l, res_h) REGISTER_REGW_EVENT_L(core, res_l, dst, 0);
        REGISTER_REGW_EVENT_H(core, res_h, dst + 1, 1);
    }

    core->pc += 4;
    return 0;
}

static int exec_abssp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_S1 << fields->s;
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;

        float *xop2 = (float *) &core->gpr[x][src2];
        float result = *xop2 < 0.0 ? -(*xop2) : *xop2;
        uint32_t *presult = (uint32_t *) & result;

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, *presult);
    }

    core->pc += 4;
    return 0;
}

static int exec_add_l(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op5_11;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;
        int result;

        if (op == 0x2)
        {
            int scst5 = SIGN_EXTEND(src1, 5);

            result = core->gpr[reg_group][src2] + scst5;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
            DBG("In %s, op=0x%x, insn=0x%x\n", __FUNCTION__, op, insn);
        } else if (op == 0x3)
        {
            result = core->gpr[reg_group][src2] + core->gpr[s][src1];
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
            DBG("In %s, op=0x%x, insn=0x%x, src1=%d, src2=%d\n", __FUNCTION__, op, insn, src1, src2);
        } else if (op == 0x20)
        {
            int scst5 = EXTEND_SIGN(src1, 5);
            int64_t result = ((int64_t) (core->gpr[s][src2 + 1] & 0xFF)) << 32;

            result |= core->gpr[s][src2];
            result += scst5;

            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result & 0xFFFFFFFF);
            WRITE_BUFFER(core, dst + 1 + s * GPR_NUM_IN_GROUP, (result >> 32) & 0xFF);
        } else if (op == 0x21)
        {
            int64_t result = ((int64_t) (core->gpr[s][src2 + 1] & 0xFF)) << 32;

            result |= core->gpr[s][src2];
            result += core->gpr[reg_group][src1];

            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result & 0xFFFFFFFF);
            WRITE_BUFFER(core, dst + 1 + s * GPR_NUM_IN_GROUP, (result >> 32) & 0xFF);
        } else if (op == 0x23)
        {
            int64_t result = (int64_t) core->gpr[reg_group][src2];

            result += core->gpr[reg_group][src1];

            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result & 0xFFFFFFFF);
            WRITE_BUFFER(core, dst + 1 + s * GPR_NUM_IN_GROUP, (result >> 32) & 0xFF);
        } else
        {
            DBG("In %s, op=0x%x, insn=0x%x\n", __FUNCTION__, op, insn);
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;

}
static int exec_add_s(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op6_11;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;
        int result;

        if (op == 0x2d)
        {
            int64_t result = ((int64_t) (core->gpr[s][src1] & 0xFF)) << 32;

            result |= core->gpr[s][src1 - 1];
            result += core->gpr[reg_group][src2];
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
            WRITE_BUFFER(core, dst + 1 + s * GPR_NUM_IN_GROUP, (result >> 32) & 0xFF);
            DBG("In %s, op=0x%x, insn=0x%x, scst5=%d\n", __FUNCTION__, op, insn, scst5);
        } else if (op == 0x6)
        {                               //C64x/C64x+
            int scst5 = SIGN_EXTEND(src1, 5);

            if (x)
                result = core->gpr[(!s) & 0x1][src2] + scst5;
            else
                result = core->gpr[s][src2] + scst5;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
            DBG("In %s, op=0x%x, insn=0x%x, scst5=%d\n", __FUNCTION__, op, insn, scst5);
        } else if (op == 0x7)
        {                               //C64x/C64x+
            if (x)
                result = core->gpr[(!s) & 0x1][src2] + core->gpr[s][src1];
            else
                result = core->gpr[s][src2] + core->gpr[s][src1];
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
            DBG("In %s, op=0x%x, insn=0x%x\n", __FUNCTION__, op, insn);
        } else
        {
            DBG("In %s, op=0x%x, insn=0x%x\n", __FUNCTION__, op, insn);
            NOT_IMP_NO_NEED;
        }

    }
    core->pc += 4;
    return 0;

}
static int exec_add_d(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op7_12;
        int op1 = fields->op2_11;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = BITS(12, 12);
        int reg_group = x ? (!s) & 0x1 : s;
        int result;

        if (op == 0x12)
        {
            result = core->gpr[s][src2] + src1;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
            DBG("In %s, op=0x%x, insn=0x%x, scst5=%d\n", __FUNCTION__, op, insn, src1);
        } else if (op == 0x10)
        {
            result = core->gpr[s][src2] + core->gpr[s][src1];
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else if (op1 == 0x2bc)
        {
            if (src1 & 0x10)
                src1 = -(src1 & 0xf);
            result = core->gpr[reg_group][src2] + src1;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
            DBG("In %s, op=0x%x, insn=0x%x, scst5=%d\n", __FUNCTION__, op, insn, src1);
        } else if (op1 == 0x2ac)
        {
            result = core->gpr[s ? 0 : 1][src2] + core->gpr[s][src1];
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else
        {
            result = core->gpr[s ? 0 : 1][src2] + core->gpr[s][src1];
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        }

    }
    core->pc += 4;
    return 0;

}
static int exec_adddp(c6k_core_t * core, uint32_t insn, opcode_fields_t * unused)
{
    if (calc_cond(core, insn))
    {
        int op = BITS(5, 11);
        int src1 = BITS(13, 17);
        int src2 = BITS(18, 22);
        int dst = BITS(23, 27);
        int s = BITS(1, 1);
        int x = BITS(12, 12);
        int gpr_group = x ? (!s) & 0x1 : s;

        uint64_t fp1, fp2;

        if (op == 0x18 || op == 0x72)
        {
            fp1 = core->gpr[s][src1 + 1];
            fp1 = (fp1 << 32);
            fp1 |= core->gpr[s][src1];
            fp2 = core->gpr[gpr_group][src2 + 1];
            fp2 = (fp2 << 32);
            fp2 |= core->gpr[gpr_group][src2];
        } else
        {
            NOT_IMP_NO_NEED;
        }

        double src_fp1, src_fp2, dst_fp;

        src_fp1 = *(double *) &fp1;
        src_fp2 = *(double *) &fp2;
        dst_fp = src_fp1 + src_fp2;
        uint64_t result = *(uint64_t *) & dst_fp;

        REGISTER_REGW_EVENT_L(core, result & 0xFFFFFFFF, s * GPR_NUM_IN_GROUP + dst, ADDDP_DELAY_SLOT - 1);
        REGISTER_REGW_EVENT_H(core, result >> 32, s * GPR_NUM_IN_GROUP + dst + 1, ADDDP_DELAY_SLOT);
    }
    core->pc += 4;
    return 0;
}

static int exec_addab(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op7_12;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        uint32_t ucst5 = (uint32_t) src1;
        int cal_num;
        int block_size;
        int cal_src1;
        int result;

        int src;

        if (op == 0x30)
            src = core->gpr[s][src1];
        else if (op == 0x32)
            src = src1;
        else
        {
            NOT_IMP_NO_NEED;
        }

        if (src2 >= 4 && src2 <= 7)
        {
            int amr_mode = check_amr_mode(core, s, src2);

            if (amr_mode == BK0)
            {
                cal_num = (core->amr >> 16) & 0x1f;
                block_size = 0x1 << (cal_num + 1);
                cal_src1 = src % block_size;
            } else if (amr_mode == BK1)
            {
                cal_num = (core->amr >> 21) & 0x1f;
                block_size = 0x1 << (cal_num + 1);
                cal_src1 = src % block_size;
            } else
            {
                cal_src1 = src;
            }
        } else
        {
            cal_src1 = src;
        }

        result = core->gpr[s][src2] + cal_src1;

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}

static int exec_addabd(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int ucst15 = fields->ucst8_22;
        int dst = fields->dst23_27;
        int y = fields->y;
        int s = fields->s;
        int baseR = y ? core->gpr[1][15] : core->gpr[1][14];    //y ? B15 : B14
        int result;

        result = baseR + (ucst15);
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    //NEW_IMP;
    core->pc += 4;

    return 0;
}

static int exec_addad(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op7_12;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int cal_num;
        int block_size;
        int cal_src1;
        int result;
        int src;

        if (op == 0x3c)
            src = core->gpr[s][src1];
        else if (op == 0x3d)
            src = src1;
        else
        {
            NOT_IMP_NO_NEED;
        }

        if (src2 >= 4 && src2 <= 7)
        {
            int amr_mode = check_amr_mode(core, s, src2);

            if (amr_mode == BK0)
            {
                cal_num = (core->amr >> 16) & 0x1f;
                block_size = 0x1 << (cal_num + 1);
                cal_src1 = (src << 3) % block_size;
            } else if (amr_mode == BK1)
            {
                cal_num = (core->amr >> 21) & 0x1f;
                block_size = 0x1 << (cal_num + 1);
                cal_src1 = (src << 3) % block_size;
            } else
            {
                cal_src1 = src << 3;
            }
        } else
        {
            cal_src1 = src << 3;
        }

        result = core->gpr[s][src2] + cal_src1;
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    core->pc += 4;
    return 0;
}
static int exec_addah(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op7_12;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int cal_num;
        int block_size;
        int cal_src1;
        int result;
        int src;

        if (op == 0x34)
            src = core->gpr[s][src1];
        else if (op == 0x36)
            src = src1;
        else
        {
            NOT_IMP_NO_NEED;
        }

        if (src2 >= 4 && src2 <= 7)
        {
            int amr_mode = check_amr_mode(core, s, src2);

            if (amr_mode == BK0)
            {
                cal_num = (core->amr >> 16) & 0x1f;
                block_size = 0x1 << (cal_num + 1);
                cal_src1 = (src << 1) % block_size;
            } else if (amr_mode == BK1)
            {
                cal_num = (core->amr >> 21) & 0x1f;
                block_size = 0x1 << (cal_num + 1);
                cal_src1 = (src << 1) % block_size;
            } else
            {
                cal_src1 = src << 1;
            }
        } else
        {
            cal_src1 = src << 1;
        }

        result = core->gpr[s][src2] + cal_src1;
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    core->pc += 4;
    return 0;
}
static int exec_addah_d(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int ucst15 = fields->ucst8_22;
        int dst = fields->dst23_27;
        int y = fields->y;
        int s = fields->s;
        int baseR = y ? core->gpr[1][15] : core->gpr[1][14];    //y ? B15 : B14
        int result;

        result = baseR + (ucst15 << 1);
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_addaw(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op7_12;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int cal_num;
        int block_size;
        int cal_src1;
        int result;
        int src;

        if (op == 0x38)
            src = core->gpr[s][src1];
        else if (op == 0x3a)
            src = src1;
        else
        {
            NOT_IMP_NO_NEED;
        }

        if (src2 >= 4 && src2 <= 7)
        {
            int amr_mode = check_amr_mode(core, s, src2);

            if (amr_mode == BK0)
            {
                cal_num = (core->amr >> 16) & 0x1f;
                block_size = 0x1 << (cal_num + 1);
                cal_src1 = (src << 2) % block_size;
            } else if (amr_mode == BK1)
            {
                cal_num = (core->amr >> 21) & 0x1f;
                block_size = 0x1 << (cal_num + 1);
                cal_src1 = (src << 2) % block_size;
            } else
            {
                cal_src1 = src << 2;
            }
        } else
        {
            cal_src1 = src << 2;
        }

        result = core->gpr[s][src2] + cal_src1;
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    core->pc += 4;
    return 0;
}

static int exec_addaw_c64xplus_only(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    uint32_t ucst15 = fields->ucst8_22;
    int dst = fields->dst23_27;
    int s = fields->s;
    int y = fields->y;
    int result;

    result = core->gpr[1][y ? 15 : 14] + (ucst15 << 2);
    WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);

    core->pc += 4;
    return 0;
}

static int exec_addk(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int scst = fields->scst7_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int result;

        scst = SIGN_EXTEND(scst, 16);

        result = core->gpr[s][dst] + scst;
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        DBG("In %s, dst=%d, scst=%d\n", __FUNCTION__, dst, scst);
    }
    core->pc += 4;
    return 0;
}
static int exec_addkpc(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int ucst3 = fields->ucst13_15;

    if (calc_cond(core, insn))
    {
        int scst7 = fields->scst16_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int result;

        DBG("In %s, scst7=%d, core->pc=0x%x\n", __FUNCTION__, scst7, core->pc);
        if (scst7 & 0x40)
        {
            scst7 = 0xFFFFFF80 | scst7;
        }
        result = core->pce1 + (scst7 << 2);
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    core->pc += 4;
    CYCLE_INC(core, ucst3);
    return 0;
}
static int exec_addsp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op5_11;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int gpr_group = x ? (!s) & 0x1 : s;

        uint32_t fp1, fp2;

        if (op == 0x10 || op == 0x70)
        {
            fp1 = core->gpr[s][src1];
            fp2 = core->gpr[gpr_group][src2];
        } else
        {
            NOT_IMP_NO_NEED;
        }

        float src_fp1, src_fp2, dst_fp;

        src_fp1 = *(float *) &fp1;
        src_fp2 = *(float *) &fp2;
        dst_fp = src_fp1 + src_fp2;
        uint32_t result = *(uint32_t *) & dst_fp;

        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, DPTRUNC_DELAY_SLOT);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_addsub(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = BITS(13, 17);
        int src2 = BITS(18, 22);
        int dst = BITS(23, 27);
        int s = BITS(1, 1);
        int x = BITS(12, 12);
        int reg_group = x ? (!s) & 0x1 : s;

        int32_t tmp_add = core->gpr[s][src1] + core->gpr[reg_group][src2];
        int32_t tmp_sub = core->gpr[s][src1] - core->gpr[reg_group][src2];

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP + 1, tmp_add);
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, tmp_sub);
        //      NEW_IMP;
    }
    core->pc += 4;
    return 0;
}
static int exec_addsub2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = BITS(13, 17);
        int src2 = BITS(18, 22);
        int dst = BITS(23, 27);
        int s = BITS(1, 1);
        int x = BITS(12, 12);
        int reg_group = x ? (!s) & 0x1 : s;

        int32_t tmp_upper_add = ((core->gpr[s][src1] >> 16) & 0xffff) + ((core->gpr[reg_group][src2] >> 16) & 0xffff);
        int32_t tmp_low_add = (core->gpr[s][src1] & 0xffff) + (core->gpr[reg_group][src2] & 0xffff);
        int32_t tmp_add = (tmp_upper_add << 16) | (tmp_low_add & 0xFFFF);

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP + 1, tmp_add);

        int32_t tmp_upper_sub = ((core->gpr[s][src1] >> 16) & 0xffff) - ((core->gpr[reg_group][src2] >> 16) & 0xffff);
        int32_t tmp_low_sub = (core->gpr[s][src1] & 0xffff) - (core->gpr[reg_group][src2] & 0xffff);
        int32_t tmp_sub = (tmp_upper_sub << 16) | (tmp_low_sub & 0xFFFF);

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, tmp_sub);

    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_addu(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op5_11;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;
        uint64_t tmp = 0;
        uint64_t result = 0;

        if (op == 0x29)
        {
            tmp = core->gpr[s][src2 + 1] & 0xFF;
            tmp = (tmp << 32);
            tmp |= core->gpr[s][src2];
            tmp &= 0x000000FFFFFFFFFFLL;        // overflowed
            result = tmp + core->gpr[reg_group][src1];
        } else if (op == 0x2b)
        {
            tmp = core->gpr[reg_group][src2];
            result = tmp + core->gpr[s][src1];
        } else
        {
            int64_t s_tmp = 0;

            tmp = core->gpr[s][src1 + 1] & 0xFFFF;
            tmp = tmp << 32;
            tmp |= core->gpr[s][src1];
            result = tmp + core->gpr[reg_group][src2];
            //NEW_IMP;
        }

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result & 0xFFFFFFFF);
        WRITE_BUFFER(core, dst + 1 + s * GPR_NUM_IN_GROUP, result >> 32);
    }
    core->pc += 4;
    return 0;
}

static int exec_add2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op2_11;
        int s = fields->s;
        int x = fields->x;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int reg_group = x ? (!s) & 0x1 : s;
        int32_t tmp_low = 0;
        int32_t tmp_upper = 0;
        int32_t result = 0;

        if (op == 0x18 || op == 0x2e || op == 0x24c)
        {
            tmp_low = (core->gpr[s][src1] & 0xFFFF) + (core->gpr[reg_group][src2] & 0xFFFF);
            tmp_upper = ((core->gpr[s][src1] >> 16) & 0xFFFF) + ((core->gpr[reg_group][src2] >> 16) & 0xFFFF);
            result = (tmp_upper << 16) | (tmp_low & 0xFFFF);
        } else
        {
            NOT_IMP_NO_NEED;
        }
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}

static int exec_add4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int reg_group = x ? (!s) & 0x1 : s;
        int8_t byte0 = 0;
        int8_t byte1 = 0;
        int8_t byte2 = 0;
        int8_t byte3 = 0;
        int32_t result = 0;

        byte0 = ((core->gpr[s][src1] & 0xFF) + (core->gpr[reg_group][src2] & 0xFF));
        byte1 = (((core->gpr[s][src1] >> 8) & 0xFF) + ((core->gpr[reg_group][src2] >> 8) & 0xFF));
        byte2 = (((core->gpr[s][src1] >> 16) & 0xFF) + ((core->gpr[reg_group][src2] >> 16) & 0xFF));
        byte3 = (((core->gpr[s][src1] >> 24) & 0xFF) + ((core->gpr[reg_group][src2] >> 24) & 0xFF));

        result = (byte0 & 0xFF) | ((byte1 & 0xFF) << 8) | ((byte2 & 0xFF) << 16) | ((byte3 & 0xFF) << 24);
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);

    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}

static int exec_and_l(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op5_11;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int reg_group = fields->x ? (!s) & 0x1 : s;
        uint32_t result;

        if (op == 0x7b)
        {
            if (OPSIZE_IS_DW(insn))
            {
                GET_DW_UINTEGER(core->gpr[s][src1], core->gpr[s][src1 + 1], op1);
                GET_DW_UINTEGER(core->gpr[reg_group][src2], core->gpr[reg_group][src2 + 1], op2);
                op1 &= op2;
                CVT_DW_TO_WORDS(op1, l, h);
                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, l);
                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP + 1, h);
            } else
            {
                result = core->gpr[reg_group][src2] & core->gpr[s][src1];
                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
            }
        } else if (op == 0x7a)
        {
            int scst5 = SIGN_EXTEND(src1, 5);

            result = core->gpr[reg_group][src2] & scst5;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }

    core->pc += 4;
    return 0;
}

static int exec_and_s(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op6_11;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;
        uint32_t result;

        if (op == 0x1f)
        {
            result = core->gpr[reg_group][src2] & core->gpr[s][src1];
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else if (op == 0x1e)
        {
            int scst5 = EXTEND_SIGN(src1, 5);

            result = core->gpr[reg_group][src2] & scst5;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}
static int exec_and_s_cst(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        uint32_t result;

        DBG("In %s, src1 = 0x%x\n", __FUNCTION__, src1);
        if (src1 & 0x10)
            src1 = 0xFFFFFFE0 | src1;
        core->gpr[s][dst] = core->gpr[s][src2] & src1;
        if (x)
            result = core->gpr[(!s) & 0x1][src2] & src1;
        else
            result = core->gpr[s][src2] & src1;
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);

    }
    core->pc += 4;
    return 0;
}

static int exec_and_d(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_D1 << fields->s;
    if (calc_cond(core, insn))
    {
        int op = fields->op6_9;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int reg_group = fields->x ? (!s) & 0x1 : s;
        uint32_t result;

        DBG("In %s, src1 = 0x%x\n", __FUNCTION__, src1);

        if (op == 0x7)
        {
            if (src1 & 0x10)
                src1 = 0xFFFFFFE0 | src1;
            result = core->gpr[reg_group][src2] & src1;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else if (op == 0x6)
        {
            result = core->gpr[reg_group][src2] & core->gpr[s][src1];
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_andn(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        uint32_t result;

        if (x)
            result = core->gpr[s][src1] & (~core->gpr[(!s) & 0x1][src2]);
        else
            result = core->gpr[s][src1] & (~core->gpr[s][src2]);
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        DBG("In %s, src1=%d, src2=%d, ret=0x%x\n", __FUNCTION__, src1, src2, core->gpr[s][dst]);
        DBG("In %s, core->gpr[s][src1]=0x%x, ~core->gpr[s][src2]=0x%x, ret=0x%x\n",
            __FUNCTION__, core->gpr[s][src1], ~core->gpr[s][src2], core->gpr[s][dst]);
    }
    core->pc += 4;
    return 0;
}
static int exec_avg2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;

        int op_low1 = SIGN_EXTEND(core->gpr[s][src1] & 0xFFFF, 16);
        int op_low2 = SIGN_EXTEND(core->gpr[reg_group][src2] & 0xFFFF, 16);
        int tmp_low = (op_low1 + op_low2 + 1) >> 1;

        int op1 = SIGN_EXTEND(core->gpr[s][src1] >> 16, 16);
        int op2 = SIGN_EXTEND(core->gpr[reg_group][src2] >> 16, 16);
        int tmp_upper = (op1 + op2 + 1) >> 1;
        int result = (tmp_upper << 16) + (tmp_low & 0x0000FFFF);

        //WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, 1);

    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_avgu4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;

        unsigned char byte0 = 0;
        unsigned char byte1 = 0;
        unsigned char byte2 = 0;
        unsigned char byte3 = 0;
        uint32_t result = 0;

        byte0 = ((core->gpr[s][src1] & 0xFF) + (core->gpr[reg_group][src2] & 0xFF) + 1) >> 1;
        byte1 = (((core->gpr[s][src1] >> 8) & 0xFF) + ((core->gpr[reg_group][src2] >> 8) & 0xFF) + 1) >> 1;
        byte2 = (((core->gpr[s][src1] >> 16) & 0xFF) + ((core->gpr[reg_group][src2] >> 16) & 0xFF) + 1) >> 1;
        byte3 = (((core->gpr[s][src1] >> 24) & 0xFF) + ((core->gpr[reg_group][src2] >> 24) & 0xFF) + 1) >> 1;

        result = (byte0 & 0xFF) | ((byte1 & 0xFF) << 8) | ((byte2 & 0xFF) << 16) | ((byte3 & 0xFF) << 24);

        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, 1);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}

static int exec_b(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int scst21 = fields->scst7_27;

        /* sign extend */
        scst21 = (scst21 & 0x100000) ? (0xFFE00000 | scst21) : scst21;
        REGISTER_BRANCH_EVENT(core, core->pce1 + (scst21 << 2));
        DBG("In %s, delay_slot=%d, \n", __FUNCTION__, core->delay_slot);
    }
    core->pc += 4;
    return 0;
}

static int exec_b_reg(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src2 = fields->src18_22;
        int x = fields->x;
        generic_address_t target_addr;

        if (x)
            target_addr = core->gpr[GPR_A][src2];
        else
            target_addr = core->gpr[GPR_B][src2];
        REGISTER_BRANCH_EVENT(core, target_addr);
    }
    core->pc += 4;
    return 0;
}

static int exec_b_irp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        core->pfc = core->irp;
        DBG("In %s, the return address from intr is 0x%x\n", __FUNCTION__, core->pfc);
        REGISTER_BRANCH_EVENT(core, core->pfc);
    }
    core->pc += 4;
    DBG("in %s,^^^^^^^^^^ core->pc=0x%x\n", __FUNCTION__);
    core->csr |= GIE_BIT;               /* FIXME, should do it after delay slot */

    /*
     * int i = 4;
     * while(((core->ifr >> i) & 0x1) == 0)
     * i++;
     * if(i < 16 ){
     * RW_WRLOCK(core->ifr_lock);
     * //   printf("In %s, clear int_number :%x\n", __func__, i);
     * core->ifr &= ~(1 << i); // clear the corresponding interrupt 
     * DBG("In %s, clear %d int, ifr=0x%x\n", __FUNCTION__, i, core->ifr);
     * RW_UNLOCK(core->ifr_lock);
     * }
     */
    core->intr_flag = 0;
    return 0;
}

static int exec_b_nrp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        core->pfc = core->nrp;
        DBG("In %s, the return address from intr is 0x%x\n", __FUNCTION__, core->pfc);
        REGISTER_BRANCH_EVENT(core, core->pfc);
    }
    core->tsr = core->ntsr;
    core->csr = (core->csr & ~0x1) | (core->tsr & 0x1);

    core->pc += 4;
    //core->csr |= GIE_BIT; /* FIXME, should do it after delay slot */
    //DBG("in %s, pc=0x%x, restored tsr=0x%x ######### \n", __FUNCTION__, core->pc, core->tsr);
    core->ier |= NMIE_BIT;
    /*
     * int i = 4;
     * while(((core->ifr >> i) & 0x1) == 0)
     * i++;
     * if(i < 16 ){
     * RW_WRLOCK(core->ifr_lock);
     * core->ifr &= ~(1 << i); // clear the corresponding interrupt 
     * DBG("In %s, clear %d int, ifr=0x%x\n", __FUNCTION__, i, core->ifr);
     * RW_UNLOCK(core->ifr_lock);
     * }
     */
    core->intr_flag = 0;
    //NEW_IMP;
    return 0;
}

static int exec_bdec(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src = BITS(13, 22);
        int s = fields->s;
        int dst = fields->dst23_27;

        if ((int) core->gpr[s][dst] >= 0)
        {
            int scst10 = EXTEND_SIGN(src, 10);

            core->pfc = core->pce1 + (scst10 << 2);
            DBG("In %s, the return address from intr is 0x%x\n", __FUNCTION__, core->pfc);
            REGISTER_BRANCH_EVENT(core, core->pfc);
            core->gpr[s][dst] -= 1;
        }
    }
    core->pc += 4;
    //NEW_IMP;
    return 0;
}

int Bit8Count(unsigned char n)
{
    n = (n & 0x55) + ((n >> 1) & 0x55);
    n = (n & 0x33) + ((n >> 2) & 0x33);
    n = (n & 0x0f) + ((n >> 4) & 0x0f);

    return n;
}

static int exec_bitc4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;

        unsigned char byte0 = 0;
        unsigned char byte1 = 0;
        unsigned char byte2 = 0;
        unsigned char byte3 = 0;
        uint32_t result = 0;

        byte0 = core->gpr[reg_group][src2] & 0xFF;
        byte1 = (core->gpr[reg_group][src2] >> 8) & 0xFF;
        byte2 = (core->gpr[reg_group][src2] >> 16) & 0xFF;
        byte3 = (core->gpr[reg_group][src2] >> 24) & 0xFF;

        char count0 = 0;
        char count1 = 0;
        char count2 = 0;
        char count3 = 0;

        count0 = Bit8Count(byte0);
        count1 = Bit8Count(byte1);
        count2 = Bit8Count(byte2);
        count3 = Bit8Count(byte3);
        /*
         * while (byte0) {
         * count0 += (byte0 & 0x1);
         * byte0 >>= 1;
         * }
         * while (byte1) {
         * count1 += (byte1 & 0x1);
         * byte1 >>= 1;
         * }
         * while (byte2) {
         * count2 += (byte2 & 0x1);
         * byte2 >>= 1;
         * }
         * while (byte3) {
         * count3 += (byte3 & 0x1);
         * byte3 >>= 1;
         * }
         */
        result = count0 | (count1 << 8) | (count2 << 16) | (count3 << 24);

        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, 1);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}

static int exec_bitr(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;
        int result;

        int reversal_data = core->gpr[reg_group][src2];
        int i;

        for (i = 0; i < 31; i++)
        {
            result |= (reversal_data & 0x1);
            result <<= 1;
            reversal_data >>= 1;
        }
        result |= ((reversal_data >> 31) & 0x1);
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}

static int exec_bnop(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int src1 = fields->src13_15;

    if (calc_cond(core, insn))
    {
        int src2 = fields->src16_27;

        src2 = SIGN_EXTEND(src2, 12);
        if (core->header)               /* compact format */
            core->pfc = core->pce1 + (src2 << 1);
        else
            core->pfc = core->pce1 + (src2 << 2);
        REGISTER_BRANCH_EVENT(core, core->pfc);
    }
    core->pc += 4;
    ADD_PENDING_NOPS(core, src1);
    return 0;
}

static int exec_bnop_reg(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int src1 = fields->src13_15;

    if (calc_cond(core, insn))
    {
        int src2 = fields->src18_22;
        int s = fields->s;
        int x = fields->x;
        int gpr_group = x ? ((!s) & 0x1) : s;

        core->pfc = core->gpr[gpr_group][src2];
        REGISTER_BRANCH_EVENT(core, core->pfc);
    }
    core->pc += 4;
    ADD_PENDING_NOPS(core, src1);
    return 0;
}

static int exec_bpos(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int gpr_group = x ? ((!s) & 0x1) : s;
        int scst10 = BITS(13, 22);

        scst10 = SIGN_EXTEND(scst10, 10);

        if (core->gpr[gpr_group][dst] >= 0)
        {
            core->pfc = core->pce1 + (scst10 << 2);
            REGISTER_BRANCH_EVENT(core, core->pfc);
        }

    }
    core->pc += 4;
    return 0;
}

static int exec_callp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int scst21 = fields->scst7_27;
    int s = fields->s;

    scst21 = SIGN_EXTEND(scst21, 21);
    core->pfc = core->pce1 + (scst21 << 2);
    REGISTER_BRANCH_EVENT(core, core->pfc);
    if (insn & 0x1)
    {
        generic_address_t addr = core->pc + 4;
        uint32_t word;
        int i = 0;                      /* index of one fetch packet */
        uint32_t layout = (core->header >> 21) & 0x7f;
        uint32_t pbits = core->header & 0x3FFF;

        i = ((addr & 0xFFFFFFFC) - core->pce1) / 4;
        int parallel = insn & 0x1;

        while (parallel)
        {
            core_read(core, 32, addr, &word);
            if (((word & 0xe0000000) == 0xe0000000) && ((addr & 0x1f) == 0x1c))
            {
                addr += 4;
                continue;
            }
            /* update header */
            if ((addr & 0x1f) == 0)
            {
                uint32_t header;

                core_read(core, 32, addr + 0x1c, &header);
                if ((header >> 28) == 0xe)
                {
                    layout = (header >> 21) & 0x7f;
                    pbits = header & 0x3FFF;
                } else
                {
                    layout = pbits = 0;
                }
                i = 0;
            }
            DBG("In %s, addr=0x%x, i = %d, layout=0x%x, pbits=0x%x, header=0x%x\n",
                __FUNCTION__, addr, i, layout, pbits, core->header);
            if ((layout >> i) & 0x1)
            {
                parallel = (pbits >> (i * 2)) & 0x1;
                addr += 2;
                if (parallel == 0)
                {
                    break;
                }
                parallel = (pbits >> (i * 2)) & 0x2;
                addr += 2;
                if (parallel == 0)
                {
                    break;
                }
            } else
            {
                parallel = word & 0x1;
                addr += 4;
                if (parallel == 0)
                {
                    break;
                }
            }
            i++;
        }

        if ((addr & 0x1c) == 0x1c && fields->has_compact_fields)
            addr += 4;

        WRITE_BUFFER(core, 3 + s * GPR_NUM_IN_GROUP, addr);
    } else
    {
        generic_address_t addr = core->pc + 4;

        if ((addr & 0x1c) == 0x1c && fields->has_compact_fields)
            addr += 4;
        WRITE_BUFFER(core, 3 + s * GPR_NUM_IN_GROUP, addr);
    }
    DBG("In %s, pfc=0x%x, return addr=0x%x\n", __FUNCTION__, core->pfc, core->gpr[s][3]);
    core->pc += 4;
    ADD_PENDING_NOPS(core, 5);
    return 0;
}

static int exec_ccmatmpy(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int op = fields->op7_11;
        int src2 = fields->src13_17;
        int src1 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        if (op == 0x05)
        {
            int32_t src1_e = core->gpr[s][src1];
            int32_t src1_o = core->gpr[s][src1 + 1];
            int32_t src2_0 = core->gpr[x][src2];
            int32_t src2_1 = core->gpr[x][src2 + 1];
            int32_t src2_2 = core->gpr[x][src2 + 2];
            int32_t src2_3 = core->gpr[x][src2 + 3];

            int64_t tmp0_e =
                ((int64_t) (shigh16(src1_e) * slow16(src2_0)) - (int64_t) (slow16(src1_e) * shigh16(src2_0)));
            int64_t tmp0_o =
                ((int64_t) (shigh16(src1_e) * shigh16(src2_0)) + (int64_t) (slow16(src1_e) * slow16(src2_0)));
            int64_t tmp1_e =
                ((int64_t) (shigh16(src1_o) * slow16(src2_2)) - (int64_t) (slow16(src1_o) * shigh16(src2_2)));
            int64_t tmp1_o =
                ((int64_t) (shigh16(src1_o) * shigh16(src2_2)) + (int64_t) (slow16(src1_o) * slow16(src2_2)));
            int64_t tmp2_e =
                ((int64_t) (shigh16(src1_e) * slow16(src2_1)) - (int64_t) (slow16(src1_e) * shigh16(src2_1)));
            int64_t tmp2_o =
                ((int64_t) (shigh16(src1_e) * shigh16(src2_1)) + (int64_t) (slow16(src1_e) * slow16(src2_1)));
            int64_t tmp3_e =
                ((int64_t) (shigh16(src1_o) * slow16(src2_3)) - (int64_t) (slow16(src1_o) * shigh16(src2_3)));
            int64_t tmp3_o =
                ((int64_t) (shigh16(src1_o) * shigh16(src2_3)) + (int64_t) (slow16(src1_o) * slow16(src2_3)));

            uint32_t dst_0 = ((sat_jz(tmp0_e + tmp1_e)) & 0xffffffff);
            uint32_t dst_1 = ((sat_jz(tmp0_o + tmp1_o)) & 0xffffffff);
            uint32_t dst_2 = ((sat_jz(tmp2_e + tmp3_e)) & 0xffffffff);
            uint32_t dst_3 = ((sat_jz(tmp2_o + tmp3_o)) & 0xffffffff);

            REGISTER_REGW_EVENT_L(core, dst_0, dst, 3);
            REGISTER_REGW_EVENT_H(core, dst_1, dst + 1, 3);
            REGISTER_REGW_EVENT_L(core, dst_2, dst + 2, 3);
            REGISTER_REGW_EVENT_H(core, dst_3, dst + 3, 3);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_ccmatmpyr1(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int op = fields->op7_11;
        int src2 = fields->src13_17;
        int src1 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        if (op == 0x07)
        {
            int32_t src1_e = core->gpr[s][src1];
            int32_t src1_o = core->gpr[s][src1 + 1];
            int32_t src2_0 = core->gpr[x][src2];
            int32_t src2_1 = core->gpr[x][src2 + 1];
            int32_t src2_2 = core->gpr[x][src2 + 2];
            int32_t src2_3 = core->gpr[x][src2 + 3];

            int64_t result0_high, result0_low, result1_high, result1_low, result2_high, result2_low, result3_high,
                result3_low;
            int64_t temp_01_re, temp_01_im, temp_23_re, temp_23_im;
            uint32_t result_low, result_high;

            result0_low = ((int64_t) (shigh16(src1_e) * slow16(src2_0)) - (int64_t) (slow16(src1_e) * shigh16(src2_0)));
            result0_high =
                ((int64_t) (shigh16(src1_e) * shigh16(src2_0)) + (int64_t) (slow16(src1_e) * slow16(src2_0)));
            result1_low = ((int64_t) (shigh16(src1_o) * slow16(src2_2)) - (int64_t) (slow16(src1_o) * shigh16(src2_2)));
            result1_high =
                ((int64_t) (shigh16(src1_o) * shigh16(src2_2)) + (int64_t) (slow16(src1_o) * slow16(src2_2)));
            result2_low = ((int64_t) (shigh16(src1_e) * slow16(src2_1)) - (int64_t) (slow16(src1_e) * shigh16(src2_1)));
            result2_high =
                ((int64_t) (shigh16(src1_e) * shigh16(src2_1)) + (int64_t) (slow16(src1_e) * slow16(src2_1)));
            result3_low = ((int64_t) (shigh16(src1_o) * slow16(src2_3)) - (int64_t) (slow16(src1_o) * shigh16(src2_3)));
            result3_high =
                ((int64_t) (shigh16(src1_o) * shigh16(src2_3)) + (int64_t) (slow16(src1_o) * slow16(src2_3)));

            temp_01_im = (sat_jz(((result0_low + result1_low) + ((int64_t) 0x4000)) << 1) >> 16) & 0xffff;
            temp_01_re = (sat_jz(((result0_high + result1_high) + ((int64_t) 0x4000)) << 1) >> 16) & 0xffff;
            temp_23_im = (sat_jz(((result2_low + result3_low) + ((int64_t) 0x4000)) << 1) >> 16) & 0xffff;
            temp_23_re = (sat_jz(((result2_high + result3_high) + ((int64_t) 0x4000)) << 1) >> 16) & 0xffff;

            result_low = temp_01_re << 16 | temp_01_im;
            result_high = temp_23_re << 16 | temp_23_im;

            REGISTER_REGW_EVENT_L(core, result_low, dst, 3);
            REGISTER_REGW_EVENT_H(core, result_high, dst + 1, 3);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_ccmpy32r1(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = x ? (!s) & 0x1 : s;
        int op = fields->op6_11;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        if (op == 0x9)
        {
            int64_t op1_real = core->gpr[s][src1 + 1];
            int64_t op1_im = core->gpr[s][src1];
            int64_t op2_real = core->gpr[x][src2 + 1];
            int64_t op2_im = core->gpr[x][src2];

            int64_t src1_real = op1_real;

            src1_real <<= 32;
            src1_real >>= 32;
            int64_t src1_im = op1_im;

            src1_im <<= 32;
            src1_im >>= 32;
            int64_t src2_real = op2_real;

            src2_real <<= 32;
            src2_real >>= 32;
            int64_t src2_im = op2_im;

            src2_im <<= 32;
            src2_im >>= 32;

            int64_t tmp_real, tmp_im;
            int64_t result_real, result_im;

            if (op1_real == 0x80000000 && op1_im == 0x80000000 && op2_real == 0x80000000 && op2_im == 0x80000000)
            {
                tmp_real = 0x7fffffff;
                result_real = tmp_real;
            } else
            {
                tmp_real = (src1_real * src2_real + src1_im * src2_im + (1 << 30)) >> 31;
                result_real = sat_jz(tmp_real);
            }
            tmp_im = (src1_real * -src2_im + src1_im * src2_real + (1 << 30)) >> 31;
            result_im = sat_jz(tmp_im);

            REGISTER_REGW_EVENT_L(core, result_im, dst, 3);
            REGISTER_REGW_EVENT_H(core, result_real, dst + 1, 3);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_clr(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        uint32_t csta = fields->csta;
        uint32_t cstb = fields->cstb;

        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        uint32_t result;

        if (cstb >= csta)
        {
            int v;

            if (csta == 0)
                v = (0xFFFFFFFFFFLL << (cstb + 1));
            else
                v = ((0xFFFFFFFFFFLL << (cstb + 1)) | (0xFFFFFFFF >> (32 - csta)));

            result = core->gpr[s][src2] & v;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
            DBG("In %s, csta=%d, cstb=%d, v=0x%x, src2=%d, dst=%d\n", __FUNCTION__, csta, cstb, v, src2, dst);
        } else
        {
            result = core->gpr[s][src2];
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
            DBG("In %s, csta=%d, cstb=%d, v=0x%x, src2=%d, dst=%d\n", __FUNCTION__, csta, cstb, v, src2, dst);
        }

    }
    core->pc += 4;
    return 0;
}

static int exec_clr_reg_form(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int reg_group = fields->x ? (!s) & 0x1 : s;
        uint32_t src1 = core->gpr[s][fields->src13_17];
        uint32_t csta = ((src1 >> (5)) & ((1 << (1 + (9) - (5))) - 1)); // src1[9..5]
        uint32_t cstb = src1 & 0x1F;    // src1[4..0]
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        uint32_t result;
        int v;

        if (cstb >= csta)
        {
            if (csta == 0)
                v = (0xFFFFFFFFFFLL << (cstb + 1));
            else
                v = ((0xFFFFFFFFFFLL << (cstb + 1)) | (0xFFFFFFFF >> (32 - csta)));
        }

        result = core->gpr[reg_group][src2] & v;
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }

    core->pc += 4;
    return 0;
}

static int exec_cmatmpy(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int op = fields->op7_11;
        int src2 = fields->src13_17;
        int src1 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        if (op == 0x04)
        {
            int32_t src1_e = core->gpr[s][src1];
            int32_t src1_o = core->gpr[s][src1 + 1];
            int32_t src2_0 = core->gpr[x][src2];
            int32_t src2_1 = core->gpr[x][src2 + 1];
            int32_t src2_2 = core->gpr[x][src2 + 2];
            int32_t src2_3 = core->gpr[x][src2 + 3];

            int64_t tmp0_e =
                ((int64_t) (shigh16(src1_e) * slow16(src2_0)) + (int64_t) (slow16(src1_e) * shigh16(src2_0)));
            int64_t tmp0_o =
                ((int64_t) (shigh16(src1_e) * shigh16(src2_0)) - (int64_t) (slow16(src1_e) * slow16(src2_0)));
            int64_t tmp1_e =
                ((int64_t) (shigh16(src1_o) * slow16(src2_2)) + (int64_t) (slow16(src1_o) * shigh16(src2_2)));
            int64_t tmp1_o =
                ((int64_t) (shigh16(src1_o) * shigh16(src2_2)) - (int64_t) (slow16(src1_o) * slow16(src2_2)));
            int64_t tmp2_e =
                ((int64_t) (shigh16(src1_e) * slow16(src2_1)) + (int64_t) (slow16(src1_e) * shigh16(src2_1)));
            int64_t tmp2_o =
                ((int64_t) (shigh16(src1_e) * shigh16(src2_1)) - (int64_t) (slow16(src1_e) * slow16(src2_1)));
            int64_t tmp3_e =
                ((int64_t) (shigh16(src1_o) * slow16(src2_3)) + (int64_t) (slow16(src1_o) * shigh16(src2_3)));
            int64_t tmp3_o =
                ((int64_t) (shigh16(src1_o) * shigh16(src2_3)) - (int64_t) (slow16(src1_o) * slow16(src2_3)));

            uint32_t dst_0 = ((sat_jz(tmp0_e + tmp1_e)) & 0xffffffff);
            uint32_t dst_1 = ((sat_jz(tmp0_o + tmp1_o)) & 0xffffffff);
            uint32_t dst_2 = ((sat_jz(tmp2_e + tmp3_e)) & 0xffffffff);
            uint32_t dst_3 = ((sat_jz(tmp2_o + tmp3_o)) & 0xffffffff);

            REGISTER_REGW_EVENT_L(core, dst_0, dst, 3);
            REGISTER_REGW_EVENT_H(core, dst_1, dst + 1, 3);
            REGISTER_REGW_EVENT_L(core, dst_2, dst + 2, 3);
            REGISTER_REGW_EVENT_H(core, dst_3, dst + 3, 3);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_cmatmpyr1(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int op = fields->op7_11;
        int src2 = fields->src13_17;
        int src1 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        if (op == 0x06)
        {
            int32_t src1_e = core->gpr[s][src1];
            int32_t src1_o = core->gpr[s][src1 + 1];
            int32_t src2_0 = core->gpr[x][src2];
            int32_t src2_1 = core->gpr[x][src2 + 1];
            int32_t src2_2 = core->gpr[x][src2 + 2];
            int32_t src2_3 = core->gpr[x][src2 + 3];

            int64_t tmp0_e =
                ((int64_t) (shigh16(src1_e) * slow16(src2_0)) + (int64_t) (slow16(src1_e) * shigh16(src2_0)));
            int64_t tmp0_o =
                ((int64_t) (shigh16(src1_e) * shigh16(src2_0)) - (int64_t) (slow16(src1_e) * slow16(src2_0)));
            int64_t tmp1_e =
                ((int64_t) (shigh16(src1_o) * slow16(src2_2)) + (int64_t) (slow16(src1_o) * shigh16(src2_2)));
            int64_t tmp1_o =
                ((int64_t) (shigh16(src1_o) * shigh16(src2_2)) - (int64_t) (slow16(src1_o) * slow16(src2_2)));
            int64_t tmp2_e =
                ((int64_t) (shigh16(src1_e) * slow16(src2_1)) + (int64_t) (slow16(src1_e) * shigh16(src2_1)));
            int64_t tmp2_o =
                ((int64_t) (shigh16(src1_e) * shigh16(src2_1)) - (int64_t) (slow16(src1_e) * slow16(src2_1)));
            int64_t tmp3_e =
                ((int64_t) (shigh16(src1_o) * slow16(src2_3)) + (int64_t) (slow16(src1_o) * shigh16(src2_3)));
            int64_t tmp3_o =
                ((int64_t) (shigh16(src1_o) * shigh16(src2_3)) - (int64_t) (slow16(src1_o) * slow16(src2_3)));

            uint32_t dst_e;
            uint32_t dst_o;

            //      shigh16(sat(((tmp0_e + tmp1_e) + 00004000h) << 1)) -> slow16(dst_e)
            dst_e = shigh16((sat_jz((tmp0_e + tmp1_e) + (int64_t) 0x4000) << 1)) & 0xFFFF;
            dst_e = (shigh16((sat_jz((tmp0_o + tmp1_o) + (int64_t) 0x4000) << 1)) << 16) | (dst_e);
            dst_o = shigh16((sat_jz((tmp2_e + tmp3_e) + (int64_t) 0x4000) << 1)) & 0xFFFF;
            dst_o = (shigh16((sat_jz((tmp2_o + tmp3_o) + (int64_t) 0x4000) << 1)) << 16) | (dst_o);

            REGISTER_REGW_EVENT_L(core, dst_e, dst, 3);
            REGISTER_REGW_EVENT_H(core, dst_o, dst + 1, 3);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_cmpeqdp(c6k_core_t * core, uint32_t insn, opcode_fields_t * unused)
{
    if (calc_cond(core, insn))
    {
        int src1 = BITS(13, 17);
        int src2 = BITS(18, 22);
        int dst = BITS(23, 27);
        int x = BITS(12, 12);
        int s = BITS(1, 1);
        uint64_t fp1, fp2;
        int reg_group = x ? (!s) & 0x1 : s;

        fp1 = core->gpr[s][src1 + 1];
        fp1 = (fp1 << 32);
        fp1 |= core->gpr[s][src1];

        fp2 = core->gpr[reg_group][src2 + 1];
        fp2 = (fp2 << 32);
        fp2 |= core->gpr[reg_group][src2];

        double src_fp1, src_fp2;

        src_fp1 = *(double *) &fp1;
        src_fp2 = *(double *) &fp2;
        int result = src_fp1 == src_fp2 ? 1 : 0;

        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, CMPEQDP_DELAY_SLOT);
    }
    core->pc += 4;
    return 0;
}

static int exec_cmpeqsp(c6k_core_t * core, uint32_t insn, opcode_fields_t * unused)
{
    if (calc_cond(core, insn))
    {
        int src1 = BITS(13, 17);
        int src2 = BITS(18, 22);
        int dst = BITS(23, 27);
        int s = BITS(1, 1);
        int x = BITS(12, 12) ? (!s) & 0x1 : s;
        int result;

        result = (core->gpr[s][src1] == core->gpr[x][src2]) ? 1 : 0;

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    core->pc += 4;
    return 0;
}

static int exec_cmpltdp(c6k_core_t * core, uint32_t insn, opcode_fields_t * unused)
{
    if (calc_cond(core, insn))
    {
        int src1 = BITS(13, 17);
        int src2 = BITS(18, 22);
        int dst = BITS(23, 27);
        int x = BITS(12, 12);
        int s = BITS(1, 1);
        uint64_t fp1, fp2;
        int reg_group = x ? (!s) & 0x1 : s;

        fp1 = core->gpr[s][src1 + 1];
        fp1 = (fp1 << 32);
        fp1 |= core->gpr[s][src1];

        fp2 = core->gpr[reg_group][src2 + 1];
        fp2 = (fp2 << 32);
        fp2 |= core->gpr[reg_group][src2];

        double src_fp1, src_fp2;

        src_fp1 = *(double *) &fp1;
        src_fp2 = *(double *) &fp2;
        int result = src_fp1 < src_fp2 ? 1 : 0;

        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, CMPLTDP_DELAY_SLOT);
    }
    core->pc += 4;
    return 0;
}
static int exec_cmpgtsp(c6k_core_t * core, uint32_t insn, opcode_fields_t * unused)
{
    if (calc_cond(core, insn))
    {
        int src1 = BITS(13, 17);
        int src2 = BITS(18, 22);
        int dst = BITS(23, 27);
        int x = BITS(12, 12);
        int s = BITS(1, 1);
        int32_t fp1, fp2;
        int reg_group = x ? (!s) & 0x1 : s;

        fp1 = core->gpr[s][src1];
        fp2 = core->gpr[reg_group][src2];

        float src_fp1, src_fp2;

        src_fp1 = *(float *) &fp1;
        src_fp2 = *(float *) &fp2;
        int result = src_fp1 > src_fp2 ? 1 : 0;

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_cmpgtdp(c6k_core_t * core, uint32_t insn, opcode_fields_t * unused)
{
    if (calc_cond(core, insn))
    {
        int src1 = BITS(13, 17);
        int src2 = BITS(18, 22);
        int dst = BITS(23, 27);
        int x = BITS(12, 12);
        int s = BITS(1, 1);
        uint64_t fp1, fp2;
        int reg_group = x ? (!s) & 0x1 : s;

        fp1 = core->gpr[s][src1 + 1];
        fp1 = (fp1 << 32);
        fp1 |= core->gpr[s][src1];

        fp2 = core->gpr[reg_group][src2 + 1];
        fp2 = (fp2 << 32);
        fp2 |= core->gpr[reg_group][src2];

        double src_fp1, src_fp2;

        src_fp1 = *(double *) &fp1;
        src_fp2 = *(double *) &fp2;
        int result = src_fp1 > src_fp2 ? 1 : 0;

        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, CMPGTDP_DELAY_SLOT);
    }
    core->pc += 4;
    return 0;
}

static int exec_cmpeq(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op5_11;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;

        fields->exec_unit = UNIT_L1 << s;
        int x = fields->x;

        switch (op)
        {
            case 0x52:
                {
                    if (x)
                        src2 = core->gpr[(!s) & 0x1][src2];
                    else
                        src2 = core->gpr[s][src2];
                    src1 = (src1 & 0x10) ? (src1 | 0xFFFFFFE0) : src1;
                    src2 = (src2 & 0x10) ? (src2 | 0xFFFFFFE0) : src2;
                    uint32_t result = (src1 == src2) ? 1 : 0;

                    WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
                }
                break;
            case 0x53:
                {
                    int reg_group = x ? (!s) & 0x1 : s;
                    uint32_t result = (core->gpr[s][src1] == core->gpr[reg_group][src2]);

                    WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
                }
                break;
            case 0x51:
                {
                    int result;
                    int reg_group = x ? (!s) & 0x1 : s;
                    int64_t op1 = (int64_t) (int32_t) core->gpr[reg_group][src1];
                    int64_t op2 = ((int64_t) (core->gpr[s][src2 + 1] & 0xFF)) << 32;

                    if (op2 & ((int64_t) 1 << 39))
                        op2 |= 0xFFFFFF0000000000LL;
                    op2 |= core->gpr[s][src2];
                    result = op1 == op2 ? 1 : 0;
                    WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
                }
                break;
            case 0x50:
                {
                    int result;
                    int64_t op1 = (int64_t) SIGN_EXTEND(src1, 5);
                    int64_t op2 = ((int64_t) (core->gpr[s][src2 + 1] & 0xFF)) << 32;

                    if (op2 & ((int64_t) 1 << 39))
                        op2 |= 0xFFFFFF0000000000LL;
                    op2 |= core->gpr[s][src2];
                    result = op1 == op2 ? 1 : 0;
                    WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
                }
                break;
            default:
                NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int32_t cmpeq2(int32_t a, int32_t b)
{
    int carry[2];

    carry[0] = ((a & 0xffff) == (b & 0xffff)) ? 1 : 0;
    a >>= 16;
    b >>= 16;

    carry[1] = ((a & 0xffff) == (b & 0xffff)) ? 1 : 0;
    return carry[1] << 1 | carry[0];
}

static int exec_dcmpeq2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int op = fields->op6_11;

        if (op == 0x1d)
        {
            uint32_t src1_o = core->gpr[s][src1 + 1];
            uint32_t src1_e = core->gpr[s][src1];
            uint32_t src2_o = core->gpr[x][src2 + 1];
            uint32_t src2_e = core->gpr[x][src2];

            uint32_t result;

            result = (cmpeq2(src1_o, src2_o) << 2) | cmpeq2(src1_e, src2_e);

            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int32_t cmpeq4(int32_t a, int32_t b)
{
    int carry[4];

    carry[0] = ((a & 0xff) == (b & 0xff)) ? 1 : 0;
    a >>= 8;
    b >>= 8;

    carry[1] = ((a & 0xff) == (b & 0xff)) ? 1 : 0;
    a >>= 8;
    b >>= 8;

    carry[2] = ((a & 0xff) == (b & 0xff)) ? 1 : 0;
    a >>= 8;
    b >>= 8;

    carry[3] = ((a & 0xff) == (b & 0xff)) ? 1 : 0;

    return carry[3] << 3 | carry[2] << 2 | carry[1] << 1 | carry[0];
}

static int exec_dcmpeq4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int op = fields->op6_11;

        if (op == 0x1c)
        {
            uint32_t src1_o = core->gpr[s][src1 + 1];
            uint32_t src1_e = core->gpr[s][src1];
            uint32_t src2_o = core->gpr[x][src2 + 1];
            uint32_t src2_e = core->gpr[x][src2];

            uint32_t result;

            result = (cmpeq4(src1_o, src2_o) << 4) | cmpeq4(src1_e, src2_e);

            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int32_t cmpgt2(int32_t a, int32_t b)
{
    int32_t l1, l2, h1, h2, c;

    l1 = 0xffff & a;
    if (0x8000 & l1)
        l1 |= 0xffff0000;
    h1 = 0xffff & (a >> 16);
    if (0x8000 & h1)
        h1 |= 0xffff0000;
    l2 = 0xffff & b;
    if (0x8000 & l2)
        l2 |= 0xffff0000;
    h2 = 0xffff & (b >> 16);
    if (0x8000 & h2)
        h2 |= 0xffff0000;

    c = ((int32_t) (l1) > (int32_t) (l2) ? 1 : 0) | ((int32_t) (h1) > (int32_t) (h2) ? 2 : 0);
    return c;
}

static int exec_dcmpgt2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int op = fields->op6_11;

        if (op == 0x14)
        {
            uint32_t src1_o = core->gpr[s][src1 + 1];
            uint32_t src1_e = core->gpr[s][src1];
            uint32_t src2_o = core->gpr[x][src2 + 1];
            uint32_t src2_e = core->gpr[x][src2];

            uint32_t result;

            result = (cmpgt2(src1_o, src2_o) << 2) | cmpgt2(src1_e, src2_e);

            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_dcmpgtu4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int op = fields->op6_11;

        if (op == 0x15)
        {
            uint32_t src1_o = core->gpr[s][src1 + 1];
            uint32_t src1_e = core->gpr[s][src1];
            uint32_t src2_o = core->gpr[x][src2 + 1];
            uint32_t src2_e = core->gpr[x][src2];

            uint32_t result;

            result = ((uint32_t) (0x000000ff & src1_e) > (uint32_t) (0x000000ff & src2_e) ? 0x01 : 0) |
                ((uint32_t) (0x0000ff00 & src1_e) > (uint32_t) (0x0000ff00 & src2_e) ? 0x02 : 0) |
                ((uint32_t) (0x00ff0000 & src1_e) > (uint32_t) (0x00ff0000 & src2_e) ? 0x04 : 0) |
                ((uint32_t) (0xff000000 & src1_e) > (uint32_t) (0xff000000 & src2_e) ? 0x08 : 0) |
                ((uint32_t) (0x000000ff & src1_o) > (uint32_t) (0x000000ff & src2_o) ? 0x10 : 0) |
                ((uint32_t) (0x0000ff00 & src1_o) > (uint32_t) (0x0000ff00 & src2_o) ? 0x20 : 0) |
                ((uint32_t) (0x00ff0000 & src1_o) > (uint32_t) (0x00ff0000 & src2_o) ? 0x40 : 0) |
                ((uint32_t) (0xff000000 & src1_o) > (uint32_t) (0xff000000 & src2_o) ? 0x80 : 0);

            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_dcmpy(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op6_11;

        if (op == 0x4)
        {
            uint32_t src1_o = core->gpr[s][src1 + 1];
            uint32_t src1_e = core->gpr[s][src1];
            uint32_t src2_o = core->gpr[x][src2 + 1];
            uint32_t src2_e = core->gpr[x][src2];

            uint32_t dst_3;
            uint32_t dst_2;
            uint32_t dst_1;
            uint32_t dst_0;

            if (src1_o == 0x80008000 && src1_e == 0x80008000 && src2_o == 0x80008000 && src2_e == 0x80008000)
            {
                dst_3 = 0x7fffffff;
                dst_1 = 0x7fffffff;
            } else
            {
                dst_2 = (slow16(src1_o) * shigh16(src2_o)) + (shigh16(src1_o) * slow16(src2_o));
                dst_0 = (slow16(src1_e) * shigh16(src2_e)) + (shigh16(src1_e) * slow16(src2_e));
            }
            dst_3 = ((shigh16(src1_o) * shigh16(src2_o)) - (slow16(src1_o) * slow16(src2_o)));
            dst_1 = ((shigh16(src1_e) * shigh16(src2_e)) - (slow16(src1_e) * slow16(src2_e)));

            REGISTER_REGW_EVENT_L(core, dst_0, dst, 3);
            REGISTER_REGW_EVENT_H(core, dst_1, dst + 1, 3);
            REGISTER_REGW_EVENT_H(core, dst_2, dst + 2, 3);
            REGISTER_REGW_EVENT_H(core, dst_3, dst + 3, 3);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int32_t cmpyr1_joule(int32_t a, int32_t b)
{
    int a_lo, a_hi, b_lo, b_hi;
    int64_t temp;
    int32_t result;
    int32_t result1;

    a_hi = shigh16(a);
    a_lo = slow16(a);
    b_hi = shigh16(b);
    b_lo = slow16(b);

    // lsb 16
    temp = (int64_t) (a_lo * b_hi) + (a_hi * b_lo) + ((int64_t) 0x4000);
    result = ((sat_jz(temp << 1)) >> 16) & 0xffff;

    // msb16
    temp = (int64_t) (a_hi * b_hi) - (a_lo * b_lo) + ((int64_t) 0x4000);

    result1 = sat_jz(temp << 1) & 0xffff0000;
    result |= result1;

    return result;
}

static int exec_dcmpyr1(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op6_11;

        if (op == 0xd)
        {
            uint32_t src1_o = core->gpr[s][src1 + 1];
            uint32_t src1_e = core->gpr[s][src1];
            uint32_t src2_o = core->gpr[x][src2 + 1];
            uint32_t src2_e = core->gpr[x][src2];

            uint32_t dst_o = cmpyr1_joule(src1_o, src2_o);
            uint32_t dst_e = cmpyr1_joule(src1_e, src2_e);

            REGISTER_REGW_EVENT_L(core, dst_e, dst, 3);
            REGISTER_REGW_EVENT_H(core, dst_o, dst + 1, 3);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_cmpeq2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;

        int result;
        int cmp_low = (core->gpr[s][src1] & 0xFFFF) == (core->gpr[reg_group][src2] & 0xFFFF) ? 1 : 0;
        int cmp_upper = (core->gpr[s][src1] >> 16) == (core->gpr[reg_group][src2] >> 16) ? 2 : 0;

        result = cmp_low | cmp_upper;
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}

static int exec_cmpeq4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;

        int result;
        int byte0 = (core->gpr[s][src1] & 0xFF) == (core->gpr[reg_group][src2] & 0xFF) ? 1 : 0;
        int byte1 = ((core->gpr[s][src1] >> 8) & 0xFF) == ((core->gpr[reg_group][src2] >> 8) & 0xFF) ? 2 : 0;
        int byte2 = ((core->gpr[s][src1] >> 16) & 0xFF) == ((core->gpr[reg_group][src2] >> 16) & 0xFF) ? 4 : 0;
        int byte3 = ((core->gpr[s][src1] >> 24) & 0xFF) == ((core->gpr[reg_group][src2] >> 24) & 0xFF) ? 8 : 0;

        result = byte0 | byte1 | byte2 | byte3;
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}

static int exec_cmpgt(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op5_11;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;

        if (op == 0x46)
        {
            src1 = SIGN_EXTEND(src1, 5);
            int op2 = core->gpr[reg_group][src2];

            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, (src1 > op2));
        } else if (op == 0x47)
        {
            int op1 = core->gpr[s][src1];
            int op2 = core->gpr[reg_group][src2];
            int result = op1 > op2 ? 1 : 0;

            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else if (op == 0x45)
        {
            int result;
            int64_t op1 = (int64_t) (int32_t) core->gpr[reg_group][src1];
            int64_t op2 = ((int64_t) (core->gpr[s][src2 + 1] & 0xFF)) << 32;

            if (op2 & ((int64_t) 1 << 39))
                op2 |= 0xFFFFFF0000000000LL;
            op2 |= core->gpr[s][src2];
            result = op1 > op2 ? 1 : 0;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else if (op == 0x44)
        {
            int result;
            int64_t op1 = (int64_t) SIGN_EXTEND(src1, 5);
            int64_t op2 = ((int64_t) (core->gpr[s][src2 + 1] & 0xFF)) << 32;

            if (op2 & ((int64_t) 1 << 39))
                op2 |= 0xFFFFFF0000000000LL;
            op2 |= core->gpr[s][src2];
            result = op1 > op2 ? 1 : 0;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}
static int exec_cmpgt2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;
        int result;

        int cmp_low = (core->gpr[s][src1] & 0xFFFF) > (core->gpr[reg_group][src2] & 0xFFFF) ? 1 : 0;
        int cmp_upper = (core->gpr[s][src1] >> 16) > (core->gpr[reg_group][src2] >> 16) ? 2 : 0;

        result = cmp_low | cmp_upper;
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_cmpgtu(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_L2 << BITS(0, 0);
    if (calc_cond(core, insn))
    {
        int op = fields->op5_11;
        uint32_t src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;
        uint32_t result;

        if (op == 0x4f)
        {
            result = (core->gpr[s][src1] > core->gpr[reg_group][src2]) ? 1 : 0;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else if (op == 0x4e)
        {
            src1 &= 0xf;
            result = (src1 > core->gpr[reg_group][src2]) ? 1 : 0;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else if (op == 0x4d)
        {
            result = (core->gpr[reg_group][src1] > core->gpr[s][src2]) ? 1 : 0;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else if (op == 0x4c)
        {
#ifdef __C62X__
            src1 &= 0xf;
#endif
            result = (src1 > core->gpr[s][src2]) ? 1 : 0;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_cmpgtu4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;
        uint32_t result;

        uint8_t byte0 = (uint8_t) (core->gpr[s][src1] & 0xFF) > (uint8_t) (core->gpr[reg_group][src2] & 0xFF) ? 1 : 0;
        uint8_t byte1 =
            (uint8_t) ((core->gpr[s][src1] >> 8) & 0xFF) > (uint8_t) ((core->gpr[reg_group][src2] >> 8) & 0xFF) ? 2 : 0;
        uint8_t byte2 =
            (uint8_t) ((core->gpr[s][src1] >> 16) & 0xFF) >
            (uint8_t) ((core->gpr[reg_group][src2] >> 16) & 0xFF) ? 4 : 0;
        uint8_t byte3 =
            (uint8_t) ((core->gpr[s][src1] >> 24) & 0xFF) >
            (uint8_t) ((core->gpr[reg_group][src2] >> 24) & 0xFF) ? 8 : 0;

        result = byte0 | byte1 | byte2 | byte3;
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_cmplt(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op5_11;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;

        fields->exec_unit = UNIT_L1 << s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;
        uint32_t result;

        if (op == 0x57)
        {
            if (x)
                result = ((int) core->gpr[s][src1] < (int) core->gpr[(!s) & 0x1][src2]) ? 1 : 0;
            else
                result = ((int) core->gpr[s][src1] < (int) core->gpr[s][src2]) ? 1 : 0;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else if (op == 0x56)
        {
            int op1 = SIGN_EXTEND(src1, 5);

            if (x)
                result = (op1 < (int) core->gpr[(!s) & 0x1][src2]) ? 1 : 0;
            else
                result = (op1 < (int) core->gpr[s][src2]) ? 1 : 0;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else if (op == 0x55)
        {
            int op1 = core->gpr[reg_group][src1];
            int64_t op2;

            op2 = ((int64_t) (core->gpr[s][src2 + 1]) << 32) | (core->gpr[s][src2]);
            result = (op1 < op2) ? 1 : 0;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
            //NEW_IMP;
        } else if (op == 0x54)
        {
            int op1 = SIGN_EXTEND(src1, 5);
            int64_t op2;

            op2 = ((int64_t) (core->gpr[s][src2 + 1]) << 32) | (core->gpr[s][src2]);
            result = (op1 < op2) ? 1 : 0;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
            //NEW_IMP;
        } else
        {
            NOT_IMP_NO_NEED;
        }

    }
    core->pc += 4;
    return 0;
}
static int exec_cmplt2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_op = core->gpr[s][src1];
        int32_t src2_op = core->gpr[s][src2];

        int32_t dst_0, dst_1;

        dst_0 = (slow16(src2_op) < slow16(src1_op)) ? 1 : 0;
        dst_1 = (shigh16(src2_op) < shigh16(src1_op)) ? 1 : 0;

        int32_t result = (dst_1 << 16) | dst_0;

        WRITE_BUFFER(core, dst, result);
    }
    core->pc += 4;
    return 0;
}

static int exec_cmpltsp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_S1 << fields->s;
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int reg_group = fields->x ? (!s) & 0x1 : s;
        int32_t result = 0;
        float *sp = (float *) &core->gpr[s][src1];
        float *xsp = (float *) &core->gpr[reg_group][src2];

        if (*sp < *xsp)
            result = 1;
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }

    core->pc += 4;
    return 0;
}

static int exec_cmpltu(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op5_11;
        uint32_t src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        uint32_t result;

        if (op == 0x5f)
        {
            if (x)
                result = (core->gpr[s][src1] < core->gpr[(!s) & 0x1][src2]) ? 1 : 0;
            else
                result = (core->gpr[s][src1] < core->gpr[s][src2]) ? 1 : 0;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else if (op == 0x5e)
        {
            uint32_t ucst4 = src1 & 0x1f;

            if (x)
                result = (ucst4 < core->gpr[(!s) & 0x1][src2]) ? 1 : 0;
            else
                result = (ucst4 < core->gpr[s][src2]) ? 1 : 0;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);

        } else if (op == 0x5d)
        {
            uint64_t ulong_src2 = ((int64_t) (core->gpr[s][src2 + 1] & 0xFF)) << 32;

            ulong_src2 |= core->gpr[s][src2];
            if (x)
                result = (core->gpr[(!s) & 0x1][src1]) < ulong_src2 ? 1 : 0;
            else
                result = (core->gpr[s][src1]) < ulong_src2 ? 1 : 0;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else if (op == 0x5c)
        {
            uint32_t ucst4 = src1 & 0x1f;
            uint64_t ulong_src2 = ((int64_t) (core->gpr[s][src2 + 1] & 0xFF)) << 32;

            ulong_src2 |= core->gpr[s][src2];

            result = (ucst4 < ulong_src2) ? 1 : 0;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}
static int exec_cmpltu4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int src1_op = core->gpr[s][src1];
        int src2_op = core->gpr[x][src2];

        int32_t dst_0 = (ubyte0(src2_op) < ubyte0(src1_op)) ? 1 : 0;
        int32_t dst_1 = (ubyte1(src2_op) < ubyte1(src1_op)) ? 2 : 0;
        int32_t dst_2 = (ubyte2(src2_op) < ubyte2(src1_op)) ? 4 : 0;
        int32_t dst_3 = (ubyte3(src2_op) < ubyte3(src1_op)) ? 8 : 0;

        int32_t result = dst_3 | dst_2 | dst_1 | dst_0;

        WRITE_BUFFER(core, dst, result);
    }
    core->pc += 4;
    return 0;
}

static int32_t complex_rot270(int32_t op1)
{
    int32_t op1_high;
    int32_t op1_low;

    op1_high = uhigh16(op1);
    op1_low = ulow16(op1);

    if (op1_high == 0x8000)
        return (op1_low << 16) | 0x7fff;

    return (op1_low << 16) | (0xFFFF & -op1_high);
}

static int exec_dcrot270(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = BITS(13, 17);
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;

        if (op == 0x1a)
        {

            int32_t src2_e = core->gpr[x][src2];
            int32_t src2_o = core->gpr[x][src2 + 1];

            uint32_t upper = complex_rot270(src2_o);
            uint32_t low = complex_rot270(src2_e);

            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, low);
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP + 1, upper);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int32_t complex_rot90(int32_t op1)
{
    int32_t op1_high;
    int32_t op1_low;

    op1_high = uhigh16(op1);
    op1_low = ulow16(op1);

    if (op1_low == 0x8000)
        return 0x7fff0000 | op1_high;

    return ((0xFFFF & -op1_low) << 16) | op1_high;
}

static int exec_dcrot90(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = BITS(13, 17);
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;

        if (op == 0x19)
        {

            int32_t src2_e = core->gpr[x][src2];
            int32_t src2_o = core->gpr[x][src2 + 1];

            int32_t upper = complex_rot90(src2_o);
            int32_t low = complex_rot90(src2_e);

            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, low);
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP + 1, upper);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_crot270(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = BITS(13, 17);
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;

        if (op == 0xa)
        {
            int32_t result = complex_rot270(core->gpr[x][src2]);

            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_crot90(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = BITS(13, 17);
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;

        if (op == 0x9)
        {
            int32_t result = complex_rot90(core->gpr[x][src2]);

            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_cmpy(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;

        int32_t result_low;
        int32_t result_upper;

        result_low =
            (core->gpr[s][src1] & 0xFFFF) * (core->gpr[reg_group][src2] >> 16) +
            (core->gpr[s][src1] >> 16) * (core->gpr[reg_group][src2] & 0xFFFF);
        result_upper =
            (core->gpr[s][src1] >> 16) * (core->gpr[reg_group][src2] >> 16) -
            (core->gpr[s][src1] & 0xFFFF) * (core->gpr[reg_group][src2] & 0xFFFF);

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result_low);
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP + 1, result_upper);

    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}

static int exec_cmpy32r1(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = x ? (!s) & 0x1 : s;
        int op = fields->op6_11;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        if (op == 0x8)
        {
            int64_t op1_real = core->gpr[s][src1 + 1];
            int64_t op1_im = core->gpr[s][src1];
            int64_t op2_real = core->gpr[x][src2 + 1];
            int64_t op2_im = core->gpr[x][src2];

            int64_t src1_real = op1_real;

            src1_real <<= 32;
            src1_real >>= 32;
            int64_t src1_im = op1_im;

            src1_im <<= 32;
            src1_im >>= 32;
            int64_t src2_real = op2_real;

            src2_real <<= 32;
            src2_real >>= 32;
            int64_t src2_im = op2_im;

            src2_im <<= 32;
            src2_im >>= 32;

            int64_t tmp_real, tmp_im;
            int64_t result_real, result_im;

            tmp_real = (src1_real * src2_real - src1_im * src2_im + (1 << 30)) >> 31;
            result_real = sat_jz(tmp_real);

            if (op1_real == 0x80000000 && op1_im == 0x80000000 && op2_real == 0x80000000 && op2_im == 0x80000000)
            {
                tmp_real = 0x7fffffff;
                result_real = tmp_real;
            } else
            {
                tmp_im = (src1_real * src2_im + src1_im * src2_real + (1 << 30)) >> 31;
                result_im = sat_jz(tmp_im);
            }

            REGISTER_REGW_EVENT_L(core, result_im, dst, 3);
            REGISTER_REGW_EVENT_H(core, result_real, dst + 1, 3);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_cmpyr(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;

        int result_lsb;
        int result_msb;

        result_msb =
            ((core->gpr[s][src1] & 0xFFFF) * (core->gpr[reg_group][src2] >> 16) +
             (core->gpr[s][src1] >> 16) * (core->gpr[reg_group][src2] & 0xFFFF) + 0x8000) >> 16;
        result_lsb =
            ((core->gpr[s][src1] >> 16) * (core->gpr[reg_group][src2] >> 16) -
             (core->gpr[s][src1] & 0xFFFF) * (core->gpr[reg_group][src2] & 0xFFFF) + 0x8000) >> 16;

        int result = result_lsb | (result_msb << 16);

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    core->pc += 4;
    return 0;
}
static int exec_cmpyr1(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        uint32_t result = cmpyr1_joule(core->gpr[s][src1], core->gpr[x][src2]);

        REGISTER_REGW_EVENT_L(core, result, dst, 3);
    }
    core->pc += 4;
    return 0;
}

static int32_t mpy_sp(int32_t op1, int32_t op2)
{
    int sign1, sign2, result_sign, rc;
    int exp1, exp2;
    float *src1 = (float *) &op2;
    float *src2 = (float *) &op1;

    float tmp;

    sign1 = extract_sign(op1);
    sign2 = extract_sign(op2);

    result_sign = sign1 ? !sign2 : sign2;

    rc = excp_chk_mpy_sp(op1, op2);

    if (rc == 1)
        return (result_sign ? 0xffffffff : 0x7fffffff);
    else if (rc == 2)
        return (result_sign ? 0xff800000 : 0x7f800000);

    exp1 = extract_exp_sp(op1);
    exp2 = extract_exp_sp(op2);

    if (!exp1 || !exp2)
        return (result_sign ? 0x80000000 : 0);  /* one operand is zero or denormal */

    tmp = (*src1) * (*src2);
    int32_t *result = (int32_t *) & tmp;

    return *result;
}

static int exec_cmpysp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op7_11;

        if (op == 0x1e)
        {
            int32_t op2 = core->gpr[s][src1];
            int32_t op3 = core->gpr[s][src1 + 1];
            int32_t op0 = core->gpr[x][src2];
            int32_t op1 = core->gpr[x][src2 + 1];

            int32_t dst_3 = mpy_sp(op3, op1);
            int32_t dst_2 = mpy_sp(op3, op0);
            int32_t dst_1 = mpy_sp(op2, op0) ^ 0x80000000;
            int32_t dst_0 = mpy_sp(op2, op1);

            REGISTER_REGW_EVENT_L(core, dst_0, dst, 3);
            REGISTER_REGW_EVENT_H(core, dst_1, dst + 1, 3);
            REGISTER_REGW_EVENT_L(core, dst_2, dst + 2, 3);
            REGISTER_REGW_EVENT_H(core, dst_3, dst + 3, 3);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_cmtl(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
    }
    NOT_IMP;
    core->pc += 4;
    return 0;
}

static int exec_dadd(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op2_11;
        uint64_t op_la, op_lb, op_ha, op_hb;

        switch (op)
        {
            case 0x218:
                op_la = src1;
                op_la = op_ha = EXTEND_SIGN(op_la, 5);
                break;
            case 0x116:
                op_la = src1;
                op_la = op_ha = EXTEND_SIGN(op_la, 5);
                break;
            case 0x078:
                op_la = (uint64_t) core->gpr[s][src1];
                op_ha = (uint64_t) core->gpr[s][src1 + 1];
                break;
            case 0x01e:
                op_la = (uint64_t) core->gpr[s][src1];
                op_ha = (uint64_t) core->gpr[s][src1 + 1];
                break;
            default:
                NOT_IMP_NO_NEED;
                break;
        }

        op_lb = (uint64_t) core->gpr[x][src2];
        op_hb = (uint64_t) core->gpr[x][src2 + 1];

        WRITE_BUFFER(core, dst, (int32_t) ((op_la + op_lb) & 0xFFFFFFFF));
        WRITE_BUFFER(core, dst + 1, (int32_t) ((op_ha + op_hb) & 0xFFFFFFFF));
    }

    core->pc += 4;
    return 0;
}

static int exec_dadd2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op2_11;

        int32_t dst_low;
        int32_t dst_upper;

        switch (op)
        {
            case 0x18:
                {
                    int32_t src1_o = core->gpr[s][src1 + 1];
                    int32_t src1_e = core->gpr[s][src1];
                    int32_t src2_o = core->gpr[x][src2 + 1];
                    int32_t src2_e = core->gpr[x][src2];

                    int32_t dst0_e = slow16(src1_e) + slow16(src2_e);
                    int32_t dst0_o = shigh16(src1_e) + shigh16(src2_e);
                    int32_t dst1_e = slow16(src1_o) + slow16(src2_o);
                    int32_t dst1_o = shigh16(src1_o) + shigh16(src2_o);

                    dst_upper = (dst1_o << 16) | dst1_e;
                    dst_low = (dst0_o << 16) | dst0_e;
                    break;
                }
            case 0x2e:
                {
                    int32_t src1_o = core->gpr[s][src1 + 1];
                    int32_t src1_e = core->gpr[s][src1];
                    int32_t src2_o = core->gpr[x][src2 + 1];
                    int32_t src2_e = core->gpr[x][src2];

                    int32_t dst0_e = slow16(src1_e) + slow16(src2_e);
                    int32_t dst0_o = shigh16(src1_e) + shigh16(src2_e);
                    int32_t dst1_e = slow16(src1_o) + slow16(src2_o);
                    int32_t dst1_o = shigh16(src1_o) + shigh16(src2_o);

                    dst_upper = (dst1_o << 16) | dst1_e;
                    dst_low = (dst0_o << 16) | dst0_e;
                    break;
                }
            default:
                NOT_IMP_NO_NEED;
                break;
        }

        WRITE_BUFFER(core, dst, (int32_t) dst_low);
        WRITE_BUFFER(core, dst + 1, (int32_t) dst_upper);
    }

    core->pc += 4;
    return 0;
}

static int exec_daddsp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op2_11;

        uint32_t dst_e;
        uint32_t dst_o;
        int32_t rmode1;
        int32_t rmode2;

        if (op == 0x1e6)
        {                               //Unit L
            fields->exec_unit = UNIT_L1 << s;
            rmode1 = fields->exec_unit == UNIT_L1 ? (((core->fadcr) >> 9) & 0x3) : (((core->fadcr) >> 25) & 0x3);
            rmode2 = fields->exec_unit == UNIT_L1 ? (((core->fmcr) >> 9) & 0x3) : (((core->fmcr) >> 25) & 0x3);
        } else if (op == 0x2c8)
        {                               //Unit S
            fields->exec_unit = UNIT_S1 << s;
            rmode1 = fields->exec_unit == UNIT_S1 ? (((core->fadcr) >> 9) & 0x3) : (((core->fadcr) >> 25) & 0x3);
            rmode2 = fields->exec_unit == UNIT_S1 ? (((core->fmcr) >> 9) & 0x3) : (((core->fmcr) >> 25) & 0x3);
        } else
        {
            NOT_IMP_NO_NEED;
        }

        dst_e = add_sp(core->gpr[s][src1], core->gpr[x][src2], rmode1, rmode2);
        dst_o = add_sp(core->gpr[s][src1 + 1], core->gpr[x][src2 + 1], rmode1, rmode2);

        REGISTER_REGW_EVENT_L(core, dst_e, dst, 2);
        REGISTER_REGW_EVENT_H(core, dst_o, dst + 1, 2);
    }

    core->pc += 4;
    return 0;
}

static int exec_dapys2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op5_11;

        int32_t dst_o = 0x0;
        int32_t dst_e = 0x0;

        if (op == 0x38)
        {

            int32_t src1_o = core->gpr[s][src1 + 1];
            int32_t src1_e = core->gpr[s][src1];
            int32_t src2_o = core->gpr[x][src2 + 1];
            int32_t src2_e = core->gpr[x][src2];

            if (src1_e & 0x000008000)
            {
                if ((src2_e & 0xFFFF) == 0x8000)
                {
                    dst_e = 0x7fff;
                } else
                {
                    dst_e = (-((int16_t) src2_e)) & 0xFFFF;
                }
            } else
            {
                dst_e = src2_e & 0xFFFF;
            }

            if (src1_e & 0x80000000)
            {
                if ((src2_e & 0xFFFF0000) == 0x80000000)
                {
                    dst_e |= 0x7fff0000;
                } else
                {
                    dst_e |= ((-((int16_t) src2_e)) & 0xFFFF) << 16;
                }
            } else
            {
                dst_e |= src2_e & 0xFFFF0000;
            }

            if (src1_o & 0x000008000)
            {
                if ((src2_o & 0xFFFF) == 0x8000)
                {
                    dst_o = 0x7fff;
                } else
                {
                    dst_o = (-((int16_t) src2_o)) & 0xFFFF;
                }
            } else
            {
                dst_o = src2_o & 0xFFFF;
            }

            if (src1_o & 0x80000000)
            {
                if ((src2_o & 0xFFFF0000) == 0x80000000)
                {
                    dst_o |= 0x7fff0000;
                } else
                {
                    dst_o |= ((-((int16_t) src2_o)) & 0xFFFF) << 16;
                }
            } else
            {
                dst_o |= src2_o & 0xFFFF0000;
            }
        } else
        {
            NOT_IMP_NO_NEED;
        }

        WRITE_BUFFER(core, dst, dst_e);
        WRITE_BUFFER(core, dst + 1, dst_o);
    }

    core->pc += 4;
    return 0;
}

static int32_t avg2(int32_t a, int32_t b)
{
    int32_t a_lo, a_hi, t0, t1;
    int32_t b_lo, b_hi;
    int32_t c_lo, c_hi;

    a_lo = a & 0xFFFF;
    a_lo |= a_lo & 0x00008000 ? 0xFFFF0000 : 0;
    b_lo = b & 0xFFFF;
    b_lo |= b_lo & 0x00008000 ? 0xFFFF0000 : 0;

    a_hi = (a >> 16) & 0xFFFF;
    a_hi |= a_hi & 0x00008000 ? 0xFFFF0000 : 0;
    b_hi = (b >> 16) & 0xFFFF;
    b_hi |= b_hi & 0x00008000 ? 0xFFFF0000 : 0;

    t0 = a_lo + b_lo + 1;
    t1 = a_hi + b_hi + 1;

    c_lo = 0x0000FFFF & (t0 >> 1);
    c_hi = 0xFFFF0000 & (t1 << 15);

    return c_hi | c_lo;
}

static int exec_davg2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op2_11;

        uint32_t dst_e;
        uint32_t dst_o;

        if (op == 0x13c)
        {

            uint32_t src1_o = core->gpr[s][src1 + 1];
            uint32_t src1_e = core->gpr[s][src1];
            uint32_t src2_o = core->gpr[x][src2 + 1];
            uint32_t src2_e = core->gpr[x][src2];

            dst_e = avg2(src1_e, src2_e);
            dst_o = avg2(src1_o, src2_o);

            REGISTER_REGW_EVENT_L(core, dst_e, dst, 1);
            REGISTER_REGW_EVENT_H(core, dst_o, dst + 1, 1);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int32_t avgnr2(int32_t a, int32_t b)
{
    int32_t a_lo, a_hi, t0, t1;
    int32_t b_lo, b_hi;
    int32_t c_lo, c_hi;

    a_lo = a & 0xFFFF;
    a_lo |= a_lo & 0x00008000 ? 0xFFFF0000 : 0;
    b_lo = b & 0xFFFF;
    b_lo |= b_lo & 0x00008000 ? 0xFFFF0000 : 0;

    a_hi = (a >> 16) & 0xFFFF;
    a_hi |= a_hi & 0x00008000 ? 0xFFFF0000 : 0;
    b_hi = (b >> 16) & 0xFFFF;
    b_hi |= b_hi & 0x00008000 ? 0xFFFF0000 : 0;

    t0 = a_lo + b_lo;
    t1 = a_hi + b_hi;

    c_lo = 0x0000FFFF & (t0 >> 1);
    c_hi = 0xFFFF0000 & (t1 << 15);

    return c_hi | c_lo;
}

static int exec_davgnr2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op2_11;

        uint32_t dst_e;
        uint32_t dst_o;

        if (op == 0x11c)
        {

            uint32_t src1_o = core->gpr[s][src1 + 1];
            uint32_t src1_e = core->gpr[s][src1];
            uint32_t src2_o = core->gpr[x][src2 + 1];
            uint32_t src2_e = core->gpr[x][src2];

            dst_e = avgnr2(src1_e, src2_e);
            dst_o = avgnr2(src1_o, src2_o);

            REGISTER_REGW_EVENT_L(core, dst_e, dst, 1);
            REGISTER_REGW_EVENT_H(core, dst_o, dst + 1, 1);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int32_t avgnru4(int32_t a, int32_t b)
{
    int32_t a0, a1, a2, a3, b0, b1, b2, b3, c0, c1, c2, c3;

    a0 = (a & 0xFF000000) >> 24;
    a1 = (a & 0x00FF0000) >> 16;
    a2 = (a & 0x0000FF00) >> 8;
    a3 = (a & 0x000000FF);

    b0 = (b & 0xFF000000) >> 24;
    b1 = (b & 0x00FF0000) >> 16;
    b2 = (b & 0x0000FF00) >> 8;
    b3 = (b & 0x000000FF);

    c0 = ((a0 + b0) << 23) & 0xFF000000;
    c1 = ((a1 + b1) << 15) & 0x00FF0000;
    c2 = ((a2 + b2) << 7) & 0x0000FF00;
    c3 = ((a3 + b3) >> 1) & 0x000000FF;

    return c0 | c1 | c2 | c3;
}

static int exec_davgnru4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op2_11;

        uint32_t dst_e;
        uint32_t dst_o;

        if (op == 0x10c)
        {

            uint32_t src1_o = core->gpr[s][src1 + 1];
            uint32_t src1_e = core->gpr[s][src1];
            uint32_t src2_o = core->gpr[x][src2 + 1];
            uint32_t src2_e = core->gpr[x][src2];

            dst_e = avgnru4(src1_e, src2_e);
            dst_o = avgnru4(src1_o, src2_o);

            REGISTER_REGW_EVENT_L(core, dst_e, dst, 1);
            REGISTER_REGW_EVENT_H(core, dst_o, dst + 1, 1);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int32_t avgu4(int32_t a, int32_t b)
{
    int32_t a0, a1, a2, a3, b0, b1, b2, b3, c0, c1, c2, c3;

    a0 = (a & 0xFF000000) >> 24;
    a1 = (a & 0x00FF0000) >> 16;
    a2 = (a & 0x0000FF00) >> 8;
    a3 = (a & 0x000000FF);

    b0 = (b & 0xFF000000) >> 24;
    b1 = (b & 0x00FF0000) >> 16;
    b2 = (b & 0x0000FF00) >> 8;
    b3 = (b & 0x000000FF);

    c0 = ((a0 + b0 + 1) << 23) & 0xFF000000;
    c1 = ((a1 + b1 + 1) << 15) & 0x00FF0000;
    c2 = ((a2 + b2 + 1) << 7) & 0x0000FF00;
    c3 = ((a3 + b3 + 1) >> 1) & 0x000000FF;

    return c0 | c1 | c2 | c3;
}

static int exec_davgu4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op2_11;

        uint32_t dst_e;
        uint32_t dst_o;

        if (op == 0x12c)
        {

            uint32_t src1_o = core->gpr[s][src1 + 1];
            uint32_t src1_e = core->gpr[s][src1];
            uint32_t src2_o = core->gpr[x][src2 + 1];
            uint32_t src2_e = core->gpr[x][src2];

            dst_e = avgu4(src1_e, src2_e);
            dst_o = avgu4(src1_o, src2_o);

            REGISTER_REGW_EVENT_L(core, dst_e, dst, 1);
            REGISTER_REGW_EVENT_H(core, dst_o, dst + 1, 1);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_dccmpy(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op2_11;

        if (op == 0x60)
        {
            uint32_t src1_o = core->gpr[s][src1 + 1];
            uint32_t src1_e = core->gpr[s][src1];
            uint32_t src2_o = core->gpr[x][src2 + 1];
            uint32_t src2_e = core->gpr[x][src2];

            uint32_t dst_3;
            uint32_t dst_2;
            uint32_t dst_1;
            uint32_t dst_0;

            if (src1_o == 0x80008000 && src1_e == 0x80008000 && src2_o == 0x80008000 && src2_e == 0x80008000)
            {
                dst_3 = 0x7fffffff;
                dst_1 = 0x7fffffff;
            } else
            {
                dst_3 = ((shigh16(src1_o) * shigh16(src2_o)) + (slow16(src1_o) * slow16(src2_o)));
                dst_1 = ((shigh16(src1_e) * shigh16(src2_e)) + (slow16(src1_e) * slow16(src2_e)));
            }
            dst_2 = (slow16(src1_o) * shigh16(src2_o)) - (shigh16(src1_o) * slow16(src2_o));
            dst_0 = (slow16(src1_e) * shigh16(src2_e)) - (shigh16(src1_e) * slow16(src2_e));

            REGISTER_REGW_EVENT_L(core, dst_0, dst, 3);
            REGISTER_REGW_EVENT_H(core, dst_1, dst + 1, 3);
            REGISTER_REGW_EVENT_H(core, dst_2, dst + 2, 3);
            REGISTER_REGW_EVENT_H(core, dst_3, dst + 3, 3);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int32_t ccmpyr1(int32_t a, int32_t b)
{
    int a_lo, a_hi, b_lo, b_hi;
    int sat;
    int64_t temp;
    int32_t result;
    int32_t result1;

    sat = 0;
    a_hi = shigh16(a);
    a_lo = slow16(a);
    b_hi = shigh16(b);
    b_lo = slow16(b);

    // lsb 16
    temp = (int64_t) (a_lo * b_hi) - (a_hi * b_lo) + ((int64_t) 0x4000);
    result = ((sat_jz(temp << 1)) >> 16) & 0xffff;

    // msb16
    temp = (int64_t) (a_hi * b_hi) + (a_lo * b_lo) + ((int64_t) 0x4000);

    result1 = sat_jz(temp << 1) & 0xffff0000;
    result |= result1;
    return result;
}

static int exec_dccmpyr1(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op2_11;

        if (op == 0xec)
        {
            uint32_t src1_o = core->gpr[s][src1 + 1];
            uint32_t src1_e = core->gpr[s][src1];
            uint32_t src2_o = core->gpr[x][src2 + 1];
            uint32_t src2_e = core->gpr[x][src2];

            uint32_t dst_e;
            uint32_t dst_o;

            dst_o = ccmpyr1(src1_o, src2_o);
            dst_e = ccmpyr1(src1_e, src2_e);

            WRITE_BUFFER(core, dst, dst_e);
            WRITE_BUFFER(core, dst + 1, dst_o);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_ddotp4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op2_11;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src1_o = core->gpr[x][src2];

        int32_t dst_e = (shigh16(src1_e) * shigh8(slow16(src1_o))) + (slow16(src1_e) * slow8(slow16(src1_o)));
        int32_t dst_o = (shigh16(src1_e) * shigh8(shigh16(src1_o))) + (slow16(src1_e) * slow8(shigh16(src1_o)));

        WRITE_BUFFER(core, dst, dst_e);
        WRITE_BUFFER(core, dst + 1, dst_o);
    }
    core->pc += 4;
    return 0;
}

static int32_t dotp4h(uint32_t dwop1_high, uint32_t dwop1_low, uint32_t dwop2_high, uint32_t dwop2_low)
{
    int64_t temp;
    int32_t mpy_res;

    temp = ((int64_t) slow16(dwop1_low)) * ((int64_t) slow16(dwop2_low)) +
        ((int64_t) shigh16(dwop1_low)) * ((int64_t) shigh16(dwop2_low)) +
        ((int64_t) slow16(dwop1_high)) * ((int64_t) slow16(dwop2_high)) +
        ((int64_t) shigh16(dwop1_high)) * ((int64_t) shigh16(dwop2_high));

    mpy_res = sat_jz(temp);

    return mpy_res;
}

static int exec_ddotp4h(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op2_11;

        if (op == 0x100)
        {
            int32_t src1_0 = core->gpr[s][src1];
            int32_t src1_1 = core->gpr[s][src1 + 1];
            int32_t src1_2 = core->gpr[s][src1 + 2];
            int32_t src1_3 = core->gpr[s][src1 + 3];

            int32_t src2_0 = core->gpr[x][src2];
            int32_t src2_1 = core->gpr[x][src2 + 1];
            int32_t src2_2 = core->gpr[x][src2 + 2];
            int32_t src2_3 = core->gpr[x][src2 + 3];

            int32_t dst_e = dotp4h(src1_1, src1_0, src2_1, src2_0);
            int32_t dst_o = dotp4h(src1_3, src1_2, src2_3, src2_2);

            REGISTER_REGW_EVENT_L(core, dst_e, dst, 3);
            REGISTER_REGW_EVENT_H(core, dst_o, dst + 1, 3);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_ddotph2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op2_11;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src1_o = core->gpr[s][src1 + 1];
        int32_t src2_op = core->gpr[x][src2];

        int64_t tmp1, tmp2;

        tmp1 = ((int64_t) shigh16(src1_o) * (int64_t) shigh16(src2_op)) +
            ((int64_t) slow16(src1_o) * (int64_t) slow16(src2_op));

        tmp2 = ((int64_t) slow16(src1_o) * (int64_t) shigh16(src2_op)) +
            ((int64_t) shigh16(src1_e) * (int64_t) slow16(src2_op));

        int32_t dst_e = sat_jz(tmp2);
        int32_t dst_o = sat_jz(tmp1);

        REGISTER_REGW_EVENT_L(core, dst_e, dst, 3);
        REGISTER_REGW_EVENT_H(core, dst_o, dst + 1, 3);
    }
    core->pc += 4;
    return 0;
}
static int exec_ddotph2r(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op2_11;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src1_o = core->gpr[s][src1 + 1];
        int32_t src2_op = core->gpr[x][src2];

        int64_t tmp1, tmp2;

        tmp1 = ((int64_t) shigh16(src1_o) * (int64_t) shigh16(src2_op)) +
            ((int64_t) slow16(src1_o) * (int64_t) slow16(src2_op) + 0x00008000);

        tmp2 = ((int64_t) slow16(src1_o) * (int64_t) shigh16(src2_op)) +
            ((int64_t) shigh16(src1_e) * (int64_t) slow16(src2_op) + 0x00008000);

        int32_t dst_e = sat_jz(tmp2);
        int32_t dst_o = sat_jz(tmp1);

        int32_t result = (dst_o & 0xFFFF0000) | (dst_e >> 16 & 0x0000FFFF);

        REGISTER_REGW_EVENT_L(core, result, dst, 3);
    }
    core->pc += 4;
    return 0;
}
static int exec_ddotpl2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src1_o = core->gpr[s][src1 + 1];
        int32_t src2_e = core->gpr[x][src2];

        int32_t dst_e =
            sat_jz((int64_t) shigh16(src1_e) * (int64_t) shigh16(src2_e) +
                   (int64_t) slow16(src1_e) * (int64_t) slow16(src2_e));
        int32_t dst_o =
            sat_jz((int64_t) slow16(src1_o) * (int64_t) shigh16(src2_e) +
                   (int64_t) shigh16(src1_e) * (int64_t) slow16(src2_e));

        REGISTER_REGW_EVENT_L(core, dst_e, dst, 3);
        REGISTER_REGW_EVENT_H(core, dst_o, dst + 1, 3);
    }
    core->pc += 4;
    return 0;
}
static int exec_ddotpl2r(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src1_o = core->gpr[s][src1 + 1];
        int32_t src2_e = core->gpr[x][src2];

        int32_t lower_half = sat_jz((int64_t) shigh16(src1_e) * (int64_t) shigh16(src2_e) +
                                    (int64_t) slow16(src1_e) * (int64_t) slow16(src2_e) + 0x8000);
        int32_t upper_half = sat_jz((int64_t) slow16(src1_o) * (int64_t) shigh16(src2_e) +
                                    (int64_t) shigh16(src1_e) * (int64_t) slow16(src2_e) + 0x8000);

        int32_t result = (shigh16(upper_half) << 16) | shigh16(lower_half);

        REGISTER_REGW_EVENT_L(core, result, dst, 3);
    }
    core->pc += 4;
    return 0;
}

static int32_t dotpsu4h(int32_t dwop1_high, int32_t dwop1_low, int32_t dwop2_high, int32_t dwop2_low)
{
    int64_t temp;
    int32_t mpy_res;
    int sat;

    temp = ((int64_t) slow16(dwop1_low)) * ((int64_t) ulow16(dwop2_low)) +
        ((int64_t) shigh16(dwop1_low)) * ((int64_t) uhigh16(dwop2_low)) +
        ((int64_t) slow16(dwop1_high)) * ((int64_t) ulow16(dwop2_high)) +
        ((int64_t) shigh16(dwop1_high)) * ((int64_t) uhigh16(dwop2_high));

    mpy_res = sat_jz(temp);
    return mpy_res;
}

static int exec_ddotpsu4h(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op6_11;

        if (op == 0x12)
        {
            int32_t src1_0 = core->gpr[s][src1];
            int32_t src1_1 = core->gpr[s][src1 + 1];
            int32_t src1_2 = core->gpr[s][src1 + 2];
            int32_t src1_3 = core->gpr[s][src1 + 3];
            int32_t src2_0 = core->gpr[x][src2];
            int32_t src2_1 = core->gpr[x][src2 + 1];
            int32_t src2_2 = core->gpr[x][src2 + 2];
            int32_t src2_3 = core->gpr[x][src2 + 3];

            int32_t dst_e = dotpsu4h(src1_1, src1_0, src2_1, src2_0);
            int32_t dst_o = dotpsu4h(src1_3, src1_2, src2_3, src2_2);

            REGISTER_REGW_EVENT_L(core, dst_e, dst, 3);
            REGISTER_REGW_EVENT_H(core, dst_o, dst + 1, 3);
        } else
        {
            NOT_IMP_NO_NEED;
        }

    }
    core->pc += 4;
    return 0;
}
static int exec_deal(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src2_e = core->gpr[x][src2];

#define DBIT(n) ((src2_e >> (n)) & 0x1)
        int i;
        int32_t result = 0x0;
        int r = 0;

        for (i = 31; i >= 0; i--)
        {
            if (i % 2 == 1)
            {
                //upper
                result |= (DBIT(i) << (31 - r / 2));
            } else
            {
                //lower
                result |= (DBIT(i) << (15 - r / 2));
            }
            r++;
        }
#undef DBIT

        REGISTER_REGW_EVENT_L(core, result, dst, 1);
    }
    core->pc += 4;
    return 0;
}
static int exec_dint(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    TSR_SET_SGIE(core, TSR_GET_GIE(core));
    CSR_SET_GIE(core, 0);
    TSR_SET_GIE(core, 0);

    core->pc += 4;
    return 0;
}

static int32_t intsp(int32_t op)
{
    uint32_t man_high, man_low;
    int32_t sign;
    int32_t exp;
    int i;

    if (!op)
        return (0);

    if (op & 0x80000000)
    {
        sign = 1;
        op = -op;
    } else
        sign = 0;

    man_high = op >> 8;
    man_low = op << 24;
    exp = 158;

    for (i = 0; i < 56; i++)
    {
        if (man_high & 0x00800000)
            break;
        man_high = (man_high << 1) | (((uint32_t) man_low) >> (31));
        man_low <<= 1;
        exp--;
    }

    return ((sign << 31) | (exp << 23) | (man_high & 0x007fffff));
}

static int32_t intspu(int32_t op)
{
    uint32_t man_high, man_low;
    int32_t exp;
    int32_t round_mode;
    int i;

    if (!op)
        return (0);
    man_high = op >> 8;
    man_low = op << 24;
    exp = 158;

    for (i = 0; i < 56; i++)
    {
        if (man_high & 0x00800000)
            break;
        man_high = (man_high << 1) | (((uint32_t) man_low) >> (31));
        man_low <<= 1;
        exp--;
    }

    return ((exp << 23) | (man_high & 0x007fffff));
}

static int exec_dinthsp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = BITS(13, 17);
        int32_t dst_e;
        int32_t dst_o;

        switch (op)
        {
            case 0x12:
                dst_e = intsp(slow16(core->gpr[x][src2]));
                dst_o = intsp(shigh16(core->gpr[x][src2]));
                break;
            case 0x17:                 //dintsp  Unit L
                dst_e = intsp(core->gpr[x][src2 + 1]);
                dst_o = intsp(core->gpr[x][src2]);
                break;
            case 0x15:                 //dintsp  Unit L
                dst_e = intsp(core->gpr[x][src2 + 1]);
                dst_o = intsp(core->gpr[x][src2]);
                break;
            case 0x13:
                dst_e = intsp(ulow16(core->gpr[x][src2]));
                dst_o = intsp(uhigh16(core->gpr[x][src2]));
                break;
            case 0x16:
                NOT_IMP;
                break;
            case 0x11:
                dst_e = intspu(core->gpr[x][src2]);
                dst_o = intspu(core->gpr[x][src2 + 1]);
                break;
            default:
                DBG("In %s, op=0x%x, insn=0x%x\n", __FUNCTION__, op, insn);
                NOT_IMP_NO_NEED;
        }
        REGISTER_REGW_EVENT_L(core, dst_e, dst, 2);
        REGISTER_REGW_EVENT_H(core, dst_o, dst + 1, 2);
    }
    core->pc += 4;
    return 0;
}

static int32_t max2(int32_t a, int32_t b)
{
    int32_t alo, blo, c;

    c = 0;
    alo = 0xffff & a;
    if (alo & 0x8000)
        alo |= 0xffff0000;
    blo = 0xffff & b;
    if (blo & 0x8000)
        blo |= 0xffff0000;
    if (alo > blo)
        c |= a & 0xffff;
    else
        c |= b & 0xffff;

    alo = 0xffff & (a >> 16);
    if (alo & 0x8000)
        alo |= 0xffff0000;
    blo = 0xffff & (b >> 16);
    if (blo & 0x8000)
        blo |= 0xffff0000;
    if (alo > blo)
        c |= (a & 0xffff0000);
    else
        c |= (b & 0xffff0000);
    return (c);
}

static int exec_dmax2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op5_11;

        if (op == 0x42)
        {
            int32_t dst_e = max2(core->gpr[s][src1], core->gpr[x][src2]);
            int32_t dst_o = max2(core->gpr[s][src1 + 1], core->gpr[x][src2 + 1]);

            WRITE_BUFFER(core, dst, dst_e);
            WRITE_BUFFER(core, dst + 1, dst_o);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int32_t max4(int32_t a, int32_t b)
{
    int32_t alo, blo, c;

    c = 0;
    alo = 0xff & a;
    blo = 0xff & b;
    if (alo > blo)
        c |= alo;
    else
        c |= blo;
    a >>= 8;
    b >>= 8;
    alo = 0xff & a;
    blo = 0xff & b;
    if (alo > blo)
        c |= alo << 8;
    else
        c |= blo << 8;
    a >>= 8;
    b >>= 8;
    alo = 0xff & a;
    blo = 0xff & b;
    if (alo > blo)
        c |= alo << 16;
    else
        c |= blo << 16;
    a >>= 8;
    b >>= 8;
    alo = 0xff & a;
    blo = 0xff & b;
    if (alo > blo)
        c |= alo << 24;
    else
        c |= blo << 24;
    return (c);
}

static int exec_dmaxu4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op5_11;

        if (op == 0x43)
        {
            int32_t dst_e = max4(core->gpr[s][src1], core->gpr[x][src2]);
            int32_t dst_o = max4(core->gpr[s][src1 + 1], core->gpr[x][src2 + 1]);

            WRITE_BUFFER(core, dst, dst_e);
            WRITE_BUFFER(core, dst + 1, dst_o);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int32_t min2(int32_t a, int32_t b)
{
    int32_t alo, blo, c;

    c = 0;
    alo = 0xffff & a;
    if (alo & 0x8000)
        alo |= 0xffff0000;
    blo = 0xffff & b;
    if (blo & 0x8000)
        blo |= 0xffff0000;
    if (alo < blo)
        c |= a & 0xffff;
    else
        c |= b & 0xffff;

    alo = 0xffff & (a >> 16);
    if (alo & 0x8000)
        alo |= 0xffff0000;
    blo = 0xffff & (b >> 16);
    if (blo & 0x8000)
        blo |= 0xffff0000;
    if (alo < blo)
        c |= (a & 0xffff0000);
    else
        c |= (b & 0xffff0000);
    return (c);
}

static int exec_dmin2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op5_11;

        if (op == 0x41)
        {
            int32_t dst_e = min2(core->gpr[s][src1], core->gpr[x][src2]);
            int32_t dst_o = min2(core->gpr[s][src1 + 1], core->gpr[x][src2 + 1]);

            WRITE_BUFFER(core, dst, dst_e);
            WRITE_BUFFER(core, dst + 1, dst_o);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int32_t min4(int32_t a, int32_t b)
{
    uint32_t alo, blo, c;

    c = 0;
    alo = 0xff & a;
    blo = 0xff & b;
    if (alo < blo)
        c |= alo;
    else
        c |= blo;
    a >>= 8;
    b >>= 8;
    alo = 0xff & a;
    blo = 0xff & b;
    if (alo < blo)
        c |= alo << 8;
    else
        c |= blo << 8;
    a >>= 8;
    b >>= 8;
    alo = 0xff & a;
    blo = 0xff & b;
    if (alo < blo)
        c |= alo << 16;
    else
        c |= blo << 16;
    a >>= 8;
    b >>= 8;
    alo = 0xff & a;
    blo = 0xff & b;
    if (alo < blo)
        c |= alo << 24;
    else
        c |= blo << 24;
    return (c);
}

static int exec_dminu4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op5_11;

        if (op == 0x48)
        {
            int32_t dst_e = min4(core->gpr[s][src1], core->gpr[x][src2]);
            int32_t dst_o = min4(core->gpr[s][src1 + 1], core->gpr[x][src2 + 1]);

            WRITE_BUFFER(core, dst, dst_e);
            WRITE_BUFFER(core, dst + 1, dst_o);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_dmpy2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op6_11;

        if (op == 0x0)
        {
            int src1_e = core->gpr[s][src1];
            int src1_o = core->gpr[s][src1 + 1];
            int src2_e = core->gpr[x][src2];
            int src2_o = core->gpr[x][src2 + 1];

            int32_t dst_0 = slow16(src1_e) * slow16(src2_e);
            int32_t dst_1 = shigh16(src1_e) * shigh16(src2_e);
            int32_t dst_2 = slow16(src1_o) * slow16(src2_o);
            int32_t dst_3 = shigh16(src1_o) * shigh16(src2_o);

            REGISTER_REGW_EVENT_L(core, dst_0, dst, 3);
            REGISTER_REGW_EVENT_H(core, dst_1, dst + 1, 3);
            REGISTER_REGW_EVENT_H(core, dst_2, dst + 2, 3);
            REGISTER_REGW_EVENT_H(core, dst_3, dst + 3, 3);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_dmpysp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op7_11;

        if (op == 0x1c)
        {
            int32_t op2 = core->gpr[s][src1];
            int32_t op3 = core->gpr[s][src1 + 1];
            int32_t op0 = core->gpr[x][src2];
            int32_t op1 = core->gpr[x][src2 + 1];

            int32_t dst_e = mpy_sp(op2, op0);
            int32_t dst_o = mpy_sp(op3, op1);

            REGISTER_REGW_EVENT_L(core, dst_e, dst, 3);
            REGISTER_REGW_EVENT_H(core, dst_o, dst + 1, 3);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_dmpysu4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op6_11;

        if (op == 0x30)
        {
            int32_t src1_e = core->gpr[s][src1];
            int32_t src1_o = core->gpr[s][src1 + 1];
            int32_t src2_e = core->gpr[x][src2];
            int32_t src2_o = core->gpr[x][src2 + 1];

            int32_t dst_0 = (sbyte0(src1_e) * ubyte0(src2_e)) & 0xffff | ((sbyte1(src1_e) * ubyte1(src2_e)) << 16);     // 3 delay slot
            int32_t dst_1 = (sbyte2(src1_e) * ubyte2(src2_e)) & 0xffff | ((sbyte3(src1_e) * ubyte3(src2_e)) << 16);     // 3 delay slot
            int32_t dst_2 = (sbyte0(src1_o) * ubyte0(src2_o)) & 0xffff | ((sbyte1(src1_o) * ubyte1(src2_o)) << 16);     // 3 delay slot
            int32_t dst_3 = (sbyte2(src1_o) * ubyte2(src2_o)) & 0xffff | ((sbyte3(src1_o) * ubyte3(src2_o)) << 16);     // 3 delay slot

            REGISTER_REGW_EVENT_L(core, dst_0, dst, 3);
            REGISTER_REGW_EVENT_H(core, dst_1, dst + 1, 3);
            REGISTER_REGW_EVENT_H(core, dst_2, dst + 2, 3);
            REGISTER_REGW_EVENT_H(core, dst_3, dst + 3, 3);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_dmpyu2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op7_11;

        if (op == 0x1)
        {
            int32_t src1_e = core->gpr[s][src1];
            int32_t src1_o = core->gpr[s][src1 + 1];
            int32_t src2_e = core->gpr[x][src2];
            int32_t src2_o = core->gpr[x][src2 + 1];

            int32_t dst_0 = ulow16(src1_e) * ulow16(src2_e);    // 3 delay slot
            int32_t dst_1 = uhigh16(src1_e) * uhigh16(src2_e);  // 3 delay slot
            int32_t dst_2 = ulow16(src1_o) * ulow16(src2_o);    // 3 delay slot
            int32_t dst_3 = uhigh16(src1_o) * uhigh16(src2_o);  // 3 delay slot

            REGISTER_REGW_EVENT_L(core, dst_0, dst, 3);
            REGISTER_REGW_EVENT_H(core, dst_1, dst + 1, 3);
            REGISTER_REGW_EVENT_H(core, dst_2, dst + 2, 3);
            REGISTER_REGW_EVENT_H(core, dst_3, dst + 3, 3);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_dmpyu4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op6_11;

        if (op == 0x4)
        {
            int32_t src1_e = core->gpr[s][src1];
            int32_t src1_o = core->gpr[s][src1 + 1];
            int32_t src2_e = core->gpr[x][src2];
            int32_t src2_o = core->gpr[x][src2 + 1];

            int32_t dst_0 = (ubyte0(src1_e) * ubyte0(src2_e)) & 0xffff | ((ubyte1(src1_e) * ubyte1(src2_e)) << 16);     // 3 delay slot
            int32_t dst_1 = (ubyte2(src1_e) * ubyte2(src2_e)) & 0xffff | ((ubyte3(src1_e) * ubyte3(src2_e)) << 16);     // 3 delay slot
            int32_t dst_2 = (ubyte0(src1_o) * ubyte0(src2_o)) & 0xffff | ((ubyte1(src1_o) * ubyte1(src2_o)) << 16);     // 3 delay slot
            int32_t dst_3 = (ubyte2(src1_o) * ubyte2(src2_o)) & 0xffff | ((ubyte3(src1_o) * ubyte3(src2_o)) << 16);     // 3 delay slot

            REGISTER_REGW_EVENT_L(core, dst_0, dst, 3);
            REGISTER_REGW_EVENT_H(core, dst_1, dst + 1, 3);
            REGISTER_REGW_EVENT_H(core, dst_2, dst + 2, 3);
            REGISTER_REGW_EVENT_H(core, dst_3, dst + 3, 3);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_dmv(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        WRITE_BUFFER(core, dst, core->gpr[x][src2]);
        WRITE_BUFFER(core, dst + 1, core->gpr[s][src1]);
    }
    core->pc += 4;
    return 0;
}

static int exec_dmvd(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        REGISTER_REGW_EVENT_L(core, core->gpr[s][src2], dst, 3);
        REGISTER_REGW_EVENT_H(core, core->gpr[x][src1], dst + 1, 3);
    }
    core->pc += 4;
    return 0;
}

static int exec_dotp2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = BITS(6, 10);

        int32_t op1 = core->gpr[s][src1];
        int32_t op2 = core->gpr[x][src2];

        int64_t result =
            ((int64_t) slow16(op1) * (int64_t) slow16(op2)) + ((int64_t) shigh16(op1) * (int64_t) shigh16(op2));

        if (op == 0xc)
        {
            REGISTER_REGW_EVENT_L(core, (int32_t) (result & 0xFFFFFFFF), dst, 3);
        } else if (op == 0xb)
        {
            REGISTER_REGW_EVENT_L(core, (int32_t) (result & 0xFFFFFFFF), dst, 3);
            REGISTER_REGW_EVENT_H(core, (int32_t) (result >> 32), dst + 1, 3);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_dotp4h(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_0 = core->gpr[s][src1];
        int32_t src1_1 = core->gpr[s][src1 + 1];

        int32_t src2_0 = core->gpr[x][src2];
        int32_t src2_1 = core->gpr[x][src2 + 1];

        uint32_t result = dotp4h(src1_1, src1_0, src2_1, src2_0);

        REGISTER_REGW_EVENT_L(core, result, dst, 3);
    }
    core->pc += 4;
    return 0;
}

static int exec_dotpn2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t op1 = core->gpr[s][src1];

        int32_t op2 = core->gpr[x][src2];

        uint32_t result = shigh16(op1) * shigh16(op2) - slow16(op1) * slow16(op2);

        REGISTER_REGW_EVENT_L(core, result, dst, 3);
    }
    core->pc += 4;
    return 0;
}
static int exec_dotpnrsu2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t op1 = core->gpr[s][src1];
        int32_t op2 = core->gpr[x][src2];

        uint32_t result = (shigh16(op1) * uhigh16(op2) - slow16(op1) * ulow16(op2) + 0x8000) >> 16;

        REGISTER_REGW_EVENT_L(core, result, dst, 3);
    }
    core->pc += 4;
    return 0;
}
static int exec_dotpnrus2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        // dotpnrsu2 = dotpnrus2;
    }
    NOT_IMP_NO_NEED;
    core->pc += 4;
    return 0;
}
static int exec_dotprsu2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t op1 = core->gpr[s][src1];
        int32_t op2 = core->gpr[x][src2];

        int64_t int33 = shigh16(op1) * uhigh16(op2) + slow16(op1) * ulow16(op2) + 0x8000;
        int32_t result = int33 >> 16;

        REGISTER_REGW_EVENT_L(core, result, dst, 3);
    }
    core->pc += 4;
    return 0;
}
static int exec_dotprus2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        // dotprus2 = dotprsu2;
    }
    NOT_IMP_NO_NEED;
    core->pc += 4;
    return 0;
}
static int exec_dotpsu4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t op1 = core->gpr[s][src1];
        int32_t op2 = core->gpr[x][src2];

        int32_t result = sbyte0(op1) * ubyte0(op2) +
            sbyte1(op1) * ubyte1(op2) + sbyte2(op1) * ubyte2(op2) + sbyte3(op1) * ubyte3(op2);

        REGISTER_REGW_EVENT_L(core, result, dst, 3);
    }
    core->pc += 4;
    return 0;
}

static int exec_dotpsu4h(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int op = fields->op6_11;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src1_o = core->gpr[s][src1 + 1];
        int32_t src2_e = core->gpr[x][src2];
        int32_t src2_o = core->gpr[x][src2 + 1];

        uint64_t temp;
        uint32_t mpy_res;

        temp = ((int64_t) slow16(src1_e)) * ((int64_t) ulow16(src2_e)) +
            ((int64_t) shigh16(src1_e)) * ((int64_t) uhigh16(src2_e)) +
            ((int64_t) slow16(src1_o)) * ((int64_t) ulow16(src2_o)) +
            ((int64_t) shigh16(src1_o)) * ((int64_t) uhigh16(src2_o));

        mpy_res = sat_jz(temp);

        int32_t dst_o = temp >> 32;
        int32_t dst_e = temp & 0xFFFFFFFF;

        if (op == 0x2)
        {
            REGISTER_REGW_EVENT_L(core, dst_e, dst, 3);
            REGISTER_REGW_EVENT_H(core, dst_o, dst + 1, 3);
        } else if (op == 0x5)
        {
            REGISTER_REGW_EVENT_L(core, mpy_res, dst, 3);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_dotpus4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{

    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t op1 = core->gpr[s][src1];
        int32_t op2 = core->gpr[x][src2];

        int32_t result = ubyte0(op2) * sbyte0(op1) +
            ubyte1(op2) * sbyte1(op1) + ubyte2(op2) * sbyte2(op1) + ubyte3(op2) * sbyte3(op1);

        REGISTER_REGW_EVENT_L(core, result, dst, 3);
    }
    core->pc += 4;
    return 0;

}
static int exec_dotpu4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int op = fields->op6_11;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];

        int32_t dst_e = ubyte0(src1_e) * ubyte0(src2_e) +
            ubyte1(src1_e) * ubyte1(src2_e) + ubyte2(src1_e) * ubyte2(src2_e) + ubyte3(src1_e) * ubyte3(src2_e);

        REGISTER_REGW_EVENT_L(core, dst_e, dst, 3);
    }
    core->pc += 4;
    return 0;
}
static int exec_dpack2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t op1 = core->gpr[s][src1];
        int32_t op2 = core->gpr[x][src2];

        int32_t dst_e = (slow16(op1) << 16) | (slow16(op2) & 0xFFFF);
        int32_t dst_o = (shigh16(op1) << 16) | (shigh16(op2) & 0xFFFF);

        WRITE_BUFFER(core, dst, dst_e);
        WRITE_BUFFER(core, dst + 1, dst_o);
    }
    core->pc += 4;
    return 0;
}
static int exec_dpackh2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src1_o = core->gpr[s][src1 + 1];
        int32_t src2_e = core->gpr[x][src2];
        int32_t src2_o = core->gpr[x][src2 + 1];

        int32_t dst_e = (shigh16(src1_e) << 16) | (shigh16(src2_e) & 0xFFFF);
        int32_t dst_o = (shigh16(src1_o) << 16) | (shigh16(src2_o) & 0xFFFF);

        WRITE_BUFFER(core, dst, dst_e);
        WRITE_BUFFER(core, dst + 1, dst_o);
    }
    core->pc += 4;
    return 0;
}

static int exec_dpackh4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op5_11;

        if (op == 0x69)
        {
            int32_t src1_e = core->gpr[s][src1];
            int32_t src1_o = core->gpr[s][src1 + 1];
            int32_t src2_e = core->gpr[x][src2];
            int32_t src2_o = core->gpr[x][src2 + 1];

            int32_t dst_e = ((sbyte3(src1_e) & 0xFF) << 24) |
                ((sbyte1(src1_e) & 0xFF) << 16) | ((sbyte3(src2_e) & 0xFF) << 8) | ((sbyte1(src2_e) & 0xFF));

            int32_t dst_o = ((sbyte3(src1_o) & 0xFF) << 24) |
                ((sbyte1(src1_o) & 0xFF) << 16) | ((sbyte3(src2_o) & 0xFF) << 8) | ((sbyte1(src2_o) & 0xFF));

            WRITE_BUFFER(core, dst, dst_e);
            WRITE_BUFFER(core, dst + 1, dst_o);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_dpackhl2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op6_11;

        if ((op == 0xe) || (op == 0x8))
        {
            int32_t src1_e = core->gpr[s][src1];
            int32_t src1_o = core->gpr[s][src1 + 1];
            int32_t src2_e = core->gpr[x][src2];
            int32_t src2_o = core->gpr[x][src2 + 1];

            int32_t dst_o = (src1_o) & 0xffff0000 | (src2_o) & 0x0000ffff;
            int32_t dst_e = (src1_e) & 0xffff0000 | (src2_e) & 0x0000ffff;

            WRITE_BUFFER(core, dst, dst_e);
            WRITE_BUFFER(core, dst + 1, dst_o);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_dpackl2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src1_o = core->gpr[s][src1 + 1];
        int32_t src2_e = core->gpr[x][src2];
        int32_t src2_o = core->gpr[x][src2 + 1];

        int32_t dst_o = (src1_o << 16) & 0xffff0000 | (src2_o) & 0x0000ffff;
        int32_t dst_e = (src1_e << 16) & 0xffff0000 | (src2_e) & 0x0000ffff;

        WRITE_BUFFER(core, dst, dst_e);
        WRITE_BUFFER(core, dst + 1, dst_o);
    }
    core->pc += 4;
    return 0;
}

static int exec_dpackl4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op5_11;

        if (op == 0x68)
        {
            int32_t src1_e = core->gpr[s][src1];
            int32_t src1_o = core->gpr[s][src1 + 1];
            int32_t src2_e = core->gpr[x][src2];
            int32_t src2_o = core->gpr[x][src2 + 1];

            int32_t dst_e = ((sbyte2(src1_e) & 0xFF) << 24) |
                ((sbyte0(src1_e) & 0xFF) << 16) | ((sbyte2(src2_e) & 0xFF) << 8) | ((sbyte0(src2_e) & 0xFF));

            int32_t dst_o = ((sbyte2(src1_o) & 0xFF) << 24) |
                ((sbyte0(src1_o) & 0xFF) << 16) | ((sbyte2(src2_o) & 0xFF) << 8) | ((sbyte0(src2_o) & 0xFF));

            WRITE_BUFFER(core, dst, dst_e);
            WRITE_BUFFER(core, dst + 1, dst_o);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_dpacklh2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src1_o = core->gpr[s][src1 + 1];
        int32_t src2_e = core->gpr[x][src2];
        int32_t src2_o = core->gpr[x][src2 + 1];

        int32_t dst_e = (src1_e << 16) & 0xffff0000 | (src2_e >> 16) & 0x0000ffff;
        int32_t dst_o = (src1_o << 16) & 0xffff0000 | (src2_o >> 16) & 0x0000ffff;

        WRITE_BUFFER(core, dst, dst_e);
        WRITE_BUFFER(core, dst + 1, dst_o);
    }
    core->pc += 4;
    return 0;
}

static int exec_dpacklh4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op5_11;

        if (op == 0x6a)
        {
            int32_t src1_e = core->gpr[s][src1];
            int32_t src2_e = core->gpr[x][src2];

            int32_t dst_e = ((sbyte2(src1_e) & 0xFF) << 24) |
                ((sbyte0(src1_e) & 0xFF) << 16) | ((sbyte2(src2_e) & 0xFF) << 8) | ((sbyte0(src2_e) & 0xFF));

            int32_t dst_o = ((sbyte3(src1_e) & 0xFF) << 24) |
                ((sbyte1(src1_e) & 0xFF) << 16) | ((sbyte3(src2_e) & 0xFF) << 8) | ((sbyte1(src2_e) & 0xFF));

            WRITE_BUFFER(core, dst, dst_e);
            WRITE_BUFFER(core, dst + 1, dst_o);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_dpackx2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];

        int32_t dst_e = ((src1_e & 0x0000ffff) << 16) | (src2_e >> 16) & 0x0000ffff;
        int32_t dst_o = ((src2_e & 0x0000ffff) << 16) | (src1_e >> 16) & 0x0000ffff;

        WRITE_BUFFER(core, dst, dst_e);
        WRITE_BUFFER(core, dst + 1, dst_o);
    }
    core->pc += 4;
    return 0;
}

static int doubleToInt(double f)
{
    int i = 0;

    if (f > 0)                          //正数  
        i = (f * 10 + 5) / 10;
    else if (f < 0)                     //负数  
        i = (f * 10 - 5) / 10;
    else
        i = 0;

    return i;
}

static int exec_dpint(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int result;

        GET_DW_DOUBLE(core->gpr[s][src2 - 1], core->gpr[s][src2], dp);
        result = doubleToInt(dp);
        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, 3);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}

static int exec_dpsp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_L1 << fields->s;
    if (calc_cond(core, insn))
    {
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int reg_group = fields->x ? (!s) & 0x1 : s;
        float sp;
        uint32_t *spi;

        // manual is not consistent with ccs compiler
        GET_DW_DOUBLE(core->gpr[s][src2 - 1], core->gpr[s][src2], dp);
        sp = (float) dp;
        spi = (uint32_t *) & sp;
        REGISTER_REGW_EVENT_L(core, *spi, s * GPR_NUM_IN_GROUP + dst, 3);
    }
    core->pc += 4;
    return 0;
}

static int exec_dptrunc(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int reg_group = fields->x ? (!s) & 0x1 : s;
        int32_t result;

        GET_DW_DOUBLE(core->gpr[s][src2 - 1], core->gpr[s][src2], dp);
        result = (int32_t) dp;
        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, DPTRUNC_DELAY_SLOT);
    }
    core->pc += 4;
    return 0;
}
static int exec_sptrunc(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int reg_group = fields->x ? (!s) & 0x1 : s;
        int32_t result;
        float dp = *(float *) &core->gpr[reg_group][src2];

        result = (int32_t) dp;
        /*
         * int tmp = 1 << 31;
         * if (result > (tmp -1) || result < (-tmp)){
         * if (core->gpr[reg_group][src2] & (0x1 << 31))
         * result = 0x80000000;
         * else 
         * result = 0x7FFFFFFF;
         * 
         * }
         */
        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, DPTRUNC_DELAY_SLOT);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}

static int32_t sadd(int32_t a, int32_t b)
{
    int32_t c;

    c = a + b;
    if ((a < 0) && (b < 0) && (c >= 0))
    {
        c = 0x80000000;
    } else if ((a >= 0) && (b >= 0) && (c < 0))
    {
        c = 0x7fffffff;
    }
    return c;
}

static int exec_dsadd(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src1_o = core->gpr[s][src1 + 1];
        int32_t src2_e = core->gpr[x][src2];
        int32_t src2_o = core->gpr[x][src2 + 1];

        int32_t dst_o = sadd(src1_o, src2_o);
        int32_t dst_e = sadd(src1_e, src2_e);

        WRITE_BUFFER(core, dst, dst_e);
        WRITE_BUFFER(core, dst + 1, dst_o);
    }
    core->pc += 4;
    return 0;
}

static int32_t sadd2(int32_t a, int32_t b)
{
    int32_t d_low, d_high, a_low, a_high, b_low, b_high;

    a_low = a & 0xffff;
    b_low = b & 0xffff;
    if (a_low & 0x8000)
        a_low = a_low | 0xffff0000;
    if (b_low & 0x8000)
        b_low = b_low | 0xffff0000;
    d_low = a_low + b_low;              /* Add the low 16-bits */
    if (d_low > (int) 0x7fff)
        d_low = 0x7fff;
    else if (d_low < (int) 0xffff8000)
        d_low = 0x8000;

    d_low &= 0xFFFF;                    /* mask off upper bits */

    a_high = a >> 16;
    b_high = b >> 16;
    if (a_high & 0x8000)
        a_high = a_high | 0xffff0000;
    if (b_high & 0x8000)
        b_high = b_high | 0xffff0000;
    d_high = a_high + b_high;           /* Add the high 16-bits */
    if (d_high > (int) 0x7fff)
        d_high = 0x7fff;
    else if (d_high < (int) 0xffff8000)
        d_high = 0x8000;

    d_high &= 0xffff;                   /* mask off upper bits */

    return (d_high << 16) | d_low;
}

static int exec_dsadd2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src1_o = core->gpr[s][src1 + 1];
        int32_t src2_e = core->gpr[x][src2];
        int32_t src2_o = core->gpr[x][src2 + 1];

        int32_t dst_o = sadd2(src1_o, src2_o);
        int32_t dst_e = sadd2(src1_e, src2_e);

        WRITE_BUFFER(core, dst, dst_e);
        WRITE_BUFFER(core, dst + 1, dst_o);
    }
    core->pc += 4;
    return 0;
}

static int32_t shl(int32_t a, int32_t l)
{
    if (l & 0x20)
    {                                   /* If bit 5 is set, then return all zeros */
        return 0;
    } else
    {
        return a << (l & 0x1f);
    }
}

static int exec_dshl(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int ucst5 = fields->src13_17;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op6_11;

        int32_t src1_op = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];
        int32_t src2_o = core->gpr[x][src2 + 1];
        int32_t shift_count = 0;

        if (op == 0x32)
        {
            shift_count = ucst5;
        } else if (op == 0x33)
        {
            shift_count = src1_op;
        } else
        {
            NOT_IMP_NO_NEED;
        }
        int32_t dst_o = shl(src2_o, shift_count);
        int32_t dst_e = shl(src2_e, shift_count);

        WRITE_BUFFER(core, dst, dst_e);
        WRITE_BUFFER(core, dst + 1, dst_o);
    }
    core->pc += 4;
    return 0;
}

static int32_t shl2(int32_t op1, int32_t op2)
{
    int32_t result;

    if (op2 & 0x10)
        return 0;                       // If we shift more than 16-bits, it's all zero!

    // Perform low 1/2 shift
    result = ((op1 & 0x0000FFFF) << (op2 & 0xf));
    result &= 0xFFFF;                   // Truncate the shift of the low 16-bits to 16-bits

    // Perform high shift, and OR in the result
    result |= ((op1 & 0xFFFF0000) << (op2 & 0xf));

    return result;
}

static int exec_dshl2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int ucst5 = fields->src13_17;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op6_11;

        int32_t src1_op = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];
        int32_t src2_o = core->gpr[x][src2 + 1];
        int32_t shift_count = 0;

        if (op == 0x6)
        {
            shift_count = ucst5;
        } else if (op == 0x12)
        {
            shift_count = src1_op;
        } else
        {
            NOT_IMP_NO_NEED;
        }
        int32_t dst_o = shl2(src2_o, shift_count);
        int32_t dst_e = shl2(src2_e, shift_count);

        WRITE_BUFFER(core, dst, dst_e);
        WRITE_BUFFER(core, dst + 1, dst_o);
    }
    core->pc += 4;
    return 0;
}

static int32_t Lmask(int32_t a)
{
    int count;

    count = a & 0x1f;
    if (!count)
        return 0;
    return ((unsigned) 0x80000000 >> count - 1) - 1 ^ 0xffffffff;
}

static int32_t shr(int32_t a, int32_t r)
{
    int32_t shft;
    int32_t res;

    if (r & 0x20)
    {                                   // Is bit 5 set?
        shft = 0x1f;
    } else
    {
        shft = r & 0x1f;                // Look at the bottom 5 bits for shift value
    }
    res = a >> shft;                    // perform shift

    // Perform the sign extension.
    if ((a & 0x80000000) != 0)
    {                                   // if the sign bit is one (negative)
        res |= Lmask(shft);             // then fill with 1's
    } else
    {
        res &= ~Lmask(shft);            // else fill with 0's
    }
    return res;
}

static int exec_dshr(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int ucst5 = fields->src13_17;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op6_11;

        int32_t src1_op = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];
        int32_t src2_o = core->gpr[x][src2 + 1];
        int32_t shift_count = 0;

        if (op == 0x36)
        {
            shift_count = ucst5;
        } else if (op == 0x37)
        {
            shift_count = src1_op;
        } else
        {
            NOT_IMP_NO_NEED;
        }
        int32_t dst_o = shr(src2_o, shift_count);
        int32_t dst_e = shr(src2_e, shift_count);

        WRITE_BUFFER(core, dst, dst_e);
        WRITE_BUFFER(core, dst + 1, dst_o);
    }
    core->pc += 4;
    return 0;
}

static int32_t shr2(int32_t a, int32_t r)
{
    int32_t upper_half, lower_half, result;

    r &= 0x1f;                          // shift count only observes bits 0-3
    upper_half = a >> r;                // break into separate operations
    lower_half = a >> r;                // both halves of word are shifted

    if ((a & 0x80000000) != 0)
    {                                   // if msb of upper half is one (negative)
        upper_half |= Lmask(r);         // then fill upper half with 1's
    } else
    {
        upper_half &= ~Lmask(r);        // else fill upper half with 0's
    }

    if ((a & 0x8000) != 0)
    {                                   // if msb of lower half is one (negative)
        lower_half |= (Lmask(r) >> 16); // then fill lower half with 1's
    } else
    {
        lower_half &= ~(Lmask(r) >> 16);        // else fill lower half with 0's
    }

    lower_half &= 0x0000ffff;           // build result
    upper_half &= 0xffff0000;
    result = upper_half | lower_half;
    return result;
}

static int exec_dshr2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int ucst5 = fields->src13_17;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op6_11;

        int32_t src1_op = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];
        int32_t src2_o = core->gpr[x][src2 + 1];
        int32_t shift_count = 0;

        if (op == 0x18)
        {
            shift_count = ucst5;
        } else if (op == 0x37)
        {
            shift_count = src1_op;
        } else
        {
            NOT_IMP_NO_NEED;
        }
        int32_t dst_o = shr2(src2_o, shift_count);
        int32_t dst_e = shr2(src2_e, shift_count);

        WRITE_BUFFER(core, dst, dst_e);
        WRITE_BUFFER(core, dst + 1, dst_o);
    }
    core->pc += 4;
    return 0;
}

static int32_t shru(int32_t a, int32_t r)
{
    if (r & 0x20)
    {                                   // 32 <= r < 64
        return 0;
    } else
    {
        return ((uint32_t) a) >> (r & 0x1f);    // Look at the bottom 5 bits for shift value
    }
}

static int exec_dshru(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int ucst5 = fields->src13_17;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op6_11;

        int32_t src1_op = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];
        int32_t src2_o = core->gpr[x][src2 + 1];
        int32_t shift_count = 0;

        if (op == 0x26)
        {
            shift_count = ucst5;
        } else if (op == 0x27)
        {
            shift_count = src1_op;
        } else
        {
            NOT_IMP_NO_NEED;
        }
        int32_t dst_o = shru(src2_o, shift_count);
        int32_t dst_e = shru(src2_e, shift_count);

        WRITE_BUFFER(core, dst, dst_e);
        WRITE_BUFFER(core, dst + 1, dst_o);
    }
    core->pc += 4;
    return 0;
}

static int32_t shru2(int32_t a, int32_t r)
{
    int32_t upper_half, lower_half, result;

    r &= 0x1f;                          // shift count only observes bits 0-3
    upper_half = a >> r;
    lower_half = a >> r;                // both halves of word are shifted

    upper_half &= ~Lmask(r);            // fill upper half with 0's
    lower_half &= ~(Lmask(r) >> 16);    // fill lower half with 0's

    lower_half &= 0x0000ffff;           // build result
    upper_half &= 0xffff0000;
    result = upper_half | lower_half;
    return result;
}

static int exec_dshru2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int ucst5 = fields->src13_17;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op6_11;

        int32_t src1_op = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];
        int32_t src2_o = core->gpr[x][src2 + 1];
        int32_t shift_count = 0;

        if (op == 0x19)
        {
            shift_count = ucst5;
        } else if (op == 0x38)
        {
            shift_count = src1_op;
        } else
        {
            NOT_IMP_NO_NEED;
        }
        int32_t dst_o = shru2(src2_o, shift_count);
        int32_t dst_e = shru2(src2_e, shift_count);

        WRITE_BUFFER(core, dst, dst_e);
        WRITE_BUFFER(core, dst + 1, dst_o);
    }
    core->pc += 4;
    return 0;
}

static int exec_dsmpy2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op6_11;

        if (op == 0x1)
        {
            int32_t src1_e = core->gpr[s][src1];
            int32_t src1_o = core->gpr[s][src1 + 1];
            int32_t src2_e = core->gpr[x][src2];
            int32_t src2_o = core->gpr[x][src2 + 1];

            int32_t dst_0 = sat_jz((slow16(src1_e) * slow16(src2_e)) << 1);
            int32_t dst_1 = sat_jz((shigh16(src1_e) * shigh16(src2_e)) << 1);
            int32_t dst_2 = sat_jz((slow16(src1_o) * slow16(src2_o)) << 1);
            int32_t dst_3 = sat_jz((shigh16(src1_o) * shigh16(src2_o)) << 1);

            REGISTER_REGW_EVENT_L(core, dst_0, dst, 3);
            REGISTER_REGW_EVENT_H(core, dst_1, dst + 1, 3);
            REGISTER_REGW_EVENT_H(core, dst_2, dst + 2, 3);
            REGISTER_REGW_EVENT_H(core, dst_3, dst + 3, 3);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int32_t spack4(int32_t a, int32_t b)
{
    int32_t x0, x1, x2, x3, d0, d1, d2, d3, dst;

    x0 = (a >> 16) & 0xffff;
    x1 = a & 0xffff;
    x2 = (b >> 16) & 0xffff;
    x3 = b & 0xffff;

    d0 = x0 & 0xff;
    if (x0 & 0xff00)
        if (x0 & 0x8000)
            d0 = 0;
        else
            d0 = 0xff;

    d1 = x1 & 0xff;
    if (x1 & 0xff00)
        if (x1 & 0x8000)
            d1 = 0;
        else
            d1 = 0xff;

    d2 = x2 & 0xff;
    if (x2 & 0xff00)
        if (x2 & 0x8000)
            d2 = 0;
        else
            d2 = 0xff;

    d3 = x3 & 0xff;
    if (x3 & 0xff00)
        if (x3 & 0x8000)
            d3 = 0;
        else
            d3 = 0xff;

    dst = (d0 << 24) | (d1 << 16) | (d2 << 8) | d3;

    return (dst);
}

static int exec_dspacku4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op6_9;

        if (op == 0x4)
        {
            int32_t src1_e = core->gpr[s][src1];
            int32_t src1_o = core->gpr[s][src1 + 1];
            int32_t src2_e = core->gpr[x][src2];
            int32_t src2_o = core->gpr[x][src2 + 1];

            int32_t dst_e = spack4(src1_o, src1_e);
            int32_t dst_o = spack4(src2_o, src2_e);

            WRITE_BUFFER(core, dst, dst_e);
            WRITE_BUFFER(core, dst + 1, dst_o);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int32_t round_int(int32_t op, int32_t sign, int32_t round, int32_t sticky, int32_t mode)
{
    int32_t round_mode;
    int32_t result;
    int32_t check;

    result = op;
    round_mode = mode;
    if (round_mode == 0)
    {
        if (round && (sticky || (result & 1)))
        {
            check = 0;
            if ((result == 0x7fffffff) && !sign)
            {
                check = 1;
            }
            if ((result == 0x80000000) && sign)
            {
                check = 1;
            }
            if (check)
            {
            } else
                result++;
        }
    } else if (round_mode == 2)
    {
        if (!sign && (round || sticky))
        {
            if (result == 0x7fffffff)
            {
            } else
                result++;
        }
    } else if (round_mode == 3)
    {
        if (sign && (round || sticky))
        {
            if (result == 0x80000000)
            {
            } else
                result++;
        }
    }
    if (sign)
        result = -result;
    return (result);

}

static int32_t spint(int32_t op, int32_t round_mode)
{
    int32_t sign;
    int32_t exp;
    int32_t man;
    int32_t round;
    int32_t sticky;
    int32_t result;
    int32_t check;

    sign = extract_sign(op);
    if (snan_chk_sp(op) || qnan_chk_sp(op))
    {
        return (sign ? 0x80000000 : 0x7fffffff);
    }

    exp = extract_exp_sp(op) - 127;
    man = extract_man_sp(op);
    check = 0;
    if (inf_chk_sp(op))
    {
        check = 1;
    }
    if (exp > 31)
    {
        check = 1;
    }
    if ((exp == 31) && !sign)
    {
        check = 1;
    }
    if ((exp == 31) && sign && (op & 0x007fffff))
    {
        check = 1;
    }
    if (check)
    {
        return (sign ? 0x80000000 : 0x7fffffff);
    }

    if (denorm_chk_sp(op))
    {
        return (0);
    }

    if (exp == -127)
        return (0);

    if (exp < 0)
    {
        result = 0;
        round = (exp == -1);
        sticky = (exp == -1) ? (man & 0x007fffff) : 1;
    } else if (exp >= 23)
    {
        result = man << (exp - 23);
        round = 0;
        sticky = 0;
    } else
    {
        result = man >> (23 - exp);
        man <<= exp + 9;
        round = (man & 0x80000000) ? 1 : 0;
        sticky = (man & 0x7fffffff) ? 1 : 0;
    }

    result = round_int(result, sign, round, sticky, round_mode);

    return result;
}

static int32_t spinth(int32_t op, int32_t round_mode)
{
    int32_t sign;
    int32_t exp;
    int32_t man;
    int32_t round;
    int32_t sticky;
    int32_t result;
    int32_t check;

    sign = extract_sign(op);
    if (snan_chk_sp(op) || qnan_chk_sp(op))
    {
        return (sign ? 0x80000000 : 0x7fff7fff);
    }

    exp = extract_exp_sp(op) - 127;
    man = extract_man_sp(op);
    check = 0;
    if (inf_chk_sp(op))
    {
        check = 1;
    }
    if (exp > 15)
    {
        check = 1;
    }
    if ((exp == 15) && !sign)
    {
        check = 1;
    }
    if ((exp == 15) && sign && (op & 0x007fff00))
    {
        check = 1;
    }
    if (check)
    {
        return (sign ? 0x80000000 : 0x7fff7fff);
    }

    if (denorm_chk_sp(op))
    {
        return (0);
    }

    if (exp == -127)
        return (0);

    if (exp < 0)
    {
        result = 0;
        round = (exp == -1);
        sticky = (exp == -1) ? (man & 0x007fffff) : 1;
    } else if (exp >= 23)
    {
        result = man << (exp - 23);
        round = 0;
        sticky = 0;
    } else
    {
        result = man >> (23 - exp);
        man <<= exp + 9;
        round = (man & 0x80000000) ? 1 : 0;
        sticky = (man & 0x7fffffff) ? 1 : 0;
    }

    result = round_int(result, sign, round, sticky, round_mode);

    return result;
}

static int exec_dspint(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op2_11;

        int32_t round_mode;

        if (op == 0xd6)
        {                               //Unit L
            fields->exec_unit = UNIT_L1 << s;
            round_mode = fields->exec_unit == UNIT_L1 ? (((core->fadcr) >> 9) & 0x3) : (((core->fadcr) >> 25) & 0x3);
        } else if (op == 0x3c8)
        {                               //Unit S
            fields->exec_unit = UNIT_S1 << s;
            round_mode = fields->exec_unit == UNIT_S1 ? (((core->fadcr) >> 9) & 0x3) : (((core->fadcr) >> 25) & 0x3);
        } else
        {
            NOT_IMP_NO_NEED;
        }

        int32_t dst_e = spint(core->gpr[x][src2], round_mode);
        int32_t dst_o = spint(core->gpr[x][src2 + 1], round_mode);

        REGISTER_REGW_EVENT_L(core, dst_e, dst, 2);
        REGISTER_REGW_EVENT_H(core, dst_o, dst + 1, 2);
    }
    core->pc += 4;
    return 0;
}

static int exec_dspinth(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op2_11;
        int32_t round_mode;

        if (op == 0xd6)
        {                               //Unit L
            fields->exec_unit = UNIT_L1 << s;
            round_mode = fields->exec_unit == UNIT_L1 ? (((core->fadcr) >> 9) & 0x3) : (((core->fadcr) >> 25) & 0x3);
        } else if (op == 0x3c8)
        {                               //Unit S
            fields->exec_unit = UNIT_S1 << s;
            round_mode = fields->exec_unit == UNIT_S1 ? (((core->fadcr) >> 9) & 0x3) : (((core->fadcr) >> 25) & 0x3);
        } else
        {
            NOT_IMP_NO_NEED;
        }

        int32_t dst_e = spinth(core->gpr[x][src2], round_mode);
        int32_t dst_o = spinth(core->gpr[x][src2 + 1], round_mode);

        int32_t result = ((dst_o << 16) & 0xFFFF0000) | (dst_e & 0xFFFF);

        REGISTER_REGW_EVENT_L(core, result, dst, 2);
    }
    core->pc += 4;
    return 0;
}

static int32_t ssub(int32_t a, int32_t b)
{
    int32_t c;

    c = a - b;
    if ((a < 0) && (b >= 0) && (c >= 0))
    {
        /* If a is negative and b is positive, but result is positive, saturate negative */
        c = 0x80000000;
    } else if ((a >= 0) && (b < 0) && (c < 0))
    {
        /* If a is positive and b is negative, but result is negative, saturate positive */
        c = 0x7fffffff;
    }
    return c;
}

static int exec_dssub(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op5_11;

        if (op == 0x67)
        {
            int32_t src1_e = core->gpr[s][src1];
            int32_t src1_o = core->gpr[s][src1 + 1];
            int32_t src2_e = core->gpr[x][src2];
            int32_t src2_o = core->gpr[x][src2 + 1];

            int32_t dst_e = ssub(src1_e, src2_e);
            int32_t dst_o = ssub(src1_o, src2_o);

            WRITE_BUFFER(core, dst, dst_e);
            WRITE_BUFFER(core, dst + 1, dst_o);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int32_t ssub2_joule(int32_t a, int32_t b)
{
    int a_lo, b_lo, a_hi, b_hi;
    int32_t result;
    int u_tmp;

    // spec says this does not change the CSR.sat bit 
    a_lo = slow16(a);
    a_hi = shigh16(a);
    b_lo = slow16(b);
    b_hi = shigh16(b);

    u_tmp = a_hi - b_hi;

    // check to see if the result saturated
    if ((a_hi & 0x8000) && !(b_hi & 0x8000) && !(u_tmp & 0x8000))
        u_tmp = 0x8000;
    if (!(a_hi & 0x8000) && (b_hi & 0x8000) && (u_tmp & 0x8000))
        u_tmp = 0x7fff;

    result = (u_tmp << 16) & 0xffff0000;

    u_tmp = a_lo - b_lo;

    // check to see if the result saturated
    if ((a_lo & 0x8000) && !(b_lo & 0x8000) && !(u_tmp & 0x8000))
        u_tmp = 0x8000;
    if (!(a_lo & 0x8000) && (b_lo & 0x8000) && (u_tmp & 0x8000))
        u_tmp = 0x7fff;

    result |= (u_tmp & 0x0000ffff);
    return result;
}

static int exec_dssub2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op5_11;

        if (op == 0x64)
        {
            int32_t src1_e = core->gpr[s][src1];
            int32_t src1_o = core->gpr[s][src1 + 1];
            int32_t src2_e = core->gpr[x][src2];
            int32_t src2_o = core->gpr[x][src2 + 1];

            int32_t dst_e = ssub2_joule(src1_e, src2_e);
            int32_t dst_o = ssub2_joule(src1_o, src2_o);

            WRITE_BUFFER(core, dst, dst_e);
            WRITE_BUFFER(core, dst + 1, dst_o);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_dsub(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src1_o = core->gpr[s][src1 + 1];
        int32_t src2_e = core->gpr[x][src2];
        int32_t src2_o = core->gpr[x][src2 + 1];

        int32_t dst_e = src1_e - src2_e;
        int32_t dst_o = src1_o - src2_o;

        WRITE_BUFFER(core, dst, dst_e);
        WRITE_BUFFER(core, dst + 1, dst_o);
    }
    core->pc += 4;
    return 0;
}

static int32_t sub2(int32_t a, int32_t b)
{
    int32_t d_low, d_high;

    d_low = 0x0000ffff & (a - b);       /* Subtract the low 16-bits */
    d_high = (0xffff0000 & a) - (0xffff0000 & b);       /* Subtract the high 16-bits */
    return d_high | d_low;
}

static int exec_dsub2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src1_o = core->gpr[s][src1 + 1];
        int32_t src2_e = core->gpr[x][src2];
        int32_t src2_o = core->gpr[x][src2 + 1];

        int32_t dst_e = sub2(src1_e, src2_e);
        int32_t dst_o = sub2(src1_o, src2_o);

        WRITE_BUFFER(core, dst, dst_e);
        WRITE_BUFFER(core, dst + 1, dst_o);
    }
    core->pc += 4;
    return 0;
}

static int32_t sub_sp(int32_t op1, int32_t op2, int rmode1, int rmode2)
{
    return (add_sp(op1, op2 ^ 0x80000000, rmode1, rmode2));
}

static int exec_dsubsp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int op = fields->op2_11;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src1_o = core->gpr[s][src1 + 1];
        int32_t src2_e = core->gpr[x][src2];
        int32_t src2_o = core->gpr[x][src2 + 1];

        int32_t rmode1;
        int32_t rmode2;

        if (op == 0x1ee)
        {                               //Unit L
            fields->exec_unit = UNIT_L1 << s;
            rmode1 = fields->exec_unit == UNIT_L1 ? (((core->fadcr) >> 9) & 0x3) : (((core->fadcr) >> 25) & 0x3);
            rmode2 = fields->exec_unit == UNIT_L1 ? (((core->fmcr) >> 9) & 0x3) : (((core->fmcr) >> 25) & 0x3);
        } else if (op == 0x2d8)
        {                               //Unit S
            fields->exec_unit = UNIT_S1 << s;
            rmode1 = fields->exec_unit == UNIT_S1 ? (((core->fadcr) >> 9) & 0x3) : (((core->fadcr) >> 25) & 0x3);
            rmode2 = fields->exec_unit == UNIT_S1 ? (((core->fmcr) >> 9) & 0x3) : (((core->fmcr) >> 25) & 0x3);
        } else
        {
            NOT_IMP_NO_NEED;
        }

        int32_t dst_e = sub_sp(src1_e, src2_e, rmode1, rmode2);
        int32_t dst_o = sub_sp(src1_o, src2_o, rmode1, rmode2);

        REGISTER_REGW_EVENT_L(core, dst_e, dst, 2);
        REGISTER_REGW_EVENT_H(core, dst_o, dst + 1, 2);
    }
    core->pc += 4;
    return 0;
}

static int32_t expand2(int32_t a)
{
    int32_t result;

    result = 0;
    if (a & 1)
        result |= 0xffff;
    if (a & 2)
        result |= 0xffff0000;

    return result;
}

static int exec_dxpnd2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = BITS(13, 17);

        if (op == 0x11)
        {
            int32_t src2_op = core->gpr[x][src2];

            int32_t dst_e = expand2(src2_op);
            int32_t dst_o = expand2(src2_op >> 2);

            REGISTER_REGW_EVENT_L(core, dst_e, dst, 1);
            REGISTER_REGW_EVENT_H(core, dst_o, dst + 1, 1);
        } else
        {
            NOT_IMP_NO_NEED;
        }

    }
    core->pc += 4;
    return 0;
}

static int32_t expand4(int32_t a)
{
    int32_t result;

    result = 0;
    if (a & 1)
        result |= 0xff;
    if (a & 2)
        result |= 0xff00;
    if (a & 4)
        result |= 0xff0000;
    if (a & 8)
        result |= 0xff000000;

    return result;
}

static int exec_dxpnd4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = BITS(13, 17);

        if (op == 0x10)
        {
            int32_t src2_op = core->gpr[x][src2];

            int32_t dst_e = expand4(src2_op);
            int32_t dst_o = expand4(src2_op >> 4);

            REGISTER_REGW_EVENT_L(core, dst_e, dst, 1);
            REGISTER_REGW_EVENT_H(core, dst_o, dst + 1, 1);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_ext(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int csta = fields->csta;
        int cstb = fields->cstb;
        int result;

        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;

        result = (int) (core->gpr[s][src2] << csta);
        result >>= cstb;
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    core->pc += 4;
    return 0;
}
static int exec_extu(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        uint32_t csta = fields->csta;
        uint32_t cstb = fields->cstb;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;

        fields->exec_unit = UNIT_S1 << s;
        int result = (core->gpr[s][src2] << csta) >> cstb;

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);

        DBG("In %s, csta=%d, cstb=%d, \n", __FUNCTION__, csta, cstb);
    }
    core->pc += 4;
    return 0;
}

static int exec_extu_reg_form(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {

        int s = fields->s;
        int reg_group = fields->x ? (!s) & 0x1 : s;
        uint32_t src1 = core->gpr[s][fields->src13_17];
        uint32_t csta = ((src1 >> (5)) & ((1 << (1 + (9) - (5))) - 1)); // src1[9..5]
        uint32_t cstb = src1 & 0x1F;    // src1[4..0]
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;

        uint32_t result = (core->gpr[reg_group][src2] << csta) >> cstb;

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);

        DBG("In %s, csta=%d, cstb=%d, \n", __FUNCTION__, csta, cstb);
    }
    core->pc += 4;
    return 0;
}

static int exec_fadddp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op5_11;
        double result;

        GET_DW_DOUBLE(core->gpr[s][src1], core->gpr[s][src1 + 1], dwop1)
            GET_DW_DOUBLE(core->gpr[x][src2], core->gpr[x][src2 + 1], xdwop2) result = dwop1 + xdwop2;
        CVT_DW_TO_WORDS(result, res_l, res_h)
            //FIXME, need to check result, and related register bit should be set, c-p: 4-337
            if (op == 0x58)
        {                               //Unit L
            fields->exec_unit = UNIT_L1 << s;
        } else if (op == 0x19)
        {                               //Unit S
            fields->exec_unit = UNIT_S1 << s;
        } else
        {
            NOT_IMP_NO_NEED;
        }

        REGISTER_REGW_EVENT_L(core, res_l, dst, 2);
        REGISTER_REGW_EVENT_H(core, res_h, dst + 1, 2);
    }
    core->pc += 4;
    return 0;
}

static int exec_faddsp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op5_11;
        float *op1 = (float *) &core->gpr[s][src1];
        float *xop2 = (float *) &core->gpr[x][src2];
        float result = (*op1) + (*xop2);
        uint32_t *presult = (uint32_t *) & result;

        if (op == 0x3c)
        {                               //Unit L
            fields->exec_unit = UNIT_L1 << s;
        } else if (op == 0x74)
        {                               //Unit S
            fields->exec_unit = UNIT_S1 << s;
        } else
        {
            NOT_IMP_NO_NEED;
        }

        REGISTER_REGW_EVENT_L(core, *presult, dst, 2);
    }
    core->pc += 4;
    return 0;
}

static int exec_fmpydp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_M1 << fields->s;
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        double result;
        int op = fields->op6_11;

        if (op == 0xf)
        {
            GET_DW_DOUBLE(core->gpr[s][src1], core->gpr[s][src1 + 1], dwop1)
                GET_DW_DOUBLE(core->gpr[x][src2], core->gpr[x][src2 + 1], xdwop2) result = dwop1 * xdwop2;
            CVT_DW_TO_WORDS(result, res_l, res_h)
                //FIXME, need to check result, and related register bit should be set, c-p: 4-337
                REGISTER_REGW_EVENT_L(core, res_l, dst, FMPYDP_DELAY_SLOT);
            REGISTER_REGW_EVENT_H(core, res_h, dst + 1, FMPYDP_DELAY_SLOT);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_fsubdp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op2_11;
        double result;

        GET_DW_DOUBLE(core->gpr[s][src1], core->gpr[s][src1 + 1], dwop1)
            GET_DW_DOUBLE(core->gpr[x][src2], core->gpr[x][src2 + 1], xdwop2) result = dwop1 - xdwop2;
        CVT_DW_TO_WORDS(result, res_l, res_h)
            //FIXME, need to check result, and related register bit should be set, c-p: 4-337
            if (op == 0x2ce)
        {                               //Unit L
        } else if (op == 0x3ec)
        {                               //Unit S
        } else
        {
            NOT_IMP_NO_NEED;
        }

        REGISTER_REGW_EVENT_L(core, res_l, dst, 2);
        REGISTER_REGW_EVENT_H(core, res_h, dst + 1, 2);
    }

    core->pc += 4;
    return 0;
}

static int exec_fsubsp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op5_11;
        float *op1 = (float *) &core->gpr[s][src1];
        float *xop2 = (float *) &core->gpr[x][src2];
        float result = (*op1) - (*xop2);
        uint32_t *presult = (uint32_t *) & result;

        if (op == 0x3d)
        {                               //Unit L
            fields->exec_unit = UNIT_L1 << s;
        } else if (op == 0x76)
        {                               //Unit S
            fields->exec_unit = UNIT_S1 << s;
        } else
        {
            NOT_IMP_NO_NEED;
        }

        REGISTER_REGW_EVENT_L(core, *presult, dst, 2);
    }
    core->pc += 4;
    return 0;
}

uint32_t gmpy(uint32_t src1, uint32_t src2, uint32_t polynomial)
{
    uint32_t pp;
    uint32_t mask, tpp;
    uint32_t I;

    pp = 0;
    mask = 0x00000100;
    for (I = 0; I < 8; I++)
    {
        if (src2 & mask)
            pp ^= src1;
        mask >>= 1;
        tpp = pp << 1;
        if (pp & 0x80000000)
            pp = polynomial ^ tpp;
        else
            pp = tpp;
    }
    if (src2 & 0x1)
        pp ^= src1;

    return pp;
}

static int exec_gmpy(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];

        uint32_t dst_e;
        uint32_t GMPY_poly;

        fields->exec_unit = UNIT_M1 << s;

        if (fields->exec_unit == UNIT_M1)
        {
            GMPY_poly = core->gplya;
        } else if (fields->exec_unit == UNIT_M2)
        {
            GMPY_poly = core->gplyb;
        }
        dst_e = gmpy(src1_e, src2_e, GMPY_poly);

        REGISTER_REGW_EVENT_L(core, dst_e, dst, 3);
    }
    core->pc += 4;
    return 0;
}

uint32_t gmpy4(uint32_t src1, uint32_t src2, uint32_t polynomial)
{
    uint32_t pp;
    uint32_t mask, tpp;
    uint32_t I;
    uint32_t GFPGFR_size;

    pp = 0;
    mask = 0x00000100;
    GFPGFR_size = ((polynomial & 0x07000000) >> 24) + 1;

    for (I = 0; I < GFPGFR_size; I++)
    {
        if (src2 & mask)
            pp ^= src1;
        mask >>= 1;
        tpp = pp << 1;
        if (pp & 0x80)
            pp = polynomial ^ tpp;
        else
            pp = tpp;
    }
    if (src2 & 0x1)
        pp ^= src1;

    return pp;
}

static int exec_gmpy4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];

        uint32_t dst_e, dst_3, dst_2, dst_1, dst_0;
        uint32_t GMPY_poly;

        fields->exec_unit = UNIT_M1 << s;
        GMPY_poly = core->gfpgfr;

        dst_0 = ubyte0(gmpy4(ubyte0(src1_e), ubyte0(src2_e), GMPY_poly));
        dst_1 = ubyte0(gmpy4(ubyte1(src1_e), ubyte1(src2_e), GMPY_poly)) << 8;
        dst_2 = ubyte0(gmpy4(ubyte2(src1_e), ubyte2(src2_e), GMPY_poly)) << 16;
        dst_3 = ubyte0(gmpy4(ubyte3(src1_e), ubyte3(src2_e), GMPY_poly)) << 24;

        dst_e = dst_3 | dst_2 | dst_1 | dst_0;

        REGISTER_REGW_EVENT_L(core, dst_e, dst, 3);
    }
    core->pc += 4;
    return 0;
}

static int exec_idle(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    //NEW_IMP;
    //NOT_IMP;
    core->pc += 4;
    return 0;
}

static int exec_intdp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_L1 << fields->s;
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int dst = fields->dst23_27;
        int src2 = fields->src18_22;

        int gpr_group = fields->x ? (!s) & 0x1 : s;
        double dst_fp = (double) ((int) core->gpr[gpr_group][src2]);

        CVT_DW_TO_WORDS(dst_fp, dst_l, dst_h);
        REGISTER_REGW_EVENT_L(core, dst_l, s * GPR_NUM_IN_GROUP + dst, INTDP_DELAY_SLOT - 1);
        REGISTER_REGW_EVENT_H(core, dst_h, s * GPR_NUM_IN_GROUP + dst + 1, INTDP_DELAY_SLOT);
    }
    core->pc += 4;
    return 0;
}

static int exec_intdpu(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_L1 << fields->s;
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int dst = fields->dst23_27;
        int src2 = fields->src18_22;

        int gpr_group = fields->x ? (!s) & 0x1 : s;
        uint32_t src2_value = core->gpr[gpr_group][src2];
        double dst_fp = (double) src2_value;

        CVT_DW_TO_WORDS(dst_fp, dst_l, dst_h);
        REGISTER_REGW_EVENT_L(core, dst_l, s * GPR_NUM_IN_GROUP + dst, INTDP_DELAY_SLOT - 1);
        REGISTER_REGW_EVENT_H(core, dst_h, s * GPR_NUM_IN_GROUP + dst + 1, INTDP_DELAY_SLOT);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_intsp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int dst = fields->dst23_27;
        int src2 = fields->src18_22;
        int op = fields->op5_11;

        if (op == 0x4a)
        {
            fields->exec_unit = UNIT_L1 << s;
        } else if (op == 0x79)
        {
            fields->exec_unit = UNIT_S1 << s;
        } else
        {
            NOT_IMP_NO_NEED;
        }

        int gpr_group = fields->x ? (!s) & 0x1 : s;
        float dst_fp = (float) ((int) (core->gpr[gpr_group][src2]));
        uint32_t result = *(uint32_t *) & dst_fp;

        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, 3);
    }
    core->pc += 4;
    return 0;
}
static int exec_intspu(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int dst = fields->dst23_27;
        int src2 = fields->src18_22;
        int op = fields->op5_11;

        if (op == 0x49)
        {
            fields->exec_unit = UNIT_L1 << s;
        } else if (op == 0x79)
        {
            fields->exec_unit = UNIT_S1 << s;
        } else
        {
            NOT_IMP_NO_NEED;
        }

        int gpr_group = fields->x ? (!s) & 0x1 : s;
        uint32_t src2_value = core->gpr[gpr_group][src2];
        float dst_fp = (float) src2_value;
        uint32_t result = *(uint32_t *) & dst_fp;

        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, 3);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}

static int exec_land(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op5_11;

        if (op == 0x78)
        {
            int32_t result = (core->gpr[s][src1] && core->gpr[x][src2]) ? 1 : 0;

            WRITE_BUFFER(core, dst, result);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_landn(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op5_11;

        if (op == 0x79)
        {
            int32_t result = (core->gpr[s][src1] && !core->gpr[x][src2]) ? 1 : 0;

            WRITE_BUFFER(core, dst, result);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

inline void lds_save_result(c6k_core_t * core, opcode_fields_t * fields, uint64_t result,
                            uint32_t reg_no, generic_address_t addr, rw_flag_t rw_flag, bool_t sign_ext, uint32_t width)
{

    REGISTER_MEMORY_EVENT(core, result, reg_no, addr, rw_flag, sign_ext, width);
    if (INSN_LDS_UNDER_PROTECTED(fields) && rw_flag == RD_FLAG)
    {
        ADD_PENDING_NOPS(core, 4);
    }
}

static int exec_ldbu(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    DBG("In %s, pc=0x%x\n", __FUNCTION__, core->pc);
    if (calc_cond(core, insn))
    {
        int op = fields->op4_6;
        int y = fields->y;
        int mode = fields->mode;
        int offset = fields->offset;
        int base = fields->base;
        int dst = fields->dst23_27;
        int s = fields->s;

        fields->exec_unit = UNIT_D1 << s;
        generic_address_t addr = calc_addr(core, base, offset, mode, y, 0);
        uint32_t result = core->gpr[s][dst];

        if (op == 1)
        {
            result &= 0xFF;
            lds_save_result(core, fields, result, dst + s * GPR_NUM_IN_GROUP, addr, RD_FLAG, 0, 8);
            DBG("In %s, pc=0x%x\n", __FUNCTION__, core->pc);
        } else if (op == 2)
        {
            if (result & 0x80)
                result = 0xFFFFFF00 | (result & 0xFF);

            lds_save_result(core, fields, result, dst + s * GPR_NUM_IN_GROUP, addr, RD_FLAG, 1, 8);
        } else
        {
            NOT_IMP_NO_NEED;
        }
        DBG("In %s, addr = 0x%x, data=0x%x\n", __FUNCTION__, addr, core->gpr[s][dst]);
    }
    core->pc += 4;
    return 0;
}
static int exec_ldbu_15(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int ucst15 = fields->ucst8_22;
        int dst = fields->dst23_27;
        int y = fields->y;
        int s = fields->s;

        fields->exec_unit = UNIT_D2;

        uint32_t result = 0;
        int base = 14 + y;
        generic_address_t addr = core->gpr[GPR_B][base] + ucst15;

        lds_save_result(core, fields, result, dst + s * GPR_NUM_IN_GROUP, addr, RD_FLAG, 0, 8);
    }
    core->pc += 4;
    return 0;
}
static int exec_ldb_15(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int ucst15 = fields->ucst8_22;
        int dst = fields->dst23_27;
        int y = fields->y;
        int s = fields->s;

        uint32_t result = 0;
        int base = 14 + y;
        generic_address_t addr = core->gpr[GPR_B][base] + ucst15;

        lds_save_result(core, fields, result, dst + s * GPR_NUM_IN_GROUP, addr, RD_FLAG, 1, 8);
    }
    core->pc += 4;
    return 0;
}

static int exec_lddw(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_D1 << fields->s;
    if (calc_cond(core, insn))
    {
        int dst = BITS(23, 27);
        int base = BITS(18, 22);
        int offset = BITS(13, 17);
        int mode = BITS(9, 12);
        int y = BITS(7, 7);
        int s = BITS(1, 1);
        generic_address_t addr = calc_addr(core, base, offset, mode, y, 3);
        uint64_t result = 0;

        lds_save_result(core, fields, result, dst + s * GPR_NUM_IN_GROUP, addr, RD_FLAG, 0, 64);
        DBG("In %s, addr = 0x%x, data=0x%x\n", __FUNCTION__, addr, core->gpr[s][dst]);

    }
    core->pc += 4;
    return 0;
}
static int exec_ldhu(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op4_6;
        int dst = fields->dst23_27;
        int base = fields->base;
        int offset = fields->offset;
        int mode = fields->mode;
        int y = fields->y;
        int s = fields->s;
        generic_address_t addr = calc_addr(core, base, offset, mode, y, 1);
        uint32_t result = 0;

        if (op == 0x0)
        {
            lds_save_result(core, fields, result, dst + s * GPR_NUM_IN_GROUP, addr, RD_FLAG, 0, 16);
        } else if (op == 0x4)
        {
            lds_save_result(core, fields, result, dst + s * GPR_NUM_IN_GROUP, addr, RD_FLAG, 1, 16);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}
static int exec_ldhu_1(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op4_6;
        int dst = fields->dst23_27;
        uint16_t ucst15 = fields->ucst8_22;
        int y = fields->y;
        int s = fields->s;
        generic_address_t addr = core->gpr[1][y ? 15 : 14] + (ucst15 << 1);
        uint32_t result = 0;

        if (op == 0x0)
        {
            lds_save_result(core, fields, result, dst + s * GPR_NUM_IN_GROUP, addr, RD_FLAG, 0, 16);
        } else if (op == 0x4)
        {
            lds_save_result(core, fields, result, dst + s * GPR_NUM_IN_GROUP, addr, RD_FLAG, 1, 16);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_ldndw(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int dst = fields->dst24_27;
        int sc = fields->sc;
        int base = fields->base;
        int offset = fields->offset;
        int mode = fields->mode;
        int y = fields->y;
        int s = fields->s;

        dst = dst << 1;
        DBG("In %s, insn = 0x%x, base=0x%x, dst=%d\n", __FUNCTION__, insn, base, dst);
        /* FIXME, sc bit need to be considered */
        generic_address_t addr = calc_addr(core, base, offset, mode, y, sc ? 3 : 0);
        uint64_t result = 0;

        lds_save_result(core, fields, result, dst + s * GPR_NUM_IN_GROUP, addr, RD_FLAG, 0, 64);
    }
    core->pc += 4;
    return 0;
}
static int exec_ldnw(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int dst = fields->dst23_27;
        int base = fields->base;
        int offset = fields->offset;
        int mode = fields->mode;
        int y = fields->y;
        int s = fields->s;
        generic_address_t addr = calc_addr(core, base, offset, mode, y, 2);
        uint32_t result = 0;

        lds_save_result(core, fields, result, dst + s * GPR_NUM_IN_GROUP, addr, RD_FLAG, 0, 32);
        DBG("In %s, addr = 0x%x, data=0x%x\n", __FUNCTION__, addr, core->gpr[s][dst]);

    }
    core->pc += 4;
    return 0;
}

static int exec_ldw(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int dst = fields->dst23_27;
        int base = fields->base;
        int offset = fields->offset;
        int mode = fields->mode;
        int y = fields->y;
        int s = fields->s;
        generic_address_t addr = calc_addr(core, base, offset, mode, y, 2);

        uint32_t result = 0;

        lds_save_result(core, fields, result, dst + s * GPR_NUM_IN_GROUP, addr, RD_FLAG, 0, 32);

        DBG("In %s, addr = 0x%x, data=0x%x, dst=%d, base%d\n", __FUNCTION__, addr, core->gpr[s][dst], dst, base);
    }
    core->pc += 4;
    return 0;
}

static int exec_ldw_15(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int ucst15 = fields->ucst8_22;
        int dst = fields->dst23_27;
        int y = fields->y;
        int s = fields->s;

        uint32_t result = 0;
        int base = 14 + y;
        generic_address_t addr = core->gpr[GPR_B][base] + (ucst15 << 2);

        DBG("########### in %s, addr=0x%x, pc=0x%x\n", __FUNCTION__, addr, core->pc);
        lds_save_result(core, fields, result, dst + s * GPR_NUM_IN_GROUP, addr, RD_FLAG, 0, 32);
    }
    core->pc += 4;
    return 0;
}
static int exec_lor(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;
        int result = 0;
        int op = fields->op5_11;

        if (op == 0x7d)
        {
            if (core->gpr[s][src1] != 0 || core->gpr[reg_group][src2] != 0)
                result = 1;

            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_lmbd(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op5_11;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int reg_group = fields->x ? (!s) & 0x1 : s;
        uint32_t result = 0;
        uint32_t search_bit = 0;
        uint32_t search_reg = core->gpr[reg_group][src2];

        if (op == 0x6a)
        {
            search_bit = src1 & 0x1;
        } else if (op == 0x6b)
        {
            search_bit = core->gpr[s][src1] & 0x1;
        } else
        {
            NOT_IMP_NO_NEED;
        }

        while (result < 32)
        {
            if (search_bit == ((search_reg >> (31 - result)) & 0x1))
                break;
            result++;
        }

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    core->pc += 4;
    return 0;
}
static int exec_max2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t dst_e = max2(core->gpr[s][src1], core->gpr[x][src2]);

        WRITE_BUFFER(core, dst, dst_e);
    }
    core->pc += 4;
    return 0;
}
static int exec_maxu4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t dst_e = max4(core->gpr[s][src1], core->gpr[x][src2]);

        WRITE_BUFFER(core, dst, dst_e);
    }
    core->pc += 4;
    return 0;
}
static int exec_mfence(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = BITS(13, 16);

        if (op == 0x4)
        {
            ADD_PENDING_NOPS(core, 5);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    //NOT_IMP;
    core->pc += 4;
    return 0;
}
static int exec_min2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t dst_e = min2(core->gpr[s][src1], core->gpr[x][src2]);

        WRITE_BUFFER(core, dst, dst_e);
    }
    core->pc += 4;
    return 0;
}
static int exec_minu4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t dst_e = min4(core->gpr[s][src1], core->gpr[x][src2]);

        WRITE_BUFFER(core, dst, dst_e);
    }
    core->pc += 4;
    return 0;
}
static int exec_mpy(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op7_11;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int gpr_group = x ? (!s) & 0x1 : s;
        int op1;

        if (op == 0x19)
        {
            op1 = SIGN_EXTEND(core->gpr[s][src1] & 0xFFFF, 16);
        } else if (op == 0x18)
        {
            op1 = SIGN_EXTEND(src1 & 0xFFFF, 16);
        } else
        {
            NOT_IMP_NO_NEED;
        }
        int op2 = SIGN_EXTEND(core->gpr[gpr_group][src2] & 0xFFFF, 16);
        int result = op1 * op2;

        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, MPY_DELAY_SLOT);

    }
    core->pc += 4;
    return 0;
}

static int exec_mpydp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int gpr_group = x ? (!s) & 0x1 : s;
        uint64_t fp1 = core->gpr[s][src1 + 1];

        fp1 = (fp1 << 32);
        fp1 |= core->gpr[s][src1];
        uint64_t fp2 = core->gpr[gpr_group][src2 + 1];

        fp2 = (fp2 << 32);
        fp2 |= core->gpr[gpr_group][src2];
        double f1 = *(double *) &fp1;
        double f2 = *(double *) &fp2;
        double dst_fp = f1 * f2;
        uint64_t result = *(uint64_t *) & dst_fp;

        REGISTER_REGW_EVENT_L(core, result & 0xFFFFFFFF, s * GPR_NUM_IN_GROUP + dst, MPYDP_DELAY_SLOT - 1);
        REGISTER_REGW_EVENT_H(core, result >> 32, s * GPR_NUM_IN_GROUP + dst + 1, MPYDP_DELAY_SLOT);
    }
    core->pc += 4;
    return 0;
}

static int exec_mpyh(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;

        int32_t result = 0;

        int op1 = SIGN_EXTEND((core->gpr[s][src1] >> 16), 16);
        int op2 = SIGN_EXTEND(core->gpr[reg_group][src2] >> 16, 16);

        result = op1 * op2;
        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, MPY_DELAY_SLOT);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_mpyhi(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;

        int64_t result = 0;

        result = (int64_t) shigh16((int32_t) core->gpr[s][src1]) * (int64_t) ((int32_t) core->gpr[reg_group][src2]);

        REGISTER_REGW_EVENT_L(core, result & 0xFFFFFFFF, s * GPR_NUM_IN_GROUP + dst, MPYIH_DELAY_SLOT);
        REGISTER_REGW_EVENT_L(core, result >> 32, s * GPR_NUM_IN_GROUP + dst + 1, MPYIH_DELAY_SLOT);
    }
    core->pc += 4;
    return 0;
}
static int exec_mpyhir(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;

        int32_t result = 0;

        result =
            ((int64_t) shigh16((int32_t) core->gpr[s][src1]) * (int64_t) ((int32_t) core->gpr[reg_group][src2]) +
             0x4000) >> 15;

        REGISTER_REGW_EVENT_L(core, result & 0xFFFFFFFF, s * GPR_NUM_IN_GROUP + dst, MPYIH_DELAY_SLOT);
    }
    core->pc += 4;
    return 0;
}
static int exec_mpyhl(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;

        int32_t result;

        result = (core->gpr[s][src1] >> 16) * (core->gpr[reg_group][src2] & 0xFFFF);
        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, MPY_DELAY_SLOT);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_mpyhlu(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = BITS(13, 17);
        int src2 = BITS(18, 22);
        int dst = BITS(23, 27);
        int x = BITS(12, 12);
        int s = BITS(1, 1);
        uint16_t op1 = (core->gpr[s][src1] >> 16);
        uint16_t op2;

        if (x)
            op2 = (uint16_t) core->gpr[(!s) & 0x1][src2] & 0xFFFF;
        else
            op2 = (uint16_t) core->gpr[s][src2] & 0xFFFF;
        uint32_t result = op1 * op2;

        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, MPYLHU_DELAY_SLOT);
    }
    core->pc += 4;
    return 0;
}
static int exec_mpyhslu(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;

        int16_t op1 = core->gpr[s][src1] >> 16;
        uint16_t op2 = (uint16_t) core->gpr[reg_group][src2] & 0xFFFF;
        int32_t result;

        result = op1 * op2;
        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, MPYLHU_DELAY_SLOT);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_mpyhsu(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;

        int16_t op1 = core->gpr[s][src1] >> 16;
        uint16_t op2 = (uint16_t) (core->gpr[reg_group][src2] >> 16);
        int32_t result;

        result = op1 * op2;
        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, MPYLHU_DELAY_SLOT);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_mpyhu(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = BITS(13, 17);
        int src2 = BITS(18, 22);
        int dst = BITS(23, 27);
        int x = BITS(12, 12);
        int s = BITS(1, 1);
        int op1 = core->gpr[s][src1] >> 16;
        int op2;

        if (x)
            op2 = core->gpr[(!s) & 0x1][src2] >> 16;
        else
            op2 = core->gpr[s][src2] >> 16;
        uint32_t result = op1 * op2;

        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, MPYLHU_DELAY_SLOT);

    }
    core->pc += 4;
    return 0;
}
static int exec_mpyhuls(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;

        uint16_t op1 = core->gpr[s][src1] >> 16;
        int16_t op2 = core->gpr[reg_group][src2] & 0xFFFF;
        int32_t result;

        result = op1 * op2;
        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, MPYLHU_DELAY_SLOT);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_mpyhus(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;

        uint16_t op1 = core->gpr[s][src1] >> 16;
        int16_t op2 = core->gpr[reg_group][src2] >> 16;
        int32_t result;

        result = op1 * op2;
        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, MPYLHU_DELAY_SLOT);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}

static int exec_mpyi(c6k_core_t * core, uint32_t insn, opcode_fields_t * unused)
{
    if (calc_cond(core, insn))
    {
        int src1 = BITS(13, 17);
        int src2 = BITS(18, 22);
        int dst = BITS(23, 27);
        int x = BITS(12, 12);
        int s = BITS(1, 1);
        int gpr_group = x ? (!s) & 0x1 : s;
        int op2 = core->gpr[gpr_group][src2];
        int op1;

        int op = BITS(7, 11);

        if (op == 0x4)
        {
            op1 = core->gpr[s][src1];
        } else if (op == 0x6)
        {
            op1 = src1;
        } else
        {
            NOT_IMP_NO_NEED;
        }
        long long result = op1 * op2;

        REGISTER_REGW_EVENT_L(core, result & 0xFFFFFFFF, s * GPR_NUM_IN_GROUP + dst, MPYI_DELAY_SLOT);
    }
    core->pc += 4;
    return 0;
}

static int exec_mpyid(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;
        int op2 = core->gpr[reg_group][src2];
        int op1;
        int op = fields->op7_11;

        if (op == 0x8)
        {
            op1 = core->gpr[s][src1];
        } else if (op == 0xc)
        {
            op1 = src1;
        } else
        {
            NOT_IMP_NO_NEED;
        }
        int64_t f1 = *(unsigned int *) &op1;
        int64_t f2 = *(int *) &op2;
        int64_t result = f1 * f2;

        REGISTER_REGW_EVENT_L(core, result & 0xFFFFFFFF, s * GPR_NUM_IN_GROUP + dst, MPYID_DELAY_SLOT);
        REGISTER_REGW_EVENT_H(core, (result >> 32) & 0xFFFFFFFF, s * GPR_NUM_IN_GROUP + dst + 1, MPYID_DELAY_SLOT);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}

static int exec_mpyih(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;
        int16_t op1 = core->gpr[s][src1] >> 16;
        int op2 = core->gpr[reg_group][src2];
        long long result = op1 * op2;

        int dst_upper_value = (result >> 32) & 0xFFFF;

        EXTEND_SIGN(dst_upper_value, 32);
        REGISTER_REGW_EVENT_L(core, result & 0xFFFFFFFF, s * GPR_NUM_IN_GROUP + dst, MPYIH_DELAY_SLOT);
        REGISTER_REGW_EVENT_H(core, dst_upper_value, s * GPR_NUM_IN_GROUP + dst + 1, MPYIH_DELAY_SLOT);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_mpyihr(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        // mpyihr = mpyhir;
    }
    NOT_IMP_NO_NEED;
    return 0;
}
static int exec_mpyil(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        // mpyil = mpyli
    }
    NOT_IMP_NO_NEED;
    return 0;
}
static int exec_mpyilr(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        // mpyilr = mpylir
    }
    NOT_IMP_NO_NEED;
    return 0;
}
static int exec_mpylh(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;
        int16_t op1 = core->gpr[s][src1] & 0xFFFF;
        int16_t op2 = (core->gpr[reg_group][src2] >> 16) & 0xFFFF;
        int result = op1 * op2;

        REGISTER_REGW_EVENT_L(core, result & 0xFFFFFFFF, s * GPR_NUM_IN_GROUP + dst, MPYLHU_DELAY_SLOT);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_mpylhu(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int op1 = core->gpr[s][src1] & 0xFFFF;
        int op2;

        if (x)
            op2 = core->gpr[(!s) & 0x1][src2] >> 16;
        else
            op2 = core->gpr[s][src2] >> 16;
        uint32_t result = op1 * op2;

        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, MPYLHU_DELAY_SLOT);
    }
    core->pc += 4;
    return 0;
}
static int exec_mpyli(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int s = fields->s;

    fields->exec_unit = UNIT_M1 << s;
    if (calc_cond(core, insn))
    {
        int dst = fields->dst23_27;
        int reg_group = fields->x ? (!s) & 0x1 : s;
        int32_t src1 = ((int32_t) core->gpr[s][fields->src13_17]) & 0xFFFF;

        src1 = EXTEND_SIGN(src1, 16);
        int32_t src2 = (int32_t) core->gpr[reg_group][fields->src18_22];
        int64_t result = (int64_t) src1 * (int64_t) src2;

        REGISTER_REGW_EVENT_L(core, (uint32_t) (result & 0xFFFFFFFF), s * GPR_NUM_IN_GROUP + dst, MPYLI_DELAY_SLOT);
        REGISTER_REGW_EVENT_H(core, (uint32_t) (result >> 32 & 0xFFFFFFFF),
                              s * GPR_NUM_IN_GROUP + dst + 1, MPYLI_DELAY_SLOT);
    }
    core->pc += 4;
    return 0;
}
static int exec_mpylir(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t dst_e = (((int64_t) slow16(core->gpr[s][src1]) * (int64_t) core->gpr[x][src2] + 0x4000) >> 15);

        REGISTER_REGW_EVENT_L(core, dst_e, dst, MPYIH_DELAY_SLOT);
    }
    core->pc += 4;
    return 0;
}
static int exec_mpylshu(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;
        int16_t op1 = core->gpr[s][src1] & 0xFFFF;
        uint16_t op2 = core->gpr[reg_group][src2] >> 16;
        int result = op1 * op2;

        REGISTER_REGW_EVENT_L(core, result & 0xFFFFFFFF, s * GPR_NUM_IN_GROUP + dst, MPYLHU_DELAY_SLOT);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_mpyluhs(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;
        uint16_t op1 = core->gpr[s][src1] & 0xFFFF;
        int16_t op2 = core->gpr[reg_group][src2] >> 16;
        int result = op1 * op2;

        REGISTER_REGW_EVENT_L(core, result & 0xFFFFFFFF, s * GPR_NUM_IN_GROUP + dst, MPYLHU_DELAY_SLOT);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}

static int exec_mpysp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_M1 << fields->s;
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int reg_group = fields->x ? (!s) & 0x1 : s;
        float *sp_src1 = (float *) &core->gpr[s][src1];
        float *xsp_src2 = (float *) &core->gpr[reg_group][src2];
        float result = (*sp_src1) * (*xsp_src2);
        uint32_t *presult = (uint32_t *) & result;

        REGISTER_REGW_EVENT_L(core, *presult, s * GPR_NUM_IN_GROUP + dst, 3);
    }

    core->pc += 4;
    return 0;
}

static int exec_mpyspdp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_M1 << fields->s;
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int reg_group = fields->x ? (!s) & 0x1 : s;
        float *sp_src1 = (float *) &core->gpr[s][src1];
        double dsp_src1 = (double) *sp_src1;
        GET_DW_DOUBLE(core->gpr[reg_group][src2], core->gpr[reg_group][src2 + 1], xdp) double result = dsp_src1 * xdp;

        CVT_DW_TO_WORDS(result, l, h);
        REGISTER_REGW_EVENT_L(core, l, s * GPR_NUM_IN_GROUP + dst, 5);
        REGISTER_REGW_EVENT_H(core, h, s * GPR_NUM_IN_GROUP + dst + 1, 6);
    }

    core->pc += 4;
    return 0;
}

static int exec_mpysp2dp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_M1 << fields->s;
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int reg_group = fields->x ? (!s) & 0x1 : s;
        float *sp_src1 = (float *) &core->gpr[s][src1];
        float *xsp_src2 = (float *) &core->gpr[reg_group][src2];
        double dsp_src1 = (double) *sp_src1;
        double dxsp_src2 = (double) *xsp_src2;
        double result = dsp_src1 * dxsp_src2;

        CVT_DW_TO_WORDS(result, l, h);
        REGISTER_REGW_EVENT_L(core, l, s * GPR_NUM_IN_GROUP + dst, 3);
        REGISTER_REGW_EVENT_H(core, h, s * GPR_NUM_IN_GROUP + dst + 1, 4);
    }

    core->pc += 4;
    return 0;
}

static int exec_mpysu(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op7_11;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int result;

        if (op == 0x1b)
        {
            int op1 = SIGN_EXTEND(core->gpr[s][src1] & 0xFFFF, 16);

            if (x)
                result = op1 * (core->gpr[(!s) & 0x1][src2] & 0xFFFF);
            else
                result = op1 * (core->gpr[s][src2] & 0xFFFF);
            REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, MPYSU_DELAY_SLOT);
            DBG("In %s, src1=%d, src2=%d\n", __FUNCTION__, src1, src2);
        } else if (op == 0x1e)
        {
            int op1 = SIGN_EXTEND(src1, 5);

            if (x)
                result = op1 * (core->gpr[(!s) & 0x1][src2] & 0xFFFF);
            else
                result = op1 * (core->gpr[s][src2] & 0xFFFF);
            DBG("In %s, src1=%d, src2=%d\n", __FUNCTION__, src1, src2);
            REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, MPYSU_DELAY_SLOT);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}
static int exec_mpysu4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];

        int32_t dst_e = (sbyte0(src1_e) * ubyte0(src2_e)) & 0xffff | ((sbyte1(src1_e) * ubyte1(src2_e)) << 16); // 3 delay slot
        int32_t dst_o = (sbyte2(src1_e) * ubyte2(src2_e)) & 0xffff | ((sbyte3(src1_e) * ubyte3(src2_e)) << 16); // 3 delay slot

        REGISTER_REGW_EVENT_L(core, dst_e, dst, 3);
        REGISTER_REGW_EVENT_H(core, dst_o, dst + 1, 3);
    }
    core->pc += 4;
    return 0;
}
static int exec_mpyu(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        uint32_t result;

        if (x)
            result = (core->gpr[s][src1] & 0xFFFF) * (core->gpr[(!s) & 0x1][src2] & 0xFFFF);
        else
            result = (core->gpr[s][src1] & 0xFFFF) * (core->gpr[s][src2] & 0xFFFF);
        DBG("In %s, src1=%d, src2=%d\n", __FUNCTION__, src1, src2);
        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, MPYU_DELAY_SLOT);
    }
    core->pc += 4;
    return 0;
}

static int exec_mpyu2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? !(s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op7_11;

        if (op == 0x4)
        {
            int32_t dst_e = ulow16((int32_t) core->gpr[s][src1]) * ulow16((int32_t) core->gpr[x][src2]);        // 3 delay slot
            int32_t dst_o = uhigh16((int32_t) core->gpr[s][src1]) * uhigh16((int32_t) core->gpr[x][src2]);      // 3 delay slot

            REGISTER_REGW_EVENT_L(core, dst_e, dst, 3);
            REGISTER_REGW_EVENT_H(core, dst_o, dst + 1, 3);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_mpyu4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src1_o = core->gpr[s][src1 + 1];
        int32_t src2_e = core->gpr[x][src2];
        int32_t src2_o = core->gpr[x][src2 + 1];

        int32_t dst_e = (ubyte0(src1_e) * ubyte0(src2_e)) | ((ubyte1(src1_e) * ubyte1(src2_e)) << 16);  // 3 delay slot
        int32_t dst_o = (ubyte2(src1_e) * ubyte2(src2_e)) | ((ubyte3(src1_e) * ubyte3(src2_e)) << 16);  // 3 delay slot

        REGISTER_REGW_EVENT_L(core, dst_e, dst, 3);
        REGISTER_REGW_EVENT_H(core, dst_o, dst + 1, 3);
    }
    core->pc += 4;
    return 0;
}
static int exec_mpyus(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int s = fields->s;
        int x = fields->x;
        int gpr_group = x ? (!s) & 0x1 : s;
        int dst = fields->dst23_27;

        uint32_t op1 = (core->gpr[s][src1] & 0xFFFF);
        int op2 = EXTEND_SIGN(core->gpr[gpr_group][src2] & 0xFFFF, 16);
        int result = op1 * op2;

        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, MPYUS_DELAY_SLOT);

    }
    core->pc += 4;
    return 0;
}
static int exec_mpyus4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        // mpyus4 = mpysu4
    }
    NOT_IMP_NO_NEED;
    return 0;
}

static int exec_mpy2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t dst_e = slow16(core->gpr[s][src1]) * slow16(core->gpr[x][src2]);
        int32_t dst_o = shigh16(core->gpr[s][src1]) * shigh16(core->gpr[x][src2]);

        REGISTER_REGW_EVENT_L(core, dst_e, dst, 3);
        REGISTER_REGW_EVENT_H(core, dst_o, dst + 1, 3);
    }
    core->pc += 4;
    return 0;
}
static int exec_mpy2ir(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];

        int32_t dst_e, dst_o;

        if ((((src1_e >> 16) & 0xFFFF) == 0x8000) && (src2_e == 0x80000000))
        {
            dst_o = 0x7FFFFFFF;
        } else
        {
            dst_o = sat_jz((int64_t) ((int64_t) shigh16(src1_e) * (int64_t) (src2_e) + (int64_t) 0x4000) >> 15);
        }

        if (((src1_e & 0xFFFF) == 0x8000) && (src2_e == (int32_t) 0x80000000))
        {
            dst_e = 0x7FFFFFFF;
        } else
        {
            dst_e = sat_jz((int64_t) ((int64_t) slow16(src1_e) * (int64_t) (src2_e) + (int64_t) 0x4000) >> 15);
        }

        REGISTER_REGW_EVENT_L(core, dst_e, dst, 3);
        REGISTER_REGW_EVENT_H(core, dst_o, dst + 1, 3);
    }
    core->pc += 4;
    return 0;
}

static int exec_mpy32(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_M1 << fields->s;
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int reg_group = fields->x ? (!s) & 0x1 : s;
        int dst = fields->dst23_27;
        uint32_t src1 = (uint32_t) core->gpr[s][fields->src13_17];
        uint32_t src2 = (uint32_t) core->gpr[reg_group][fields->src18_22];
        int64_t f1 = *(int *) &src1;
        int64_t f2 = *(int *) &src2;
        int64_t result = f1 * f2;

        REGISTER_REGW_EVENT_L(core, result & 0xFFFFFFFF, s * GPR_NUM_IN_GROUP + dst, MPY32_DELAY_SLOT);
    }
    core->pc += 4;
    return 0;
}

static int exec_mpy32_64(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_M1 << fields->s;
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int reg_group = fields->x ? (!s) & 0x1 : s;
        int dst = fields->dst23_27;
        uint32_t src1 = (uint32_t) core->gpr[s][fields->src13_17];
        uint32_t src2 = (uint32_t) core->gpr[reg_group][fields->src18_22];
        int64_t f1 = *(int *) &src1;
        int64_t f2 = *(int *) &src2;
        int64_t result = f1 * f2;

        REGISTER_REGW_EVENT_L(core, result & 0xFFFFFFFF, s * GPR_NUM_IN_GROUP + dst, MPY32_DELAY_SLOT);
        REGISTER_REGW_EVENT_H(core, (result >> 32) & 0xFFFFFFFF, s * GPR_NUM_IN_GROUP + dst + 1, MPY32_DELAY_SLOT);
    }
    core->pc += 4;
    return 0;
}

static int exec_mpy32su(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_M1 << fields->s;
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int reg_group = fields->x ? (!s) & 0x1 : s;
        int dst = fields->dst23_27;
        uint32_t src1 = (uint32_t) core->gpr[s][fields->src13_17];
        uint32_t src2 = (uint32_t) core->gpr[reg_group][fields->src18_22];
        int64_t f1 = *(int *) &src1;
        int64_t f2 = *(unsigned int *) &src2;
        int64_t result = f1 * f2;

        REGISTER_REGW_EVENT_L(core, result & 0xFFFFFFFF, s * GPR_NUM_IN_GROUP + dst, MPY32_DELAY_SLOT);
        REGISTER_REGW_EVENT_H(core, result >> 32, s * GPR_NUM_IN_GROUP + dst + 1, MPY32_DELAY_SLOT);
    }
    core->pc += 4;
    return 0;
}

static int exec_mpy32u(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_M1 << fields->s;
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int reg_group = fields->x ? (!s) & 0x1 : s;
        int dst = fields->dst23_27;
        uint64_t src1 = (uint64_t) core->gpr[s][fields->src13_17];
        uint64_t src2 = (uint64_t) core->gpr[reg_group][fields->src18_22];
        uint64_t result = src1 * src2;

        REGISTER_REGW_EVENT_L(core, result & 0xFFFFFFFF, s * GPR_NUM_IN_GROUP + dst, MPY32_DELAY_SLOT);
        REGISTER_REGW_EVENT_H(core, result >> 32, s * GPR_NUM_IN_GROUP + dst + 1, MPY32_DELAY_SLOT);
    }
    core->pc += 4;
    return 0;
}

static int exec_mpy32us(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_M1 << fields->s;
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int reg_group = fields->x ? (!s) & 0x1 : s;
        int dst = fields->dst23_27;
        uint32_t src1 = (uint32_t) core->gpr[s][fields->src13_17];
        uint32_t src2 = (uint32_t) core->gpr[reg_group][fields->src18_22];
        int64_t f1 = *(unsigned int *) &src1;
        int64_t f2 = *(int *) &src2;
        int64_t result = f1 * f2;

        REGISTER_REGW_EVENT_L(core, result & 0xFFFFFFFF, s * GPR_NUM_IN_GROUP + dst, MPY32_DELAY_SLOT);
        REGISTER_REGW_EVENT_H(core, result >> 32, s * GPR_NUM_IN_GROUP + dst + 1, MPY32_DELAY_SLOT);
    }
    core->pc += 4;
    return 0;
}
static int exec_mv(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op5_11;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;
        uint32_t result_lt;
        long long result_llt;
        uint64_t src2_lt;
        int result_st;
        int result_dt;

        if (op == 0x2)
        {
            result_st = core->gpr[reg_group][src2] & 0xFFFFFFFF;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result_st);
        } else if (op == 0x20)
        {
            src2_lt = core->gpr[s][src2 + 1] << 32;
            result_llt = src2_lt | core->gpr[s][src2];
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result_llt & 0xFFFFFFFF);
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP + 1, result_llt >> 32);
        } else if (op == 0xd)
        {
            result_st = core->gpr[reg_group][src2];
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result_st);
        } else if (op == 0x4a)
        {
            result_dt = core->gpr[s][src2];
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result_dt);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_mvc(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op2_11;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int crlo = fields->crlo18_22;

        if (op == 0xe8)
        {
            /* from register to control */
            int x = fields->x;
            int src = fields->src18_22;
            int crlo = fields->crlo23_27;
            int reg_group = x ? (!s) & 0x1 : s;

            switch (crlo)
            {
                case 0x0:
                    core->amr = core->gpr[reg_group][src];
                    DBG("In %s, src=%d, amr=%x\n", __FUNCTION__, src, core->amr);
                    break;
                case 0x1:
                    core->csr = (core->csr & 0xFFFF0100) | (core->gpr[reg_group][src] & (~0xFFFF0100));
#if 0
                    //GIE bit is same in tsr and csr
                    if (core->pc >= 0x80022014 && core->pc <= 0x80022094)
                        printf("In %s, ########### write CSR at 0x%x,csr=0x%x,value=0x%x\n", __FUNCTION__, core->pc,
                               core->csr, core->gpr[reg_group][src]);
                    if (core->pc >= 0x80021a64 && core->pc <= 0x80021ad8)
                        printf("In %s, write CSR at 0x%x,csr=0x%x,value=0x%x\n", __FUNCTION__, core->pc, core->csr,
                               core->gpr[reg_group][src]);
                    //GIE bit is same in tsr and csr
                    if ((core->tsr & 0x1) != (core->csr & 0x1))
                        printf("In %s, GIE bit is changed!!!write CSR at 0x%x,after wirte csr=0x%x,value=0x%x\n",
                               __FUNCTION__, core->pc, core->csr, core->gpr[reg_group][src]);
#endif
                    core->tsr = (core->csr & 0x1) | (core->tsr & ~0x1);
#if 0
                    if (core->gpr[reg_group][src] & 0x1)
                        printf("In %s, write CSR at 0x%x,csr=0x%x,value=0x%x\n", __FUNCTION__, core->pc, core->csr,
                               core->gpr[reg_group][src]);
#endif
                    DBG("In %s, src=%d, csr=%d\n", __FUNCTION__, src, core->csr);
                    break;
                case 0x2:
                    core->isr = core->gpr[reg_group][src];
                    core->ifr |= core->isr;
                    DBG("In %s, src=%d, isr=%d\n", __FUNCTION__, src, core->isr);
                    break;
                case 0x3:
                    core->icr = core->gpr[reg_group][src];
                    DBG("In %s, src=%d, icr=%d\n", __FUNCTION__, src, core->icr);
                    break;
                case 0x4:
                    core->ier = core->gpr[reg_group][src];
                    DBG("In %s, src=%d, ier=%d\n", __FUNCTION__, src, core->ier);
                    break;
                case 0x5:
                    core->istp = core->gpr[reg_group][src];
                    DBG("In %s, src=%d, istp=%d\n", __FUNCTION__, src, core->istp);
                    break;
                case 0x6:
                    core->irp = core->gpr[reg_group][src];
                    DBG("In %s, src=%d, irp=%d\n", __FUNCTION__, src, core->irp);
                    break;
                case 0x7:
                    core->nrp = core->gpr[reg_group][src];
                    DBG("In %s, src=%d, nrp=%d\n", __FUNCTION__, src, core->nrp);
                    break;
                case 0xa:
                    //enable counting from 0
                    core->tscl = 0;
                    DBG("In %s, src=%d, tscl=%d\n", __FUNCTION__, src, core->tscl);
                    break;
                case 0xb:
                    core->tsch = core->gpr[reg_group][src];
                    DBG("In %s, src=%d, tsch=%d\n", __FUNCTION__, src);
                    break;
                case 0xd:
                    core->ilc = core->gpr[reg_group][src];
                    DBG("In %s, src=%d, ilc=%d\n", __FUNCTION__, src, core->ilc);
                    break;
                case 0xe:
                    core->rilc = core->gpr[reg_group][src];
                    DBG("In %s, src=%d, rilc=%d\n", __FUNCTION__, src, core->rilc);
                    break;
                case 0x11:
                    core->dnum = core->gpr[reg_group][src];
                    DBG("In %s, src=%d, dnum=%d\n", __FUNCTION__, src, core->dnum);
                    break;
                case 0x12:
                    core->fadcr = core->gpr[reg_group][src];
                    DBG("In %s, src=%d, fadcr=%d\n", __FUNCTION__, src, core->fadcr);
                    break;
                case 0x14:
                    core->fmcr = core->gpr[reg_group][src];
                    DBG("In %s, src=%d, fmcr=%d\n", __FUNCTION__, src, core->fmcr);
                    break;
                case 0x15:
                    core->ssr = (core->ssr & 0xFFFFFFC0) | (core->gpr[reg_group][src] & (~0xFFFFFFC0));
                    DBG("In %s, src=%d, fmcr=%d\n", __FUNCTION__, src, core->fmcr);
                    break;
                case 0x16:
                    core->gplya = core->gpr[reg_group][src];
                    DBG("In %s, src=%d, gplya=%d\n", __FUNCTION__, src, core->gplya);
                    break;
                case 0x17:
                    core->gplyb = core->gpr[reg_group][src];
                    DBG("In %s, src=%d, gplya=%d\n", __FUNCTION__, src, core->gplya);
                    break;
                case 0x18:
                    core->gfpgfr = core->gpr[reg_group][src];
                    DBG("In %s, src=%d, gfpgfr=%d\n", __FUNCTION__, src, core->gfpgfr);
                    break;
                case 0x19:
                    core->dier = core->gpr[reg_group][src];
                    DBG("In %s, src=%d, dier=%d\n", __FUNCTION__, src, core->dier);
                    break;
                case 0x1a:
                    core->tsr = core->gpr[reg_group][src];
                    //GIE bit is same in tsr and csr
                    core->csr = (core->tsr & 0x1) | (core->csr & ~0x1);
#if 0
                    if (core->csr && 0x1)
                        printf("In %s, write CSR by tsr at 0x%x,csr=0x%x\n", __FUNCTION__, core->pc, core->csr);
#endif
                    DBG("In %s, src=%d, tsr=%d\n", __FUNCTION__, src, core->tsr);
                    break;
                case 0x1b:
                    core->itsr = core->gpr[reg_group][src];
                    DBG("In %s, src=%d, itsr=%d\n", __FUNCTION__, src, core->itsr);
                    break;
                case 0x1c:
                    core->ntsr = core->gpr[reg_group][src];
                    DBG("In %s, src=%d, btsr=%d\n", __FUNCTION__, src, core->ntsr);
                    break;
                case 0x1d:
                    core->ecr = core->gpr[reg_group][src];
                    //core->tsr &= ~core->ecr;
                    DBG("In %s, src=%d, ecr=%d\n", __FUNCTION__, src, core->ecr);
                    break;
                default:
                    DBG("In %s, crlo=0x%x, insn=0x%x\n", __FUNCTION__, crlo, insn);
                    NOT_IMP;
            }
        } else if (op == 0xf8)
        {
            /* from control register to general register */
            int src = fields->src18_22;
            int dst = fields->dst23_27;
            int x = fields->x;
            int reg_group = x ? (!s) & 0x1 : s;

            switch (src)
            {
                case 0x0:
                    core->gpr[reg_group][dst] = core->amr;
                    DBG("In %s, src=%d, amr=%d\n", __FUNCTION__, src, core->amr);
                    break;

                case 0x1:
                    core->gpr[reg_group][dst] = core->csr;
                    DBG("In %s, src=%d, csr=%d\n", __FUNCTION__, src, core->csr);
                    break;
                case 0x2:
                    core->gpr[reg_group][dst] = core->isr;
                    DBG("In %s, src=%d, isr=%d\n", __FUNCTION__, src, core->isr);
                    break;
                case 0x3:
                    core->gpr[reg_group][dst] = core->icr;
                    DBG("In %s, src=%d, icr=%d\n", __FUNCTION__, src, core->icr);
                    break;
                case 0x4:
                    core->gpr[reg_group][dst] = core->ier;
                    DBG("In %s, src=%d, ier=%d\n", __FUNCTION__, src, core->ier);
                    break;
                case 0x5:
                    core->gpr[reg_group][dst] = core->istp;
                    DBG("In %s, src=%d, istp=%d\n", __FUNCTION__, src, core->istp);
                    break;
                case 0x6:
                    core->gpr[reg_group][dst] = core->irp;
                    DBG("In %s, src=%d, irp=%d\n", __FUNCTION__, src, core->irp);
                    break;
                case 0x7:
                    core->gpr[reg_group][dst] = core->nrp;
                    DBG("In %s, src=%d, nrp=%d\n", __FUNCTION__, src, core->nrp);
                    break;
                case 0xa:
                    core->gpr[reg_group][dst] = core->tscl;
                    DBG("In %s, src=%d, tscl=%d\n", __FUNCTION__, src, core->tscl);
                    break;
                case 0xb:
                    core->gpr[reg_group][dst] = core->tsch;
                    DBG("In %s, src=%d, tsch=%d\n", __FUNCTION__, src, core->tsch);
                    break;

                case 0xd:
                    core->gpr[reg_group][dst] = core->ilc;
                    DBG("In %s, src=%d, ilc=%d\n", __FUNCTION__, src, core->ilc);
                    break;
                case 0xe:
                    core->gpr[reg_group][dst] = core->rilc;
                    DBG("In %s, src=%d, rilc=%d\n", __FUNCTION__, src, core->rilc);
                    break;
                case 0x11:
                    core->gpr[reg_group][dst] = core->dnum;
                    DBG("In %s, src=%d, dnum=%d\n", __FUNCTION__, src, core->dnum);
                    break;
                case 0x12:
                    core->gpr[reg_group][dst] = core->fadcr;
                    DBG("In %s, src=%d, fadcr=%d\n", __FUNCTION__, src, core->fadcr);
                    break;
                case 0x14:
                    core->gpr[reg_group][dst] = core->fmcr;
                    DBG("In %s, src=%d, fmcr=%d\n", __FUNCTION__, src, core->fmcr);
                    break;
                case 0x15:
                    core->gpr[reg_group][dst] = core->ssr;
                    DBG("In %s, src=%d, fmcr=%d\n", __FUNCTION__, src, core->fmcr);
                    break;
                case 0x16:
                    core->gpr[reg_group][dst] = core->gplya;
                    DBG("In %s, src=%d, gplya=%d\n", __FUNCTION__, src, core->gplya);
                    break;
                case 0x17:
                    core->gpr[reg_group][dst] = core->gplyb;
                    DBG("In %s, src=%d, gplyb=%d\n", __FUNCTION__, src, core->gplyb);
                    break;
                case 0x18:
                    core->gpr[reg_group][dst] = core->gfpgfr;
                    DBG("In %s, src=%d, gfpgfr=%d\n", __FUNCTION__, src, core->gfpgfr);
                    break;
                case 0x19:
                    core->gpr[reg_group][dst] = core->dier;
                    DBG("In %s, src=%d, dier=%d\n", __FUNCTION__, src, core->dier);
                    break;
                case 0x1a:
                    core->gpr[reg_group][dst] = core->tsr;
                    DBG("In %s, src=%d, tsr=%d\n", __FUNCTION__, src, core->tsr);
                    break;
                case 0x1b:
                    core->gpr[reg_group][dst] = core->itsr;
                    DBG("In %s, src=%d, itsr=%d\n", __FUNCTION__, src, core->itsr);
                    break;
                case 0x1c:
                    core->gpr[reg_group][dst] = core->ntsr;     //ntsr == ierr
                    DBG("In %s, src=%d, ntsr=%d\n", __FUNCTION__, src, core->ntsr);
                    break;
                case 0x1d:
                    core->gpr[reg_group][dst] = core->ecr;
                    DBG("In %s, src=%d, ecr=%d\n", __FUNCTION__, src, core->ecr);
                    break;
                default:
                    DBG("In %s, dst=0x%x, insn=0x%x\n", __FUNCTION__, dst, insn);
                    NOT_IMP;
            }

        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_mvd(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int dst = fields->dst23_27;
        int src2 = fields->src18_22;
        int s = fields->s;
        int gpr_group = fields->x ? (!s) & 0x1 : s;
        int result = core->gpr[gpr_group][src2];

        //      REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, MVD_DELAY_SLOT);
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    core->pc += 4;
    return 0;
}

static int exec_mvk_s(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int dst = fields->dst23_27;
        int cst16 = fields->cst7_22;
        int s = fields->s;
        uint32_t result;

        if (cst16 & 0x8000)
            result = cst16 | 0xFFFF0000;
        else
            result = cst16;
        /* write the result to WB buffer */
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    } else
    {
        //NOT_IMP;
    }
    core->pc += 4;
    return 0;
}
static int exec_mvk_l(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int dst = fields->dst23_27;
        int scst5 = fields->scst18_22;
        int s = fields->s;
        int x = fields->x;

        scst5 = (scst5 & 0x10) ? (scst5 | 0xFFFFFFE0) : scst5;
        int result = scst5;

        /* write the result to WB buffer */
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    core->pc += 4;
    return 0;
}
static int exec_mvk_d(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int dst = fields->dst23_27;
        int scst5 = fields->src13_17;
        int s = fields->s;

        scst5 = EXTEND_SIGN(scst5, 5);
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, scst5);
    }
    core->pc += 4;
    return 0;
}
static int exec_mvkh(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int dst = fields->dst23_27;
        int cst16 = fields->cst7_22;
        int s = fields->s;
        int result = (cst16 << 16) | (core->gpr[s][dst] & 0xFFFF);

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        DBG("In %s, dst=%d\n", __FUNCTION__, dst);
    }
    core->pc += 4;
    return 0;
}
static int exec_mvklh(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int dst = fields->dst23_27;
        int s = fields->s;
        uint16_t cst16 = fields->cst7_22;
        int result = (cst16 << 16) | (core->gpr[s][dst] & 0xFFFF);

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}

static int exec_neg(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op1 = fields->op2_11;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int result;

        if (op1 == 0x168)
        {                               // .S unit 
            if (x)
                result = 0 - (int) core->gpr[(!s) & 0x1][src2];
            else
                result = 0 - (int) core->gpr[s][src2];
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else if (op1 == 0x36)
        {
            if (x)
                result = 0 - (int) core->gpr[(!s) & 0x1][src2];
            else
                result = 0 - (int) core->gpr[s][src2];
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else if (op1 == 0x126)
        {
            int64_t tmp = ((int64_t) core->gpr[s][src2 + 1] & 0xFF) << 32;

            tmp = EXTEND_SIGN(tmp, 40);
            tmp |= core->gpr[s][src2];
            tmp = -tmp;
            tmp &= 0xFFFFFFFFFFLL;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP + 1, tmp >> 32);
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, tmp & 0xFFFFFFFF);
            //NEW_IMP;
        } else
        {
            DBG("In %s, op1=0x%x\n", __FUNCTION__, op1);
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}
static int exec_nop(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int src = fields->src13_16;

    core->pc += 4;
    CYCLE_INC(core, src);

    return 0;
}
static int exec_norm(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op5_11;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int gpr_group = x ? (!s) & 0x1 : s;
        int32_t normee = (int32_t) core->gpr[gpr_group][src2];

        if (op == 0x63)
        {                               // .L unit
            int number = 0;

            if (normee < 0)
            {
                while (normee < 0)
                {
                    number++;
                    normee <<= 1;
                }
            } else
            {
                while ((core->gpr[gpr_group][src2] >> (31 - number)) == 0)
                {
                    number++;
                    if (number == 32)
                    {
                        break;
                    }
                }
            }
            uint32_t result = number - 1;

            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else if (op == 0x60)
        {
            uint64_t ulong_src2 = ((int64_t) (core->gpr[s][src2 + 1] & 0xFF)) << 32;

            ulong_src2 |= core->gpr[s][src2];
            int number = 0;

            if (normee < 0)
            {
                while (normee < 0)
                {
                    number++;
                    normee <<= 1;
                }
            } else
            {
                while ((ulong_src2 >> (39 - number)) == 0)
                {
                    number++;
                    if (number == 40)
                    {
                        break;
                    }
                }
            }
            uint32_t result = number - 1;

            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
            //NOT_IMP;
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}
static int exec_not(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;
        int op = fields->op5_11;
        uint32_t result;

        if (op == 0x6e || op == 0x15)
        {
            result = -1 ^ (uint32_t) core->gpr[reg_group][src2];
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_or_d(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_D1 << fields->s;
    if (calc_cond(core, insn))
    {
        int op = fields->op6_9;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        uint32_t result;
        int reg_group = fields->x ? (!s) & 0x1 : s;

        if (op == 3)
        {
            src1 = SIGN_EXTEND(src1, 5);
            result = src1 | core->gpr[reg_group][src2];
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else if (op == 2)
        {
            result = core->gpr[s][src1] | core->gpr[reg_group][src2];
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}
static int exec_or_l(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op5_11;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;

        fields->exec_unit = UNIT_L1 << s;
        int x = fields->x ? (!s) & 0x1 : s;
        uint32_t result;

        if (op == 0x7f)
        {
            if (OPSIZE_IS_DW(insn))
            {
                GET_DW_UINTEGER(core->gpr[s][src1], core->gpr[s][src1 + 1], op1);
                GET_DW_UINTEGER(core->gpr[x][src2], core->gpr[x][src2 + 1], op2);
                op1 |= op2;
                CVT_DW_TO_WORDS(op1, l, h);
                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, l);
                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP + 1, h);
            } else
            {
                result = core->gpr[s][src1] | core->gpr[x][src2];
                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
            }
        } else if (op == 0x7e)
        {
            src1 = (src1 & 0x10) ? (src1 | 0xFFFFFFe0) : src1;
            result = src1 | core->gpr[x][src2];
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_or_s(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op6_11;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;
        uint32_t result;

        if (op == 0x1a)
        {
            src1 = SIGN_EXTEND(src1, 5);
            if (x)
                result = src1 | core->gpr[(!s) & 0x1][src2];
            else
                result = src1 | core->gpr[s][src2];
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);

            DBG("In %s, s=%d, dst=%d, src1=%d, src2=%d\n", __FUNCTION__, s, dst, src1, core->gpr[GPR_A][src2]);
        } else if (op == 0x1b)
        {
            if (OPSIZE_IS_DW(insn))
            {
                GET_DW_UINTEGER(core->gpr[s][src1], core->gpr[s][src1 + 1], op1);
                GET_DW_UINTEGER(core->gpr[reg_group][src2], core->gpr[reg_group][src2 + 1], op2);
                op1 |= op2;
                CVT_DW_TO_WORDS(op1, l, h);
                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, l);
                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP + 1, h);
            } else
            {
                if (x)
                    result = core->gpr[s][src1] | core->gpr[(!s) & 0x1][src2];
                else
                    result = core->gpr[s][src1] | core->gpr[s][src2];
                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);

                DBG("In %s, s=%d, dst=%d, src1=%d, src2=%d\n", __FUNCTION__, s, dst, src1, core->gpr[GPR_A][src2]);
            }
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_pack2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int result;

        if (x)
        {
            result = (core->gpr[(~s) & 0x1][src2] & 0xFFFF) | ((core->gpr[s][src1] & 0xFFFF) << 16);
        } else
        {
            result = (core->gpr[s][src2] & 0xFFFF) | ((core->gpr[s][src1] & 0xFFFF) << 16);
        }
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    core->pc += 4;
    return 0;
}
static int exec_packh2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];

        int32_t dst_e = (shigh16(src1_e) << 16) | (shigh16(src2_e) & 0xFFFF);

        WRITE_BUFFER(core, dst, dst_e);
    }
    core->pc += 4;
    return 0;
}
static int exec_packh4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];

        int32_t dst_e = ((sbyte3(src1_e) & 0xFF) << 24) |
            ((sbyte1(src1_e) & 0xFF) << 16) | ((sbyte3(src2_e) & 0xFF) << 8) | ((sbyte1(src2_e) & 0xFF));

        WRITE_BUFFER(core, dst, dst_e);
    }
    core->pc += 4;
    return 0;
}
static int exec_packhl2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];

        int32_t dst_e = (src1_e) & 0xffff0000 | (src2_e) & 0x0000ffff;

        WRITE_BUFFER(core, dst, dst_e);
    }
    core->pc += 4;
    return 0;
}
static int exec_packlh2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int src1_v, src2_v, result;

        if (fields->op5_11 == 0x1b)
            fields->exec_unit = UNIT_L1 << fields->s;
        else if (fields->op5_11 == 0x21)
            fields->exec_unit = UNIT_S1 << fields->s;
        else
        {
            NOT_IMP_NO_NEED;
        }

        if (fields->x)
        {
            src1_v = core->gpr[s][src1];
            src2_v = core->gpr[~s & 0x1][src2];
            result = MSBn(uint32_t, src2_v, 16) | LSBn(uint32_t, src1_v, 16) << 16;
        } else
        {
            src1_v = core->gpr[s][src1];
            src2_v = core->gpr[s][src2];
            result = MSBn(uint32_t, src2_v, 16) | LSBn(uint32_t, src1_v, 16) << 16;
        }

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_packl4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int upper_half = ((core->gpr[s][src1] << 8) & 0xFF000000) | ((core->gpr[s][src1] << 16) & 0x00FF0000);
        int lower_half;
        int result;

        if (x)
            lower_half = ((core->gpr[(~s) & 0x1][src2] >> 8) & 0xFF00) | (core->gpr[(~s) & 0x1][src2] & 0xFF);
        else
            lower_half = ((core->gpr[s][src2] >> 8) & 0xFF00) | (core->gpr[s][src2] & 0xFF);
        result = upper_half | lower_half;
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    core->pc += 4;
    return 0;
}

static int exec_qmpy32(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op7_11;

        if (op == 0x10)
        {
            uint32_t dst_0 = core->gpr[s][src1] * core->gpr[x][src2];
            uint32_t dst_1 = core->gpr[s][src1 + 1] * core->gpr[x][src2 + 1];
            uint32_t dst_2 = core->gpr[s][src1 + 2] * core->gpr[x][src2 + 2];
            uint32_t dst_3 = core->gpr[s][src1 + 3] * core->gpr[x][src2 + 3];

            REGISTER_REGW_EVENT_L(core, dst_0, dst, 3);
            REGISTER_REGW_EVENT_H(core, dst_1, dst + 1, 3);
            REGISTER_REGW_EVENT_L(core, dst_2, dst + 2, 3);
            REGISTER_REGW_EVENT_H(core, dst_3, dst + 3, 3);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_qmpysp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op7_11;

        if (op == 0x1d)
        {
            int32_t src1_0 = core->gpr[s][src1];
            int32_t src1_1 = core->gpr[s][src1 + 1];
            int32_t src1_2 = core->gpr[s][src1 + 2];
            int32_t src1_3 = core->gpr[s][src1 + 3];
            int32_t src2_0 = core->gpr[x][src2];
            int32_t src2_1 = core->gpr[x][src2 + 1];
            int32_t src2_2 = core->gpr[x][src2 + 2];
            int32_t src2_3 = core->gpr[x][src2 + 3];

            int32_t dst_0 = mpy_sp(src1_0, src2_0);
            int32_t dst_1 = mpy_sp(src1_1, src2_1);
            int32_t dst_2 = mpy_sp(src1_2, src2_2);
            int32_t dst_3 = mpy_sp(src1_3, src2_3);

            REGISTER_REGW_EVENT_L(core, dst_0, dst, 3);
            REGISTER_REGW_EVENT_H(core, dst_1, dst + 1, 3);
            REGISTER_REGW_EVENT_L(core, dst_2, dst + 2, 3);
            REGISTER_REGW_EVENT_H(core, dst_3, dst + 3, 3);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static int32_t smpy32r1(int32_t a, int32_t b)
{
    uint32_t temp_high, temp_low;
    int64_t temp2;
    int32_t result;
    int64_t a64, b64, c64;

    // preliminary change to match the yet to be released function
    if (a == 0x80000000 && b == 0x80000000)
    {
        // special case - due to precision cannot produce result - spec says sat to 0x7fffffff
        // only case of possible sat
        result = 0x7fffffff;
    } else
    {
        a64 = (int64_t) a;              // NOTE:  We ARE expecting the 32-bit signed operand to be sign extended to 64-bits!
        b64 = (int64_t) b;
        c64 = a64 * b64;

        temp_low = c64 & 0xFFFFFFFF;
        temp_high = c64 >> 32;

        temp2 = ((((int64_t) temp_high) << 32) | (temp_low & 0xffffffff)) + ((int64_t) 0x40000000);
        result = temp2 >> 31;
    }

    return result;
}

static int exec_qsmpy32r1(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op6_11;

        if (op == 0x1a)
        {
            uint32_t dst_0 = smpy32r1(core->gpr[s][src1], core->gpr[x][src2]);
            uint32_t dst_1 = smpy32r1(core->gpr[s][src1 + 1], core->gpr[x][src2 + 1]);
            uint32_t dst_2 = smpy32r1(core->gpr[s][src1 + 2], core->gpr[x][src2 + 2]);
            uint32_t dst_3 = smpy32r1(core->gpr[s][src1 + 3], core->gpr[x][src2 + 3]);

            REGISTER_REGW_EVENT_L(core, dst_0, dst, 3);
            REGISTER_REGW_EVENT_H(core, dst_1, dst + 1, 3);
            REGISTER_REGW_EVENT_L(core, dst_2, dst + 2, 3);
            REGISTER_REGW_EVENT_H(core, dst_3, dst + 3, 3);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}

static dword rcp_dp(dword op)
{
    dword result;
    int sign, exp;
    dword man;
    unsigned index;

    if (snan_chk_dp(op))
    {
        result.high = 0x7fffffff;
        result.low = 0xffffffff;
        return (result);
    }

    if (qnan_chk_dp(op))
    {
        result.high = 0x7fffffff;
        result.low = 0xffffffff;
        return (result);
    }

    sign = extract_sign(op.high);
    exp = extract_exp_dp(op);

    if (exp == 0)
    {
        result.high = sign ? 0xfff00000 : 0x7ff00000;
        result.low = 0;
        return (result);
    }

    if (inf_chk_dp(op))
    {
        result.high = sign ? 0x80000000 : 0;
        result.low = 0;
        return (result);
    }

    man = extract_man_dp(op);
    if ((exp > 2045) || ((exp == 2045) && ((man.high & 0x000fffff) | man.low)))
    {                                   /* op > 2^1022 ?; 2045 = 1022 + 0x3ff */
        result.high = sign ? 0x80000000 : 0;
        result.low = 0;
        return (result);
    }

    index = (man.high >> 12) & 0x000000ff;
    exp = 0x7fd - exp;
    exp += !index;
    result.high = rcp(index) << 12;
    result.high |= (exp << 20) & 0x7ff00000;
    result.high |= sign << 31;
    result.low = 0;

    return (result);
}

static int exec_rcpdp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_S1 << fields->s;
    if (calc_cond(core, insn))
    {
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;

        //uint64_t *dpi;
        dword dwop;
        dword dwdst;
        double result;

        GET_DW_DOUBLE(core->gpr[s][src2 - 1], core->gpr[s][src2], xdp);

        DOUBLE64_TO_DWORD(xdp, dwop);
        dwdst = rcp_dp(dwop);
        DWORD_TO_DOUBLE64(dwdst, result);

        /*
         * double dp = 1.0 / xdp;
         * 
         * dpi = (uint64_t*)&dp;
         * *dpi &= 0xFFFFF00000000000LL;
         */

        CVT_DW_TO_WORDS(result, l, h);
        REGISTER_REGW_EVENT_L(core, l, s * GPR_NUM_IN_GROUP + dst, RCPDP_DELAY_SLOT - 1);
        REGISTER_REGW_EVENT_H(core, h, s * GPR_NUM_IN_GROUP + dst + 1, RCPDP_DELAY_SLOT);
    }
    core->pc += 4;
    return 0;
}

static int exec_rcpsp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_S1 << fields->s;
    if (calc_cond(core, insn))
    {
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int gpr_group = fields->x ? (!s) & 0x1 : s;
        uint32_t *spi;

        float src_fp = *(float *) &core->gpr[gpr_group][src2];
        float res_fp = 1.0 / src_fp;

        spi = (uint32_t *) & res_fp;
        //*spi &= 0xFFFF8000;

        REGISTER_REGW_EVENT_L(core, *spi, s * GPR_NUM_IN_GROUP + dst, 0);
    }
    core->pc += 4;
    return 0;
}

static int exec_rint(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    CSR_SET_GIE(core, TSR_GET_SGIE(core));
    TSR_SET_GIE(core, TSR_GET_SGIE(core));
    TSR_SET_SGIE(core, 0);

    core->pc += 4;
    return 0;
}
static int exec_rotl(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int ucst5 = fields->src13_17;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op6_11;

        uint32_t src1_op = core->gpr[s][src1];
        uint32_t src2_op = core->gpr[x][src2];
        int32_t shift_count = 0;

        if (op == 0x1e)
        {
            shift_count = ucst5;
        } else if (op == 0x1d)
        {
            shift_count = src1_op % 32;
        } else
        {
            NOT_IMP_NO_NEED;
        }

        int32_t result = (src2_op << shift_count) | (src2_op >> (32 - shift_count));

        REGISTER_REGW_EVENT_L(core, result, dst, 1);
    }
    core->pc += 4;
    return 0;
}
static int exec_rpack2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int sat;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];

        int32_t upper = (shigh16(sat_jz((int64_t) ((int64_t) src1_e << 1))));

        sat = sat_check32((int64_t) ((int64_t) src1_e << 1));
        int32_t lower = (shigh16(sat_jz((int64_t) ((int64_t) src2_e << 1))));

        sat |= sat_check32((int64_t) ((int64_t) src2_e << 1));

        int32_t result = (upper << 16) | lower;

        if (sat)
        {
            core->csr |= (0x1 << 9);
            core->ssr |= (0x1 << 3);
        }

        WRITE_BUFFER(core, dst, result);
    }
    core->pc += 4;
    return 0;
}
static dword rcpsq_dp(dword op)
{
    dword result;
    int sign, exp, result_exp;
    dword man;
    unsigned index;

    if (snan_chk_dp(op))
    {
        result.high = 0x7fffffff;
        result.low = 0xffffffff;
        return (result);
    }

    if (qnan_chk_dp(op))
    {
        result.high = 0x7fffffff;
        result.low = 0xffffffff;
        return (result);
    }

    sign = extract_sign(op.high);
    exp = extract_exp_dp(op);
    if (exp == 0)
    {
        result.high = sign ? 0xfff00000 : 0x7ff00000;
        result.low = 0;
        return (result);
    }

    if (sign)
    {
        result.high = 0x7fffffff;
        result.low = 0xffffffff;
        return (result);
    }

    if (inf_chk_dp(op))
    {
        result.high = 0;
        result.low = 0;
        return (result);
    }

    index = (op.high >> 12) & 0x000001ff;
    result_exp = exp;
    if (exp & 1)
        result_exp++;
    result_exp >>= 1;
    result_exp += 511;
    result_exp = 0x7fd - result_exp;
    result_exp += (index == 256);
    result.high = rcpsq(index) << 12;
    result.high |= (result_exp << 20) & 0x7ff00000;
    result.high |= sign << 31;
    result.low = 0;

    return (result);
}

static int exec_rsqrdp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_S1 << fields->s;
    if (calc_cond(core, insn))
    {
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;

        dword dwop;
        dword dwdst;
        double result;

        GET_DW_DOUBLE(core->gpr[s][src2 - 1], core->gpr[s][src2], xdp);
        DOUBLE64_TO_DWORD(xdp, dwop);

        dwdst = rcpsq_dp(dwop);

        DWORD_TO_DOUBLE64(dwdst, result);

        CVT_DW_TO_WORDS(result, l, h);
        REGISTER_REGW_EVENT_L(core, l, s * GPR_NUM_IN_GROUP + dst, RCPDP_DELAY_SLOT - 1);
        REGISTER_REGW_EVENT_H(core, h, s * GPR_NUM_IN_GROUP + dst + 1, RCPDP_DELAY_SLOT);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}

static int32_t rcpsq_sp(int32_t op)
{
    int32_t sign, exp, man;
    uint32_t index;
    int32_t result, result_exp;

    if (snan_chk_sp(op))
    {
        return (0x7fffffff);
    }
    if (qnan_chk_sp(op))
    {
        return (0x7fffffff);
    }

    sign = extract_sign(op);
    exp = extract_exp_sp(op);
    if (exp == 0)
    {
        return (sign ? 0xff800000 : 0x7f800000);
    }
    if (sign)
    {
        return (0x7fffffff);
    }
    if (inf_chk_sp(op))
        return (0);

    index = (op >> 15) & 0x000001ff;
    result_exp = exp;
    if (exp & 1)
        result_exp++;
    result_exp >>= 1;
    result_exp += 63;
    result_exp = 0xfd - result_exp;
    result_exp += (index == 256);
    result = rcpsq(index) << 15;
    result |= (sign << 31) | (result_exp << 23);

    return (result);
}

static int exec_rsqrsp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t result = rcpsq_sp(core->gpr[x][src2]);

        WRITE_BUFFER(core, dst, result);
    }
    core->pc += 4;
    return 0;
}

static int exec_sadd(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int scst5 = EXTEND_SIGN(src1, 5);
        int reg_group = x ? (!s) & 0x1 : s;
        int op = fields->op5_11;
        int pwr_ps = (int32_t) 2 << 31;
        int pwr_ng = -(int32_t) 2 << 31;
        int64_t pwr_lps = (int32_t) 2 << 39;
        int64_t pwr_lng = -(int32_t) 2 << 39;
        int result_st;
        int64_t result_sl;

        if (op == 0x13)
        {
            result_st = core->gpr[s][src1] + core->gpr[reg_group][src2];
        } else if (op == 0x31)
        {
            result_sl = core->gpr[s][src1];
            result_sl += (int64_t)core->gpr[reg_group][src2] | ((int64_t)core->gpr[reg_group][src2 + 1]) << 32;
        } else if (op == 0x12)
        {
            result_st = scst5 + core->gpr[reg_group][src2];
        } else if (op == 0x30)
        {
            result_sl = scst5 + ((int64_t)core->gpr[reg_group][src2 + 1] << 32 | (int64_t)core->gpr[reg_group][src2]);
        } else
        {
            NOT_IMP_NO_NEED;
        }

        if (op == 0x13 || op == 0x12)
        {
            if (result_st > (pwr_ps - 1))
            {
                result_st = pwr_ps - 1;
                core->csr |= (0x1 << 9);
            } else if (result_st < pwr_ng)
            {
                result_st = pwr_ng;
                core->csr |= (0x1 << 9);
            }
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result_st);
        } else if (op == 0x31 || op == 0x30)
        {
            if (result_sl > (pwr_lps - 1))
            {
                result_sl = pwr_lps - 1;
                core->csr |= (0x1 << 9);
            } else if (result_sl < pwr_lng)
            {
                result_sl = pwr_lng;
                core->csr |= (0x1 << 9);
            }
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result_sl & 0xFFFFFFFF);
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP + 1, result_sl >> 32);
        }
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_sadd2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];

        int32_t dst_e = sadd2(src1_e, src2_e);

        WRITE_BUFFER(core, dst, dst_e);
    }
    core->pc += 4;
    return 0;
}
static int exec_saddsub(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int sat;
        int32_t dst_o = sat_jz(core->gpr[s][src1] + core->gpr[x][src2]);

        sat = sat_check32(core->gpr[s][src1] + core->gpr[x][src2]);
        int32_t dst_e = sat_jz(core->gpr[s][src1] - core->gpr[x][src2]);

        sat |= sat_check32(core->gpr[s][src1] - core->gpr[x][src2]);

        if (sat)
        {
            core->csr |= (0x1 << 9);
            core->ssr |= s ? (0x1 << 1) : 0x1;
        }

        WRITE_BUFFER(core, dst, dst_e);
        WRITE_BUFFER(core, dst + 1, dst_o);
    }
    core->pc += 4;
    return 0;
}
static int exec_saddsub2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int sat;

        int32_t dst_o = ((sadd2(slow16(core->gpr[s][src1]), slow16(core->gpr[x][src2]))) & 0xFFFF) |
            ((sadd2(shigh16(core->gpr[s][src1]), shigh16(core->gpr[x][src2])) << 16) & 0xFFFF0000);
        int32_t dst_e = ((ssub2_joule(slow16(core->gpr[s][src1]), slow16(core->gpr[x][src2]))) & 0xFFFF) |
            ((ssub2_joule(shigh16(core->gpr[s][src1]), shigh16(core->gpr[x][src2])) << 16) & 0xFFFF0000);

        WRITE_BUFFER(core, dst, dst_e);
        WRITE_BUFFER(core, dst + 1, dst_o);
    }
    core->pc += 4;
    return 0;
}
static int exec_saddsu2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        // saddsu2 = saddus2;
    }
    NOT_IMP_NO_NEED;
    core->pc += 4;
    return 0;
}

static int exec_saddus2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int sat;
        int32_t upper = sat_jz((int64_t) uhigh16(core->gpr[s][src1]) + (int64_t) shigh16(core->gpr[x][src2]));

        sat = sat_check32(uhigh16(core->gpr[s][src1]) + shigh16(core->gpr[x][src2]));
        int32_t lower = sat_jz((int64_t) ulow16(core->gpr[s][src1]) + (int64_t) slow16(core->gpr[x][src2]));

        sat |= sat_check32(ulow16(core->gpr[s][src1]) + slow16(core->gpr[x][src2]));

        lower = lower >= 0xFFFF ? 0xFFFF : lower;
        upper = upper >= 0xFFFF ? 0xFFFF : upper;

        int32_t result = (upper << 16) | lower;

        if (sat)
        {
            core->csr |= (0x1 << 9);
        }
        WRITE_BUFFER(core, dst, result);
    }
    core->pc += 4;
    return 0;
}

static int32_t usat(int32_t a)
{
    int i;

    i = a;
    if (a >= ((int32_t) 0xFF))
        i = 0xFF;
    return i;
}

static int exec_saddu4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t a0 = usat(ubyte0(core->gpr[s][src1]) + ubyte0(core->gpr[x][src2]));
        int32_t a1 = usat(ubyte1(core->gpr[s][src1]) + ubyte1(core->gpr[x][src2]));
        int32_t a2 = usat(ubyte2(core->gpr[s][src1]) + ubyte2(core->gpr[x][src2]));
        int32_t a3 = usat(ubyte3(core->gpr[s][src1]) + ubyte3(core->gpr[x][src2]));

        int32_t result = a3 << 24 | a2 << 16 | a1 << 8 | a0;

        WRITE_BUFFER(core, dst, result);
    }
    core->pc += 4;
    return 0;
}
static int exec_sat(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;

        int64_t tmp;
        int result;
        int64_t pwr_ps = 0xFFFFFFFF;
        int64_t pwr_ng = -(1LL << 32);

        tmp = (((((int64_t) (core->gpr[s][src2 + 1])) << 32) | (((int64_t) core->gpr[s][src2]))) & 0xFFFFFFFFFFLL);
        tmp = EXTEND_SIGN(tmp, 40);

        if (tmp > pwr_ps)
        {
            result = pwr_ps;
            core->csr |= (0x1 << 9);
        } else if (tmp < pwr_ng)
        {
            result = pwr_ng;
            core->csr |= (0x1 << 9);
        } else
            result = (int) (tmp & 0xFFFFFFFF);

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}

static int exec_set(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        uint32_t csta = fields->csta;
        uint32_t cstb = fields->cstb;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        uint32_t result;

        uint32_t v = 0xFFFFFFFF;

        if (cstb >= csta)
        {
            v = 0xFFFFFFFF << csta;
            v &= 0xFFFFFFFF >> (31 - cstb);
        }

        result = core->gpr[s][src2] | v;
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }

    core->pc += 4;
    return 0;
}

static int exec_set_reg_form(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int reg_group = fields->x ? (!s) & 0x1 : s;
        uint32_t src1 = core->gpr[s][fields->src13_17];

        //uint32_t csta =  src1 & 0x3E0; // src1[9..5]
        uint32_t csta = ((src1 >> (5)) & ((1 << (1 + (9) - (5))) - 1)); // src1[9..5]
        uint32_t cstb = src1 & 0x1F;    // src1[4..0]
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        uint32_t result;

        uint32_t v = 0xFFFFFFFF;

        if (cstb >= csta)
        {
            v = 0xFFFFFFFF << csta;
            v &= 0xFFFFFFFF >> (31 - cstb);
        }

        result = core->gpr[reg_group][src2] | v;
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }

    core->pc += 4;
    return 0;
}

static int exec_shfl(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src2_e = core->gpr[x][src2];

#define DBIT(n) ((src2_e >> (n)) & 0x1)
        int i;
        int result = 0x0;

        for (i = 31; i >= 0; i--)
        {
            if (i >= 16)
            {
                // 31 30 29 ... to 31 29 27 ...
                result |= (DBIT(i) << (31 - (31 - i) * 2));
            } else
            {
                // 15 14 13 ... to 30 28 26 ...
                result |= (DBIT(i) << (i * 2));
            }
        }
#undef DBIT

        REGISTER_REGW_EVENT_L(core, result, dst, 1);
    }
    core->pc += 4;
    return 0;
}
static int exec_shfl3(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        uint32_t inp0, inp1, inp2;
        uint64_t result = 0;

        inp0 = core->gpr[x][src2] & 0xFFFF;
        inp1 = core->gpr[s][src1] & 0xFFFF;
        inp2 = (core->gpr[s][src1] >> 16) & 0xFFFF;

        int i;

        for (i = 0; i < 16; i++)
        {
            result |= (uint64_t) ((inp0 >> i) & 0x1) << (i * 3);
            result |= (uint64_t) ((inp1 >> i) & 0x1) << ((i * 3) + 1);
            result |= (uint64_t) ((inp2 >> i) & 0x1) << ((i * 3) + 2);
        }

        WRITE_BUFFER(core, dst, result & 0xFFFFFFFF);
        WRITE_BUFFER(core, dst + 1, (result >> 32) & 0xFFFFFFFF);
    }
    core->pc += 4;
    return 0;
}
static int exec_shl(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op6_11;
        int s = fields->s;

        fields->exec_unit = UNIT_S1 << s;
        int x = fields->x;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        uint32_t result;
        int reg_group = x ? (!s) & 0x1 : s;

        if (op == 0x32)
        {
            result = core->gpr[reg_group][src2] << src1;
            DBG("In %s, src1=0x%x, src2=0x%x\n", __FUNCTION__, src1, src2);
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else if (op == 0x33)
        {
            uint32_t shift = core->gpr[s][src1] & 0x3F;
            int32_t result = (int32_t) core->gpr[reg_group][src2];

            if (shift > 39)
                shift = 40;
            if (shift >= 32)
                result = 0;
            else
                result <<= shift;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else if (op == 0x30)
        {
            int shift;

            shift = src1;
            if (shift > 31)
                shift = 31;
            uint64_t t;

            t = core->gpr[reg_group][src2 + 1];
            t = (t << 32);
            t |= core->gpr[reg_group][src2];
            uint64_t result;

            result = (t << shift);
            result &= 0xFFFFFFFFFFLL;
            WRITE_BUFFER(core, dst + 1 + s * GPR_NUM_IN_GROUP, result >> 32);
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result & 0xFFFFFFFF);
            DBG("In %s, src1=0x%x, src2=0x%x\n", __FUNCTION__, src1, src2);
        } else if (op == 0x31)
        {
            int shift;

            shift = core->gpr[s][src1] & 0x3F;
            if (shift > 39)
                shift = 40;
            uint64_t t;

            t = (core->gpr[reg_group][src2 + 1]);
            t = (t << 32);
            t |= core->gpr[reg_group][src2];
            uint64_t result;

            result = (t << shift);
            result &= 0xFFFFFFFFFFLL;
            WRITE_BUFFER(core, dst + 1 + s * GPR_NUM_IN_GROUP, result >> 32);
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result & 0xFFFFFFFF);
            DBG("In %s, src1=0x%x, src2=0x%x\n", __FUNCTION__, src1, src2);
        } else if (op == 0x13)
        {
            uint32_t shift = core->gpr[s][src1] & 0x3F;

            if (shift > 39)
                shift = 40;
            uint64_t t = core->gpr[reg_group][src2];

            t <<= shift;
            t &= 0xFFFFFFFFFFLL;
            WRITE_BUFFER(core, dst + 1 + s * GPR_NUM_IN_GROUP, t >> 32);
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, t & 0xFFFFFFFF);
            //NEW_IMP;
        } else if (op == 0x12)
        {
            uint64_t result = core->gpr[reg_group][src2];

            result <<= src1;
            result &= 0xFFFFFFFFFFLL;
            WRITE_BUFFER(core, dst + 1 + s * GPR_NUM_IN_GROUP, result >> 32);
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result & 0xFFFFFFFF);
            //NEW_IMP;
        } else
        {
            NOT_IMP_NO_NEED;
        }

    }
    core->pc += 4;
    return 0;
}

static int exec_shl2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int ucst5 = fields->src13_17;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op6_11;

        int32_t src1_op = core->gpr[s][src1];
        int32_t src2_op = core->gpr[x][src2];
        int32_t shift_count = 0;

        if (op == 0x3)
        {
            shift_count = ucst5;
        } else if (op == 0x13)
        {
            shift_count = src1_op;
        } else
        {
            NOT_IMP_NO_NEED;
        }
        int32_t result = shl2(src2_op, shift_count);

        WRITE_BUFFER(core, dst, result);
    }
    core->pc += 4;
    return 0;
}

static int exec_shlmb(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t result = (ubyte2(core->gpr[x][src2]) << 24) |
            (ubyte1(core->gpr[x][src2]) << 16) | (ubyte0(core->gpr[x][src2]) << 8) | (ubyte3(core->gpr[s][src1]));

        WRITE_BUFFER(core, dst, result);
    }
    core->pc += 4;
    return 0;
}
static int exec_shr(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op6_11;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;

        fields->exec_unit = UNIT_S1 << fields->s;
        uint32_t shift = op % 2 ? core->gpr[s][src1] & 0x3F : src1;

        if (shift > 39 && shift < 64)
            shift = 40;

        if (op == 0x36 || op == 0x37)
        {
            int op2;
            int result;

            if (x)
                op2 = core->gpr[(!s) & 0x1][src2];
            else
                op2 = core->gpr[s][src2];

            if (shift >= 32)
                result = 0;
            else
                result = (op2 >> shift);
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else if (op == 0x35 || op == 0x34)
        {
            int64_t op2, op3;

            if (x)
            {
                op2 = core->gpr[(!s) & 0x1][src2];
                op3 = ((int64_t) core->gpr[(!s) & 0x1][src2 + 1]) << 32;
            } else
            {
                op2 = core->gpr[s][src2];
                op3 = ((int64_t) core->gpr[s][src2 + 1]) << 32;
            }

            if (op3 & 0x0000008000000000LL)
                op3 |= 0xFFFFFF0000000000LL;
            op2 |= op3;
            op2 >>= shift;

            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, op2 & 0xFFFFFFFF);
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP + 1, (op2 >> 32) & 0xFFFFFFFF);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}
static int exec_shr2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int ucst5 = fields->src13_17;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op2_11;

        int32_t src1_op = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];
        int32_t shift_count = 0;

        if (op == 0x188)
        {
            shift_count = ucst5;
        } else if (op == 0x37c)
        {
            shift_count = src1_op;
        } else
        {
            NOT_IMP_NO_NEED;
        }

        int32_t dst_e = shr2(src2_e, shift_count);

        WRITE_BUFFER(core, dst, dst_e);
    }
    core->pc += 4;
    return 0;
}
static int exec_shrmb(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t result = (ubyte0(core->gpr[s][src1]) << 24) |
            (ubyte3(core->gpr[x][src2]) << 16) | (ubyte2(core->gpr[x][src2]) << 8) | (ubyte1(core->gpr[x][src2]));

        WRITE_BUFFER(core, dst, result);
    }
    core->pc += 4;
    return 0;
}
static int exec_shru(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op6_11;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;

        if (op == 0x26)
        {
            /* ucst5 */
            uint32_t result;

            src1 = src1 & 0x1F;
            if (x)
                result = core->gpr[(!s) & 0x1][src2] >> src1;
            else
                result = core->gpr[s][src2] >> src1;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);

            DBG("In %s, src1=%d, dst=%d, result=0x%x\n", __FUNCTION__, src1, dst, core->gpr[s][dst]);
        } else if (op == 0x27)
        {
            uint32_t shift = core->gpr[s][src1] & 0x3F;
            int reg_group = fields->x ? (!s) & 0x1 : s;

            if (shift > 39)
                shift = 40;
            uint32_t result;

            if (shift >= 32)
                result = 0;
            else
                result = core->gpr[reg_group][src2] >> shift;

            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else if (op == 0x25)
        {
            uint64_t t;
            int shift = core->gpr[s][src1] & 0x3F;

            if (shift > 39)
                shift = 40;
            t = core->gpr[s][src2 + 1] & 0xFF;
            t = (t << 32);
            t |= core->gpr[s][src2];
            uint64_t result;

            result = t >> shift;
            WRITE_BUFFER(core, dst + 1 + s * GPR_NUM_IN_GROUP, result >> 32);
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result & 0xFFFFFFFF);
        } else if (op == 0x24)
        {
            uint64_t t;
            int shift = src1 & 0x1F;

            t = core->gpr[s][src2 + 1] & 0xFF;
            t = (t << 32);
            t |= core->gpr[s][src2];
            uint64_t result;

            result = t >> shift;
            WRITE_BUFFER(core, dst + 1 + s * GPR_NUM_IN_GROUP, result >> 32);
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result & 0xFFFFFFFF);
        } else
        {
            NOT_IMP_NO_NEED;
        }
    }
    core->pc += 4;
    return 0;
}
static int exec_shru2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int ucst5 = fields->src13_17;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;
        int op = fields->op2_11;

        int32_t src1_op = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];
        int32_t shift_count = 0;

        if (op == 0x198)
        {
            shift_count = ucst5;
        } else if (op == 0x38c)
        {
            shift_count = src1_op;
        }
        int32_t dst_e = shru2(src2_e, shift_count);

        WRITE_BUFFER(core, dst, dst_e);
    }
    core->pc += 4;
    return 0;
}
static int exec_sl(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
    }
    NOT_IMP;
    core->pc += 4;
    return 0;
}
static int exec_smpy(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;

        int result;
        int op1 = core->gpr[s][src1] & 0xFFFF;
        int op2 = core->gpr[reg_group][src2] & 0xFFFF;

        result = (op1 * op2) << 1;

        if (result == 0x80000000)
        {
            result = 0x7FFFFFFF;
            core->csr |= (0x1 << 9);
        }
        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, MPY_DELAY_SLOT);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_smpyh(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;

        int result;
        int op1 = core->gpr[s][src1] >> 16;
        int op2 = core->gpr[reg_group][src2] >> 16;

        result = (op1 * op2) << 1;
        if (result == 0x80000000)
        {
            result = 0x7FFFFFFF;
            core->csr |= (0x1 << 9);
        }
        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, MPY_DELAY_SLOT);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_smpyhl(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;

        int result;
        int op1 = core->gpr[s][src1] >> 16;
        int op2 = core->gpr[reg_group][src2] & 0xFFFF;

        result = (op1 * op2) << 1;
        if (result == 0x80000000)
        {
            result = 0x7FFFFFFF;
            core->csr |= (0x1 << 9);
        }
        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, MPY_DELAY_SLOT);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_smpylh(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;

        int result;
        int op1 = core->gpr[s][src1] & 0xFFFF;
        int op2 = core->gpr[reg_group][src2] >> 16;

        result = (op1 * op2) << 1;
        if (result == 0x80000000)
        {
            result = 0x7FFFFFFF;
            core->csr |= (0x1 << 9);
        }
        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, MPY_DELAY_SLOT);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_smpy2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];

        int32_t dst_e = sat_jz((slow16(src1_e) * slow16(src2_e)) << 1);
        int32_t dst_o = sat_jz((shigh16(src1_e) * shigh16(src2_e)) << 1);

        REGISTER_REGW_EVENT_L(core, dst_e, dst, 3);
        REGISTER_REGW_EVENT_H(core, dst_o, dst + 1, 3);
    }
    core->pc += 4;
    return 0;
}

static int32_t smpy32(int32_t a, int32_t b)
{
    uint32_t temp_high, temp_low;
    int64_t temp2;
    int32_t result;
    int64_t a64, b64, c64;

    // preliminary change to match the yet to be released function
    if (a == 0x80000000 && b == 0x80000000)
    {
        // special case - due to precision cannot produce result - spec says sat to 0x7fffffff
        // only case of possible sat
        result = 0x7fffffff;
    } else
    {
        a64 = (int64_t) a;              // NOTE:  We ARE expecting the 32-bit signed operand to be sign extended to 64-bits!
        b64 = (int64_t) b;
        c64 = (a64 * b64) << 1;

        result = c64 >> 32;
    }
    return result;
}

static int exec_smpy32(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        uint32_t result = smpy32((int32_t) core->gpr[s][src1], (int32_t) core->gpr[x][src2]);

        int sat = sat_check32(core->gpr[s][src1]);

        sat |= sat_check32(core->gpr[x][src2]);

        if (sat)
        {
            core->csr |= (0x1 << 9);
            core->ssr |= (0x1 << 3);
        }

        REGISTER_REGW_EVENT_L(core, result, dst, 3);
    }
    core->pc += 4;
    return 0;
}
static int exec_spack2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];
        int32_t result;
        int sat;

        if (src2_e > 0x00007FFF)
        {
            result = 0x7FFF;
            sat = 0x1;
        } else if (src2_e < 0xFFFF8000)
        {
            result = 0x8000;
            sat |= 0x1;
        } else
        {
            result = slow16(src2_e);
        }

        if (src1_e > 0x00007FFF)
        {
            result |= 0x7FFF0000;
            sat |= 0x1;
        } else if (src1_e < 0xFFFF8000)
        {
            result |= 0x80000000;
            sat |= 0x1;
        } else
        {
            result |= slow16(src1_e) << 16;
        }

        if (sat)
        {
            core->csr |= (0x1 << 9);
        }

        WRITE_BUFFER(core, dst, result);
    }
    core->pc += 4;
    return 0;
}
static int exec_spacku4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];

        int32_t dst_e = spack4(src1_e, src2_e);

        WRITE_BUFFER(core, dst, dst_e);
    }
    core->pc += 4;
    return 0;
}

static int exec_spdp(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_S1 << fields->s;
    if (calc_cond(core, insn))
    {
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int reg_group = fields->x ? (!s) & 0x1 : s;
        float *sp = (float *) &core->gpr[reg_group][src2];
        double dp = (double) *sp;

        CVT_DW_TO_WORDS(dp, l, h);
        REGISTER_REGW_EVENT_L(core, l, s * GPR_NUM_IN_GROUP + dst, 0);
        REGISTER_REGW_EVENT_H(core, h, s * GPR_NUM_IN_GROUP + dst + 1, 1);
    }

    core->pc += 4;
    return 0;
}

static int floatToInt(float f)
{
    int i = 0;

    if (f > 0)                          //正数  
        i = (f * 10 + 5) / 10;
    else if (f < 0)                     //负数  
        i = (f * 10 - 5) / 10;
    else
        i = 0;

    return i;

}
static int exec_spint(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_S1 << fields->s;
    if (calc_cond(core, insn))
    {
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int reg_group = fields->x ? (!s) & 0x1 : s;

        /*
         * float *sp = (float*)&core->gpr[reg_group][src2];
         * //int(8.6) in c is 8  for tic6x  need be 9
         * int result = floatToInt(*sp);
         * REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, 3);
         */
        int32_t round_mode;

        fields->exec_unit = UNIT_L1 << s;
        round_mode = fields->exec_unit == UNIT_L1 ? (((core->fadcr) >> 9) & 0x3) : (((core->fadcr) >> 25) & 0x3);

        int32_t result = spint(core->gpr[reg_group][src2], round_mode);

        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, 3);
    }
    core->pc += 4;
    return 0;
}
static int exec_spkernel(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        core->sploop_end = core->pc;
        int fstg_fcyc = fields->fstg_fcyc;

        if (core->sploop_flag == SPLOOP)
            core->ilc = core->ilc == 0 ? 0 : core->ilc - 1;
        DBG("In %s, ilc=0x%x\n", __FUNCTION__, core->ilc);
    }
    core->pc += 4;
    return 0;
}
static int exec_spkernelr(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
    }
    NOT_IMP;
    return 0;
}
static int exec_sploop(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        core->sploop_ii = fields->ii + 1;
        core->sploop_flag = SPLOOP;
        core->sploop_begin = core->pc;
        if (fields->p)
            core->sploop_begin_paralleled = 1;
        DBG("begin loop at 0x%x, ilc=%d\n", core->pc, core->ilc);
    }
    core->pc += 4;
    return 0;
}
static int exec_sploopd(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        core->sploop_ii = fields->ii + 1;
        core->sploop_flag = SPLOOPD;
        core->sploop_begin = core->pc;
        if (fields->p)
            core->sploop_begin_paralleled = 1;
    }
    core->pc += 4;
    return 0;
}
static int exec_sploopw(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        core->sploop_begin = core->pc;
        if (fields->p)
            core->sploop_begin_paralleled = 1;
        core->sploopw_cond = insn;
        core->sploop_flag = SPLOOPW;
        core->sploop_ii = fields->ii + 1;
        core->ilc = 0;
        DBG("begin loopw at 0x%x, cond=%x\n", core->pc, core->sploopw_cond);
    }
    core->pc += 4;
    return 0;
}
static int exec_spmask(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    core->spmask = insn & 0x1 == 0x0 ? 0x0 : insn >> 18;

    core->spmask_begin = core->pc;
    core->pc += 4;
    return 0;
}
static int exec_spmaskr(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
    }
    NOT_IMP;
    return 0;
}
static int exec_sshl(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int ucst5 = src1;
        int reg_group = x ? (!s) & 0x1 : s;
        int op = fields->op6_11;
        int result;
        uint32_t op1;

        if (op == 0x23)
        {
            op1 = core->gpr[s][src1];
        } else if (op == 0x22)
        {
            op1 = ucst5;
        } else
        {
            NOT_IMP_NO_NEED;
        }

        int op2 = 31 - op1;
        int count0 = 0;
        int count1 = 0;
        int src2_value = core->gpr[reg_group][src2] >> op2;
        int tmp = 0xFFFFFFFF >> op2;

        if (src2_value == 0x0 || src2_value == tmp)
        {
            result = core->gpr[reg_group][src2] << op1;
        } else if (core->gpr[reg_group][src2] > 0)
        {
            result = 0x7FFFFFFF;
            core->csr |= (0x1 << 9);
        } else if (core->gpr[reg_group][src2] < 0)
        {
            result = 0x80000000;
            core->csr |= (0x1 << 9);
        }

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_sshvl(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];
        int32_t dst_e;

        if (src1_e >= 0 && src1_e <= (int32_t) 0x7FFFFFFF)
        {
            dst_e = sat_jz((int64_t) src2_e << src1_e);
        }

        if (src1_e >= (int32_t) 0x80000000 && src1_e < 0)
        {
            dst_e = src2_e >> (-src1_e);
        }

        if (src1_e > (int32_t) 0x7FFFFFFF)
        {
            dst_e = sat_jz((int64_t) src1_e << 31);
        }

        if (src1_e < (int32_t) 0x80000000)
        {
            dst_e = src2_e >> 31;
        }

        REGISTER_REGW_EVENT_L(core, dst_e, dst, 1);
    }
    core->pc += 4;
    return 0;
}
static int exec_sshvr(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];
        int32_t dst_e;

        if (src1_e >= 0 && src1_e <= (int32_t) 0x7FFFFFFF)
        {
            dst_e = src2_e >> src1_e;
        }

        if (src1_e >= (int32_t) 0x80000000 && src1_e < 0)
        {
            dst_e = sat_jz((int64_t) src2_e << (-src1_e));
        }

        if (src1_e > (int32_t) 0x7FFFFFFF)
        {
            dst_e = src2_e >> 31;
        }

        if (src1_e < (int32_t) 0x80000000)
        {
            dst_e = sat_jz((int64_t) src1_e << 31);
        }

        REGISTER_REGW_EVENT_L(core, dst_e, dst, 1);
    }
    core->pc += 4;
    return 0;
}
static int exec_ssub(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int op = fields->op5_11;
        int reg_group = x ? (!s) % 0x1 : s;
        int scst5 = EXTEND_SIGN(src1, 5);
        int result_st;
        int64_t result_sl;
        int pwr_ps = (int32_t) 2 << 31;
        int pwr_ng = -(int32_t) 2 << 31;
        int64_t pwr_lps = (int64_t) 2 << 39;
        int64_t pwr_lng = -(int64_t) 2 << 39;

        if (op == 0xf)
        {
            result_st = core->gpr[s][src1] - core->gpr[reg_group][src2];
        } else if (op == 0x1f)
        {
            result_st = core->gpr[reg_group][src1] - core->gpr[s][src2];
        } else if (op == 0xe)
        {
            result_st = scst5 - core->gpr[reg_group][src2];
        } else if (op == 0x2c)
        {
            int64_t tmp_upper, tmp;

            tmp_upper = ((int64_t) core->gpr[s][src2] & 0xFF) << 32;
            tmp_upper = EXTEND_SIGN(tmp_upper, 40);
            tmp = (int64_t) core->gpr[s][src2 + 1];
            tmp |= tmp_upper;
            result_sl = scst5 - tmp;
        } else
        {
            NOT_IMP_NO_NEED;
        }

        if (op == 0x2c)
        {
            if (result_sl > (pwr_lps - 1))
            {
                result_sl = pwr_lps - 1;
                core->csr |= (0x1 << 9);
            } else if (result_sl < pwr_lng)
            {
                result_sl = pwr_lng;
                core->csr |= (0x1 << 9);
            }

            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result_sl & 0xFFFFFFFF);
            WRITE_BUFFER(core, dst + 1 + s * GPR_NUM_IN_GROUP, result_sl >> 32);
        } else
        {
            if (result_st > (pwr_ps - 1))
            {
                result_st = pwr_ps - 1;
                core->csr |= (0x1 << 9);
            } else if (result_st < pwr_ng)
            {
                result_st = pwr_ng;
                core->csr |= (0x1 << 9);
            }

            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result_st);
        }
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_ssub2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];

        int32_t dst_e = ssub2_joule(src1_e, src2_e);

        WRITE_BUFFER(core, dst, dst_e);
    }
    core->pc += 4;
    return 0;
}
static int exec_stb(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op4_6;
        int y = fields->y;
        int mode = fields->mode;
        int offset = fields->offset;
        int base = fields->base;
        int src = fields->dst23_27;
        int s = fields->s;
        generic_address_t addr = calc_addr(core, base, offset, mode, y, 0);

        lds_save_result(core, fields, core->gpr[s][src] & 0xFF, s * GPR_NUM_IN_GROUP + src, addr, WR_FLAG, 0, 8);
    }
    core->pc += 4;
    return 0;
}
static int exec_stb_15(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int ucst15 = fields->ucst8_22;
        int src = fields->dst23_27;
        int y = fields->y;
        int s = fields->s;
        int base = 14 + y;
        generic_address_t addr = ucst15 + core->gpr[GPR_B][base];

        lds_save_result(core, fields, core->gpr[s][src] & 0xFF, s * GPR_NUM_IN_GROUP + src, addr, WR_FLAG, 0, 8);
    }
    core->pc += 4;
    return 0;
}
static int exec_stdw(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_D1 << fields->s;
    if (calc_cond(core, insn))
    {
        int mode = fields->mode;
        int offset = fields->offset;
        int base = fields->base;
        int src = (fields->dst24_27) << 1;
        int sc = fields->sc;
        int y = fields->y;
        int s = fields->s;
        generic_address_t addr = calc_addr(core, base, offset, mode, y, 3);

        /* sc need to be considered */
        uint64_t result = core->gpr[s][src] | ((uint64_t) core->gpr[s][src + 1] << 32);

        lds_save_result(core, fields, result, s * GPR_NUM_IN_GROUP + src, addr, WR_FLAG, 0, 64);
        /* FIXME, just for inc/dec base twice for dword */
        DBG("In %s, addr=0x%x, src=%d, base=%d\n", __FUNCTION__, addr, src, base);
    }
    core->pc += 4;
    return 0;
}
static int exec_sth(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int mode = fields->mode;
        int offset = fields->offset;
        int base = fields->base;
        int src = fields->dst23_27;
        int y = fields->y;
        int s = fields->s;
        generic_address_t addr = calc_addr(core, base, offset, mode, y, 1);

        lds_save_result(core, fields, core->gpr[s][src] & 0xFFFF, s * GPR_NUM_IN_GROUP + src, addr, WR_FLAG, 0, 16);
    }
    core->pc += 4;
    return 0;
}
static int exec_sth_15(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int ucst15 = fields->ucst8_22;
        int src = (fields->dst23_27);
        int y = fields->y;
        int s = fields->s;
        int base = 14 + y;
        generic_address_t addr = (ucst15 << 1) + core->gpr[GPR_B][base];

        lds_save_result(core, fields, core->gpr[s][src] & 0xFFFF, s * GPR_NUM_IN_GROUP + src, addr, WR_FLAG, 0, 16);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_stndw(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
#define STNDW_ADDR_MOD_SLOT 1
#define STNDW_LOAD_VALUE_SLOT 5
    if (calc_cond(core, insn))
    {
        int mode = fields->mode;
        int offset = fields->offset;
        int base = fields->base;
        int src = (fields->dst24_27) << 1;
        int sc = fields->sc;
        int y = fields->y;
        int s = fields->s;
        generic_address_t addr = calc_addr(core, base, offset, mode, y, sc ? 3 : 0);

        /* sc need to be considered */
        uint64_t result = core->gpr[s][src] | ((uint64_t) core->gpr[s][src + 1] << 32);

        lds_save_result(core, fields, result, s * GPR_NUM_IN_GROUP + src, addr, WR_FLAG, 0, 64);
        /* FIXME, just for inc/dec base twice for dword */
        DBG("In %s, addr=0x%x, a[5]=0x%x\n", __FUNCTION__, addr, core->gpr[s][5]);
    }
    core->pc += 4;
    return 0;
}
static int exec_stnw(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int mode = fields->mode;
        int offset = fields->offset;
        int base = fields->base;
        int src = (fields->dst23_27);
        int y = fields->y;
        int s = fields->s;
        generic_address_t addr = calc_addr(core, base, offset, mode, y, 2);

        DBG("In %s, addr=0x%x\n", __FUNCTION__, addr);
        lds_save_result(core, fields, core->gpr[s][src], s * GPR_NUM_IN_GROUP + src, addr, WR_FLAG, 0, 32);
    }
    core->pc += 4;
    return 0;
}
static int exec_stw(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int mode = fields->mode;
        int offset = fields->offset;
        int base = fields->base;
        int src = (fields->dst23_27);
        int y = fields->y;
        int s = fields->s;
        generic_address_t addr = calc_addr(core, base, offset, mode, y, 2);

        DBG("In %s, addr=0x%x, src=%d, value=0x%x\n", __FUNCTION__, addr, src, core->gpr[s][src]);
        lds_save_result(core, fields, core->gpr[s][src], s * GPR_NUM_IN_GROUP + src, addr, WR_FLAG, 0, 32);
    }
    core->pc += 4;
    return 0;
}
static int exec_stw_15(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int ucst15 = fields->ucst8_22;
        int src = (fields->dst23_27);
        int y = fields->y;
        int s = fields->s;
        int base = 14 + y;
        generic_address_t addr = (ucst15 << 2) + core->gpr[GPR_B][base];

        lds_save_result(core, fields, core->gpr[s][src], 0xFF, addr, WR_FLAG, 0, 32);
    }
    core->pc += 4;
    return 0;
}
static int exec_sub(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op5_11;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = fields->x ? (!s) & 0x1 : s;

        if (op == 0x27)
        {
            uint64_t result;

            fields->exec_unit = UNIT_L1 << fields->s;
            if (x)
            {
                result = core->gpr[s][src1] - core->gpr[(!s) & 0x1][src2];
            } else
            {
                result = core->gpr[s][src1] - core->gpr[s][src2];
            }
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result & 0xFFFFFFFF);
            WRITE_BUFFER(core, dst + 1 + s * GPR_NUM_IN_GROUP, (result >> 32) & 0xFF);
            DBG("In %s, ZERO dst=%d, src1=%d, src2=%d, result=0x%x\n", __FUNCTION__, dst, src1, src2, result);
        } else if (op == 0x7)
        {
            int result;

            fields->exec_unit = UNIT_L1 << fields->s;
            if (x)
            {
                result = core->gpr[s][src1] - core->gpr[(!s) & 0x1][src2];
            } else
            {
                result = core->gpr[s][src1] - core->gpr[s][src2];
            }
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
            DBG("In %s, dst=%d, src1=%d, src2=%d, result=0x%x\n", __FUNCTION__, dst, src1, src2, result);
        } else if (op == 0x6)
        {
            src1 = SIGN_EXTEND(src1, 5);
            int result;
            int op2;

            fields->exec_unit = UNIT_L1 << fields->s;
            if (x)
            {
                op2 = core->gpr[(!s) & 0x1][src2];
            } else
            {
                op2 = core->gpr[s][src2];
            }
            result = src1 - op2;
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
            DBG("In %s, dst=%d, src1=%d, src2=%d, result=0x%x\n", __FUNCTION__, dst, src1, src2, result);

        } else if (op == 0x17)
        {
            int result;

            fields->exec_unit = UNIT_L1 << fields->s;

            if (x)
            {
                /* FIXME, why s is applied to src1 */
                result = (int) core->gpr[(!s) & 0x1][src1] - core->gpr[s][src2];
            } else
            {
                result = (int) core->gpr[s][src1] - core->gpr[s][src2];
            }
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
            DBG("In %s, dst=%d, src1=%d, src2=%d, result=0x%x\n", __FUNCTION__, dst, src1, src2, result);
        } else
        {
            int op1 = BITS(2, 11);

            if (op1 == 0x178)
            {                           // .S unit
                int result;

                fields->exec_unit = UNIT_S1 << fields->s;
                if (x)
                {
                    result = (int) core->gpr[s][src1] - core->gpr[(!s) & 0x1][src2];
                } else
                {
                    result = (int) core->gpr[s][src1] - core->gpr[s][src2];
                }
                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
            } else if (op1 == 0x168)
            {                           // .S unit
                int result;

                fields->exec_unit = UNIT_S1 << fields->s;
                src1 = EXTEND_SIGN(src1, 5);
                result = src1 - (int) core->gpr[reg_group][src2];
                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
            } else if (op1 == 0x35c)
            {
                fields->exec_unit = UNIT_S1 << fields->s;
                int result = (int) core->gpr[reg_group][src2] - (int) core->gpr[s][src1];

                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
            } else
            {
                NOT_IMP_NO_NEED;
            }
        }
    }
    core->pc += 4;
    return 0;
}

static int exec_sub_d(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op7_12;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int result;

        if (op == 0x11)
        {
            result = core->gpr[s][src2] - core->gpr[s][src1];
            DBG("In %s, dst=%d, src1=%d, src2=%d\n", __FUNCTION__, dst, src1, src2);
        } else if (op == 0x13)
        {
            result = core->gpr[s][src2] - src1;
            DBG("In %s, dst=%d, src1=%d, src2=%d\n", __FUNCTION__, dst, src1, src2);
        } else
        {
            NOT_IMP_NO_NEED;
        }
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    core->pc += 4;
    return 0;
}

static int exec_subdp(c6k_core_t * core, uint32_t insn, opcode_fields_t * unused)
{
    if (calc_cond(core, insn))
    {
        int op = BITS(5, 11);
        int src1 = BITS(13, 17);
        int src2 = BITS(18, 22);
        int dst = BITS(23, 27);
        int s = BITS(1, 1);
        int x = BITS(12, 12);
        int gpr_group = x ? (!s) & 0x1 : s;

        uint64_t fp1, fp2;

        fp1 = core->gpr[s][src1 + 1];
        fp1 = (fp1 << 32);
        fp1 |= core->gpr[s][src1];

        fp2 = core->gpr[gpr_group][src2 + 1];
        fp2 = (fp2 << 32);
        fp2 |= core->gpr[gpr_group][src2];

        if (op == 0x19 || op == 0x73)
        {
        } else if (op == 0x1d)
        {
            /* cross pass src1 */
            fp1 = core->gpr[gpr_group][src1 + 1];
            fp1 = (fp1 << 32);
            fp1 |= core->gpr[gpr_group][src1];

            fp2 = core->gpr[s][src2 + 1];
            fp2 = (fp2 << 32);
            fp2 |= core->gpr[s][src2];
        } else if (op == 0x77)
        {
            uint64_t tmp;

            tmp = fp2;
            fp2 = fp1;
            fp1 = tmp;
        } else
        {
            NOT_IMP_NO_NEED;
        }

        double src_fp1 = *(double *) &fp1;
        double src_fp2 = *(double *) &fp2;
        double dst_fp = src_fp1 - src_fp2;
        uint64_t result = *(uint64_t *) & dst_fp;

        REGISTER_REGW_EVENT_L(core, result & 0xFFFFFFFF, s * GPR_NUM_IN_GROUP + dst, SUBDP_DELAY_SLOT);
        REGISTER_REGW_EVENT_H(core, result >> 32, s * GPR_NUM_IN_GROUP + dst + 1, SUBDP_DELAY_SLOT);
    }
    core->pc += 4;
    return 0;
}

static int exec_subsp(c6k_core_t * core, uint32_t insn, opcode_fields_t * unused)
{
    if (calc_cond(core, insn))
    {
        int op = BITS(5, 11);
        int src1 = BITS(13, 17);
        int src2 = BITS(18, 22);
        int dst = BITS(23, 27);
        int s = BITS(1, 1);
        int x = BITS(12, 12);
        int gpr_group = x ? (!s) & 0x1 : s;

        uint32_t fp1, fp2;

        if (op == 0x11 || op == 0x71 || op == 0x75)
        {
            fp1 = core->gpr[s][src1];
            fp2 = core->gpr[gpr_group][src2];
        } else if (op == 0x15)
        {
            /* cross pass src1 */
            fp1 = core->gpr[gpr_group][src1];
            fp2 = core->gpr[s][src2];
        } else
        {
            NOT_IMP_NO_NEED;
        }

        float src_fp1 = *(float *) &fp1;
        float src_fp2 = *(float *) &fp2;
        float dst_fp;

        if (op == 0x71)
            dst_fp = src_fp2 - src_fp1;
        else
            dst_fp = src_fp1 - src_fp2;

        uint32_t result = *(uint32_t *) & dst_fp;

        REGISTER_REGW_EVENT_L(core, result, s * GPR_NUM_IN_GROUP + dst, SUBSP_DELAY_SLOT);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}

static int exec_sub_d_x(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;

        fields->exec_unit = UNIT_D1 << s;
        int x = BITS(12, 12);
        int reg_group = x ? (!s) & 0x1 : s;
        int fp1, fp2;

        fp1 = core->gpr[s][src1];
        fp2 = core->gpr[reg_group][src2];
        int result = fp1 - fp2;

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    core->pc += 4;
    return 0;
}

static int exec_subab(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op7_12;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int y = fields->y;
        uint32_t ucst5 = (uint32_t) src1;
        int cal_num;
        int block_size;
        int cal_src1;
        int result;
        int src;

        if (op == 0x31)
            src = core->gpr[s][src1];
        else if (op == 0x33)
            src = src1;
        else
        {
            NOT_IMP_NO_NEED;
        }

        if (src2 >= 4 && src2 <= 7)
        {
            int amr_mode = check_amr_mode(core, s, src2);

            if (amr_mode == BK0)
            {
                cal_num = (core->amr >> 16) & 0x1f;
                block_size = 0x1 << (cal_num + 1);
                cal_src1 = src % block_size;
            } else if (amr_mode == BK1)
            {
                cal_num = (core->amr >> 21) & 0x1f;
                block_size = 0x1 << (cal_num + 1);
                cal_src1 = src % block_size;
            } else
            {
                cal_src1 = src;
            }
        } else
        {
            cal_src1 = src;
        }

        result = core->gpr[s][src2] - cal_src1;

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}

static int exec_subabs4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];

        int32_t result = (abs(ubyte0(src1_e) - ubyte0(src2_e))) |
            (abs(ubyte1(src1_e) - ubyte1(src2_e)) << 8) |
            (abs(ubyte2(src1_e) - ubyte2(src2_e)) << 16) | (abs(ubyte3(src1_e) - ubyte3(src2_e)) << 24);

        WRITE_BUFFER(core, dst, result);
    }
    core->pc += 4;
    return 0;
}
static int exec_subah(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_D1 << fields->s;
    if (calc_cond(core, insn))
    {
        int op = fields->op7_12;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        uint32_t ucst5 = (uint32_t) src1;
        int cal_num;
        int block_size;
        int cal_src1;
        int result;
        int src;

        if (op == 0x35)
            src = core->gpr[s][src1];
        else if (op == 0x37)
            src = ucst5;
        else
        {
            NOT_IMP_NO_NEED;
        }

        if (src2 >= 4 && src2 <= 7)
        {
            int amr_mode = check_amr_mode(core, s, src2);

            if (amr_mode == BK0)
            {
                cal_num = (core->amr >> 16) & 0x1f;
                block_size = 0x1 << (cal_num + 1);
                cal_src1 = (src << 1) % block_size;
            } else if (amr_mode == BK1)
            {
                cal_num = (core->amr >> 21) & 0x1f;
                block_size = 0x1 << (cal_num + 1);
                cal_src1 = (src << 1) % block_size;
            } else
            {
                cal_src1 = src << 1;
            }
        } else
        {
            cal_src1 = src << 1;
        }

        result = core->gpr[s][src2] - cal_src1;

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}

static int exec_subaw(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    fields->exec_unit = UNIT_D1 << fields->s;
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int op = fields->op7_12;
        int cal_num;
        int block_size;
        int cal_src1;
        int src;

        if (op == 0x39)
            src = core->gpr[s][src1];
        else if (op == 0x3b)
            src = src1;
        else
        {
            NOT_IMP_NO_NEED;
        }

        if (src2 >= 4 && src2 <= 7)
        {
            int amr_mode = check_amr_mode(core, s, src2);

            if (amr_mode == BK0)
            {
                cal_num = (core->amr >> 16) & 0x1f;
                block_size = 0x1 << (cal_num + 1);
                cal_src1 = (src << 2) % block_size;
            } else if (amr_mode == BK1)
            {
                cal_num = (core->amr >> 21) & 0x1f;
                block_size = 0x1 << (cal_num + 1);
                cal_src1 = (src << 2) % block_size;
            } else
            {
                cal_src1 = src << 2;
            }
        } else
        {
            cal_src1 = src << 2;
        }

        int32_t result = (int32_t) core->gpr[s][src2];

        result -= cal_src1;
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    core->pc += 4;
    return 0;
}

static int exec_subc(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int gpr_group = x ? (!s) & 0x1 : s;
        uint32_t result;

        if (core->gpr[s][src1] >= core->gpr[gpr_group][src2])
        {
            result = ((core->gpr[s][src1] - core->gpr[gpr_group][src2]) << 1) + 1;
        } else
            result = core->gpr[s][src1] << 1;
        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    core->pc += 4;
    return 0;
}
static int exec_subu(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = BITS(5, 11);

        int src1 = BITS(13, 17);
        int src2 = BITS(18, 22);
        int dst = BITS(23, 27);
        int s = BITS(1, 1);
        int x = BITS(12, 12);
        int gpr_group = x ? (!s) & 0x1 : s;

        if (op == 0x2f)
        {
            int64_t result;

            result = (int64_t) core->gpr[s][src1] - (int64_t) core->gpr[gpr_group][src2];
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result & 0xFFFFFFFF);
            WRITE_BUFFER(core, dst + 1 + s * GPR_NUM_IN_GROUP, (result >> 32) & 0xFF);
        } else if (op == 0x3f)
        {
            int64_t result;

            result = (int64_t) core->gpr[gpr_group][src1] - (int64_t) core->gpr[s][src2];
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result & 0xFFFFFFFF);
            WRITE_BUFFER(core, dst + 1 + s * GPR_NUM_IN_GROUP, (result >> 32) & 0xFF);
        } else
        {
            NOT_IMP_NO_NEED;
        }

    }
    core->pc += 4;
    return 0;
}
static int exec_sub2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int x = fields->x;
        int reg_group = x ? (!s) & 0x1 : s;
        int result;

        int src1_low = core->gpr[s][src1] & 0xFFFF;
        int src1_upper = core->gpr[s][src1] >> 16;
        int src2_low = core->gpr[reg_group][src2] & 0xFFFF;
        int src2_upper = core->gpr[reg_group][src2] >> 16;

        int dst_low = src1_low - src2_low;
        int dst_upper = src1_upper - src2_upper;

        result = dst_low | (dst_upper << 16);

        WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
    }
    //NEW_IMP;
    core->pc += 4;
    return 0;
}
static int exec_sub4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];

        int32_t result = (((ubyte0(src1_e) - ubyte0(src2_e)) & 0xFF)) |
            (((ubyte1(src1_e) - ubyte1(src2_e)) & 0xFF) << 8) |
            (((ubyte2(src1_e) - ubyte2(src2_e)) & 0xFF) << 16) | (((ubyte3(src1_e) - ubyte3(src2_e)) & 0xFF) << 24);

        WRITE_BUFFER(core, dst, result);
    }
    core->pc += 4;
    return 0;
}
static int exec_swap2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t result = (core->gpr[x][src2] >> 16 & 0xFFFF) | ((core->gpr[x][src2] & 0xFFFF) << 16);

        WRITE_BUFFER(core, dst, result);
    }
    core->pc += 4;
    return 0;
}
static int exec_swap4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src2_e = core->gpr[x][src2];

        int32_t result = ubyte0(src2_e) << 8 | ubyte1(src2_e) | ubyte2(src2_e) << 24 | ubyte3(src2_e) << 16;

        WRITE_BUFFER(core, dst, result);
    }
    core->pc += 4;
    return 0;
}

#define C674X_GEE 2
#define C674X_XEN 3
static int exec_swe(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        core->efr |= (0x1);
        core->tsr &= ~(0x1 << 16);
        //if software exception enable, cause exception isr 
        ///* enable all exceptions */
        //if ((core->tsr >> C674X_GEE & 0x1) && (core->tsr >> C674X_XEN & 0x1))
        if (1)
        {
            core->ntsr &= ~(0x1 << 16);
            core->ifr |= (1 << 1);
        }
    }
    core->pc += 4;
    return 0;
}
static int exec_swenr(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
    }
    NOT_IMP;
    core->pc += 4;
    return 0;
}

static int32_t unpklu4(int32_t a)
{
    int32_t upper_half, lower_half, result;

    upper_half = a << 8 & 0x00ff0000;
    lower_half = a & 0x000000ff;
    result = upper_half | lower_half;
    return result;
}

static int32_t unpkhu4(int32_t a)
{
    int32_t upper_half, lower_half, result;

    upper_half = a >> 8 & 0x00ff0000;
    lower_half = a >> 16 & 0x000000ff;
    result = upper_half | lower_half;
    return result;
}

static int exec_unpkbu4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t dst_e = unpklu4(core->gpr[x][src2]);
        int32_t dst_o = unpkhu4(core->gpr[x][src2]);

        WRITE_BUFFER(core, dst, dst_e);
        WRITE_BUFFER(core, dst + 1, dst_o);
    }
    core->pc += 4;
    return 0;
}

static int exec_unpkh2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t dst_e = slow16(core->gpr[x][src2]);
        int32_t dst_o = shigh16(core->gpr[x][src2]);

        WRITE_BUFFER(core, dst, dst_e);
        WRITE_BUFFER(core, dst + 1, dst_o);
    }
    core->pc += 4;
    return 0;
}

static int exec_unpkhu2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t dst_e = ulow16(core->gpr[x][src2]);
        int32_t dst_o = uhigh16(core->gpr[x][src2]);

        write_buffer(core, dst, dst_e);
        write_buffer(core, dst + 1, dst_o);
    }
    core->pc += 4;
    return 0;
}

static int exec_unpkhu4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t result = unpkhu4(core->gpr[x][src2]);

        write_buffer(core, dst, result);
    }
    core->pc += 4;
    return 0;
}
static int exec_unpklu4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t result = unpklu4(core->gpr[x][src2]);

        write_buffer(core, dst, result);
    }
    core->pc += 4;
    return 0;
}
static int exec_xor(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int op = fields->op2_11;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27;
        int s = fields->s;
        int reg_group = fields->x ? (!s) & 0x1 : s;
        int op1;
        uint32_t result;

        if (op == 0x376)
        {
            op1 = SIGN_EXTEND(src1, 5);
            if (OPSIZE_IS_DW(insn))
            {
                uint32_t result_low, result_high;

                result_high = op1 ^ (core->gpr[reg_group][src2 + 1]);
                result_low = op1 ^ (core->gpr[reg_group][src2]);
                CVT_DW_TO_WORDS(op1, l, h);
                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result_low);
                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP + 1, result_high);
            } else
            {
                result = op1 ^ core->gpr[reg_group][src2];
                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
            }
        } else if (op == 0xa8)
        {
            op1 = SIGN_EXTEND(src1, 5);
            if (OPSIZE_IS_DW(insn))
            {
                uint32_t result_low, result_high;

                result_high = op1 ^ (core->gpr[reg_group][src2 + 1]);
                result_low = op1 ^ (core->gpr[reg_group][src2]);
                CVT_DW_TO_WORDS(op1, l, h);
                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result_low);
                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP + 1, result_high);
            } else
            {
                result = op1 ^ core->gpr[reg_group][src2];
                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
            }
        } else if (op == 0xb8)
        {
            if (OPSIZE_IS_DW(insn))
            {
                GET_DW_UINTEGER(core->gpr[s][src1], core->gpr[s][src1 + 1], op1);
                GET_DW_UINTEGER(core->gpr[reg_group][src2], core->gpr[reg_group][src2 + 1], op2);
                op1 ^= op2;
                CVT_DW_TO_WORDS(op1, l, h);
                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, l);
                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP + 1, h);
            } else
            {
                op1 = core->gpr[s][src1];
                result = op1 ^ core->gpr[reg_group][src2];
                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
            }
        }

        else if (op == 0x37e)
        {
            if (OPSIZE_IS_DW(insn))
            {
                GET_DW_UINTEGER(core->gpr[s][src1], core->gpr[s][src1 + 1], op1);
                GET_DW_UINTEGER(core->gpr[reg_group][src2], core->gpr[reg_group][src2 + 1], op2);
                op1 ^= op2;
                CVT_DW_TO_WORDS(op1, l, h);
                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, l);
                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP + 1, h);
            } else
            {
                op1 = core->gpr[s][src1];
                result = op1 ^ core->gpr[reg_group][src2];
                WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
            }
        } else if (op == 0x2ec)
        {
            fields->exec_unit = UNIT_D1 << s;
            op1 = core->gpr[s][src1];
            result = op1 ^ core->gpr[reg_group][src2];
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else if (op == 0x2fc)
        {
            fields->exec_unit = UNIT_D1 << s;
            op1 = EXTEND_SIGN(src1, 5);
            result = op1 ^ core->gpr[reg_group][src2];
            WRITE_BUFFER(core, dst + s * GPR_NUM_IN_GROUP, result);
        } else
        {
            NOT_IMP;
        }
    }
    core->pc += 4;
    return 0;
}

uint32_t xormpy(uint32_t src1, uint32_t src2)
{
    uint32_t pp;
    uint32_t mask, tpp;
    uint32_t I;

    pp = 0;
    mask = 0x00000100;
    for (I = 0; I < 8; I++)
    {
        if (src2 & mask)
            pp ^= src1;
        mask >>= 1;
        pp <<= 1;
    }
    if (src2 & 0x1)
        pp ^= src1;

    return pp;
}

static int exec_xormpy(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src1 = fields->src13_17;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src1_e = core->gpr[s][src1];
        int32_t src2_e = core->gpr[x][src2];

        uint32_t dst_e;
        uint32_t GMPY_poly;

        fields->exec_unit = UNIT_M1 << s;

        src2_e = src2_e & 0x1FF;
        dst_e = xormpy(src1_e, src2_e);

        REGISTER_REGW_EVENT_L(core, dst_e, dst, 3);
    }
    core->pc += 4;
    return 0;
}
static int exec_xpnd2(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src2_op = core->gpr[x][src2];

        int32_t dst_e = expand2(src2_op);

        REGISTER_REGW_EVENT_L(core, dst_e, dst, 1);
    }
    core->pc += 4;
    return 0;
}
static int exec_xpnd4(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
        int s = fields->s;
        int x = fields->x ? (!s) & 0x1 : s;
        int src2 = fields->src18_22;
        int dst = fields->dst23_27 + s * GPR_NUM_IN_GROUP;

        int32_t src2_op = core->gpr[x][src2];

        int32_t dst_e = expand4(src2_op);

        REGISTER_REGW_EVENT_L(core, dst_e, dst, 1);
    }
    core->pc += 4;
    return 0;
}
static int exec_zero(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    if (calc_cond(core, insn))
    {
    }
    NOT_IMP_NO_NEED;
    return 0;
}

const ISEITEM insn32_decode_table[] = {
    {"dapys2", 2, 6, {2, 11, 0x1c6, 28, 31, 0x1}, 0,},
    {"ABS", 3, 6, {2, 4, 0x6, 5, 11, 0x1a, 13, 17, 0x0}, 0,},
    {"ABS", 3, 6, {2, 4, 0x6, 5, 11, 0x38, 13, 17, 0x0}, 0,},
    {"ABS2", 2, 6, {2, 11, 0xd6, 13, 17, 0x4}, 0,},

    {"dshl2", 2, 6, {2, 11, 0x128, 28, 31, 0x1}, 0,},
    {"dshl2", 2, 6, {2, 11, 0x68, 28, 31, 0x1}, 0,},

    {"add_d", 2, 6, {2, 6, 0x10, 7, 12, 0x10}, 0,},
    {"add_d", 2, 6, {2, 6, 0x10, 7, 12, 0x12}, 0,},
    {"add_dx", 1, 6, {2, 11, 0x2ac}, 0,},
    {"add_dxc", 1, 6, {2, 11, 0x2bc}, 0,},

    {"adddp", 2, 6, {2, 4, 0x6, 5, 11, 0x18,}, 6 | DOUBLE_SLOT,},
    {"adddp", 2, 6, {2, 4, 0x6, 5, 11, 0x72,}, 6 | DOUBLE_SLOT,},

    {"addab", 2, 6, {2, 6, 0x10, 7, 12, 0x30}, 0},
    {"addab", 2, 6, {2, 6, 0x10, 7, 12, 0x32}, 0},
    {"addabd", 2, 6, {2, 6, 0xf, 28, 31, 0x1}, 0},
    {"addad", 2, 6, {2, 6, 0x10, 7, 12, 0x3c}, 0,},
    {"addad", 2, 6, {2, 6, 0x10, 7, 12, 0x3d}, 0,},
    {"addah", 2, 6, {2, 6, 0x10, 7, 12, 0x34}, 0,},
    {"addah", 2, 6, {2, 6, 0x10, 7, 12, 0x36}, 0,},
    {"addahd", 2, 6, {2, 6, 0x17, 28, 31, 0x1}, 0,},
    {"addaw", 2, 6, {2, 6, 0x10, 7, 12, 0x38}, 0,},
    {"addaw", 2, 6, {2, 6, 0x10, 7, 12, 0x3a}, 0,},
    {"addaw", 2, 6, {2, 6, 0x1f, 28, 31, 0x1}, 0,},     // c64x+ only
    {"addk", 1, 6, {2, 6, 0x14}, 0,},
    {"addkpc", 1, 6, {2, 12, 0x58}, KPC_SLOT,},
    {"addsp", 2, 6, {2, 4, 0x6, 5, 11, 0x10}, 3 | SGL_SLOT,},
    {"addsp", 2, 6, {2, 4, 0x6, 5, 11, 0x70}, 3 | SGL_SLOT,},
    {"addsub", 3, 6, {2, 11, 0x66, 23, 23, 0x0, 28, 31, 0x1}, 0,},
    {"addsub2", 3, 6, {2, 11, 0x6e, 23, 23, 0x0, 28, 31, 0x1}, 0,},
    {"addu", 1, 6, {2, 11, 0x15e}, 0,},
    {"addu", 1, 6, {2, 11, 0x14e}, 0,},
    {"addu", 2, 6, {2, 11, 0x2e8, 13, 13, 1}, 0,},
    {"dadd2", 2, 6, {2, 11, 0x18, 28, 31, 0x1}, 0,},    // Unit S
    {"dadd2", 2, 6, {2, 11, 0x2e, 28, 31, 0x1}, 0,},    // Unit L
    {"add2", 1, 6, {2, 11, 0x18}, 0,},
    {"add2", 1, 6, {2, 11, 0x2e}, 0,},
    {"add2", 1, 6, {2, 11, 0x24c}, 0,},
    {"add4", 1, 6, {2, 11, 0x32e}, 0,},

    {"and_l", 2, 6, {2, 4, 0x6, 5, 11, 0x7b}, 0,},
    {"and_l", 2, 6, {2, 4, 0x6, 5, 11, 0x7a}, 0,},
    {"and_s", 2, 6, {2, 5, 0x8, 6, 11, 0x1f}, 0,},
    {"and_s", 2, 6, {2, 5, 0x8, 6, 11, 0x1e}, 0,},
    {"and_s_cst", 1, 6, {2, 11, 0x1e8}, 0,},
    {"and_d", 3, 6, {2, 5, 0xc, 6, 9, 0x6, 10, 11, 0x2}, 0,},
    {"and_d", 3, 6, {2, 5, 0xc, 6, 9, 0x7, 10, 11, 0x2}, 0,},
    {"andn", 1, 6, {2, 11, 0x3e6}, 0,},
    {"andn", 1, 6, {2, 11, 0x36c}, 0,},
    {"andn", 1, 6, {2, 11, 0x20c}, 0,},

    {"davg2", 2, 6, {2, 11, 0x13c, 28, 31, 0x1}, 1 | SGDW_SLOT,},
    {"davgnr2", 2, 6, {2, 11, 0x11c, 28, 31, 0x1}, 1 | SGDW_SLOT,},
    {"davgnru4", 2, 6, {2, 11, 0x10c, 28, 31, 0x1}, 1 | SGDW_SLOT,},
    {"davgu4", 2, 6, {2, 11, 0x12c, 28, 31, 0x1}, 1 | SGDW_SLOT,},
    {"avg2", 1, 6, {2, 11, 0x13c}, 1 | SGDW_SLOT,},
    {"avgu4", 1, 6, {2, 11, 0x12c}, 1 | SGDW_SLOT,},

    {"dccmpy", 2, 6, {2, 11, 0x60, 28, 31, 0x01}, 3 | FOUR_SLOT,},
    {"dccmpyr1", 2, 6, {2, 11, 0xec, 28, 31, 0x01}, 3 | SGDW_SLOT,},

    /* avoid decode with callp instruction */
    {"b", 2, 6, {2, 6, 0x4, 28, 31, 0x0,}, B_SLOT,},
    {"b", 2, 6, {2, 6, 0x4, 29, 31, 0x1,}, B_SLOT,},
    {"b", 2, 6, {2, 6, 0x4, 29, 31, 0x2,}, B_SLOT,},
    {"b", 2, 6, {2, 6, 0x4, 29, 31, 0x3,}, B_SLOT,},
    {"b", 2, 6, {2, 6, 0x4, 29, 31, 0x4,}, B_SLOT,},
    {"b", 2, 6, {2, 6, 0x4, 29, 31, 0x5,}, B_SLOT,},
    {"b", 2, 6, {2, 6, 0x4, 29, 31, 0x6,}, B_SLOT,},

    {"b_reg", 3, 6, {1, 11, 0x1b1, 13, 17, 0x0, 23, 27, 0x0,}, B_SLOT,},
    {"b_irp", 2, 6, {2, 11, 0x38, 13, 22, 0xc0}, B_SLOT,},
    {"b_nrp", 2, 6, {2, 11, 0x38, 13, 22, 0xe0}, B_SLOT,},
    {"bdec", 1, 6, {2, 12, 0x408,}, B_SLOT,},

    {"bitc4", 2, 6, {2, 11, 0x3c, 13, 17, 0x1e}, 1 | SGL_SLOT},
    {"bitr", 2, 6, {2, 11, 0x3c, 13, 17, 0x1f}, 1 | SGL_SLOT},
    {"bnop", 1, 6, {2, 12, 0x48,}, BNOP_SLOT,},
    {"bnop_reg", 3, 6, {1, 11, 0x1b1, 16, 17, 0x0, 23, 27, 0x1,}, BNOP_SLOT,},
    {"bpos", 1, 6, {2, 11, 0x8,}, B_SLOT,},

    {"cmpy32r1", 2, 6, {2, 11, 0x8c, 28, 31, 0x1,}, 3 | SGDW_SLOT,},
    {"callp", 2, 6, {2, 6, 0x4, 28, 31, 0x1,}, CALLP_SLOT,},
    {"ccmatmpy", 2, 6, {2, 11, 0xa0, 28, 31, 0x1,}, 3 | FOUR_SLOT,},
    {"ccmatmpyr1", 2, 6, {2, 11, 0xe0, 28, 31, 0x1,}, 3 | SGDW_SLOT,},
    {"ccmpy32r1", 2, 6, {2, 11, 0x9c, 28, 31, 0x1,}, 3 | SGDW_SLOT,},
    {"clr", 1, 6, {2, 7, 0x32}, 0,},
    {"clr", 1, 6, {2, 11, 0x3f8}, 0,},
    {"cmatmpy", 2, 6, {2, 11, 0x80, 28, 31, 0x1,}, 3 | FOUR_SLOT,},
    {"cmatmpyr1", 2, 6, {2, 11, 0xc0, 28, 31, 0x1,}, 3 | SGDW_SLOT,},

    {"cmpeq", 2, 6, {2, 4, 0x6, 5, 11, 0x53}, 0,},
    {"cmpeq", 2, 6, {2, 4, 0x6, 5, 11, 0x52}, 0,},
    {"cmpeq", 2, 6, {2, 4, 0x6, 5, 11, 0x51}, 0,},
    {"cmpeq", 2, 6, {2, 4, 0x6, 5, 11, 0x50}, 0,},

    {"dcmpeq2", 2, 6, {2, 11, 0x1d8, 28, 31, 0x01}, 0,},
    {"dcmpeq4", 2, 6, {2, 11, 0x1c8, 28, 31, 0x01}, 0,},
    {"dcmpgt2", 2, 6, {2, 11, 0x148, 28, 31, 0x01}, 0,},
    {"dcmpgtu4", 2, 6, {2, 11, 0x158, 28, 31, 0x01}, 0,},
    {"dcmpy", 2, 6, {2, 11, 0x40, 28, 31, 0x01}, 3 | FOUR_SLOT,},       // FIXME, op not consistent with the manual
    {"dcmpyr1", 2, 6, {2, 11, 0xdc, 28, 31, 0x01}, 3 | SGDW_SLOT,},

    {"cmpeq2", 1, 6, {2, 11, 0x1d8}, 0,},
    {"cmpeq4", 1, 6, {2, 11, 0x1c8}, 0,},
    {"cmpeqdp", 1, 6, {2, 11, 0x288}, 1 | SGL_SLOT},
    {"cmpeqsp", 1, 6, {2, 11, 0x388}, 0,},
    {"cmpltdp", 1, 6, {2, 11, 0x2a8}, 1 | SGL_SLOT},
    {"cmpgtsp", 1, 6, {2, 11, 0x398}, 0,},
    {"cmpgtdp", 1, 6, {2, 11, 0x298}, 1 | SGL_SLOT},

    {"cmpgt", 2, 6, {2, 4, 0x6, 5, 11, 0x44}, 0,},
    {"cmpgt", 2, 6, {2, 4, 0x6, 5, 11, 0x45}, 0,},
    {"cmpgt", 2, 6, {2, 4, 0x6, 5, 11, 0x46}, 0,},
    {"cmpgt", 2, 6, {2, 4, 0x6, 5, 11, 0x47}, 0,},

    {"cmpgt2", 1, 6, {2, 11, 0x148}, 0,},
    {"cmpgtu", 2, 6, {2, 4, 0x6, 5, 11, 0x4c}, 0,},
    {"cmpgtu", 2, 6, {2, 4, 0x6, 5, 11, 0x4d}, 0,},
    {"cmpgtu", 2, 6, {2, 4, 0x6, 5, 11, 0x4e}, 0,},
    {"cmpgtu", 2, 6, {2, 4, 0x6, 5, 11, 0x4f}, 0,},
    {"cmpgtu4", 1, 6, {2, 11, 0x158}, 0,},
    {"cmplt", 2, 6, {2, 4, 0x6, 5, 11, 0x54}, 0,},
    {"cmplt", 2, 6, {2, 4, 0x6, 5, 11, 0x55}, 0,},
    {"cmplt", 2, 6, {2, 4, 0x6, 5, 11, 0x56}, 0,},
    {"cmplt", 2, 6, {2, 4, 0x6, 5, 11, 0x57}, 0,},
    {"cmplt2", 1, 6, {2, 11, 0x148,}, 0,},
    {"cmpltsp", 1, 6, {2, 11, 0x3a8,}, 0,},
    {"cmpltu", 2, 6, {2, 4, 0x6, 5, 11, 0x5c}, 0,},
    {"cmpltu", 2, 6, {2, 4, 0x6, 5, 11, 0x5d}, 0,},
    {"cmpltu", 2, 6, {2, 4, 0x6, 5, 11, 0x5e}, 0,},
    {"cmpltu", 2, 6, {2, 4, 0x6, 5, 11, 0x5f}, 0,},
    {"cmpltu4", 1, 6, {2, 11, 0x158}, 0,},

    {"dcrot270", 2, 6, {2, 11, 0xd6, 13, 17, 0x1a}, 0,},
    {"dcrot90", 2, 6, {2, 11, 0xd6, 13, 17, 0x19}, 0,},
    {"crot270", 2, 6, {2, 11, 0xd6, 13, 17, 0xa}, 0,},
    {"crot90", 2, 6, {2, 11, 0xd6, 13, 17, 0x9}, 0,},

    {"cmpy", 1, 6, {2, 11, 0xac}, 3 | SGDW_SLOT,},
    {"cmpyr", 2, 6, {2, 11, 0xbc, 28, 31, 0x1}, 3 | SGL_SLOT,},
    {"cmpyr1", 2, 6, {2, 11, 0xcc, 28, 31, 0x1}, 3 | SGL_SLOT,},
    {"cmpysp", 2, 6, {2, 11, 0x3c0, 28, 31, 0x1}, 3 | FOUR_SLOT,},
    {"cmtl", 0, 6, {2, 4, 0x6}, 0,},    // what't this,  c64x+ insn

    {"dadd", 1, 6, {2, 11, 0x218}, 0,}, // Unit S
    {"dadd", 2, 6, {2, 11, 0x78, 28, 31, 0x1}, 0,},     // Unit S
    {"dadd", 1, 6, {2, 11, 0x116}, 0,}, // Unit L
    {"dadd", 2, 6, {2, 11, 0x01e, 28, 31, 0x1}, 0,},    // Unit L
    {"daddsp", 2, 6, {2, 11, 0x1e6, 28, 31, 0x1}, 2 | SGDW_SLOT,},      // Unit L
    {"daddsp", 2, 6, {2, 11, 0x2c8, 28, 31, 0x1}, 2 | SGDW_SLOT,},      // Unit S

    {"ddotp4", 2, 6, {2, 11, 0x18c, 28, 31, 0x1}, 3 | SGDW_SLOT,},
    {"ddotp4h", 2, 6, {2, 11, 0x100, 28, 31, 0x1}, 3 | SGDW_SLOT,},     // FIXME, op not consistent with the manual
    {"ddotph2", 2, 6, {2, 11, 0x17c, 28, 31, 0x1}, 3 | SGDW_SLOT,},
    {"ddotph2r", 2, 6, {2, 11, 0x15c, 28, 31, 0x1}, 3 | FOUR_SLOT,},
    {"ddotpl2", 2, 6, {2, 11, 0x16c, 28, 31, 0x1}, 3 | SGDW_SLOT,},
    {"ddotpl2r", 2, 6, {2, 11, 0x14c, 28, 31, 0x1}, 3 | SGL_SLOT,},
    {"ddotpsu4h", 2, 6, {2, 11, 0x120, 28, 31, 0x1}, 3 | SGDW_SLOT,},   // FIXME, op not consistent with the manual

    {"deal", 2, 6, {2, 11, 0x3c, 13, 17, 0x1d}, 1 | SGL_SLOT,},
    {"dint", 1, 6, {1, 31, 0x8002000}, 0,},
    {"dinthsp", 2, 6, {2, 11, 0xd6, 13, 17, 0x12}, 2 | SGDW_SLOT,},     // 16 bit, Unit L
    {"dinthsp", 2, 6, {2, 11, 0x3c8, 13, 17, 0x12}, 2 | SGDW_SLOT,},    // 16 bit, Unit S
    {"dinthsp", 2, 6, {2, 11, 0xd6, 13, 17, 0x17}, 2 | SGDW_SLOT,},     // 32 bit, Unit L
    {"dinthsp", 2, 6, {2, 11, 0x3c8, 13, 17, 0x15}, 2 | SGDW_SLOT,},    // 32 bit, Unit S
    {"dinthspu", 2, 6, {2, 11, 0xd6, 13, 17, 0x13}, 2 | SGDW_SLOT,},    // 32 bit, Unit L
    {"dinthspu", 2, 6, {2, 11, 0x3c8, 13, 17, 0x13}, 2 | SGDW_SLOT,},   // 32 bit, Unit S
    {"dintspu", 2, 6, {2, 11, 0xd6, 13, 17, 0x16}, 2 | SGDW_SLOT,},     // 32 bit, Unit L
    {"dintspu", 2, 6, {2, 11, 0x3c8, 13, 17, 0x11}, 2 | SGDW_SLOT,},    // 32 bit, Unit S
    {"dmax2", 2, 6, {2, 11, 0x216, 28, 31, 0x1}, 0,},
    {"dmaxu4", 2, 6, {2, 11, 0x21e, 28, 31, 0x1}, 0,},
    {"dmin2", 2, 6, {2, 11, 0x20e, 28, 31, 0x1}, 0,},
    {"dminu4", 2, 6, {2, 11, 0x246, 28, 31, 0x1}, 0,},
    {"dmpy2", 2, 6, {2, 11, 0xc, 28, 31, 0x1}, 3 | FOUR_SLOT,},
    {"dmpysp", 2, 6, {2, 11, 0x380, 28, 31, 0x1}, 3 | SGDW_SLOT,},
    {"dmpysu4", 2, 6, {2, 11, 0x300, 28, 31, 0x01}, 3 | SGL_SLOT,},     // FIXME, op not consistent with the manual
    {"dmpyu2", 2, 6, {2, 11, 0x20, 28, 31, 0x1}, 3 | FOUR_SLOT,},
    {"dmpyu4", 2, 6, {2, 11, 0x4c, 28, 31, 0x1}, 3 | FOUR_SLOT,},
    {"rpack2", 2, 6, {2, 11, 0x3bc, 28, 31, 0x1}, 0,},
    {"dmv", 1, 6, {2, 11, 0x3bc}, 0,},
    {"dmv", 1, 6, {2, 11, 0x366}, 0,},
    {"dmvd", 2, 6, {2, 11, 0x33c, 28, 31, 0x1}, 3 | SGDW_SLOT,},        //Unit S
    {"dmvd", 2, 6, {2, 11, 0x366, 28, 31, 0x1}, 3 | SGDW_SLOT,},        //Unit L

    // there are two results, W/DW
    {"dotp2", 1, 6, {2, 11, 0xcc}, 3 | SGL_SLOT,},
    {"dotp2", 1, 6, {2, 11, 0xbc}, 3 | SGDW_SLOT,},
    {"dotp4h", 2, 6, {2, 11, 0x6c, 28, 31, 0x1}, 3 | SGDW_SLOT,},
    {"dotp4h", 0, 6, {2, 11, 0x7c}, 3 | SGDW_SLOT,},
    {"dotpn2", 1, 6, {2, 11, 0x9c}, 3 | SGL_SLOT,},
    {"dotpnrsu2", 1, 6, {2, 11, 0x7c}, 3 | SGL_SLOT,},
    {"dotpnrus2", 0, 6, {2, 4, 0x6}, 3 | SGL_SLOT,},
    {"dotprsu2", 1, 6, {2, 11, 0xdc}, 3 | SGL_SLOT,},
    {"dotprus2", 1, 6, {2, 11, 0xdc}, 3 | SGL_SLOT,},
    {"dotpsu4h", 2, 6, {2, 11, 0x2c, 28, 31, 0x1}, 3 | SGL_SLOT,},      // dst
    {"dotpsu4h", 2, 6, {2, 11, 0x5c, 28, 31, 0x1}, 3 | SGDW_SLOT,},     // dwdst
    {"dotpsu4", 1, 6, {2, 11, 0x2c}, 3 | SGL_SLOT,},
    {"dotpus4", 1, 6, {2, 11, 0x2c}, 3 | SGL_SLOT,},
    {"dotpu4", 1, 6, {2, 11, 0x6c}, 3 | SGL_SLOT,},

    {"dpack2", 2, 6, {2, 11, 0x1a6, 28, 31, 0x1}, 0,},
    {"dpackh2", 2, 6, {2, 11, 0xf6, 28, 31, 0x1}, 0,},
    {"dpackh2", 2, 6, {2, 11, 0x98, 28, 31, 0x1}, 0,},
    {"dpackh4", 2, 6, {2, 11, 0x34e, 28, 31, 0x1}, 0,},
    {"dpackhl2", 2, 6, {2, 11, 0x88, 28, 31, 0x1}, 0,},
    {"dpackhl2", 2, 6, {2, 11, 0xe6, 28, 31, 0x1}, 0,},
    {"dpackl2", 2, 6, {2, 11, 0x6, 28, 31, 0x1}, 0,},
    {"dpackl2", 2, 6, {2, 11, 0x3fc, 28, 31, 0x1}, 0,},
    {"dpackl4", 2, 6, {2, 11, 0x346, 28, 31, 0x1}, 0,},
    {"dpacklh2", 2, 6, {2, 11, 0xde, 28, 31, 0x1}, 0,},
    {"dpacklh2", 2, 6, {2, 11, 0x108, 28, 31, 0x1}, 0,},
    {"dpacklh4", 2, 6, {2, 11, 0x356, 28, 31, 0x1}, 0,},
    {"dpackx2", 3, 6, {2, 11, 0x19e, 23, 23, 0x0, 28, 31, 0x1}, 0,},
    {"dpint", 2, 6, {2, 11, 0x46, 13, 17, 0x10}, 3 | SGDW_SLOT,},
    {"dpint", 2, 6, {2, 11, 0x46, 13, 17, 0x0}, 3 | SGDW_SLOT,},        // FIXME, not consistent with the manual
    {"dpsp", 1, 6, {2, 11, 0x4e,}, 3 | SGL_SLOT,},      // FIXME, not consistent with ccs
    {"dptrunc", 2, 6, {2, 11, 0xe, 13, 17, 0x10}, 3 | SGL_SLOT,},       // FIXME, not consistent with the manual
    {"dptrunc", 2, 6, {2, 11, 0xe, 13, 17, 0xa}, 3 | SGL_SLOT,},        // FIXME, not consistent with the manual
    {"dptrunc", 2, 6, {2, 11, 0xe, 13, 17, 0x4}, 3 | SGL_SLOT,},        // FIXME, not consistent with the manual
    {"dptrunc", 2, 6, {2, 11, 0xe, 13, 17, 0x6}, 3 | SGL_SLOT,},        // FIXME, not consistent with the manual
    {"dptrunc", 2, 6, {2, 11, 0xe, 13, 17, 0x8}, 3 | SGL_SLOT,},        // FIXME, not consistent with the manual
    {"dptrunc", 2, 6, {2, 11, 0xe, 13, 17, 0xc}, 3 | SGL_SLOT,},        // FIXME, not consistent with the manual
    {"dptrunc", 2, 6, {2, 11, 0xe, 13, 17, 0x0}, 3 | SGL_SLOT,},        // FIXME, not consistent with the manual
    {"dptrunc", 2, 6, {2, 11, 0xe, 13, 17, 0x2}, 3 | SGL_SLOT,},        // FIXME, not consistent with the manual
    {"sptrunc", 2, 6, {2, 11, 0x5e, 13, 17, 0x0}, 3 | SGL_SLOT,},       // FIXME, not consistent with the manual
    {"dsadd", 2, 6, {2, 11, 0x208, 28, 31, 0x1}, 0,},
    {"dsadd", 2, 6, {2, 11, 0x9e, 28, 31, 0x1}, 0,},
    {"dsadd2", 2, 6, {2, 11, 0x30c, 28, 31, 0x1}, 0,},
    {"dsadd2", 2, 6, {2, 11, 0xa6, 28, 31, 0x1}, 0,},
    {"dshl", 2, 6, {2, 11, 0x338, 28, 31, 0x1}, 0,},
    {"dshl", 2, 6, {2, 11, 0x328, 28, 31, 0x1}, 0,},
    {"dshr", 2, 6, {2, 11, 0x378, 28, 31, 0x1}, 0,},
    {"dshr", 2, 6, {2, 11, 0x368, 28, 31, 0x1}, 0,},
    {"dshr2", 2, 6, {2, 11, 0x37c, 28, 31, 0x1}, 0,},
    {"dshr2", 2, 6, {2, 11, 0x188, 28, 31, 0x1}, 0,},
    {"dshru", 2, 6, {2, 11, 0x278, 28, 31, 0x1}, 0,},
    {"dshru", 2, 6, {2, 11, 0x268, 28, 31, 0x1}, 0,},
    {"dshru2", 2, 6, {2, 11, 0x38c, 28, 31, 0x1}, 0,},
    {"dshru2", 2, 6, {2, 11, 0x198, 28, 31, 0x1}, 0,},
    {"dsmpy2", 2, 6, {2, 11, 0x1c, 28, 31, 0x1}, 3 | FOUR_SLOT,},
    {"dspacku4", 2, 6, {2, 11, 0x34c, 28, 31, 0x1}, 0,},
    {"dspint", 2, 6, {2, 11, 0xd6, 13, 17, 0x14}, 2 | SGDW_SLOT,},      // Uint L
    {"dspint", 2, 6, {2, 11, 0x3c8, 13, 17, 0x14}, 2 | SGDW_SLOT,},     // Uint S
    {"dspinth", 2, 6, {2, 11, 0xd6, 13, 17, 0x10}, 2 | SGL_SLOT,},      // Uint L
    {"dspinth", 2, 6, {2, 11, 0x3c8, 13, 17, 0x10}, 2 | SGL_SLOT,},     // Uint S
    {"dssub", 2, 6, {2, 11, 0x33e, 28, 31, 0x1}, 0,},
    {"dssub2", 2, 6, {2, 11, 0x326, 28, 31, 0x1}, 0,},
    {"dsub", 2, 6, {2, 11, 0x178, 28, 31, 0x1}, 0,},    // Unit S
    {"dsub", 2, 6, {2, 11, 0x3e, 28, 31, 0x1}, 0,},     // Uint L
    {"dsub2", 2, 6, {2, 11, 0x118, 28, 31, 0x1}, 0,},   // Uint S
    {"dsub2", 2, 6, {2, 11, 0x26, 28, 31, 0x1}, 0,},    // Uint L
    {"dsubsp", 2, 6, {2, 11, 0x1ee, 28, 31, 0x1}, 2 | SGDW_SLOT,},      // Uint L
    {"dsubsp", 2, 6, {2, 11, 0x2d8, 28, 31, 0x1}, 2 | SGDW_SLOT,},      // Uint S
    {"dxpnd2", 2, 6, {2, 11, 0x3c, 13, 17, 0x11}, 1 | SGDW_SLOT,},
    {"dxpnd4", 2, 6, {2, 11, 0x3c, 13, 17, 0x10}, 1 | SGDW_SLOT,},
    {"xpnd2", 2, 6, {2, 11, 0x3c, 13, 17, 0x19}, 1 | SGL_SLOT,},
    {"xpnd4", 2, 6, {2, 11, 0x3c, 13, 17, 0x18}, 1 | SGL_SLOT,},
    {"ext", 1, 6, {2, 7, 0x12}, 0,},
    {"extu", 1, 6, {2, 7, 0x2}, 0,},
    {"extu", 1, 6, {2, 11, 0x2b8}, 0,},

    {"fadddp", 1, 6, {2, 11, 0x2c6}, 2 | SGDW_SLOT,},
    {"fadddp", 1, 6, {2, 11, 0x0c8}, 2 | SGDW_SLOT,},
    {"faddsp", 1, 6, {2, 11, 0x1e6}, 2 | SGL_SLOT,},
    {"faddsp", 1, 6, {2, 11, 0x3a6}, 2 | SGL_SLOT,},
    {"mpy2ir", 2, 6, {2, 11, 0xfc, 28, 31, 0x1}, 3 | SGDW_SLOT,},
    {"fmpydp", 1, 6, {2, 11, 0x0fc}, 3 | SGDW_SLOT,},
    {"fsubdp", 1, 6, {2, 11, 0x2ce}, 2 | SGDW_SLOT,},
    {"fsubdp", 1, 6, {2, 11, 0x3ec}, 2 | SGDW_SLOT,},
    {"fsubsp", 1, 6, {2, 11, 0x1ee}, 2 | SGL_SLOT,},
    {"fsubsp", 1, 6, {2, 11, 0x3b6}, 2 | SGL_SLOT,},

    {"gmpy", 2, 6, {2, 11, 0x1fc, 28, 31, 0x1}, 3 | SGL_SLOT,},
    {"gmpy4", 2, 6, {2, 11, 0x11c}, 3 | SGL_SLOT,},
    {"idle", 1, 6, {2, 17, 0x7800}, 0,},
    {"intdp", 2, 6, {2, 11, 0x1CE, 13, 17, 0x0}, 4 | DOUBLE_SLOT,},
    {"intdpu", 2, 6, {2, 11, 0x1DE, 13, 17, 0x0}, 4 | DOUBLE_SLOT,},
    {"intsp", 1, 6, {2, 11, 0x256}, 3 | SGL_SLOT,},
    {"intsp", 2, 6, {2, 11, 0x3c8, 13, 17, 0x5}, 3 | SGL_SLOT,},
    {"intspu", 2, 6, {2, 4, 0x6, 5, 11, 0x49}, 3 | SGL_SLOT,},
    {"intspu", 2, 6, {2, 11, 0x3c8, 13, 17, 0x1}, 3 | SGL_SLOT,},

    {"land", 1, 6, {2, 11, 0x3c6,}, 0,},
    {"landn", 1, 6, {2, 11, 0x3ce,}, 0,},
    {"ldbu", 3, 6, {2, 3, 0x1, 4, 6, 0x1, 8, 8, 0x0,}, LD_SLOT,},
    {"ldbu", 3, 6, {2, 3, 0x1, 4, 6, 0x2, 8, 8, 0x0,}, LD_SLOT,},
    {"ldbu_15", 1, 6, {2, 6, 0x7,}, LD_SLOT,},
    {"ldb_15", 1, 6, {2, 6, 0xb,}, LD_SLOT,},
    {"lddw", 2, 6, {2, 6, 0x19, 8, 8, 0x1,}, LD_SLOT,},
    {"ldhu", 3, 6, {2, 3, 0x1, 4, 6, 0x0, 8, 8, 0x0}, LD_SLOT,},
    {"ldhu", 3, 6, {2, 3, 0x1, 4, 6, 0x4, 8, 8, 0x0}, LD_SLOT,},
    {"ldhu_1", 1, 6, {2, 6, 0x3,}, LD_SLOT,},
    {"ldhu_1", 1, 6, {2, 6, 0x13,}, LD_SLOT,},
    {"ldndw", 2, 6, {2, 6, 0x9, 8, 8, 1,}, LD_SLOT,},
    {"ldnw", 2, 6, {2, 6, 0xd, 8, 8, 1,}, LD_SLOT,},
    {"ldw", 2, 6, {2, 6, 0x19, 8, 8, 0,}, LD_SLOT,},
    {"ldw_15", 1, 6, {2, 6, 0x1b,}, LD_SLOT,},

    {"lor", 1, 6, {2, 11, 0x3ee}, 0,},
    {"lmbd", 2, 6, {2, 4, 0x6, 5, 11, 0x6a,}, 0,},
    {"lmbd", 2, 6, {2, 4, 0x6, 5, 11, 0x6b,}, 0,},
    {"max2", 1, 6, {2, 11, 0x216,}, 0,},
    {"max2", 1, 6, {2, 11, 0x3dc,}, 0,},
    {"maxu4", 1, 6, {2, 11, 0x21e,}, 0,},
    {"mfence", 3, 6, {2, 11, 0x0, 13, 17, 0x4, 28, 31, 0x1}, 0,},
    {"min2", 1, 6, {2, 11, 0x20e,}, 0,},        // Uint L
    {"min2", 1, 6, {2, 11, 0x3cc,}, 0,},        // Uint S
    {"minu4", 1, 6, {2, 11, 0x246,}, 0,},

    {"mpy", 2, 6, {2, 6, 0x0, 7, 11, 0x19,}, 1 | SGL_SLOT,},
    {"mpy", 2, 6, {2, 6, 0x0, 7, 11, 0x18,}, 1 | SGL_SLOT,},
    {"mpydp", 1, 6, {2, 11, 0x1C0,}, 9 | DOUBLE_SLOT,},

    {"mpyh", 1, 6, {2, 11, 0x20,}, 1 | SGL_SLOT,},
    {"mpyhi", 1, 6, {2, 11, 0x14c,}, 3 | SGDW_SLOT,},
    {"mpyhir", 1, 6, {2, 11, 0x10c,}, 3 | SGL_SLOT,},
    {"mpyhl", 1, 6, {2, 11, 0x120,}, 1 | SGL_SLOT,},
    {"mpyhlu", 1, 6, {2, 11, 0x1e0,}, 1 | SGL_SLOT,},
    {"mpyhslu", 1, 6, {2, 11, 0x160,}, 1 | SGL_SLOT,},
    {"mpyhsu", 1, 6, {2, 11, 0x60,}, 1 | SGL_SLOT,},
    {"mpyhu", 1, 6, {2, 11, 0xe0,}, 1 | SGL_SLOT,},
    {"mpyhuls", 1, 6, {2, 11, 0x1a0,}, 1 | SGL_SLOT,},
    {"mpyhus", 1, 6, {2, 11, 0xa0,}, 1 | SGL_SLOT,},
    {"mpyi", 2, 6, {2, 6, 0x0, 7, 11, 0x6,}, 8 | SGL_SLOT,},
    {"mpyi", 2, 6, {2, 6, 0x0, 7, 11, 0x4,}, 8 | SGL_SLOT,},
    {"mpyid", 2, 6, {2, 6, 0x0, 7, 11, 0x8,}, 9 | SGDW_SLOT,},
    {"mpyid", 2, 6, {2, 6, 0x0, 7, 11, 0xc,}, 9 | SGDW_SLOT,},

    {"mpyih", 1, 6, {2, 11, 0x14c,}, 3 | SGDW_SLOT,},
    {"mpyihr", 0, 6, {2, 4, 0x6,}, 3 | SGL_SLOT,},
    {"mpyil", 0, 6, {2, 4, 0x6,}, 3 | SGDW_SLOT,},
    {"mpyilr", 0, 6, {2, 4, 0x6,}, 3 | SGL_SLOT,},
    {"mpylh", 1, 6, {2, 11, 0x220,}, 1 | SGL_SLOT,},
    {"mpylhu", 1, 6, {2, 11, 0x2e0,}, 1 | SGL_SLOT,},
    {"mpyli", 1, 6, {2, 11, 0x15c,}, 3 | SGDW_SLOT,},
    {"mpylir", 1, 6, {2, 11, 0xec,}, 3 | SGL_SLOT,},
    {"mpylshu", 1, 6, {2, 11, 0x260,}, 1 | SGL_SLOT,},
    {"mpyluhs", 1, 6, {2, 11, 0x2a0,}, 1 | SGL_SLOT,},
    {"mpysp", 1, 6, {2, 11, 0x380,}, 3 | SGL_SLOT,},
    {"mpyspdp", 1, 6, {2, 11, 0x16c,}, 6 | DOUBLE_SLOT,},
    {"mpysp2dp", 1, 6, {2, 11, 0x17c,}, 4 | DOUBLE_SLOT,},
    {"mpysu", 2, 6, {2, 6, 0x0, 7, 11, 0x1b,}, 1 | SGL_SLOT,},
    {"mpysu", 2, 6, {2, 6, 0x0, 7, 11, 0x1e,}, 1 | SGL_SLOT,},
    {"mpysu4", 1, 6, {2, 11, 0x5c,}, 3 | SGDW_SLOT,},

    {"mpyu", 1, 6, {2, 11, 0x3e0,}, 1 | SGL_SLOT,},
    {"mpyu2", 1, 6, {2, 11, 0x8c,}, 3 | SGDW_SLOT,},    // FIXME, op not consistent with the manual
    {"mpyu4", 1, 6, {2, 11, 0x4c,}, 3 | SGDW_SLOT,},
    {"qmpysp", 2, 6, {2, 11, 0x3a0, 28, 31, 0x1}, 3 | FOUR_SLOT,},
    {"qsmpy32r1", 2, 6, {2, 11, 0x1ac, 28, 31, 0x1}, 3 | FOUR_SLOT,},
    {"mpyus", 1, 6, {2, 11, 0x3a0,}, 1 | SGL_SLOT,},
    {"mpyus4", 0, 6, {2, 4, 0x6,}, 3 | SGDW_SLOT,},
    {"mpy2", 1, 6, {2, 11, 0xc,}, 3 | SGDW_SLOT,},
    {"qmpy32", 2, 6, {2, 11, 0x200, 28, 31, 0x1}, 3 | FOUR_SLOT,},
    {"mpy32", 1, 6, {2, 11, 0x200,}, 3 | FOUR_SLOT,},
    {"mpy32_64", 1, 6, {2, 11, 0x280,}, 3 | SGDW_SLOT,},
    {"mpy32su", 1, 6, {2, 11, 0x2c0,}, 3 | SGDW_SLOT,},
    {"mpy32u", 1, 6, {2, 11, 0x18c,}, 3 | SGDW_SLOT,},
    {"smpy32", 2, 6, {2, 11, 0x19c, 28, 31, 0x1}, 3 | SGL_SLOT,},
    {"mpy32us", 1, 6, {2, 11, 0x19c,}, 3 | SGDW_SLOT,},
    {"mv", 3, 6, {2, 4, 0x6, 5, 11, 0x2, 13, 17, 0x0,}, 0,},
    {"mv", 3, 6, {2, 4, 0x6, 5, 11, 0x20, 13, 17, 0x0,}, 0,},
    {"mv", 2, 6, {2, 11, 0x68, 13, 17, 0x0}, 0,},
    {"mv", 1, 6, {2, 17, 0x250}, 0,},

    {"mvc", 1, 6, {2, 11, 0xe8,}, 0,},
    {"mvc", 1, 6, {2, 11, 0xf8,}, 0,},
    {"mvd", 2, 6, {2, 11, 0x3c, 13, 17, 0x1a,}, 3 | SGL_SLOT,},
    {"mvk_s", 1, 6, {2, 6, 0xa,}, 0,},
    {"mvk_l", 2, 6, {2, 11, 0xd6, 13, 17, 0x5,}, 0,},
    {"mvk_d", 2, 6, {2, 12, 0x10, 18, 22, 0x0,}, 0,},
    {"mvkh", 1, 6, {2, 5, 0xa,}, 0,},
    {"mvklh", 1, 6, {2, 6, 0x1a,}, 0,},

    {"neg", 3, 6, {2, 5, 0x8, 6, 11, 0x16, 13, 17, 0,}, 0,},
    {"neg", 3, 6, {2, 4, 0x6, 5, 11, 0x6, 13, 17, 0,}, 0,},
    {"neg", 3, 6, {2, 4, 0x6, 5, 11, 0x24, 13, 17, 0,}, 0,},

    {"nop", 2, 6, {1, 12, 0x0, 17, 31, 0x0,}, NOP_SLOT,},
    {"norm", 2, 6, {2, 4, 0x6, 5, 11, 0x63, 13, 17, 0x0,}, 0,},
    {"norm", 2, 6, {2, 4, 0x6, 5, 11, 0x60, 13, 17, 0x0,}, 0,},
    {"not", 2, 6, {2, 11, 0x1bb, 13, 17, 0x1f}, 0,},
    {"not", 2, 6, {2, 11, 0xa8, 13, 17, 0x1f}, 0,},
    {"or_d", 3, 6, {2, 5, 0xc, 6, 9, 0x2, 10, 11, 0x2,}, 0,},
    {"or_d", 3, 6, {2, 5, 0xc, 6, 9, 0x3, 10, 11, 0x2,}, 0,},
    {"or_l", 2, 6, {2, 4, 0x6, 5, 11, 0x7f,}, 0,},
    {"or_l", 2, 6, {2, 4, 0x6, 5, 11, 0x7e,}, 0,},
    {"or_s", 2, 6, {2, 5, 0x8, 6, 11, 0x1a,}, 0,},
    {"or_s", 2, 6, {2, 5, 0x8, 6, 11, 0x1b,}, 0,},

    {"pack2", 1, 6, {2, 11, 0x6,}, 0,},
    {"pack2", 1, 6, {2, 11, 0x3fc,}, 0,},

    {"packh2", 1, 6, {2, 11, 0xf6,}, 0,},       // Uint L
    {"packh2", 1, 6, {2, 11, 0x98,}, 0,},       // Uint S
    {"packh4", 1, 6, {2, 11, 0x34e,}, 0,},      // Uint L
    {"packhl2", 1, 6, {2, 11, 0xe6,}, 0,},
    {"packhl2", 1, 6, {2, 11, 0x88,}, 0,},
    {"packlh2", 2, 6, {2, 4, 0x6, 5, 11, 0x1b}, 0,},
    {"packlh2", 2, 6, {2, 4, 0x0, 5, 11, 0x21}, 0,},
    {"packl4", 1, 6, {2, 11, 0x346,}, 0,},

    {"rcpdp", 2, 6, {2, 11, 0x2d8, 13, 13, 0x0}, 1 | DOUBLE_SLOT},      /* c6678 bit 13_17 reserved in the manual，add bit 13 to different from add_s */
    {"rcpdp", 2, 6, {2, 11, 0x2d8, 13, 17, 0x6}, 1 | DOUBLE_SLOT},      /* not consistent with the manual */
    {"rcpsp", 2, 6, {2, 11, 0x3d8, 13, 17, 0x0}, 0 | SGL_SLOT},

    {"rint", 1, 6, {1, 31, 0x8003000,}, 0,},
    {"rotl", 1, 6, {2, 11, 0x1dc,}, 1 | SGL_SLOT,},
    {"rotl", 1, 6, {2, 11, 0x1ec,}, 1 | SGL_SLOT,},
    {"rsqrdp", 1, 6, {2, 11, 0x2e8,}, 1 | DOUBLE_SLOT,},
    {"rsqrsp", 2, 6, {2, 11, 0x3e8, 13, 17, 0x0}, 0,},

    {"sadd", 2, 6, {2, 4, 0x6, 5, 11, 0x13}, 0,},
    {"sadd", 2, 6, {2, 4, 0x6, 5, 11, 0x31}, 0,},
    {"sadd", 2, 6, {2, 4, 0x6, 5, 11, 0x12}, 0,},
    {"sadd", 2, 6, {2, 4, 0x6, 5, 11, 0x30}, 0,},
    {"sadd2", 1, 6, {2, 11, 0x30c,}, 0,},
    {"saddsub", 2, 6, {2, 11, 0x76, 28, 31, 0x1}, 0,},
    {"saddsub2", 2, 6, {2, 11, 0x7e, 28, 31, 0x1}, 0,},
    {"saddus2", 1, 6, {2, 11, 0x31c,}, 0,},
    {"saddsu2", 1, 6, {2, 11, 0x31c,}, 0,},
    {"saddu4", 1, 6, {2, 11, 0x33c,}, 0,},

    {"sat", 2, 6, {2, 11, 0x206, 13, 17, 0x0}, 0,},
    {"set", 1, 6, {2, 7, 0x22,}, 0,},
    {"set", 1, 6, {2, 11, 0x3b8,}, 0,},
    {"shfl", 1, 6, {2, 11, 0x3c,}, 1 | SGL_SLOT,},
    {"shfl3", 2, 6, {2, 11, 0x1b6, 28, 31, 0x1}, 0,},
    {"shl2", 2, 6, {2, 11, 0x138, 28, 31, 0x1,}, 0},
    {"shl2", 2, 6, {2, 11, 0x38, 28, 31, 0x1,}, 0},
    {"shl", 2, 6, {2, 5, 0x8, 6, 11, 0x32,}, 0},
    {"shl", 2, 6, {2, 5, 0x8, 6, 11, 0x33,}, 0},
    {"shl", 2, 6, {2, 5, 0x8, 6, 11, 0x31,}, 0},
    {"shl", 2, 6, {2, 5, 0x8, 6, 11, 0x30,}, 0},
    {"shl", 2, 6, {2, 5, 0x8, 6, 11, 0x12,}, 0},
    {"shl", 2, 6, {2, 5, 0x8, 6, 11, 0x13,}, 0},
    {"shlmb", 1, 6, {2, 11, 0x30e,}, 0,},       // Uint S
    {"shlmb", 1, 6, {2, 11, 0x39c,}, 0,},       // Uint L
    {"shr", 2, 6, {2, 5, 0x8, 6, 11, 0x36,}, 0,},
    {"shr", 2, 6, {2, 5, 0x8, 6, 11, 0x35,}, 0,},
    {"shr", 2, 6, {2, 5, 0x8, 6, 11, 0x37,}, 0,},
    {"shr", 2, 6, {2, 5, 0x8, 6, 11, 0x34,}, 0,},
    {"shr2", 1, 6, {2, 11, 0x37c,}, 0,},        // Unit S uing form
    {"shr2", 1, 6, {2, 11, 0x188,}, 0,},        // Unit S cst form
    {"shrmb", 1, 6, {2, 11, 0x316,}, 0,},       // Unit L
    {"shrmb", 1, 6, {2, 11, 0x3ac,}, 0,},       // Unit S

    {"shru", 2, 6, {2, 5, 0x8, 6, 11, 0x27,}, 0,},
    {"shru", 2, 6, {2, 5, 0x8, 6, 11, 0x25,}, 0,},
    {"shru", 2, 6, {2, 5, 0x8, 6, 11, 0x26,}, 0,},
    {"shru", 2, 6, {2, 5, 0x8, 6, 11, 0x24,}, 0,},
    {"shru2", 1, 6, {2, 11, 0x38c}, 0,},        // Unit S uing form
    {"shru2", 1, 6, {2, 11, 0x198}, 0,},        // Unit S cst form
    {"sl", 0, 6, {2, 4, 0x6}, 0,},      // what's this, c64x+ insn

    {"smpy", 1, 6, {2, 11, 0x340,}, 1 | SGL_SLOT,},
    {"smpyh", 1, 6, {2, 11, 0x40,}, 1 | SGL_SLOT,},
    {"smpyhl", 1, 6, {2, 11, 0x140,}, 1 | SGL_SLOT,},
    {"smpylh", 1, 6, {2, 11, 0x240,}, 1 | SGL_SLOT,},
    {"smpy2", 1, 6, {2, 11, 0x1c,}, 3 | SGDW_SLOT,},
    {"spack2", 1, 6, {2, 11, 0x32c,}, 0,},
    {"spacku4", 1, 6, {2, 11, 0x34c,}, 0,},
    {"spdp", 2, 6, {2, 11, 0x28, 13, 17, 0x0,}, 1 | DOUBLE_SLOT,},
    {"spint", 2, 6, {2, 11, 0x56, 13, 17, 0x0,}, 3 | SGL_SLOT,},
    {"spkernel", 2, 6, {2, 21, 0xd000, 28, 31, 0x0,}, 0,},
    {"spkernelr", 0, 6, {2, 4, 0x6,}, 0,},
    {"sploop", 1, 6, {2, 22, 0xe000,}, 0,},
    {"sploopd", 1, 6, {2, 22, 0xe800,}, 0,},
    {"sploopw", 1, 6, {2, 22, 0xf800,}, 0,},
    {"spmask", 2, 6, {2, 17, 0xc000, 26, 31, 0x0,}, 0,},
    {"spmaskr", 0, 6, {2, 4, 0x6,}, 0,},
    {"sshl", 2, 6, {2, 5, 0x8, 6, 11, 0x23}, 0,},
    {"sshl", 2, 6, {2, 5, 0x8, 6, 11, 0x22}, 0,},
    {"sshvl", 1, 6, {2, 11, 0x1cc,}, 1 | SGL_SLOT,},
    {"sshvr", 1, 6, {2, 11, 0x1ac,}, 1 | SGL_SLOT,},
    {"ssub", 2, 6, {2, 4, 0x6, 5, 11, 0xf}, 0,},
    {"ssub", 2, 6, {2, 4, 0x6, 5, 11, 0x1f}, 0,},
    {"ssub", 2, 6, {2, 4, 0x6, 5, 11, 0xe}, 0,},
    {"ssub", 2, 6, {2, 4, 0x6, 5, 11, 0x2c}, 0,},
    {"ssub2", 1, 6, {2, 11, 0x326,}, 0,},

    // st insn delay slot means write mem delay
    {"stb", 2, 6, {2, 6, 0xd, 8, 8, 0x0,}, ST_SLOT,},
    {"stb_15", 1, 6, {2, 6, 0xf,}, ST_SLOT,},
    {"stdw", 2, 6, {2, 6, 0x11, 8, 8, 0x1,}, ST_SLOT,},
    {"sth", 2, 6, {2, 6, 0x15, 8, 8, 0x0,}, ST_SLOT,},
    {"sth_15", 1, 6, {2, 6, 0x17,}, ST_SLOT,},
    {"stndw", 2, 6, {2, 6, 0x1d, 8, 8, 0x1,}, ST_SLOT,},
    {"stnw", 2, 6, {2, 6, 0x15, 8, 8, 0x1,}, ST_SLOT,},
    {"stw", 2, 6, {2, 6, 0x1d, 8, 8, 0x0,}, ST_SLOT,},
    {"stw_15", 1, 6, {2, 6, 0x1f,}, ST_SLOT,},

    {"sub", 2, 6, {2, 4, 0x6, 5, 11, 0x7,}, 0,},
    {"sub", 2, 6, {2, 4, 0x6, 5, 11, 0x17,}, 0,},
    {"sub", 2, 6, {2, 4, 0x6, 5, 11, 0x27,}, 0,},
    {"sub", 2, 6, {2, 4, 0x6, 5, 11, 0x37,}, 0,},
    {"sub", 2, 6, {2, 4, 0x6, 5, 11, 0x6,}, 0,},
    {"sub", 2, 6, {2, 4, 0x6, 5, 11, 0x24,}, 0,},
    {"sub", 2, 6, {2, 5, 0x8, 6, 11, 0x17,}, 0,},       /* .S unit */
    {"sub", 2, 6, {2, 5, 0x8, 6, 11, 0x16,}, 0,},       /* .S unit */
    {"sub", 1, 6, {2, 11, 0x35c,}, 0,}, /* .S unit with reversed operators */

    {"subdp", 2, 6, {2, 4, 0x6, 5, 11, 0x19,}, 6 | DOUBLE_SLOT,},
    {"subdp", 2, 6, {2, 4, 0x6, 5, 11, 0x1d,}, 6 | DOUBLE_SLOT,},
    {"subdp", 2, 6, {2, 4, 0x6, 5, 11, 0x73,}, 6 | DOUBLE_SLOT,},
    {"subdp", 2, 6, {2, 4, 0x6, 5, 11, 0x77,}, 6 | DOUBLE_SLOT,},

    {"subsp", 2, 6, {2, 4, 0x6, 5, 11, 0x11,}, 3 | SGL_SLOT,},
    {"subsp", 2, 6, {2, 4, 0x6, 5, 11, 0x15,}, 3 | SGL_SLOT,},
    {"subsp", 2, 6, {2, 4, 0x6, 5, 11, 0x71,}, 3 | SGL_SLOT,},
    {"subsp", 2, 6, {2, 4, 0x6, 5, 11, 0x75,}, 3 | SGL_SLOT,},

    {"sub_d_x", 1, 6, {2, 11, 0x2cc,}, 0,},     /* sub D, cross path */

    {"sub_d", 2, 6, {2, 6, 0x10, 7, 12, 0x11,}, 0,},    /* sub D, not cross path */
    {"sub_d", 2, 6, {2, 6, 0x10, 7, 12, 0x13,}, 0,},    /* sub D, not cross path */

    {"subab", 2, 6, {2, 6, 0x10, 7, 12, 0x31}, 0,},
    {"subab", 2, 6, {2, 6, 0x10, 7, 12, 0x33}, 0,},
    {"subabs4", 1, 6, {2, 11, 0x2D6,}, 0,},
    {"subah", 2, 6, {2, 6, 0x10, 7, 12, 0x35}, 0,},
    {"subah", 2, 6, {2, 6, 0x10, 7, 12, 0x37}, 0,},
    {"subaw", 1, 6, {2, 12, 0x730}, 0,},
    {"subpw", 1, 6, {2, 12, 0x770}, 0,},
    {"subc", 2, 6, {2, 4, 0x6, 5, 11, 0x4b,}, 0,},
    {"subu", 1, 6, {2, 11, 0x1fe,}, 0,},
    {"subu", 1, 6, {2, 11, 0x17e,}, 0,},
    {"sub2", 1, 6, {2, 11, 0x26,}, 0,},
    {"sub2", 1, 6, {2, 11, 0x118,}, 0,},
    {"sub2", 1, 6, {2, 11, 0x25c,}, 0,},
    {"sub4", 1, 6, {2, 11, 0x336,}, 0,},

    {"swap2", 1, 6, {2, 11, 0xde,}, 0,},        // Unit L
    {"swap2", 1, 6, {2, 11, 0x108,}, 0,},       // Unit S
    {"swap4", 2, 6, {2, 11, 0xd6, 13, 17, 0x1}, 0,},
    {"swe", 3, 6, {2, 11, 0x0, 12, 26, 0x0, 28, 31, 0x1}, 0,},
    {"swenr", 0, 6, {2, 4, 0x6,}, 0,},
    {"unpkbu4", 2, 6, {2, 11, 0x3c8, 13, 17, 0x8,}, 0,},        //Unit S
    {"unpkbu4", 2, 6, {2, 11, 0xd6, 13, 17, 0x8,}, 0,}, //Uint L
    {"unpkh2", 2, 6, {2, 11, 0x3c8, 13, 17, 0x7,}, 0,}, //Unit S
    {"unpkh2", 2, 6, {2, 11, 0xd6, 13, 17, 0x7,}, 0,},  //Uint L
    {"unpkhu2", 2, 6, {2, 11, 0x3c8, 13, 17, 0x6,}, 0,},        //Unit S
    {"unpkhu2", 2, 6, {2, 11, 0xd6, 13, 17, 0x6,}, 0,}, //Uint L
    {"unpkhu4", 2, 6, {2, 11, 0x3c8, 13, 17, 0x3,}, 0,},        //Uint S
    {"unpkhu4", 2, 6, {2, 11, 0xd6, 13, 17, 0x3,}, 0,}, //Uint L
    {"unpklu4", 2, 6, {2, 11, 0x3c8, 13, 17, 0x2,}, 0,},        //Uint S
    {"unpklu4", 2, 6, {2, 11, 0xd6, 13, 17, 0x2,}, 0,}, //Uint L
    {"xor", 2, 6, {2, 5, 0x8, 6, 11, 0xa,}, 0,},        /* .S unit */
    {"xor", 2, 6, {2, 5, 0x8, 6, 11, 0xb,}, 0,},        /* .S unit */
    {"xor", 2, 6, {2, 4, 0x6, 5, 11, 0x6e,}, 0,},       /* .L unit */
    {"xor", 2, 6, {2, 4, 0x6, 5, 11, 0x6f,}, 0,},       /* .L unit */
    {"xor", 2, 6, {2, 5, 0xc, 6, 9, 0xe,}, 0,}, /* .D unit */
    {"xor", 2, 6, {2, 5, 0xc, 6, 9, 0xf,}, 0,}, /* .D unit */
    {"xormpy", 2, 6, {2, 11, 0x1bc, 28, 31, 0x1}, 3 | SGL_SLOT,},

    {"ABSDP", 1, 6, {2, 11, 0x2c8,}, 1 | DOUBLE_SLOT,},
    {"ABSSP", 2, 6, {2, 11, 0x3c8, 13, 17, 0x0}, 0,},

    {"add_l", 2, 6, {2, 4, 0x6, 5, 11, 0x3}, 0,},
    {"add_l", 2, 6, {2, 4, 0x6, 5, 11, 0x23}, 0,},
    {"add_l", 2, 6, {2, 4, 0x6, 5, 11, 0x21}, 0,},
    {"add_l", 2, 6, {2, 4, 0x6, 5, 11, 0x2}, 0,},
    {"add_l", 2, 6, {2, 4, 0x6, 5, 11, 0x20}, 0,},

    {"add_s", 2, 6, {2, 5, 0x8, 6, 11, 0x2d}, 0,},
    {"add_s", 2, 6, {2, 5, 0x8, 6, 11, 0x7}, 0,},
    {"add_s", 2, 6, {2, 5, 0x8, 6, 11, 0x6}, 0,},
    {"zero", 0, 6, {2, 4, 0x6,}, 0,},
};

insn_action_t insn_action[] = {
    exec_dapys2,
    exec_abs,
    exec_abs,
    exec_abs2,

    exec_dshl2,
    exec_dshl2,

    exec_add_d,
    exec_add_d,
    exec_add_d,
    exec_add_d,

    exec_adddp,
    exec_adddp,

    exec_addab,
    exec_addab,
    exec_addabd,
    exec_addad,
    exec_addad,
    exec_addah,
    exec_addah,
    exec_addah_d,
    exec_addaw,
    exec_addaw,
    exec_addaw_c64xplus_only,

    exec_addk,
    exec_addkpc,
    exec_addsp,
    exec_addsp,
    exec_addsub,
    exec_addsub2,
    exec_addu,
    exec_addu,
    exec_addu,
    exec_dadd2,
    exec_dadd2,
    exec_add2,
    exec_add2,
    exec_add2,
    exec_add4,

    exec_and_l,
    exec_and_l,
    exec_and_s,
    exec_and_s,
    exec_and_s_cst,
    exec_and_d,
    exec_and_d,

    exec_andn,
    exec_andn,
    exec_andn,

    exec_davg2,
    exec_davgnr2,
    exec_davgnru4,
    exec_davgu4,
    exec_avg2,
    exec_avgu4,

    exec_dccmpy,
    exec_dccmpyr1,

    exec_b,
    exec_b,
    exec_b,
    exec_b,
    exec_b,
    exec_b,
    exec_b,

    exec_b_reg,
    exec_b_irp,
    exec_b_nrp,
    exec_bdec,

    exec_bitc4,
    exec_bitr,
    exec_bnop,
    exec_bnop_reg,
    exec_bpos,

    exec_cmpy32r1,
    exec_callp,
    exec_ccmatmpy,
    exec_ccmatmpyr1,
    exec_ccmpy32r1,
    exec_clr,
    exec_clr_reg_form,
    exec_cmatmpy,
    exec_cmatmpyr1,

    exec_cmpeq,
    exec_cmpeq,
    exec_cmpeq,
    exec_cmpeq,

    exec_dcmpeq2,
    exec_dcmpeq4,
    exec_dcmpgt2,
    exec_dcmpgtu4,
    exec_dcmpy,
    exec_dcmpyr1,

    exec_cmpeq2,
    exec_cmpeq4,
    exec_cmpeqdp,
    exec_cmpeqsp,
    exec_cmpltdp,
    exec_cmpgtsp,
    exec_cmpgtdp,
    exec_cmpgt,
    exec_cmpgt,
    exec_cmpgt,
    exec_cmpgt,

    exec_cmpgt2,

    exec_cmpgtu,
    exec_cmpgtu,
    exec_cmpgtu,
    exec_cmpgtu,

    exec_cmpgtu4,
    exec_cmplt,
    exec_cmplt,
    exec_cmplt,
    exec_cmplt,

    exec_cmplt2,
    exec_cmpltsp,
    exec_cmpltu,
    exec_cmpltu,
    exec_cmpltu,
    exec_cmpltu,

    exec_cmpltu4,

    exec_dcrot270,
    exec_dcrot90,
    exec_crot270,
    exec_crot90,

    exec_cmpy,
    exec_cmpyr,
    exec_cmpyr1,
    exec_cmpysp,
    exec_cmtl,

    exec_dadd,
    exec_dadd,
    exec_dadd,
    exec_dadd,
    exec_daddsp,
    exec_daddsp,

    exec_ddotp4,
    exec_ddotp4h,
    exec_ddotph2,
    exec_ddotph2r,
    exec_ddotpl2,
    exec_ddotpl2r,
    exec_ddotpsu4h,

    exec_deal,
    exec_dint,
    exec_dinthsp,
    exec_dinthsp,
    exec_dinthsp,
    exec_dinthsp,
    exec_dinthsp,
    exec_dinthsp,
    exec_dinthsp,
    exec_dinthsp,
    exec_dmax2,
    exec_dmaxu4,
    exec_dmin2,
    exec_dminu4,
    exec_dmpy2,
    exec_dmpysp,
    exec_dmpysu4,
    exec_dmpyu2,
    exec_dmpyu4,
    exec_rpack2,
    exec_dmv,
    exec_dmv,
    exec_dmvd,
    exec_dmvd,

    exec_dotp2,
    exec_dotp2,
    exec_dotp4h,
    exec_dotp4h,
    exec_dotpn2,
    exec_dotpnrsu2,
    exec_dotpnrus2,
    exec_dotprsu2,
    exec_dotprus2,
    exec_dotpsu4h,
    exec_dotpsu4h,
    exec_dotpsu4,
    exec_dotpus4,
    exec_dotpu4,

    exec_dpack2,
    exec_dpackh2,
    exec_dpackh2,
    exec_dpackh4,
    exec_dpackhl2,
    exec_dpackhl2,
    exec_dpackl2,
    exec_dpackl2,
    exec_dpackl4,
    exec_dpacklh2,
    exec_dpacklh2,
    exec_dpacklh4,
    exec_dpackx2,
    exec_dpint,
    exec_dpint,
    exec_dpsp,
    exec_dptrunc,
    exec_dptrunc,
    exec_dptrunc,
    exec_dptrunc,
    exec_dptrunc,
    exec_dptrunc,
    exec_dptrunc,
    exec_dptrunc,
    exec_sptrunc,
    exec_dsadd,
    exec_dsadd,
    exec_dsadd2,
    exec_dsadd2,
    exec_dshl,
    exec_dshl,
    exec_dshr,
    exec_dshr,
    exec_dshr2,
    exec_dshr2,
    exec_dshru,
    exec_dshru,
    exec_dshru2,
    exec_dshru2,
    exec_dsmpy2,
    exec_dspacku4,
    exec_dspint,
    exec_dspint,
    exec_dspinth,
    exec_dspinth,
    exec_dssub,
    exec_dssub2,
    exec_dsub,
    exec_dsub,
    exec_dsub2,
    exec_dsub2,
    exec_dsubsp,
    exec_dsubsp,
    exec_dxpnd2,
    exec_dxpnd4,
    exec_xpnd2,
    exec_xpnd4,
    exec_ext,
    exec_extu,
    exec_extu_reg_form,

    exec_fadddp,
    exec_fadddp,
    exec_faddsp,
    exec_faddsp,
    exec_mpy2ir,
    exec_fmpydp,
    exec_fsubdp,
    exec_fsubdp,
    exec_fsubsp,
    exec_fsubsp,

    exec_gmpy,
    exec_gmpy4,
    exec_idle,
    exec_intdp,
    exec_intdpu,
    exec_intsp,
    exec_intsp,
    exec_intspu,
    exec_intspu,

    exec_land,
    exec_landn,
    exec_ldbu,
    exec_ldbu,
    exec_ldbu_15,
    exec_ldb_15,
    exec_lddw,
    exec_ldhu,
    exec_ldhu,
    exec_ldhu_1,
    exec_ldhu_1,
    exec_ldndw,
    exec_ldnw,
    exec_ldw,
    exec_ldw_15,

    exec_lor,
    exec_lmbd,
    exec_lmbd,

    exec_max2,
    exec_max2,
    exec_maxu4,
    exec_mfence,
    exec_min2,
    exec_min2,
    exec_minu4,

    exec_mpy,
    exec_mpy,
    exec_mpydp,
    exec_mpyh,
    exec_mpyhi,
    exec_mpyhir,
    exec_mpyhl,
    exec_mpyhlu,
    exec_mpyhslu,
    exec_mpyhsu,
    exec_mpyhu,
    exec_mpyhuls,
    exec_mpyhus,
    exec_mpyi,
    exec_mpyi,
    exec_mpyid,
    exec_mpyid,

    exec_mpyih,
    exec_mpyihr,
    exec_mpyil,
    exec_mpyilr,
    exec_mpylh,
    exec_mpylhu,
    exec_mpyli,
    exec_mpylir,
    exec_mpylshu,
    exec_mpyluhs,
    exec_mpysp,
    exec_mpyspdp,
    exec_mpysp2dp,

    exec_mpysu,
    exec_mpysu,
    exec_mpysu4,

    exec_mpyu,
    exec_mpyu2,
    exec_mpyu4,
    exec_qmpysp,
    exec_qsmpy32r1,
    exec_mpyus,
    exec_mpyus4,
    exec_mpy2,
    exec_qmpy32,
    exec_mpy32,
    exec_mpy32_64,
    exec_mpy32su,
    exec_mpy32u,
    exec_smpy32,
    exec_mpy32us,
    exec_mv,
    exec_mv,
    exec_mv,
    exec_mv,

    exec_mvc,
    exec_mvc,

    exec_mvd,
    exec_mvk_s,
    exec_mvk_l,
    exec_mvk_d,
    exec_mvkh,
    exec_mvklh,

    exec_neg,
    exec_neg,
    exec_neg,

    exec_nop,
    exec_norm,
    exec_norm,
    exec_not,
    exec_not,

    exec_or_d,
    exec_or_d,
    exec_or_l,
    exec_or_l,
    exec_or_s,
    exec_or_s,

    exec_pack2,
    exec_pack2,
    exec_packh2,
    exec_packh2,
    exec_packh4,
    exec_packhl2,
    exec_packhl2,
    exec_packlh2,
    exec_packlh2,
    exec_packl4,

    exec_rcpdp,
    exec_rcpdp,
    exec_rcpsp,
    exec_rint,
    exec_rotl,
    exec_rotl,
    exec_rsqrdp,
    exec_rsqrsp,

    exec_sadd,
    exec_sadd,
    exec_sadd,
    exec_sadd,
    exec_sadd2,
    exec_saddsub,
    exec_saddsub2,
    exec_saddus2,
    exec_saddsu2,
    exec_saddu4,

    exec_sat,
    exec_set,
    exec_set_reg_form,
    exec_shfl,
    exec_shfl3,
    exec_shl2,
    exec_shl2,
    exec_shl,
    exec_shl,
    exec_shl,
    exec_shl,
    exec_shl,
    exec_shl,

    exec_shlmb,
    exec_shlmb,
    exec_shr,
    exec_shr,
    exec_shr,
    exec_shr,
    exec_shr2,
    exec_shr2,
    exec_shrmb,
    exec_shrmb,

    exec_shru,
    exec_shru,
    exec_shru,
    exec_shru,
    exec_shru2,
    exec_shru2,
    exec_sl,

    exec_smpy,
    exec_smpyh,
    exec_smpyhl,
    exec_smpylh,
    exec_smpy2,
    exec_spack2,
    exec_spacku4,
    exec_spdp,
    exec_spint,
    exec_spkernel,
    exec_spkernelr,
    exec_sploop,
    exec_sploopd,
    exec_sploopw,
    exec_spmask,
    exec_spmaskr,
    exec_sshl,
    exec_sshl,
    exec_sshvl,
    exec_sshvr,
    exec_ssub,
    exec_ssub,
    exec_ssub,
    exec_ssub,
    exec_ssub2,

    exec_stb,
    exec_stb_15,
    exec_stdw,
    exec_sth,
    exec_sth_15,
    exec_stndw,
    exec_stnw,
    exec_stw,
    exec_stw_15,

    exec_sub,
    exec_sub,
    exec_sub,
    exec_sub,
    exec_sub,
    exec_sub,
    exec_sub,                           /* .S unit */
    exec_sub,                           /* .S unit */
    exec_sub,                           /* .S unit with reversed operators */

    exec_subdp,                         /* .S unit */
    exec_subdp,                         /* .S unit */
    exec_subdp,                         /* .S unit */
    exec_subdp,                         /* .S unit */

    exec_subsp,                         /* .L unit */
    exec_subsp,                         /* .L unit */
    exec_subsp,                         /* .S unit */
    exec_subsp,                         /* .S unit */

    exec_sub_d_x,
    exec_sub_d,
    exec_sub_d,

    exec_subab,
    exec_subab,
    exec_subabs4,
    exec_subah,
    exec_subah,
    exec_subaw,
    exec_subaw,
    exec_subc,
    exec_subu,
    exec_subu,
    exec_sub2,
    exec_sub2,
    exec_sub2,
    exec_sub4,

    exec_swap2,
    exec_swap2,
    exec_swap4,
    exec_swe,
    exec_swenr,
    exec_unpkbu4,
    exec_unpkbu4,
    exec_unpkh2,
    exec_unpkh2,
    exec_unpkhu2,
    exec_unpkhu2,
    exec_unpkhu4,
    exec_unpkhu4,
    exec_unpklu4,
    exec_unpklu4,
    exec_xor,
    exec_xor,
    exec_xor,
    exec_xor,
    exec_xor,
    exec_xor,

    exec_xormpy,

    exec_absdp,
    exec_abssp,

    exec_add_l,
    exec_add_l,
    exec_add_l,
    exec_add_l,
    exec_add_l,

    exec_add_s,
    exec_add_s,
    exec_add_s,
    exec_zero,
    NULL,
};

static void exec_sploop_insn(c6k_core_t * core, uint64_t insn, int *iic)
{
    opcode_fields_t fields;
    int64_t dummy;
    int32_t index;

    if (insn & 0xFFFFFFFF00000000LL)
    {
        core->header = (insn >> 32);
        insn &= 0xFFFFFFFF;
        if (!(insn & 0xFFFF0000))
        {
            insn &= 0x0000FFFF;
            TRANS_COMPACT_FIELDS(fields, (core->header & 0x1), (core->header >> 14));
            insn_action_t action = get_insn16_action(core, insn, fields.compact_br, &index, &dummy);

            action(core, insn, &fields);
            if (!(fields.compact_p))
            {
                *iic += 1;
                write_back(core);
                CYCLE_INC(core, 1);
            }
            return;
        }
        // should get header protect info for legacy ld insns
        TRANS_COMPACT_FIELDS(fields, 0 /*dummy */ , (core->header >> 14));
    }
    trans_opcode_fields((uint32_t) insn, &fields);
    exec_32b_insn(core, (uint32_t) insn, &fields, &dummy);
    if ((insn & 0x1) == 0x0)
    {
        *iic += 1;
        write_back(core);
        CYCLE_INC(core, 1);
    }
}

static void exec_bb_sploop_cache(c6k_core_t * core, insn_cache_t * cache, int *iic)
{
    int pbit;
    uint32_t insn = cache->insn;

    core->header = cache->header;

    pbit = cache->type == INSN_REGULAR ? insn & 0x1 : cache->fields.compact_p;

    DBG("exec_bb_sploop_cache: pc = %08x, insn = %08x ..\n", core->pc, cache->insn);

    cache->fields.exec_unit = 0xFFFF;
    cache->action(core, insn, &(cache->fields));

    if (pbit == 0)
    {
        *iic += 1;
    }
}

#define CLEAR_SPLOOP_VARIABLES(core) { \
	(core)->buffer_pos = 0; \
	(core)->buffer_index = 0; \
	(core)->sploop_flag = 0; \
	(core)->sploop_begin = 0xFFFFFFFF; \
	(core)->sploop_end = 0xFFFFFFFF; \
	(core)->sploop_outside = 0xFFFFFFFF; \
	(core)->spmask = 0; \
	(core)->spmask_begin = 0; \
	(core)->sploop_ii = 0; \
	(core)->sploop_curr_ii = 0; \
	(core)->sploop_halt = 0; \
	(core)->ilc = 0; }

static inline void execute_legacy_sploop(c6k_core_t * core)
{
    uint32_t i, iic = 0;

    if (core->sploop_flag == SPLOOP)
    {
        while (core->ilc)
        {
            for (i = core->buffer_index; i < core->buffer_pos; i++)
            {
                core->pc = core->sploop_pc[i];
                exec_sploop_insn(core, core->legacy_sploop_buffer[i], &iic);
                core->insn_num++;

                core->buffer_index++;
                if (i == core->buffer_pos - 1)
                    core->buffer_index = 0;
                core->pc = core->sploop_pc[core->buffer_index];

                //breakpoint
                if (check_pc_hit(&(core->bp_manager), core->pc, core->insn_num))
                {
                    DBG("-------pc is %x\n", core->pc);
                    return;
                }
                //stepi                
                if (core->stop_step != 0 && core->insn_num != 0 && core->stop_step == core->insn_num - 1)
                {
                    core->insn_num--;
                    return;
                }

            }
        }
    }

    if (core->sploop_flag == SPLOOPD)
    {
        while (core->ilc)
        {
            for (i = core->buffer_index; i < core->buffer_pos; i++)
            {
                core->pc = core->sploop_pc[i];
                exec_sploop_insn(core, core->legacy_sploop_buffer[i], &iic);
                core->insn_num++;

                core->buffer_index++;
                if (i == core->buffer_pos - 1)
                {
                    core->buffer_index = 0;
                    core->ilc--;
                }
                core->pc = core->sploop_pc[core->buffer_index];

                //breakpoint
                if (check_pc_hit(&(core->bp_manager), core->pc, core->insn_num))
                {
                    DBG("-------pc is %x\n", core->pc);
                    return;
                }
                //stepi                
                if (core->stop_step != 0 && core->insn_num != 0 && core->stop_step == core->insn_num - 1)
                {
                    core->insn_num--;
                    return;
                }

            }
        }
    }

    /*      if (core->sploop_flag == SPLOOPD) {
     * uint32_t j = 0;
     * uint32_t index = core->sploop_ii;
     * while (core->ilc) {
     * for(i = core->start_loop; i < core->buffer_pos_a; i++) {
     * for (j = 0; j < core->pos_size[i];j++){
     * core->pc = core->sploopd_pc[i][j];
     * exec_sploop_insn(core, core->sploopd_buffer[i][j], &iic);
     * }
     * // run stage 2 also run stage 1 instruction
     * if (i >= index){
     * for (j = 0;j < core->pos_size[i - index]; j++){
     * core->pc = core->sploopd_pc[i - index][j];
     * exec_sploop_insn(core, core->sploopd_buffer[i - index][j], &iic);
     * core->start_loop = i - index + 1;
     * }
     * }
     * }
     * }
     * }
     */

    core->pc = core->sploop_outside;
    DBG("outside pc is %x\n", core->pc);
    CLEAR_SPLOOP_VARIABLES(core);
}

static inline void execute_legacy_sploopw(c6k_core_t * core)
{
    uint32_t iic, i = core->buffer_index;
    uint32_t run_time = 0;

    while (calc_cond(core, core->sploopw_cond))
    {
        iic = core->sploop_curr_ii;
        while (iic < core->sploop_ii)
        {
            core->pc = core->sploop_pc[i % core->buffer_pos];
            exec_sploop_insn(core, core->legacy_sploop_buffer[i % core->buffer_pos], &iic);
            core->insn_num++;
            core->sploop_curr_ii = iic;
            i++;
            core->buffer_index = i;
            core->pc = core->sploop_pc[i % core->buffer_pos];

            //breakpoint 
            if (check_pc_hit(&(core->bp_manager), core->pc, core->insn_num))
            {
                DBG("-------pc is %x\n", core->pc);
                return;
            }
            //stepi
            if (core->stop_step != 0 && core->insn_num != 0 && core->stop_step == core->insn_num - 1)
            {
                core->insn_num--;
                return;
            }

            if (!(i % core->buffer_pos))
            {
                break;
            }
        }
        core->sploop_curr_ii = 0;
        run_time += 1;
    }

    if (run_time > 0)
    {
        for (i = i % core->buffer_pos; i < core->buffer_pos; i++)
        {
            core->pc = core->sploop_pc[i % core->buffer_pos];
            exec_sploop_insn(core, core->legacy_sploop_buffer[i % core->buffer_pos], &iic);
        }
    }

    core->pc = core->sploop_outside;
    DBG("outside pc is %x\n", core->pc);
    CLEAR_SPLOOP_VARIABLES(core);
}

static inline void execute_bb_sploop(c6k_core_t * core)
{
    uint32_t i, iic = 0;

    if (core->sploop_flag == SPLOOPD)
        core->ilc += 4;

    while (core->ilc)
    {
        for (i = 0; i < core->buffer_pos; i++)
        {
            core->pc = core->sploop_pc[i];
            exec_bb_sploop_cache(core, (insn_cache_t *) core->bb_sploop_buffer[i], &iic);
        }
    }

    core->pc = core->sploop_outside;
    CLEAR_SPLOOP_VARIABLES(core);

}

static inline void execute_bb_sploopw(c6k_core_t * core)
{
    uint32_t iic, i = 0;

    while (calc_cond(core, core->sploopw_cond))
    {
        iic = 0;
        while (iic < core->sploop_ii)
        {
            core->pc = core->sploop_pc[i % core->buffer_pos];
            exec_bb_sploop_cache(core, (insn_cache_t *) core->bb_sploop_buffer[i % core->buffer_pos], &iic);
            i++;
            if (!(i % core->buffer_pos))
            {
                break;
            }
        }
    }

    core->pc = core->sploop_outside;
    CLEAR_SPLOOP_VARIABLES(core);

}

static inline uint32_t test_and_switch(c6k_core_t * core)
{
    if (core->register_branch_event == legacy_register_branch_event)
        return 0;

    core->register_branch_event = legacy_register_branch_event;
    core->register_regw_event_h = legacy_register_regw_event;
    core->register_regw_event_l = legacy_register_regw_event;
    core->register_memory_event = legacy_register_memory_event;
    core->cycle_inc = legacy_cycle_inc;
    core->add_pending_nops = legacy_add_pending_nops;

    return 1;
}

static inline void switch_back(c6k_core_t * core, uint32_t need_switch)
{
    if (!need_switch)
        return;

    core->register_branch_event = bb_register_branch_event;
    core->register_regw_event_h = bb_register_regw_event_h;
    core->register_regw_event_l = bb_register_regw_event_l;
    core->register_memory_event = bb_register_memory_event;
    core->cycle_inc = bb_cycle_inc;
    core->add_pending_nops = bb_add_pending_nops;
}

static inline void legacy_load_to_sploop_buffer(c6k_core_t * core, int pbit,
                                                insn_cache_t * cache, uint32_t header, uint32_t pc, uint32_t insn)
{
    uint32_t skip = 0;

    if (core->sploop_begin_paralleled)
    {
        // filter out the insns which paralleled with sploop/d/w
        if (!pbit)
        {
            core->sploop_begin_paralleled = 0;
            /* sploop_begin labels the sploop, and the next insn
             * will be the first insn in sploop buffer
             */
            core->sploop_begin = pc;
        }
    } else if (pc > core->sploop_begin)
    {
        int pos = core->buffer_pos;

        // FIXME, there's have unmasked insns in the row, but need cooperate by specific exec_insn
        if (core->spmask)
        {
            skip = core->spmask & cache->fields.exec_unit;

            if (!pbit)
                core->spmask = 0;
        }

        if (!skip || pc == core->spmask_begin)
        {
            /* label compact insn's header with bit[0] = 0, which means non parallel */
            if (cache->type == INSN_COMPACT)
                header &= 0xFFFFFFFE;

            if (pc == core->spmask_begin)
            {
                /* label regular spmask insn with bit[0] = 0, which means non parallel */
                if (cache->type == INSN_REGULAR)
                    insn &= 0xFFFFFFFE;
            } else if ((cache->type == INSN_COMPACT) && pbit)
            {
                /* non spmask, compact insn with parallel, set header[0] = 1 */
                header |= 0x1;
            }

            core->legacy_sploop_buffer[pos] = (((uint64_t) header) << 32) | insn;

            core->sploop_pc[pos] = pc;
            core->buffer_pos++;
            DBG("legacy buf pc is %x pos %d\n", pc, core->buffer_pos - 1);
        }

    }
#if 0
    int pos_a = core->buffer_pos_a;
    int pos_b = core->buffer_pos_b;

    // FIXME, there's have unmasked insns in the row, but need cooperate by specific exec_insn
    if (core->spmask)
    {
        skip = core->spmask & cache->fields.exec_unit;
        if (!pbit)
            core->spmask = 0;
    }

    if (!skip || pc == core->spmask_begin)
    {
        /* label compact insn's header with bit[0] = 0, which means non parallel */
        if (cache->type == INSN_COMPACT)
            header &= 0xFFFFFFFE;

        if (pc == core->spmask_begin)
        {
            /* label regular spmask insn with bit[0] = 0, which means non parallel */
            if (cache->type == INSN_REGULAR)
                insn &= 0xFFFFFFFE;
        } else if ((cache->type == INSN_COMPACT) && pbit)
        {
            /* non spmask, compact insn with parallel, set header[0] = 1 */
            header |= 0x1;
        }

        core->sploopd_buffer[pos_a][pos_b] = (((uint64_t) header) << 32) | insn;

        core->sploopd_pc[pos_a][pos_b] = pc;
        core->buffer_pos_b++;
        if (pbit == 0)
        {
            core->pos_size[pos_a] = core->buffer_pos_b;
            core->buffer_pos_a++;
            core->buffer_pos_b = 0;
        }
    }
#endif

    if (core->sploop_end != 0xFFFFFFFF && (pbit == 0))
    {
        uint32_t need_switch;
        uint32_t mini_nums[4] = { 4, 2, 2, 1 };
        uint32_t idx = core->sploop_ii - 1;

        core->sploop_outside = core->pc;
        core->sploop_halt = 1;

        need_switch = test_and_switch(core);
        switch (core->sploop_flag)
        {
            case SPLOOPW:
                execute_legacy_sploopw(core);
                switch_back(core, need_switch);
                break;
            case SPLOOP:
                execute_legacy_sploop(core);
                switch_back(core, need_switch);
                break;
            case SPLOOPD:
                if (idx > 3)
                    idx = 3;
                if (core->ilc > 0)
                    core->ilc += mini_nums[idx] - 1;
                execute_legacy_sploop(core);
                switch_back(core, need_switch);
                break;
            default:
                break;
        }
    }
}

static inline void bb_load_to_sploop_buffer(c6k_core_t * core, int pbit,
                                            insn_cache_t * cache, uint32_t header, uint32_t pc, uint32_t insn)
{
    uint32_t skip = 0;

    if (core->sploop_begin_paralleled)
    {
        // filter out the insns which paralleled with sploop/d/w
        if (!pbit)
        {
            core->sploop_begin_paralleled = 0;
            /* sploop_begin labels the sploop, and the next insn
             * will be the first insn in sploop buffer
             */
            core->sploop_begin = pc;
        }
    } else if (pc > core->sploop_begin)
    {
        int pos = core->buffer_pos;

        // FIXME, there's have unmasked insns in the row, but need cooperate by specific exec_insn
        if (core->spmask)
        {
            skip = core->spmask & cache->fields.exec_unit;
            if (!pbit)
                core->spmask = 0;
        }

        if (!skip || pc == core->spmask_begin)
        {
            /* label compact insn's header with bit[0] = 0, which means non parallel */
            if (cache->type == INSN_COMPACT)
                header &= 0xFFFFFFFE;

            if (pc == core->spmask_begin)
            {
                /* label regular spmask insn with bit[0] = 0, which means non parallel */
                if (cache->type == INSN_REGULAR)
                    insn &= 0xFFFFFFFE;
            } else if ((cache->type == INSN_COMPACT) && pbit)
            {
                /* non spmask, compact insn with parallel, set header[0] = 1 */
                header |= 0x1;
            }

            core->bb_sploop_buffer[pos] = cache;

            core->sploop_pc[pos] = pc;
            core->buffer_pos++;
            DBG("bb buf pc is %x pos %d\n", pc, core->buffer_pos - 1);
        }
    }

    if ((core->sploop_end != 0xFFFFFFFF) && (pbit == 0))
    {
        uint32_t mini_nums[4] = { 4, 2, 2, 1 };
        uint32_t idx = core->sploop_ii - 1;

        core->sploop_outside = core->pc;
        core->sploop_halt = 1;

        switch (core->sploop_flag)
        {
            case SPLOOPW:
                execute_bb_sploopw(core);
                break;
            case SPLOOP:
                execute_bb_sploop(core);
                break;
            case SPLOOPD:
                if (idx > 3)
                    idx = 3;
                if (core->ilc > 0)
                    execute_bb_sploop(core);
                break;
            default:
                break;
        }
    }
}

uint32_t legacy_exec_insn_cache(c6k_core_t * core, insn_cache_t * cache)
{
    int pbit = 0;
    uint32_t insn = cache->insn;
    uint32_t pc = core->pc;
    uint32_t header;

    const char *insn_types[] = {
        "INSN_REGULAR", "INSN_COMPACT", "INSN_HEADER", "INSN_CHECKPOINT_WB",
        "INSN_CHECKPOINT_LD", "INSN_CHECKPOINT_ST", "INSN_CHECKPOINT_B",
        "INSN_CHECKPOINT_REGWL", "INSN_CHECKPOINT_REGWH"
    };
    if (cache->type >= INSN_HEADER)
    {
        if (cache->type == INSN_HEADER)
        {
            core->pc += 4;
        }
        return 0;
    }

    DBG("Lcache@%016lx exec insn, pc = %08x, addr = %08x, insn = %08x, insn_type = %s\n",
        (uint64_t) cache, core->pc, cache->addr, cache->insn, insn_types[cache->type - 1]);

    if (!core->sploop_halt)
    {

        header = core->header = cache->header;

        pbit = cache->type == INSN_REGULAR ? insn & 0x1 : cache->fields.compact_p;

        cache->fields.exec_unit = 0xFFFF;
        cache->action(core, insn, &(cache->fields));

        if (pbit == 0)
        {
            write_back(core);
        }

        legacy_load_to_sploop_buffer(core, pbit, cache, header, pc, insn);

        if (pbit == 0)
        {
            CYCLE_INC(core, 1);
        }

        if (pbit == 0)
            PRINT_ALL_GPR_TO_LOG(core);

    } else
    {
        switch (core->sploop_flag)
        {
            case SPLOOPW:
                execute_legacy_sploopw(core);
                break;
            case SPLOOP:
                execute_legacy_sploop(core);
                break;
            case SPLOOPD:
                execute_legacy_sploop(core);
                break;
            default:
                break;
        }

    }
    /* if we reach new packet region */
    if ((core->pc & 0x1f) == 0)
    {
        return 0;
    }
    return 0;
}

uint32_t bb_exec_insn_cache(c6k_core_t * core, insn_cache_t * cache)
{
    int pbit = 0;
    uint32_t insn = cache->insn;
    uint32_t pc = core->pc;
    uint32_t skip = 0;
    uint32_t header;

    const char *insn_types[] = {
        "INSN_REGULAR", "INSN_COMPACT", "INSN_HEADER", "INSN_CHECKPOINT_WB",
        "INSN_CHECKPOINT_LD", "INSN_CHECKPOINT_ST", "INSN_CHECKPOINT_B",
        "INSN_CHECKPOINT_REGWL", "INSN_CHECKPOINT_REGWH"
    };
    DBG("Bcache@%016lx exec insn, pc = %08x, addr = %08x, insn = %08x, insn_type = %s\n",
        (uint64_t) cache, core->pc, cache->addr, cache->insn, insn_types[cache->type - 1]);

    if (cache->type == INSN_HEADER)
    {
        core->pc += 4;
        return 0;
    }

    if (!core->sploop_halt)
    {

        header = core->header = cache->header;

        cache->fields.exec_unit = 0xFFFF;
        cache->action(core, insn, &(cache->fields));

        if (cache->type == INSN_REGULAR)
        {
            pbit = insn & 0x1;
        } else if (cache->type == INSN_COMPACT)
        {
            pbit = cache->fields.compact_p;
        } else if (cache->type > INSN_HEADER)
        {
            return 0;
        }

        legacy_load_to_sploop_buffer(core, pbit, cache, header, pc, insn);

        if (pbit == 0)
        {
            CYCLE_INC(core, 1);
        }

    } else
    {
        uint32_t need_switch = test_and_switch(core);

        switch (core->sploop_flag)
        {
            case SPLOOPW:
                execute_legacy_sploopw(core);
                switch_back(core, need_switch);
                break;
            case SPLOOP:
                execute_legacy_sploop(core);
                switch_back(core, need_switch);
                break;
            case SPLOOPD:
                execute_legacy_sploop(core);
                switch_back(core, need_switch);
                break;
            default:
                break;
        }

    }

    /* if we reach new packet region */
    if ((core->pc & 0x1f) == 0)
    {
        return 0;
    }
    return 0;
}

int get_insn_index(uint32_t insn)
{
    int index;
    int64_t delay_slot;
    int ret = decode_instr(insn, &index, (ISEITEM *) insn32_decode_table,
                           sizeof (insn32_decode_table) / sizeof (ISEITEM),
                           -1, &delay_slot);

    if (ret == DECODE_SUCCESS)
        return index;
    else
    {
        skyeye_log(Error_log, __FUNCTION__, "Can't implement the instruction 0x%x \n", insn);
        return -1;
    }
}

int get_insn_number()
{
    return sizeof (insn32_decode_table) / sizeof (ISEITEM);
}

char *get_insn_name(int index)
{
    if (index < get_insn_number())
    {
        return insn32_decode_table[index].name;
    } else if (index >= get_insn_number() && index < (get_insn_number() + get_insn16_number()))
    {
        return get_insn16_name(index - get_insn_number());
    } else
        return NULL;
}

int32_t get_insn_delay_slot(uint32_t insn)
{
    int index, max;

    index = get_insn_index(insn);
    max = get_insn_number();
    if (index < 0)
        return -1;
    if (index > max)
        return -1;
    return insn32_decode_table[index].delay_slot;
}

insn_action_t get_insn_action(c6k_core_t * core, uint32_t insn, int32_t * index, int64_t * delay_slot)
{
    insn_action_t ret = NULL;

    DBG("In %s, core->pc=0x%x, insn=0x%x\n", __FUNCTION__, core->pc, insn);
    if (decode_instr(insn, index, (ISEITEM *) insn32_decode_table,
                     sizeof (insn32_decode_table) / sizeof (ISEITEM), -1, delay_slot) == DECODE_SUCCESS)
    {
        DBG("In %s, instr=0x%x, index=0x%x\n", __FUNCTION__, insn, *index);
        return insn_action[*index];
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "decode error for 0x%x at 0x%x\n", insn, core->pc);
        skyeye_exit(-1);
    }
    return ret;
}

int trans_opcode_fields(uint32_t insn, opcode_fields_t * fields)
{
    fields->op1_11 = BITS(1, 11);
    fields->op2_11 = BITS(2, 11);
    fields->op4_6 = BITS(4, 6);
    fields->op5_11 = BITS(5, 11);
    fields->op6_11 = BITS(6, 11);
    fields->op6_9 = BITS(6, 9);
    fields->op7_11 = BITS(7, 11);
    fields->op7_12 = BITS(7, 12);

    fields->src13_16 = BITS(13, 16);

    /* src1 */
    fields->src13_17 = BITS(13, 17);
    fields->src13_15 = BITS(13, 15);

    /* src2 */
    fields->src18_22 = BITS(18, 22);
    fields->src16_27 = BITS(16, 27);

    fields->dst23_27 = BITS(23, 27);
    fields->dst24_27 = BITS(24, 27);

    fields->s = BITS(1, 1);
    fields->x = BITS(12, 12);
    fields->y = BITS(7, 7);
    fields->p = BITS(0, 0);
    fields->sc = BITS(23, 23);
    fields->ii = BITS(23, 27);
    fields->mode = BITS(9, 12);

    fields->base = BITS(18, 22);
    fields->offset = BITS(13, 17);

    fields->cst7_22 = BITS(7, 22);

    fields->scst7_22 = BITS(7, 22);
    fields->scst7_27 = BITS(7, 27);
    fields->scst16_22 = BITS(16, 22);
    fields->scst18_22 = BITS(18, 22);

    fields->ucst13_15 = BITS(13, 15);
    fields->ucst8_22 = BITS(8, 22);

    fields->csta = BITS(13, 17);
    fields->cstb = BITS(8, 12);
    fields->crlo18_22 = BITS(18, 22);
    fields->crlo23_27 = BITS(23, 27);
    fields->fstg_fcyc = BITS(22, 27);

    return 0;
}

int exec_32b_insn(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields, int64_t * delay_slot)
{
    int index;
    int ret;

    DBG("In %s, core->pc=0x%x, insn=0x%x\n", __FUNCTION__, core->pc, insn);
    if ((ret = decode_instr(insn, &index, (ISEITEM *) insn32_decode_table,
                            sizeof (insn32_decode_table) / sizeof (ISEITEM), -1, delay_slot)) == DECODE_SUCCESS)
    {
        DBG("In %s, instr=0x%x, index=0x%x\n", __FUNCTION__, insn, index);
        insn_action[index] (core, insn, fields);
    } else
    {
        DBG("In %s, decode error for 0x%x at 0x%x\n", __FUNCTION__, insn, core->pc);
        skyeye_exit(-1);
    }
    return ret;
}

/*
 * br: -1, don't care about it in 32bits insns
 *     0/1, if decodes compact insns, the fetch header's bits[15] is br's value
 */
static inline int is_target_slot(ISEITEM * slot, int br, int64_t * delay_slot)
{
    int magics[2] = { NONBR_INSN_IN_UNIT_S, BR_INSN_IN_UNIT_S };
    int i = slot->attribute_value * 3;

    if (br == -1)
    {
        *delay_slot = slot->delay_slot;
        return DECODE_SUCCESS;
    }

    if (slot->content[i] == 0)
    {
        *delay_slot = slot->delay_slot;
        return DECODE_SUCCESS;
    } else if (magics[br] == slot->content[i])
    {
        *delay_slot = slot->delay_slot;
        return DECODE_SUCCESS;
    }

    return DECODE_FAILURE;
}

int decode_instr(uint32_t insn, int32_t * idx, ISEITEM * table, int table_len, int br, int64_t * delay_slot)
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
            ret = is_target_slot(slot, br, delay_slot);

        if (ret == DECODE_SUCCESS)
        {
            *idx = i;
            //printf("In %s, instr_name :%s, insn:%x\n", __FUNCTION__, slot->name, insn);
            return ret;
        }
    }
    return ret;
}
int check_amr_mode(c6k_core_t * core, int s, int src_mode)
{
    int mode = 0;

    if (s == 0)
    {
        switch (src_mode)
        {
            case 4:
                if ((core->amr & 0x3) == 0x1)
                    mode = BK0;
                else if ((core->amr & 0x3) == 0x2)
                    mode = BK1;
            case 5:
                if ((core->amr & 0xc) == 0x4)
                    mode = BK0;
                else if ((core->amr & 0xc) == 0x8)
                    mode = BK1;
            case 6:
                if ((core->amr & 0x30) == 0x10)
                    mode = BK0;
                else if ((core->amr & 0x30) == 0x20)
                    mode = BK1;
            case 7:
                if ((core->amr & 0xc0) == 0x40)
                    mode = BK0;
                else if ((core->amr & 0xc0) == 0x80)
                    mode = BK1;
        }
    } else
    {
        switch (src_mode)
        {
            case 4:
                if ((core->amr & 0x300) == 0x100)
                    mode = BK0;
                else if ((core->amr & 0x300) == 0x200)
                    mode = BK1;
            case 5:
                if ((core->amr & 0xc00) == 0x400)
                    mode = BK0;
                else if ((core->amr & 0xc00) == 0x800)
                    mode = BK1;
            case 6:
                if ((core->amr & 0x3000) == 0x1000)
                    mode = BK0;
                else if ((core->amr & 0x3000) == 0x2000)
                    mode = BK1;
            case 7:
                if ((core->amr & 0xc000) == 0x4000)
                    mode = BK0;
                else if ((core->amr & 0xc000) == 0x8000)
                    mode = BK1;
        }
    }
    return mode;
}
