/*************************************************************************
  > File Name    : bm3823_uart.c
  > Author       : zhangjianhao
  > Created Time : 2021/06/20 13:51:33
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
#include "bm3823_system_control.h"

void uart_interrupt_handler(conf_object_t *opaque)
{
    bm3823_system_control_device *dev = opaque->obj;

    if (dev->regs.uart_reg.uartctrl1.ti)
    {
        if (dev->signal.iface != NULL)
        {
            dev->signal.iface->raise_signal(dev->signal.obj, IRQ_NUM_UART1);
        }
    }
}

void uart_tick_handler(conf_object_t *opaque)
{
    bm3823_system_control_device *dev = opaque->obj;

    if (dev->regs.uart_reg.uartctrl1.re || dev->regs.uart_reg.uartctrl1.te)
        dev->uart1_scaler --;

    if (dev->regs.uart_reg.uartctrl2.re || dev->regs.uart_reg.uartctrl2.te)
        dev->uart2_scaler --;

    if ( dev->uart1_scaler == 0)
    {
        dev->uart1_scaler = dev->regs.uart_reg.uartscaler1.srv;
    }

    if (dev->uart2_scaler == 0)
    {
        dev->uart2_scaler = dev->regs.uart_reg.uartscaler2.srv;
    }
}

void uart_read_handler(conf_object_t *opaque)
{
    bm3823_system_control_device *dev = opaque->obj;
    exception_t ret = -1;
    unsigned char buf;
    if (dev->term1.intf != NULL)
    {
        if ((ret = (dev->term1.intf->read(dev->term1.obj, &buf, 1)) != No_exp))
        {
            /*Receiver enable*/
            if (dev->regs.uart_reg.uartctrl1.re)
            {
                if (buf == 127)
                    buf = '\b';
                else if(buf == 10 || buf == 13)
                {
                    buf = '\r';
                }
                dev->regs.uart_reg.uartdata1.rtd = buf;
                dev->regs.uart_reg.uartstatus1.dr = 1;
                if (dev->regs.uart_reg.uartctrl1.ri)
                {
                    if(dev->signal.iface != NULL)
                    {
                        dev->signal.iface->raise_signal(dev->signal.obj, IRQ_NUM_UART1);
                    }
                }
            }
        }
    }

    if (dev->term2.intf != NULL)
    {
        if ((ret = (dev->term2.intf->read(dev->term2.obj, &buf, 1)) != No_exp)) {
            /*Receiver enable*/
            if (dev->regs.uart_reg.uartctrl2.re)
            {
                if (buf == 127)
                    buf = '\b';
                else if(buf == 10 || buf == 13)
                {
                    buf = '\r';
                }
                dev->regs.uart_reg.uartdata2.rtd = buf;
                dev->regs.uart_reg.uartstatus2.dr = 1;
                if (dev->regs.uart_reg.uartctrl2.ri)
                {
                    if(dev->signal.iface != NULL)
                    {
                        dev->signal.iface->raise_signal(dev->signal.obj, IRQ_NUM_UART2);
                    }
                }
            }
        }
    }
}

exception_t uart_memory_write(conf_object_t *opaque, generic_address_t offset, uint32_t data, size_t count)
{
    bm3823_system_control_device *dev = opaque->obj;
    switch (offset)
    {
        case UDATA1:
            dev->regs.uart_reg.uartdata1.rtd = (uint8_t)data;
            dev->regs.uart_reg.uartstatus1.ts = 0;
            if (dev->regs.uart_reg.uartctrl1.te)
            {
                if (dev->term1.obj)
                {
                    dev->term1.intf->write(dev->term1.obj, &data, 1);
                }

                if (dev->uart_file.obj)
                {
                    dev->uart_file.intf->write(dev->uart_file.obj, &data, 1);
                }
                /*set Transmitter shift register empty*/
                dev->regs.uart_reg.uartstatus1.ts = 1;
                /*set Transmitter hold register empty*/
                dev->regs.uart_reg.uartstatus1.th = 1;
            }

            break;
        case USTAT1:
            skyeye_log(Warning_log, __FUNCTION__, "uas1 register is  only read!\n");
            break;
        case UCTRL1:
            dev->regs.uart_reg.uartctrl1.all = data;
            if (data & 0x8)
            {
                dev->regs.uart_reg.uartstatus1.th = 1;
            }
            break;
        case USCAL1:
            dev->regs.uart_reg.uartscaler1.all = data;
            break;
        case UDATA2:
            dev->regs.uart_reg.uartdata2.rtd = (uint8_t)data;
            dev->regs.uart_reg.uartstatus2.ts = 0;
            if (dev->regs.uart_reg.uartctrl2.te)
            {
                if (dev->term2.intf)
                {
                    dev->term2.intf->write(dev->term2.obj, &data, 1);
                }

                if (dev->uart_file.intf)
                {
                    dev->uart_file.intf->write(dev->uart_file.obj, &data, 1);
                }
                /*set Transmitter shift register empty*/
                dev->regs.uart_reg.uartstatus2.ts = 1;
                /*set Transmitter hold register empty*/
                dev->regs.uart_reg.uartstatus2.th = 1;
                /*Transmitter interrupt enable*/
                if (dev->regs.uart_reg.uartctrl2.ti)
                {
                    if(dev->signal.iface != NULL)
                    {
                        dev->signal.iface->raise_signal(dev->signal.obj, IRQ_NUM_UART2);
                    }
                }
            }
            break;
        case USTAT2:
            dev->regs.uart_reg.uartstatus2.all = data;
            break;
        case UCTRL2:
            dev->regs.uart_reg.uartctrl2.all = data;
            if (data & 0x8)
            {
                dev->regs.uart_reg.uartstatus2.th = 1;
            }
            break;
        case USCAL2:
            dev->regs.uart_reg.uartscaler2.all = data;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in uart_memory_write\n", offset);
            return Not_found_exp;
    }

    return No_exp;
}

exception_t uart_memory_read(conf_object_t *opaque, generic_address_t offset, uint32_t *val, size_t count)
{
    bm3823_system_control_device *dev = opaque->obj;

    switch (offset)
    {
        case UDATA1:
            if (dev->regs.uart_reg.uartctrl1.re)
            {
                *val = dev->regs.uart_reg.uartdata1.rtd;
                dev->regs.uart_reg.uartstatus1.dr = 0;
            }
            break;
        case USTAT1:
            *val = dev->regs.uart_reg.uartstatus1.all;
            break;
        case UCTRL1:
            *val = dev->regs.uart_reg.uartctrl1.all;
            break;
        case USCAL1:
            *val = dev->regs.uart_reg.uartscaler1.all;
            break;
        case UDATA2:
            if (dev->regs.uart_reg.uartctrl2.re)
            {
                *val = dev->regs.uart_reg.uartdata2.rtd;
                dev->regs.uart_reg.uartstatus2.dr = 0;
            }
            break;
        case USTAT2:
            *val = dev->regs.uart_reg.uartstatus2.all;
            break;
        case UCTRL2:
            *val = dev->regs.uart_reg.uartctrl2.all;
            break;
        case USCAL2:
            *val = dev->regs.uart_reg.uartscaler2.all;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in uart_memory_read\n", offset);
            return Not_found_exp;
    }

    return No_exp;

}
