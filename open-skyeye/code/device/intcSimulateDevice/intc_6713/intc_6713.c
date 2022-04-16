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
* @file intc_6713.c
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

#include "intc_6713.h"

static exception_t intc_6713_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct intc_6713_device *dev = opaque->obj;
	intc_6713_reg_t* regs = dev->regs;
	switch(offset) {
		case 0x8:
			*(uint32_t *)buf = regs->ext_pol;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in intc_6713\n", offset);
			return Invarg_exp;
	}
	return No_exp;

}

static exception_t intc_6713_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct intc_6713_device *dev = opaque->obj;
	intc_6713_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t*)buf;
	switch(offset) {
		case 0x8:
			regs->ext_pol = val;
			break;

		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in intc_6713\n", offset);
			return Invarg_exp;
	}
	return No_exp;

}

static char* intc_6713_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t intc_6713_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	intc_6713_device *dev = conf_obj->obj;
	intc_6713_reg_t* regs = dev->regs;
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static char* intc_6713_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	intc_6713_device *dev = conf_obj->obj;
	intc_6713_reg_t* regs = dev->regs;
	*((uint32_t*)regs + id) = value;

	return NULL;
}

static uint32_t intc_6713_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	intc_6713_device *dev = conf_obj->obj;
	intc_6713_reg_t* regs = dev->regs;
	return  sizeof(intc_6713_reg_t) / 4;


}

static uint32_t intc_6713_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	intc_6713_device *dev = conf_obj->obj;
	intc_6713_reg_t* regs = dev->regs;
	uint32_t regnum;
	regnum = sizeof(intc_6713_reg_t)/ 4;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;


}


static conf_object_t* new_intc_6713(char* obj_name){
	intc_6713_device* dev = skyeye_mm_zero(sizeof(intc_6713_device));
	intc_6713_reg_t* regs =  skyeye_mm_zero(sizeof(intc_6713_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init intc_6713 regs */
	dev->regs = regs;

	return dev->obj;
}
void free_intc_6713(conf_object_t* conf_obj){
    intc_6713_device* dev = conf_obj->obj;
    skyeye_free(dev->regs);
    skyeye_free(dev);
	return;
}


static intc_register_attribute(conf_class_t* clss){
}
void init_intc_6713(){
	static skyeye_class_t class_data = {
		.class_name = "c6713_intc",
		.class_desc = "c6713_intc",
		.new_instance = new_intc_6713,
		.free_instance = free_intc_6713,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_c6713),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = intc_6713_read,
		.write = intc_6713_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = intc_6713_get_regvalue_by_id,
		.get_regname_by_id = intc_6713_get_regname_by_id,
		.set_regvalue_by_id = intc_6713_set_regvalue_by_id,
		.get_regnum = intc_6713_get_regnum,
		.get_regid_by_name = intc_6713_get_regid_by_name,
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
	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	intc_register_attribute(clss);
}
