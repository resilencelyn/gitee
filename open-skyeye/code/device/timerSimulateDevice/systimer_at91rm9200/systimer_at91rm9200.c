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
* @file systimer_at91rm9200.c
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

#include "systimer_at91rm9200.h"

static void at91rm9200x_systimer_callback(conf_object_t* obj)
{

	systimer_at91rm9200_device *dev = (systimer_at91rm9200_device *)obj;

	RW_WRLOCK(dev->pitlock);
	dev->regs.st_sr |= 1 << 0;

	if (dev->signal_iface != NULL && dev->signal != NULL) {
		if (dev->regs.st_imr & 1) {
			/* raise signal */
			if (dev->signal_iface != NULL) {
				dev->signal_iface->raise_signal(dev->signal, dev->sig_no);
			}
		}
	}

	dev->st_piv_dc = dev->regs.st_pimr & 0xffff;
	if (dev->st_piv_dc == 0)
		dev->st_piv_dc = 65536;
	RW_UNLOCK(dev->pitlock);

}
static void at91rm9200x_syswdt_callback(conf_object_t* obj)
{

	systimer_at91rm9200_device *dev = (systimer_at91rm9200_device *)obj;

	RW_WRLOCK(dev->rttlock);
	if ((dev->st_wdv_dc -= 1) <= 0) {
		dev->regs.st_sr |= 1 << 1;

		if (dev->signal_iface != NULL && dev->signal != NULL) {
			if (dev->regs.st_wdmr & (1 << 16)) {
				if (dev->regs.st_imr & (1 << 1)) {
					/* raise signal to reset*/
					if (dev->signal_iface != NULL)
						dev->signal_iface->raise_signal(dev->signal, dev->sig_no);
				}
			}
		}

		dev->st_wdv_dc = dev->regs.st_wdmr & 0xffff;
		if (dev->st_wdv_dc == 0)
			dev->st_wdv_dc = 65536;
	}
	RW_UNLOCK(dev->rttlock);
}
static void at91rm9200x_sysrtt_callback(conf_object_t *obj)
{
	systimer_at91rm9200_device *dev = (systimer_at91rm9200_device *)obj;
	uint32_t limit = 1 << 20, st_crtr, st_rtar;

	RW_WRLOCK(dev->rttlock);
	st_crtr = dev->regs.st_crtr & 0xfffff;

	if (st_crtr == 0xfffff)
		dev->regs.st_crtr &= ~0xfffff;
	else
		dev->regs.st_crtr += 1;

	dev->regs.st_sr |= 1 << 2;

	st_crtr = dev->regs.st_crtr & 0xfffff;
	if ((st_rtar = dev->regs.st_rtar & 0xfffff) == 0)
		st_rtar = 1 << 20; /* max val is 1048576 */

	if (st_crtr == st_rtar) {
		dev->regs.st_sr |= 1 << 3;
		if (dev->regs.st_imr & (1 << 3)) {
			/* raise signal */
			if (dev->signal_iface != NULL)
				dev->signal_iface->raise_signal(dev->signal, dev->sig_no);
		}
	}
	RW_UNLOCK(dev->rttlock);
}

static void restart_pit(systimer_at91rm9200_device *dev)
{
	unsigned int us, slck = 32768, mode = Periodic_sched;

	RW_WRLOCK(dev->pitlock);
	dev->st_piv_dc = dev->regs.st_pimr & 0xffff;
	if (dev->st_piv_dc == 0)
		dev->st_piv_dc = 65536;

	us = dev->st_piv_dc * 1000 / slck;
	us = us == 0 ? 1000 : us;
	RW_UNLOCK(dev->pitlock);

	if (dev->scheduler_id != -1) {
		mod_thread_scheduler(dev->scheduler_id, us, mode);
		return;
	}

	create_thread_scheduler(us, mode, at91rm9200x_systimer_callback, (void*)dev, &dev->scheduler_id);
}

static void restart_wdt(systimer_at91rm9200_device *dev)
{
	unsigned int us, slck = 32768, div = 128, mode = Periodic_sched;

	RW_WRLOCK(dev->rttlock);
	dev->st_wdv_dc = dev->regs.st_wdmr & 0xffff;
	if (dev->st_wdv_dc == 0)
		dev->st_wdv_dc = 65536;

	us = 1000 * div / slck;
	us = us == 0 ? 1000 : us;
	RW_UNLOCK(dev->rttlock);

	if (dev->wdt_thr_id != -1) {
		mod_thread_scheduler(dev->wdt_thr_id, us, mode);
		return;
	}

	create_thread_scheduler(us, mode, at91rm9200x_syswdt_callback, (void*)dev, &dev->wdt_thr_id);
}

static void restart_rtt(systimer_at91rm9200_device *dev)
{
	uint32_t rtpres, us, slck = 32768, mode = Periodic_sched;

	rtpres = dev->regs.st_rtmr & 0xffff;
	rtpres = rtpres == 0 ? 65536 : rtpres;

	us = 1000 * rtpres / slck;
	us = us == 0 ? 1000 : us;

	if (dev->rtt_thr_id != -1) {

		RW_WRLOCK(dev->rttlock);
		dev->regs.st_crtr &= ~0xfffff;
		RW_UNLOCK(dev->rttlock);

		mod_thread_scheduler(dev->rtt_thr_id, us, mode);
		return;
	}

	create_thread_scheduler(us, mode, at91rm9200x_sysrtt_callback, (void*)dev, &dev->rtt_thr_id);
}

static exception_t systimer_at91rm9200_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	systimer_at91rm9200_device *dev = opaque->obj;

	switch (offset) {
		case ST_PIMR:
			*(uint32_t *)buf = dev->regs.st_pimr;
			break;
		case ST_WDMR:
			*(uint32_t *)buf = dev->regs.st_wdmr;
			break;
		case ST_RTMR:
			*(uint32_t *)buf = dev->regs.st_rtmr;
			break;
		case ST_SR:
			*(uint32_t *)buf = dev->regs.st_sr;
			break;
		case ST_RTAR:
			*(uint32_t *)buf = dev->regs.st_rtar;
			break;
		case ST_CRTR:
			*(uint32_t *)buf = dev->regs.st_crtr;
			break;
		case ST_IMR:
			*(uint32_t *)buf = dev->regs.st_imr;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in systimer_at91rm9200\n", offset);
			break;
	}
	return No_exp;
}

static exception_t systimer_at91rm9200_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	systimer_at91rm9200_device *dev = opaque->obj;

	switch (offset) {
		case ST_CR:
			dev->regs.st_cr = *(uint32_t *)buf;
			restart_wdt(dev);
			break;
		case ST_PIMR:
			dev->regs.st_pimr = *(uint32_t *)buf;
			restart_pit(dev);
			break;
		case ST_WDMR:
			dev->regs.st_wdmr = *(uint32_t *)buf;
			break;
		case ST_RTMR:
			dev->regs.st_rtmr = *(uint32_t *)buf;
			restart_rtt(dev);
			break;
		case ST_IER:
			dev->regs.st_ier = *(uint32_t *)buf;
			dev->regs.st_imr |= *(uint32_t *)buf;
			break;
		case ST_IDR:
			dev->regs.st_idr = *(uint32_t *)buf;
			dev->regs.st_imr &= ~(*(uint32_t *)buf);
			break;
		case ST_RTAR:
			dev->regs.st_rtar = *(uint32_t *)buf;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in systimer_at91rm9200\n", offset);
			break;
	}
	return No_exp;
}

static char* systimer_at91rm9200_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t systimer_at91rm9200_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	systimer_at91rm9200_device *dev = conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)&(dev->regs) + id;
	return *regs_value;
}

static exception_t systimer_at91rm9200_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	systimer_at91rm9200_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;

	return No_exp;
}

static uint32_t systimer_at91rm9200_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t systimer_at91rm9200_get_regid_by_name(conf_object_t* conf_obj, char *name)
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

static exception_t systimer_at91rm9200_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	systimer_at91rm9200_device *dev = opaque->obj;
}

static conf_object_t* new_systimer_at91rm9200(char* obj_name){
	int i;
	systimer_at91rm9200_device* dev = skyeye_mm_zero(sizeof(systimer_at91rm9200_device));
	dev->obj = new_conf_object(obj_name, dev);

	/* init systimer_at91rm9200 regs */
	dev->scheduler_id = -1;
	dev->wdt_thr_id = -1;
	dev->rtt_thr_id = -1;

	dev->regs.st_pimr	= 0x00000000;
	dev->regs.st_wdmr	= 0x00020000;
	dev->regs.st_rtmr 	= 0x00008000;
	dev->regs.st_imr	= 0x00000000;
	dev->regs.st_rtar 	= 0x00000000;
	dev->regs.st_crtr 	= 0x00000000;

	RWLOCK_INIT(dev->rttlock);
	RWLOCK_INIT(dev->wdtlock);
	RWLOCK_INIT(dev->pitlock);

	dev->sig_no = 1;

	return dev->obj;
}
static exception_t free_systimer_at91rm9200(conf_object_t* opaque){
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	systimer_at91rm9200_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	systimer_at91rm9200_device *dev = conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	systimer_at91rm9200_device *dev = obj->obj;
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
	systimer_at91rm9200_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}


static systimer_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of systimer");
}
void init_systimer_at91rm9200(){
	static skyeye_class_t class_data = {
		.class_name = "at91rm9200_systimer",
		.class_desc = "at91rm9200_systimer",
		.new_instance = new_systimer_at91rm9200,
		.free_instance = free_systimer_at91rm9200,
		.get_attr = NULL,
		.set_attr = systimer_at91rm9200_set_attr,
		.module_type = SKYML_ONCHIP(SC_at91rm9200),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = systimer_at91rm9200_read,
		.write = systimer_at91rm9200_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = systimer_at91rm9200_get_regvalue_by_id,
		.get_regname_by_id = systimer_at91rm9200_get_regname_by_id,
		.set_regvalue_by_id = systimer_at91rm9200_set_regvalue_by_id,
		.get_regnum = systimer_at91rm9200_get_regnum,
		.get_regid_by_name = systimer_at91rm9200_get_regid_by_name,
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

	systimer_register_attribute(clss);
}
