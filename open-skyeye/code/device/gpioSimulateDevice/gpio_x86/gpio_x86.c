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
* @file gpio_x86.c
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
#include <skyeye_core.h>
#include <memory_space.h>
#include <skyeye_device.h>
#define DEBUG
#include <skyeye_log.h>

#include "gpio_x86.h"

exception_t update_gpio(conf_object_t *obj, uint32_t regvalue){
	x86_gpio_device *dev = obj->obj;
	gpio_reg_t* regs = dev->regs;
	regs->GPBDAT.all = regvalue;
	return No_exp;
}
exception_t update_gpio_on_line(conf_object_t *obj, uint32_t regvalue){
	x86_gpio_device *dev = obj->obj;
	gpio_reg_t* regs = dev->regs;
	if(dev->gpio_connector && dev->connector_iface){
		dev->connector_iface->update(dev->gpio_connector, regvalue);
	}
	if(dev->gpio_gate_a && dev->gate_a_iface){
		dev->gate_a_iface->update(dev->gpio_gate_a, regvalue);
	}
	if(dev->gpio_gate_b && dev->gate_b_iface){
		dev->gate_b_iface->update(dev->gpio_gate_b, regvalue);
	}
	return No_exp;
}
static exception_t x86_gpio_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct x86_gpio_device *dev = opaque->obj;
	gpio_reg_t* regs = dev->regs;
	switch(offset) {
		case 0x0:
			*(uint32_t*)buf = regs->GPACTRL.all; 
			break;
		case 0x2:
			*(uint16_t*)buf = regs->GPAQSEL1.all;
			break;
		case 0x4:
			*(uint16_t*)buf = regs->GPAQSEL2.all;
			break;
		case 0x6:
			*(uint16_t*)buf = regs->GPAMUX1.all;
			break;
		case 0x8:
			*(uint16_t*)buf = regs->GPAMUX2.all;
			break;
		case 0xa:
			*(uint32_t*)buf = regs->GPADIR.all;
			break;
		case 0xc:
			*(uint32_t*)buf = regs->GPAPUD.all;
			break;
		case 0x10:
			*(uint32_t*)buf = regs->GPBCTRL.all;
			break;
		case 0x12:
			*(uint16_t*)buf = regs->GPBQSEL1.all;
			break;
		case 0x14:
			*(uint16_t*)buf = regs->GPBQSEL2.all;
			break;
		case 0x16:
			*(uint16_t*)buf = regs->GPBMUX1.all;
			break;
		case 0x18:
			*(uint16_t*)buf = regs->GPBMUX2.all;
			break;
		case 0x1a:
			*(uint32_t*)buf = regs->GPBDIR.all;
			break;
		case 0x1c:
			*(uint32_t*)buf = regs->GPBPUD.all;
			break;
		case 0x26:
			*(uint32_t*)buf = regs->GPCMUX1.all;
			break;
		case 0x28:
			*(uint32_t*)buf = regs->GPCMUX2.all;
			break;
		case 0x2a:
			*(uint32_t*)buf = regs->GPCDIR.all;
			break;
		case 0x2c:
			*(uint32_t*)buf = regs->GPCPUD.all;
			break;
		case 0x40:
			*(uint32_t*)buf = regs->GPADAT.all;
			break;
		case 0x42:
			*(uint32_t*)buf = regs->GPASET.all;
			break;
		case 0x44:
			*(uint32_t*)buf = regs->GPACLEAR.all;
			break;
		case 0x46:
			*(uint32_t*)buf = regs->GPATOGGLE.all;
			break;
		case 0x48:
			*(uint32_t*)buf = regs->GPBDAT.all;
			break;
		case 0x4a:
			*(uint32_t*)buf = regs->GPBSET.all;
			break;
		case 0x4c:
			*(uint32_t*)buf = regs->GPBCLEAR.all;
			break;
		case 0x4e:
			*(uint32_t*)buf = regs->GPBTOGGLE.all;
			break;
		case 0x50:
			*(uint32_t*)buf = regs->GPCDAT.all;
			break;
		case 0x52:
			*(uint32_t*)buf = regs->GPCSET.all;
			break;
		case 0x54:
			*(uint32_t*)buf = regs->GPCCLEAR.all;
			break;
		case 0x56:
			*(uint32_t*)buf = regs->GPCTOGGLE.all;
			break;
		case 0x60:
			*(uint16_t*)buf = regs->GPIOXINT1SEL.all;
			break;
		case 0x61:
			*(uint16_t*)buf = regs->GPIOXINT2SEL.all;
			break;
		case 0x62:
			*(uint16_t*)buf = regs->GPIOXNMISEL.all;
			break;
		case 0x63:
			*(uint16_t*)buf = regs->GPIOXINT3SEL.all;
			break;
		case 0x64:
			*(uint16_t*)buf = regs->GPIOXINT4SEL.all;
			break;
		case 0x65:
			*(uint16_t*)buf = regs->GPIOXINT5SEL.all;
			break;
		case 0x66:
			*(uint16_t*)buf = regs->GPIOXINT6SEL.all;
			break;
		case 0x67:
			*(uint16_t*)buf = regs->GPIOXINT7SEL.all;
			break;
		case 0x68:
			*(uint16_t*)buf = regs->GPIOLPMSEL.all;
			break;
		default:
			skyeye_log(Warning_log, __FUNCTION__,"Can not read the register at 0x%x in gpio\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

static exception_t x86_gpio_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct x86_gpio_device *dev = opaque->obj;
	gpio_reg_t* regs = dev->regs;
	uint32_t val32 = *(uint32_t*)buf;
	uint16_t val16 = *(uint16_t*)buf;
	switch(offset) {
		case 0x0:
			regs->GPACTRL.all = val32; 
			break;
		case 0x2:
			regs->GPAQSEL1.all = val16;
			break;
		case 0x4:
			regs->GPAQSEL2.all = val16;
			break;
		case 0x6:
			regs->GPAMUX1.all = val16;
			break;
		case 0x8:
			regs->GPAMUX2.all = val16;
			break;
		case 0xa:
			regs->GPADIR.all = val32;
			break;
		case 0xc:
			regs->GPAPUD.all = val32;
			break;
		case 0x10:
			regs->GPBCTRL.all = val32;
			break;
		case 0x12:
			regs->GPBQSEL1.all = val16;
			break;
		case 0x14:
			regs->GPBQSEL2.all = val16;
			break;
		case 0x16:
			regs->GPBMUX1.all = val16;
			break;
		case 0x18:
			regs->GPBMUX2.all = val16;
			break;
		case 0x1a:
			regs->GPBDIR.all = val32;
			break;
		case 0x1c:
			regs->GPBPUD.all = val32;
			break;
		case 0x26:
			regs->GPCMUX1.all = val16;
			break;
		case 0x28:
			regs->GPCMUX2.all = val16;
			break;
		case 0x2a:
			regs->GPCDIR.all = val32;
			break;
		case 0x2c:
			regs->GPCPUD.all = val32;
			break;
		case 0x40:
			regs->GPADAT.all = val32;
			update_gpio_on_line(opaque, regs->GPADAT.all);
			break;
		case 0x42:
			regs->GPASET.all = val32;
			break;
		case 0x44:
			regs->GPACLEAR.all = val32;
			break;
		case 0x46:
			regs->GPATOGGLE.all = val32;
			break;
		case 0x48:
			regs->GPBDAT.all = val32;
			update_gpio_on_line(opaque, regs->GPBDAT.all);
			break;
		case 0x4a:
			regs->GPBSET.all = val32;
			break;
		case 0x4c:
			regs->GPBCLEAR.all = val32;
			break;
		case 0x4e:
			regs->GPBTOGGLE.all = val32;
			break;
		case 0x50:
			regs->GPCDAT.all = val32;
			update_gpio_on_line(opaque, regs->GPCDAT.all);
			break;
		case 0x52:
			regs->GPCSET.all = val32;
			break;
		case 0x54:
			regs->GPCCLEAR.all = val32;
			break;
		case 0x56:
			regs->GPCTOGGLE.all = val32;
			break;
		case 0x60:
			regs->GPIOXINT1SEL.all = val16;
			break;
		case 0x61:
			regs->GPIOXINT2SEL.all = val16;
			break;
		case 0x62:
			regs->GPIOXNMISEL.all = val16;
			break;
		case 0x63:
			regs->GPIOXINT3SEL.all = val16;
			break;
		case 0x64:
			regs->GPIOXINT4SEL.all = val16;
			break;
		case 0x65:
			regs->GPIOXINT5SEL.all = val16;
			break;
		case 0x66:
			regs->GPIOXINT6SEL.all = val16;
			break;
		case 0x67:
			regs->GPIOXINT7SEL.all = val16;
			break;
		case 0x68:
			regs->GPIOLPMSEL.all = val16;
			break;
		default:
			skyeye_log(Warning_log, __FUNCTION__, "Can not write the register at 0x%x in gpio\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

char* x86_gpio_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	x86_gpio_device *dev = conf_obj->obj;
	return dev->regs_name[id];
}

uint32_t x86_gpio_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	x86_gpio_device *dev = conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)(dev->regs) + id;
	return *regs_value;
}

static char* x86_gpio_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	x86_gpio_device *dev = conf_obj->obj;
	gpio_reg_t *regs = (dev->regs);
	*((uint32_t*)regs + id) = value;

	return NULL;
}

static uint32_t x86_gpio_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	x86_gpio_device *dev = conf_obj->obj;
	gpio_reg_t *regs = (dev->regs);
	//return  sizeof(gpio_reg_t) / 4;
    return 40;
}

static uint32_t x86_gpio_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	x86_gpio_device *dev = conf_obj->obj;
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

static conf_object_t* new_x86_gpio(char* obj_name){
	x86_gpio_device* dev = skyeye_mm_zero(sizeof(x86_gpio_device));
	gpio_reg_t* regs =  skyeye_mm_zero(sizeof(gpio_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init gpio regs */

	dev->regs = regs;
	dev->regs_name = &regs_name;
	return dev->obj;
}
void free_x86_gpio(conf_object_t* conf_obj){
	x86_gpio_device* dev = conf_obj->obj;	

	skyeye_free(dev->regs);
	skyeye_free(dev->obj);
	skyeye_free(dev);
	return;
}

void x86_gpio_register_interface(conf_class_t* clss) {
	static const memory_space_intf io_memory = {
		.read = x86_gpio_read,
		.write = x86_gpio_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);	
	
	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = x86_gpio_get_regvalue_by_id,
		.get_regname_by_id = x86_gpio_get_regname_by_id,
		.set_regvalue_by_id = x86_gpio_set_regvalue_by_id,
		.get_regnum = x86_gpio_get_regnum,
		.get_regid_by_name = x86_gpio_get_regid_by_name,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);
}


static exception_t connector_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	x86_gpio_device *dev = obj->obj;
	dev->gpio_connector = obj2;
	dev->connector_iface = (skyeye_gpio_connector_intf *)SKY_get_iface(dev->gpio_connector, SKYEYE_GPIO_CONNECTOR);
	if (dev->connector_iface == NULL){
		if (dev->gpio_connector == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SKYEYE_GPIO_CONNECTOR);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SKYEYE_GPIO_CONNECTOR, obj2->objname);

		return Not_found_exp;
	}
//	update_gpio_on_line(obj, dev->regs->gpaqsel1);
	return No_exp;
}
 
static exception_t connector_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	x86_gpio_device *dev = obj->obj;
	*obj2 = dev->gpio_connector;
	*port = NULL;
	return No_exp;
}

static exception_t gate_b_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	x86_gpio_device *dev = obj->obj;
	dev->gpio_gate_b = obj2;
	dev->gate_b_iface = (skyeye_gate_b_intf*)SKY_get_iface(dev->gpio_gate_b, LOGIC_GATE_B);
	if (dev->gate_b_iface == NULL){
		if (dev->gpio_gate_b == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", LOGIC_GATE_B);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", LOGIC_GATE_B, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}
 
static exception_t gate_b_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	x86_gpio_device *dev = obj->obj;
	*obj2 = dev->gpio_gate_b;
	*port = NULL;
	return No_exp;
}


static exception_t gate_a_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	x86_gpio_device *dev = obj->obj;
	dev->gpio_gate_a = obj2;
	dev->gate_a_iface = (skyeye_gate_a_intf*)SKY_get_iface(dev->gpio_gate_a, LOGIC_GATE_A);
	if (dev->gate_a_iface == NULL){
		if (dev->gpio_gate_a == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", LOGIC_GATE_A);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", LOGIC_GATE_A, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}
 
static exception_t gate_a_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	x86_gpio_device *dev = obj->obj;
	*obj2 = dev->gpio_gate_a;
	*port = NULL;
	return No_exp;
}
void init_x86_gpio(){
	static skyeye_class_t class_data = {
		.class_name = "x86_gpio",
		.class_desc = "x86 gpio",
		.new_instance = new_x86_gpio,
		.free_instance = free_x86_gpio,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_OFFCHIP,
	};
		
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const skyeye_gpio_connector_intf iface = {
		.update =  update_gpio
	};
	SKY_register_iface(clss, SKYEYE_GPIO_CONNECTOR, &iface);

	static const memory_space_intf io_memory = {
		.read = x86_gpio_read,
		.write = x86_gpio_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);	
	
	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = x86_gpio_get_regvalue_by_id,
		.get_regname_by_id = x86_gpio_get_regname_by_id,
		.set_regvalue_by_id = x86_gpio_set_regvalue_by_id,
		.get_regnum = x86_gpio_get_regnum,
		.get_regid_by_name = x86_gpio_get_regid_by_name,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = SKYEYE_GPIO_CONNECTOR,
			.iface = &iface,
		},
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
		(struct ConnectDescription ){
			.name = LOGIC_GATE_A,
			.set = gate_a_set,
			.get = gate_a_get,
		},
		(struct ConnectDescription) {
			.name = LOGIC_GATE_B,
			.set = gate_b_set,
			.get = gate_b_get,
		},
		(struct ConnectDescription) {
			.name = SKYEYE_GPIO_CONNECTOR,
			.set = connector_set,
			.get = connector_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}
