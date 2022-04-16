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
* @file c66x_core.c
* @brief The core of c66x
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
#include <skyeye_core_intf.h>
#include <skyeye_int_register.h>
#include <sim_control.h>
#include "c6k_cpu.h"
#include "c6k_decode.h"
static const char *c66x_regstr[] = {
    "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7",
    "A8", "A9", "A10", "A11", "A12", "A13", "A14", "A15",
    "B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7",
    "B8", "B9", "B10", "B11", "B12", "B13", "B14", "B15",
    "CSR_REG", "PC_REG",
    "A16", "A17", "A18", "A19", "A20", "A21", "A22", "A23",
    "A24", "A25", "A26", "A27", "A28", "A29", "A30", "A31",
    "B16", "B17", "B18", "B19", "B20", "B21", "B22", "B23",
    "B24", "B25", "B26", "B27", "B28", "B29", "B30", "B31",
    "TSR_REG", "ILC_REG", "RILC_REG", "AMR_REG",
    "IFR_REG", "IER_REG", "ISTP_REG", "IRP_REG",
    "NRP_REG", "FADCR_REG", "FAUCR_REG", "FMCR_REG",
    "GFPGFR", "ER", "EM", "DIER",
    "ITSR", "NTSR", "EFR", "ECR", "IEER",
    "SSR", "GPYLA", "GPYLB", "TSCL", "TSCH", "DNUM",
    NULL
};
typedef enum
{
    A0 = 0, A1, A2, A3, A4, A5, A6, A7,
    A8, A9, A10, A11, A12, A13, A14, A15,
    B0, B1, B2, B3, B4, B5, B6, B7,
    B8, B9, B10, B11, B12, B13, B14, B15,
    CSR_REG, PC_REG,
    A16, A17, A18, A19, A20, A21, A22, A23,
    A24, A25, A26, A27, A28, A29, A30, A31,
    B16, B17, B18, B19, B20, B21, B22, B23,
    B24, B25, B26, B27, B28, B29, B30, B31,
    TSR_REG, ILC_REG, RILC_REG, AMR_REG,
    IFR_REG, IER_REG, ISTP_REG, IRP_REG,
    NRP_REG, FADCR_REG, FAUCR_REG, FMCR_REG,
    GFPGFR_REG, ER_REG, EM_REG, DIER_REG,
    ITSR_REG, NTSR_REG, EFR_REG, ECR_REG, IEER_REG,
    SSR_REG, GPYLA_REG, GPYLB_REG, TSCL_REG, TSCH_REG, DNUM_REG,
    C66X_REGNUM
} reg_id_t;

static uint32_t c66x_get_regnum(conf_object_t * obj)
{
    return C66X_REGNUM;
}

static uint32_t c66x_get_regvalue_by_id(conf_object_t * obj, uint32_t id)
{
    c6k_core_t *core = (c6k_core_t *) obj->obj;

    if (id == PC_REG)
        return core->pc;
    else if (id >= A0 && id <= A15)
        return core->gpr[GPR_A][id - A0];
    else if (id >= A16 && id <= A31)
        return core->gpr[GPR_A][id - A16 + 16];
    else if (id >= B0 && id <= B15)
        return core->gpr[GPR_B][id - B0];
    else if (id >= B16 && id <= B31)
        return core->gpr[GPR_B][id - B16 + 16];
    else if (id == CSR_REG)
        return core->csr;
    else if (id == AMR_REG)
        return core->amr;
    else if (id == IFR_REG)
        return core->ifr;
    else if (id == IER_REG)
        return core->ier;
    else if (id == ISTP_REG)
        return core->istp;
    else if (id == IRP_REG)
        return core->irp;
    else if (id == NRP_REG)
        return core->nrp;
    else if (id == FADCR_REG)
        return core->fadcr;
    else if (id == FMCR_REG)
        return core->fmcr;
    else
        return 0x0;                     /* something wrong */
}

static char *c66x_get_regname_by_id(conf_object_t * conf_obj, uint32_t id)
{
    assert((id >= 0) && (id <= C66X_REGNUM));
    return (char *) c66x_regstr[id];
}

static exception_t c66x_set_regvalue_by_id(conf_object_t * obj, uint32_t value, uint32_t id)
{
    c6k_core_t *core = (c6k_core_t *) obj->obj;

    if (id >= A0 && id <= A15)
        core->gpr[GPR_A][id - A0] = value;
    else if (id >= A16 && id <= A31)
        core->gpr[GPR_A][id - A16 + 16] = value;
    else if (id >= B0 && id <= B15)
        core->gpr[GPR_B][id - B0] = value;
    else if (id >= B16 && id <= B31)
        core->gpr[GPR_B][id - B16 + 16] = value;
    else if (id == PC_REG)
        core->pc = value;
    else if (id == CSR_REG)
        core->csr = value;
    else if (id == AMR_REG)
        core->amr = value;
    else if (id == IFR_REG)
        core->ifr = value;
    else if (id == IER_REG)
        core->ier = value;
    else if (id == ISTP_REG)
        core->istp = value;
    else if (id == IRP_REG)
        core->irp = value;
    else if (id == NRP_REG)
        core->nrp = value;
    else if (id == FADCR_REG)
        core->fadcr = value;
    else if (id == FMCR_REG)
        core->fmcr = value;

    return No_exp;
}

uint32 c66x_get_regid_by_name(conf_object_t * opaque, char *name)
{
    int i;

    for (i = 0; i < C66X_REGNUM; i++)
    {
        if (strcmp(name, c66x_regstr[i]) == 0)
            return i;
    }
    return 0;
}

static conf_object_t *new_c66x_core(const char *obj_name)
{
    return new_c6k_core(obj_name);
}

static exception_t free_c66x_core(conf_object_t * conf_obj)
{
    free_c6k_core(conf_obj);
    return No_exp;
}

exception_t reset_c66x_core(conf_object_t * obj, const char *parameter)
{
    reset_c6k_core(obj, parameter);
    return No_exp;
}

exception_t config_c66x_core(conf_object_t * obj, const char *parameter)
{
    c6k_core_t *core = obj->obj;

    config_c6k_core(obj);
    core->csr &= 0xffff;
    core->csr |= ((21 << 24) | (1 << 16));
    return No_exp;
}

static void c66x_register_attribute(conf_class_t * clss)
{
    return c6k_register_attribute(clss);
}

void init_c66x_core()
{
    static skyeye_class_t class_data = {
        .class_name = "c66x_core",
        .class_desc = "TI C66x Floating Point processor core",
        .new_instance = new_c66x_core,
        .free_instance = free_c66x_core,
        .reset_instance = reset_c66x_core,
        .config_instance = config_c66x_core,
    };

    conf_class_t *clss = SKY_register_cpu_class(class_data.class_name, &class_data);
    static const core_info_intf core_info = {
        .set_pc = c6k_set_pc,
        .get_pc = c6k_get_pc,
        .get_steps = c6k_get_step,
        .get_endian = c6k_get_endian,
        .get_alignment = c6k_get_alignment,
        .reset = c6k_reset,
        .disassemble = c6k_disassemble,
        .get_architecture = c6k_get_architecture,
        .get_address_width = c6k_get_address_width,
        .get_cpu_run_status = c6k_get_cpu_run_status,
        .set_memory_watch_status = c6k_set_memory_watch_status,
        .get_cpu_exception_num = c6k_get_cpu_exception_num,
        .set_cpu_exception_by_id = c6k_set_cpu_exception_by_id,
        .get_cpu_exception_name_by_id = c6k_get_cpu_exception_name_by_id
    };
    SKY_register_iface(clss, CORE_INFO_INTF_NAME, &core_info);

    static const skyeye_reg_intf reg = {
        .get_regvalue_by_id = c66x_get_regvalue_by_id,
        .get_regname_by_id = c66x_get_regname_by_id,
        .set_regvalue_by_id = c66x_set_regvalue_by_id,
        .get_regid_by_name = c66x_get_regid_by_name,
        .get_regnum = c66x_get_regnum,
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &reg);

    static const core_exec_intf core_exec = {
        .run = c6k_per_cpu_step,
        .stepi = c6k_stepi,
        .stop = NULL,
    };
    SKY_register_iface(clss, CORE_EXEC_INTF_NAME, &core_exec);

    static const core_signal_intf core_signal = {
        .signal = c6k_signal,
    };
    SKY_register_iface(clss, CORE_SIGNAL_INTF_NAME, &core_signal);

    static const memory_space_intf memory_space = {
        .read = c6k_multicore_space_read,
        .write = c6k_multicore_space_write
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &memory_space);
    static const core_breakpoint_op_intf bp_op = {
        .insert_bp = c6k_insert_bp,
        .delete_bp = c6k_delete_bp,
        .get_bp_numbers = c6k_get_bp_numbers,
        .get_bp_addr_by_id = c6k_get_bp_addr_by_id,
        .get_bp_trigger = c6k_get_trigger,
        .check_bp_trigger = c6k_check_bp_trigger,
        .clear_bp_trigger = c6k_clear_bp_trigger,
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
            .set = c6k_memory_space_set,
            .get = c6k_memory_space_get,
        }
    };
    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);

    c66x_register_attribute(clss);
}
