/* Copyright (C) 
 * 
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
 * @file swt_sparc.c
 * @brief The implementation of system controller
 * @author 
 * @version 78.77
 */

#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h> 
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_core.h>
#include <skyeye_signal.h>
#include <skyeye_uart_ops.h>
#include <skyeye_dev_bus_intf.h>
#define DEBUG
#include <skyeye_log.h>

#include "swt_sparc.h"

void check_power_st(swt_sparc_device *dev, char* recv_buf)
{
    swt_sparc_reg_t* regs = dev->regs;
    uint8_t lss1_xb, lss1_yb, lss1_zb, lss2_xb, lss2_yb, lss2_zb;
    uint8_t Ires, fog1, fog2, st3, st2, st1, Rcws, Rcwz, Rcwy, Rcwx;

    lss1_xb = recv_buf[0];
    lss1_yb = recv_buf[1];
    lss1_zb = recv_buf[2];
    lss2_xb = recv_buf[3];
    lss2_yb = recv_buf[4];
    lss2_zb = recv_buf[5];

    uint8_t lss_data = lss1_xb | (lss1_yb<<1) | (lss1_zb<<2) | (lss2_xb<<3) | (lss2_yb<<4) | (lss2_zb<<5);

    Rcwx = recv_buf[6];
    Rcwy = recv_buf[7];
    Rcwz = recv_buf[8];
    Rcws = recv_buf[9] & 0x1;
    st1 = recv_buf[10] & 0x1;
    st2 = recv_buf[11] & 0x1;
    st3 = recv_buf[12] & 0x1;
    fog1 = recv_buf[13];
    fog2 = recv_buf[14];
    Ires = recv_buf[15] & 0x1;

    uint8_t power_reg1 = (st3<<7) | (st2<<6) | (st1<<5) | (Rcws<<4) | (Rcwz<<3) | (Rcwy<<2) | (Rcwx<<1);
    uint8_t power_reg2 = (Ires<<6) | (fog2<<5) | (fog1<<4);

    regs->swt_reg_2 = power_reg1 & 0xFF;
    regs->swt_reg_4 = lss_data | (power_reg2 << 8);

    return;
}

static int io_linker_recv_data(conf_object_t *opaque, void* buf, int length, int io_id)
{
    swt_sparc_device *dev = opaque->obj;
    swt_sparc_reg_t* regs = dev->regs;
    char recv_buf[BUFF_SIZE] = {0};
    memcpy(recv_buf, buf, length);
    check_power_st(dev, recv_buf);

    return 0;
}

static exception_t swt_sparc_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
    struct swt_sparc_device *dev = opaque->obj;
    swt_sparc_reg_t* regs = dev->regs;

    switch(offset)
    {
        case SWT_REG_1:
            *(uint32_t *)buf = regs->swt_reg_1;
            break;
        case SWT_REG_2:
            *(uint32_t *)buf = regs->swt_reg_2;
            break;
        case SWT_REG_3:
            *(uint32_t *)buf = regs->swt_reg_3;
            break;
        case SWT_REG_4:
            *(uint32_t *)buf = regs->swt_reg_4;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in swt_sparc\n", offset);
            return Invarg_exp;
    }

    return No_exp;
}

static exception_t swt_sparc_write(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
    struct swt_sparc_device *dev = opaque->obj;
    swt_sparc_reg_t* regs = dev->regs;
    uint32_t val = *(uint32_t*)buf;

    switch(offset)
    {
        case SWT_REG_1:
            regs->swt_reg_1 = val;
            break;
        case SWT_REG_2:
            regs->swt_reg_2 = val;
            break;
        case SWT_REG_3:
            regs->swt_reg_3 = val;
            break;
        case SWT_REG_4:
            regs->swt_reg_4 = val;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in swt_sparc\n", offset);
            return Invarg_exp;
    }
    return No_exp;
}

static char* swt_sparc_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
    return regs_name[id];
}

static uint32_t swt_sparc_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id)
{
    swt_sparc_device *dev = conf_obj->obj;
    swt_sparc_reg_t* regs = dev->regs;
    uint32_t* regs_value = (uint32_t*)regs + id;
    return *regs_value;
}

static exception_t swt_sparc_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
    swt_sparc_device *dev = conf_obj->obj;
    swt_sparc_reg_t* regs = dev->regs;
    *((uint32_t*)regs + id) = value;

    return No_exp;
}

static uint32_t swt_sparc_get_regnum(conf_object_t* conf_obj)
{
    swt_sparc_device *dev = conf_obj->obj;
    swt_sparc_reg_t* regs = dev->regs;
    return  sizeof(swt_sparc_reg_t) / 4;
}

static uint32_t swt_sparc_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
    swt_sparc_device *dev = conf_obj->obj;
    swt_sparc_reg_t* regs = dev->regs;
    uint32_t regnum;
    regnum = sizeof(swt_sparc_reg_t) / 4;
    int i;
    for(i = 0; i < regnum; i++)
    {
        if(strcmp(name, regs_name[i]) == 0)
            return i;
    }
    return regnum;
}

static conf_object_t* new_swt_sparc(const char* obj_name)
{
    swt_sparc_device* dev = skyeye_mm_zero(sizeof(swt_sparc_device));
    swt_sparc_reg_t* regs =  skyeye_mm_zero(sizeof(swt_sparc_reg_t));
    dev->obj = new_conf_object(obj_name, dev);
    /* init swt_sparc regs */
    dev->regs = regs;

    return dev->obj;
}

static exception_t free_swt_sparc(conf_object_t* conf_obj)
{
    swt_sparc_device *dev = conf_obj->obj;
    skyeye_free(dev->regs);
    skyeye_free(dev->obj);
    skyeye_free(dev);
    return No_exp;
}

static exception_t io_linker_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    swt_sparc_device* dev = (swt_sparc_device*)(obj->obj);
    dev->io_linker = obj2;
    dev->io_linker_iface = (io_linker_intf*)SKY_get_iface(dev->io_linker, IO_LINKER_INTF);

    if (dev->io_linker_iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get 'io_linker_iface' interface from %s fail.", obj2->objname);
    }
    return No_exp;

}

static exception_t io_linker_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    swt_sparc_device* dev = (swt_sparc_device*)(obj->obj);
    *obj2 = dev->io_linker;
    *port = NULL;
    return No_exp;
}

static swt_sparc_register_attribute(conf_class_t* clss)
{
}

void init_swt_sparc()
{
    static skyeye_class_t class_data =
    {
        .class_name = "sparc_swt",
        .class_desc = "sparc_swt",
        .new_instance = new_swt_sparc,
        .free_instance = free_swt_sparc
    };

    conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
    static const memory_space_intf io_memory =
    {
        .read = swt_sparc_read,
        .write = swt_sparc_write,
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

    static const skyeye_reg_intf reg_intf =
    {
        .get_regvalue_by_id = swt_sparc_get_regvalue_by_id,
        .get_regname_by_id = swt_sparc_get_regname_by_id,
        .set_regvalue_by_id = swt_sparc_set_regvalue_by_id,
        .get_regnum = swt_sparc_get_regnum,
        .get_regid_by_name = swt_sparc_get_regid_by_name,
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

    static const io_dev_intf io_dev_interface =
    {
        .receive_data = io_linker_recv_data,
    };
    SKY_register_iface(clss, IO_DEV_INTF, &io_dev_interface);

    static const struct InterfaceDescription ifaces[] =
    {
        (struct InterfaceDescription)
        {
            .name = MEMORY_SPACE_INTF_NAME,
            .iface = &io_memory,
        },
        (struct InterfaceDescription)
        {
            .name = SKYEYE_REG_INTF,
            .iface = &reg_intf,
        },
        (struct InterfaceDescription)
        {
            .name = IO_DEV_INTF,
            .iface = &io_dev_interface,
        }
    };
    static const struct ConnectDescription connects[] =
    {
        (struct ConnectDescription ){
            .name = IO_LINKER_INTF,
            .set = io_linker_set,
            .get = io_linker_get,
        }
    };
    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);

    swt_sparc_register_attribute(clss);
}
