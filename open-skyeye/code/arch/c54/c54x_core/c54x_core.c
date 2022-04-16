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
* @file c54x_core.c
* @brief The core of c54x
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-09-03
*/
#include "c54x_cpu.h"
static const char *c54x_regstr[] = {
    "PC_REG", "TRN", "SP_REG", "ST0", "ST1", "AR0", "AR1", "AR2", "AR3", "AR4", "AR5",
    "AR6", "AR7", "TREG", "ACC_A", "ACC_B", "PMST", "BK_REG", "BRC", "IMR", "RSA",
    "IFR", "REA", "PMR_VALUE",
    NULL
};
typedef enum
{
    PC_REG = 0, TRN, SP_REG, ST0, ST1, AR0, AR1, AR2, AR3, AR4, AR5, AR6, AR7,
    TREG, ACC_A, ACC_B, PMST, BK_REG, BRC, IMR, RSA, IFR, REA, PMR_VALUE,
    C54_REGNUM
} reg_id_t;

static conf_object_t *new_c54x_core(char *obj_name)
{
    c54x_core_t *core = skyeye_mm_zero(sizeof (c54x_core_t));
    conf_object_t *obj = new_conf_object(obj_name, core);

    core->obj = obj;

    core->c54x_memory_intf = skyeye_mm_zero(sizeof (memory_space_intf));
    core->c54x_memory_intf->read = c54x_space_read;
    core->c54x_memory_intf->write = c54x_space_write;

    core->delay_slot_cycle = 0;
    core->delay_enable = 0;
    core->pend_nops_flag = 1;
    core->rptc = -1;
    core->MMR_BUFFER[CPU_ST0] = 0x1800;
    core->MMR_BUFFER[CPU_ST1] = 0x2900;
    core->MMR_BUFFER[CPU_SP] = 0x5126;
    core->MMR_BUFFER[CPU_PMST] = 0xffc0;

    return core->obj;
}

void free_c54x_core(conf_object_t * dev)
{
    c54x_core_t *core = (c54x_core_t *) dev->obj;

    skyeye_free(core->obj);
    skyeye_free(core);

}

exception_t reset_c54x_core(conf_object_t * obj, const char *parameter)
{
    c54x_core_t *core = (c54x_core_t *) obj->obj;

    return No_exp;
}

static uint32 c54x_get_regnum(conf_object_t * obj)
{
    return C54_REGNUM;
}

static uint32 c54x_get_regvalue_by_id(conf_object_t * obj, uint32 id)
{
    c54x_core_t *core = (c54x_core_t *) obj->obj;

    switch (id)
    {
        case PC_REG:
            return core->pc;
        case TRN:
            return core->MMR_BUFFER[CPU_TRN];
        case SP_REG:
            return core->MMR_BUFFER[CPU_SP];
        case ST0:
            return core->MMR_BUFFER[CPU_ST0];
        case ST1:
            return core->MMR_BUFFER[CPU_ST1];
        case AR0:
        case AR1:
        case AR2:
        case AR3:
        case AR4:
        case AR5:
        case AR6:
        case AR7:
            return core->MMR_BUFFER[CPU_AR0 + id - AR0];
        case TREG:
            return core->MMR_BUFFER[CPU_TREG];
        case ACC_A:
            return core->acc_a;
        case ACC_B:
            return core->acc_b;
        case PMST:
            return core->MMR_BUFFER[CPU_PMST];
        case BK_REG:
            return core->MMR_BUFFER[CPU_BK];
        case BRC:
            return core->MMR_BUFFER[CPU_BRC];
        case IMR:
            return core->MMR_BUFFER[CPU_IMR];
        case RSA:
            return core->MMR_BUFFER[CPU_RSA];
        case IFR:
            return core->MMR_BUFFER[CPU_IFR];
        case REA:
            return core->MMR_BUFFER[CPU_REA];
        case PMR_VALUE:
            return core->pmr_value;
        default:
            /* something wrong */
            return 0xFFFFFFFF;
    }

}
static exception_t c54x_set_regvalue_by_id(conf_object_t * obj, uint32 value, uint32 id)
{
    c54x_core_t *core = (c54x_core_t *) obj->obj;

    switch (id)
    {
        case PC_REG:
            core->pc = value;
            break;
        case TRN:
            core->MMR_BUFFER[CPU_TRN] = value;
            break;
        case SP_REG:
            core->MMR_BUFFER[CPU_SP] = value;
            break;
        case ST0:
            core->MMR_BUFFER[CPU_ST0] = value;
            break;
        case ST1:
            core->MMR_BUFFER[CPU_ST1] = value;
            break;
        case AR0:
        case AR1:
        case AR2:
        case AR3:
        case AR4:
        case AR5:
        case AR6:
        case AR7:
            core->MMR_BUFFER[CPU_AR0 + id - AR0] = value;
            break;
        case TREG:
            core->MMR_BUFFER[CPU_TREG] = value;
            break;
        case ACC_A:
            core->acc_a = value;
            break;
        case ACC_B:
            core->acc_b = value;
            break;
        case PMST:
            core->MMR_BUFFER[CPU_PMST] = value;
            break;
        case BK_REG:
            core->MMR_BUFFER[CPU_BK] = value;
            break;
        case BRC:
            core->MMR_BUFFER[CPU_BRC] = value;
            break;
        case IMR:
            core->MMR_BUFFER[CPU_IMR] = value;
            break;
        case RSA:
            core->MMR_BUFFER[CPU_RSA] = value;
            break;
        case IFR:
            core->MMR_BUFFER[CPU_IFR] = value;
            break;
        case REA:
            core->MMR_BUFFER[CPU_REA] = value;
            break;
        case PMR_VALUE:
            core->pmr_value = value;
            break;
        default:
            /* something wrong */
            skyeye_log(Error_log, __func__, "Can't write value to the register at id :%d\n", id);
            break;
    }
    return No_exp;
}
static uint32 c54x_get_regid_by_name(conf_object_t * opaque, char *name)
{
    int i;

    for (i = 0; i < C54_REGNUM; i++)
    {
        if (strcmp(name, c54x_regstr[i]) == 0)
            return i;
    }
    return 0;
}
static const char *c54x_get_regname_by_id(conf_object_t * obj, uint32 id)
{
    return c54x_regstr[id];
}

DEF_LIST_UINTEGER_ATTR_FUNC(c54x_core_t, MMR_BUFFER, 32);
DEF_UINTEGER_ATTR_FUNC(c54x_core_t, pc);
DEF_UINTEGER_ATTR_FUNC(c54x_core_t, pmr_value);
DEF_UINTEGER_ATTR_FUNC(c54x_core_t, stop_step);
DEF_UINTEGER_ATTR_FUNC(c54x_core_t, insn_num);
DEF_UINTEGER_ATTR_FUNC(c54x_core_t, intr_flag);

static void c54x_register_attribute(conf_class_t * clss)
{
    DEF_ATTR(MMR_BUFFER, list, NULL);
    DEF_ATTR(pc, uinteger, NULL);
    DEF_ATTR(pmr_value, uinteger, NULL);
    DEF_ATTR(stop_step, uinteger, NULL);
    DEF_ATTR(insn_num, uinteger, NULL);
    DEF_ATTR(intr_flag, uinteger, NULL);
    return;
}

void init_c54x_core()
{
    static skyeye_class_t class_data = {
        .class_name = "c54x_core",
        .class_desc = "TI c54x Floating Point processor core",
        .new_instance = new_c54x_core,
        .free_instance = free_c54x_core,
        .reset_instance = reset_c54x_core,
        .get_attr = NULL,
        .set_attr = NULL
    };

    conf_class_t *clss = SKY_register_cpu_class(class_data.class_name, &class_data);
    static const core_info_intf core_info = {
        .set_pc = c54x_set_pc,
        .get_pc = c54x_get_pc,
        .get_steps = c54x_get_steps,
        .get_endian = c54x_get_endian,
        .get_alignment = c54x_get_alignment,
        .reset = c54x_reset,
        .get_address_width = c54x_get_address_width,
        .disassemble = c54x_disassemble,
        .get_architecture = c54x_get_architecture,
        .get_cpu_exception_num = c54x_get_cpu_exception_num,
        .set_cpu_exception_by_id = c54x_set_cpu_exception_by_id,
        .get_cpu_exception_name_by_id = c54x_get_cpu_exception_name_by_id
    };
    SKY_register_iface(clss, CORE_INFO_INTF_NAME, &core_info);

    static const skyeye_reg_intf reg = {
        .get_regvalue_by_id = c54x_get_regvalue_by_id,
        .get_regname_by_id = c54x_get_regname_by_id,
        .set_regvalue_by_id = c54x_set_regvalue_by_id,
        .get_regid_by_name = c54x_get_regid_by_name,
        .get_regnum = c54x_get_regnum,
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &reg);

    static const core_exec_intf core_exec = {
        .run = per_cpu_step,
        .stepi = c54x_stepi,
        .stop = NULL,
    };
    SKY_register_iface(clss, CORE_EXEC_INTF_NAME, &core_exec);

    static const core_signal_intf core_signal = {
        .signal = c54x_signal,
    };
    SKY_register_iface(clss, CORE_SIGNAL_INTF_NAME, &core_signal);

    static const memory_space_intf memory_space = {
        .read = c54x_space_read,
        .write = c54x_space_write
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &memory_space);

    static const core_breakpoint_op_intf bp_op = {
        .insert_bp = c54x_insert_bp,
        .delete_bp = c54x_delete_bp,
        .get_bp_numbers = c54x_get_bp_numbers,
        .get_bp_addr_by_id = c54x_get_bp_addr_by_id,
        .get_bp_trigger = c54x_get_trigger,
        .check_bp_trigger = c54x_check_bp_trigger,
        .clear_bp_trigger = c54x_clear_bp_trigger,
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

    c54x_register_attribute(clss);
}
