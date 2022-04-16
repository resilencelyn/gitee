/*
 * e200.c:
 *
 * Copyright (C) 2014 Oubang Shen <shenoubang@gmail.com>
 * Skyeye Develop Group, for help please send mail to
 * <skyeye-developer@lists.gro.clinux.org>
 *
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <skyeye_types.h>
#include <skyeye_class.h>
#include <skyeye_core_intf.h>
#include <skyeye_int_register.h>
#include <memory_space.h>
#include <skyeye_iface.h>
#include <skyeye_attribute.h>
#include <skyeye_mm.h>
#include "ppc_mmu.h"
#include "ppc_exc.h"
#include "ppc_cpu.h"
#include "ppc_dec.h"
#include "ppc_io.h"
#include "e200.h"
#include "skyeye_log.h"

/*
 * get ccsr base address according to the register value
 */
static uint32 e200_get_ccsr_base(uint32 reg)
{
    return reg;
}

static void e200_dec_io_do_cycle(e500_core_t * core)
{
    core->tbl++;
    /* if tbl overflow, we increase tbh */
    if (core->tbl == 0)
        core->tbu++;
    /* trigger interrupt when dec value from 1 to 0 */
    if (core->dec > 0)
        core->dec--;
    /* if decrementer eqauls zero */
    if((core->tcr & DEC_TIMER_ENABLE) != 0)
    {
        if ((core->dec == 0) && (core->msr & MSR_EE))
        {
            /* trigger timer interrupt */
            ppc_exception(core, DEC, 0, core->pc);
            core->dec = 150000;
        }
    }

    return;
}

static exception_t raise_signal(conf_object_t * obj, interrupt_signal_t * signal)
{
    e500_core_t *core = (e500_core_t *) obj->obj;

    if (core->msr & 0x8000)             //EE == 1
    {
        core->ipi_flag = 1;

        if (signal != NULL)
        {
            core->hardware_vector_mode = signal->powerpc_signal.hardware_vector_mode;
        } else
        {
            core->hardware_vector_mode = 0;
        }

        if (core->hardware_vector_mode)
        {
            core->hardware_vector_offset = signal->powerpc_signal.irq_vector_number * 16;
        }
    }
}

static signal_status_t get_signal_status(conf_object_t * obj)
{
    e500_core_t *core = (e500_core_t *) obj->obj;

    signal_status_t status;

    // default no signal
    status = SIGNAL_Pending;

    if (core->ipi_flag == 1)
    {
        status = SIGNAL_Pending;
    }

    if (core->exception_status)
    {
        status = SIGNAL_Active;
    }

    return status;
}

static conf_object_t *new_e200_core(char *obj_name)
{
    conf_object_t *obj = new_ppc_core(obj_name);
    e500_core_t *core = (e500_core_t *) obj->obj;

    int j;

    for (j = 0; j < 16; j++)
    {
        core->sr[j] = 0x2aa * j;
    }

    core->pvr = 0x80040010;             /* PVR for mpc8641D */
    /* e200 core initialization */
    core->effective_to_physical = e600_effective_to_physical;
    core->ppc_exception = e500_ppc_exception;
    core->syscall_number = PPC_EXC_SC;
    core->dec_io_do_cycle = e200_dec_io_do_cycle;
    core->get_ccsr_base = e200_get_ccsr_base;
    //core->ccsr_size = 0xfffff;
    core->ccsr_size = 0x100000;
    /* FIXME, we should give the default value to all the register
     * according to the reset settings section of e200 manual
     */
    core->msr = 0x00000040;
    core->l1cfg[0] = 0x28581810;
    core->l1cfg[1] = 0x8581810;
    core->isWake = 1;

    return obj;
}

void free_e200(conf_object_t * obj)
{

    free_ppc_core(obj);
    return;
}

exception_t reset_e200(conf_object_t * obj, const char *parameter)
{
    e500_core_t *core = (e500_core_t *) (obj->obj);

    return No_exp;
}

static void e200_reset(conf_object_t * conf_obj)
{
    return;
}

static exception_t e200_update_memiface_cache(conf_object_t * conf_obj)
{
    return No_exp;
}

static attr_value_t get_attr_ccsr(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    e500_core_t *core = (e500_core_t *) conf_obj->obj;
    attr_value_t value = SKY_make_attr_object(core->cpu->ccsr);

    return value;
}

static exception_t set_attr_ccsr(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    e500_core_t *core = (e500_core_t *) conf_obj->obj;

    core->cpu->ccsr = SKY_attr_object(*value);
    return No_exp;
}

void init_e200()
{
    static skyeye_class_t class_data = {
        .class_name = "e200_core",
        .class_desc = "powerpc e200 core",
        .new_instance = new_e200_core,
        .free_instance = free_e200,
        .config_instance = config_ppc_core,
        .reset_instance = reset_e200,
        .get_attr = NULL,
        .set_attr = NULL
    };
    conf_class_t *clss = SKY_register_cpu_class(class_data.class_name, &class_data);
    static const core_info_intf core_info = {
        .set_pc = ppc_set_pc,
        .get_pc = ppc_get_pc,
        .get_steps = ppc_get_step,
        .get_endian = ppc_get_endian,
        .get_alignment = ppc_get_alignment,
        .set_textsection_info = ppc_set_textsection_info,
        .get_architecture = ppc_get_architecture,
        .disassemble = ppc_disassemble,
        .reset = e200_reset,
        .get_address_width = ppc_get_address_width,
        .set_memory_watch_status = ppc_set_memory_watch_status,
        .get_cpu_exception_num = ppc_get_cpu_exception_num,
        .set_cpu_exception_by_id = ppc_set_cpu_exception_by_id,
        .get_cpu_exception_name_by_id = ppc_get_cpu_exception_name_by_id
    };
    SKY_register_iface(clss, CORE_INFO_INTF_NAME, &core_info);

    static const skyeye_reg_intf reg = {
        .get_regvalue_by_id = ppc_get_regval_by_id,
        .get_regname_by_id = ppc_get_regname_by_id,
        .set_regvalue_by_id = ppc_set_regval_by_id,
        .get_regid_by_name = ppc_get_regid_by_name,
        .get_regnum = ppc_get_regnum
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &reg);

    static const skyeye_reg64_intf reg64 = {
        .get_regvalue_by_id = ppc_get_reg64val_by_id,
        .get_regname_by_id = ppc_get_regname_by_id,
        .set_regvalue_by_id = ppc_set_reg64val_by_id,
        .get_regid_by_name = ppc_get_regid_by_name,
        .get_regnum = ppc_get_regnum
    };
    SKY_register_iface(clss, SKYEYE_REG64_INTF, &reg64);

    static const core_exec_intf core_exec = {
        .run = ppc_run,
        .stop = NULL,
        .stepi = ppc_stepi,
    };
    SKY_register_iface(clss, CORE_EXEC_INTF_NAME, &core_exec);

    static const memory_space_intf memory_space = {
        .read = ppc_space_read,
        .write = ppc_space_write,
        .get_page = ppc_get_page
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &memory_space);

    static const core_update_memiface_cache core_update_memiface_cache = {
        .update_memiface_cache = e200_update_memiface_cache,
    };

    SKY_register_iface(clss, CORE_UPDATE_MEMIFACE_CACHE_NAME, &core_update_memiface_cache);

    static const core_signal_intf core_signal = {
        .signal = raise_signal,
        .get_signal_status = get_signal_status,
    };
    SKY_register_iface(clss, CORE_SIGNAL_INTF_NAME, &core_signal);

    static const core_breakpoint_op_intf bp_op = {
        .insert_bp = ppc_insert_bp,
        .delete_bp = ppc_delete_bp,
        .get_bp_numbers = ppc_get_bp_numbers,
        .get_bp_addr_by_id = ppc_get_bp_addr_by_id,
        .get_bp_trigger = ppc_get_trigger,
        .check_bp_trigger = ppc_check_bp_trigger,
        .clear_bp_trigger = ppc_clear_bp_trigger,
    };
    SKY_register_iface(clss, CORE_BREAKPOINT_OP_INTF_NAME, &bp_op);

    static const struct InterfaceDescription ifaces[] = {
        (struct InterfaceDescription) {
            .name = CORE_EXEC_INTF_NAME,
            .iface = &core_exec,
        },
        (struct InterfaceDescription) {
            .name = MEMORY_SPACE_INTF_NAME,
            .iface = &memory_space,
        },
        (struct InterfaceDescription) {
            .name = CORE_SIGNAL_INTF_NAME,
            .iface = &core_signal,
        },
        (struct InterfaceDescription) {
            .name = CORE_BREAKPOINT_OP_INTF_NAME,
            .iface = &bp_op,
        },
        (struct InterfaceDescription) {
            .name = SKYEYE_REG_INTF,
            .iface = &reg,
        },
        (struct InterfaceDescription) {
            .name = SKYEYE_REG64_INTF,
            .iface = &reg64,
        }
    };

    static const struct ConnectDescription connects[] = {
        (struct ConnectDescription) {
            .name = MEMORY_SPACE_INTF_NAME,
            .set = ppc_memory_space_set,
            .get = ppc_memory_space_get,
        }
    };
    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);

    ppc_register_attribute(clss);
}
