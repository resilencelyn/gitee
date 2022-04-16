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
* @file timer_1ms_am3359.c
* @brief The implementation of system controller
* @author zyuming: zyumingfit@gmail.com
* @version 78.77
*/

#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_log.h>
#include <skyeye_attribute.h>
#include "skyeye_thread.h"
#include "timer_1ms_am3359.h"
#define DEBUG

static int is_equal(uint32_t val1, uint32_t val2)
{
	return (val1 & 0xffffff) == (val2 & 0xffffff);
}
static void am3359_timer_1ms_cb(conf_object_t* obj)
{
	timer_1ms_am3359_device *dev = obj->obj;
	int mode = (dev->regs.tclr >> 1) & 0x1;

	RW_WRLOCK(dev->lock);
	if (0xffffffff - dev->regs.tcrr <= INTERVAL) {
		if (mode == 0)
			dev->regs.tcrr = 0;
		else
			dev->regs.tcrr = dev->regs.tldr;

		dev->regs.tocr++;
		if (dev->regs.tier & (1 << 1) && is_equal(dev->regs.tocr, dev->regs.towr)) {
			dev->regs.tocr = 0;
			dev->regs.tisr |= 1 << 1;
			/* raise over flow singal */
			if (dev->signal_iface != NULL && dev->signal != NULL)
				dev->signal_iface->raise_signal(dev->signal,
						dev->interrupt_number);
		}
	} else {
		dev->regs.tcrr += INTERVAL;
		if (dev->regs.tmar + INTERVAL > dev->regs.tcrr) {
			if (dev->regs.tclr & (1 << 6) && (dev->regs.tier & 0x1)) {
				/* raise match signal */
				dev->regs.tisr |= 0x1;
				if (dev->signal_iface != NULL && dev->signal != NULL)
					dev->signal_iface->raise_signal(dev->signal,
							dev->interrupt_number);
			}
		}
	}
	RW_UNLOCK(dev->lock);
}
static void do_tclr_action(conf_object_t *opaque)
{
	timer_1ms_am3359_device *dev = opaque->obj;
	int start = dev->regs.tclr & 1;
#if 0
	int pre = (dev->regs.tclr >> 5) & 0x1;
	int ptv = (dev->regs.tclr >> 2) & 0x3;
	int divisor = pre == 0 ? 1 : 2 * (ptv + 1);
#endif
	int mode = (dev->regs.tclr >> 1) & 0x1 ? Periodic_sched : Oneshot_sched;
	unsigned int ms = 1;

	if (start) {
		if (dev->scheduler_id == -1)
			create_thread_scheduler(ms, mode, am3359_timer_1ms_cb,
					(void *)opaque, &dev->scheduler_id);
		else
			mod_thread_scheduler(dev->scheduler_id, ms, mode);
	} else if (start == 0) {
		if (dev->scheduler_id != -1) {
			del_thread_scheduler(dev->scheduler_id);
			dev->scheduler_id = -1;
		}
	}
}
static void do_soft_reset(conf_object_t *opaque)
{
	timer_1ms_am3359_device *dev = opaque->obj;
	memset(&dev->regs, 0, sizeof(dev->regs));

	dev->regs.tidr = 0x15;
	dev->regs.ttgr = 0xffffffff;

	if (dev->scheduler_id != -1) {
		del_thread_scheduler(dev->scheduler_id);
		dev->scheduler_id = -1;
	}
}
static exception_t timer_1ms_am3359_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	timer_1ms_am3359_device *dev = opaque->obj;
	uint32_t *val = (uint32_t *)buf;

	switch (offset) {
		case DMTIMER_1MS_TIDR:
			*val = dev->regs.tidr;
			break;
		case DMTIMER_1MS_TIOCP_CFG:
			*val = dev->regs.tiocp_cfg & ~(1 << 1);
			break;
		case DMTIMER_1MS_TISTAT:
			*val = dev->regs.tistat;
			break;
		case DMTIMER_1MS_TISR:
			*val = dev->regs.tisr;
			break;
		case DMTIMER_1MS_TIER:
			*val = dev->regs.tier;
			break;
		case DMTIMER_1MS_TWER:
			*val = dev->regs.twer;
			break;
		case DMTIMER_1MS_TCLR:
			*val = dev->regs.tclr;
			break;
		case DMTIMER_1MS_TCRR:
			*val = dev->regs.tcrr;
			break;
		case DMTIMER_1MS_TLDR:
			*val = dev->regs.tldr;
			break;
		case DMTIMER_1MS_TTGR:
			*val = 0xffffffff;
			break;
		case DMTIMER_1MS_TWPS:
			*val = dev->regs.twps;
			break;
		case DMTIMER_1MS_TMAR:
			*val = dev->regs.tmar;
			break;
		case DMTIMER_1MS_TCAR1:
			*val = dev->regs.tcar1;
			break;
		case DMTIMER_1MS_TSICR:
			*val = dev->regs.tsicr;
			break;
		case DMTIMER_1MS_TCAR2:
			*val = dev->regs.tcar2;
			break;
		case DMTIMER_1MS_TPIR:
			*val = dev->regs.tpir;
			break;
		case DMTIMER_1MS_TNIR:
			*val = dev->regs.tnir;
			break;
		case DMTIMER_1MS_TCVR:
			*val = dev->regs.tcvr;
			break;
		case DMTIMER_1MS_TOCR:
			*val = dev->regs.tocr;
			break;
		case DMTIMER_1MS_TOWR:
			*val = dev->regs.towr;
			break;
		default:
			skyeye_log(Warning_log, __FUNCTION__, "Can not read the register at 0x%x in %s\n", offset, __FILE__);
			break;
	}

	return No_exp;
}
static exception_t timer_1ms_am3359_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	timer_1ms_am3359_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf, oldtcrr;

	switch(offset) {
		case DMTIMER_1MS_TIOCP_CFG:
			dev->regs.tiocp_cfg = val;
			if (val & 0x1 && dev->regs.tsicr & (1 << 1)) {
				do_soft_reset(opaque);
				dev->regs.tistat |= 1;
			}
			break;
		case DMTIMER_1MS_TISR:
			dev->regs.tisr &= val;
			break;
		case DMTIMER_1MS_TIER:
			dev->regs.tier = val;
			break;
		case DMTIMER_1MS_TWER:
			dev->regs.twer = val;
			break;
		case DMTIMER_1MS_TCLR:
			dev->regs.tclr = val;
			do_tclr_action(opaque);
			break;
		case DMTIMER_1MS_TCRR:
			RW_WRLOCK(dev->lock);
			oldtcrr = dev->regs.tcrr;
			dev->regs.tcrr = val;
			RW_UNLOCK(dev->lock);

			/* only write restart with a new val */
			if (oldtcrr != val)
				do_tclr_action(opaque);
			break;
		case DMTIMER_1MS_TLDR:
			RW_WRLOCK(dev->lock);
			dev->regs.tldr = val;
			RW_UNLOCK(dev->lock);
			break;
		case DMTIMER_1MS_TTGR:
			dev->regs.ttgr = val;
			RW_WRLOCK(dev->lock);
			dev->regs.tcrr = dev->regs.tldr;
			RW_UNLOCK(dev->lock);
			break;
		case DMTIMER_1MS_TMAR:
			RW_WRLOCK(dev->lock);
			dev->regs.tmar = val;
			RW_UNLOCK(dev->lock);
			break;
		case DMTIMER_1MS_TSICR:
			dev->regs.tsicr = val;
			break;
		case DMTIMER_1MS_TCAR2:
			dev->regs.tcar2 = val;
			break;
		case DMTIMER_1MS_TPIR:
			dev->regs.tpir = val;
			break;
		case DMTIMER_1MS_TNIR:
			dev->regs.tnir = val;
			break;
		case DMTIMER_1MS_TCVR:
			dev->regs.tcvr = val;
			break;
		case DMTIMER_1MS_TOCR:
			dev->regs.tocr = val;
			break;
		case DMTIMER_1MS_TOWR:
			dev->regs.towr = val;
			break;
		default:
			skyeye_log(Warning_log, __FUNCTION__, "Can not write the register at 0x%x in %s\n", offset, __FILE__);
			break;
	}

	return No_exp;
}
static char* timer_1ms_am3359_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}
static int32_t timer_1ms_am3359_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	timer_1ms_am3359_device *dev = conf_obj->obj;
	uint32_t *regs_value = (uint32_t*)&(dev->regs) + id;
	return *regs_value;
}

static exception_t timer_1ms_am3359_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	timer_1ms_am3359_device *dev = conf_obj->obj;
	struct registers *regs = &dev->regs;
	*((uint32_t*)regs + id) = value;

	return No_exp;
}

static uint32_t timer_1ms_am3359_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	timer_1ms_am3359_device *dev = conf_obj->obj;
	return  sizeof(struct registers) / 4;
}

static uint32_t timer_1ms_am3359_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	timer_1ms_am3359_device *dev = conf_obj->obj;
	struct registers *regs = &dev->regs;
	uint32_t regnum = sizeof(struct registers) / 4;
	int i;

	for(i =0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_timer_1ms_am3359(char* obj_name) {
	timer_1ms_am3359_device* dev = skyeye_mm_zero(sizeof(timer_1ms_am3359_device));
	dev->obj = new_conf_object(obj_name, dev);

	dev->scheduler_id = -1;

	dev->regs.tidr = 0x15;
	dev->regs.ttgr = 0xffffffff;
	dev->regs.tistat |= 1;

	RWLOCK_INIT(dev->lock);

	return dev->obj;
}
static exception_t free_timer_1ms_am3359(conf_object_t *opaque) {
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}
static exception_t reset_am3359_timer_1ms(conf_object_t *opaque, const char *args)
{
	timer_1ms_am3359_device* dev = opaque->obj;

	memset(&dev->regs, 0, sizeof(dev->regs));

	dev->regs.tidr = 0x15;
	dev->regs.ttgr = 0xffffffff;
	dev->regs.tistat |= 1;

	return No_exp;
}
static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	timer_1ms_am3359_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);
	if (dev->signal_iface == NULL){
		skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.",
				GENERAL_SIGNAL_INTF_NAME, obj2->objname);
	}
	return No_exp;
}
static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	timer_1ms_am3359_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}
static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	timer_1ms_am3359_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj,
		attr_value_t* value, attr_value_t* idx)
{
	timer_1ms_am3359_device *dev = conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}
static void timer_register_attribute(conf_class_t* clss) {
	SKY_register_attribute(clss, "int_number", get_attr_int_number,
			NULL, set_attr_int_number, NULL, SKY_Attr_Optional,
			"uinteger", "interrupt number of timer");
}
void init_timer_1ms_am3359(void){
	static skyeye_class_t class_data = {
		.class_name = "am3359_timer_1ms",
		.class_desc = "am3359_timer_1ms",
		.new_instance = new_timer_1ms_am3359,
		.free_instance = free_timer_1ms_am3359,
		.reset_instance = reset_am3359_timer_1ms,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_am335x SC_am3359),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = timer_1ms_am3359_read,
		.write = timer_1ms_am3359_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = timer_1ms_am3359_get_regvalue_by_id,
		.get_regname_by_id = timer_1ms_am3359_get_regname_by_id,
		.set_regvalue_by_id = timer_1ms_am3359_set_regvalue_by_id,
		.get_regnum = timer_1ms_am3359_get_regnum,
		.get_regid_by_name = timer_1ms_am3359_get_regid_by_name,
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
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	timer_register_attribute(clss);
}
