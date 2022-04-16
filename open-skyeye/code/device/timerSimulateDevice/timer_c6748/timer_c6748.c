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
 * @file timer_c6748.c
 * @brief The implementation of system controller
 * @author
 * @version 78.77
 */

#include <skyeye_lock.h>
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
#include "timer_c6748.h"

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
	TINT12,
	TINT34
};

#define T64P0_TINT12	4
#define T64P2_TINTALL   25	
#define T64P1_TINT12	40
#define T64P1_TINT34	48
#define T64P0_TINT34	64
#define T64P3_TINTALL	86

#define T64P2_CMPINT0	78
#define T64P2_CMPINT1	79
#define T64P2_CMPINT2	80
#define T64P2_CMPINT3	81
#define T64P2_CMPINT4	82
#define T64P2_CMPINT5	83
#define T64P2_CMPINT6	84
#define T64P2_CMPINT7	85

static int cmp_ints [8] = {
	T64P2_CMPINT0,
	T64P2_CMPINT1,
	T64P2_CMPINT2,
	T64P2_CMPINT3,
	T64P2_CMPINT4,
	T64P2_CMPINT5,
	T64P2_CMPINT6,
	T64P2_CMPINT7
};

static void do_signal_raise(timer_c6748_device *dev, int type)
{
	int signo;

	if (dev->signal_iface == NULL || dev->signal == NULL)
		return;

	if (dev->timer_number == 0) {
		signo = type == TINT12 ? T64P0_TINT12 : T64P0_TINT34;
		dev->signal_iface->raise_signal(dev->signal, signo);
	} else if (dev->timer_number == 1) {
		signo = type == TINT12 ? T64P1_TINT12 : T64P1_TINT34;
		dev->signal_iface->raise_signal(dev->signal, signo);
	} else if (dev->timer_number == 2) {
		signo = T64P2_TINTALL;
		dev->signal_iface->raise_signal(dev->signal, signo);
	} else if (dev->timer_number == 3) {
		signo = T64P3_TINTALL;
		dev->signal_iface->raise_signal(dev->signal, signo);
	}
}
static void do_one_shot_gp_64(conf_object_t *opaque)
{
	timer_c6748_device *dev = opaque->obj;

	RW_WRLOCK(dev->lock);
	dev->regs.tim64 = 0;
	RW_UNLOCK(dev->lock);

	if (dev->regs.intctlstat & 1) {
		dev->regs.intctlstat |= 1 << 1;
		/* raise signal TINT12 */
		do_signal_raise(dev, TINT12);
	}
	/* FIXME: edma TEVT12 and output events TM64P_OUT12 */
}
static void do_oneshot_chained_32(conf_object_t *opaque)
{
	timer_c6748_device *dev = opaque->obj;

	RW_WRLOCK(dev->lock);

	dev->regs.tim12 = 0;
	dev->regs.tim34 = 0;

	RW_UNLOCK(dev->lock);

	if (dev->regs.intctlstat & 1) {
		dev->regs.intctlstat |= 1 << 1;
		/* raise signal TINT12 */
		do_signal_raise(dev, TINT12);
	}
	/* FIXME: edma TEVT12 and output events TM64P_OUT12 */
}
static void do_period_gp_64(conf_object_t *opaque)
{
	timer_c6748_device *dev = opaque->obj;
	uint64_t prd64;
	int mode = BITS(dev->regs.tcr, 6, 7);

	RW_WRLOCK(dev->lock);

	prd64 = dev->regs.prd34;
	prd64 |= prd64 << 32 | dev->regs.prd12;

	if (dev->regs.tim64 != 0 || dev->regs.prd64 != 0) {
		//dev->regs.tim64++;
		/* tim64++ is to low, speed it */
		/* dev->regs.tim64++; */
		//step = 1 + dev->regs.prd64 / 50;
		dev->regs.tim64 += dev->step1;
	}

	/* if (dev->regs.tim64 == dev->regs.prd64) { */
	if ((dev->step1 <= dev->regs.prd64 ? (dev->regs.tim64 >= dev->regs.prd64 - dev->step1):(dev->regs.tim64 >= 0))
			&& dev->regs.tim64 <= dev->regs.prd64 + dev->step1) {
		if (dev->regs.intctlstat & 1) {
			dev->regs.intctlstat |= 1 << 1;
			/* raise signal TINT12 */
			do_signal_raise(dev, TINT12);
		}
		/* FIXME: edma TEVT12 and output events TM64P_OUT12 */
		if (mode == TM_CONTINU) {
			dev->regs.tim64 = 0;
		} else if (mode == TM_CON_PERIOD) {
			dev->regs.tim64 = 0;
			dev->regs.prd64 = dev->regs.rel64;
		}
	}

	RW_UNLOCK(dev->lock);
}
static void do_period_chained_32(conf_object_t *opaque)
{
	timer_c6748_device *dev = opaque->obj;
	int mode = BITS(dev->regs.tcr, 6, 7);
	//uint32_t step = 1 + dev->regs.prd34 / 50;

	RW_WRLOCK(dev->lock);

	/* tim34++ is to low, speed it */
	/* dev->regs.tim34++; */
	dev->regs.tim34 += dev->step1;

	/* if (dev->regs.tim34 == dev->regs.prd34) { */
	if ((dev->step1 <= dev->regs.prd34 ? (dev->regs.tim34 >= dev->regs.prd34 - dev->step1):(dev->regs.tim34 >= 0))
			&& dev->regs.tim34 <= dev->regs.prd34 + dev->step1) {
		if (dev->regs.tim12 != 0 || dev->regs.prd12 != 0)
			dev->regs.tim12++;
		if (dev->regs.tim12 == dev->regs.prd12) {
			if (dev->regs.intctlstat & 1) {
				dev->regs.intctlstat |= 1 << 1;
				/* raise signal TINT12 */
				do_signal_raise(dev, TINT12);
			}
			/* FIXME: edma TEVT12 and output events TM64P_OUT12 */
			if (mode == TM_CONTINU) {
				dev->regs.tim12 = 0;
			} else if (mode == TM_CON_PERIOD) {
				dev->regs.tim12 = 0;
				dev->regs.prd12 = dev->regs.rel12;
				dev->regs.prd34 = dev->regs.rel34;
			}
		}
		dev->regs.tim34 = 0;
	}

	RW_UNLOCK(dev->lock);
}
static void do_timer2_compare(conf_object_t *opaque)
{
	timer_c6748_device *dev = opaque->obj;
	uint32_t *p, i;

	/* only timer2 has compare register */
	if (dev->timer_number == 2 && BITS(dev->regs.tgcr , 4, 4)) {
		p = &dev->regs.cmp0;
		for (i = 0; i < 8; i++) {
			if (dev->regs.tim12 == p[i] && p[i]) {
				if (dev->regs.intctlstat & 1) {
					dev->regs.intctlstat |= 1 << 1;
					/* raise signal CMPINT */
					if (dev->signal_iface != NULL && dev->signal != NULL)
						dev->signal_iface->raise_signal(dev->signal, cmp_ints[i]);
				}
				/* FIXME: edma TEVT12 and output events TM64P_OUT12 */
			}
		}
	}
}
static void do_one_shot_unchained12_32(conf_object_t *opaque)
{
	timer_c6748_device *dev = opaque->obj;

	RW_WRLOCK(dev->lock);

	do_timer2_compare(opaque);

	dev->regs.tim12 = dev->regs.prd12;

	if (dev->regs.intctlstat & 1) {
		dev->regs.intctlstat |= 1 << 1;
		/* raise signal TINT12 */
		do_signal_raise(dev, TINT12);
	}
	/* FIXME: edma TEVT12 and output events TM64P_OUT12 */
	dev->regs.tim12 = 0;

	RW_UNLOCK(dev->lock);

}
static void do_period_unchained12_32(conf_object_t *opaque)
{
	timer_c6748_device *dev = opaque->obj;
	int mode = BITS(dev->regs.tcr, 6, 7);
	uint32_t *p, i;

	RW_WRLOCK(dev->lock);

	p = &dev->regs.cmp0;

	if (dev->regs.tim12 != 0 || dev->regs.prd12 != 0) {
		/* tim12++ is to low, speed it */
		/* dev->regs.tim12++; */
		//step = 1 + dev->regs.prd12 / 50;
		dev->regs.tim12 += dev->step1;
	}

	/* if (dev->regs.tim12 >= dev->regs.prd12) { */
	if ((dev->step1 <= dev->regs.tim12 ? (dev->regs.tim12 >= dev->regs.prd12 - dev->step1):(dev->regs.tim12 >= 0))
			&& dev->regs.tim12 <= dev->regs.prd12 + dev->step1) {
		do_timer2_compare(opaque);
		if (dev->regs.intctlstat & 1) {
			dev->regs.intctlstat |= 1 << 1;
			/* raise signal TINT12 */
			do_signal_raise(dev, TINT12);
		}
		/* FIXME: edma TEVT12 and output events TM64P_OUT12 */
		if (mode == TM_CONTINU) {
			dev->regs.tim12 = 0;
		}  else if (mode == TM_CON_PERIOD) {
			dev->regs.tim12 = 0;
			dev->regs.prd12 = dev->regs.rel12;
		}
	}

	RW_UNLOCK(dev->lock);
}
static void unchained34_periodic_cb(conf_object_t *opaque)
{
	timer_c6748_device *dev = opaque->obj;
	int mode = BITS(dev->regs.tcr, 22, 23);
	int psc34 = BITS(dev->regs.tgcr, 8, 11);
	int tddr34 = BITS(dev->regs.tgcr, 12, 15);
	//uint32_t step;

	RW_WRLOCK(dev->lock);

	//step = 1 + psc34 / 2;
	/* tim12++ is to low, speed it */
	/* tddr34++; */
	tddr34 += dev->step2;
	tddr34 %= 0xf;
	dev->regs.tgcr = SET_BITS(dev->regs.tgcr, 12, 15, tddr34);

	/* if (tddr34 == psc34) { */
	if ((dev->step2 < psc34 ? (tddr34 >= psc34 - dev->step2) : (tddr34 >= 0)) 
			&& tddr34 <= psc34 + dev->step2) {
		if (dev->regs.tim34 != 0 || dev->regs.prd34 != 0){
			dev->regs.tim34 += dev->step2;
		}
		if (dev->regs.tim34 >= dev->regs.prd34) {
			if (dev->regs.intctlstat & (1 << 16)) {
				dev->regs.intctlstat |= 1 << 17;
				/* raise signal TINT34 */
				do_signal_raise(dev, TINT34);
			}
			/* FIXME: edma TEVT34 */
			if (mode == TM_CONTINU) {
				dev->regs.tim34 = 0;
			} else if (mode == TM_CON_PERIOD) {
				dev->regs.tim34 = 0;
				dev->regs.prd34 = dev->regs.rel34;
			}
		}
	}

	RW_UNLOCK(dev->lock);
}
static void unchained34_oneshot_cb(conf_object_t *opaque)
{
	timer_c6748_device *dev = opaque->obj;

	RW_WRLOCK(dev->lock);

	dev->regs.tgcr = SET_BITS(dev->regs.tgcr, 12, 15, 0);
	dev->regs.tim34 = 0;

	RW_UNLOCK(dev->lock);

	if (dev->regs.intctlstat & (1 << 16)) {
		dev->regs.intctlstat |= 1 << 17;
		/* raise signal TINT34 */
		do_signal_raise(dev, TINT34);
	}
	/* FIXME: edma TEVT34 */
	system_disable_timer(dev->handle2);
}
static void watchdog_periodic_cb(conf_object_t *opaque)
{
	timer_c6748_device *dev = opaque->obj;

	if (++dev->regs.tim64 == dev->regs.prd64) {
		/* FIXME: generate watchdog timeout event */
		dev->regs.wdtcr &= ~(1 << 14);
		dev->regs.wdtcr |= 1 << 15;
	}
}
static void one_shot_cb(conf_object_t *opaque)
{
	timer_c6748_device *dev = opaque->obj;

	switch (BITS(dev->regs.tgcr, 2, 3)) {
		case GP_64:
			do_one_shot_gp_64(opaque);
			break;
		case UNCHAINED_32:
			do_one_shot_unchained12_32(opaque);
			break;
		case CHAINED_32:
			do_oneshot_chained_32(opaque);
			break;
		default:
			break;
	}
	system_disable_timer(dev->handle1);
}
static void con_period_cb(conf_object_t *opaque)
{
	timer_c6748_device *dev = opaque->obj;

	switch (BITS(dev->regs.tgcr, 2, 3)) {
		case GP_64:
			do_period_gp_64(opaque);
			break;
		case UNCHAINED_32:
			do_period_unchained12_32(opaque);
			break;
		case CHAINED_32:
			do_period_chained_32(opaque);
			break;
		default:
			break;
	}
}
static exception_t do_with_tcr(conf_object_t *opaque)
{
	timer_c6748_device *dev = opaque->obj;
	uint32_t ms = 1, mode12 = BITS(dev->regs.tcr, 6, 7);
	uint32_t oldmode12 = BITS(dev->old_tcr, 6, 7);
	uint32_t mode34 = BITS(dev->regs.tcr, 22, 23);
	uint32_t oldmode34 = BITS(dev->old_tcr, 22, 23);
	uint32_t psc34 = BITS(dev->regs.tgcr, 8, 11);
	uint32_t freq = 24.0;
	if(dev->regs.tcr & 0x0100){
		if(dev->pll0_freq_clock != NULL){
			freq = dev->pll0_freq_clock->get_device_clock(dev->pll0, PLL0_C6748_AUXCLK);
		}else{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.\n", "PLL1 device", "PLL1_C6748_AUXCLK");
		}
	}else{
		if(dev->exte_clock != 0){
			freq = dev->exte_clock;
		}else{
			skyeye_log(Error_log, __FUNCTION__, "The external clock is zero\n");
		}
	}
	double period_timer = 1.0 / (freq * 1000000.0);
	/* TCR[ENAMODE12] */
	switch (mode12) {
		case TM_DISABLE:
			if (dev->sche_id1 != -1) {
				//del_thread_scheduler(dev->sche_id1);
				//dev->sche_id1 = -1;
				system_disable_timer(dev->handle1);
				dev->step1 = 0;
				dev->handle1 = NULL;
			}
			break;
		case TM_ONESHOT:
			/*if (BITS(dev->regs.tgcr, 2, 3) == GP_64) {
			  ms = 1000 * dev->regs.prd64 / (freq * 1000000);
			  ms = ms == 0 ? 1 : ms;
			  } else if (BITS(dev->regs.tgcr, 2, 3) == CHAINED_32) {
			  ms = 1000 * dev->regs.prd34 * dev->regs.prd12 / (freq * 1000000);
			  ms = ms == 0 ? 1 : ms;
			  }*/
			//if (dev->sche_id1 != -1) {
			if (dev->handle1 != NULL) {
				if (mode12 != oldmode12) {
					//del_thread_scheduler(dev->sche_id1);
					//create_thread_scheduler(ms, Oneshot_sched, one_shot_cb, (void *)opaque, &dev->sche_id1);
					system_disable_timer(dev->handle1);
					if(period_timer >= 1.0){
						dev->step1 = period_timer;
						dev->handle1 = system_register_timer_handler(dev->obj, (uint32_t)period_timer, (time_sched_t)one_shot_cb, SCHED_S|SCHED_ONCE);
					}else if(period_timer * 1000 >= 1.0){
						dev->step1 = period_timer * 1000;
						dev->handle1 = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000), (time_sched_t)one_shot_cb, SCHED_MS|SCHED_ONCE);
					}else if(period_timer * 1000000 >= 1.0){
						dev->step1 = period_timer * 1000000;
						dev->handle1 = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000000), (time_sched_t)one_shot_cb, SCHED_US|SCHED_ONCE);
					}else if(period_timer * 1000000000 >= 1.0){
						dev->step1 = period_timer * 1000000000;
						dev->handle1 = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000000000), (time_sched_t)one_shot_cb, SCHED_NS|SCHED_ONCE);
					}
				} else {
					//mod_thread_scheduler(dev->sche_id1, ms, Oneshot_sched);
				}
				break;
			}
			//create_thread_scheduler(ms, Oneshot_sched, one_shot_cb, (void *)opaque, &dev->sche_id1);
			if(period_timer >= 1.0){
				dev->step1 = period_timer;
				dev->handle1 = system_register_timer_handler(dev->obj, (uint32_t)period_timer, (time_sched_t)one_shot_cb, SCHED_S|SCHED_ONCE);
			}else if(period_timer * 1000 >= 1.0){
				dev->step1 = period_timer *1000;
				dev->handle1 = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000), (time_sched_t)one_shot_cb, SCHED_MS|SCHED_ONCE);
			}else if(period_timer * 1000000 >= 1.0){
				dev->step1 = period_timer * 1000000;
				dev->handle1 = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000000), (time_sched_t)one_shot_cb, SCHED_US|SCHED_ONCE);
			}else if(period_timer * 1000000000 >= 1.0){
				dev->step1 = period_timer * 1000000000;
				dev->handle1 = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000000000), (time_sched_t)one_shot_cb, SCHED_NS|SCHED_ONCE);
			}
			break;
		case TM_CONTINU:
		case TM_CON_PERIOD:
			ms = 1000 / (freq * 1000000);
			ms = ms == 0 ? 1 : ms;
			//if (dev->sche_id1 != -1) {
			if (dev->handle1 != NULL) {
				if (mode12 != oldmode12) {
					//del_thread_scheduler(dev->sche_id1);
					//create_thread_scheduler(ms, Periodic_sched, con_period_cb, (void *)opaque, &dev->sche_id1);
					system_disable_timer(dev->handle1);
					if(period_timer >= 1.0){
						dev->step1 = period_timer;
						dev->handle1 = system_register_timer_handler(dev->obj, (uint32_t)period_timer, (time_sched_t)con_period_cb, SCHED_S|SCHED_NORMAL);
					}else if(period_timer * 1000 >= 1.0){
						dev->step1 = period_timer * 1000;
						dev->handle1 = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000), (time_sched_t)con_period_cb, SCHED_MS|SCHED_NORMAL);
					}else if(period_timer * 1000000 >= 1.0){
						dev->step1 = period_timer * 1000000;
						dev->handle1 = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000000), (time_sched_t)con_period_cb, SCHED_US|SCHED_NORMAL);
					}else if(period_timer * 1000000000 >= 1.0){
						dev->step1 = period_timer * 1000000000;
						dev->handle1 = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000000000), (time_sched_t)con_period_cb, SCHED_NS|SCHED_NORMAL);
					}
				} else {
					//mod_thread_scheduler(dev->sche_id1, ms, Periodic_sched);
				}
				break;
			}
			//create_thread_scheduler(ms, Periodic_sched, con_period_cb, (void *)opaque, &dev->sche_id1);
			if(period_timer >= 1.0){
				dev->step1 = period_timer;
				dev->handle1 = system_register_timer_handler(dev->obj, (uint32_t)period_timer, (time_sched_t)con_period_cb, SCHED_S|SCHED_NORMAL);
			}else if(period_timer * 1000 >= 1.0){
				dev->step1 = period_timer * 1000;
				dev->handle1 = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000), (time_sched_t)con_period_cb, SCHED_MS|SCHED_NORMAL);
			}else if(period_timer * 1000000 >= 1.0){
				dev->step1 = period_timer * 1000000;
				dev->handle1 = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000000), (time_sched_t)con_period_cb, SCHED_US|SCHED_NORMAL);
			}else if(period_timer * 1000000000 >= 1.0){
				dev->step1 = period_timer * 1000000000;
				dev->handle1 = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000000000), (time_sched_t)con_period_cb, SCHED_NS|SCHED_NORMAL);
			}
			break;
	}

	if (BITS(dev->regs.tgcr, 2, 3) == UNCHAINED_32) {
		switch (mode34) {
			case TM_DISABLE:
				if (dev->sche_id2 != -1) {
					//del_thread_scheduler(dev->sche_id2);
					//dev->sche_id2 = -1;
					system_disable_timer(dev->handle2);
					dev->step2 = 0;
					dev->handle2 = NULL;
				}
				break;
			case TM_ONESHOT:
				//ms = 1000 * psc34 * dev->regs.tim34 / (freq * 1000000);
				//ms = ms == 0 ? 1 : ms;
				//if (dev->sche_id2 != -1) {
				if (dev->handle2 != NULL) {
					if (mode34 != oldmode34) {
						//del_thread_scheduler(dev->sche_id2);
						//create_thread_scheduler(ms, Oneshot_sched, unchained34_oneshot_cb, (void *)opaque, &dev->sche_id2);
						system_disable_timer(dev->handle2);
						if(period_timer >= 1.0){
							dev->step2 = period_timer;
							dev->handle2 = system_register_timer_handler(dev->obj, (uint32_t)period_timer, (time_sched_t)unchained34_oneshot_cb, SCHED_S|SCHED_ONCE);
						}else if(period_timer * 1000 >= 1.0){
							dev->step2 = period_timer * 1000;
							dev->handle2 = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000), (time_sched_t)unchained34_oneshot_cb, SCHED_MS|SCHED_ONCE);
						}else if(period_timer * 1000000 >= 1.0){
							dev->step2 = period_timer * 1000000;
							dev->handle2 = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000000), (time_sched_t)unchained34_oneshot_cb, SCHED_US|SCHED_ONCE);
						}else if(period_timer * 1000000000 >= 1.0){
							dev->step2 = period_timer * 1000000000;
							dev->handle2 = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000000000), (time_sched_t)unchained34_oneshot_cb, SCHED_NS|SCHED_ONCE);
						}
					} else {
						//mod_thread_scheduler(dev->sche_id2, ms, Oneshot_sched);
					}
					break;
				}
				//create_thread_scheduler(ms, Oneshot_sched, unchained34_oneshot_cb, (void *)opaque, &dev->sche_id2);
				if(period_timer >= 1.0){
					dev->step2 = period_timer;
					dev->handle2 = system_register_timer_handler(dev->obj, (uint32_t)period_timer, (time_sched_t)unchained34_oneshot_cb, SCHED_S|SCHED_ONCE);
				}else if(period_timer * 1000 >= 1.0){
					dev->step2 = period_timer * 1000;
					dev->handle2 = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000), (time_sched_t)unchained34_oneshot_cb, SCHED_MS|SCHED_ONCE);
				}else if(period_timer * 1000000 >= 1.0){
					dev->step2 = period_timer * 1000000;
					dev->handle2 = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000000), (time_sched_t)unchained34_oneshot_cb, SCHED_US|SCHED_ONCE);
				}else if(period_timer * 1000000000 >= 1.0){
					dev->step2 = period_timer * 1000000000;
					dev->handle2 = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000000000), (time_sched_t)unchained34_oneshot_cb, SCHED_NS|SCHED_ONCE);
				}
				break;
			case TM_CONTINU:
			case TM_CON_PERIOD:
				ms = 1000 * psc34 * dev->regs.tim34 / (freq * 1000000);
				ms = ms == 0 ? 1 : ms;
				if (dev->sche_id2 != -1) {
					if (mode34 != oldmode34) {
						//del_thread_scheduler(dev->sche_id2);
						//create_thread_scheduler(ms, Periodic_sched, unchained34_periodic_cb, (void *)opaque, &dev->sche_id2);
						system_disable_timer(dev->handle2);
						if(period_timer >= 1.0){
							dev->step2 = period_timer;
							dev->handle2 = system_register_timer_handler(dev->obj, (uint32_t)period_timer, (time_sched_t)unchained34_periodic_cb, SCHED_S|SCHED_NORMAL);
						}else if(period_timer * 1000 >= 1.0){
							dev->step2 = period_timer * 1000;
							dev->handle2 = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000), (time_sched_t)unchained34_periodic_cb, SCHED_MS|SCHED_NORMAL);
						}else if(period_timer * 1000000 >= 1.0){
							dev->step2 = period_timer * 1000000;
							dev->handle2 = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000000), (time_sched_t)unchained34_periodic_cb, SCHED_US|SCHED_NORMAL);
						}else if(period_timer * 1000000000 >= 1.0){
							dev->step2 = period_timer * 1000000000;
							dev->handle2 = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000000000), (time_sched_t)unchained34_periodic_cb, SCHED_NS|SCHED_NORMAL);
						}
					} else {
						//mod_thread_scheduler(dev->sche_id2, ms, Periodic_sched);
					}
					break;
				}
				//create_thread_scheduler(ms, Periodic_sched, unchained34_periodic_cb, (void *)opaque, &dev->sche_id2);
				if(period_timer >= 1.0){
					dev->step2 = period_timer;
					dev->handle2 = system_register_timer_handler(dev->obj, (uint32_t)period_timer, (time_sched_t)unchained34_periodic_cb, SCHED_S|SCHED_NORMAL);
				}else if(period_timer * 1000 >= 1.0){
					dev->step2 = period_timer * 1000;
					dev->handle2 = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000), (time_sched_t)unchained34_periodic_cb, SCHED_MS|SCHED_NORMAL);
				}else if(period_timer * 1000000 >= 1.0){
					dev->step2 = period_timer * 1000000;
					dev->handle2 = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000000), (time_sched_t)unchained34_periodic_cb, SCHED_US|SCHED_NORMAL);
				}else if(period_timer * 1000000000 >= 1.0){
					dev->step2 = period_timer * 1000000000;
					dev->handle2 = system_register_timer_handler(dev->obj, (uint32_t)(period_timer * 1000000000), (time_sched_t)unchained34_periodic_cb, SCHED_NS|SCHED_NORMAL);
				}
				break;
		}
		}
		return No_exp;
	}
	static exception_t do_with_wdtcr(conf_object_t *opaque)
	{
		timer_c6748_device *dev = opaque->obj;

		if (dev->sche_id3 != -1)
			return No_exp;

		create_thread_scheduler(1000, Periodic_sched, watchdog_periodic_cb, (void *)opaque, &dev->sche_id3);
		return No_exp;
	}
	static void do_with_intctlstat(timer_c6748_device *dev, uint32_t data)
	{
		dev->regs.intctlstat = data;

		if (BITS(data, 1, 1))
			dev->regs.intctlstat &= ~(1 << 1);
		if (BITS(data, 3, 3))
			dev->regs.intctlstat &= ~(1 << 3);
		if (BITS(data, 17, 17))
			dev->regs.intctlstat &= ~(1 << 17);
		if (BITS(data, 19, 19))
			dev->regs.intctlstat &= ~(1 << 19);
	}
	static exception_t timer_c6748_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
	{
		timer_c6748_device *dev = opaque->obj;
		uint32_t *data = (uint32_t *)buf;

		switch (offset) {
			case REVID:
				*data = dev->regs.revid;
				break;
			case EMUMGT:
				*data = dev->regs.emumgt;
				break;
			case GPINTGPEN:
				*data = dev->regs.gpintgpen;
				break;
			case GPDATGPDIR:
				*data = dev->regs.gpdatgpdir;
				break;
			case TIM12:
				RW_WRLOCK(dev->lock);
				*data = dev->regs.tim12;
				if (BITS(dev->regs.tgcr, 2, 3) == GP_64)
					dev->tim34_shadow = dev->regs.tim34;
				else if (BITS(dev->regs.tgcr, 2, 3) == UNCHAINED_32) {
					if (BITS(dev->regs.tgcr, 4, 4) == 1
							&& BITS(dev->regs.tcr, 10, 10)) {
						dev->regs.cap12 = dev->regs.tim12;
						if (BITS(dev->regs.tcr, 6, 7) == TM_CON_PERIOD)
							dev->regs.prd12 = dev->regs.rel12;
					}
				}
				RW_UNLOCK(dev->lock);
				break;
			case TIM34:
				if (BITS(dev->regs.tgcr, 2, 3) == GP_64) {
					*data = dev->tim34_shadow;
				} else if (BITS(dev->regs.tgcr, 2, 3) == UNCHAINED_32) {
					RW_WRLOCK(dev->lock);
					*data = dev->regs.tim34;
					if (BITS(dev->regs.tgcr, 4, 4) == 1
							&& BITS(dev->regs.tcr, 26, 26)) {
						dev->regs.cap34 = dev->regs.tim34;
						if (BITS(dev->regs.tcr, 22, 23) == TM_CON_PERIOD)
							dev->regs.prd34 = dev->regs.rel34;
					}
					RW_UNLOCK(dev->lock);
				} else {
					RW_RDLOCK(dev->lock);
					*data = dev->regs.tim34;
					RW_UNLOCK(dev->lock);
				}
				break;
			case PRD12:
				*data = dev->regs.prd12;
				break;
			case PRD34:
				*data = dev->regs.prd34;
				break;
			case TCR:
				*data = dev->regs.tcr;
				break;
			case TGCR:
				*data = dev->regs.tgcr;
				break;
			case WDTCR:
				*data = dev->regs.wdtcr;
				break;
			case REL12:
				*data = dev->regs.rel12;
				break;
			case REL34:
				*data = dev->regs.rel34;
				break;
			case CAP12:
				*data = dev->regs.cap12;
				break;
			case CAP34:
				*data = dev->regs.cap34;
				break;
			case INTCTLSTAT:
				*data = dev->regs.intctlstat;
				break;
			case CMP0:
				*data = dev->regs.cmp0;
				break;
			case CMP1:
				*data = dev->regs.cmp1;
				break;
			case CMP2:
				*data = dev->regs.cmp2;
				break;
			case CMP3:
				*data = dev->regs.cmp3;
				break;
			case CMP4:
				*data = dev->regs.cmp4;
				break;
			case CMP5:
				*data = dev->regs.cmp5;
				break;
			case CMP6:
				*data = dev->regs.cmp6;
				break;
			case CMP7:
				*data = dev->regs.cmp7;
				break;
			default:
				skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in timer_c6748\n", offset);
				break;
		}
		//printf("%d %s offset = 0x%x value = 0x%x\n",__LINE__,__func__,offset,*data);
		return No_exp;
	}

	static exception_t timer_c6748_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
	{
		timer_c6748_device *dev = opaque->obj;
		uint32_t data = *(uint32_t *)buf;

		//printf("%d %s offset = 0x%x value = 0x%x\n",__LINE__,__func__,offset,data);
		switch (offset) {
			case EMUMGT:
				dev->regs.emumgt = data;
				break;
			case GPINTGPEN:
				dev->regs.gpintgpen = data;
				break;
			case GPDATGPDIR:
				dev->regs.gpdatgpdir = data;
				break;
			case TIM12:
				dev->regs.tim12 = data;
				break;
			case TIM34:
				dev->regs.tim34 = data;
				break;
			case PRD12:
				dev->regs.prd12 = data;
				break;
			case PRD34:
				dev->regs.prd34 = data;
				break;
			case TCR:
				dev->old_tcr = dev->regs.tcr;
				dev->regs.tcr = data;
				do_with_tcr(opaque);
				break;
			case TGCR:
				dev->regs.tgcr = data;
				break;
			case WDTCR:
				dev->regs.wdtcr = data;
				if (BITS(dev->regs.tgcr, 2, 3) == WDOG_64
						&& BITS(dev->regs.wdtcr, 14, 14))
					do_with_wdtcr(opaque);
				if ((data >> 16) == 0xda7e) {
					RW_WRLOCK(dev->lock);
					dev->regs.tim64 = 0;
					RW_UNLOCK(dev->lock);
				}
				break;
			case REL12:
				dev->regs.rel12 = data;
				break;
			case REL34:
				dev->regs.rel34 = data;
				break;
			case CAP12:
				dev->regs.cap12 = data;
				break;
			case CAP34:
				dev->regs.cap34 = data;
				break;
			case INTCTLSTAT:
				do_with_intctlstat(dev, data);
				break;
			case CMP0:
				dev->regs.cmp0 = data;
				break;
			case CMP1:
				dev->regs.cmp1 = data;
				break;
			case CMP2:
				dev->regs.cmp2 = data;
				break;
			case CMP3:
				dev->regs.cmp3 = data;
				break;
			case CMP4:
				dev->regs.cmp4 = data;
				break;
			case CMP5:
				dev->regs.cmp5 = data;
				break;
			case CMP6:
				dev->regs.cmp6 = data;
				break;
			case CMP7:
				dev->regs.cmp7 = data;
				break;
			default:
				skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in timer_c6748\n", offset);
				break;
		}
		return No_exp;
	}

	static char* timer_c6748_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
	{
		return regs_name[id];
	}

	static uint32_t timer_c6748_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
		timer_c6748_device *dev = conf_obj->obj;
		uint32_t* regs_value = (uint32_t*)&(dev->regs) + id;
		return *regs_value;
	}

	static exception_t timer_c6748_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
	{
		timer_c6748_device *dev = conf_obj->obj;
		struct registers *regs = &(dev->regs);
		*((uint32_t*)regs + id) = value;

		return No_exp;
	}

	static uint32_t timer_c6748_get_regnum(conf_object_t* conf_obj, uint32_t id)
	{
		return  24;
	}

	static uint32_t timer_c6748_get_regid_by_name(conf_object_t* conf_obj, char *name)
	{
		uint32_t regnum;
		regnum = sizeof(struct registers)/ 4;
		int i;

		for(i =0; i < regnum; i++) {
			if(strcmp(name, regs_name[i]) == 0)
				return i;
		}
		return regnum;
	}

	static exception_t timer_c6748_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
	{
		timer_c6748_device *dev = opaque->obj;
	}

	static conf_object_t* new_timer_c6748(char* obj_name){
		int i;
		timer_c6748_device* dev = skyeye_mm_zero(sizeof(timer_c6748_device));
		dev->obj = new_conf_object(obj_name, dev);

		/* init timer_c6748 regs */
		dev->regs.revid = 0x4472020;
		dev->regs.intctlstat |= 1 << 0;
		dev->regs.intctlstat |= 1 << 16;

		dev->sche_id1 = -1; /* used for timer12 and timer gp64 mode */
		dev->sche_id2 = -1; /* used for timer34 unchained mode */
		dev->sche_id3 = -1; /* used for watchdog */

		dev->handle1 = NULL;
		dev->handle2 = NULL;
		RWLOCK_INIT(dev->lock);

		return dev->obj;
	}
	static exception_t free_timer_c6748(conf_object_t* opaque){
		timer_c6748_device *dev = opaque->obj;

		RWLOCK_DESTROY(dev->lock);
		skyeye_free(opaque->objname);
		skyeye_free(opaque->obj);
		skyeye_free(opaque);

		return No_exp;
	}

	static attr_value_t get_attr_timer_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
		timer_c6748_device *dev = conf_obj->obj;
		attr_value_t timer_number = SKY_make_attr_uinteger(dev->timer_number);
		return timer_number;
	}
	static exception_t set_attr_timer_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
		timer_c6748_device *dev = conf_obj->obj;
		dev->timer_number = SKY_attr_uinteger(*value);
		return No_exp;
	}

	static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
	{
		timer_c6748_device *dev = obj->obj;
		dev->signal = obj2;
		dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);

		if (dev->signal_iface == NULL){
			if (dev->signal == NULL)
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
		timer_c6748_device *dev = obj->obj;
		*obj2 = dev->signal;
		*port = NULL;
		return No_exp;
	}

	static attr_value_t get_attr_external_clock(void* arg, conf_object_t* obj, attr_value_t* idx){
		timer_c6748_device *dev = obj->obj;
		attr_value_t exte_clock = SKY_make_attr_uinteger(dev->exte_clock);
		return exte_clock;
	}
	static exception_t set_attr_extenal_clock(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx){
		timer_c6748_device *dev = obj->obj;
		dev->exte_clock = SKY_attr_uinteger(*value);
		return No_exp;
	}

	static systimer_register_attribute(conf_class_t* clss){
		SKY_register_attribute(clss, "timer_number", get_attr_timer_number, NULL, set_attr_timer_number, NULL, SKY_Attr_Optional, "uinteger", "timer number of timer");
		SKY_register_attribute(clss, "external_clock", get_attr_external_clock, NULL, set_attr_extenal_clock, NULL ,SKY_Attr_Optional, "uinteger", "external clock oscin");
	}

	static exception_t pll0_clock_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
	{
		timer_c6748_device *dev = obj->obj;
		dev->pll0 = obj2;
		dev->pll0_freq_clock = (device_clock_intf *)SKY_get_iface(dev->pll0, DEVICE_CLOCK_INTF);

		if (dev->pll0_freq_clock == NULL){
			if (dev->pll0 == NULL)
			{
				skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", DEVICE_CLOCK_INTF);
			} else
				skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", DEVICE_CLOCK_INTF, obj2->objname);

			return Not_found_exp;
		}
		return No_exp;
	}

	static exception_t pll0_clock_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
	{
		timer_c6748_device *dev = obj->obj;
		*obj2 = dev->pll0;
		*port = NULL;
		return No_exp;
	}

	void init_timer_c6748(){
		static skyeye_class_t class_data = {
			.class_name = "c6748_timer",
			.class_desc = "c6748_timer",
			.new_instance = new_timer_c6748,
			.free_instance = free_timer_c6748,
			.get_attr = NULL,
			.set_attr = timer_c6748_set_attr,
			.module_type = SKYML_ONCHIP(SC_c6748),
		};

		conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
		static const memory_space_intf io_memory = {
			.read = timer_c6748_read,
			.write = timer_c6748_write,
		};
		SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

		static const skyeye_reg_intf reg_intf = {
			.get_regvalue_by_id = timer_c6748_get_regvalue_by_id,
			.get_regname_by_id = timer_c6748_get_regname_by_id,
			.set_regvalue_by_id = timer_c6748_set_regvalue_by_id,
			.get_regnum = timer_c6748_get_regnum,
			.get_regid_by_name = timer_c6748_get_regid_by_name,
		};
		SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

		static const struct InterfaceDescription ifaces[] = {
			(struct InterfaceDescription) {
				.name = MEMORY_SPACE_INTF_NAME,
				.iface = &io_memory,
			},
			(struct InterfaceDescription) {
				.name = SKYEYE_REG_INTF,
				.iface = &reg_intf,
			},
		};
		static const struct ConnectDescription connects[] = {
			(struct ConnectDescription) {
				.name = GENERAL_SIGNAL_INTF_NAME,
				.set = general_signal_set,
				.get = general_signal_get,
			},
			(struct ConnectDescription) {
				.name = DEVICE_CLOCK_INTF,
				.set = pll0_clock_set,
				.get = pll0_clock_get,
			},
		};
		class_register_ifaces(clss, ifaces);
		class_register_connects(clss, connects);

		systimer_register_attribute(clss);
	}
