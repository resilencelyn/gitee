/*************************************************************************
  > File Name    : bm3823_timer.c
  > Author       : zhangjianhao
  > Mail         : jiachao@163.com
  > Created Time : 2021/06/20 13:44:19
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
#include <skyeye_device.h>
#include <skyeye_system.h>


#define DEBUG
#include <skyeye_log.h>
#include "bm3823_system_control.h"

#define DEFAULT_BUS_FREQ    33000000

void timer1_interrupt_handler(conf_object_t *opaque)
{
    bm3823_system_control_device *dev = opaque->obj;

    if (dev->regs.timer_reg.timerctrl1.en0)
    {
        if (dev->signal.iface != NULL)
        {
            dev->signal.iface->raise_signal(dev->signal.obj, IRQ_NUM_TIMER1);
        }
    }
}

void timer2_interrupt_handler(conf_object_t *opaque)
{
    bm3823_system_control_device *dev = opaque->obj;

    if (dev->regs.timer_reg.timerctrl2.en1)
    {
        if (dev->signal.iface != NULL)
        {
            dev->signal.iface->raise_signal(dev->signal.obj, IRQ_NUM_TIMER2);
        }
    }
}

void timer_count_handler(conf_object_t *opaque)
{
    bm3823_system_control_device *dev = opaque->obj;

    /*decremented on each clock cycle*/
    dev->regs.timer_reg.scalercnt.scalercnt --;

    if (dev->regs.timer_reg.scalercnt.scalercnt == 0)
    {
        /*decremented each time the prescaler*/
        if (dev->regs.timer_reg.timerctrl1.en0)
        {
            dev->regs.timer_reg.timercnt1 --;
        }

        if (dev->regs.timer_reg.timerctrl1.en0)
        {
            dev->regs.timer_reg.timercnt2 --;
        }

        dev->regs.timer_reg.wdog --;

        /*Timer1 interrupt general condtion*/
        if (dev->regs.timer_reg.timercnt1 == 0)
        {
            if (dev->regs.timer_reg.timerctrl1.rl0)
            {
                dev->regs.timer_reg.timercnt1 = dev->regs.timer_reg.timerload1; /*reload counter*/
            }
            if (dev->signal.iface != NULL)
            {
                dev->signal.iface->raise_signal(dev->signal.obj, IRQ_NUM_TIMER1);
            }
        }

        /*Timer2 interrupt general condtion*/
       if (dev->regs.timer_reg.timercnt2 == 0)
        {
            if (dev->regs.timer_reg.timerctrl2.rl1)
            {
                dev->regs.timer_reg.timercnt1 = dev->regs.timer_reg.timerload2; /*reload counter*/
            }
            if (dev->signal.iface != NULL)
            {
                dev->signal.iface->raise_signal(dev->signal.obj, IRQ_NUM_TIMER2);
            }
        }

        /*WDG interrupt general condtion*/
        if (dev->regs.timer_reg.wdog == 0)
        {
            dev->regs.timer_reg.wdog = 0xffffffff;
        }
        /*the counter is reloaded from the prescaler*/
        dev->regs.timer_reg.scalercnt.scalercnt = dev->regs.timer_reg.scalerload.scalerload;
    }
}

exception_t timer_memory_write(conf_object_t *opaque, generic_address_t offset, uint32_t data, size_t count)
{
    bm3823_system_control_device *dev = opaque->obj;

    switch (offset)
    {
        case TCNT1:
            dev->regs.timer_reg.timercnt1 = data;
            break;
        case TRLD1:
            dev->regs.timer_reg.timerload1 = data;
            break;
        case TCTRL1:
            dev->regs.timer_reg.timerctrl1.all = data;
            if (dev->regs.timer_reg.timerctrl1.ld0)
            {
                dev->regs.timer_reg.timercnt1 = dev->regs.timer_reg.timerload1; /*reload counter*/
                dev->regs.timer_reg.timerctrl1.ld0 = 0; /*Always Read as 0*/
            }

            if (dev->regs.timer_reg.timerctrl1.en0)
            {
                  double period = (dev->regs.timer_reg.scalerload.all + 1);
                  double period_timer = period / DEFAULT_BUS_FREQ;

                  /*printf(" period : %lf\n",period);
                  printf(" period_timer: %lf\n",period_timer);
                  printf(" dev->regs.timer_reg.timerload1: %ld\n",(dev->regs.timer_reg.timerload1 + 1));*/
                  if(period_timer >= 1.0 )
                  {
                       period_timer = period_timer * (double)(dev->regs.timer_reg.timerload1 + 1);
                       dev->timer1_handle = system_register_timer_handler(opaque, (uint32_t)(period_timer), (time_sched_t)timer1_interrupt_handler, SCHED_S|SCHED_NORMAL);
                       /*dev->timer1_handle = system_register_timer_handler(opaque,   (uint32_t)period_timer , (time_sched_t)timer_count_handler, SCHED_S|SCHED_NORMAL);*/
                  } else if(period_timer * 1000 >= 1.0)
                  {
                       period_timer = period_timer * 1000 * (double)(dev->regs.timer_reg.timerload1 + 1);
                       dev->timer1_handle = system_register_timer_handler(opaque, (uint32_t)(period_timer), (time_sched_t)timer1_interrupt_handler, SCHED_MS|SCHED_NORMAL);
                       /*dev->timer1_handle = system_register_timer_handler(opaque,   (uint32_t)(period_timer * 1000) , (time_sched_t)timer_count_handler, SCHED_MS|SCHED_NORMAL);*/
                  } else if(period_timer * 1000000 >= 1.0)
                  {
                       period_timer = period_timer * 10000000 * (double)(dev->regs.timer_reg.timerload1 + 1);
                       dev->timer1_handle = system_register_timer_handler(opaque, (uint32_t)(period_timer), (time_sched_t)timer1_interrupt_handler, SCHED_US|SCHED_NORMAL);
                       /*dev->timer1_handle = system_register_timer_handler(opaque,   (uint32_t)(period_timer * 1000000) , (time_sched_t)timer_count_handler, SCHED_US|SCHED_NORMAL);*/
                  } else if(period_timer * 1000000000 >= 1.0)
                  {
                       period_timer = period_timer * 1000000000 * (double)(dev->regs.timer_reg.timerload1 + 1);
                       dev->timer1_handle = system_register_timer_handler(opaque, (uint32_t)(period_timer), (time_sched_t)timer1_interrupt_handler, SCHED_NS|SCHED_NORMAL);
                       /*dev->timer1_handle = system_register_timer_handler(opaque,   (uint64_t)(period_timer * 1000000000) , (time_sched_t)timer_count_handler, SCHED_NS|SCHED_NORMAL);*/
                  }

             } else
             {
                if(dev->timer1_handle != -1)
                {
                   system_disable_timer(dev->timer1_handle);
                }
             }
            break;
        case WDOG:
            dev->regs.timer_reg.wdog = data;
            break;
        case TCNT2:
            dev->regs.timer_reg.timercnt2 = data;
            break;
        case TRLD2:
             dev->regs.timer_reg.timerload2 = data;
            break;
        case TCTRL2:
            dev->regs.timer_reg.timerctrl2.all = data;
            if (dev->regs.timer_reg.timerctrl2.ld1)
            {
                dev->regs.timer_reg.timercnt2 = dev->regs.timer_reg.timerload2; /*reload counter*/
                dev->regs.timer_reg.timerctrl2.ld1 = 0; /*Always Read as 0*/
            }

            if (dev->regs.timer_reg.timerctrl2.en1)
            {
                  double period = (dev->regs.timer_reg.scalerload.all + 1);
                  double period_timer = period / DEFAULT_BUS_FREQ;

                  /*printf(" period : %lf\n",period);
                  printf(" period_timer: %lf\n",period_timer);
                  printf(" dev->regs.timer_reg.timerload1: %ld\n",(dev->regs.timer_reg.timerload1 + 1));*/

                  if(period_timer >= 1.0 )
                  {
                       period_timer = period_timer * (double)(dev->regs.timer_reg.timerload2 + 1);
                       dev->timer2_handle = system_register_timer_handler(opaque, (uint32_t)(period_timer), (time_sched_t)timer2_interrupt_handler, SCHED_S|SCHED_NORMAL);
                       /*dev->timer1_handle = system_register_timer_handler(opaque,   (uint32_t)period_timer , (time_sched_t)timer_count_handler, SCHED_S|SCHED_NORMAL);*/
                  } else if(period_timer * 1000 >= 1.0)
                  {
                       period_timer = period_timer * 1000 * (double)(dev->regs.timer_reg.timerload2 + 1);
                       dev->timer2_handle = system_register_timer_handler(opaque, (uint32_t)(period_timer), (time_sched_t)timer2_interrupt_handler, SCHED_MS|SCHED_NORMAL);
                       /*dev->timer1_handle = system_register_timer_handler(opaque,   (uint32_t)(period_timer * 1000) , (time_sched_t)timer_count_handler, SCHED_MS|SCHED_NORMAL);*/
                  } else if(period_timer * 1000000 >= 1.0)
                  {
                       period_timer = period_timer * 10000000 * (double)(dev->regs.timer_reg.timerload2 + 1);
                       dev->timer2_handle = system_register_timer_handler(opaque, (uint32_t)(period_timer), (time_sched_t)timer2_interrupt_handler, SCHED_US|SCHED_NORMAL);
                       /*dev->timer1_handle = system_register_timer_handler(opaque,   (uint32_t)(period_timer * 1000000) , (time_sched_t)timer_count_handler, SCHED_US|SCHED_NORMAL);*/
                  } else if(period_timer * 1000000000 >= 1.0)
                  {
                       period_timer = period_timer * 1000000000 * (double)(dev->regs.timer_reg.timerload2 + 1);
                       dev->timer2_handle = system_register_timer_handler(opaque, (uint32_t)(period_timer), (time_sched_t)timer2_interrupt_handler, SCHED_NS|SCHED_NORMAL);
                       /*dev->timer1_handle = system_register_timer_handler(opaque,   (uint64_t)(period_timer * 1000000000) , (time_sched_t)timer_count_handler, SCHED_NS|SCHED_NORMAL);*/
                  }

             } else
             {
                if(dev->timer2_handle != -1)
                {
                   system_disable_timer(dev->timer2_handle);
                }
             }
            break;
        case SCNT:
            dev->regs.timer_reg.scalercnt.all = data;
            break;
        case SRLD:
            dev->regs.timer_reg.scalerload.all = data;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in timer_memory_write\n", offset);
            return Not_found_exp;
    }

    return No_exp;
}

exception_t timer_memory_read(conf_object_t *opaque, generic_address_t offset, uint32_t *val, size_t count)
{
    bm3823_system_control_device *dev = opaque->obj;
    timer_reg_t *timer_reg = &dev->regs.timer_reg;

    switch (offset)
    {
        case TCNT1:
            *val = dev->regs.timer_reg.timercnt1;
            break;
        case TRLD1:
            *val = dev->regs.timer_reg.timerload1;
            break;
        case TCTRL1:
            *val = dev->regs.timer_reg.timerctrl1.all;
            break;
        case WDOG:
            *val = dev->regs.timer_reg.wdog;
            break;
        case TCNT2:
            *val = dev->regs.timer_reg.timercnt2;
            break;
        case TRLD2:
            *val = dev->regs.timer_reg.timerload2;
            break;
        case TCTRL2:
            *val = dev->regs.timer_reg.timerctrl2.all;
            break;
        case SCNT:
            *val = dev->regs.timer_reg.scalercnt.all;
            break;
        case SRLD:
            *val = dev->regs.timer_reg.scalerload.all;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in timer_memory_read\n", offset);
            return Not_found_exp;
    }

    return No_exp;
}
