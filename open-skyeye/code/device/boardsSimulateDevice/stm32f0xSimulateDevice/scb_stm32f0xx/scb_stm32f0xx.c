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
* @file scb_stm32f0xx.c
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

#include "scb_stm32f0xx.h"

static void stm32f0xx_scb_callback(conf_object_t* obj)
{

	scb_stm32f0xx_device *dev = (scb_stm32f0xx_device *)obj;
#if 0
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
#endif
}

static exception_t scb_stm32f0xx_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	scb_stm32f0xx_device *dev = opaque->obj;

	switch (offset) {
		case CPUID:
			*(uint32_t *)buf = dev->regs.cpuid;
			break;
		case ICSR:
			*(uint32_t *)buf = dev->regs.icsr;
			break;
		case AIRCR:
			*(uint32_t *)buf = dev->regs.aircr;
			break;
		case SCR:
			*(uint32_t *)buf = dev->regs.scr;
			break;
		case CCR:
			*(uint32_t *)buf = dev->regs.ccr;
			break;
		case SHPR2:
			*(uint32_t *)buf = dev->regs.shpr2;
			break;
		case SHPR3:
			*(uint32_t *)buf = dev->regs.shpr3;
			break;

		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in scb_stm32f0xx\n", offset);
			break;
	}
	return No_exp;
}

static exception_t scb_stm32f0xx_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	scb_stm32f0xx_device *dev = opaque->obj;

	switch (offset) {
		case CPUID:
			dev->regs.cpuid = *(uint32_t *)buf;
			break;
		case ICSR:
			dev->regs.icsr = *(uint32_t *)buf;
			break;
		case AIRCR:
			dev->regs.aircr = *(uint32_t *)buf;
			break;
		case SCR:
			dev->regs.scr = *(uint32_t *)buf;
			break;
		case CCR:
			dev->regs.ccr = *(uint32_t *)buf;
			break;
		case SHPR2:
			dev->regs.shpr2 = *(uint32_t *)buf;
			break;
		case SHPR3:
			dev->regs.shpr3 = *(uint32_t *)buf;
			break;

		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in scb_stm32f0xx\n", offset);
			break;
	}
	return No_exp;
}

static char* scb_stm32f0xx_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t scb_stm32f0xx_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	scb_stm32f0xx_device *dev = conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)&(dev->regs) + id;
	return *regs_value;
}

static exception_t scb_stm32f0xx_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	scb_stm32f0xx_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;

	return No_exp;
}

static uint32_t scb_stm32f0xx_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t scb_stm32f0xx_get_regid_by_name(conf_object_t* conf_obj, char *name)
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

static conf_object_t* new_scb_stm32f0xx(char* obj_name){
	int i;
	scb_stm32f0xx_device* dev = skyeye_mm_zero(sizeof(scb_stm32f0xx_device));
	dev->obj = new_conf_object(obj_name, dev);

	/* init scb_stm32f0xx regs */
	dev->scheduler_id = -1;

	dev->regs.cpuid	= 0x410cc200;
	dev->regs.icsr	= 0x0;
	dev->regs.aircr	= 0xFA050000;
	dev->regs.scr	= 0x0;
	dev->regs.ccr	= 0x204;
	dev->regs.shpr2	= 0x0;
	dev->regs.shpr3	= 0x0;

	RWLOCK_INIT(dev->rttlock);
	RWLOCK_INIT(dev->wdtlock);
	RWLOCK_INIT(dev->pitlock);

	dev->sig_no = 1;

	return dev->obj;
}
static exception_t free_scb_stm32f0xx(conf_object_t* opaque){
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	scb_stm32f0xx_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	scb_stm32f0xx_device *dev = conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	scb_stm32f0xx_device *dev = obj->obj;
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
	scb_stm32f0xx_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}


static scb_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of scb");
}
void init_scb_stm32f0xx(){
	static skyeye_class_t class_data = {
		.class_name = "scb_stm32f0xx",
		.class_desc = "scb_stm32f0xx",
		.new_instance = new_scb_stm32f0xx,
		.free_instance = free_scb_stm32f0xx,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_OFFCHIP,
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = scb_stm32f0xx_read,
		.write = scb_stm32f0xx_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = scb_stm32f0xx_get_regvalue_by_id,
		.get_regname_by_id = scb_stm32f0xx_get_regname_by_id,
		.set_regvalue_by_id = scb_stm32f0xx_set_regvalue_by_id,
		.get_regnum = scb_stm32f0xx_get_regnum,
		.get_regid_by_name = scb_stm32f0xx_get_regid_by_name,
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

	scb_register_attribute(clss);
}
