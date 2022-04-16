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
* @file vic_s3c6410.c
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

#include "vic_s3c6410.h"


static void
s3c6410x_update_int (vic_s3c6410_device *dev)
{
	interrupt_signal_t interrupt_signal;
	if (dev->regs.vic_fiqstatus != 0 || dev->regs.vic_fiqstatus != 0) {
		interrupt_signal.arm_signal.firq =  Low_level;
	} else {
		interrupt_signal.arm_signal.firq =  High_level;
	}

	if (dev->regs.vic_irqstatus != 0 || dev->regs.vic_irqstatus != 0) {
		interrupt_signal.arm_signal.irq =  Low_level;
	} else {
		interrupt_signal.arm_signal.irq =  High_level;
	}
	interrupt_signal.arm_signal.reset =  Prev_level;
	if (dev->signal_iface)
		dev->signal_iface->signal(dev->signal, &interrupt_signal);
}


static int vic_s3c6410_raise_signal(conf_object_t *opaque, int line){
	vic_s3c6410_device *dev = opaque->obj;
	dev->regs.vic_rawintr |= 1 << line;
	dev->regs.vic_irqstatus |=  ((1 << line) & ~(dev->regs.vic_intselect) & dev->regs.vic_intenable);
	dev->regs.vic_fiqstatus |=  ((1 << line) & dev->regs.vic_intselect & dev->regs.vic_intenable);
	s3c6410x_update_int(dev);

	return No_exp;
}

static int vic_s3c6410_lower_signal(conf_object_t *opaque, int line)
{
	struct vic_s3c6410_device *dev = opaque->obj;
	return No_exp;
}


static exception_t vic_s3c6410_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	vic_s3c6410_device *dev = opaque->obj;
	switch (offset) {
		case VICIRQSTATUS:
			*(uint32_t *)buf = dev->regs.vic_irqstatus;
			dev->regs.vic_irqstatus = 0;
			s3c6410x_update_int(dev);
			break;
		case VICFIQSTATUS:
			*(uint32_t *)buf = dev->regs.vic_fiqstatus;
			dev->regs.vic_fiqstatus = 0;
			break;
		case VICRAWINTR:
			*(uint32_t *)buf = dev->regs.vic_rawintr;
			dev->regs.vic_rawintr = 0;
			break;
		case VICINTSELECT:
			*(uint32_t *)buf = dev->regs.vic_intselect;
			break;
		case VICINTENABLE:
			*(uint32_t *)buf = dev->regs.vic_intenable;
			break;

		case VICSOFTINT:
			*(uint32_t *)buf = dev->regs.vic_softint;
			break;
		case VICPROTECTION:
			*(uint32_t *)buf = dev->regs.vic_protection;
			break;
		case VICSWPRIORITYMASK:
			*(uint32_t *)buf = dev->regs.vic_swprioritymask;
			break;
		case VICPRIORITYDAISY:
			*(uint32_t *)buf = dev->regs.vic_prioritydaisy;
			break;
		default:
			if (offset >= VICVECTADDR0 && offset <= VICVECTADDR0 + 0x7c && offset & 0x3 == 0)
				*(uint32_t *)buf  = dev->regs.vic_vectaddr[(offset)>>2];
			if (offset >= VICVECPRIORITY0 && offset <= VICVECPRIORITY0 + 0x7c && offset & 0x3 == 0)
				*(uint32_t *)buf  = dev->regs.vic_vecpriority[(offset)>>2];
	}

	return No_exp;
}

static exception_t vic_s3c6410_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{

	vic_s3c6410_device *dev = opaque->obj;
	switch (offset) {
		case VICINTSELECT:
			dev->regs.vic_intselect = *(uint32_t *)buf;
			break;
		case VICINTENABLE:
			dev->regs.vic_intenable |= *(uint32_t *)buf;
			break;
		case VICINTENCLEAR:
			/* dev->vic_intenclear = data; */
			/* write 1 clear the intenable register */
			dev->regs.vic_intenable &= ~(*(uint32_t *)buf);
			break;
		case VICSOFTINT:
			dev->regs.vic_softint = *(uint32_t *)buf;
			break;
		case VICSOFTINTCLEAR:
			dev->regs.vic_softintclear = *(uint32_t *)buf;
			break;
		case VICPROTECTION:
			dev->regs.vic_protection = *(uint32_t *)buf;
			break;
		case VICSWPRIORITYMASK:
			dev->regs.vic_swprioritymask = *(uint32_t *)buf;
			break;
		case VICPRIORITYDAISY:
			dev->regs.vic_prioritydaisy = *(uint32_t *)buf;
			break;
		default:
			if (offset >= VICVECTADDR0 && offset <= VICVECTADDR0 + 0x7c && offset & 0x3 == 0)
				dev->regs.vic_vectaddr[(offset)>>2]= *(uint32_t *)buf;
			if (offset >= VICVECPRIORITY0 && offset <= VICVECPRIORITY0 + 0x7c && offset & 0x3 == 0)
				dev->regs.vic_vecpriority[(offset)>>2] = *(uint32_t *)buf;
	
	}
	return No_exp;
}

static char* vic_s3c6410_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	vic_s3c6410_device *dev = conf_obj->obj;
	return regs_name[id];
}
static char* vic_s3c6410_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	vic_s3c6410_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;

	return NULL;
}

static uint32_t vic_s3c6410_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	vic_s3c6410_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	return  sizeof(struct registers) / 4;


}

static uint32_t vic_s3c6410_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	vic_s3c6410_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t regnum;
	regnum = sizeof(struct registers)/ 4;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;


}


static uint32_t vic_s3c6410_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	vic_s3c6410_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static exception_t vic_s3c6410_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	vic_s3c6410_device *dev = opaque->obj;
}

static conf_object_t* new_vic_s3c6410(char* obj_name){
	vic_s3c6410_device* dev = skyeye_mm_zero(sizeof(vic_s3c6410_device));
	dev->obj = new_conf_object(obj_name, dev);
	return dev->obj;
}
static void free_vic_s3c6410(conf_object_t* conf_obj){
	vic_s3c6410_device *dev = conf_obj->obj;
    skyeye_free(dev->obj);
    skyeye_free(dev);
	return;	
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	vic_s3c6410_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(0);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	vic_s3c6410_device *dev = conf_obj->obj;
	return No_exp;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	vic_s3c6410_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (core_signal_intf *)SKY_get_iface(dev->signal, CORE_SIGNAL_INTF_NAME);
	if(dev->signal_iface == NULL){
		if (dev->signal == NULL)
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", CORE_SIGNAL_INTF_NAME);
		else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", CORE_SIGNAL_INTF_NAME, obj2->objname);
	}
	return No_exp;
}
 
static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	vic_s3c6410_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}
static vic_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of timer");
}
void init_vic_s3c6410(){
	static skyeye_class_t class_data = {
		.class_name = "s3c6410_vic",
		.class_desc = "s3c6410_vic",
		.new_instance = new_vic_s3c6410,
		.free_instance = free_vic_s3c6410,
		.get_attr = NULL,
		.set_attr = vic_s3c6410_set_attr,
		.module_type = SKYML_ONCHIP(SC_s3c6410),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = vic_s3c6410_read,
		.write = vic_s3c6410_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = vic_s3c6410_get_regvalue_by_id,
		.get_regname_by_id = vic_s3c6410_get_regname_by_id,
		.set_regvalue_by_id = vic_s3c6410_set_regvalue_by_id,
		.get_regnum = vic_s3c6410_get_regnum,
		.get_regid_by_name = vic_s3c6410_get_regid_by_name,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const general_signal_intf signal_intf = {
		.raise_signal = vic_s3c6410_raise_signal,
		.lower_signal = vic_s3c6410_lower_signal,
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
