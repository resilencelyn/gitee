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
* @file gpio.c
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

#include "gpio.h"


exception_t  update_gpio(conf_object_t *obj, uint32_t regvalue){
	gpio_device *dev = obj->obj;
	gpio_reg_t* regs = dev->regs;
	regs->pin = regvalue;
	return No_exp;
}
exception_t  update_gpio_on_line(conf_object_t *obj, uint32_t regvalue){
	gpio_device *dev = obj->obj;
	if(dev->gpio_connector && dev->connector_iface){
		dev->connector_iface->update(dev->gpio_connector, dev->regs->pin);
	}
	if(dev->gpio_gate_a && dev->gate_a_iface){
		dev->gate_a_iface->update(dev->gpio_gate_a, dev->regs->pin);
	}
	if(dev->gpio_gate_b && dev->gate_b_iface){
		dev->gate_b_iface->update(dev->gpio_gate_b, dev->regs->pin);
	}
	return No_exp;
}


static char* gpio_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t gpio_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	gpio_device *dev = conf_obj->obj;
	gpio_reg_t* regs = dev->regs;
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t gpio_set_regvalue_by_id(conf_object_t* conf_obj, int32_t value, uint32_t id){
	gpio_device *dev = conf_obj->obj;
	gpio_reg_t* regs = dev->regs;
	*((uint32_t*)regs + id) = value;
	update_gpio_on_line(conf_obj, regs->pin);
	return 0;
}
uint32_t gpio_get_regid_by_name(conf_object_t* opaque, char* name){
	uint32_t regnum = 0, i = 0;
	gpio_device *dev = opaque->obj;
	gpio_reg_t* regs = dev->regs;
	regnum = sizeof(gpio_reg_t)/ 4;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

uint32_t gpio_get_regnum(conf_object_t* opaque){
	gpio_device *dev = opaque->obj;
	gpio_reg_t* regs = dev->regs;
	return  sizeof(gpio_reg_t) / 4;
}

static exception_t gpio_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	gpio_device *dev = opaque->obj;
	gpio_reg_t* regs = dev->regs;
	switch (offset) {
		case 0:
			regs->pin = *(uint32_t *)buf;
			update_gpio_on_line(opaque, regs->pin);
			break;
		default:
			break;
	}

	return No_exp;
}

static exception_t gpio_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	gpio_device *dev = opaque->obj;
	gpio_reg_t* regs = dev->regs;
	switch (offset) {
		case 0:
			*(uint32_t *)buf = regs->pin;
			break;
		default:
			break;
	}
	return No_exp;
}

static conf_object_t* new_gpio(char* obj_name){
	gpio_device* dev = skyeye_mm_zero(sizeof(gpio_device));
	gpio_reg_t* regs =  skyeye_mm_zero(sizeof(gpio_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	dev->regs = regs;
	dev->gpio_connector = NULL;
	dev->connector_iface = NULL;
	dev->gpio_gate_a = NULL;
	dev->gate_a_iface = NULL;
	dev->gpio_gate_b = NULL;
	dev->gate_b_iface = NULL;

	return dev->obj;
}
static void free_gpio(conf_object_t* conf_obj){
	gpio_device *dev = conf_obj->obj;
    skyeye_free(dev->regs);
    skyeye_free(dev->obj);
    skyeye_free(dev);
	return;	
}

static void gpio_register_interface(conf_class_t* clss){
	static const skyeye_gpio_connector_intf iface = {
		.update =  update_gpio
	};
	SKY_register_iface(clss, SKYEYE_GPIO_CONNECTOR, &iface);

	static const memory_space_intf io_memory = {
		.read = gpio_read,
		.write = gpio_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = gpio_get_regvalue_by_id,
		.get_regname_by_id = gpio_get_regname_by_id,
		.set_regvalue_by_id = gpio_set_regvalue_by_id,
		.get_regid_by_name = gpio_get_regid_by_name,
		.get_regnum = gpio_get_regnum
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);
}

static attr_value_t get_attr_connector(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	gpio_device *dev = conf_obj->obj;
	attr_value_t gpio_connector = SKY_make_attr_object(dev->gpio_connector);
	return gpio_connector;
}
static exception_t set_attr_connector(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	gpio_device *dev = conf_obj->obj;
	dev->gpio_connector = SKY_attr_object(*value);
	dev->connector_iface = (skyeye_gpio_connector_intf *)SKY_get_iface(dev->gpio_connector, SKYEYE_GPIO_CONNECTOR);
	update_gpio_on_line(conf_obj, dev->regs->pin);
	return No_exp;
}

static exception_t connector_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	gpio_device *dev = obj->obj;
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
	update_gpio_on_line(obj, dev->regs->pin);
	return No_exp;
}

static exception_t connector_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	gpio_device *dev = obj->obj;
	*obj2 = dev->gpio_connector;
	*port = NULL;
	return No_exp;
}



static exception_t gate_b_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	gpio_device *dev = obj->obj;
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
	gpio_device *dev = obj->obj;
	*obj2 = dev->gpio_gate_b;
	*port = NULL;
	return No_exp;
}


static exception_t gate_a_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	gpio_device *dev = obj->obj;
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
	gpio_device *dev = obj->obj;
	*obj2 = dev->gpio_gate_a;
	*port = NULL;
	return No_exp;
}

static gpio_register_attribute(conf_class_t* clss){
}
void init_gpio(){
	static skyeye_class_t class_data = {
		.class_name = "gpio",
		.class_desc = "gpio",
		.new_instance = new_gpio,
		.free_instance = free_gpio,
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
		.read = gpio_read,
		.write = gpio_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = gpio_get_regvalue_by_id,
		.get_regname_by_id = gpio_get_regname_by_id,
		.set_regvalue_by_id = gpio_set_regvalue_by_id,
		.get_regid_by_name = gpio_get_regid_by_name,
		.get_regnum = gpio_get_regnum
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
		},
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

	gpio_register_attribute(clss);
}
