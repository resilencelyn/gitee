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
* @file gpio_c6713.c
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

#include "gpio_c6713.h"

static char* gpio_c6713_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t gpio_c6713_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	gpio_c6713_device *dev = conf_obj->obj;
	gpio_c6713_reg_t* regs = dev->regs;
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t gpio_c6713_set_regvalue_by_id(conf_object_t* conf_obj, int32_t value, uint32_t id){
	gpio_c6713_device *dev = conf_obj->obj;
	gpio_c6713_reg_t* regs = dev->regs;
	*((uint32_t*)regs + id) = value;
	return 0;
}

uint32_t gpio_c6713_get_regid_by_name(conf_object_t* opaque, char* name){
	uint32_t regnum = 0, i = 0;
	gpio_c6713_device *dev = opaque->obj;
	gpio_c6713_reg_t* regs = dev->regs;
	regnum = sizeof(gpio_c6713_reg_t)/ 4;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

uint32_t gpio_c6713_get_regnum(conf_object_t* opaque){
	gpio_c6713_device *dev = opaque->obj;
	gpio_c6713_reg_t* regs = dev->regs;
	return  sizeof(gpio_c6713_reg_t) / 4;
}

void call_signal_func(conf_object_t* opaque, int int_number){
	gpio_c6713_device *dev = opaque->obj;
	gpio_c6713_reg_t* regs = dev->regs;
	core_signal_intf* core_signal = dev->core_signal;
	interrupt_signal_t interrupt_signal;
	int i = 0;

	for(i = 0; i < 16; i++)
		interrupt_signal.c6k_signal.ifr[i] = Prev_level;

	if (int_number < 16){
		interrupt_signal.c6k_signal.ifr[int_number] = High_level;
		if (core_signal)
			core_signal->signal(dev->core, &interrupt_signal);
	}

	return;
}
static exception_t gpio_c6713_read(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	gpio_c6713_device *dev = opaque->obj;
	gpio_c6713_reg_t* regs = dev->regs;
	switch (offset) {
		case 0x0:
			*(uint32_t *)buf = regs->gpen;
			break;
		case 0x4:
			*(uint32_t *)buf = regs->gpdir;
			break;
		case 0x8:
			*(uint32_t *)buf = regs->gpval;
			break;
		case 0x10:
			*(uint32_t *)buf = regs->gpdh;
			break;
		case 0x14:
			*(uint32_t *)buf = regs->gpdl;
			break;
		case 0x18:
			*(uint32_t *)buf = regs->gphm;
			break;
		case 0x1c:
			*(uint32_t *)buf = regs->gplm;
			break;
		case 0x20:
			*(uint32_t *)buf = regs->gpgc;
			break;
		case 0x24:
			*(uint32_t *)buf = regs->gppol;
			break;
		default:
			printf("In %s, read error value from offset 0x%x\n", __FUNCTION__, offset);
			break;
	}

	return No_exp;
}

static exception_t gpio_c6713_write(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	gpio_c6713_device *dev = opaque->obj;
	gpio_c6713_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t*) buf;
	int int_number = 0;
	switch (offset) {
		case 0x0:
			regs->gpen = val;
			break;
		case 0x4:
			regs->gpdir = val;
			break;
		case 0x8:
			regs->gpval = val;
			break;
		case 0x10:
			regs->gpdh = val;
			break;
		case 0x14:
			regs->gpdl= val;
			break;
		case 0x18:
			regs->gphm = val;
		/*	if (EN == 1){
				while (val & 0x1 == 0){
					int_number ++;
					val >>= 1;
				}
				if (dev->core_signal){
					call_signal_func(opaque, int_number);			
				}
			}
			*/
			break;
		case 0x1c:
			regs->gplm = val;
			break;
		case 0x20:
			regs->gpgc = val;
			/*
			 * if (EN == 1){
				while (val & 0x1 == 0){
					int_number ++;
					val >>= 1;
				}
				if (dev->core_signal){
					call_signal_func(opaque, int_number);			
				}
			}
			*/
			break;
		case 0x24:
			regs->gppol = val;
			break;
		default:
			printf("In %s, read error value from offset 0x%x\n", __FUNCTION__, offset);
			break;
	}
	return No_exp;
}

static conf_object_t* new_gpio_c6713(char* obj_name){
	gpio_c6713_device* dev = skyeye_mm_zero(sizeof(gpio_c6713_device));
	gpio_c6713_reg_t* regs =  skyeye_mm_zero(sizeof(gpio_c6713_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/*Init gpio regs*/
	regs->gpen = 0xf9;
	regs->gpdir = 0;
	regs->gpval = 0;
	regs->gpdh = 0;
	regs->gpdl = 0;
	regs->gphm = 0;
	regs->gplm = 0;
	regs->gpgc = 0;
	regs->gppol = 0;
	dev->regs = regs;

	return dev->obj;
}
static void free_gpio_c6713(conf_object_t* conf_obj){
	gpio_c6713_device* dev = conf_obj->obj;
	skyeye_free(dev->regs);
	skyeye_free(dev->obj);
	skyeye_free(dev);
	return;	
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	gpio_c6713_device *dev = obj->obj;
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
	gpio_c6713_device *dev = obj->obj;
	*obj2 = dev->core;
	*port = NULL;
	return No_exp;
}

void init_gpio_c6713(){
	static skyeye_class_t class_data = {
		.class_name = "c6713_gpio",
		.class_desc = "c6713_gpio",
		.new_instance = new_gpio_c6713,
		.free_instance = free_gpio_c6713,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_c6713),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = gpio_c6713_read,
		.write = gpio_c6713_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = gpio_c6713_get_regvalue_by_id,
		.get_regname_by_id = gpio_c6713_get_regname_by_id,
		.set_regvalue_by_id = gpio_c6713_set_regvalue_by_id,
		.get_regid_by_name = gpio_c6713_get_regid_by_name,
		.get_regnum = gpio_c6713_get_regnum
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
		(struct ConnectDescription ){
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		},

	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}
