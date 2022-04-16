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
* @file timer_c6678.c
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
#include <skyeye_system.h>

#include "timer_c6678.h"

#define DEFAULT_FREQ 200000000  // 1200M/6

enum TM_ACTION {
	TM_DISABLE,
	TM_ONESHOT,
	TM_CONTINU,
	TM_CON_PERIOD
};

enum TM_MODE {
	GP_64,
	UNCHAINED_32,
	WDOG_64,
	CHAINED_32
};

enum INT_TYPE {
	TINTLO,
	TINTHI
};
static void do_signal_raise(timer_c6678_device *dev)
{
    //check if we should send a interrupt 
    if(dev->regs->intctlstat & 0x1)
    {
	    core_signal_intf* core_signal = dev->core_signal;
	    interrupt_signal_t interrupt_signal;

	    int i = 0;
	    if (core_signal != NULL){
		    for(; i < 16; i++)
			    interrupt_signal.c6k_signal.ifr[i] = Prev_level;
			    interrupt_signal.c6k_signal.ifr[dev->int_number] = High_level;
			    core_signal->signal(dev->core, &interrupt_signal);
	    }
    }

    //send time event by general_intf to intc
    general_signal_intf* general_signal = dev->general_signal;
    general_signal->event_signal(dev->general_core, dev->event_number,&(dev->core_number));

                    
    return;
}
static void do_one_shot_gp_64(conf_object_t *opaque)
{
	timer_c6678_device *dev = opaque->obj;
	RW_WRLOCK(dev->lock);
	dev->regs->cnt = 0;
	RW_WRLOCK(dev->lock);

	/* raise signal TINTLO */
	do_signal_raise(dev);
	/* FIXME: edma TEVT12 and output events TM64P_OUT12 */

    //just one shot, disable timer
    system_disable_timer(dev->handle1);
    dev->handle1 = -1;
    
    RW_UNLOCK(dev->lock);
}
static void do_oneshot_chained_32(conf_object_t *opaque)
{
	timer_c6678_device *dev = opaque->obj;
        
    uint32_t period=0;
	RW_WRLOCK(dev->lock);

    dev->regs->cntlo ++;
    if(BITS(dev->regs->tcr,3,3))
      period = dev->regs->prdlo * 2;
    
    if (dev->regs->cntlo == period) {
        /* raise signal TINTLO */
        do_signal_raise(dev);
        /* FIXME: edma TEVT12 and output events TM64P_OUT12 */
        dev->regs->cntlo = 0;
    }
    dev->regs->cnthi = 0;
    
    //just one shot, disable timer
    system_disable_timer(dev->handle2);
    dev->handle2  = -1;

    RW_UNLOCK(dev->lock);
	/* FIXME: edma TEVTLO and output events TM64P_OUT12 */
}

static void do_one_shot_unchained12_32(conf_object_t *opaque)
{
	timer_c6678_device *dev = opaque->obj;

	RW_WRLOCK(dev->lock);

		/* raise signal TINTLO */
	do_signal_raise(dev);
	/* FIXME: edma TEVT12 and output events TM64P_OUT12 */
	dev->regs->cntlo = 0;

    system_disable_timer(dev->handle3);
    dev->handle3 = -1;
	RW_UNLOCK(dev->lock);

}
static void do_period_gp_64(conf_object_t *opaque)
{
	timer_c6678_device *dev = opaque->obj;
	int mode = BITS(dev->regs->tcr, 6, 7);

	RW_WRLOCK(dev->lock);

		/* raise signal TINTLO */
		do_signal_raise(dev);
		/* FIXME: edma TEVT12 and output events TM64P_OUT12 */
		dev->regs->cnt = 0;
		// the mode is period reload or not（TCR: bit-7 and bit-6 is 1）
		if(((dev->regs->tcr)>>6)&0x3 == 0x3){
			//DEBUG(Error_log, __FUNCTION__, "TCR: bit-7 and bit-6 is 1 and 1:period reload \n");
			// timer puls feature is available or not
			if(((dev->regs->tcr)>>4)&0x1 == 0x1){
				// the two conditions above is satisfied, the value of RELLO and RELHI were send to PRDLO and PRDHI
                if(dev->regs->prdlo == dev->regs->rello && dev->regs->prdhi == dev->regs->relhi)
                {
                     RW_UNLOCK(dev->lock);
                     return ;
                }
                else
                {
				    dev->regs->prdlo = dev->regs->rello;
				    dev->regs->prdhi = dev->regs->relhi;

                    system_disable_timer(dev->handle1);
                    dev->handle1 = -1;

                    double period = (double)dev->regs->prd;
                    if(BITS(dev->regs->tcr,3,3))
                        period = period *2;
                    double period_timer = period/ DEFAULT_FREQ;

                    printf("%lf reload period_timer\n",period_timer);
                    printf("%lf reload period\n",period);

                    if(period_timer >= 1.0 )
                        dev->handle1 = system_register_timer_handler(dev->obj,   (uint32_t)period_timer , (time_sched_t)do_period_gp_64, SCHED_S|SCHED_NORMAL);
                    else if(period_timer * 1000 >= 1.0)
                        dev->handle1 = system_register_timer_handler(dev->obj,   (uint32_t)(period_timer * 1000) , (time_sched_t)do_period_gp_64,SCHED_MS|SCHED_NORMAL);
                    else if(period_timer * 1000000 >= 1.0)
                        dev->handle1 = system_register_timer_handler(dev->obj,   (uint32_t)(period_timer * 1000000) , (time_sched_t)do_period_gp_64,SCHED_US|SCHED_NORMAL);
                    else if(period_timer * 1000000000 >= 1.0)
                        dev->handle1 = system_register_timer_handler(dev->obj,   (uint32_t)(period_timer * 1000000000) , (time_sched_t)do_period_gp_64,SCHED_NS|SCHED_NORMAL);

                }
			}
		}

	RW_UNLOCK(dev->lock);
}
static void do_period_chained_32(conf_object_t *opaque)
{
	timer_c6678_device *dev = opaque->obj;
    uint32_t period=0;
    RW_WRLOCK(dev->lock);
    dev->regs->cntlo ++;
    if(BITS(dev->regs->tcr,3,3))
        period = dev->regs->prdlo * 2;

	if (dev->regs->cntlo == period) {
			/* raise signal TINTLO */
			do_signal_raise(dev);
			/* FIXME: edma TEVT12 and output events TM64P_OUT12 */
			dev->regs->cntlo = 0;
	    }
		dev->regs->cnthi = 0;

	RW_UNLOCK(dev->lock);
}

static void do_period_unchained12_32(conf_object_t *opaque)
{
	timer_c6678_device *dev = opaque->obj;
	uint32_t *p, i, step;

	RW_WRLOCK(dev->lock);

		/* raise signal TINTLO */
		do_signal_raise(dev);
		/* FIXME: edma TEVT12 and output events TM64P_OUT12 */
		dev->regs->cntlo = 0;

		// the mode is period reload or not（TCR: bit-7 and bit-6 is 1）
		if(((dev->regs->tcr)>>6)&0x3 == 0x3){
			//DEBUG(Error_log, __FUNCTION__, "TCR: bit-7 and bit-6 is 1 and 1:period reload \n");
			// timer puls feature is available or not
			if(((dev->regs->tcr)>>4)&0x1 == 0x1){
				// the two conditions above is satisfied, the value of RELLO and RELHI were send to PRDLO and PRDHI
                if(dev->regs->prdlo == dev->regs->rello)
                {
                     RW_UNLOCK(dev->lock);
                     return;
                }
                else
                {
				    dev->regs->prdlo = dev->regs->rello;
				    dev->regs->prdhi = dev->regs->relhi;

                    system_disable_timer(dev->handle3);
                    dev->handle3 = -1;

                    double period = (double)dev->regs->prdlo;
                    if(BITS(dev->regs->tcr,3,3))
                        period = period *2;
                    double period_timer = period/ DEFAULT_FREQ;

                    printf("%lf reload period_timer\n",period_timer);
                    printf("%lf reload period\n",period);

                    if(period_timer >= 1.0 )
                        dev->handle3 = system_register_timer_handler(dev->obj,   (uint32_t)period_timer , (time_sched_t)do_period_unchained12_32, SCHED_S|SCHED_NORMAL);
                    else if(period_timer * 1000 >= 1.0)
                        dev->handle3 = system_register_timer_handler(dev->obj,   (uint32_t)(period_timer * 1000) , (time_sched_t)do_period_unchained12_32,SCHED_MS|SCHED_NORMAL);
                    else if(period_timer * 1000000 >= 1.0)
                        dev->handle3 = system_register_timer_handler(dev->obj,   (uint32_t)(period_timer * 1000000) , (time_sched_t)do_period_unchained12_32,SCHED_US|SCHED_NORMAL);
                    else if(period_timer * 1000000000 >= 1.0)
                        dev->handle3 = system_register_timer_handler(dev->obj,   (uint32_t)(period_timer * 1000000000) , (time_sched_t)do_period_unchained12_32,SCHED_NS|SCHED_NORMAL);

                }
			}
		}


	RW_UNLOCK(dev->lock);
}
static void unchained34_periodic_cb(conf_object_t *opaque)
{
	timer_c6678_device *dev = opaque->obj;
	int mode = BITS(dev->regs->tcr, 22, 23);
	int psc34 = BITS(dev->regs->tgcr, 8, 11);
	int tddr34 = BITS(dev->regs->tgcr, 12, 15);
	uint32_t step;

	RW_WRLOCK(dev->lock);

	step = 1 + psc34 / 2;
	/* cntlo++ is to low, speed it */
	/* tddr34++; */
	tddr34 += step;
	tddr34 %= 0xf;
	dev->regs->tgcr = SET_BITS(dev->regs->tgcr, 12, 15, tddr34);

	/* if (tddr34 == psc34) { */
	if (tddr34 >= psc34 - step && tddr34 <= psc34 + step) {
		if (dev->regs->cnthi != 0 || dev->regs->prdhi != 0)
			dev->regs->cnthi++;
		if (dev->regs->cnthi == dev->regs->prdhi) {
			/* raise signal TINTHI */
			do_signal_raise(dev);
			/* FIXME: edma TEVT34 */
			dev->regs->cnthi = 0;
		}
	}

	RW_UNLOCK(dev->lock);
}
static void unchained34_oneshot_cb(conf_object_t *opaque)
{
	timer_c6678_device *dev = opaque->obj;

	RW_WRLOCK(dev->lock);

	dev->regs->tgcr = SET_BITS(dev->regs->tgcr, 12, 15, 0);
	dev->regs->cnthi = 0;

	RW_UNLOCK(dev->lock);

	/* raise signal TINTHI */
	do_signal_raise(dev);
	/* FIXME: edma TEVT34 */
}
static void watchdog_periodic_cb(conf_object_t *opaque)
{
	timer_c6678_device *dev = opaque->obj;

	if (++dev->regs->cnt == dev->regs->prd) {
		do_signal_raise(dev);
		/* FIXME: generate watchdog timeout event */
		dev->regs->wdtcr &= ~(1 << 14);
		dev->regs->wdtcr |= 1 << 15;
	}
}
static exception_t do_with_tcr(conf_object_t *opaque)
{
	timer_c6678_device *dev = opaque->obj;
	timer_c6678_reg_t* regs = dev->regs;
	uint32_t ms = 1, mode12 = BITS(regs->tcr, 6, 7);
	uint32_t oldmode12 = BITS(dev->old_tcr, 6, 7);
	uint32_t mode34 = BITS(regs->tcr, 22, 23);
	uint32_t oldmode34 = BITS(dev->old_tcr, 22, 23);
	uint32_t psc34 = BITS(regs->tgcr, 8, 11);
	uint32_t freq = 75;

	/* TCR[ENAMODE12] */
    //printf("do_with_tcr\n");
    if(BITS(regs->tgcr, 2, 3) == GP_64)
    {
        switch(mode12)
        {
            case TM_DISABLE:
                if(dev->handle1 != -1)
                {
                  system_disable_timer(dev->handle1);
                }
                break;
            case TM_ONESHOT:
                if(dev->handle1 != -1 && mode12 == oldmode12 && BITS(regs->tgcr, 2, 3) == BITS(dev->old_tgcr,2,3))
                  system_enable_timer(dev->handle1);
                else  
                {
                    if(dev->handle1 != -1)
                    {
                      system_disable_timer(dev->handle1);
                      dev->handle1 = -1;
                    }
                    double period = (double)dev->regs->prd;
                    if(BITS(dev->regs->tcr,3,3))
                    {
                        period = period *2;
                    }
                    double period_timer = period/ DEFAULT_FREQ;

                    printf("one shot 64 %lf period_timer\n",period_timer);
                    printf("one shot 64 %lf period\n",period);

                    if(period_timer >= 1.0 )
                        dev->handle1 = system_register_timer_handler(dev->obj,   (uint32_t)period_timer , (time_sched_t)do_one_shot_gp_64, SCHED_S|SCHED_NORMAL);
                    else if(period_timer * 1000 >= 1.0)
                        dev->handle1 = system_register_timer_handler(dev->obj,   (uint32_t)(period_timer * 1000) , (time_sched_t)do_one_shot_gp_64,SCHED_MS|SCHED_NORMAL);
                    else if(period_timer * 1000000 >= 1.0)
                        dev->handle1 = system_register_timer_handler(dev->obj,   (uint32_t)(period_timer * 1000000) , (time_sched_t)do_one_shot_gp_64,SCHED_US|SCHED_NORMAL);
                    else if(period_timer * 1000000000 >= 1.0)
                        dev->handle1 = system_register_timer_handler(dev->obj,   (uint32_t)(period_timer * 1000000000) , (time_sched_t)do_one_shot_gp_64,SCHED_NS|SCHED_NORMAL);

                }
                break;
		    case TM_CONTINU:
		    case TM_CON_PERIOD:
                if(dev->handle1 != -1 && mode12 == oldmode12 && BITS(regs->tgcr, 2, 3) == BITS(dev->old_tgcr,2,3))
                  system_enable_timer(dev->handle1);
                else  
                {
                    if(dev->handle1 != -1)
                    {
                      system_disable_timer(dev->handle1);
                      dev->handle1 = -1;
                    }
                    double period = (double)dev->regs->prd;
                    if(BITS(dev->regs->tcr,3,3))
                        period = period *2;
                    double period_timer = period/ DEFAULT_FREQ;

                    printf("period 64 %lf period_timer\n",period_timer);
                    printf("period 64 %lf period\n",period);

                    if(period_timer >= 1.0 )
                        dev->handle1 = system_register_timer_handler(dev->obj,   (uint32_t)period_timer , (time_sched_t)do_period_gp_64, SCHED_S|SCHED_NORMAL);
                    else if(period_timer * 1000 >= 1.0)
                        dev->handle1 = system_register_timer_handler(dev->obj,   (uint32_t)(period_timer * 1000) , (time_sched_t)do_period_gp_64,SCHED_MS|SCHED_NORMAL);
                    else if(period_timer * 1000000 >= 1.0)
                        dev->handle1 = system_register_timer_handler(dev->obj,   (uint32_t)(period_timer * 1000000) , (time_sched_t)do_period_gp_64,SCHED_US|SCHED_NORMAL);
                    else if(period_timer * 1000000000 >= 1.0)
                        dev->handle1 = system_register_timer_handler(dev->obj,   (uint32_t)(period_timer * 1000000000) , (time_sched_t)do_period_gp_64,SCHED_NS|SCHED_NORMAL);
                }
                break;
        }

    }else if(BITS(regs->tgcr, 2, 3) == CHAINED_32)
    {
        switch(mode12)
        {
        case TM_DISABLE:
                if(dev->handle2 != -1)
                {
                  //printf("disable chained\n");
                  system_disable_timer(dev->handle2);
                }
                break;
            case TM_ONESHOT:
                if(dev->handle2 != -1 && mode12 == oldmode12 && BITS(regs->tgcr, 2, 3) == BITS(dev->old_tgcr,2,3))
                  system_enable_timer(dev->handle2);
                else  
                {
                    if(dev->handle2 != -1)
                    {
                      system_disable_timer(dev->handle2);
                      dev->handle2 = -1;
                    }
                    double period = (double)dev->regs->prdhi;
                    double period_timer = period/ DEFAULT_FREQ;

                    printf("oneshot chained32 %lf period_timer\n",period_timer);
                    printf("oneshot chained32 %lf period\n",period);

                    if(period_timer >= 1.0 )
                        dev->handle2 = system_register_timer_handler(dev->obj,   (uint32_t)period_timer , (time_sched_t)do_oneshot_chained_32, SCHED_S|SCHED_NORMAL);
                    else if(period_timer * 1000 >= 1.0)
                        dev->handle2 = system_register_timer_handler(dev->obj,   (uint32_t)(period_timer * 1000) , (time_sched_t)do_oneshot_chained_32,SCHED_MS|SCHED_NORMAL);
                    else if(period_timer * 1000000 >= 1.0)
                        dev->handle2 = system_register_timer_handler(dev->obj,   (uint32_t)(period_timer * 1000000) , (time_sched_t)do_oneshot_chained_32,SCHED_US|SCHED_NORMAL);
                    else if(period_timer * 1000000000 >= 1.0)
                        dev->handle2 = system_register_timer_handler(dev->obj,   (uint32_t)(period_timer * 1000000000) , (time_sched_t)do_oneshot_chained_32,SCHED_NS|SCHED_NORMAL);

                }
                break;
		    case TM_CONTINU:
            //no TM_CON_PERIOD mode
		    //case TM_CON_PERIOD:
                if(dev->handle2 != -1 && mode12 == oldmode12  && BITS(regs->tgcr, 2, 3) == BITS(dev->old_tgcr,2,3))
                  system_enable_timer(dev->handle2);
                else  
                {
                    if(dev->handle2 != -1)
                    {
                      system_disable_timer(dev->handle2);
                      dev->handle2 = -1;
                    }
                    double period = (double)dev->regs->prdhi;
                    if(BITS(dev->regs->tcr,3,3))
                        period = period *2;
                    double period_timer = period/ DEFAULT_FREQ;

                    printf("continue chained32 %lf period_timer\n",period_timer);
                    printf("continue chained32 %lf period\n",period);

                    if(period_timer >= 1.0 )
                        dev->handle2 = system_register_timer_handler(dev->obj,   (uint32_t)period_timer , (time_sched_t)do_period_chained_32, SCHED_S|SCHED_NORMAL);
                    else if(period_timer * 1000 >= 1.0)
                        dev->handle2 = system_register_timer_handler(dev->obj,   (uint32_t)(period_timer * 1000) , (time_sched_t)do_period_chained_32,SCHED_MS|SCHED_NORMAL);
                    else if(period_timer * 1000000 >= 1.0)
                        dev->handle2 = system_register_timer_handler(dev->obj,   (uint32_t)(period_timer * 1000000) , (time_sched_t)do_period_chained_32,SCHED_US|SCHED_NORMAL);
                    else if(period_timer * 1000000000 >= 1.0)
                        dev->handle2 = system_register_timer_handler(dev->obj,   (uint32_t)(period_timer * 1000000000) , (time_sched_t)do_period_chained_32,SCHED_NS|SCHED_NORMAL);
                }
                break; 
        }
    }else if (BITS(regs->tgcr, 2, 3) == UNCHAINED_32) {
        //for now do not care 32-bit timer with prescaler

     switch(mode12)
        {
        case TM_DISABLE:
                if(dev->handle3 != -1)
                {
                  system_disable_timer(dev->handle3);
                }
                break;
            case TM_ONESHOT:
                if(dev->handle3 != -1 && mode12 == oldmode12  && BITS(regs->tgcr, 2, 3) == BITS(dev->old_tgcr,2,3))
                  system_enable_timer(dev->handle3);
                else  
                {
                    if(dev->handle3 != -1)
                    {
                      system_disable_timer(dev->handle3);
                      dev->handle3 = -1;
                    }

                    double period = (double)dev->regs->prdlo;
                    double period_timer = period/ DEFAULT_FREQ;
                    
                    if(BITS(dev->regs->tcr,3,3))
                        period = period *2;

					#if 0 //fix me it will always register timer_handler to out max timer_handle cnt range
                    printf("oneshot unchained32 %lf period_timer\n",period_timer);
                    printf("oneshot unchained32 %lf period\n",period);
                    if(period_timer >= 1.0 )
                        dev->handle3 = system_register_timer_handler(dev->obj,   (uint32_t)period_timer , (time_sched_t)do_one_shot_unchained12_32, SCHED_S|SCHED_ONCE);
                    else if(period_timer * 1000 >= 1.0)
                        dev->handle3 = system_register_timer_handler(dev->obj,   (uint32_t)(period_timer * 1000) , (time_sched_t)do_one_shot_unchained12_32,SCHED_MS|SCHED_ONCE);
                    else if(period_timer * 1000000 >= 1.0)
                        dev->handle3 = system_register_timer_handler(dev->obj,   (uint32_t)(period_timer * 1000000) , (time_sched_t)do_one_shot_unchained12_32,SCHED_US|SCHED_ONCE);
                    else if(period_timer * 1000000000 >= 1.0)
                        dev->handle3 = system_register_timer_handler(dev->obj,   (uint32_t)(period_timer * 1000000000) , (time_sched_t)do_one_shot_unchained12_32,SCHED_NS|SCHED_ONCE);
					#endif

                }
                break;
		    case TM_CONTINU:
		    case TM_CON_PERIOD:
                if(dev->handle3 != -1 && mode12 == oldmode12  && BITS(regs->tgcr, 2, 3) == BITS(dev->old_tgcr,2,3))
                  system_enable_timer(dev->handle3);
                else  
                {
                    if(dev->handle3 != -1)
                    {
                      system_disable_timer(dev->handle3);
                      dev->handle3 = -1;
                    }
                    double period = (double)dev->regs->prd;
                    if(BITS(dev->regs->tcr,3,3))
                        period = period *2;
                    double period_timer = period/ DEFAULT_FREQ;

                    printf("period unchained32 %lf period_timer\n",period_timer);
                    printf("period unchained32 %lf period\n",period);

                    if(period_timer >= 1.0 )
                        dev->handle3 = system_register_timer_handler(dev->obj,   (uint32_t)period_timer , (time_sched_t)do_period_unchained12_32, SCHED_S|SCHED_NORMAL);
                    else if(period_timer * 1000 >= 1.0)
                        dev->handle3 = system_register_timer_handler(dev->obj,   (uint32_t)(period_timer * 1000) , (time_sched_t)do_period_unchained12_32,SCHED_MS|SCHED_NORMAL);
                    else if(period_timer * 1000000 >= 1.0)
                        dev->handle3 = system_register_timer_handler(dev->obj,   (uint32_t)(period_timer * 1000000) , (time_sched_t)do_period_unchained12_32,SCHED_US|SCHED_NORMAL);
                    else if(period_timer * 1000000000 >= 1.0)
                        dev->handle3 = system_register_timer_handler(dev->obj,   (uint32_t)(period_timer * 1000000000) , (time_sched_t)do_period_unchained12_32,SCHED_NS|SCHED_NORMAL);
                }
                break; 
        }

     switch(mode34)
     {
        case TM_DISABLE:
            break;
        case TM_ONESHOT:
            printf("mode34 unchained oneshot mode\n");
            break;
        case TM_CONTINU:
        case TM_CON_PERIOD:
            printf("mode34 unchained continue mode\n");
            break;
     }  

	}

	return No_exp;
}
static exception_t do_with_wdtcr(conf_object_t *opaque)
{
    printf("enter watch dog mode\n");
	timer_c6678_device *dev = opaque->obj;

	if (dev->handle3 != -1)
		return No_exp;

	create_thread_scheduler(1000, Periodic_sched, watchdog_periodic_cb, (void *)opaque, &dev->handle3);
	return No_exp;
}
static exception_t timer_c6678_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	timer_c6678_device *dev = opaque->obj;
	timer_c6678_reg_t* regs = dev->regs;
	uint32_t *data = (uint32_t *)buf;
	int time_mode = BITS(regs->tgcr, 2, 3);
	switch(offset) {
		case CLKSPD:
			*data = regs->clkspd;
			break;
		case CNTLO:
            skyeye_log(Error_log, __FUNCTION__,"read cntlo,not working\n");
			*data = regs->cntlo;
			regs->caplo = regs->cntlo;
			if (time_mode == GP_64)
				dev->cnthi_shadow = regs->cnthi;
			break;
		case CNTHI:
            skyeye_log(Error_log, __FUNCTION__,"read cnthi,not working\n");
			if (time_mode == GP_64){
				*data = dev->cnthi_shadow;
				regs->caphi = regs->cnthi;
			}	
			else if (time_mode == UNCHAINED_32){
				if (regs->tgcr & 0x2)	
					*data = regs->cnthi;
					regs->caphi = regs->cnthi;
			}
			break;
		case PRDLO:
			*data = regs->prdlo;
			break;
		case PRDHI:
			*data = regs->prdhi;
			break;
		case TCR:
			*data = regs->tcr;
			break;
		case TGCR:
			*data = regs->tgcr;
			break;
		case WDTCR:
			*data = regs->wdtcr;
			break;
		case RELLO:
			*data = regs->rello;
			break;
		case RELHI:
			*data = regs->relhi;
			break;
		case CAPLO:
			*data = regs->caplo;
			break;
		case CAPHI:
			*data = regs->caphi;
			break;
		case INTCTLSTAT:
			*data = regs->intctlstat;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in timer_c6678\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

static exception_t timer_c6678_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	timer_c6678_device *dev = opaque->obj;
	timer_c6678_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t*)buf;
	switch(offset) {
	case CLKSPD:
		regs->clkspd = val;
		break;
	case CNTLO:
		regs->cntlo = val;
		break;
	case CNTHI:
		regs->cnthi = val;
		break;
	case PRDLO:
		regs->prdlo = val;
		break;
	case PRDHI:
		regs->prdhi = val;
		break;
	case TCR:
        dev->old_tcr = regs->tcr;
		regs->tcr = val;
		do_with_tcr(opaque);
		break;
	case TGCR:
        dev->old_tgcr = regs->tgcr;
		regs->tgcr = val;
        
        //TIMHIRS TIMLORS reset
        if(BITS(regs->tgcr, 0, 1) == 0)
        {
            if(dev->handle1 != -1)
            {
              system_disable_timer(dev->handle1);
                  dev->handle1 = -1;
            }
            if(dev->handle2 != -1 )
            {
              system_disable_timer(dev->handle2);
                  dev->handle2 = -1;
            }
            if(dev->handle3 != -1)
            {
              system_disable_timer(dev->handle3);
                  dev->handle3 = -1;
            }
        }
		dev->timer_count = (regs->tgcr >> 12) & 0xF;
		break;
	case WDTCR:
		regs->wdtcr = val;
		if (BITS(regs->tgcr, 2, 3) == WDOG_64
			&& BITS(dev->regs->wdtcr, 14, 14))
				do_with_wdtcr(opaque);
			if ((val >> 16) == 0xda7e) {
				RW_WRLOCK(dev->lock);
				regs->cnt = 0;
				RW_UNLOCK(dev->lock);
			}
		break;
	case RELLO:
		regs->rello = val;
		break;
	case RELHI:
		regs->relhi = val;
		break;
	case CAPLO:
		regs->caplo = val;
		break;
	case CAPHI:
		regs->caphi = val;
		break;
	case INTCTLSTAT:
		regs->intctlstat = (val | 0x000f000f);
		break;
	default:
		skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in timer_c6678\n", offset);
		return Invarg_exp;
	}
	return No_exp;
}

static char* timer_c6678_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t timer_c6678_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	timer_c6678_device *dev = conf_obj->obj;
	timer_c6678_reg_t* regs = dev->regs;
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static char* timer_c6678_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	timer_c6678_device *dev = conf_obj->obj;
	timer_c6678_reg_t* regs = dev->regs;
	*((uint32_t*)regs + id) = value;

	return NULL;
}

static uint32_t timer_c6678_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	timer_c6678_device *dev = conf_obj->obj;
	timer_c6678_reg_t* regs = dev->regs;
	return  sizeof(timer_c6678_reg_t) / 4;
}

static uint32_t timer_c6678_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	timer_c6678_device *dev = conf_obj->obj;
	timer_c6678_reg_t* regs = dev->regs;
	uint32_t regnum;
	regnum = sizeof(timer_c6678_reg_t)/ 4;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}

	return regnum;
}

static conf_object_t* new_timer_c6678(char* obj_name){
	timer_c6678_device* dev = skyeye_mm_zero(sizeof(timer_c6678_device));
	timer_c6678_reg_t* regs =  skyeye_mm_zero(sizeof(timer_c6678_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init timer_c6678 regs */
	dev->regs = regs;

    dev->handle1 = -1;
    dev->handle2 = -1;
    dev->handle3 = -1;
	dev->regs->clkspd = 8 << 16;
    
	RWLOCK_INIT(dev->lock);

	system_register_timer_handler(dev->obj,   10 , (time_sched_t)do_one_shot_unchained12_32,SCHED_MS|SCHED_NORMAL);
	return dev->obj;
}
static void free_timer_c6678(conf_object_t* conf_obj){
	timer_c6678_device* dev = conf_obj->obj;
	RWLOCK_DESTROY(dev->lock);
    skyeye_free(dev->regs);
	skyeye_free(dev);
	skyeye_free(conf_obj);

	return;	
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	timer_c6678_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->int_number);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	timer_c6678_device *dev = conf_obj->obj;
	dev->int_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static attr_value_t get_attr_event_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
    timer_c6678_device *dev = conf_obj->obj;
    attr_value_t event_number = SKY_make_attr_uinteger(dev->event_number);
    return event_number;
}
static exception_t set_attr_event_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
    timer_c6678_device *dev = conf_obj->obj;
    dev->event_number = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_timer_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
    timer_c6678_device *dev = conf_obj->obj;
    attr_value_t core_number = SKY_make_attr_uinteger(dev->core_number);
    return core_number;
}
static exception_t set_attr_timer_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
    timer_c6678_device *dev = conf_obj->obj;
    dev->core_number = SKY_attr_uinteger(*value);
    return No_exp;
}

static timer_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of timer");
	SKY_register_attribute(clss, "event_number", get_attr_event_number, NULL, set_attr_event_number, NULL, SKY_Attr_Optional, "uinteger", "event number of timer");
	SKY_register_attribute(clss, "timer_number", get_attr_timer_number, NULL, set_attr_timer_number, NULL, SKY_Attr_Optional, "uinteger", "number of timer");
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	timer_c6678_device *dev = obj->obj;
	dev->core = obj2;
	dev->core_signal = (core_signal_intf *)SKY_get_iface(dev->core, CORE_SIGNAL_INTF_NAME);
	if (dev->core_signal == NULL)
	{
		if (dev->core == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", CORE_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", CORE_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}

	return No_exp;
}
 
static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	timer_c6678_device *dev = obj->obj;
	*obj2 = dev->core;
	*port = NULL;
	return No_exp;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	timer_c6678_device *dev = obj->obj;
	dev->general_core = obj2;
	dev->general_signal = (general_signal_intf *)SKY_get_iface(dev->general_core, GENERAL_SIGNAL_INTF_NAME);

	if (dev->general_signal == NULL)
	{
		if (dev->general_core == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
        timer_c6678_device *dev = obj->obj;
        *obj2 = dev->general_core;
        *port = NULL;
        
        return No_exp;
}

void init_timer_c6678(){
	static skyeye_class_t class_data = {
		.class_name = "c6678_timer",
		.class_desc = "c6678_timer",
		.new_instance = new_timer_c6678,
		.free_instance = free_timer_c6678,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_c6678),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = timer_c6678_read,
		.write = timer_c6678_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = timer_c6678_get_regvalue_by_id,
		.get_regname_by_id = timer_c6678_get_regname_by_id,
		.set_regvalue_by_id = timer_c6678_set_regvalue_by_id,
		.get_regnum = timer_c6678_get_regnum,
		.get_regid_by_name = timer_c6678_get_regid_by_name,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf,
		},
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME, 
			.iface = &io_memory,
		},
 	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription ){
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		},
                
        (struct ConnectDescription ){
            .name = GENERAL_SIGNAL_INTF_NAME,
            .set = general_signal_set,
            .get = general_signal_get,
        },

	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	timer_register_attribute(clss);
}
