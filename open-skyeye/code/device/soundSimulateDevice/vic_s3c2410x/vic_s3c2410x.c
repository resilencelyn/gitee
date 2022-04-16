/* Copyright (C)
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundatdev->; either version 2
* of the License, or (at your optdev->) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundatdev->, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*/
/**
* @file vic_s3c2410x.c
* @brief The implementatdev-> of system controller
* @author
* @versdev-> 78.77
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

#include "vic_s3c2410x.h"

static int vic_s3c2410x_raise_signal(conf_object_t *opaque, int line){
	vic_s3c2410x_device *dev = opaque->obj;
	interrupt_signal_t interrupt_signal;
	int res = 1 << line;

	switch (res) {
		case INT_ADC:
			dev->regs.subsrcpnd |= SUB_INT_ADC;
			break;
		case INT_UART0:
			dev->regs.subsrcpnd |= SUB_INT_RXD0;
			dev->regs.subsrcpnd |= SUB_INT_TXD0;
			dev->regs.subsrcpnd |= SUB_INT_ERR0;
			dev->regs.subsrcpnd &= ~(dev->regs.intsubmsk);
			break;
		case INT_UART1:
			dev->regs.subsrcpnd |= SUB_INT_RXD1;
			dev->regs.subsrcpnd |= SUB_INT_TXD1;
			dev->regs.subsrcpnd |= SUB_INT_ERR1;
			break;
		case INT_UART2:
			dev->regs.subsrcpnd |= SUB_INT_RXD2;
			dev->regs.subsrcpnd |= SUB_INT_TXD2;
			dev->regs.subsrcpnd |= SUB_INT_ERR2;
			break;
		default:
			break;
	}

	dev->regs.srcpnd |= res;

	dev->regs.srcpnd &= ~(dev->regs.intmsk);
	if (dev->regs.intmsk & res)
		return No_exp;

	if (dev->regs.intmod & res) {
		if (dev->regs.intmod & ~(res)) {
			printf("line: %d\tfunc: %s\tonly one firq bit can be set, intmod: 0x%p\n",
					__LINE__, __FUNCTION__, dev->regs.intmod);
			return No_exp;
		}
		/*firq*/
		interrupt_signal.arm_signal.firq = Low_level;
		interrupt_signal.arm_signal.irq = Prev_level;

		interrupt_signal.arm_signal.reset = Prev_level;
		if (dev->signal_iface)
			dev->signal_iface->signal(dev->signal, &interrupt_signal);
	} else {
		/*in single process, can ignore priority register?*/
		dev->regs.intpnd = res;
		dev->regs.intoffset = line;

		/*irq*/
		interrupt_signal.arm_signal.firq = Prev_level;
		interrupt_signal.arm_signal.irq = Low_level;

		interrupt_signal.arm_signal.reset = Prev_level;
		if (dev->signal_iface)
			dev->signal_iface->signal(dev->signal, &interrupt_signal);
	}

	return No_exp;
}

static int vic_s3c2410x_lower_signal(conf_object_t *opaque, int line)
{
	struct vic_s3c2410x_device *dev = opaque->obj;
	return No_exp;
}


static exception_t vic_s3c2410x_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	vic_s3c2410x_device *dev = opaque->obj;

	switch (offset) {
		case SRCPND:
			*(uint32_t *)buf = dev->regs.srcpnd;
			break;
		case INTMOD:
			*(uint32_t *)buf = dev->regs.intmod;
			break;
		case INTMSK:
			*(uint32_t *)buf = dev->regs.intmsk;
			break;
		case PRIORITY:
			*(uint32_t *)buf = dev->regs.priority;
			break;
		case INTPND:
			*(uint32_t *)buf = dev->regs.intpnd;
			break;
		case INTOFFSET:
			*(uint32_t *)buf = dev->regs.intoffset;
			break;
		case SUBSRCPND:
			*(uint32_t *)buf = dev->regs.subsrcpnd;
			break;
		case INTSUBMSK:
			*(uint32_t *)buf = dev->regs.intsubmsk;
			break;
		default:
			break;
	}
	return No_exp;
}

static exception_t vic_s3c2410x_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	vic_s3c2410x_device *dev = opaque->obj;
	uint32_t val;

	switch (offset) {
		case SRCPND:
			val = *(uint32_t *)buf;
			dev->regs.srcpnd &= ~val;
			dev->regs.intoffset = 0x00;
			break;
		case INTMOD:
			dev->regs.intmod = *(uint32_t *)buf;
			break;
		case INTMSK:
			dev->regs.intmsk = *(uint32_t *)buf;
			break;
		case PRIORITY:
			dev->regs.priority = *(uint32_t *)buf;
			break;
		case INTPND:
			val = *(uint32_t *)buf;
			dev->regs.intpnd &= ~val;
			dev->regs.intoffset = 0x00;
			break;
		case SUBSRCPND:
			val = *(uint32_t *)buf;
			dev->regs.subsrcpnd &= ~val;
			break;
		case INTSUBMSK:
			dev->regs.intsubmsk = *(uint32_t *)buf;
			break;
		default:
			break;
	}
	return No_exp;
}

static char* vic_s3c2410x_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t vic_s3c2410x_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	vic_s3c2410x_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;

	return No_exp;
}

static uint32_t vic_s3c2410x_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t vic_s3c2410x_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers)/ 4;
	int i;

	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t vic_s3c2410x_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id) {
	vic_s3c2410x_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static exception_t vic_s3c2410x_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	vic_s3c2410x_device *dev = opaque->obj;
}

static conf_object_t* new_vic_s3c2410x(char* obj_name){
	vic_s3c2410x_device* dev = skyeye_mm_zero(sizeof(vic_s3c2410x_device));
	dev->obj = new_conf_object(obj_name, dev);

	dev->regs.srcpnd = 0x0;
	dev->regs.intmod = 0x0;
	dev->regs.intmsk = 0xffffffff;
	dev->regs.priority = 0x7f;
	dev->regs.intpnd = 0x0;
	dev->regs.intoffset = 0x0;
	dev->regs.subsrcpnd = 0x0;
	dev->regs.intsubmsk = 0x7ff;

	return dev->obj;
}
static exception_t free_vic_s3c2410x(conf_object_t* opaque) {
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx) {
	vic_s3c2410x_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(0);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	vic_s3c2410x_device *dev = conf_obj->obj;
	return No_exp;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	vic_s3c2410x_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (core_signal_intf *)SKY_get_iface(dev->signal, CORE_SIGNAL_INTF_NAME);
	if(dev->signal_iface == NULL) {
		if (dev->signal == NULL)
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", CORE_SIGNAL_INTF_NAME);
		else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", CORE_SIGNAL_INTF_NAME, obj2->objname);
	}
	return No_exp;
}

static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	vic_s3c2410x_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}
static vic_register_attribute(conf_class_t* clss) {
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of timer");
}
void init_vic_s3c2410x(){
	static skyeye_class_t class_data = {
		.class_name = "s3c2410x_vic",
		.class_desc = "s3c2410x_vic",
		.new_instance = new_vic_s3c2410x,
		.free_instance = free_vic_s3c2410x,
		.get_attr = NULL,
		.set_attr = vic_s3c2410x_set_attr,
		.module_type = SKYML_ONCHIP(SC_s3c2410x1),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = vic_s3c2410x_read,
		.write = vic_s3c2410x_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = vic_s3c2410x_get_regvalue_by_id,
		.get_regname_by_id = vic_s3c2410x_get_regname_by_id,
		.set_regvalue_by_id = vic_s3c2410x_set_regvalue_by_id,
		.get_regnum = vic_s3c2410x_get_regnum,
		.get_regid_by_name = vic_s3c2410x_get_regid_by_name,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const general_signal_intf signal_intf = {
		.raise_signal = vic_s3c2410x_raise_signal,
		.lower_signal = vic_s3c2410x_lower_signal,
	};
	SKY_register_iface(clss, GENERAL_SIGNAL_INTF_NAME, &signal_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = GENERAL_SIGNAL_INTF_NAME,
			.iface = &signal_intf,
		},
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
		(struct ConnectDescription ){
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);


	vic_register_attribute(clss);
}
