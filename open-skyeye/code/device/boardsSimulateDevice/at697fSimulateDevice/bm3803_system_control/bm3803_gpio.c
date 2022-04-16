/*************************************************************************
  > File Name    : bm3803_gpio.c
  > Author       : jiachao
  > Mail         : jiachao@163.com
  > Created Time : 2019/12/24 13:50:24
 ************************************************************************/
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_signal.h>
#define DEBUG
#include <skyeye_log.h>
#include "bm3803_system_control.h"

void ioit1_interrupt_check(conf_object_t *opaque, int i, uint32_t data)
{
    bm3803_system_control_device *dev = opaque->obj;
    int io_port;

    io_port = IT1ISEL_CHECK(i);
    /*edge triggered mode*/
    if (IT1LE_CHECK(i))
    {
        /*active high*/
        if (IT1PL_CHECK(i))
        {
            if ((data >> io_port) & 0x1)
            {
                if (((dev->regs.gpio_reg.iodat.all >> io_port) & 0x1) == 0)
                {
                    /*gpio data from 0 to 1 general interrupt*/
                    if(dev->signal.iface != NULL)
                    {
                        dev->signal.iface->raise_signal(dev->signal.obj, IO0 + i);
                    }
                    else
                    {
                        interrupt_request_handle(opaque, IO0 + i);
                    }
                }
            }
        } else
        {
            /*active lower*/
            if (((data >> io_port) & 0x1) == 0)
            {
                if ((dev->regs.gpio_reg.iodat.all >> io_port) & 0x1)
                {
                    /*gpio data from 1 to 0 general interrupt*/
                    if(dev->signal.iface != NULL)
                    {
                        dev->signal.iface->raise_signal(dev->signal.obj, IO0 + i);
                    }
                    else
                    {
                        interrupt_request_handle(opaque, IO0 + i);
                    }
                }
            }
        }
    } else
    {
        /*level triggered mode*/
        /*high level*/
        if (IT1PL_CHECK(i))
        {
            if ((data >> io_port) & 0x1)
            {
                if (((dev->regs.gpio_reg.iodat.all >> io_port) & 0x1) == 0)
                {
                    /*gpio data from 0 to 1 general interrupt*/
                    if(dev->signal.iface != NULL)
                    {
                        dev->signal.iface->raise_signal(dev->signal.obj, IO0 + i);
                    }
                    else
                    {
                        interrupt_request_handle(opaque, IO0 + i);
                    }	
                }
            }
        }
    }
}

void ioit2_interrupt_check(conf_object_t *opaque, int i, uint32_t data)
{
    bm3803_system_control_device *dev = opaque->obj;
    int io_port, isTrigger = 0, irq_num;

    io_port = IT2ISEL_CHECK(i);
    /*edge triggered mode*/
    if (IT2LE_CHECK(i))
    {
        /*active high*/
        if (IT2PL_CHECK(i))
        {
            if ((data >> io_port) & 0x1)
            {
                if (((dev->regs.gpio_reg.iodat.all >> io_port) & 0x1) == 0)
                {
                    /*gpio data from 0 to 1 general interrupt*/
                    isTrigger = 1;
                }
            }
        } else
        {
            /*active lower*/
            if (((data >> io_port) & 0x1) == 0)
            {
                if ((dev->regs.gpio_reg.iodat.all >> io_port) & 0x1)
                {
                    /*gpio data from 1 to 0 general interrupt*/
                    isTrigger = 1;
                }
            }
        }
    } else
    {
        /*level triggered mode*/
        /*high level*/
        if (IT2PL_CHECK(i))
        {
            if ((data >> io_port) & 0x1)
            {
                if (((dev->regs.gpio_reg.iodat.all >> io_port) & 0x1) == 0)
                {
                    /*gpio data from 0 to 1 general interrupt*/
                    isTrigger = 1;
                }
            }
        }
    }

    if (isTrigger)
    {
        switch (i)
        {
            case 0:
                irq_num = IO4;
                break;
            case 1:
                irq_num = IO5;
                break;
            case 2:
                irq_num = IO6;
                break;
            case 3:
                irq_num = IO7;
                break;
            default:
                printf("Error IO NUM = %d\n", i);
                break;
        }
        if(dev->signal.iface != NULL)
        {
            dev->signal.iface->raise_signal(dev->signal.obj, irq_num);
        }
        else
        {
            interrupt_request_handle(opaque, irq_num);
        }	
    }
}

void check_external_interrupt(conf_object_t *opaque, uint32_t data)
{
    bm3803_system_control_device *dev = opaque->obj;
    int i, io_port;

    for (i = 0; i < 8; i++)
    {
        if (i < 4)
        {
            if (IT1EN_CHECK(i))
            {
                ioit1_interrupt_check(opaque, i, data);
            }
        } else
        {
            if (IT2EN_CHECK(i - 4))
            {
                ioit2_interrupt_check(opaque, i - 4, data);
            }
        }
    }
}

exception_t gpio_memory_write(conf_object_t *opaque, generic_address_t offset, uint32_t data, size_t count)
{
    bm3803_system_control_device *dev = opaque->obj;

    switch (offset)
    {
        case IODAT:
            dev->regs.gpio_reg.iodat.all = data;
            check_external_interrupt(opaque, data);
            break;
        case IODIR:
            dev->regs.gpio_reg.iodir.all = data;
            break;
        case IOIT1:
            dev->regs.gpio_reg.ioit1.all = data;
            break;
        case IOIT2:
            dev->regs.gpio_reg.ioit2.all = data;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in gpio_memory_write\n", offset);
            return Not_found_exp;
    }

    return No_exp;
}

exception_t gpio_memory_read(conf_object_t *opaque, generic_address_t offset, uint32_t *val, size_t count)
{
    bm3803_system_control_device *dev = opaque->obj;

    switch (offset)
    {
        case IODAT:
            *val = dev->regs.gpio_reg.iodat.all;
            break;
        case IODIR:
            *val = dev->regs.gpio_reg.iodir.all;
            break;
        case IOIT1:
            *val = dev->regs.gpio_reg.ioit1.all;
            break;
        case IOIT2:
            *val = dev->regs.gpio_reg.ioit2.all;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in gpio_memory_read\n", offset);
            return Not_found_exp;
    }

    return No_exp;
}
