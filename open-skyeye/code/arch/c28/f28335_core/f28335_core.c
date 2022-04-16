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
* @file f28335_core.c
* @brief The core of c28x
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-09-03
*/
#include "c28_cpu.h"

#if 0
const char *f28335_regstr[] = {
    "ACC", "P_REG", "XT_REG", "XAR0", "XAR1", "XAR2", "XAR3", "XAR4", "XAR5", "XAR6", "XAR7", "PC_REG",
    "RPC", "ST0", "ST1", "DP_REG", "SP_REG", "IER", "IFR", "R0H", "R1H", "R2H", "R3H", "R4H",
    "R5H", "R6H", "R7H", "STF", "RB",
    NULL
};

typedef enum
{
    ACC = 0, P_REG, XT_REG, XAR0, XAR1, XAR2, XAR3, XAR4, XAR5, XAR6, XAR7, PC_REG, RPC, ST0, ST1, DP_REG, SP_REG,
    IER, IFR, R0H, R1H, R2H, R3H, R4H, R5H, R6H, R7H, STF, RB,
    F28335_REGNUM,
} reg_id_t;
#else
const char *f28335_regstr[] = {
    "XAR0", "XAR1", "XAR2", "XAR3", "XAR4", "XAR5", "XAR6", "XAR7", "PC_REG",
    "DP_REG", "SP_REG", "XT_REG", "P_REG", "RPTC", "RPC", "ACC", "ST0", "ST1", "IFR", "IER",

    "R0H", "R1H", "R2H", "R3H", "R4H",
    "R5H", "R6H", "R7H", "STF", "RB",
    NULL
};

typedef enum
{
    XAR0 = 0, XAR1, XAR2, XAR3, XAR4, XAR5, XAR6, XAR7, PC_REG, DP_REG, SP_REG, XT_REG,
    P_REG, RPTC, RPC, ACC, ST0, ST1, IFR, IER,

    R0H, R1H, R2H, R3H, R4H, R5H, R6H, R7H, STF, RB,
    F28335_REGNUM,
} reg_id_t;
#endif

static conf_object_t *new_f28335_core(char *obj_name)
{
    c28_core_t *core = skyeye_mm_zero(sizeof (c28_core_t));
    conf_object_t *obj = new_conf_object(obj_name, core);

    core->obj = obj;
    core->c28_memory_intf = skyeye_mm_zero(sizeof (memory_space_intf));
    core->c28_memory_intf->read = c28x_space_read;
    core->c28_memory_intf->write = c28x_space_write;
    core->c28_memory_intf->read_hl = c28x_space_read_hl;
    core->c28_memory_intf->write_hl = c28x_space_write_hl;
    core->st1 = 0x80b;
    core->freq_hz = 100000000;

    return core->obj;
}

static void free_f28335_core(conf_object_t * dev)
{
    c28_core_t *core = (c28_core_t *) dev->obj;

    skyeye_free(core->c28_memory_intf);
    skyeye_free(core->obj);
    skyeye_free(core);
}

static exception_t reset_f28335_core(conf_object_t * obj, const char *parameter)
{
    c28_core_t *core = (c28_core_t *) obj->obj;

    return No_exp;
}

static uint32 f28335_get_regnum(conf_object_t * obj)
{
    return F28335_REGNUM;
}

static uint32 f28335_get_regvalue_by_id(conf_object_t * obj, uint32 id)
{
    c28_core_t *core = (c28_core_t *) obj->obj;

    if (id == PC_REG)
        return core->pc;
    else if (id >= XAR0 && id <= XAR7)
    {
        return core->xar[id].xar;
    } else if (id == ACC)
        return core->acc;
    else if (id == P_REG)
        return core->p;
    else if (id == XT_REG)
        return core->xt;
    else if (id == DP_REG)
        return core->dp;
    else if (id == IFR)
        return core->ifr;
    else if (id == IER)
        return core->ier;
    else if (id == RPC)
        return core->rpc;
    else if (id == RPTC)
        return core->ic;
    else if (id == SP_REG)
        return core->sp;
    else if (id == ST0)
        return core->st0;
    else if (id == ST1)
        return core->st1;
    else if (id >= R0H && id <= R7H)
        return core->fp_regs[id - 19].Ra;
    else if (id == STF)
        return core->stf;
    else if (id == RB)
        return core->rb;
    else
        return 0xFFFFFFFF;              /* something wrong */
}

static exception_t f28335_set_regvalue_by_id(conf_object_t * obj, uint32 value, uint32 id)
{
    c28_core_t *core = (c28_core_t *) obj->obj;

    if (id == PC_REG)
        core->pc = value;
    else if (id >= XAR0 && id <= XAR7)
    {
        core->xar[id].xar = value;
    } else if (id == ACC)
        core->acc = value;
    else if (id == P_REG)
        core->p = value;
    else if (id == XT_REG)
        core->xt = value;
    else if (id == DP_REG)
        core->dp = value;
    else if (id == IFR)
        core->ifr = value;
    else if (id == IER)
        core->ier = value;
    else if (id == RPC)
        core->rpc = value;
    else if (id == RPTC)
        core->ic = value;
    else if (id == SP_REG)
        core->sp = value;
    else if (id == ST0)
        core->st0 = value;
    else if (id == ST1)
        core->st1 = value;
    else if (id >= R0H && id <= R7H)
        core->fp_regs[id - 19].Ra = value;
    else if (id == STF)
        core->stf = value;
    else if (id == RB)
        core->rb = value;
    else
    {
        printf("In %s, write core Register %d Error.\n", __func__, id);
    }

    return No_exp;
}

static uint32 f28335_get_regid_by_name(conf_object_t * opaque, char *name)
{
    int i;

    for (i = 0; i < F28335_REGNUM; i++)
    {
        if (strcmp(name, f28335_regstr[i]) == 0)
            return i;
    }
    return 0;
}

const char *f28335_get_regname_by_id(conf_object_t * obj, uint32 id)
{
    return f28335_regstr[id];
}

static attr_value_t get_attr_freq_hz(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    c28_core_t *core = (c28_core_t *) conf_obj->obj;
    attr_value_t freq_hz = SKY_make_attr_uinteger(core->freq_hz);

    return freq_hz;
}

static exception_t set_attr_freq_hz(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    c28_core_t *core = (c28_core_t *) conf_obj->obj;

    core->freq_hz = SKY_attr_uinteger(*value);
    return No_exp;
}

void f28335_register_attribute(conf_class_t * clss)
{
    SKY_register_attribute(clss, "freq_hz", get_attr_freq_hz, NULL, set_attr_freq_hz, NULL, SKY_Attr_Optional,
                           "uinteger", "f28335 cpu freq_hz");
}

void init_f28335_core()
{
    static skyeye_class_t class_data = {
        .class_name = "f28335_core",
        .class_desc = "TI f28335 Floating Point processor core",
        .new_instance = new_f28335_core,
        .free_instance = free_f28335_core,
        .reset_instance = reset_f28335_core,
        .get_attr = NULL,
        .set_attr = NULL
    };

    conf_class_t *clss = SKY_register_cpu_class(class_data.class_name, &class_data);

    static const core_info_intf core_info = {
        .set_pc = c28x_set_pc,
        .get_pc = c28x_get_pc,
        .get_steps = c28x_get_steps,
        .get_endian = c28x_get_endian,
        .get_alignment = c28x_get_alignment,
        .reset = c28x_reset,
        .get_address_width = c28x_get_address_width,
        .disassemble = c28x_disassemble,
        .get_architecture = c28x_get_architecture,
        .get_cpu_run_status = c28x_get_cpu_run_status,
        .set_memory_watch_status = c28x_set_memory_watch_status,
        .get_cpu_exception_num = c28x_get_cpu_exception_num,
        .set_cpu_exception_by_id = c28x_set_cpu_exception_by_id,
        .get_cpu_exception_name_by_id = c28x_get_cpu_exception_name_by_id
    };
    SKY_register_iface(clss, CORE_INFO_INTF_NAME, &core_info);

    static const skyeye_reg_intf reg = {
        .get_regvalue_by_id = f28335_get_regvalue_by_id,
        .get_regname_by_id = f28335_get_regname_by_id,
        .set_regvalue_by_id = f28335_set_regvalue_by_id,
        .get_regid_by_name = f28335_get_regid_by_name,
        .get_regnum = f28335_get_regnum,
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &reg);

    static const core_exec_intf core_exec = {
        .run = per_cpu_step,
        .stepi = c28x_stepi,
        .stop = NULL,
    };
    SKY_register_iface(clss, CORE_EXEC_INTF_NAME, &core_exec);

    static const core_signal_intf core_signal = {
        .signal = c28x_signal,
    };
    SKY_register_iface(clss, CORE_SIGNAL_INTF_NAME, &core_signal);

    static const memory_space_intf memory_space = {
        .read = c28x_space_read,
        .write = c28x_space_write,
        .read_hl = c28x_space_read_hl,
        .write_hl = c28x_space_write_hl
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &memory_space);

    static const core_breakpoint_op_intf bp_op = {
        .insert_bp = c28x_insert_bp,
        .delete_bp = c28x_delete_bp,
        .get_bp_numbers = c28x_get_bp_numbers,
        .get_bp_addr_by_id = c28x_get_bp_addr_by_id,
        .get_bp_trigger = c28x_get_trigger,
        .check_bp_trigger = c28x_check_bp_trigger,
        .clear_bp_trigger = c28x_clear_bp_trigger,
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

    f28335_register_attribute(clss);
}
