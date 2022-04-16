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
* @file c55_core.c
* @brief The core of c55
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-09-03
*/
#include "c55_cpu.h"
extern void c55_memory_write(c55_core_t * core, uint32_t addr, uint16_t buffer);
static const char *c5510_regstr[] = {
    "PC_REG", "XSP", "XSSP", "AC0_REG", "AC1_REG", "AC2_REG", "AC3_REG", "XDP",
    "XAR0", "XAR1", "XAR2", "XAR3", "XAR4", "XAR5", "XAR6",
    "XAR7", "T0_REG", "T1_REG", "T2_REG", "T3_REG", "ST0_55", "ST1_55", "ST2_55",
    "ST3_55", "RPTC", "CSR_REG", "BRC0", "BRC1", "RSA0", "REA0", "RSA1", "REA1", "IER0", "IFR0",
    "IER1", "IFR1",
    NULL
};
typedef enum
{
    PC_REG = 0, XSP, XSSP, AC0_REG, AC1_REG, AC2_REG, AC3_REG,
    XDP, XAR0, XAR1, XAR2, XAR3, XAR4, XAR5, XAR6, XAR7, T0_REG, T1_REG,
    T2_REG, T3_REG, ST0_55, ST1_55, ST2_55, ST3_55, RPTC, CSR_REG, BRC0, BRC1,
    RSA0, REA0, RSA1, REA1, IER0, IFR0, IER1, IFR1,
    C5510_REGNUM
} reg_id_t;

static conf_object_t *new_c5510_core(char *obj_name)
{
    c55_core_t *core = skyeye_mm_zero(sizeof (c55_core_t));
    conf_object_t *obj = new_conf_object(obj_name, core);

    core->obj = obj;

    core->c55_memory_intf = skyeye_mm_zero(sizeof (memory_space_intf));
    core->c55_memory_intf->read = c55x_space_read;
    core->c55_memory_intf->write = c55x_space_write;
    core->ivpd = 0xFFFF;
    core->ivph = 0xFFFF;
    core->st0_55 = 0x3800;
    core->st1_55 = 0x2920;
    core->st2_55 = 0x7000;
    core->st3_55 = 0x1300;
    core->vector_baseaddr = 0x100;
    core->init_memory_flag = 0;
    core->exec_enable = 1;

    return core->obj;
}

void free_c5510_core(conf_object_t * dev)
{
    c55_core_t *core = (c55_core_t *) dev->obj;

    skyeye_free(core->obj);
    skyeye_free(core);

}

exception_t reset_c5510_core(conf_object_t * obj, const char *parameter)
{
    c55_core_t *core = (c55_core_t *) obj->obj;

    return No_exp;
}

static uint32 c5510_get_regnum(conf_object_t * obj)
{
    return C5510_REGNUM;
}

static uint32 c5510_get_regvalue_by_id(conf_object_t * obj, uint32 id)
{
    c55_core_t *core = (c55_core_t *) obj->obj;

    if (id == PC_REG)
        return core->pc;
    else if (id >= XAR0 && id <= XAR7)
    {
        return core->xar[id - 8].xar;
    } else if (id == XSP)
        return core->xsp;
    else if (id == XSSP)
        return core->xssp;
    else if (id == AC0_REG)
        return core->ac[0];
    else if (id == AC1_REG)
        return core->ac[1];
    else if (id == AC2_REG)
        return core->ac[2];
    else if (id == AC3_REG)
        return core->ac[3];
    else if (id == XDP)
        return core->xdp;
    else if (id == T0_REG)
        return core->t[0];
    else if (id == T1_REG)
        return core->t[1];
    else if (id == T2_REG)
        return core->t[2];
    else if (id == T3_REG)
        return core->t[3];
    else if (id == ST0_55)
        return core->st0_55;
    else if (id == ST1_55)
        return core->st1_55;
    else if (id == ST2_55)
        return core->st2_55;
    else if (id == ST3_55)
        return core->st3_55;
    else if (id == RPTC)
        return core->rptc;
    else if (id == CSR_REG)
        return core->csr;
    else if (id == BRC0)
        return core->brc0;
    else if (id == BRC1)
        return core->brc1;
    else if (id == RSA0)
        return core->rsa0;
    else if (id == REA0)
        return core->rea0;
    else if (id == RSA1)
        return core->rsa1;
    else if (id == REA1)
        return core->rea1;
    else if (id == IER0)
        return core->ier0;
    else if (id == IFR0)
        return core->ifr0;
    else if (id == IER1)
        return core->ier1;
    else if (id == IFR1)
        return core->ifr1;
    else
        return 0xFFFFFFFF;

    /* something wrong */
}
static exception_t c5510_set_regvalue_by_id(conf_object_t * obj, uint32 value, uint32 id)
{
    c55_core_t *core = (c55_core_t *) obj->obj;

    if (id == PC_REG)
        core->pc = value;
    else if (id >= XAR0 && id <= XAR7)
    {
        core->xar[id - 8].xar = value;
    } else if (id == XSP)
        core->xsp = value;
    else if (id == XSSP)
        core->xssp = value;
    else if (id == AC0_REG)
        core->ac[0] = value;
    else if (id == AC1_REG)
        core->ac[1] = value;
    else if (id == AC2_REG)
        core->ac[2] = value;
    else if (id == AC3_REG)
        core->ac[3] = value;
    else if (id == XDP)
        core->xdp = value;
    else if (id == T0_REG)
        core->t[0] = value;
    else if (id == T1_REG)
        core->t[1] = value;
    else if (id == T2_REG)
        core->t[2] = value;
    else if (id == T3_REG)
        core->t[3] = value;
    else if (id == ST0_55)
        core->st0_55 = value;
    else if (id == ST1_55)
        core->st1_55 = value;
    else if (id == ST2_55)
        core->st2_55 = value;
    else if (id == ST3_55)
        core->st3_55 = value;
    else if (id == RPTC)
        core->rptc = value;
    else if (id == CSR_REG)
        core->csr = value;
    else if (id == BRC0)
        core->brc0 = value;
    else if (id == BRC1)
        core->brc1 = value;
    else if (id == RSA0)
        core->rsa0 = value;
    else if (id == REA0)
        core->rea0 = value;
    else if (id == RSA1)
        core->rsa1 = value;
    else if (id == REA1)
        core->rea1 = value;
    else if (id == IER0)
        core->ier0 = value;
    else if (id == IFR0)
        core->ifr0 = value;
    else if (id == IER1)
        core->ier1 = value;
    else if (id == IFR1)
        core->ifr1 = value;
    else
        return 0xFFFFFFFF;
    return No_exp;
}
static uint32 c5510_get_regid_by_name(conf_object_t * opaque, char *name)
{
    int i;

    for (i = 0; i < C5510_REGNUM; i++)
    {
        if (strcmp(name, c5510_regstr[i]) == 0)
            return i;
    }
    return 0;
}
static const char *c5510_get_regname_by_id(conf_object_t * obj, uint32 id)
{
    return c5510_regstr[id];
}

#define REG_NUM 8
static attr_value_t get_attr_xar_reg(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    c55_core_t *core = (c55_core_t *) conf_obj->obj;
    attr_value_t value = SKY_alloc_attr_list(REG_NUM);
    attr_value_t item;
    uint32_t i;

    for (i = 0; i < REG_NUM; i++)
    {
        uint32_t data = core->xar[i].xar;

        item = SKY_make_attr_uinteger(data);
        SKY_attr_list_set_item(&value, i, item);
    }
    return value;
}
static exception_t set_attr_xar_reg(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx)
{
    c55_core_t *core = (c55_core_t *) conf_obj->obj;
    uint32_t i = 0, data = 0;
    attr_value_t item;

    for (i = 0; i < REG_NUM; i++)
    {
        item = SKY_attr_list_item(*value, i);
        data = SKY_attr_uinteger(item);
        core->xar[i].xar = data;
    }
    return No_exp;
}

DEF_LIST_UINTEGER_ATTR_FUNC(c55_core_t, ac, 4);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, xcdp);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, xdp);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, xsp);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, xssp);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, bk03);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, bk47);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, bkc);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, brs1);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, brc0);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, brc1);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, bsa01);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, bsa23);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, bsa45);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, bsa67);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, bsac);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, cfct);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, csr);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, dbier);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, dbstat);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, ier);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, ifr);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, ivpd);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, ivph);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, pc);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, pdp);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, rea0);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, rea1);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, reta);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, rptc);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, rsa0);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, rsa1);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, rptb_enable);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, st0_55);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, st1_55);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, st2_55);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, st3_55);
DEF_LIST_UINTEGER_ATTR_FUNC(c55_core_t, t, 4);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, trn0);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, trn1);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, pce1);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, stop_step);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, insn_num);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, insn_length);
DEF_UINTEGER_ATTR_FUNC(c55_core_t, intr_flag);

static void c5510_register_attribute(conf_class_t * clss)
{
    SKY_register_attribute(clss, "XAR_REG", get_attr_xar_reg, NULL, set_attr_xar_reg, NULL, SKY_Attr_Optional,
                           "uinteger", "xar register value of processor core");
    DEF_ATTR(ac, list, NULL);
    DEF_ATTR(xcdp, uinteger, NULL);
    DEF_ATTR(xdp, uinteger, NULL);
    DEF_ATTR(xsp, uinteger, NULL);
    DEF_ATTR(xssp, uinteger, NULL);
    DEF_ATTR(bk03, uinteger, NULL);
    DEF_ATTR(bk47, uinteger, NULL);
    DEF_ATTR(bkc, uinteger, NULL);
    DEF_ATTR(brs1, uinteger, NULL);
    DEF_ATTR(brc0, uinteger, NULL);
    DEF_ATTR(brc1, uinteger, NULL);
    DEF_ATTR(bsa01, uinteger, NULL);
    DEF_ATTR(bsa23, uinteger, NULL);
    DEF_ATTR(bsa45, uinteger, NULL);
    DEF_ATTR(bsa67, uinteger, NULL);
    DEF_ATTR(bsac, uinteger, NULL);
    DEF_ATTR(cfct, uinteger, NULL);
    DEF_ATTR(csr, uinteger, NULL);
    DEF_ATTR(dbier, uinteger, NULL);
    DEF_ATTR(dbstat, uinteger, NULL);
    DEF_ATTR(ier, uinteger, NULL);
    DEF_ATTR(ifr, uinteger, NULL);
    DEF_ATTR(ivpd, uinteger, NULL);
    DEF_ATTR(ivph, uinteger, NULL);
    DEF_ATTR(pc, uinteger, NULL);
    DEF_ATTR(pdp, uinteger, NULL);
    DEF_ATTR(rea0, uinteger, NULL);
    DEF_ATTR(rea1, uinteger, NULL);
    DEF_ATTR(reta, uinteger, NULL);
    DEF_ATTR(rptc, uinteger, NULL);
    DEF_ATTR(rsa0, uinteger, NULL);
    DEF_ATTR(rsa1, uinteger, NULL);
    DEF_ATTR(rptb_enable, uinteger, NULL);
    DEF_ATTR(st0_55, uinteger, NULL);
    DEF_ATTR(st1_55, uinteger, NULL);
    DEF_ATTR(st2_55, uinteger, NULL);
    DEF_ATTR(st3_55, uinteger, NULL);
    DEF_ATTR(t, list, NULL);
    DEF_ATTR(trn0, uinteger, NULL);
    DEF_ATTR(trn1, uinteger, NULL);
    DEF_ATTR(pce1, uinteger, NULL);
    DEF_ATTR(stop_step, uinteger, NULL);
    DEF_ATTR(insn_num, uinteger, NULL);
    DEF_ATTR(insn_length, uinteger, NULL);
    DEF_ATTR(intr_flag, uinteger, NULL);
    return;
}

void init_c5510_core()
{
    static skyeye_class_t class_data = {
        .class_name = "c5510_core",
        .class_desc = "TI c55 Floating Point processor core",
        .new_instance = new_c5510_core,
        .free_instance = free_c5510_core,
        .reset_instance = reset_c5510_core,
        .get_attr = NULL,
        .set_attr = NULL
    };

    conf_class_t *clss = SKY_register_cpu_class(class_data.class_name, &class_data);
    static const core_info_intf core_info = {
        .set_pc = c55x_set_pc,
        .get_pc = c55x_get_pc,
        .get_steps = c55x_get_steps,
        .get_endian = c55x_get_endian,
        .get_alignment = c55x_get_alignment,
        .reset = c55x_reset,
        .get_address_width = c55x_get_address_width,
        .disassemble = c55x_disassemble,
        .get_architecture = c55x_get_architecture,
        .get_cpu_exception_num = c55x_get_cpu_exception_num,
        .set_cpu_exception_by_id = c55x_set_cpu_exception_by_id,
        .get_cpu_exception_name_by_id = c55x_get_cpu_exception_name_by_id
    };
    SKY_register_iface(clss, CORE_INFO_INTF_NAME, &core_info);

    static const skyeye_reg_intf reg = {
        .get_regvalue_by_id = c5510_get_regvalue_by_id,
        .get_regname_by_id = c5510_get_regname_by_id,
        .set_regvalue_by_id = c5510_set_regvalue_by_id,
        .get_regid_by_name = c5510_get_regid_by_name,
        .get_regnum = c5510_get_regnum,
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &reg);

    static const core_exec_intf core_exec = {
        .run = per_cpu_step,
        .stepi = c55x_stepi,
        .stop = NULL,
    };
    SKY_register_iface(clss, CORE_EXEC_INTF_NAME, &core_exec);

    static const core_signal_intf core_signal = {
        .signal = c55x_signal,
    };
    SKY_register_iface(clss, CORE_SIGNAL_INTF_NAME, &core_signal);

    static const memory_space_intf memory_space = {
        .read = c55x_space_read,
        .write = c55x_space_write
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &memory_space);

    static const core_breakpoint_op_intf bp_op = {
        .insert_bp = c55x_insert_bp,
        .delete_bp = c55x_delete_bp,
        .get_bp_numbers = c55x_get_bp_numbers,
        .get_bp_addr_by_id = c55x_get_bp_addr_by_id,
        .get_bp_trigger = c55x_get_trigger,
        .check_bp_trigger = c55x_check_bp_trigger,
        .clear_bp_trigger = c55x_clear_bp_trigger,
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

    c5510_register_attribute(clss);
}
