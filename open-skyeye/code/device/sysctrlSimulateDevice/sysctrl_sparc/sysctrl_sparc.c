/* Copyright (C) 
 * 2011 - Michael.Kang blackfin.kang@gmail.com
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
 * @file sysctrl_sparc.c
 * @brief The implementation of system controller
 * @author Michael.Kang blackfin.kang@gmail.com
 * @version 78.77
 * @date 2011-12-12
 */

#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_core.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h> 
#include <memory_space.h>
#include <skyeye_device.h>
#define DEBUG
#include <skyeye_log.h>

#include "sysctrl_sparc.h"

void send_signal_thread(conf_object_t *opaque)
{
    sparc_sysctrl_device *dev = opaque->obj;
    irq_mesg_t irq_mesg;
    interrupt_signal_t interrupt_signal;

    if(CheckCanReadNum(dev->recv_fifo) == 0)
    {
        return;
    }

    ReadFIFO(dev->recv_fifo, &irq_mesg, 1);
    interrupt_signal.sparc_signal.irq_no = irq_mesg.irq_num;
    interrupt_signal.sparc_signal.level = irq_mesg.level;

    if (dev->signal_iface)
    {
        dev->signal_iface->signal(dev->signal, &interrupt_signal);
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "signal_iface Error.\n");
    }
}

static int sparc_sysctrl_raise_signal(conf_object_t *opaque, int irq_num)
{
    sparc_sysctrl_device *dev = opaque->obj;
    interrupt_signal_t interrupt_signal;

    if(irq_num > 0 && irq_num <= 15)
    {
        PENDREG |= (1 << irq_num);
    }

    if(PENDREG && (~(MASKREG) & (0x1 << irq_num)))
    {
        irq_mesg_t irq_mesg;
        irq_mesg.irq_num = irq_num;
        irq_mesg.level = High_level;

        RW_WRLOCK(dev->lock);
        WriteFIFO(dev->recv_fifo, &irq_mesg, 1);
        RW_UNLOCK(dev->lock);
    }

    return No_exp;
}

static int sparc_sysctrl_lower_signal(conf_object_t *opaque, int clr_reg)
{
    struct sparc_sysctrl_device *dev = opaque->obj;
    interrupt_signal_t interrupt_signal;

    PENDREG &= ~(clr_reg);

    return No_exp;
}

static void timer_callback(conf_object_t *opaque)
{
    struct sparc_sysctrl_device *dev = opaque->obj;
    sysctrl_reg_t* regs = dev->regs;

    if (GPTE && (GPTSL == 0))
    {
        regs->rtccr -= 2000;
        regs->gptcr -= 2000;
    }

    if (regs->gptcr == 0)
    {
        regs->gptcr = dev->gptcr;
        sparc_sysctrl_raise_signal(opaque, GPT_INT);
    }

    return;
}

static exception_t sparc_sysctrl_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
    struct sparc_sysctrl_device *dev = opaque->obj;
    sysctrl_reg_t* regs = dev->regs;
    DBG("Jiachao ######################### In %s, offset=0x%x\n", __FUNCTION__, offset);
    switch(offset)
    {
        case O_SYSCTR:
            *(uint32_t*)buf = regs->sysctr.all;
            break;
        case O_SWRST:
            *(uint32_t*)buf = regs->swrst;
            break;
        case O_PDOWN:
            *(uint32_t*)buf = regs->pdown;
            break;
        case O_MCNFR:
            *(uint32_t*)buf = regs->mcnfr.all;
            break;
        case O_IOCNFR:
            *(uint32_t*)buf = regs->iocnfr.all;
            break;
        case O_WSCNFR:
            *(uint32_t*)buf = regs->wscnfr;
            break;
        case O_APS1BR:
            *(uint32_t*)buf = regs->aps1br;
            break;
        case O_APS1ER:
            *(uint32_t*)buf = regs->aps1er;
            break;
        case O_APS2BR:
            *(uint32_t*)buf = regs->aps2br;
            break;
        case O_APS2ER:
            *(uint32_t*)buf = regs->aps2er;
            break;
        case O_INTSHR:
            *(uint32_t*)buf = regs->intshr.all;
            break;
        case O_INTPDR:
            *(uint32_t*)buf = regs->intpdr.all;
            break;
        case O_INTMKR:
            *(uint32_t*)buf = regs->intmkr;
            break;
        case O_INTCLR:
            *(uint32_t*)buf = regs->intclr;
            break;
        case O_INTFCR:
            *(uint32_t*)buf = regs->intfcr;
            break;
        case O_WDOGTR:
            *(uint32_t*)buf = regs->wdogtr;
            break;
        case O_WDOGST:
            *(uint32_t*)buf = regs->wdogst;
            break;
        case O_RTCCR:
            *(uint32_t*)buf = regs->rtccr;
            break;
        case O_RTCSR:
            *(uint32_t*)buf = regs->rtcsr;
            break;
        case O_GPTCR:
            *(uint32_t*)buf = regs->gptcr;
            break;
        case O_GPTSR:
            *(uint32_t*)buf = regs->gptsr;
            break;
        case O_TIMCTR:
            *(uint32_t*)buf = regs->timctr.all;
            break;
        case O_SYSFSR:
            *(uint32_t*)buf = regs->sysfsr;
            break;
        case O_FAILAR:
            *(uint32_t*)buf = regs->failar;
            break;
        case O_ERRRSR:
            *(uint32_t*)buf = regs->errrsr;
            break;
        case O_TESTCTR:
            *(uint32_t*)buf = regs->testctr.all;
            break;
        case O_GPICNFR:
            *(uint32_t*)buf = regs->gpicnfr;
            break;
        case O_GPIDATR:
            *(uint32_t*)buf = regs->gpidatr;
            break;
        default:
            printf("Can not read the register at 0x%x in sysctrl\n", offset);
            *(uint32_t*)buf = 0;
            return No_exp;
    }

    return No_exp;
}

static exception_t sparc_sysctrl_write(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
    struct sparc_sysctrl_device *dev = opaque->obj;
    sysctrl_reg_t* regs = dev->regs;
    DBG("Jiachao ######################### In %s, offset=0x%x\n", __FUNCTION__, offset);
    uint32_t val = *(uint32_t*)buf;
    switch(offset)
    {
        case O_SYSCTR:
            regs->sysctr.all = val;
            break;
        case O_SWRST:
            regs->swrst = val;
            break;
        case O_PDOWN:
            regs->pdown = val;
            break;
        case O_SYSFSR:
            regs->sysfsr = val;
            break;
        case O_FAILAR:
            regs->failar = val;
            break;
        case O_ERRRSR:
            regs->errrsr = val;
            break;
        case O_TESTCTR:
            regs->testctr.all = val;
            break;
        case O_MCNFR:
            regs->mcnfr.all = val;
            break;
        case O_IOCNFR:
            regs->iocnfr.all = val;
            break;
        case O_WSCNFR:
            regs->wscnfr = val;
            break;
        case O_APS1BR:
            regs->aps1br = val;
            break;
        case O_APS1ER:
            regs->aps1er = val;
            break;
        case O_APS2BR:
            regs->aps2br = val;
            break;
        case O_APS2ER:
            regs->aps2er = val;
            break;
        case O_INTSHR:
            regs->intshr.all = val;
            break;
        case O_INTPDR:
            regs->intpdr.all = val;
            break;
        case O_INTMKR:
            regs->intmkr = val;
            break;
        case O_INTCLR:
            //regs->intclr = val;
            sparc_sysctrl_lower_signal(opaque, val);
            break;
        case O_INTFCR:
            regs->intfcr = val;
            break;
        case O_WDOGTR:
            regs->wdogtr = val;
            break;
        case O_WDOGST:
            regs->wdogst = val;
            break;
        case O_RTCCR:
            regs->rtccr = val;
            break;
        case O_RTCSR:
            regs->rtcsr = val;
            break;
        case O_GPTCR:
            regs->gptcr = val;
            dev->gptcr = regs->gptcr;
            if (dev->handle == -1 && regs->gptcr > 0)
            {
                dev->handle = system_register_timer_handler(opaque, 2000, (time_sched_t)timer_callback, SCHED_US | SCHED_NORMAL);
            }
            break;
        case O_GPTSR:
            regs->gptsr = val;
            break;
        case O_TIMCTR:
            regs->timctr.all = val;
            break;
        case O_GPICNFR:
            regs->gpicnfr = val;
            break;
        case O_GPIDATR:
            regs->gpidatr = val;
            break;
        default:
            printf("Can not write the register at 0x%x in sysctrl\n", offset);
            return Invarg_exp;
    }
    return No_exp;
}


char* sparc_sysctrl_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
    sparc_sysctrl_device *dev = conf_obj->obj;
    return regs_name[id];
}

uint32_t sparc_sysctrl_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id)
{
    sparc_sysctrl_device *dev = conf_obj->obj;
    uint32_t* regs_value = (uint32_t*)(dev->regs) + id;
    return *regs_value;
}

static exception_t sparc_sysctrl_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
    sparc_sysctrl_device *dev = conf_obj->obj;
    sysctrl_reg_t *regs = (dev->regs);
    *((uint32_t*)regs + id) = value;

    return No_exp;
}

static uint32_t sparc_sysctrl_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
    return regs_offset[id];
}

static uint32_t sparc_sysctrl_get_regnum(conf_object_t* conf_obj)
{
    sparc_sysctrl_device *dev = conf_obj->obj;
    sysctrl_reg_t *regs = (dev->regs);
    return  28;
}

static uint32_t sparc_sysctrl_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
    sparc_sysctrl_device *dev = conf_obj->obj;
    sysctrl_reg_t *regs = (dev->regs);
    uint32_t regnum;
    regnum = sizeof(sysctrl_reg_t)/ 4;
    int i;
    for(i = 0; i < regnum; i++)
    {
        if(strcmp(name, regs_name[i]) == 0)
            return i;
    }
    return regnum;
}

static conf_object_t* new_sparc_sysctrl(const char* obj_name)
{
    sparc_sysctrl_device* dev = skyeye_mm_zero(sizeof(sparc_sysctrl_device));
    sysctrl_reg_t* regs =  skyeye_mm_zero(sizeof(sysctrl_reg_t));
    dev->obj = new_conf_object(obj_name, dev);
    /* init sysctrl regs */
    dev->regs = regs;
    dev->timer_sched_id = -1;
    dev->timer_cnt = 0;
    MASKREG = 0xffff;
    dev->recv_fifo = CreateFIFO(2048);
    dev->gptcr = 50000;
    dev->handle = -1;
    RWLOCK_INIT(dev->lock);

    return dev->obj;
}

static exception_t free_sparc_sysctrl(conf_object_t* conf_obj)
{
    sparc_sysctrl_device *dev = conf_obj->obj;
    skyeye_free(dev->obj);
    skyeye_free(dev->regs);
    skyeye_free(dev);
    return No_exp;
}

static exception_t config_sparc_sysctrl(conf_object_t* conf_obj)
{
    system_register_timer_handler(conf_obj, 2, (time_sched_t)send_signal_thread, SCHED_MS|SCHED_NORMAL);

    return No_exp;
}

static void sparc_sysctrl_register_attribute(conf_class_t* clss)
{
    return;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    struct sparc_sysctrl_device *dev = obj->obj;
    dev->signal = obj2;
    dev->signal_iface = (core_signal_intf *)SKY_get_iface(dev->signal, CORE_SIGNAL_INTF_NAME);

    if (dev->signal_iface == NULL)
    {
        if (dev->signal == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", CORE_SIGNAL_INTF_NAME);
        } else
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", CORE_SIGNAL_INTF_NAME, obj2->objname);
        }

        return Not_found_exp;
    }
    return No_exp;
}

static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    struct sparc_sysctrl_device *dev = obj->obj;
    *obj2 = dev->signal;
    *port = NULL;
    return No_exp;
}

void init_sparc_sysctrl(){
    static skyeye_class_t class_data =
    {
        .class_name = "sparc_sysctrl",
        .class_desc = "sparc sysctrl",
        .new_instance = new_sparc_sysctrl,
        .free_instance = free_sparc_sysctrl,
        .config_instance = config_sparc_sysctrl
    };

    conf_class_t *clss = SKY_register_device_class(class_data.class_name, &class_data);
    static const memory_space_intf io_memory =
    {
        .read = sparc_sysctrl_read,
        .write = sparc_sysctrl_write,
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

    static const skyeye_reg_intf reg_intf =
    {
        .get_regvalue_by_id = sparc_sysctrl_get_regvalue_by_id,
        .get_regname_by_id = sparc_sysctrl_get_regname_by_id,
        .set_regvalue_by_id = sparc_sysctrl_set_regvalue_by_id,
        .get_regnum = sparc_sysctrl_get_regnum,
        .get_regid_by_name = sparc_sysctrl_get_regid_by_name,
        .get_regoffset_by_id = sparc_sysctrl_get_regoffset_by_id,
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

    static const general_signal_iface_t signal_iface =
    {
        .raise_signal = sparc_sysctrl_raise_signal,
        .lower_signal = sparc_sysctrl_lower_signal
    };
    SKY_register_iface(clss, GENERAL_SIGNAL_INTF_NAME, &signal_iface);

    static const struct InterfaceDescription ifaces[] =
    {
        (struct InterfaceDescription)
        {
            .name = SKYEYE_REG_INTF,
            .iface = &reg_intf,
        },
        (struct InterfaceDescription)
        {
            .name = MEMORY_SPACE_INTF_NAME,
            .iface = &io_memory,
        },
        (struct InterfaceDescription)
        {
            .name = GENERAL_SIGNAL_INTF_NAME,
            .iface = &signal_iface,
        },

    };
    static const struct ConnectDescription connects[] =
    {
        (struct ConnectDescription )
        {
            .name = CORE_SIGNAL_INTF_NAME,
            .set = core_signal_set,
            .get = core_signal_get,
        },
    };
    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);

    sparc_sysctrl_register_attribute(clss);
}
