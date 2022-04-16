/*
 * sparc_v7.c:
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
#include <skyeye_attribute.h>
#include "sparc_v7.h"
#include "skyeye_mm.h"
#include "skyeye_iface.h"
//#include "dyncom/sparc_dyncom_run.h"
//#include "dyncom/sparc_dyncom_interface.h"
#include "sparc_arch_interface.h"
#include "sim_control.h"
#include "skyeye_callback.h"
#include "skyeye_disassemble.h"

void init_sparc_v7()
{
    static skyeye_class_t class_data = {
        .class_name = "sparc_v7",
        .class_desc = "sparc v7 core",
        .new_instance = new_sparc,
        .free_instance = free_sparc,
        .reset_instance = reset_sparc,
        .config_instance = config_sparc,
        .get_attr = NULL,
        .set_attr = NULL
    };
    conf_class_t *clss = SKY_register_cpu_class(class_data.class_name, &class_data);

    static const core_info_intf core_info = {
        .set_pc = sparc_set_pc,
        .get_pc = sparc_get_pc,
        .get_steps = sparc_get_steps,
        .get_endian = sparc_get_endian,
        .get_alignment = sparc_get_alignment,
        .get_architecture = sparc_get_architecture,
        .reset = sparc_reset,
        .disassemble = sparc_disassemble,
        .get_address_width = sparc_get_address_width,
        .get_cpu_exception_num = sparc_get_cpu_exception_num,
        .set_cpu_exception_by_id = sparc_set_cpu_exception_by_id,
        .get_cpu_exception_name_by_id = sparc_get_cpu_exception_name_by_id
    };
    SKY_register_iface(clss, CORE_INFO_INTF_NAME, &core_info);

    static const skyeye_reg_intf reg = {
        .get_regvalue_by_id = sparc_get_regvalue_by_id,
        .get_regname_by_id = sparc_get_regname_by_id,
        .set_regvalue_by_id = sparc_set_regvalue_by_id,
        .get_regid_by_name = sparc_get_regid_by_name,
        .get_regnum = sparc_get_regnum
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &reg);

    static const core_exec_intf core_exec = {
        .run = sparc_core_run,
        .stop = NULL,
        .stepi = sparc_stepi,
        .power = sparc_power,
    };
    SKY_register_iface(clss, CORE_EXEC_INTF_NAME, &core_exec);

    static const memory_space_intf memory_space = {
        .read = sparc_space_read,
        .write = sparc_space_write,
        .get_page = sparc_get_page
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &memory_space);

    static const core_signal_intf core_signal = {
        .signal = sparc_signal,
    };
    SKY_register_iface(clss, CORE_SIGNAL_INTF_NAME, &core_signal);

    static const core_breakpoint_op_intf bp_op = {
        .insert_bp = sparc_insert_bp,
        .delete_bp = sparc_delete_bp,
        .get_bp_numbers = sparc_get_bp_numbers,
        .get_bp_addr_by_id = sparc_get_bp_addr_by_id,
        .get_bp_trigger = sparc_get_trigger,
        .check_bp_trigger = sparc_check_bp_trigger,
        .clear_bp_trigger = sparc_clear_bp_trigger,
    };
    SKY_register_iface(clss, CORE_BREAKPOINT_OP_INTF_NAME, &bp_op);

    static const struct InterfaceDescription ifaces[] = {
        (struct InterfaceDescription) {
            .name = CORE_INFO_INTF_NAME,
            .iface = &core_info,
        },
        (struct InterfaceDescription) {
            .name = SKYEYE_REG_INTF,
            .iface = &reg,
        },
        (struct InterfaceDescription) {
            .name = CORE_EXEC_INTF_NAME,
            .iface = &core_exec,
        },
        (struct InterfaceDescription) {
            .name = MEMORY_SPACE_INTF_NAME,
            .iface = &memory_space,
        },
        (struct InterfaceDescription) {
            .name = CORE_BREAKPOINT_OP_INTF_NAME,
            .iface = &bp_op,
        },
        (struct InterfaceDescription) {
            .name = CORE_SIGNAL_INTF_NAME,
            .iface = &core_signal,
        }

    };
    static const struct ConnectDescription connects[] = {
        (struct ConnectDescription) {
            .name = MEMORY_SPACE_INTF_NAME,
            .set = memory_space_set,
            .get = memory_space_get,
        }
    };
    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);

    sparc_register_attribute(clss);
}
