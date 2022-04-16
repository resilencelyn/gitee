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
* @file usbhost_s3c6410.c
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
#include <skyeye_attribute.h> 
#include <memory_space.h>
#include <skyeye_device.h>
#define DEBUG
#include <skyeye_log.h>

#include "usbhost_s3c6410.h"

static exception_t s3c6410_usbhost_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct s3c6410_usbhost_device *dev = opaque->obj;
	usbhost_reg_t* regs = dev->regs;
	switch(offset) {
		case 0x0:
			*(uint32_t*)buf = regs->hc_revision;
			break;		
		case 0x4:
			*(uint32_t*)buf = regs->hc_control;
			break;
		case 0x8:
			*(uint32_t*)buf = regs->hc_commonstatus;
			break;
		case 0x34:
			*(uint32_t*)buf = regs->hc_rminterval;
			break;
		case 0x44:
			*(uint32_t*)buf = regs->hc_ls_threashold;
			break;
		case 0x48:
			*(uint32_t*)buf = regs->hc_rhdescriptor_a;
			break;
		case 0x54:
			*(uint32_t*)buf = regs->hc_rhportstatus1;
			break;

		default:
			printf("Can not read the register at 0x%x in usbhost\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

static exception_t s3c6410_usbhost_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct s3c6410_usbhost_device *dev = opaque->obj;
	usbhost_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t*)buf;
	switch(offset) {
		case 0x0:
			regs->hc_revision = val;
			break;		
		case 0x4:
			regs->hc_control = val;
			break;
		case 0x8:
			regs->hc_commonstatus = val;
			break;
		case 0x34:
			regs->hc_rminterval = val;
			break;
		case 0x44:
			regs->hc_ls_threashold = val;
			break;
		case 0x48:
			regs->hc_rhdescriptor_a = val;
			break;
		case 0x54:
			regs->hc_rhportstatus1 = val;
			break;
		default:
			printf("Can not write the register at 0x%x in usbhost\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

char* s3c6410_usbhost_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	s3c6410_usbhost_device *dev = conf_obj->obj;
	return dev->regs_name[id];
}

uint32_t s3c6410_usbhost_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	s3c6410_usbhost_device *dev = conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)(dev->regs) + id;
	return *regs_value;
}

static char* s3c6410_usbhost_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	s3c6410_usbhost_device *dev = conf_obj->obj;
	usbhost_reg_t *regs = (dev->regs);
	*((uint32_t*)regs + id) = value;

	return NULL;
}
static uint32_t s3c6410_usbhost_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	s3c6410_usbhost_device *dev = conf_obj->obj;
	usbhost_reg_t *regs = (dev->regs);
	return  sizeof(usbhost_reg_t) / 4;


}

static uint32_t s3c6410_usbhost_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	s3c6410_usbhost_device *dev = conf_obj->obj;
	usbhost_reg_t *regs = (dev->regs);
	uint32_t regnum;
	regnum = sizeof(usbhost_reg_t)/ 4;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;


}



static conf_object_t* new_s3c6410_usbhost(char* obj_name){
	s3c6410_usbhost_device* dev = skyeye_mm_zero(sizeof(s3c6410_usbhost_device));
	usbhost_reg_t* regs =  skyeye_mm_zero(sizeof(usbhost_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init usbhost regs */
	regs->hc_revision = 0x10;
	regs->hc_control = 0x0;
	regs->hc_commonstatus = 0x0;
	regs->hc_rminterval = 0x2edf;
	regs->hc_ls_threashold = 0x628;
	regs->hc_rhdescriptor_a = 0x02001202;
	regs->hc_rhportstatus1 = 0x100;
	dev->regs = regs;
	dev->regs_name = regs_name;
		return dev->obj;
}
void free_s3c6410_usbhost(conf_object_t* obj){
	s3c6410_usbhost_device *dev = obj->obj;
	skyeye_free(dev->regs);
	skyeye_free(dev);
}

void s3c6410_usbhost_register_attribute(conf_class_t* clss){
}

void s3c6410_usbhost_register_interface(conf_class_t* clss) {
	}
void init_s3c6410_usbhost(){
	static skyeye_class_t class_data = {
		.class_name = "s3c6410_usbhost",
		.class_desc = "s3c6410 usbhost",
		.new_instance = new_s3c6410_usbhost,
		.free_instance = free_s3c6410_usbhost,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_s3c6410),
	};
		
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	s3c6410_usbhost_register_interface(clss);
	static const memory_space_intf io_memory = {
		.read = s3c6410_usbhost_read,
		.write = s3c6410_usbhost_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);	

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = s3c6410_usbhost_get_regvalue_by_id,
		.get_regname_by_id = s3c6410_usbhost_get_regname_by_id,
		.set_regvalue_by_id = s3c6410_usbhost_set_regvalue_by_id,
		.get_regnum = s3c6410_usbhost_get_regnum,
		.get_regid_by_name = s3c6410_usbhost_get_regid_by_name,
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

	s3c6410_usbhost_register_attribute(clss);
}
