/* Copyright (C)
* 2011 - Michael.Kang blackfin.kang@gmail.com
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
* @file sysctrl_s3c2410x.c
* @brief The implementation of system controller
* @author Michael.Kang blackfin.kang@gmail.com
* @version 78.77
* @date 2011-12-12
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
#include <skyeye_attribute.h>
#include <skyeye_device.h>
#define DEBUG
#include <skyeye_log.h>

#include "sysctrl_s3c2410x.h"

static exception_t s3c2410x_sysctrl_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct s3c2410x_sysctrl_device *dev = opaque->obj;
	sysctrl_reg_t* regs = dev->regs;
	switch(offset) {
		case 0x00:
			*(uint32_t*)buf = regs->locktime;
			break;
		case 0x04:
			*(uint32_t*)buf = regs->mpllcon;
			break;
		case 0x08:
			*(uint32_t*)buf = regs->upllcon;
			break;
		case 0x0c:
			*(uint32_t*)buf = regs->clkcon;
			break;
		case 0x10:
			*(uint32_t*)buf = regs->clkslow;
			break;
		case 0x14:
			*(uint32_t*)buf = regs->clkdivn;
			break;
		default:
			printf("Can not read the register at 0x%x in sysctrl\n", offset);
			*(uint32_t*)buf = 0;
			return No_exp;
	}
	return No_exp;
}

static exception_t s3c2410x_sysctrl_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct s3c2410x_sysctrl_device *dev = opaque->obj;
	sysctrl_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t*)buf;
	switch(offset) {
		case 0x00:
			regs->locktime = val;
			break;
		case 0x04:
			regs->mpllcon = val;
			break;
		case 0x08:
			regs->upllcon = val;
			break;
		case 0x0c:
			regs->clkcon = val;
			break;
		case 0x10:
			regs->clkslow = val;
			break;
		case 0x14:
			regs->clkdivn = val;
			break;
		default:
			printf("Can not write the register at 0x%x in sysctrl\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}


static char* s3c2410x_sysctrl_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	s3c2410x_sysctrl_device *dev = conf_obj->obj;
	return dev->regs_name[id];
}


static uint32_t s3c2410x_sysctrl_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	s3c2410x_sysctrl_device *dev = conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)(dev->regs) + id;
	return *regs_value;
}

static exception_t s3c2410x_sysctrl_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	s3c2410x_sysctrl_device *dev = conf_obj->obj;
	sysctrl_reg_t *regs = (dev->regs);
	*((uint32_t*)regs + id) = value;

	return No_exp;
}

static uint32_t s3c2410x_sysctrl_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	return  sizeof(sysctrl_reg_t) / 4;
}

static uint32_t s3c2410x_sysctrl_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	uint32_t regnum;
	regnum = sizeof(sysctrl_reg_t)/ 4;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_s3c2410x_sysctrl(char* obj_name){
	s3c2410x_sysctrl_device* dev = skyeye_mm_zero(sizeof(s3c2410x_sysctrl_device));
	sysctrl_reg_t* regs =  skyeye_mm_zero(sizeof(sysctrl_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init sysctrl regs */
	regs->locktime	= 0x00ffffff;
	regs->mpllcon	= 0x0005c080;
	regs->upllcon	= 0x00028080;
	regs->clkcon	= 0x0007fff0;
	regs->clkslow	= 0x00000004;
	regs->clkdivn	= 0x00000000;

	dev->regs = regs;
	dev->regs_name = regs_name;

	return dev->obj;
}
static exception_t free_s3c2410x_sysctrl(conf_object_t* opaque) {
	s3c2410x_sysctrl_device* dev = opaque->obj;

	skyeye_free(dev->regs);
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}

void s3c2410x_sysctrl_register_attribute(conf_class_t* clss){
}

void init_s3c2410x_sysctrl(){
	static skyeye_class_t class_data = {
		.class_name = "s3c2410x_sysctrl",
		.class_desc = "s3c2410x sysctrl",
		.new_instance = new_s3c2410x_sysctrl,
		.free_instance = free_s3c2410x_sysctrl,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_s3c2410x1),
	};
	conf_class_t *clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = s3c2410x_sysctrl_read,
		.write = s3c2410x_sysctrl_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = s3c2410x_sysctrl_get_regvalue_by_id,
		.get_regname_by_id = s3c2410x_sysctrl_get_regname_by_id,
		.set_regvalue_by_id = s3c2410x_sysctrl_set_regvalue_by_id,
		.get_regnum = s3c2410x_sysctrl_get_regnum,
		.get_regid_by_name = s3c2410x_sysctrl_get_regid_by_name,
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

	s3c2410x_sysctrl_register_attribute(clss);
}
