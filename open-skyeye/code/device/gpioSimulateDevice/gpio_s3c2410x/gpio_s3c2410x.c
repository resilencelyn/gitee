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
* @file gpio_s3c2410x.c
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
#include <skyeye_attribute.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#define DEBUG
#include <skyeye_log.h>

#include "gpio_s3c2410x.h"

static exception_t s3c2410x_gpio_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{

	struct s3c2410x_gpio_device *dev = opaque->obj;
	gpio_reg_t* regs = dev->regs;
	switch(offset) {
		case 0x00:
			*(uint32_t*)buf = regs->gpacon;
			break;
		case 0x04:
			*(uint32_t*)buf = regs->gpadat;
			break;
		case 0x10:
			*(uint32_t*)buf = regs->gpbcon;
			break;
		case 0x14:
			*(uint32_t*)buf = regs->gpbdat;
			break;
		case 0x18:
			*(uint32_t*)buf = regs->gpbup;
			break;
		case 0x20:
			*(uint32_t*)buf = regs->gpccon;
			break;
		case 0x24:
			*(uint32_t*)buf = regs->gpcdat;
			break;
		case 0x28:
			*(uint32_t*)buf = regs->gpcup;
			break;
		case 0x30:
			*(uint32_t*)buf = regs->gpdcon;
			break;
		case 0x34:
			*(uint32_t*)buf = regs->gpddat;
			break;
		case 0x38:
			*(uint32_t*)buf = regs->gpdup;
			break;
		case 0x40:
			*(uint32_t*)buf = regs->gpecon;
			break;
		case 0x44:
			*(uint32_t*)buf = regs->gpedat;
			break;
		case 0x48:
			*(uint32_t*)buf = regs->gpeup;
			break;
		case 0x50:
			*(uint32_t*)buf = regs->gpfcon;
			break;
		case 0x54:
			*(uint32_t*)buf = regs->gpfdat;
			break;
		case 0x58:
			*(uint32_t*)buf = regs->gpfup;
			break;
		case 0x60:
			*(uint32_t*)buf = regs->gpgcon;
			break;
		case 0x64:
			*(uint32_t*)buf = regs->gpgdat;
			break;
		case 0x68:
			*(uint32_t*)buf = regs->gpgup;
			break;
		case 0x70:
			*(uint32_t*)buf = regs->gphcon;
			break;
		case 0x74:
			*(uint32_t*)buf = regs->gphdat;
			break;
		case 0x78:
			*(uint32_t*)buf = regs->gphup;
			break;
		/*external interrupt put here?*/
		case 0x80:
			*(uint32_t*)buf = regs->misccr;
			break;
		case 0x88:
			*(uint32_t*)buf = regs->extint0;
			break;
		case 0x8c:
			*(uint32_t*)buf = regs->extint1;
			break;
		case 0x90:
			*(uint32_t*)buf = regs->extint2;
			break;
		case 0x94:
			*(uint32_t*)buf = regs->eintflt0;
			break;
		case 0x98:
			*(uint32_t*)buf = regs->eintflt1;
			break;
		case 0x9c:
			*(uint32_t*)buf = regs->eintflt2;
			break;
		case -0x99fff60: /*how to define gpio size in the config of json?*/
			*(uint32_t*)buf = regs->eintflt3;
			break;
		case 0xa4:
			*(uint32_t*)buf = regs->eintmask;
			break;
		case 0xa8:
			*(uint32_t*)buf = regs->eintpend;
			break;
		/********************************/
		case 0xac:
			*(uint32_t*)buf = regs->gstatus0;
			break;
		case 0xb0:
			*(uint32_t*)buf = regs->gstatus1;
			break;
		case 0xb4:
			*(uint32_t*)buf = regs->gstatus2;
			break;
		case 0xb8:
			*(uint32_t*)buf = regs->gstatus3;
			break;
		case 0xbc:
			*(uint32_t*)buf = regs->gstatus4;
			break;
		default:
			skyeye_log(Warning_log, __FUNCTION__,"Can not read the register at 0x%x in gpio\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

static exception_t s3c2410x_gpio_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct s3c2410x_gpio_device *dev = opaque->obj;
	gpio_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t*)buf;
	switch(offset) {
		case 0x00:
			regs->gpacon = val;
			break;
		case 0x04:
			regs->gpadat = val;
			break;
		case 0x10:
			regs->gpbcon = val;
			break;
		case 0x14:
			regs->gpbdat = val;
			break;
		case 0x18:
			regs->gpbup = val;
			break;
		case 0x20:
			regs->gpccon = val;
			break;
		case 0x24:
			regs->gpcdat = val;
			break;
		case 0x28:
			regs->gpcup = val;
			break;
		case 0x30:
			regs->gpdcon = val;
			break;
		case 0x34:
			regs->gpddat = val;
			break;
		case 0x38:
			regs->gpdup = val;
			break;
		case 0x40:
			regs->gpecon = val;
			break;
		case 0x44:
			regs->gpedat = val;
			break;
		case 0x48:
			regs->gpeup = val;
			break;
		case 0x50:
			regs->gpfcon = val;
			break;
		case 0x54:
			regs->gpfdat = val;
			break;
		case 0x58:
			regs->gpfup = val;
			break;
		case 0x60:
			regs->gpgcon = val;
			break;
		case 0x64:
			regs->gpgdat = val;
			break;
		case 0x68:
			regs->gpgup = val;
			break;
		case 0x70:
			regs->gphcon = val;
			break;
		case 0x74:
			regs->gphdat = val;
			break;
		case 0x78:
			regs->gphup = val;
			break;
		/*external interrupt put here?*/
		case 0x80:
			regs->misccr = val;
			break;
		case 0x88:
			regs->extint0 = val;
			break;
		case 0x8c:
			regs->extint1 = val;
			break;
		case 0x90:
			regs->extint2 = val;
			break;
		case 0x94:
			regs->eintflt0 = val;
			break;
		case 0x98:
			regs->eintflt1 = val;
			break;
		case 0x9c:
			regs->eintflt2 = val;
			break;
		case -0x99fff60:
			regs->eintflt3 = val;
			break;
		case 0xa4:
			regs->eintmask = val;
			break;
		case 0xa8:
			regs->eintpend = val;
			break;
		/********************************/
		case 0xb4:
			regs->gstatus2 = val;
			break;
		case 0xb8:
			regs->gstatus3 = val;
			break;
		case 0xbc:
			regs->gstatus4 = val;
			break;
		default:
			skyeye_log(Warning_log, __FUNCTION__, "Can not write the register at 0x%x in gpio\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

static char* s3c2410x_gpio_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	s3c2410x_gpio_device *dev = conf_obj->obj;
	return dev->regs_name[id];
}

static uint32_t s3c2410x_gpio_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	s3c2410x_gpio_device *dev = conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)(dev->regs) + id;
	return *regs_value;
}

static exception_t s3c2410x_gpio_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	s3c2410x_gpio_device *dev = conf_obj->obj;
	gpio_reg_t *regs = (dev->regs);
	*((uint32_t*)regs + id) = value;

	return No_exp;
}

static uint32_t s3c2410x_gpio_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	s3c2410x_gpio_device *dev = conf_obj->obj;
	gpio_reg_t *regs = (dev->regs);
	return  sizeof(gpio_reg_t) / 4;
}

static uint32_t s3c2410x_gpio_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	s3c2410x_gpio_device *dev = conf_obj->obj;
	gpio_reg_t *regs = (dev->regs);
	uint32_t regnum;
	regnum = sizeof(gpio_reg_t)/ 4;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_s3c2410x_gpio(char* obj_name){
	s3c2410x_gpio_device* dev = skyeye_mm_zero(sizeof(s3c2410x_gpio_device));
	gpio_reg_t* regs =  skyeye_mm_zero(sizeof(gpio_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init gpio regs */
	regs->gpacon = 0x7fffff;

	regs->gpbcon = 0x0;
	regs->gpbup = 0x0;

	regs->gpccon = 0x0;
	regs->gpcup = 0x0;

	regs->gpdcon = 0x0;
	regs->gpdup = 0xf000;

	regs->gpecon = 0x0;
	regs->gpeup = 0x0;

	regs->gpfcon = 0x0;
	regs->gpfup = 0x0;

	regs->gpgcon = 0x0;
	regs->gpgup = 0xf800;

	regs->gphcon = 0x0;
	regs->gphup = 0x0;

	regs->misccr = 0x10330;
	regs->dclkcon = 0x0;

	regs->extint0 = 0x0;
	regs->extint1 = 0x0;
	regs->extint2 = 0x0;

	regs->eintflt2 = 0x0;
	regs->eintflt3 = 0x0;

	regs->eintmask = 0x00fffff0;
	regs->eintpend = 0x0;

	regs->gstatus1 = 0x32410000;
	regs->gstatus2 = 0x1;
	regs->gstatus3 = 0x0;
	regs->gstatus4 = 0x0;

	dev->regs = regs;
	dev->regs_name = regs_name;
	return dev->obj;
}
static exception_t free_s3c2410x_gpio(conf_object_t* opaque) {
	s3c2410x_gpio_device* dev = opaque->obj;

	skyeye_free(dev->regs);
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}

void s3c2410x_gpio_register_attribute(conf_class_t* clss){

}

void init_s3c2410x_gpio(){
	static skyeye_class_t class_data = {
		.class_name = "s3c2410x_gpio",
		.class_desc = "s3c2410x gpio",
		.new_instance = new_s3c2410x_gpio,
		.free_instance = free_s3c2410x_gpio,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_s3c2410x1),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = s3c2410x_gpio_read,
		.write = s3c2410x_gpio_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = s3c2410x_gpio_get_regvalue_by_id,
		.get_regname_by_id = s3c2410x_gpio_get_regname_by_id,
		.set_regvalue_by_id = s3c2410x_gpio_set_regvalue_by_id,
		.get_regnum = s3c2410x_gpio_get_regnum,
		.get_regid_by_name = s3c2410x_gpio_get_regid_by_name,
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
		}
 	};
	static const struct ConnectDescription connects[] = {
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	s3c2410x_gpio_register_attribute(clss);
}
