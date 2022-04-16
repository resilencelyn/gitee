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
 * @file dmc_sparc.c
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
#define DEBUG
#include <skyeye_log.h>

#include "dmc_fifo.h"
#include "dmc_sparc.h"

static void dmc_receive(conf_object_t *obj, void *buf)
{
    dmc_sparc_device *dev = obj->obj;
    WriteFIFO(dev->fifo, (char *)buf, 1);

    return ;
}

static void dmc_raise_signal(conf_object_t *obj, int irq_number)
{
    dmc_sparc_device *dev = obj->obj;
    if (dev->sparc_signal.signal_iface)
    {
        dev->sparc_signal.signal_iface->raise_signal(dev->sparc_signal.signal, irq_number);
    }

}
static exception_t dmc_sparc_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
    struct dmc_sparc_device *dev = opaque->obj;
    dmc_sparc_reg_t* regs = dev->regs;
    switch(offset)
    {
        case 0x20:
            *(uint32_t *)buf = regs->read_status_reg;
            break;
        case 0x24:
            ReadFIFO(dev->fifo, buf, 1);
            break;
        default:
            printf("Can not read the register at 0x%x in dmc_sparc\n", offset);
            return Invarg_exp;
    }
    return No_exp;
}

static exception_t dmc_sparc_write(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
    struct dmc_sparc_device *dev = opaque->obj;
    dmc_sparc_reg_t* regs = dev->regs;
    uint32_t val = *(uint32_t*)buf;

    switch(offset)
    {
        case 0x0:
            regs->reset_reg = val;
            break;
        case 0x4:
            regs->write_fifo_reg = val;
            if(dev->dmc_linker_iface)
            {
                dev->dmc_linker_iface->send_data(dev->dmc_linker, opaque, buf);
            }
            break;
        case 0x8:
            regs->status_reg = val;
            if(dev->dmc_linker_iface)
            {
                dev->dmc_linker_iface->raise_signal(dev->dmc_linker, opaque, dev->irq_number);
            }
            break;
        case 0xc:
            regs->clear_status_reg = val;
            break;
        default:
            printf("Can not write the register at 0x%x in dmc_sparc\n", offset);
            return Invarg_exp;
    }
    return No_exp;

}

static char* dmc_sparc_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
    return regs_name[id];
}

static uint32_t dmc_sparc_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
    dmc_sparc_device *dev = conf_obj->obj;
    dmc_sparc_reg_t* regs = dev->regs;
    uint32_t* regs_value = (uint32_t*)regs + id;
    return *regs_value;
}

static exception_t dmc_sparc_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
    dmc_sparc_device *dev = conf_obj->obj;
    dmc_sparc_reg_t* regs = dev->regs;
    *((uint32_t*)regs + id) = value;

    return No_exp;
}

static uint32_t dmc_sparc_get_regnum(conf_object_t* conf_obj)
{
    dmc_sparc_device *dev = conf_obj->obj;
    dmc_sparc_reg_t* regs = dev->regs;
    return  sizeof(dmc_sparc_reg_t) / 4;
}

static uint32_t dmc_sparc_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
    dmc_sparc_device *dev = conf_obj->obj;
    dmc_sparc_reg_t* regs = dev->regs;
    uint32_t regnum;
    regnum = sizeof(dmc_sparc_reg_t) / 4;
    int i;
    for(i = 0; i < regnum; i++)
    {
        if(strcmp(name, regs_name[i]) == 0)
            return i;
    }

    return regnum;
}

static exception_t dmc_linker_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    dmc_sparc_device *dev =  obj->obj;
    dev->dmc_linker = obj2;
    dev->dmc_linker_iface = (dmc_linker_intf *)SKY_get_iface(dev->dmc_linker, DMC_LINKER_INTF);

    if (dev->dmc_linker_iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get 'dmc_linker_intf' interface from %s fail.", obj2->objname);
        return Not_found_exp;
    }
    return No_exp;

}

static exception_t dmc_linker_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    dmc_sparc_device *dev =  obj->obj;
    *obj2 = dev->dmc_linker;
    *port = NULL;
    return No_exp;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    dmc_sparc_device *dev = obj->obj;
    dev->sparc_signal.signal = obj2;
    dev->sparc_signal.signal_iface = (general_signal_intf *)SKY_get_iface(dev->sparc_signal.signal, GENERAL_SIGNAL_INTF_NAME);

    if (dev->sparc_signal.signal_iface == NULL)
    {
        if (dev->sparc_signal.signal == NULL)
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
        else
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);
    }
    return No_exp;
}

static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    return No_exp;
}

static attr_value_t get_attr_irq_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    dmc_sparc_device* dev = (dmc_sparc_device*)(conf_obj->obj);
    attr_value_t irq_number = SKY_make_attr_uinteger(dev->irq_number);
    return irq_number;
}

static exception_t set_attr_irq_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    dmc_sparc_device* dev = (dmc_sparc_device*)(conf_obj->obj);
    dev->irq_number = SKY_attr_uinteger(*value);
    return No_exp;
}

static conf_object_t* new_dmc_sparc(const char* obj_name)
{
    dmc_sparc_device* dev = skyeye_mm_zero(sizeof(dmc_sparc_device));
    dmc_sparc_reg_t* regs =  skyeye_mm_zero(sizeof(dmc_sparc_reg_t));
    dev->obj = new_conf_object(obj_name, dev);
    /* init dmc_sparc regs */
    dev->regs = regs;
    dev->fifo = CreateFIFO(2048);

    return dev->obj;
}

static exception_t free_dmc_sparc(conf_object_t* conf_obj)
{
    dmc_sparc_device* dev = conf_obj->obj;
    skyeye_free(dev->regs);
    FreeFIFO(dev->fifo);
    skyeye_free(dev);

    return No_exp;
}

static void dmc_register_attribute(conf_class_t* clss)
{
    SKY_register_attribute(clss, "irq_number", get_attr_irq_number, NULL, set_attr_irq_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of dmc_sparc");
    return;
}

void init_dmc_sparc()
{
    static skyeye_class_t class_data =
    {
        .class_name = "sparc_dmc",
        .class_desc = "sparc_dmc",
        .new_instance = new_dmc_sparc,
        .free_instance = free_dmc_sparc,
        .get_attr = NULL,
        .set_attr = NULL,
        .module_type = SKYML_OFFCHIP,
    };

    conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

    static const memory_space_intf io_memory =
    {
        .read = dmc_sparc_read,
        .write = dmc_sparc_write,
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

    static const skyeye_reg_intf reg_intf =
    {
        .get_regvalue_by_id = dmc_sparc_get_regvalue_by_id,
        .get_regname_by_id = dmc_sparc_get_regname_by_id,
        .set_regvalue_by_id = dmc_sparc_set_regvalue_by_id,
        .get_regnum = dmc_sparc_get_regnum,
        .get_regid_by_name = dmc_sparc_get_regid_by_name,
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

    static const dmc_dev_intf dmc_intf =
    {
        .receive_data = dmc_receive,
        .raise_signal = dmc_raise_signal,
    };
    SKY_register_iface(clss, DMC_DEV_INTF, &dmc_intf);

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
            .name = DMC_DEV_INTF,
            .iface = &dmc_intf,
        },
    };
    static const struct ConnectDescription connects[] =
    {
        (struct ConnectDescription)
        {
            .name = DMC_LINKER_INTF,
            .set = dmc_linker_set,
            .get = dmc_linker_get,
        },
        (struct ConnectDescription)
        {
            .name = GENERAL_SIGNAL_INTF_NAME,
            .set = general_signal_set,
            .get = general_signal_get,
        },
    };

    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);

    dmc_register_attribute(clss);
}
