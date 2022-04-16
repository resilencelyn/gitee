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
* @file c6k_generic_core.c
* @brief The core of c6678
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-09-03
*/
#include "skyeye_internal.h"
#include "skyeye_types.h"
#include "skyeye_config.h"
#include "skyeye_options.h"
#include <skyeye_exec.h>
#include <skyeye_cell.h>
#include <skyeye_arch.h>
#include <skyeye_mm.h>
#include <skyeye_obj.h>
#include <skyeye_class.h>
#include <skyeye_disassemble.h>
#include <skyeye_iface.h>
#include <skyeye_callback.h>
#include <skyeye_core_intf.h>
#include <skyeye_int_register.h>
#include <skyeye_attribute.h>
#include <sim_control.h>
#include "c6k_cpu.h"
#include "c6k_decode.h"
//#include "dyncom/c6kplus_dyncom_run.h"
#undef PRINT_LOG
#define IS_COMPACT_LINE(layout, row)	(((layout) >> (row)) & 0x1)
#define IS_PARALLEL(parallel, order)	(((parallel) >> (order)) & 0x1)

static fetch_context_t *alloc_fetch_context(c6k_core_t * core)
{
    fetch_context_t *fc = (fetch_context_t *) skyeye_mm_zero(sizeof (fetch_context_t));

    fc->insn_cache = (insn_cache_t *) skyeye_mm_zero(sizeof (insn_cache_t) * core->insn_cache_size);
    fc->bb_cache = (basic_block_t *) skyeye_mm_zero(sizeof (basic_block_t) * core->insn_cache_size);
    fc->cache_size = core->insn_cache_size;
    fc->curr_ic = fc->insn_cache;
    fc->curr_bb = INVALID_CURR_BB(fc);
    return fc;
}

static void free_fetch_context(c6k_core_t * core)
{
    fetch_context_t *fc = (fetch_context_t *) core->fetch_context;

    skyeye_free(fc->insn_cache);
    skyeye_free(fc->bb_cache);
    skyeye_free(fc);
}

void invalidate_bb_cache(c6k_core_t * core)
{
    fetch_context_t *fc = (fetch_context_t *) core->fetch_context;

    SET_CURR_BB(fc, INVALID_CURR_BB(fc));
}

static int exec_branch_out(c6k_core_t * core, uint32_t unused, opcode_fields_t * unused2)
{
    fetch_context_t *fc = (fetch_context_t *) core->fetch_context;

    if (core->br_target != BAD_ADDR)
    {
        DBG("pc: %08x branch out with new pc: %08x, BO_ENTRY = %08x ....\n",
            core->pc, core->br_target, HASH(fc, core->br_target));
        core->pc = core->br_target;
        core->br_target = BAD_ADDR;

        /* The branch sometimes will triggered in sploop,
         * so we have to reset the sploop buffer */
        core->sploop_begin = core->sploop_end = core->sploop_outside = BAD_ADDR;
    } else
    {
        DBG("pc: %08x FAKE branch out with new pc: %08x ....\n", core->pc, BAD_ADDR);
    }

    invalidate_bb_cache(core);

    return 0;
}

static inline void block_complete(fetch_context_t * fc, uint32_t block_entry)
{
    uint32_t i;

    SET_BB_TYPE(fc, SIMPLE_BB, block_entry);
    for (i = 0; i < DELAY_SLOT_NUM; i++)
    {
        if (fc->delay_slot_queue[i])
        {
            SET_BB_TYPE(fc, LEGACY_BB, block_entry);
        }
    }

    BB_END(fc);
}

static inline insn_cache_t *add_checkpoint_func(fetch_context_t * fc, insn_action_t action, uint32_t steps,
                                                uint32_t type)
{
#define DRAIN_BUFFER	0xFFFFFFFF
    const char *insn_types[] = {
        "INSN_REGULAR", "INSN_COMPACT", "INSN_HEADER", "INSN_CHECKPOINT_WB",
        "INSN_CHECKPOINT_LD", "INSN_CHECKPOINT_ST", "INSN_CHECKPOINT_B",
        "INSN_CHECKPOINT_REGWL", "INSN_CHECKPOINT_REGWH"
    };
    insn_cache_t *cache = GET_CURR_IC(fc);

    INC_CURR_IC(fc);
    cache->type = type;
    cache->insn = steps;
    cache->action = action;
    DBG("add_checkpoint_func: cache@%016lx type: %s \n", (uint64_t) cache, insn_types[type - 1]);

    return cache;
}

static void update_status(insn_cache_t * cache, fetch_context_t * fc, uint32_t block_entry)
{
    int nops = 0;
    uint32_t curr_dsq_idx = fc->curr_dsq_idx;
    uint64_t *delay_slot_queue = fc->delay_slot_queue;
    uint32_t steps, i, reg_addr;

    //      insn_cache_t *log_cache = add_checkpoint_func(fc, exec_write_back, DRAIN_BUFFER, INSN_CHECKPOINT_WB);

    if (delay_slot_queue[curr_dsq_idx] & NOP_SLOT)
    {
        nops = GET_NP_CNT(delay_slot_queue[curr_dsq_idx]);
        CLR_NP_SLOT(delay_slot_queue[curr_dsq_idx]);
    }

    do
    {
        DBG("cache->addr = %08x: call update_status @queue[%d] = %16lx ...\n",
            cache->addr, curr_dsq_idx, delay_slot_queue[curr_dsq_idx]);
        /*
         * if (delay_slot_queue[curr_dsq_idx] & SGL_SLOT) {
         * steps = GET_SGL_CNT(delay_slot_queue[curr_dsq_idx]);
         * for (i = 0; i < steps; i++) {
         * FC_REGWL_POP(fc, reg_addr,curr_dsq_idx);
         * log_cache = add_checkpoint_func(fc, exec_regw_l, reg_addr, INSN_CHECKPOINT_REGWL);
         * }
         * CLR_SGL_SLOT(delay_slot_queue[curr_dsq_idx]);
         * }
         * 
         * if (delay_slot_queue[curr_dsq_idx] & SGH_SLOT) {
         * steps = GET_SGH_CNT(delay_slot_queue[curr_dsq_idx]);
         * for (i = 0; i < steps; i++) {
         * FC_REGWH_POP(fc, reg_addr,curr_dsq_idx);
         * log_cache = add_checkpoint_func(fc, exec_regw_h, reg_addr, INSN_CHECKPOINT_REGWH);
         * }
         * CLR_SGH_SLOT(delay_slot_queue[curr_dsq_idx]);
         * }
         * 
         * if (delay_slot_queue[curr_dsq_idx] & LD_SLOT) {
         * steps = GET_LD_CNT(delay_slot_queue[curr_dsq_idx]);
         * DBG("GET_LD_CNT: steps = %d ...\n", steps);
         * for (i = 0; i < steps; i++) {
         * FC_MEMR_POP(fc, reg_addr);
         * log_cache = add_checkpoint_func(fc, exec_read_memory, reg_addr, INSN_CHECKPOINT_LD);
         * }
         * CLR_LD_SLOT(delay_slot_queue[curr_dsq_idx]);
         * }
         * 
         * if (delay_slot_queue[curr_dsq_idx] & ST_SLOT) {
         * steps = GET_ST_CNT(delay_slot_queue[curr_dsq_idx]);
         * for (i = 0; i < steps; i++) {
         * FC_MEMW_POP(fc, reg_addr);
         * log_cache = add_checkpoint_func(fc, exec_write_memory, reg_addr, INSN_CHECKPOINT_ST);
         * }
         * CLR_ST_SLOT(delay_slot_queue[curr_dsq_idx]);
         * }
         * if (delay_slot_queue[curr_dsq_idx] & B_SLOT) {
         * steps = GET_BR_CNT(delay_slot_queue[curr_dsq_idx]);
         * for (i = 0; i < steps; i++) {
         * log_cache = add_checkpoint_func(fc, exec_branch_out, 1, INSN_CHECKPOINT_B);
         * DBG("cache->addr: %08x is a br_func SLOT ...\n", cache->addr);
         * }
         * CLR_BR_SLOT(delay_slot_queue[curr_dsq_idx]);
         * log_cache->addr = cache->addr;
         * SET_CURR_BB_TAIL(fc, log_cache);
         * 
         * block_complete(fc);
         * }
         */

        if (delay_slot_queue[curr_dsq_idx] & B_SLOT)
        {
            CLR_BR_SLOT(delay_slot_queue[curr_dsq_idx]);
            SET_BB_TAIL(fc, cache, block_entry);

            block_complete(fc, block_entry);
        }

        curr_dsq_idx = (curr_dsq_idx + 1) % DELAY_SLOT_NUM;
        DBG("after update_status, curr_dsq_idx = %d ....\n", curr_dsq_idx);
    } while (nops-- > 0);
    fc->curr_dsq_idx = curr_dsq_idx;
}

static void push_delay_slot(uint64_t delay_slot, insn_cache_t * cache, fetch_context_t * fc, int32_t prot)
{
    int32_t index;
    uint32_t insn = cache->insn;
    uint32_t curr_dsq_idx = fc->curr_dsq_idx;
    uint64_t *delay_slot_queue = fc->delay_slot_queue;

    if (delay_slot & MIXED_LS_SLOT)
    {
        int ds = delay_slot & 0x0000FFFFLL;

        if (BITS(ds, ds))
            delay_slot = LD_SLOT;
        else
            delay_slot = ST_SLOT;
    }

    DBG("push_delay_slot: delay_slot = %016lx ...\n", delay_slot);
    switch (delay_slot & SLOT_TYPE_MASK)
    {
            // two LDs in the same slot is the top
        case LD_SLOT:
            if (prot)
                SET_NP_SLOT(delay_slot_queue[curr_dsq_idx], 4);
            break;

            /*
             * case LD_SLOT:
             * if (prot)
             * SET_NP_SLOT(delay_slot_queue[curr_dsq_idx], 4);
             * index = (curr_dsq_idx + 4) % DELAY_SLOT_NUM;
             * SET_LD_SLOT(delay_slot_queue[index], 1);
             * FC_MEMR_PUSH(fc, cache->addr);
             * DBG("curr_dsq_idx = %d, push LD_SLOT to queue[%d], "
             * "slot = %16lx, cnt = %llu, insn = %08x, prot = %d\n",
             * curr_dsq_idx, index, delay_slot_queue[index],
             * GET_LD_CNT(delay_slot_queue[index]), insn, prot);
             * break;
             * case ST_SLOT:
             * index = (curr_dsq_idx + 4) % DELAY_SLOT_NUM;
             * SET_ST_SLOT(delay_slot_queue[index], 1);
             * FC_MEMW_PUSH(fc, cache->addr);
             * DBG("curr_dsq_idx = %d, push ST_SLOT to queue[%d], "
             * "slot = %16lx, cnt = %llu, insn = %08x\n",
             * curr_dsq_idx, index, delay_slot_queue[index],
             * GET_ST_CNT(delay_slot_queue[index]) , insn);
             * break;
             */
        case KPC_SLOT:
            SET_NP_SLOT(delay_slot_queue[curr_dsq_idx], (BITS(13, 15)));
            DBG("curr_dsq_idx = %d, push KPC_SLOT to queue[%d], slot = %16lx, insn = %08x\n",
                curr_dsq_idx, curr_dsq_idx, delay_slot_queue[curr_dsq_idx], insn);
            break;
        case NOP_SLOT:
            SET_NP_SLOT(delay_slot_queue[curr_dsq_idx], (BITS(13, 16)));
            DBG("curr_dsq_idx = %d, push NOP_SLOT to queue[%d], slot = %16lx, insn = %08x\n",
                curr_dsq_idx, curr_dsq_idx, delay_slot_queue[curr_dsq_idx], insn);
            break;
            /*
             * case DOUBLE_SLOT:
             * index = (curr_dsq_idx + (delay_slot & SLOT_CNT_MASK) - 1) % DELAY_SLOT_NUM;
             * SET_SGL_SLOT(delay_slot_queue[index], 1);
             * FC_REGWL_PUSH(fc, cache->addr,index);
             * DBG("curr_dsq_idx = %d, push SGL_SLOT to queue[%d], slot = %16lx, insn = %08x\n",
             * curr_dsq_idx, index, delay_slot_queue[index], insn);
             * index = (curr_dsq_idx + (delay_slot & SLOT_CNT_MASK)) % DELAY_SLOT_NUM;
             * SET_SGH_SLOT(delay_slot_queue[index], 1);
             * FC_REGWH_PUSH(fc, cache->addr,index);
             * DBG("curr_dsq_idx = %d, push SGH_SLOT to queue[%d], slot = %16lx, insn = %08x\n",
             * curr_dsq_idx, index, delay_slot_queue[index], insn);
             * break;
             * case SGDW_SLOT:
             * index = (curr_dsq_idx + (delay_slot & SLOT_CNT_MASK)) % DELAY_SLOT_NUM;
             * SET_SGL_SLOT(delay_slot_queue[index], 1);
             * SET_SGH_SLOT(delay_slot_queue[index], 1);
             * FC_REGWL_PUSH(fc, cache->addr,index);
             * FC_REGWH_PUSH(fc, cache->addr,index);
             * DBG("curr_dsq_idx = %d, push SGDW_SLOT to queue[%d], slot = %16lx, insn = %08x\n",
             * curr_dsq_idx, index, delay_slot_queue[index], insn);
             * break;
             * case SGL_SLOT:
             * index = (curr_dsq_idx + (delay_slot & SLOT_CNT_MASK)) % DELAY_SLOT_NUM;
             * SET_SGL_SLOT(delay_slot_queue[index], 1);
             * FC_REGWL_PUSH(fc, cache->addr,index);
             * DBG("curr_dsq_idx = %d, push SGL_SLOT to queue[%d], slot = %16lx, insn = %08x\n",
             * curr_dsq_idx, index, delay_slot_queue[index], insn);
             * break;
             * case FOUR_SLOT:
             * index = (curr_dsq_idx + (delay_slot & SLOT_CNT_MASK)) % DELAY_SLOT_NUM;
             * SET_SGL_SLOT(delay_slot_queue[index], 1);
             * SET_SGH_SLOT(delay_slot_queue[index], 1);
             * FC_REGWL_PUSH(fc, cache->addr,index);
             * FC_REGWH_PUSH(fc, cache->addr,index);
             * SET_SGL_SLOT(delay_slot_queue[index], 1);
             * SET_SGH_SLOT(delay_slot_queue[index], 1);
             * FC_REGWL_PUSH(fc, cache->addr,index);
             * FC_REGWH_PUSH(fc, cache->addr,index);
             * DBG("curr_dsq_idx = %d, push SGDW_SLOT to queue[%d], slot = %16lx, insn = %08x\n",
             * curr_dsq_idx, index, delay_slot_queue[index], insn);
             * break;
             */
        case B_SLOT:
            index = (curr_dsq_idx + 5) % DELAY_SLOT_NUM;
            SET_BR_SLOT(delay_slot_queue[index], 1);
            DBG("curr_dsq_idx = %d, push a B_SLOT to queue[%d], slot = %16lx, insn = %08x ..\n",
                curr_dsq_idx, index, delay_slot_queue[index], insn);
            break;
        case BNOP_SLOT:
            SET_NP_SLOT(delay_slot_queue[curr_dsq_idx], (BITS(13, 15)));
            index = (curr_dsq_idx + 5) % DELAY_SLOT_NUM;
            SET_BR_SLOT(delay_slot_queue[index], 1);
            DBG("curr_dsq_idx = %d, push a BNOP_SLOT to queue[%d], slot = %16lx, insn = %08x ..\n",
                curr_dsq_idx, index, delay_slot_queue[index], insn);
            break;
        case CALLP_SLOT:
            SET_NP_SLOT(delay_slot_queue[curr_dsq_idx], 5);
            index = (curr_dsq_idx + 5) % DELAY_SLOT_NUM;
            SET_BR_SLOT(delay_slot_queue[index], 1);
            DBG("curr_dsq_idx = %d, push a CALLP_SLOT to queue[%d], slot = %16lx, insn = %08x ..\n",
                curr_dsq_idx, index, delay_slot_queue[index], insn);
            break;
        default:
            DBG("push a NOTHING to queue[%d], slot = %16lx, insn = %08x ..\n", curr_dsq_idx,
                delay_slot_queue[curr_dsq_idx], insn);
            break;
    }
}

/* construct one BB indexed by pc, return the insn cache of BB start */
insn_cache_t *fetch_packet_to_cache(conf_object_t * conf_obj, uint32_t pc)
{
    c6k_core_t *core = (c6k_core_t *) conf_obj->obj;
    int status, i, header, has_header, half;
    uint32_t insns[8];
    uint32_t layout, exp = 0, pbit = 0, pbits;
    uint32_t addr = 0;
    insn_cache_t *cache;
    int32_t index;
    int64_t delay_slot;
    uint32_t base, half_init;
    uint32_t block_entry;
    fetch_context_t *fc = (fetch_context_t *) core->fetch_context;

  retry:
    block_entry = HASH(fc, pc);
    addr = pc & 0xFFFFFFE0;
    /* 
     *
     * Update the fc, initilized the cache indexed by block_entry 
     * start of the BB is set to curr_ic 
     * */
    INIT_FC(fc, block_entry);

    base = (pc - addr) / 4;
    half_init = (pc - addr) % 4 ? 1 : 0;
  loop:
    for (i = base; i < FP_SIZE; i++)
    {
        status = core->c6k_memory_intf->read(conf_obj, addr + i * 4, &insns[i], 32 / 8);
    }
    header = (insns[FP_SIZE - 1] >> 28) == 0xE ? insns[FP_SIZE - 1] : 0x0;

    if (header)
    {
        has_header = 1;
        uint32_t insn = header;

        layout = BITS(21, 27);
        exp = BITS(14, 20);
        pbits = BITS(0, 13);
    } else
        has_header = 0;

    i = base;
    half = half_init;
    addr += i * 4 + half * 2;
    /* fetch insn to the insn cache indicated by curr_ic */
    while ((i < (FP_SIZE - has_header)) && (!BB_COMPLETED(fc)))
    {
        cache = GET_CURR_IC(fc);
        if (CACHE_FULL == INC_CURR_IC(fc))
        {
            goto retry;
        }

        cache->fields.has_compact_fields = 0;
        cache->header = 0;
        if (has_header && IS_COMPACT_LINE(layout, i))
        {
            pbit = IS_PARALLEL(pbits, (2 * i + (half % 2)));
            cache->addr = addr;
            cache->insn = half % 2 == 0 ? insns[i] & 0x0000FFFF : insns[i] >> 16;
            TRANS_COMPACT_FIELDS(cache->fields, pbit, exp);
            cache->header = insns[FP_SIZE - 1];
            cache->action = get_insn16_action(core, cache->insn, cache->fields.compact_br, &index, &delay_slot);
            cache->type = INSN_COMPACT;
            cache->parallel = pbit;
            cache->index = index;
            cache->size = 2;

            //set flag for uspl/dr uspk uspm
            if (cache->index >= 43 && cache->index <= 47)
                SET_BB_SPLOOP(fc, 1, block_entry);

            DBG("block push addr = %08x, insn = %08x to steps %ld, pbit = %d, "
                "curr_dsq_idx = %u, curr_bb = %08x ...\n",
                addr, cache->insn, cache - fc->bb_cache[block_entry].start, pbit, fc->curr_dsq_idx, fc->curr_bb);
            push_delay_slot(delay_slot, cache, fc, cache->fields.compact_prot);

            if (!pbit)
                update_status(cache, fc, block_entry);

            addr += 2;
            if (++half % 2)
                continue;
        } else
        {
            cache->addr = addr;
            cache->insn = insns[i];
            trans_opcode_fields(insns[i], &(cache->fields));
            cache->action = get_insn_action(core, insns[i], &index, &delay_slot);
            cache->type = INSN_REGULAR;
            cache->parallel = (cache->insn & 0x1);
            cache->index = index;
            cache->size = 4;

            //set flag for sploop/w/d spkernel/r  spmask/r
            if (cache->index >= 422 && cache->index <= 428)
                SET_BB_SPLOOP(fc, 1, block_entry);

            DBG("block push addr = %08x, insn = %08x to steps %ld, prot = %d, "
                "curr_dsq_idx = %u, curr_bb = %08x ...\n",
                addr, cache->insn, cache - fc->bb_cache[block_entry].start,
                cache->fields.compact_prot, fc->curr_dsq_idx, fc->curr_bb);

            if (has_header)
            {
                TRANS_COMPACT_FIELDS(cache->fields, 0, exp);
                cache->header = insns[FP_SIZE - 1];

                push_delay_slot(delay_slot, cache, fc, cache->fields.compact_prot);
            } else
            {
                push_delay_slot(delay_slot, cache, fc, 0);
            }

            if (!(cache->insn & 0x1))
                update_status(cache, fc, block_entry);

            addr += 4;
        }
        i++;
    }

    // if block_complete, don't care about header
    if (has_header && (!BB_COMPLETED(fc)))
    {

        cache = GET_CURR_IC(fc);
        INC_CURR_IC(fc);
        cache->type = INSN_HEADER;
        cache->addr = addr;
        cache->size = 4;
        DBG("header cache->addr %x, cache @%016lx ..\n", cache->addr, (uint64_t) cache);
        cache->insn = header;
        addr += 4;                      // skip header
    }

    if (!BB_COMPLETED(fc))
    {
        base = half_init = 0;
        goto loop;                      /* read next packet */
    }

    return GET_BB_ENTRY(fc, block_entry);
}

static void check_intr(c6k_core_t * core)
{
    if (core->intr_flag)
        return;
    if ((core->ifr != 0 && (core->csr & GIE_BIT)) || (core->ifr & 0x2))
    {
        int slot_id = 0;

        for (; slot_id < DELAY_SLOT_NUM; slot_id++)
        {
            /* can not taken when some event still exist in the slot */
            if (core->delay_slot_cycle[slot_id] != 0)
            {
                return;
            }
        }

        int i = 0;

        for (; i < 16; i++)
        {
            if (((core->ifr >> i) & 0x1) && (core->ier & (1 << i)))
            {
                break;
            }

        }

        if (i < 16)
        {
            /* save the intr return address and jump to intr handler */
            if (i == 1)
            {
                core->ntsr = core->tsr;
                //software exception 
                core->nrp = core->pc;
            } else
            {
                core->itsr = core->tsr;
                core->irp = core->pc;   /* save the return address */
            }
            //DBG("In %s, ************ INTR Happed core->csr == 0x%x,tsr=0x%x saved itsr=0x%x\n", __FUNCTION__, core->csr, core->tsr, core->itsr);
            core->pc = core->istp + i * 32;
            core->ifr &= ~(1 << i);     // clear the corresponding interrupt 
            //we should copy GIT to PGIE
            core->csr &= ~GIE_BIT;      /* disable global interrupt */
            core->tsr &= ~GIE_BIT;      /* disable global interrupt */
            //do not use intr_flag for ucos 
            //core->intr_flag = 1;
        }
    }
}

exception_t c6k_space_read(conf_object_t * obj, generic_address_t addr, void *buf, size_t count)
{
    c6k_core_t *core = (c6k_core_t *) obj->obj;
    memory_space_intf *iface = core->memory_space_iface;

    return iface->read(core->memory_space, addr, buf, count);
}

exception_t c6k_space_write(conf_object_t * obj, generic_address_t addr, void *buf, size_t count)
{
    c6k_core_t *core = (c6k_core_t *) obj->obj;
    memory_space_intf *iface = core->memory_space_iface;

    return iface->write(core->memory_space, addr, buf, count);
}

get_page_t *c6k_space_get_page(conf_object_t * obj, generic_address_t addr)
{
    c6k_core_t *core = (c6k_core_t *) obj->obj;
    memory_space_intf *iface = core->memory_space_iface;

    return iface->get_page(core->memory_space, addr);
}

exception_t c6k_multicore_space_read(conf_object_t * obj, generic_address_t addr, void *buf, size_t count)
{

    c6k_core_t *core = (c6k_core_t *) obj->obj;

    if ((addr >= 0x00800000 && addr <= 0x0087FFFF) || (addr >= 0x00E00000 && addr <= 0x00E07FFF)
        || (addr >= 0x00F00000 && addr <= 0x00F07FFF))
    {
        //loacl L2 L1P L1D SRAM to CorePac SRAM
        addr = addr + 0x10000000 + (core->dnum * 0x1000000);
    }
#define SEMAPTHORE 1
#if SEMAPTHORE
    uint32_t value = 0;

    if ((addr >= 0x2640100) && (addr < 0x2640400))
    {
        memory_space_intf *iface = core->memory_space_iface;

        iface->read(core->memory_space, addr, buf, count);
        value = *(int *) buf;
        //DBG("===> read value: 0x%X\n\n", value);

        if ((addr & 0xF00) == 0x100)
        {
            if ((value & 0x1) == 0x1)
            {
                uint32_t reg = 0;

                reg = (core->dnum) | 0x80000000;
                //DBG("\n%s %d reg 0x%x\n", __func__, __LINE__, reg);
                iface->write(core->memory_space, addr, &reg, count);
            }
        }
        return No_exp;
    }
#endif

    memory_space_intf *iface = core->memory_space_iface;

    return iface->read(core->memory_space, addr, buf, count);
}

exception_t c6k_multicore_space_write(conf_object_t * obj, generic_address_t addr, void *buf, size_t count)
{
    c6k_core_t *core = (c6k_core_t *) obj->obj;

    if ((addr >= 0x00800000 && addr <= 0x0087FFFF) || (addr >= 0x00E00000 && addr <= 0x00E07FFF)
        || (addr >= 0x00F00000 && addr <= 0x00F07FFF))
    {
        //loacl L2 L1P L1D SRAM to CorePac SRAM
        addr = addr + 0x10000000 + (core->dnum * 0x1000000);
    }
#if SEMAPTHORE
    if ((addr >= 0x2640100) && (addr < 0x2640400))
    {
        uint32_t value = 0;

        value = *(uint32_t *) buf;
        //DBG("===> write value: 0x%X\n\n", value);

        if ((addr & 0xF00) == 0x100)
        {
            if ((value & 0x1) == 0x0)
            {
                uint32_t reg = 0;

                reg = core->dnum << 8;
                //DBG("%s write Core_id reg 0x%x\n", __func__, __LINE__, reg);
                memory_space_intf *iface = core->memory_space_iface;

                iface->write(core->memory_space, addr, &reg, count);
                return No_exp;
            }
        }
    }
#endif

    memory_space_intf *iface = core->memory_space_iface;

    return iface->write(core->memory_space, addr, buf, count);
}

get_page_t *c6k_multicore_space_get_page(conf_object_t * obj, generic_address_t addr)
{
    c6k_core_t *core = (c6k_core_t *) obj->obj;

    if ((addr >= 0x00800000 && addr <= 0x0087FFFF) || (addr >= 0x00E00000 && addr <= 0x00E07FFF)
        || (addr >= 0x00F00000 && addr <= 0x00F07FFF))
    {
        //loacl L2 L1P L1D SRAM to CorePac SRAM
        addr = addr + 0x10000000 + (core->dnum * 0x1000000);
    }

    memory_space_intf *iface = core->memory_space_iface;

    return iface->get_page(core->memory_space, addr);
}

void c6k_set_pc(conf_object_t * obj, generic_address_t addr)
{
    c6k_core_t *core = (c6k_core_t *) obj->obj;

    core->pc = addr;
    return;
}

void c6k_set_textsection_info(conf_object_t * obj, generic_address_t code_start_addr, generic_address_t code_end_addr)
{
    c6k_core_t *core = (c6k_core_t *) obj->obj;

    core->code_start_addr = code_start_addr;
    core->code_end_addr = code_end_addr;
    DBG("%x %x for text section\n", code_start_addr, code_end_addr);

    return;
}

void c6k_do_pre_translate(conf_object_t * obj)
{
    c6k_core_t *core = (c6k_core_t *) obj->obj;

#ifdef LLVM_EXIST
    c6kplus_static_binary_translate(core);
#endif
    return;
}

generic_address_t c6k_get_pc(conf_object_t * obj)
{
    c6k_core_t *core = (c6k_core_t *) obj->obj;

    return core->pc;
}

void c6k_stepi(conf_object_t * obj, uint64_t steps)
{
    c6k_core_t *core = (c6k_core_t *) obj->obj;

    core->stop_step = core->insn_num + steps;
}

static void print_allGprRegisters(c6k_core_t * core)
{
    int i;

    printf("######Current PC = %x\n", core->pc);
    printf("\t");
    for (i = 0; i < 32; i++)
    {
        printf("[A%-2d]:%-8x", i, core->gpr[0][i]);
        if ((i + 1) % 8 == 0)
            printf("\n\t");
    }
    printf("\n\t");
    for (i = 0; i < 32; i++)
    {
        printf("[B%-2d]:%-8x", i, core->gpr[1][i]);
        if ((i + 1) % 8 == 0)
            printf("\n\t");
    }
    printf("\n");
}

static inline uint32_t no_legacy_left(c6k_core_t * core)
{
    uint32_t i, clean = 0;

    clean += core->wb_result_pos;
    if (clean)
        goto out;

    for (i = 0; i < DELAY_SLOT_NUM; i++)
    {
        clean += core->delay_slot_cycle[i];
        if (clean)
            goto out;
    }
  out:
    return !clean;
}

static inline void setup_register_handlers(c6k_core_t * core, bb_type_t type)
{
    core->write_buffer = write_buffer;
    //because of sploop for c64x+  disable bb mode in mode0
    //btw, there is no evidence show bb mode boost the perf
#if 0
    if ((type == SIMPLE_BB) && no_legacy_left(core) && !core->debug_mode)
    {
        core->register_branch_event = bb_register_branch_event;
        core->register_regw_event_h = bb_register_regw_event_h;
        core->register_regw_event_l = bb_register_regw_event_l;
        core->register_memory_event = bb_register_memory_event;
        core->exec_insn_cache = bb_exec_insn_cache;
        core->cycle_inc = bb_cycle_inc;
        core->add_pending_nops = bb_add_pending_nops;
        DBG("BASIC BLOCK START RUNNING: bb_type = SIMPLE_BB, running_type = BLOCK\n");
    } else
#endif
    {
        core->register_branch_event = legacy_register_branch_event;
        core->register_regw_event_h = legacy_register_regw_event;
        core->register_regw_event_l = legacy_register_regw_event;
        core->register_memory_event = legacy_register_memory_event;
        core->exec_insn_cache = legacy_exec_insn_cache;
        core->cycle_inc = legacy_cycle_inc;
        core->add_pending_nops = legacy_add_pending_nops;
        DBG("BASIC BLOCK START RUNNING: bb_type = %s, running_type = SINGLE\n",
            type == SIMPLE_BB ? "SIMPLE_BB" : "LEGACY_BB");
    }
}

void c6k_step_once(conf_object_t * conf_obj)
{
    c6k_core_t *core = (c6k_core_t *) conf_obj->obj;
    uint32_t curr_bb_idx;
    insn_cache_t *cache;
    static int i, j;
    static uint32_t last_real_pc;
    sim_pc_data_t sim_pc_data;
    fetch_context_t *fc = (fetch_context_t *) core->fetch_context;

    if (core->pc_bb_base == 0xFFFFFFFF)
        core->pc_bb_base = core->pc;
    curr_bb_idx = HASH(fc, core->pc_bb_base);
    core->pc_bb_base = 0xFFFFFFFF;

    /* First step, assume the current pc is a start of BB */
    SET_CURR_BB(fc, curr_bb_idx);
    /* Try to find a BB(indexed by current pc) whose start is valid */
    if (!GET_CURR_BB_ENTRY(fc))
    {
        RW_WRLOCK(core->fetch_lock);
        /* if start does not valid, the BB is null */
        if (!GET_CURR_BB_ENTRY(fc))
        {
            SET_BB_IC(fc, fetch_packet_to_cache(conf_obj, core->pc));
            DBG("NEW ENTRY HITED: %d ..\n", ++i);
        } else
        {
            /* find the start of BB who contains current pc */
            SET_BB_IC(fc, GET_BB_ENTRY(fc, curr_bb_idx));
            DBG("CACHED ENTRY HITED: %d ..\n", ++j);
        }
        RW_UNLOCK(core->fetch_lock);
    } else
    {
        SET_BB_IC(fc, GET_BB_ENTRY(fc, curr_bb_idx));
        DBG("CACHED ENTRY HITED: %d ..\n", ++j);
    }
    /* find from start of one BB , try to find the current instruction cache who match pc  */
    while ((core->pc != GET_BB_IC(fc)->addr) && (GET_BB_IC(fc) <= GET_CURR_BB_TAIL(fc)))
    {
        //DBG("core->pc=0x%x, IC->addr=0x%x\n", core->pc, )
        NEXT_BB_IC(fc);
    }

    /* execute one bb until IC meet the end of BB */
    while ((curr_bb_idx == GET_CURR_BB(fc)) && (GET_BB_IC(fc) <= GET_CURR_BB_TAIL(fc)))
    {
        cache = NEXT_BB_IC(fc);
        core->pce1 = core->pc & 0xFFFFFFE0;
        if (cache->type == INSN_REGULAR || cache->type == INSN_COMPACT)
        {
            core->insn_num++;
            last_real_pc = core->pc;

            //take care of code coverage 
            sim_pc_data.cycle = 1;
            sim_pc_data.startpc = 0xFFFFFFFF;
            SKY_pc_hap_occurred(&sim_pc_data, conf_obj, core->pc);
            /*
             * if(core->codecov)
             * {
             * if((core->codecov == 1))
             * {
             * //sim_pc_data.cycle = get_insn_delay_slot(cache->insn);
             * sim_pc_data.cycle = 1;
             * sim_pc_data.startpc = 0xFFFFFFFF;
             * SKY_pc_hap_occurred(&sim_pc_data, conf_obj, core->pc);
             * }else if(core->codecov == 2)
             * {
             * //sim_pc_data.cycle +=get_insn_delay_slot(cache->insn);
             * sim_pc_data.cycle += 1;
             * 
             * //if((curr_bb_idx == GET_CURR_BB(fc)) && (GET_BB_IC(fc) == GET_CURR_BB_TAIL(fc)))
             * if(last_real_pc == GET_CURR_BB_TAIL(fc)->addr)
             * {
             * DBG("hit codecov, %x, %x \n",GET_CURR_BB_ENTRY(fc)->addr,last_real_pc);
             * sim_pc_data.startpc = GET_CURR_BB_ENTRY(fc)->addr;
             * SKY_pc_hap_occurred(&sim_pc_data, conf_obj, last_real_pc);
             * sim_pc_data.cycle = 0;
             * }
             * }
             * }
             */
        }
#ifdef PRINT_LOG
        print_allGprRegisters(core);
#endif

        core->cache = cache;
        core->exec_insn_cache(core, cache);

        if (cache->type <= INSN_HEADER)
        {
            insn_cache_t *start = GET_CURR_BB_ENTRY(fc);

            //breakpoint
            if (check_breakpoint(&(core->bp_manager), core->pc, core->insn_num))
            {
                DBG("++++++pc is %x\n", core->pc);
                if (start != NULL)
                    core->pc_bb_base = GET_CURR_BB_ENTRY(fc)->addr;
                goto out;
            }
            //stepi
            if (core->stop_step != 0 && core->insn_num != 0 && core->stop_step == core->insn_num)
            {
                SIM_stop(NULL);
                core->stop_step = 0;
                if (start != NULL)
                {
                    core->pc_bb_base = GET_CURR_BB_ENTRY(fc)->addr;
                }
                goto out;
            }

            if (core->ifr & 0x2)
            {                           //SWE insn
                //DBG("######################## in %s, swe exp,b0=0x%x, pc=0x%x\n", __FUNCTION__,core->gpr[GPR_B][0], core->pc);
                break;
            }

        }
    }

    if (core->pc == 0)
    {
        skyeye_log(Warning_log, __FUNCTION__, "Current PC = 0!\n");
        //printf("step out ...\n");
        //skyeye_exit(-1);
    }

    check_intr(core);
  out:
    SET_CURR_BB(fc, INVALID_CURR_BB(fc));

    return;
}

exception_t reset_c6k_core(conf_object_t * obj, const char *parameter)
{
    return No_exp;
}

void c6k_reset(conf_object_t * obj)
{
    c6k_core_t *core = (c6k_core_t *) obj->obj;

    core->wb_result_pos = 0;

    core->sploop_begin = core->sploop_end = core->sploop_outside = 0xFFFFFFFF;
    core->sploop_begin_paralleled = 0;

    core->spmask_begin = 0xFFFFFFFF;
    core->sploop_ii = 0;
    core->sploop_curr_ii = 0;
    core->ilc = 0;
    core->buffer_pos = 0;
    core->buffer_index = 0;
    core->sploop_halt = 0;
    core->load_mask = 0;
    core->load_addr = 0;

    core->stop_step = 0;
    core->csr = 0x40010100;
    core->ier = 0x1;

    core->pc_bb_base = 0xFFFFFFFF;

    INIT_BUFFERS(core);
    return;
}

uint32_t c6k_per_cpu_step(conf_object_t * obj)
{
    c6k_core_t *core = (c6k_core_t *) obj->obj;

    if (core->wake_flag)
    {
        while (!core->run_flag) ;
    }
    //uint32_t pcx = core->pc;
    uint64_t steps = core->insn_num;
    uint32_t cys = core->cycle_num;

#ifdef LLVM_EXIST
    if (core->mode == DYNCOM_RUN)
    {
        c6kplus_dyncom_step_once(core);
    } else if (core->mode == HYBRID_RUN)
    {
        if (c6k_get_bp_numbers(obj))
            c6k_step_once(obj);
        else
            c6kplus_hybird_step_once(obj);
            ;
    } else
    {
        c6k_step_once(obj);
    }
#else
    c6k_step_once(obj);
#endif

    //printf("pc: %x  insn_num: %d  cy: %d\n",pcx,core->insn_num-steps,core->cycle_num-cys);
    //return core->insn_num - steps;

    //we set tscl and tsch  here
    int c_bit = (core->tscl >> 31) & 0x1;

    core->tscl += (core->cycle_num - cys);
    if (c_bit == 1 && ((core->tscl >> 31) & 0x1) == 0)
    {
        core->tsch++;
        core->tscl = 0;
    }
    //return core->cycle_num - cys;
    return core->insn_num - steps;
}

uint64_t c6k_get_step(conf_object_t * conf_obj)
{
    c6k_core_t *core = (c6k_core_t *) conf_obj->obj;

    return core->insn_num;
}

endian_t c6k_get_endian(conf_object_t * conf_obj)
{
    return Little_endian;
}

align_t c6k_get_alignment(conf_object_t * obj)
{
    return Align;
}

exception_t c6k_signal(conf_object_t * conf_obj, interrupt_signal_t * signal)
{                                       /* called in another thread */
    c6k_core_t *core = (c6k_core_t *) conf_obj->obj;
    c6k_signal_t *c6k_signal = &signal->c6k_signal;
    int i = 0;

    //DBG("in %s, begin csr=0x%x\n", __FUNCTION__, core->csr);
    for (; i < 16; i++)
    {
        if ((i == 2) || (i == 3))       /* reserved */
            continue;
        if (c6k_signal->ifr[i] != Prev_level)
        {
            RW_WRLOCK(core->ifr_lock);
            if (c6k_signal->ifr[i] == Low_level)
            {
                core->ifr &= ~(1 << i);
            } else
            {                           /* high level */
                core->ifr |= (1 << i);
            }
            RW_UNLOCK(core->ifr_lock);
            break;
        }
    }
    return No_exp;
}

conf_object_t *new_c6k_core(const char *obj_name)
{
    c6k_core_t *core = skyeye_mm_zero(sizeof (c6k_core_t));

    core->obj = new_conf_object(obj_name, core);
    core->freq_hz = 200000000;          //200MHZ
    core->insn_cache_size = CACHE_SIZE;
    return core->obj;
}

exception_t config_c6k_core(conf_object_t * obj)
{
    c6k_core_t *core = obj->obj;

    if (strstr(obj->class_name, "c66x_core"))
    {
        core->c6k_memory_intf = skyeye_mm_zero(sizeof (memory_space_intf));
        core->c6k_memory_intf->read = c6k_multicore_space_read;
        core->c6k_memory_intf->write = c6k_multicore_space_write;
        core->c6k_memory_intf->get_page = c6k_multicore_space_get_page;
    } else
    {
        core->c6k_memory_intf = skyeye_mm_zero(sizeof (memory_space_intf));
        core->c6k_memory_intf->read = c6k_space_read;
        core->c6k_memory_intf->write = c6k_space_write;
        core->c6k_memory_intf->get_page = c6k_space_get_page;
        core->dnum = 0xFFFFFFFF;
    }

    c6k_reset(obj);
    //debug mode do not init in c6k_reset
    core->debug_mode = 0;
    RWLOCK_INIT(core->ifr_lock);
    RWLOCK_INIT(core->fetch_lock);

    core->wake_flag = 0;
    core->run_flag = 0;
    core->fetch_context = alloc_fetch_context(core);

    //FIXME this should be written in core init
    //set for cpuid
    //core->csr = 0x0c010100;
    core->csr = 0x44010100;             // set for c6k instruction test, initial value in CCS
    //set for gfpgfr
    core->gfpgfr = 0x0700001d;

    setup_register_handlers(core, 0);
#ifdef LLVM_EXIST
    if (core->mode != 0)
        c6kplus_dyncom_init(core);
#endif
    return No_exp;
}

void free_c6k_core(conf_object_t * dev)
{
    c6k_core_t *core = dev->obj;

#ifdef LLVM_EXIST
    if (core->mode == HYBRID_RUN || core->mode == DYNCOM_RUN)
    {
        c6kplus_dyncom_fini(core);
    }
#endif
    free_fetch_context(core);
    skyeye_free(core->c6k_memory_intf);
    skyeye_free(core->obj);
    skyeye_free(core);
}

DEF_DOUBLE_ARRAY_UINTEGER_ATTR_FUNC(c6k_core_t, gpr, GPR_GROUP * GPR_NUM_IN_GROUP, GPR_GROUP, GPR_NUM_IN_GROUP);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, amr);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, gsr);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, gfpgfr);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, icr);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, ier);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, ifr);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, irp);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, isr);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, nrp);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, pce1);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, dier);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, dnum);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, ecr);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, efr);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, gplya);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, gplyb);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, ierr);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, ntsr);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, ilc);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, itsr);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, tscl);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, tsr);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, fadcr);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, fmcr);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, csr);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, ssr);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, istp);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, rilc);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, stop_step);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, pc);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, pc_bb_base);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, pfc);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, cycle_num);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, insn_num);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, delay_slot);
DEF_LIST_UINTEGER_ATTR_FUNC(c6k_core_t, pfc_branch, DELAY_SLOT_NUM);
DEF_LIST_UINTEGER_ATTR_FUNC(c6k_core_t, delay_slot_cycle, DELAY_SLOT_NUM);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, curr_slot_id);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, mem_access_slot_id);
DEF_DOUBLE_ARRAY_UINTEGER_ATTR_FUNC(c6k_core_t, mem_access_result, DELAY_SLOT_NUM * MEM_ACCESS_BUF_SIZE, DELAY_SLOT_NUM,
                                    MEM_ACCESS_BUF_SIZE);
DEF_DOUBLE_ARRAY_UINTEGER_ATTR_FUNC(c6k_core_t, mem_access_reg, DELAY_SLOT_NUM * MEM_ACCESS_BUF_SIZE, DELAY_SLOT_NUM,
                                    MEM_ACCESS_BUF_SIZE);
DEF_DOUBLE_ARRAY_UINTEGER_ATTR_FUNC(c6k_core_t, mem_access_addr, DELAY_SLOT_NUM * MEM_ACCESS_BUF_SIZE, DELAY_SLOT_NUM,
                                    MEM_ACCESS_BUF_SIZE);
DEF_LIST_UINTEGER_ATTR_FUNC(c6k_core_t, mem_access_buf_pos, DELAY_SLOT_NUM);
DEF_DOUBLE_ARRAY_UINTEGER_ATTR_FUNC(c6k_core_t, mem_wr_reg, DELAY_SLOT_NUM * MEM_ACCESS_BUF_SIZE, DELAY_SLOT_NUM,
                                    MEM_ACCESS_BUF_SIZE);
DEF_DOUBLE_ARRAY_UINTEGER_ATTR_FUNC(c6k_core_t, mem_wr_result, DELAY_SLOT_NUM * MEM_ACCESS_BUF_SIZE, DELAY_SLOT_NUM,
                                    MEM_ACCESS_BUF_SIZE);
DEF_DOUBLE_ARRAY_UINTEGER_ATTR_FUNC(c6k_core_t, mem_wr_addr, DELAY_SLOT_NUM * MEM_ACCESS_BUF_SIZE, DELAY_SLOT_NUM,
                                    MEM_ACCESS_BUF_SIZE);
DEF_LIST_UINTEGER_ATTR_FUNC(c6k_core_t, mem_wr_buf_pos, DELAY_SLOT_NUM);
DEF_DOUBLE_ARRAY_UINTEGER_ATTR_FUNC(c6k_core_t, regw_regno_buf, DELAY_SLOT_NUM * REGW_BUF_SIZE, DELAY_SLOT_NUM,
                                    REGW_BUF_SIZE);
DEF_DOUBLE_ARRAY_UINTEGER_ATTR_FUNC(c6k_core_t, regw_regval_buf, DELAY_SLOT_NUM * REGW_BUF_SIZE, DELAY_SLOT_NUM,
                                    REGW_BUF_SIZE);
DEF_LIST_UINTEGER_ATTR_FUNC(c6k_core_t, regw_buf_pos, DELAY_SLOT_NUM);
DEF_LIST_UINTEGER_ATTR_FUNC(c6k_core_t, regw_regno_buf_l, REGW_BUF_SIZE + 1);
DEF_LIST_UINTEGER_ATTR_FUNC(c6k_core_t, regw_regval_buf_l, REGW_BUF_SIZE + 1);
DEF_LIST_UINTEGER_ATTR_FUNC(c6k_core_t, regw_addr_l, REGW_BUF_SIZE + 1);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, regw_buf_pos_l);
DEF_LIST_UINTEGER_ATTR_FUNC(c6k_core_t, regw_regno_buf_h, REGW_BUF_SIZE + 1);
DEF_LIST_UINTEGER_ATTR_FUNC(c6k_core_t, regw_regval_buf_h, REGW_BUF_SIZE + 1);
DEF_LIST_UINTEGER_ATTR_FUNC(c6k_core_t, regw_addr_h, REGW_BUF_SIZE + 1);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, regw_buf_pos_h);
DEF_LIST_UINTEGER_ATTR_FUNC(c6k_core_t, memr_reg, DELAY_SLOT_NUM + 1);
DEF_LIST_UINTEGER_ATTR_FUNC(c6k_core_t, memr_addr, DELAY_SLOT_NUM + 1);
DEF_LIST_UINTEGER_ATTR_FUNC(c6k_core_t, memr_reg_addr, DELAY_SLOT_NUM + 1);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, memr_buf_pos);
DEF_LIST_UINTEGER_ATTR_FUNC(c6k_core_t, memw_reg, DELAY_SLOT_NUM + 1);
DEF_LIST_UINTEGER_ATTR_FUNC(c6k_core_t, memw_result, DELAY_SLOT_NUM + 1);
DEF_LIST_UINTEGER_ATTR_FUNC(c6k_core_t, memw_addr, DELAY_SLOT_NUM + 1);
DEF_LIST_UINTEGER_ATTR_FUNC(c6k_core_t, memw_reg_addr, DELAY_SLOT_NUM + 1);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, memw_buf_pos);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, br_target);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, header);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, parallel);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, buffer_pos);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, sploop_begin);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, sploop_end);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, sploop_begin_paralleled);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, sploop_flag);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, sploopw_cond);
DEF_LIST_UINTEGER_ATTR_FUNC(c6k_core_t, legacy_sploop_buffer, 32);
DEF_LIST_UINTEGER_ATTR_FUNC(c6k_core_t, sploop_pc, 32);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, sploop_ii);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, spmask);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, spmask_begin);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, spmask_end);
DEF_LIST_UINTEGER_ATTR_FUNC(c6k_core_t, wb_result, GPR_NUM_IN_GROUP * GPR_GROUP);
DEF_LIST_UINTEGER_ATTR_FUNC(c6k_core_t, wb_index, GPR_NUM_IN_GROUP * GPR_GROUP);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, wb_result_pos);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, wb_flag);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, intr_flag);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, load_mask);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, pending_nops);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, wake_flag);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, run_flag);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, mode);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, debug_mode);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, codecov);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, freq_hz);
DEF_UINTEGER_ATTR_FUNC(c6k_core_t, insn_cache_size);

void c6k_register_attribute(conf_class_t * clss)
{
    DEF_DISABLE_SHOW_ATTR(gpr, list, "c6k gpr register");
    DEF_DISABLE_SHOW_ATTR(amr, uinteger, "c6k amr register");
    DEF_DISABLE_SHOW_ATTR(gsr, uinteger, "c6k gsr register");
    DEF_DISABLE_SHOW_ATTR(gfpgfr, uinteger, "c6k gfpgfr register");
    DEF_DISABLE_SHOW_ATTR(icr, uinteger, "c6k icr register");
    DEF_DISABLE_SHOW_ATTR(ier, uinteger, "c6k ier register");
    DEF_DISABLE_SHOW_ATTR(ifr, uinteger, "c6k ifr register");
    DEF_DISABLE_SHOW_ATTR(irp, uinteger, "c6k irp register");
    DEF_DISABLE_SHOW_ATTR(isr, uinteger, "c6k isr register");
    DEF_DISABLE_SHOW_ATTR(nrp, uinteger, "c6k nrp register");
    DEF_DISABLE_SHOW_ATTR(pce1, uinteger, "c6k pce1 register");
    DEF_DISABLE_SHOW_ATTR(dier, uinteger, "c6k dier register");
    DEF_ATTR(dnum, uinteger, "c6k dnum register");
    DEF_DISABLE_SHOW_ATTR(ecr, uinteger, "c6k ecr register");
    DEF_DISABLE_SHOW_ATTR(efr, uinteger, "c6k efr register");
    DEF_DISABLE_SHOW_ATTR(gplya, uinteger, "c6k gplya register");
    DEF_DISABLE_SHOW_ATTR(gplyb, uinteger, "c6k gplyb register");
    DEF_DISABLE_SHOW_ATTR(ierr, uinteger, "c6k ierr register");
    DEF_DISABLE_SHOW_ATTR(ntsr, uinteger, "c6k ntsr register");
    DEF_DISABLE_SHOW_ATTR(ilc, uinteger, "c6k ilc register");
    DEF_DISABLE_SHOW_ATTR(itsr, uinteger, "c6k itsr register");
    DEF_DISABLE_SHOW_ATTR(tscl, uinteger, "c6k tscl register");
    DEF_DISABLE_SHOW_ATTR(tsr, uinteger, "c6k tsr register");
    DEF_DISABLE_SHOW_ATTR(fadcr, uinteger, "c6k fadcr register");
    DEF_DISABLE_SHOW_ATTR(fmcr, uinteger, "c6k fmcr register");
    DEF_DISABLE_SHOW_ATTR(csr, uinteger,  "c6k csr register");
    DEF_DISABLE_SHOW_ATTR(ssr, uinteger, "c6k ssr register");
    DEF_DISABLE_SHOW_ATTR(istp, uinteger, "c6k istp register");
    DEF_DISABLE_SHOW_ATTR(rilc, uinteger, "c6k rilc register");
    DEF_DISABLE_SHOW_ATTR(stop_step, uinteger, "c6k stop_step");
    DEF_DISABLE_SHOW_ATTR(pc, uinteger, "c6k pc register");
    DEF_DISABLE_SHOW_ATTR(pc_bb_base, uinteger, "c6k pc_bb_base set");
    DEF_DISABLE_SHOW_ATTR(pfc, uinteger, "c6k pfc set");
    DEF_DISABLE_SHOW_ATTR(cycle_num, uinteger, "c6k cycle_num set");
    DEF_DISABLE_SHOW_ATTR(insn_num, uinteger, "c6k insn_num set");
    DEF_DISABLE_SHOW_ATTR(delay_slot, uinteger, "c6k delay_slot set");
    DEF_DISABLE_SHOW_ATTR(pfc_branch, list, "c6k pfc_branch set");
    DEF_DISABLE_SHOW_ATTR(delay_slot_cycle, list, "c6k delay_slot_cycle set");
    DEF_DISABLE_SHOW_ATTR(curr_slot_id, uinteger, "c6k curr_slot_id set");
    DEF_DISABLE_SHOW_ATTR(mem_access_slot_id, uinteger, "c6k mem_access_slot_id set");
    DEF_DISABLE_SHOW_ATTR(mem_access_result, list, "c6k mem_access_result set");
    DEF_DISABLE_SHOW_ATTR(mem_access_reg, list, "c6k mem_access_reg set");
    DEF_DISABLE_SHOW_ATTR(mem_access_addr, list, "c6k mem_access_addr set");
    DEF_DISABLE_SHOW_ATTR(mem_access_buf_pos, list, "c6k mem_access_buf_pos set");
    DEF_DISABLE_SHOW_ATTR(mem_wr_reg, list, "c6k mem_wr_reg set");
    DEF_DISABLE_SHOW_ATTR(mem_wr_result, list, "c6k mem_wr_result set");
    DEF_DISABLE_SHOW_ATTR(mem_wr_addr, list, "c6k mem_wr_addr set");
    DEF_DISABLE_SHOW_ATTR(mem_wr_buf_pos, list, "c6k mem_wr_buf_pos set");
    DEF_DISABLE_SHOW_ATTR(regw_regno_buf, list, "c6k regw_regno_buf set");
    DEF_DISABLE_SHOW_ATTR(regw_regval_buf, list, "c6k regw_regval_buf set");
    DEF_DISABLE_SHOW_ATTR(regw_buf_pos, list, "c6k regw_buf_pos set");
    DEF_DISABLE_SHOW_ATTR(regw_regno_buf_l, list, "c6k regw_regno_buf_l set");
    DEF_DISABLE_SHOW_ATTR(regw_regval_buf_l, list, "c6k regw_regval_buf_l set");
    DEF_DISABLE_SHOW_ATTR(regw_addr_l, list, "c6k regw_addr_l set");
    DEF_DISABLE_SHOW_ATTR(regw_buf_pos_l, uinteger, "c6k regw_buf_pos_l set");
    DEF_DISABLE_SHOW_ATTR(regw_regno_buf_h, list, "c6k regw_regno_buf_h set");
    DEF_DISABLE_SHOW_ATTR(regw_regval_buf_h, list, "c6k regw_regval_buf_h set");
    DEF_DISABLE_SHOW_ATTR(regw_addr_h, list, "c6k regw_addr_h set");
    DEF_DISABLE_SHOW_ATTR(regw_buf_pos_h, uinteger, "c6k regw_buf_pos_h set");
    DEF_DISABLE_SHOW_ATTR(memr_reg, list, "c6k memr_reg set");
    DEF_DISABLE_SHOW_ATTR(memr_addr, list, "c6k memr_addr set");
    DEF_DISABLE_SHOW_ATTR(memr_reg_addr, list, "c6k memr_reg_addr set");
    DEF_DISABLE_SHOW_ATTR(memr_buf_pos, uinteger, "c6k memr_buf_pos set");
    DEF_DISABLE_SHOW_ATTR(memw_reg, list, "c6k memw_reg set");
    DEF_DISABLE_SHOW_ATTR(memw_result, list, "c6k memw_result set");
    DEF_DISABLE_SHOW_ATTR(memw_addr, list, "c6k memw_addr set");
    DEF_DISABLE_SHOW_ATTR(memw_reg_addr, list, "c6k memw_reg_addr set");
    DEF_DISABLE_SHOW_ATTR(memw_buf_pos, uinteger, "c6k memw_buf_pos set");
    DEF_DISABLE_SHOW_ATTR(br_target, uinteger, "c6k br_target set");
    DEF_DISABLE_SHOW_ATTR(header, uinteger, "c6k header set");
    DEF_DISABLE_SHOW_ATTR(parallel, uinteger, "c6k parallel set");
    DEF_DISABLE_SHOW_ATTR(buffer_pos, uinteger, "c6k buffer_pos set");
    DEF_DISABLE_SHOW_ATTR(sploop_begin, uinteger, "c6k sploop_begin set");
    DEF_DISABLE_SHOW_ATTR(sploop_end, uinteger, "c6k sploop_end set");
    DEF_DISABLE_SHOW_ATTR(sploop_begin_paralleled, uinteger, "c6k sploop_begin_paralleled set");
    DEF_DISABLE_SHOW_ATTR(sploop_flag, uinteger, "c6k sploop_flag set");
    DEF_DISABLE_SHOW_ATTR(sploopw_cond, uinteger, "c6k sploopw_cond set");
    DEF_DISABLE_SHOW_ATTR(legacy_sploop_buffer, list, "c6k legacy_sploop_buffer set");
    DEF_DISABLE_SHOW_ATTR(sploop_pc, list, "c6k sploop_pc set");
    DEF_DISABLE_SHOW_ATTR(sploop_ii, uinteger, "c6k sploop_ii set");
    DEF_DISABLE_SHOW_ATTR(spmask, uinteger, "c6k spmask set");
    DEF_DISABLE_SHOW_ATTR(spmask_begin, uinteger, "c6k spmask_begin set");
    DEF_DISABLE_SHOW_ATTR(spmask_end, uinteger, "c6k spmask_end set");
    DEF_DISABLE_SHOW_ATTR(wb_result, list, "c6k wb_result set");
    DEF_DISABLE_SHOW_ATTR(wb_index, list, "c6k wb_index set");
    DEF_DISABLE_SHOW_ATTR(wb_result_pos, uinteger, "c6k wb_result_pos set");
    DEF_DISABLE_SHOW_ATTR(wb_flag, uinteger, "c6k wb_flag set");
    DEF_DISABLE_SHOW_ATTR(intr_flag, uinteger, "c6k intr_flag set");
    DEF_ATTR(load_mask, uinteger, "c6k load_mask set");
    DEF_DISABLE_SHOW_ATTR(pending_nops, uinteger, "c6k pending_nops set");
    DEF_ATTR(wake_flag, uinteger, "c6k wake dsp");
    DEF_ATTR(run_flag, uinteger, "c6k run flag");
    DEF_ATTR(mode, uinteger, "c6k run mode");
    DEF_ATTR(debug_mode, uinteger, "c6k debug mode set");
    DEF_DISABLE_SHOW_ATTR(codecov, uinteger, "c6k codecov set");
    DEF_ATTR(freq_hz, uinteger, "c6k freq_hz set");
    DEF_ATTR(insn_cache_size, uinteger, NULL);
    return;
}

char *c6k_disassemble(conf_object_t * obj, generic_address_t addr)
{
    c6k_core_t *core = (c6k_core_t *) obj->obj;

    return skyeye_disassemble(obj, addr);
}

char *c6k_get_architecture(conf_object_t * obj)
{
    c6k_core_t *core = (c6k_core_t *) obj->obj;

    return "tic6x";
}

int c6k_insert_bp(conf_object_t * conf_obj, generic_address_t addr)
{
    c6k_core_t *core = (c6k_core_t *) conf_obj->obj;
    int ret;

    ret = create_a_breakpoint(&core->bp_manager, addr);
    if (ret != 0)
        return ret;
    return 0;
}

int c6k_delete_bp(conf_object_t * conf_obj, generic_address_t addr)
{
    c6k_core_t *core = (c6k_core_t *) conf_obj->obj;
    int ret;

    ret = remove_bp_by_addr(&core->bp_manager, addr);
    if (ret != 0)
        return ret;
    return 0;
}

uint32_t c6k_get_bp_numbers(conf_object_t * conf_obj)
{
    c6k_core_t *core = (c6k_core_t *) conf_obj->obj;

    return get_bp_numbers(&core->bp_manager);
}

uint32_t c6k_get_bp_addr_by_id(conf_object_t * conf_obj, int id)
{
    c6k_core_t *core = (c6k_core_t *) conf_obj->obj;
    breakpoint_t *bp = get_bp_by_id(&core->bp_manager, id);

    if (bp)
        return bp->address;
    return 0;
}

int c6k_get_trigger(conf_object_t * conf_obj)
{
    c6k_core_t *core = (c6k_core_t *) conf_obj->obj;

    return get_breakpoint_hit_id(&core->bp_manager);
}

bool_t c6k_check_bp_trigger(conf_object_t * conf_obj)
{
    c6k_core_t *core = (c6k_core_t *) conf_obj->obj;
    int ret;

    ret = check_breakpoint_hit_flag(&core->bp_manager);
    if (ret == 0)
        return True;
    return False;
}

int c6k_clear_bp_trigger(conf_object_t * conf_obj)
{
    c6k_core_t *core = (c6k_core_t *) conf_obj->obj;

    clear_breakpoint_hit_flag(&core->bp_manager);
    return 0;
}

exception_t c6k_memory_space_set(conf_object_t * obj, conf_object_t * obj2, const char *port, int index)
{
    c6k_core_t *core = (c6k_core_t *) obj->obj;

    core->memory_space = obj2;
    core->memory_space_iface = (memory_space_intf *) SKY_get_iface(core->memory_space, MEMORY_SPACE_INTF_NAME);
    if (core->memory_space_iface == NULL)
        skyeye_log(Warning_log, __FUNCTION__, "Get 'memory_space' interface from %s fail.", obj2->objname);
    return No_exp;
}

exception_t c6k_memory_space_get(conf_object_t * obj, conf_object_t ** obj2, char **port, int index)
{
    c6k_core_t *core = (c6k_core_t *) obj->obj;

    *obj2 = core->memory_space;
    *port = NULL;
    return No_exp;
}

uint32_t c6k_get_address_width(conf_object_t * obj)
{
    return 4;
}

void c6k_get_cpu_run_status(conf_object_t * obj, uint32_t * cpu_freq, uint64_t * cpu_insn_num)
{
    c6k_core_t *core = (c6k_core_t *) obj->obj;

    *cpu_freq = core->freq_hz;
    *cpu_insn_num = core->insn_num;
    return;
}

void c6k_set_memory_watch_status(conf_object_t * obj, uint32_t addr, int status)
{
    c6k_core_t *core = (c6k_core_t *) obj->obj;

#if LLVM_EXIST
    if (core->mode != 0)
        set_dyncom_memory_watch_status(core, addr, status);
#endif
    return;
}

int c6k_get_cpu_exception_num(conf_object_t * obj)
{
    c6k_core_t *core = (c6k_core_t *) obj->obj;

    return C6K_IRQ_NUM;
}

void c6k_set_cpu_exception_by_id(conf_object_t * obj, int exception_id)
{
    c6k_core_t *core = (c6k_core_t *) obj->obj;

    if (exception_id < C6K_IRQ_NUM)
        core->ifr |= (1 << exception_id);

    return;
}

char *c6k_get_cpu_exception_name_by_id(conf_object_t * obj, int exception_id)
{
    c6k_core_t *core = (c6k_core_t *) obj->obj;

    strcpy(core->exception_name, "IRQ");

    assert(exception_id < C6K_IRQ_NUM);
    sprintf(core->exception_name, "%s%d", core->exception_name, exception_id);
    return core->exception_name;
}
