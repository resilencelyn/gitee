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
* @file ctrl_am3359.c
* @brief The implementation of system controller
* @author Kewei Yu: keweihk@gmail.com
* @version 78.77
*/

#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_attribute.h>
#include <skyeye_iface.h>
#define DEBUG
#include <skyeye_log.h>
#include <skyeye_core.h>

#include "ctrl_am3359.h"
static exception_t reset_ctrl_am3359(conf_object_t* opaque, const char* args);
static exception_t ctrl_am3359_read(conf_object_t *opaque,
		generic_address_t offset, void* buf, size_t count)
{
	ctrl_am3359_device *dev = opaque->obj;
	ctrl_am3359_reg_t *regs = &dev->regs;
	uint32_t index = 0, *val = (uint32_t *)buf;

	switch(offset) {
		case CTRL_STATUS:
			*val = regs->control_status;
			break;
		case CTRL_DEV_ID:
			*val = regs->device_id;
			break;
		case CTRL_DEV_FEATURE:
			*val = regs->dev_feature;
			break;
		default:
			skyeye_log(Warning_log, __FUNCTION__, "Can not read the register at 0x%x in %s\n", offset, __FILE__);
			break;
	}

	return No_exp;
}
static exception_t ctrl_am3359_write(conf_object_t *opaque,
		generic_address_t offset, uint32_t* buf, size_t count)
{
	ctrl_am3359_device *dev = opaque->obj;
	ctrl_am3359_reg_t* regs = &dev->regs;
	uint32_t val = *(uint32_t*)buf, line, index = 0;

	switch(offset) {
		default:
			skyeye_log(Warning_log, __FUNCTION__, "Can not write the register at 0x%x in %s\n", offset, __FILE__);
			break;
	}
	return No_exp;
}
static void init_regs(ctrl_am3359_device *dev)
{
	memset(&dev->regs, 0, sizeof(dev->regs));

	dev->regs.control_status	= 0x00080332;
	dev->regs.device_id		= 0x0b94402e;
	dev->regs.dev_feature 		= 0x00000000;
}
static conf_object_t* new_ctrl_am3359(char* obj_name)
{
	ctrl_am3359_device *dev = skyeye_mm_zero(sizeof(ctrl_am3359_device));

	dev->obj = new_conf_object(obj_name, dev);
	init_regs(dev);

	return dev->obj;
}
static exception_t reset_ctrl_am3359(conf_object_t* opaque, const char* args)
{
	ctrl_am3359_device *dev = opaque->obj;
	init_regs(dev);

	return No_exp;
}
static exception_t free_ctrl_am3359(conf_object_t* opaque) {
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}
static uint32_t ctrl_am3359_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id) {
	ctrl_am3359_device *dev = conf_obj->obj;
	struct ctrl_am3359_reg *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static char* ctrl_am3359_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	ctrl_am3359_device *dev = conf_obj->obj;
	return regs_name[id];
}
static exception_t ctrl_am3359_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	ctrl_am3359_device *dev = conf_obj->obj;
	struct ctrl_am3359_reg *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;

	return No_exp;
}
static uint32_t ctrl_am3359_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	ctrl_am3359_device *dev = conf_obj->obj;
	struct ctrl_am3359_reg *regs = &(dev->regs);
	return  sizeof(struct ctrl_am3359_reg) / 4;
}
static uint32_t ctrl_am3359_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	ctrl_am3359_device *dev = conf_obj->obj;
	struct ctrl_am3359_reg *regs = &(dev->regs);
	uint32_t regnum;
	regnum = sizeof(struct ctrl_am3359_reg) / 4;
	int i;

	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
void init_ctrl_am3359()
{
	static skyeye_class_t class_data = {
		.class_name	= "am3359_ctrl",
		.class_desc 	= "am3359_ctrl",
		.new_instance	= new_ctrl_am3359,
		.free_instance	= free_ctrl_am3359,
		.reset_instance	= reset_ctrl_am3359,
		.set_attr	= NULL,
		.get_attr	= NULL,
		.module_type = SKYML_ONCHIP(SC_am335x SC_am3359),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read	= ctrl_am3359_read,
		.write	= ctrl_am3359_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = ctrl_am3359_get_regvalue_by_id,
		.get_regname_by_id = ctrl_am3359_get_regname_by_id,
		.set_regvalue_by_id = ctrl_am3359_set_regvalue_by_id,
		.get_regnum = ctrl_am3359_get_regnum,
		.get_regid_by_name = ctrl_am3359_get_regid_by_name,
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

	class_register_ifaces(clss, ifaces);
}
