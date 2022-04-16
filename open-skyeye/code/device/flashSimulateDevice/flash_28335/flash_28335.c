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
* @file flash_28335.c
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

#include "flash_28335.h"

static exception_t flash_28335_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	flash_28335_device *dev = opaque->obj;
	flash_28335_reg_t* regs = dev->regs;
	switch(offset) {
		case FOPT:
			*(uint16_t *)buf = regs->fopt.all;
			break;
		case FPWR:
			*(uint16_t *)buf = regs->fpwr.all;
			break;
		case FSTATUS:
			*(uint16_t *)buf = regs->fstatus.all;
			break;
		case FSTDBYWAIT:
			*(uint16_t *)buf = regs->fstdbywait.all;
			break;
		case FACTIVEWAIT:
			*(uint16_t *)buf = regs->factivewait.all;
			break;
		case FBANKWAIT:
			*(uint16_t *)buf = regs->fbankwait.all;
			break;
		case FOTPWAIT:
			*(uint16_t *)buf = regs->foptwait.all;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in flash_28335\n", offset);
			return Invarg_exp;
	}
	return No_exp;

}

static exception_t flash_28335_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	flash_28335_device *dev = opaque->obj;
	flash_28335_reg_t* regs = dev->regs;
	uint16_t val = *(uint16_t*)buf;
	switch(offset) {
		case FOPT:
			regs->fopt.all = val;
			break;
		case FPWR:
			regs->fpwr.all = val;
			break;
		case FSTATUS:
			regs->fstatus.all = val;
			break;
		case FSTDBYWAIT:
			regs->fstdbywait.all = val;
			break;
		case FACTIVEWAIT:
			regs->factivewait.all = val;
			break;
		case FBANKWAIT:
			regs->fbankwait.all = val;
			break;
		case FOTPWAIT:
			regs->foptwait.all = val;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in flash_28335\n", offset);
			return Invarg_exp;
	}
	return No_exp;

}

static char* flash_28335_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t flash_28335_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	flash_28335_device *dev = conf_obj->obj;
	flash_28335_reg_t* regs = dev->regs;
	uint16_t* regs_value = (uint16_t*)regs + id;
	return *regs_value;
}

static char* flash_28335_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	flash_28335_device *dev = conf_obj->obj;
	flash_28335_reg_t* regs = dev->regs;
	*((uint16_t*)regs + id) = value;

	return NULL;
}

static uint32_t flash_28335_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

static uint32_t flash_28335_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	flash_28335_device *dev = conf_obj->obj;
	flash_28335_reg_t* regs = dev->regs;
	return  sizeof(flash_28335_reg_t) / 2;
}

static uint32_t flash_28335_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	flash_28335_device *dev = conf_obj->obj;
	flash_28335_reg_t* regs = dev->regs;
	uint32_t regnum;
	regnum = sizeof(flash_28335_reg_t)/ 2;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}

	return regnum;
}


static conf_object_t* new_flash_28335(char* obj_name){
	flash_28335_device* dev = skyeye_mm_zero(sizeof(flash_28335_device));
	flash_28335_reg_t* regs =  skyeye_mm_zero(sizeof(flash_28335_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init flash_28335 regs */
	dev->regs = regs;
	dev->sched_id = -1;
	//create_thread_scheduler((unsigned int )1000, Periodic_sched, flash_interrupt_callback,  (void *)dev, &dev->sched_id);

	return dev->obj;
}
void free_flash_28335(conf_object_t* conf_obj){
	flash_28335_device* dev = (flash_28335_device *)conf_obj->obj;
	skyeye_free(dev->obj);
	skyeye_free(dev->regs);
	skyeye_free(dev);

	return;
}

static flash_28335_register_attribute(conf_class_t* clss){
}

static exception_t set_attr_signal(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	flash_28335_device *dev = obj->obj;
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
 
static exception_t get_attr_signal(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	flash_28335_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

void init_flash_28335(){
	static skyeye_class_t class_data = {
		.class_name = "f28335_flash",
		.class_desc = "f28335_flash",
		.new_instance = new_flash_28335,
		.free_instance = free_flash_28335,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_f28335),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = flash_28335_read,
		.write = flash_28335_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = flash_28335_get_regvalue_by_id,
		.get_regname_by_id = flash_28335_get_regname_by_id,
		.set_regvalue_by_id = flash_28335_set_regvalue_by_id,
		.get_regnum = flash_28335_get_regnum,
		.get_regid_by_name = flash_28335_get_regid_by_name,
		.get_regoffset_by_id = flash_28335_get_regoffset_by_id,
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
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = set_attr_signal,
			.get = get_attr_signal,
		},
	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	flash_28335_register_attribute(clss);
}
