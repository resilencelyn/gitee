/*************************************************************************
  > File Name    : bm3803_uart.c
  > Author       : jiachao
  > Mail         : jiachao@163.com
  > Created Time : 2019/12/24 13:51:33
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

void uart_interrupt_handler(conf_object_t *opaque)
{
    bm3803_system_control_device *dev = opaque->obj;

    if (dev->regs.uart_reg.uac1.ti)
    {
        if(dev->signal.iface != NULL)
        {
            dev->signal.iface->raise_signal(dev->signal.obj, UART1);
        }
        else
        {
            interrupt_request_handle(opaque, UART1);
        }
    }
}

void uart_tick_handler(conf_object_t *opaque)
{
    bm3803_system_control_device *dev = opaque->obj;

    if (dev->regs.uart_reg.uac1.re || dev->regs.uart_reg.uac1.te)
        dev->uart1_scaler --;

    if (dev->regs.uart_reg.uac2.re || dev->regs.uart_reg.uac2.te)
        dev->uart2_scaler --;

    if (dev->uart1_scaler == 0)
    {
        dev->uart1_scaler = dev->regs.uart_reg.uasca1.rv;
    }

    if (dev->uart2_scaler == 0)
    {
        dev->uart2_scaler = dev->regs.uart_reg.uasca2.rv;
    }
}

void uart_read_handler(conf_object_t *opaque)
{
    bm3803_system_control_device *dev = opaque->obj;
    exception_t ret = -1;
    unsigned char buf;
    if (dev->term1.intf != NULL)
    {
        if ((ret = (dev->term1.intf->read(dev->term1.obj, &buf, 1)) != No_exp))
        {
            /*Receiver enable*/
            if (dev->regs.uart_reg.uac1.re)
            {
                if (buf == 127)
                    buf = '\b';
                else if(buf == 10 || buf == 13)
                {
                    buf = '\r';
                }
                dev->regs.uart_reg.uad1.rtd = buf;
                dev->regs.uart_reg.uas1.dr = 1;
                if (dev->regs.uart_reg.uac1.ri)
                {
                    if(dev->signal.iface != NULL)
                    {
                        dev->signal.iface->raise_signal(dev->signal.obj, UART1);
                    }
                    else
                    {
                        interrupt_request_handle(opaque, UART1);
                    }

                }
            }
        }
    }

    if (dev->term2.intf != NULL)
    {
        if ((ret = (dev->term2.intf->read(dev->term2.obj, &buf, 1)) != No_exp)) {
            /*Receiver enable*/
            if (dev->regs.uart_reg.uac2.re)
            {
                if (buf == 127)
                    buf = '\b';
                else if(buf == 10 || buf == 13)
                {
                    buf = '\r';
                }
                dev->regs.uart_reg.uad2.rtd = buf;
                dev->regs.uart_reg.uas2.dr = 1;
                //if (dev->regs.uart_reg.uac2.ri)
                //	interrupt_request_handle(opaque, UART2);
            }
        }
    }
}

exception_t uart_memory_write(conf_object_t *opaque, generic_address_t offset, uint32_t data, size_t count)
{
    bm3803_system_control_device *dev = opaque->obj;
    switch (offset)
    {
        case UAD1:
            dev->regs.uart_reg.uad1.rtd = (uint8_t)data;
            //printf("####%d####\n",dev->regs.uart_reg.uad1.rtd);
            dev->regs.uart_reg.uas1.ts = 0;
            if (dev->regs.uart_reg.uac1.te)
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
                dev->regs.uart_reg.uas1.ts = 1;
                /*set Transmitter hold register empty*/
                dev->regs.uart_reg.uas1.th = 1;
                /*Transmitter interrupt enable*/
                if (dev->regs.uart_reg.uac1.ti)
                {
                    //interrupt_request_handle(opaque, UART1);
                }
            }

            break;
        case UAS1:
            //dev->regs.uart_reg.uas1.all = data;
            skyeye_log(Warning_log, __FUNCTION__, "uas1 register is  only read!\n");
            break;
        case UAC1:
            dev->regs.uart_reg.uac1.all = data;
            if (data & 0x8)
            {
                dev->regs.uart_reg.uas1.th = 1;
                //interrupt_request_handle(opaque, UART1);
            }
            break;
        case UASCA1:
            dev->regs.uart_reg.uasca1.all = data;
            break;
        case UAD2:
            dev->regs.uart_reg.uad2.rtd = (uint8_t)data;
            dev->regs.uart_reg.uas2.ts = 0;
            if (dev->regs.uart_reg.uac2.te)
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
                dev->regs.uart_reg.uas2.ts = 1;
                /*set Transmitter hold register empty*/
                dev->regs.uart_reg.uas2.th = 1;
                /*Transmitter interrupt enable*/
                if (dev->regs.uart_reg.uac2.ti)
                {
                    if(dev->signal.iface != NULL)
                    {
                        dev->signal.iface->raise_signal(dev->signal.obj, UART2);
                    }
                    else
                    {
                        interrupt_request_handle(opaque, UART2);
                    }
                }
            }
            break;
        case UAS2:
            dev->regs.uart_reg.uas2.all = data;
            break;
        case UAC2:
            dev->regs.uart_reg.uac2.all = data;
            if (data & 0x8)
            {
                dev->regs.uart_reg.uas2.th = 1;
                //interrupt_request_handle(opaque, UART2);
            }

            break;
        case UASCA2:
            dev->regs.uart_reg.uasca2.all = data;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in uart_memory_write\n", offset);
            return Not_found_exp;
    }

    return No_exp;
}

exception_t uart_memory_read(conf_object_t *opaque, generic_address_t offset, uint32_t *val, size_t count)
{
    bm3803_system_control_device *dev = opaque->obj;

    switch (offset)
    {
        case UAD1:
            if (dev->regs.uart_reg.uac1.re)
            {
                *val = dev->regs.uart_reg.uad1.rtd;
                dev->regs.uart_reg.uas1.dr = 0;
            }
            break;
        case UAS1:
            *val = dev->regs.uart_reg.uas1.all;
            break;
        case UAC1:
            *val = dev->regs.uart_reg.uac1.all;
            break;
        case UASCA1:
            *val = dev->regs.uart_reg.uasca1.all;
            break;
        case UAD2:
            if (dev->regs.uart_reg.uac2.re)
            {
                *val = dev->regs.uart_reg.uad2.rtd;
                dev->regs.uart_reg.uas2.dr = 0;
            }
            break;
        case UAS2:
            *val = dev->regs.uart_reg.uas2.all;
            break;
        case UAC2:
            *val = dev->regs.uart_reg.uac2.all;
            break;
        case UASCA2:
            *val = dev->regs.uart_reg.uasca2.all;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in uart_memory_read\n", offset);
            return Not_found_exp;
    }

    return No_exp;

}
