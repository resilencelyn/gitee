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
* @file c55x_core.c
* @brief The core of c55x or c55x
* @author jiachao 
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
#include <skyeye_iface.h>
#include <skyeye_disassemble.h>
#include <skyeye_callback.h>
#include <skyeye_core_intf.h>
#include <skyeye_int_register.h>
#include <sim_control.h>
#include "c55_cpu.h"
#include "c55_decode.h"

#define SKYEYE_LOG 0

uint32_t exec_insn_cache(conf_object_t * conf_obj, c55_core_t * core, insn_cache_t * cache);

void c55x_set_pc(conf_object_t * obj, generic_address_t addr)
{
    c55_core_t *core = (c55_core_t *) obj->obj;

    core->pc = addr;
    return;
}

generic_address_t c55x_get_pc(conf_object_t * obj)
{
    c55_core_t *core = (c55_core_t *) obj->obj;

    return core->pc;
}

uint32_t c55x_get_address_width(conf_object_t * obj)
{
    return 1;
}

char *c55x_disassemble(conf_object_t * obj, generic_address_t addr)
{
    c55_core_t *core = (c55_core_t *) obj->obj;

    return skyeye_disassemble(obj, addr);
}

char *c55x_get_architecture(conf_object_t * obj)
{
    c55_core_t *core = (c55_core_t *) obj->obj;

    return "tic55x";
}

int c55x_get_cpu_exception_num(conf_object_t * obj)
{
    c55_core_t *core = (c55_core_t *) obj->obj;

    return C55X_IRQ_NUM;
}

void c55x_set_cpu_exception_by_id(conf_object_t * obj, int exception_id)
{
    c55_core_t *core = (c55_core_t *) obj->obj;

    if (exception_id < C55X_IRQ_NUM)
    {
        RW_WRLOCK(core->ifr_lock);
        core->ifr |= (1 << exception_id);
        RW_UNLOCK(core->ifr_lock);
    } else
    {
        //need trigger IRQ to PIE
    }

    return;
}

char *c55x_get_cpu_exception_name_by_id(conf_object_t * obj, int exception_id)
{
    c55_core_t *core = (c55_core_t *) obj->obj;

    strcpy(core->exception_name, "IRQ");

    assert(exception_id < C55X_IRQ_NUM);
    sprintf(core->exception_name, "%s%d", core->exception_name, exception_id);
    return core->exception_name;
}

void register_pairs_save(c55_core_t * core)
{
    core->sp -= 1;
    c55_memory_write(core, core->sp, core->st2_55);

    core->ssp -= 1;
    c55_memory_write(core, core->ssp, (core->st0_55 & 0xFE00)); //push st0_55 to ssp

    core->sp -= 1;
    c55_memory_write(core, core->sp, core->st1_55);

    core->ssp -= 1;
    c55_memory_write(core, core->ssp, core->dbstat);

    core->sp -= 1;
    c55_memory_write(core, core->sp, (core->pc & 0xFFFF));

    core->ssp -= 1;
    c55_memory_write(core, core->ssp, ((core->pc >> 16) & 0xFF));

    return;
}
static void check_intr(c55_core_t * core)
{
    if (core->intr_flag)
        return;
    if (core->ifr != 0)
    {
        int i = 2;
        int intm = core->st1_55 & INTM_BIT;
        int isr_addr = 0;

        while (((core->ifr >> i) & 0x1) == 0)
            i++;
        if (i < 32)
        {
            if ((core->ier & (1 << i)) && (intm == 0))
            {
                /* save the intr return address and registers to memory */
                register_pairs_save(core);

                core->st1_55 |= INTM_BIT;       /* disable global interrupt */
                core->dbier |= (0x1 << i);
                /*2-23 interrupt vectors is triggered at a pin or by a
                 * peripheral of DSP */

                if (i >= 16 && i <= 23)
                {
                    isr_addr = (core->vector_baseaddr + i * 8) >> core->ivph;
                } else
                {
                    isr_addr = (core->vector_baseaddr + i * 8) >> core->ivpd;
                }
                core->pc = c55_memory_read(core, isr_addr) << 16 | c55_memory_read(core, isr_addr + 1);
                core->intr_flag = 1;
            }
            core->ifr &= ~(0x1 << i);   /* clear ifr */
        }
    }
}

exception_t c55x_space_read(conf_object_t * obj, generic_address_t addr, void *buf, size_t count)
{
    c55_core_t *core = (c55_core_t *) obj->obj;
    memory_space_intf *iface = core->memory_space_iface;

    return iface->read(core->memory_space, addr, buf, count);
}

exception_t c55x_space_write(conf_object_t * obj, generic_address_t addr, void *buf, size_t count)
{
    c55_core_t *core = (c55_core_t *) obj->obj;
    memory_space_intf *iface = core->memory_space_iface;

    return iface->write(core->memory_space, addr, buf, count);
}

void c55x_stepi(conf_object_t * obj, uint64_t steps)
{
    c55_core_t *core = (c55_core_t *) obj->obj;

    core->stop_step = core->insn_num + steps;
    return;
}

static void init_memory_data(conf_object_t * conf_obj)
{
    c55_core_t *core = (c55_core_t *) conf_obj->obj;

    c55_memory_write(core, ST0_ADDR, core->st0_55);
    c55_memory_write(core, ST1_ADDR, core->st1_55);
    c55_memory_write(core, ST2_ADDR, core->st2_55);
    c55_memory_write(core, ST3_ADDR, core->st3_55);
    c55_memory_write(core, IVPD_ADDR, core->ivpd);
    c55_memory_write(core, IVPH_ADDR, core->ivph);
    return;
}

exception_t run_exec_insn(conf_object_t * conf_obj)
{
    c55_core_t *core = (c55_core_t *) conf_obj->obj;
    uint32_t addr = core->pc;

    /* first init memory data */
    if (core->init_memory_flag == 0)
    {
        init_memory_data(conf_obj);
        core->init_memory_flag = 1;
    }
    int status;
    insn_cache_t *cache = &(core->insn_cache[HASH(addr)]);

    if (cache->addr != addr)            /* miss */
    {
        /* do translate and replace the old one */
        uint64_t insn;
        uint32_t insn1;

        cache->addr = addr;
        status = c55x_space_read(conf_obj, addr, &insn, 32 / 8);
        status = c55x_space_read(conf_obj, addr + 4, &insn1, 32 / 8);
        insn = (insn << 16) | (insn1 >> 16);
        cache->insn = insn;
        trans_opcode_fields(insn, &(cache->fields));
        cache->action = get_insn_action(core, insn);
    }
    SKY_pc_hap_occurred(NULL, conf_obj, core->pc);

    exec_insn_cache(conf_obj, core, cache);
    return No_exp;
}

uint32_t c55x_step_once(conf_object_t * conf_obj)
{
    c55_core_t *core = (c55_core_t *) conf_obj->obj;
    uint32_t cycle_num = core->insn_num;

    if (No_exp != run_exec_insn(conf_obj))
    {
        printf("In %s, run exec_insn function error at pc = 0x%x\n", __func__, core->pc);
        skyeye_exit(-1);
    }
#if SKYEYE_LOG
    printf("######Current PC:%x  XSP:%x\n", core->pc, core->sp);
    //printf("######Current PC:%x  XSP:%x  XSSP:%x\n", core->pc, core->sp, core->ssp);
    printf("AC0:%llx  AC1:%llx  AC2:%llx  AC3:%llx\n", core->ac[0] & 0xFFFFFFFFFF, core->ac[1] & 0xFFFFFFFFFF,
           core->ac[2] & 0xFFFFFFFFFF, core->ac[3] & 0xFFFFFFFFFF);
    printf("XAR0:%x  XAR1:%x  XAR2:%x  XAR3:%x  XAR4:%x  XAR5:%x\n", core->xar[0].xar, core->xar[1].xar,
           core->xar[2].xar, core->xar[3].xar, core->xar[4].xar, core->xar[5].xar);
    printf("XAR6:%x  XAR7:%x  ST0:%x  ST1:%x  ST2:%x  ST3:%x\n", core->xar[6].xar, core->xar[7].xar, core->st0_55,
           core->st1_55, core->st2_55, core->st3_55);
    printf("T0:%x  T1:%x  T2:%x  T3:%x\n", core->t[0], core->t[1], core->t[2], core->t[3]);
#endif
    //check repeat block loop
    if (core->rptb_enable == 1)
    {
        if (core->brc0 != 0)
        {
            if (core->pc == core->rea0)
            {
                run_exec_insn(conf_obj);
                core->pc = core->rsa0;
                core->brc0--;
            }
        } else
        {
            core->rptb_enable = 0;
        }
    }

    int id = check_breakpoint(&(core->bp_manager), core->pc, core->insn_num);

    if (core->stop_step != 0 && core->insn_num != 0 && core->stop_step == core->insn_num)
    {
        SIM_stop(NULL);
        core->stop_step = 0;
    }
    if (core->pc == 0)
    {
        skyeye_log(Error_log, __func__, "Get the error ic value\n");
        skyeye_exit(-1);
    }
    check_intr(core);
    return(core->insn_num - cycle_num);
}

void c55x_reset(conf_object_t * obj)
{
    c55_core_t *core = (c55_core_t *) obj->obj;

    return;
}

void per_cpu_step(conf_object_t * obj)
{
    c55x_step_once(obj);
    return;
}

endian_t c55x_get_endian(conf_object_t * obj)
{
    return Big_endian;
}

align_t c55x_get_alignment(conf_object_t * obj)
{
    return Align;
}

exception_t c55x_signal(conf_object_t * conf_obj, interrupt_signal_t * signal)
{                                       /* called in another thread */
    c55_core_t *core = (c55_core_t *) conf_obj->obj;
    c5k_signal_t *c5k_signal = &signal->c5k_signal;
    int i = 0;

    for (; i < 32; i++)
    {
        if ((i == 0) || (i == 1) || (i == 10))  /* reserved */
            continue;
        if (c5k_signal->ifr[i] != Prev_level)
        {
            RW_WRLOCK(core->ifr_lock);
            if (c5k_signal->ifr[i] == Low_level)
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

uint64_t c55x_get_steps(conf_object_t * conf_obj)
{
    c55_core_t *core = (c55_core_t *) conf_obj->obj;

    return core->insn_num;
}

void per_cpu_stop(conf_object_t * obj)
{
    return;
}

int c55x_insert_bp(conf_object_t * conf_obj, generic_address_t addr)
{
    c55_core_t *core = (c55_core_t *) conf_obj->obj;
    int ret;

    ret = create_a_breakpoint(&core->bp_manager, addr);
    if (ret != 0)
        return ret;
    return 0;
}

int c55x_delete_bp(conf_object_t * conf_obj, generic_address_t addr)
{
    c55_core_t *core = (c55_core_t *) conf_obj->obj;
    int ret;

    ret = remove_bp_by_addr(&core->bp_manager, addr);
    if (ret != 0)
        return ret;
    return 0;
}

int c55x_get_bp_numbers(conf_object_t * conf_obj)
{
    c55_core_t *core = (c55_core_t *) conf_obj->obj;

    return get_bp_numbers(&core->bp_manager);
}

uint32_t c55x_get_bp_addr_by_id(conf_object_t * conf_obj, int id)
{
    c55_core_t *core = (c55_core_t *) conf_obj->obj;
    breakpoint_t *bp = get_bp_by_id(&core->bp_manager, id);

    if (bp)
        return bp->address;
    return 0;
}

int c55x_get_trigger(conf_object_t * conf_obj)
{
    c55_core_t *core = (c55_core_t *) conf_obj->obj;

    return get_breakpoint_hit_id(&core->bp_manager);
}

bool_t c55x_check_bp_trigger(conf_object_t * conf_obj)
{
    c55_core_t *core = (c55_core_t *) conf_obj->obj;
    int ret;

    ret = check_breakpoint_hit_flag(&core->bp_manager);
    if (ret == 0)
        return True;
    return False;
}

int c55x_clear_bp_trigger(conf_object_t * conf_obj)
{
    c55_core_t *core = (c55_core_t *) conf_obj->obj;

    clear_breakpoint_hit_flag(&core->bp_manager);
    return 0;
}

exception_t memory_space_set(conf_object_t * obj, conf_object_t * obj2, const char *port, int index)
{
    c55_core_t *core = (c55_core_t *) obj->obj;

    core->memory_space = obj2;
    if (core->memory_space == NULL)
    {
        skyeye_log(Warning_log, __FUNCTION__, "Set 'memory space' attribute Error");
        return Not_found_exp;
    }
    core->memory_space_iface = (memory_space_intf *) SKY_get_iface(core->memory_space, MEMORY_SPACE_INTF_NAME);
    if (core->memory_space_iface == NULL)
    {
        skyeye_log(Warning_log, __FUNCTION__, "Get 'memory_space' interface from %s fail.", obj2->objname);
        return Not_found_exp;
    }
    return No_exp;
}

exception_t memory_space_get(conf_object_t * obj, conf_object_t ** obj2, const char **port, int index)
{
    c55_core_t *core = (c55_core_t *) obj->obj;

    *obj2 = core->memory_space;
    *port = NULL;
    return No_exp;
}
