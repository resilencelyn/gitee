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
* @file c28x_core.c
* @brief The core of c28x or c28x
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
#include <skyeye_callback.h>
#include <skyeye_disassemble.h>
#include <skyeye_core_intf.h>
#include <skyeye_int_register.h>
#include <sim_control.h>
#include "c28_cpu.h"
#include "c28_decode.h"

uint32_t exec_insn_cache(c28_core_t * core, insn_cache_t * cache);
extern void check_periph_intr(c28_core_t * core, int irq_group_num, int irq_num);
extern void check_non_periph_intr(c28_core_t * core, int cpu_irq_num);

void c28x_set_pc(conf_object_t * obj, generic_address_t addr)
{
    c28_core_t *core = (c28_core_t *) obj->obj;

    core->pc = addr;
    return;
}

generic_address_t c28x_get_pc(conf_object_t * obj)
{
    c28_core_t *core = (c28_core_t *) obj->obj;

    return core->pc;
}

uint32_t c28x_get_address_width(conf_object_t * obj)
{
    return 1;
}

char *c28x_disassemble(conf_object_t * obj, generic_address_t addr)
{
    c28_core_t *core = (c28_core_t *) obj->obj;

    return skyeye_disassemble(obj, addr);
}

char *c28x_get_architecture(conf_object_t * obj)
{
    c28_core_t *core = (c28_core_t *) obj->obj;

    return "tic28x";
}

void c28x_get_cpu_run_status(conf_object_t * obj, uint32_t * cpu_freq, uint64_t * cpu_insn_num)
{
    c28_core_t *core = (c28_core_t *) obj->obj;

    *cpu_freq = core->freq_hz;
    *cpu_insn_num = core->insn_num;
    return;
}

void c28x_set_memory_watch_status(conf_object_t * obj, uint32_t addr, int status)
{
    c28_core_t *core = (c28_core_t *) obj->obj;

    //Because c28x no dyncom or hybird mode, so not implement

    return;
}

int c28x_get_cpu_exception_num(conf_object_t * obj)
{
    c28_core_t *core = (c28_core_t *) obj->obj;

    return C28X_IRQ_NUM;
}

void c28x_set_cpu_exception_by_id(conf_object_t * obj, int exception_id)
{
    c28_core_t *core = (c28_core_t *) obj->obj;

    if (exception_id > 11 && exception_id < C28X_IRQ_NUM)
    {
        RW_WRLOCK(core->ifr_lock);
        core->ifr |= (1 << exception_id);
        core->pie_group_num = exception_id;
        core->signal_type = Non_Periph_Type;
        RW_UNLOCK(core->ifr_lock);
    } else
    {
        //need trigger IRQ to PIE
    }

    return;
}

char *c28x_get_cpu_exception_name_by_id(conf_object_t * obj, int exception_id)
{
    c28_core_t *core = (c28_core_t *) obj->obj;

    strcpy(core->exception_name, "IRQ");

    assert(exception_id < C28X_IRQ_NUM);
    sprintf(core->exception_name, "%s%d", core->exception_name, exception_id);
    return core->exception_name;
}

exception_t c28x_space_read(conf_object_t * obj, generic_address_t addr, void *buf, size_t count)
{
    c28_core_t *core = (c28_core_t *) obj->obj;
    memory_space_intf *iface = core->memory_space_iface;

    return iface->read(core->memory_space, addr, buf, count);
}

exception_t c28x_space_write(conf_object_t * obj, generic_address_t addr, void *buf, size_t count)
{
    c28_core_t *core = (c28_core_t *) obj->obj;
    memory_space_intf *iface = core->memory_space_iface;

    return iface->write(core->memory_space, addr, buf, count);
}

exception_t c28x_space_read_hl(conf_object_t * obj, generic_address_t addr, void *buf, size_t count, int hl_flag)
{
    c28_core_t *core = (c28_core_t *) obj->obj;
    memory_space_intf *iface = core->memory_space_iface;

    return iface->read_hl(core->memory_space, addr, buf, count, hl_flag);
}

exception_t c28x_space_write_hl(conf_object_t * obj, generic_address_t addr, void *buf, size_t count, int hl_flag)
{
    c28_core_t *core = (c28_core_t *) obj->obj;
    memory_space_intf *iface = core->memory_space_iface;

    return iface->write_hl(core->memory_space, addr, buf, count, hl_flag);
}

void c28x_stepi(conf_object_t * obj, uint64_t steps)
{
    c28_core_t *core = (c28_core_t *) obj->obj;

    core->stop_step = core->insn_num + 1;
    return;
}

int c28x_step_once(conf_object_t * conf_obj)
{
    c28_core_t *core = (c28_core_t *) conf_obj->obj;
    uint32_t addr = core->pc;
    insn_cache_t *cache = &(core->insn_cache[HASH(addr)]);

    uint64_t cycle_num = core->insn_num;

    //check repeat block loop
    if (core->rb_bit.RA == 1)
    {
        if (core->rb_bit.RC != 0)
        {
            if ((core->pc - core->insn_length) == core->rb_ret)
            {
                core->pc = core->rb_ret - core->rb_bit.RSIZE;
                core->rb_bit.RC--;
            }
        } else
            core->rb_bit.RA = 0;
    }
    // PC = 0 need to process
    if ((cache->addr != addr) || (cache->addr == 0))    /* miss */
    {
        /* do translate and replace the old one */
        uint32_t insn;

        cache->addr = addr;
        c28x_space_read(conf_obj, addr, &insn, 32 / 8);
        cache->insn = insn;
        trans_opcode_fields(insn, &(cache->fields));
        cache->action = get_insn_action(core, insn);
    }
    exec_insn_cache(core, cache);
    SKY_pc_hap_occurred(NULL, conf_obj, core->pc);
    int id = check_breakpoint(&(core->bp_manager), core->pc, core->insn_num);
    int i, j;

    if (core->signal_type == Periph_Type)
    {
        for (i = 0; i < PIE_GROUP_NUM; i++)
        {
            for (j = 0; j < PIE_INT_NUM; j++)
            {
                if (core->pie_intr_num[i][j] == 1)
                {
                    check_periph_intr(core, i, j);
                    break;
                }
            }
        }
    } else if (core->signal_type == Non_Periph_Type)
    {
        check_non_periph_intr(core, core->pie_group_num);
    }

    if (core->stop_step != 0 && core->insn_num != 0 && core->stop_step == core->insn_num)
    {
        SIM_stop(NULL);
        core->stop_step = 0;
    }
    if (core->pc == 0)
        skyeye_log(Error_log, __func__, "Get the error ic value\n");
    return (core->insn_num - cycle_num);
}

void c28x_reset(conf_object_t * obj)
{
    c28_core_t *core = (c28_core_t *) obj->obj;

    return;
}

int per_cpu_step(conf_object_t * obj)
{
    int step = c28x_step_once(obj);

    return step;
}

endian_t c28x_get_endian(conf_object_t * obj)
{
    return Little_endian;
}

align_t c28x_get_alignment(conf_object_t * obj)
{
    return Align;
}

exception_t c28x_Non_Periph_signal(conf_object_t * conf_obj, c28_signal_t * signal)
{
    c28_core_t *core = conf_obj;
    c28_signal_t *c28_signal = signal;
    int i = c28_signal->pie_group;

    if (i >= 12 && i < 16)
    {
        RW_WRLOCK(core->ifr_lock);
        core->ifr |= (1 << i);
        core->pie_group_num = i;
        core->signal_type = Non_Periph_Type;
        RW_UNLOCK(core->ifr_lock);
    } else
    {
        printf("In %s, c28x cpu interrupt group num is not defined.\n", __func__, c28_signal->pie_group);
    }
}

exception_t c28x_Periph_signal(conf_object_t * conf_obj, c28_signal_t * signal)
{
    c28_core_t *core = conf_obj;
    c28_signal_t *c28_signal = signal;
    int i = c28_signal->pie_x;
    int j = c28_signal->pie_y;

    RW_WRLOCK(core->ifr_lock);
    core->ifr |= (1 << i);
    core->pie_intr_num[i][j] = 1;
    core->signal_type = Periph_Type;
    RW_UNLOCK(core->ifr_lock);

    return No_exp;
}

exception_t c28x_signal(conf_object_t * conf_obj, c28_signal_t * signal)
{                                       /* called in another thread */
    c28_core_t *core = (c28_core_t *) conf_obj->obj;

    if (signal->irq_type == Periph_Type)
    {
        c28x_Periph_signal(core, signal);
    } else if (signal->irq_type == Non_Periph_Type)
    {
        c28x_Non_Periph_signal(core, signal);
    } else
    {
        printf("In %s, c28x cpu interrupt type: %d is not defined.\n", __FUNCTION__, signal->irq_type);
    }

    return No_exp;
}

uint64_t c28x_get_steps(conf_object_t * conf_obj)
{
    c28_core_t *core = (c28_core_t *) conf_obj->obj;

    return core->insn_num;
}

void per_cpu_stop(conf_object_t * obj)
{
    return;
}

int c28x_insert_bp(conf_object_t * conf_obj, generic_address_t addr)
{
    c28_core_t *core = (c28_core_t *) conf_obj->obj;
    int ret;

    ret = create_a_breakpoint(&core->bp_manager, addr);
    if (ret != 0)
        return ret;
    return 0;
}

int c28x_delete_bp(conf_object_t * conf_obj, generic_address_t addr)
{
    c28_core_t *core = (c28_core_t *) conf_obj->obj;
    int ret;

    ret = remove_bp_by_addr(&core->bp_manager, addr);
    if (ret != 0)
        return ret;
    return 0;
}

int c28x_get_bp_numbers(conf_object_t * conf_obj)
{
    c28_core_t *core = (c28_core_t *) conf_obj->obj;

    return get_bp_numbers(&core->bp_manager);
}

uint32_t c28x_get_bp_addr_by_id(conf_object_t * conf_obj, int id)
{
    c28_core_t *core = (c28_core_t *) conf_obj->obj;
    breakpoint_t *bp = get_bp_by_id(&core->bp_manager, id);

    if (bp)
        return bp->address;
    return 0;
}

int c28x_get_trigger(conf_object_t * conf_obj)
{
    c28_core_t *core = (c28_core_t *) conf_obj->obj;

    return get_breakpoint_hit_id(&core->bp_manager);
}

bool_t c28x_check_bp_trigger(conf_object_t * conf_obj)
{
    c28_core_t *core = (c28_core_t *) conf_obj->obj;
    int ret;

    ret = check_breakpoint_hit_flag(&core->bp_manager);
    if (ret == 0)
        return True;
    return False;
}

int c28x_clear_bp_trigger(conf_object_t * conf_obj)
{
    c28_core_t *core = (c28_core_t *) conf_obj->obj;

    clear_breakpoint_hit_flag(&core->bp_manager);
    return 0;
}

exception_t memory_space_set(conf_object_t * obj, conf_object_t * obj2, const char *port, int index)
{
    c28_core_t *core = (c28_core_t *) obj->obj;

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
    c28_core_t *core = (c28_core_t *) obj->obj;

    *obj2 = core->memory_space;
    *port = NULL;
    return No_exp;
}
