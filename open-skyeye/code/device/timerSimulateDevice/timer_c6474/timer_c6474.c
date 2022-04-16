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
* @file timer_c6474.c
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

#include "timer_c6474.h"

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
static void do_signal_raise(timer_c6474_device *dev)
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
static void do_one_shot_gp_64(conf_object_t *opaque)
{
	timer_c6474_device *dev = opaque->obj;
	RW_WRLOCK(dev->lock);
	dev->regs->cnt = 0;
	RW_UNLOCK(dev->lock);

	/* raise signal TINTLO */
	do_signal_raise(dev);
	/* FIXME: edma TEVT12 and output events TM64P_OUT12 */
}
static void do_oneshot_chained_32(conf_object_t *opaque)
{
	timer_c6474_device *dev = opaque->obj;

	RW_WRLOCK(dev->lock);

	dev->regs->cntlo = 0;
	dev->regs->cnthi = 0;

	RW_UNLOCK(dev->lock);

	do_signal_raise(dev);
	/* FIXME: edma TEVTLO and output events TM64P_OUT12 */
}
static void do_one_shot_unchained12_32(conf_object_t *opaque)
{
	timer_c6474_device *dev = opaque->obj;

	RW_WRLOCK(dev->lock);

		/* raise signal TINTLO */
	do_signal_raise(dev);
	/* FIXME: edma TEVT12 and output events TM64P_OUT12 */
	dev->regs->cntlo = 0;

	RW_UNLOCK(dev->lock);

}
static void do_period_gp_64(conf_object_t *opaque)
{
	timer_c6474_device *dev = opaque->obj;
	uint64_t prd64, step;
	int mode = BITS(dev->regs->tcr, 6, 7);

	RW_WRLOCK(dev->lock);

	prd64 = dev->regs->prdhi;
	prd64 |= prd64 << 32 | dev->regs->prdlo;

	if (dev->regs->cnt != 0 || dev->regs->prd != 0) {
		dev->regs->cnt++;
		/* cnt+ is to low, speed it */
		/* dev->regs->cnt++; */
		step = 1 + dev->regs->prd / 50;
		dev->regs->cnt += step;
	}

	/* if (dev->regs->cnt == dev->regs->prd) { */
	if (dev->regs->cnt >= dev->regs->prd - step
	&& dev->regs->cnt <= dev->regs->prd + step) {
		/* raise signal TINTLO */
		do_signal_raise(dev);
		/* FIXME: edma TEVT12 and output events TM64P_OUT12 */
		dev->regs->cnt = 0;
	}

	RW_UNLOCK(dev->lock);
}
static void do_period_chained_32(conf_object_t *opaque)
{
	skyeye_log(Error_log, __FUNCTION__, "get into function: do_period_chained_32\n");
	timer_c6474_device *dev = opaque->obj;
	int mode = BITS(dev->regs->tcr, 6, 7);
	uint32_t step = 1 + dev->regs->prdhi / 50;

	RW_WRLOCK(dev->lock);

	/* cnthi++ is to low, speed it */
	/* dev->regs.cnthi++; */
	dev->regs->cnthi += step;

	/* if (dev->regs.tim34 == dev->regs.prd34) { */
	if (dev->regs->cnthi >= dev->regs->prdhi - step
	&& dev->regs->cnthi <= dev->regs->prdhi + step) {
		skyeye_log(Error_log, __FUNCTION__, " cnthi=prdhi \n");
		if (dev->regs->cntlo != 0 || dev->regs->prdlo != 0)
			skyeye_log(Error_log, __FUNCTION__, "cntlo != 0 || prdlo != 0 \n");
			skyeye_log(Error_log, __FUNCTION__, "the value of cntlo is: %x\n", dev->regs->cntlo);
			skyeye_log(Error_log, __FUNCTION__, "the value of prdlo is: %x\n", dev->regs->prdlo);
			dev->regs->cntlo++;
		if (dev->regs->cntlo == dev->regs->prdlo) {
			skyeye_log(Error_log, __FUNCTION__, " cntlo=prdlo \n");
			/* raise signal TINTLO */
			do_signal_raise(dev);
			/* FIXME: edma TEVT12 and output events TM64P_OUT12 */
			dev->regs->cntlo = 0;
		}
		dev->regs->cnthi = 0;
	}

	RW_UNLOCK(dev->lock);
}
static void do_period_unchained12_32(conf_object_t *opaque)
{
	timer_c6474_device *dev = opaque->obj;
	int mode = BITS(dev->regs->tcr, 6, 7);
	uint32_t *p, i, step;

	RW_WRLOCK(dev->lock);

	if (dev->regs->cntlo != 0 || dev->regs->prdlo != 0) {
		/* cntlo++ is to low, speed it */
		/* dev->regs->cntlo++; */
		step = 1 + dev->regs->prdlo / 50;
		dev->regs->cntlo += step;
	}

	/* if (dev->regs->cntlo >= dev->regs->prdlo) { */
	if (dev->regs->cntlo >= dev->regs->prdlo - step
	&& dev->regs->cntlo <= dev->regs->prdlo + step) {
		/* raise signal TINTLO */
		do_signal_raise(dev);
		/* FIXME: edma TEVT12 and output events TM64P_OUT12 */
		dev->regs->cntlo = 0;
	}

	RW_UNLOCK(dev->lock);
}
static void unchained34_periodic_cb(conf_object_t *opaque)
{
	timer_c6474_device *dev = opaque->obj;
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
	timer_c6474_device *dev = opaque->obj;

	RW_WRLOCK(dev->lock);

	dev->regs->tgcr = SET_BITS(dev->regs->tgcr, 12, 15, 0);
	dev->regs->cnthi = 0;

	RW_UNLOCK(dev->lock);

	/* raise signal TINTHI */
	do_signal_raise(dev);
	/* FIXME: edma TEVT34 */
}
static void one_shot_cb(conf_object_t *opaque)
{
	timer_c6474_device *dev = opaque->obj;
	switch (BITS(dev->regs->tgcr, 2, 3)) {
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
}
static void con_period_cb(conf_object_t *opaque)
{
	timer_c6474_device *dev = opaque->obj;

	switch (BITS(dev->regs->tgcr, 2, 3)) {
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
static void watchdog_periodic_cb(conf_object_t *opaque)
{
	timer_c6474_device *dev = opaque->obj;

	if (++dev->regs->cnt == dev->regs->prd) {
		do_signal_raise(dev);
		/* FIXME: generate watchdog timeout event */
		dev->regs->wdtcr &= ~(1 << 14);
		dev->regs->wdtcr |= 1 << 15;
	}
}
static exception_t do_with_tcr(conf_object_t *opaque)
{
	timer_c6474_device *dev = opaque->obj;
	timer_c6474_reg_t* regs = dev->regs;
	uint32_t ms = 1, mode12 = BITS(regs->tcr, 6, 7);
	uint32_t oldmode12 = BITS(dev->old_tcr, 6, 7);
	uint32_t mode34 = BITS(regs->tcr, 22, 23);
	uint32_t oldmode34 = BITS(dev->old_tcr, 22, 23);
	uint32_t psc34 = BITS(regs->tgcr, 8, 11);
	uint32_t freq = 75;

	/* TCR[ENAMODE12] */
	switch (mode12) {
		case TM_DISABLE:
			if (dev->sche_id1 != -1) {
				del_thread_scheduler(dev->sche_id1);
				dev->sche_id1 = -1;
			}
			break;
		case TM_ONESHOT:
			if (BITS(regs->tgcr, 2, 3) == GP_64) {
				ms = 1000 * regs->prd / (freq * 1000000);
				ms = ms == 0 ? 1000 : ms;
			} else if (BITS(regs->tgcr, 2, 3) == CHAINED_32) {
				ms = 1000 * regs->prdhi * regs->prdlo / (freq * 1000000);
				ms = ms == 0 ? 1000 : ms;
			}
			if (dev->sche_id1 != -1) {
				if (mode12 != oldmode12) {
					del_thread_scheduler(dev->sche_id1);
					create_thread_scheduler(ms, Oneshot_sched, one_shot_cb, (void *)opaque, &dev->sche_id1);
				} else {
					mod_thread_scheduler(dev->sche_id1, ms, Oneshot_sched);
				}
				break;
			}
			create_thread_scheduler(ms, Oneshot_sched, one_shot_cb, (void *)opaque, &dev->sche_id1);
			break;
		case TM_CONTINU:
		case TM_CON_PERIOD:
			ms = 1000 / (freq * 1000000);
			ms = ms == 0 ? 1000: ms;
			if (dev->sche_id1 != -1) {
				if (mode12 != oldmode12) {
					del_thread_scheduler(dev->sche_id1);
					create_thread_scheduler(ms, Periodic_sched, con_period_cb, (void *)opaque, &dev->sche_id1);
				} else {
					mod_thread_scheduler(dev->sche_id1, ms, Periodic_sched);
				}
				break;
			}
			create_thread_scheduler(ms, Periodic_sched, con_period_cb, (void *)opaque, &dev->sche_id1);
			break;
	}

	if (BITS(regs->tgcr, 2, 3) == UNCHAINED_32) {
		switch (mode34) {
			case TM_DISABLE:
				if (dev->sche_id2 != -1) {
					del_thread_scheduler(dev->sche_id2);
					dev->sche_id2 = -1;
				}
				break;
			case TM_ONESHOT:
				ms = 1000 * psc34 * regs->cnthi / (freq * 1000000);
				ms = ms == 0 ? 1 : ms;
				if (dev->sche_id2 != -1) {
					if (mode34 != oldmode34) {
						del_thread_scheduler(dev->sche_id2);
						create_thread_scheduler(ms, Oneshot_sched, unchained34_oneshot_cb, (void *)opaque, &dev->sche_id2);
					} else {
						mod_thread_scheduler(dev->sche_id2, ms, Oneshot_sched);
					}
					break;
				}
				create_thread_scheduler(ms, Oneshot_sched, unchained34_oneshot_cb, (void *)opaque, &dev->sche_id2);
				break;
			case TM_CONTINU:
			case TM_CON_PERIOD:
				ms = 1000 * psc34 * regs->cnthi / (freq * 1000000);
				ms = ms == 0 ? 1 : ms;
				if (dev->sche_id2 != -1) {
					if (mode34 != oldmode34) {
						del_thread_scheduler(dev->sche_id2);
						create_thread_scheduler(ms, Periodic_sched, unchained34_periodic_cb, (void *)opaque, &dev->sche_id2);
					} else {
						mod_thread_scheduler(dev->sche_id2, ms, Periodic_sched);
					}
					break;
				}
				create_thread_scheduler(ms, Periodic_sched, unchained34_periodic_cb, (void *)opaque, &dev->sche_id2);
				break;
		}
	}
	return No_exp;
}
static exception_t do_with_wdtcr(conf_object_t *opaque)
{
	timer_c6474_device *dev = opaque->obj;

	if (dev->sche_id3 != -1)
		return No_exp;

	create_thread_scheduler(1000, Periodic_sched, watchdog_periodic_cb, (void *)opaque, &dev->sche_id3);
	return No_exp;
}
static exception_t timer_c6474_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	timer_c6474_device *dev = opaque->obj;
	timer_c6474_reg_t* regs = dev->regs;
	uint32_t *data = (uint32_t *)buf;
	int time_mode = BITS(regs->tgcr, 2, 3);
	switch(offset) {
		case CLKSPD:
			*data = regs->clkspd;
			break;
		case CNTLO:
			*data = regs->cntlo;
			if (time_mode == GP_64)
				dev->cnthi_shadow = regs->cnthi;
			break;
		case CNTHI:
			if (time_mode == GP_64)
				*data = dev->cnthi_shadow;
			else if (time_mode == UNCHAINED_32){
				if (regs->tgcr & 0x2)	
					*data = regs->cnthi;
			}
			break;
		case PRDLO:
			*data = regs->prdlo;
			break;
		case PRDHI:
			*data = regs->prdhi;
			break;
		case TCR:
			dev->old_tcr = regs->tcr;
			*data = regs->tcr;
			break;
		case TGCR:
			*data = regs->tgcr;
			break;
		case WDTCR:
			*data = regs->wdtcr;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in timer_c6474\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

static exception_t timer_c6474_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	timer_c6474_device *dev = opaque->obj;
	timer_c6474_reg_t* regs = dev->regs;
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
		regs->tcr = val;
		do_with_tcr(opaque);
		break;
	case TGCR:
		regs->tgcr = val;
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
	default:
		skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in timer_c6474\n", offset);
		return Invarg_exp;
	}
	return No_exp;
}

static char* timer_c6474_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t timer_c6474_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	timer_c6474_device *dev = conf_obj->obj;
	timer_c6474_reg_t* regs = dev->regs;
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static char* timer_c6474_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	timer_c6474_device *dev = conf_obj->obj;
	timer_c6474_reg_t* regs = dev->regs;
	*((uint32_t*)regs + id) = value;

	return NULL;
}

static uint32_t timer_c6474_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	timer_c6474_device *dev = conf_obj->obj;
	timer_c6474_reg_t* regs = dev->regs;
	// return  sizeof(timer_c6474_reg_t) / 4;
    return 8;
}

static uint32_t timer_c6474_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	timer_c6474_device *dev = conf_obj->obj;
	timer_c6474_reg_t* regs = dev->regs;
	uint32_t regnum;
	regnum = sizeof(timer_c6474_reg_t)/ 4;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}

	return regnum;
}

static conf_object_t* new_timer_c6474(char* obj_name){
	timer_c6474_device* dev = skyeye_mm_zero(sizeof(timer_c6474_device));
	timer_c6474_reg_t* regs =  skyeye_mm_zero(sizeof(timer_c6474_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init timer_c6474 regs */
	dev->regs = regs;

	dev->sche_id1 = -1;
	dev->sche_id2 = -1;
	dev->sche_id3 = -1;
	RWLOCK_INIT(dev->lock);

	return dev->obj;
}
static void free_timer_c6474(conf_object_t* conf_obj){
	timer_c6474_device* dev = conf_obj->obj;
	RWLOCK_DESTROY(dev->lock);
	skyeye_free(dev);
	skyeye_free(conf_obj);

	return;	
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	timer_c6474_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->int_number);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	timer_c6474_device *dev = conf_obj->obj;
	dev->int_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static timer_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of timer");
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	timer_c6474_device *dev = obj->obj;
	dev->core = obj2;
	dev->core_signal = (core_signal_intf *)SKY_get_iface(dev->core, CORE_SIGNAL_INTF_NAME);

	if (dev->core_signal == NULL){
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
	timer_c6474_device *dev = obj->obj;
	*obj2 = dev->core;
	*port = NULL;
	return No_exp;
}

void init_timer_c6474(){
	static skyeye_class_t class_data = {
		.class_name = "c6474_timer",
		.class_desc = "c6474_timer",
		.new_instance = new_timer_c6474,
		.free_instance = free_timer_c6474,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_c6474),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = timer_c6474_read,
		.write = timer_c6474_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = timer_c6474_get_regvalue_by_id,
		.get_regname_by_id = timer_c6474_get_regname_by_id,
		.set_regvalue_by_id = timer_c6474_set_regvalue_by_id,
		.get_regnum = timer_c6474_get_regnum,
		.get_regid_by_name = timer_c6474_get_regid_by_name,
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

	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	timer_register_attribute(clss);
}
