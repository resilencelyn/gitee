/*************************************************************************
  > File Name    : bm3803_timer.c
  > Author       : jiachao
  > Mail         : jiachao@163.com
  > Created Time : 2019/12/24 13:44:19
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

void timer_interrupt_handler(conf_object_t *opaque)
{
    bm3803_system_control_device *dev = opaque->obj;

    if (dev->regs.timer_reg.timctr1.en)
    {
        if(dev->signal.iface != NULL)
        {
            dev->signal.iface->raise_signal(dev->signal.obj, Timer1);
        }
        else
        {
            interrupt_request_handle(opaque, Timer1);
        }
    }
    if (dev->regs.timer_reg.timctr2.en)
    {
        if(dev->signal.iface != NULL)
        {
            dev->signal.iface->raise_signal(dev->signal.obj, Timer1);
        }
        else
        {
            interrupt_request_handle(opaque, Timer2);
        }
    }	
}

void timer_count_handler(conf_object_t *opaque)
{
    bm3803_system_control_device *dev = opaque->obj;

    /*decremented on each clock cycle*/
    dev->regs.timer_reg.scac.cnt --;

    if (dev->regs.timer_reg.scac.cnt == 0)
    {
        /*decremented each time the prescaler*/
        //printf("decremented each time the prescaler\n");
        if (dev->regs.timer_reg.timctr1.en){
            //printf("Current timer reg timec1 : %d\n",dev->regs.timer_reg.timc1);
            dev->regs.timer_reg.timc1 --;
        }

        if (dev->regs.timer_reg.timctr2.en){
            //printf("dev->regs.timer_reg.timctr2.en\n");
            dev->regs.timer_reg.timc2 --;
        }

        dev->regs.timer_reg.wdg --;

        /*Timer1 interrupt general condtion*/
        if (dev->regs.timer_reg.timc1 == 0)
        {
            //printf("interrupt timer1\n");
            if (dev->regs.timer_reg.timctr1.rl)
            {
                dev->regs.timer_reg.timc1 = dev->regs.timer_reg.timr1; /*reload counter*/
            }
            if(dev->signal.iface != NULL)
            {
                dev->signal.iface->raise_signal(dev->signal.obj, Timer1);
            }
            else
            {
                interrupt_request_handle(opaque, Timer1);
            }
        }

        /*Timer2 interrupt general condtion*/
        if (dev->regs.timer_reg.timc2 == 0)
        {
            //printf("interrupt timer2\n");
            if (dev->regs.timer_reg.timctr2.rl)
            {
                dev->regs.timer_reg.timc2 = dev->regs.timer_reg.timr2; /*reload counter*/
            }
            if(dev->signal.iface != NULL)
            {
                dev->signal.iface->raise_signal(dev->signal.obj, Timer1);
            }
            else
            {
                interrupt_request_handle(opaque, Timer2);
            }
        }

        /*WDG interrupt general condtion*/
        if (dev->regs.timer_reg.wdg == 0)
        {
            //system reset
            dev->regs.timer_reg.wdg = 0xffffffff;
        }
        /*the counter is reloaded from the prescaler*/
        dev->regs.timer_reg.scac.cnt = dev->regs.timer_reg.scar.rv;
    }
}

exception_t timer_memory_write(conf_object_t *opaque, generic_address_t offset, uint32_t data, size_t count)
{
    bm3803_system_control_device *dev = opaque->obj;

    switch (offset)
    {
        case TIMC1:
            dev->regs.timer_reg.timc1 = data;
            break;
        case TIMR1:
            dev->regs.timer_reg.timr1 = data;
            break;
        case TIMCTR1:
            dev->regs.timer_reg.timctr1.all = data;
            if (dev->regs.timer_reg.timctr1.ld)
            {
                dev->regs.timer_reg.timc1 = dev->regs.timer_reg.timr1; /*reload counter*/
                dev->regs.timer_reg.timctr1.ld = 0; /*Always Read as 0*/
            }
            break;
        case WDG:
            dev->regs.timer_reg.wdg = data;
            break;
        case TIMC2:
            dev->regs.timer_reg.timc2 = data;
            break;
        case TIMR2:
            dev->regs.timer_reg.timr2 = data;
            break;
        case TIMCTR2:
            dev->regs.timer_reg.timctr2.all = data;
            if (dev->regs.timer_reg.timctr2.ld)
            {
                dev->regs.timer_reg.timc2 = dev->regs.timer_reg.timr2; /*reload counter*/
                dev->regs.timer_reg.timctr2.ld = 0; /*Always Read as 0*/
            }
            break;
        case SCAC:
            dev->regs.timer_reg.scac.all = data;
            break;
        case SCAR:
            dev->regs.timer_reg.scar.all = data;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in timer_memory_write\n", offset);
            return Not_found_exp;
    }

    return No_exp;
}

exception_t timer_memory_read(conf_object_t *opaque, generic_address_t offset, uint32_t *val, size_t count)
{
    bm3803_system_control_device *dev = opaque->obj;
    timer_reg_t *timer_reg = &dev->regs.timer_reg;

    switch (offset)
    {
        case TIMC1:
            *val = dev->regs.timer_reg.timc1;
            break;
        case TIMR1:
            *val = dev->regs.timer_reg.timr1;
            break;
        case TIMCTR1:
            *val = dev->regs.timer_reg.timctr1.all;
            break;
        case WDG:
            *val = dev->regs.timer_reg.wdg;
            break;
        case TIMC2:
            *val = dev->regs.timer_reg.timc2;
            break;
        case TIMR2:
            *val = dev->regs.timer_reg.timr2;
            break;
        case TIMCTR2:
            *val = dev->regs.timer_reg.timctr2.all;
            break;
        case SCAC:
            *val = dev->regs.timer_reg.scac.all;
            break;
        case SCAR:
            *val = dev->regs.timer_reg.scar.all;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in timer_memory_read\n", offset);
            return Not_found_exp;
    }

    return No_exp;
}
