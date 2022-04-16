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
* @file gpio_c64x.c
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

#include "gpio_c64x.h"

exception_t  update_gpio_c64x(conf_object_t *obj, uint32_t regvalue){
	gpio_c64x_device *dev = obj->obj;
	gpio_c64x_reg_t* regs = dev->regs;
	regs->OUT_DATA = regvalue;
	return No_exp;
}

exception_t  update_gpio_c64x_on_line(conf_object_t *obj, uint32_t regvalue){
	gpio_c64x_device *dev = obj->obj;
	if(dev->gpio_c64x_connector && dev->connector_iface){
		dev->connector_iface->update(dev->gpio_c64x_connector, dev->regs->OUT_DATA);
	}
	if(dev->gpio_c64x_gate_a && dev->gate_a_iface){
		dev->gate_a_iface->update(dev->gpio_c64x_gate_a, dev->regs->OUT_DATA);
	}
	if(dev->gpio_c64x_gate_b && dev->gate_b_iface){
		dev->gate_b_iface->update(dev->gpio_c64x_gate_b, dev->regs->OUT_DATA);
	}
	return No_exp;
}

static char* gpio_c64x_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t gpio_c64x_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	gpio_c64x_device *dev = conf_obj->obj;
	gpio_c64x_reg_t* regs = dev->regs;
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t gpio_c64x_set_regvalue_by_id(conf_object_t* conf_obj, int32_t value, uint32_t id){
	gpio_c64x_device *dev = conf_obj->obj;
	gpio_c64x_reg_t* regs = dev->regs;
	*((uint32_t*)regs + id) = value;
	switch(id){
		case 3:
			regs->OUT_DATA |= value;
			regs->SET_DATA = 0;
			break;
		case 4:
			regs->OUT_DATA &= ~value;
			regs->CLR_DATA = 0;
			break;
		default:
			break;
	}
	update_gpio_c64x_on_line(conf_obj, regs->OUT_DATA);
	return 0;
}

uint32_t gpio_c64x_get_regid_by_name(conf_object_t* opaque, char* name){
	uint32_t regnum = 0, i = 0;
	gpio_c64x_device *dev = opaque->obj;
	gpio_c64x_reg_t* regs = dev->regs;
	regnum = sizeof(gpio_c64x_reg_t)/ 4;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

uint32_t gpio_c64x_get_regnum(conf_object_t* opaque){
	gpio_c64x_device *dev = opaque->obj;
	gpio_c64x_reg_t* regs = dev->regs;
	return  sizeof(gpio_c64x_reg_t) / 4;
}

void call_signal_func(conf_object_t* opaque, int int_number){
	gpio_c64x_device *dev = opaque->obj;
	gpio_c64x_reg_t* regs = dev->regs;
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
static exception_t gpio_c64x_read(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	gpio_c64x_device *dev = opaque->obj;
	gpio_c64x_reg_t* regs = dev->regs;
	switch (offset) {
		case 0x0:
			*(uint32_t *)buf = regs->BINTEN;
			break;
		case 0x4:
			*(uint32_t *)buf = regs->IODIR;
			break;
		case 0x8:
			*(uint32_t *)buf = regs->OUT_DATA;
			break;
		case 0xc:
			*(uint32_t *)buf = regs->SET_DATA;
			break;
		case 0x10:
			*(uint32_t *)buf = regs->CLR_DATA;
			break;
		case 0x14:
			*(uint32_t *)buf = regs->IN_DATA;
			break;
		case 0x18:
			*(uint32_t *)buf = regs->SET_RIS_TRIG;
			break;
		case 0x1c:
			*(uint32_t *)buf = regs->CLR_RIS_TRIG;
			break;
		case 0x20:
			*(uint32_t *)buf = regs->SET_FAL_TRIG;
			break;
		case 0x24:
			*(uint32_t *)buf = regs->CLR_FAL_TRIG;
			break;
		default:
			printf("In %s, read error value from offset 0x%x\n", __FUNCTION__, offset);
			break;
	}

	return No_exp;
}

static exception_t gpio_c64x_write(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	gpio_c64x_device *dev = opaque->obj;
	gpio_c64x_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t*) buf;
	int int_number = 0;
	switch (offset) {
		case 0x0:
			regs->BINTEN = val;
			break;
		case 0x4:
			regs->IODIR = val;
			break;
		case 0x8:
			regs->OUT_DATA = val;
			update_gpio_c64x_on_line(opaque, regs->OUT_DATA);
			break;
		case 0xc:
			regs->OUT_DATA |= val;
			break;
		case 0x10:
			regs->OUT_DATA &= ~val;
			break;
		case 0x14:
			regs->IN_DATA = val;
			break;
		case 0x18:
			regs->SET_RIS_TRIG = val;
			if (EN == 1){
				while (val & 0x1 == 0){
					int_number ++;
					val >>= 1;
				}
				if (dev->core_signal){
					call_signal_func(opaque, int_number);			
				}
			}
			break;
		case 0x1c:
			regs->CLR_RIS_TRIG = val;
			break;
		case 0x20:
			regs->SET_FAL_TRIG = val;
			if (EN == 1){
				while (val & 0x1 == 0){
					int_number ++;
					val >>= 1;
				}
				if (dev->core_signal){
					call_signal_func(opaque, int_number);			
				}
			}
			break;
		case 0x24:
			regs->CLR_FAL_TRIG = val;
			break;
		default:
			printf("In %s, read error value from offset 0x%x\n", __FUNCTION__, offset);
			break;
	}
	return No_exp;
}

static conf_object_t* new_gpio_c64x(char* obj_name){
	gpio_c64x_device* dev = skyeye_mm_zero(sizeof(gpio_c64x_device));
	gpio_c64x_reg_t* regs =  skyeye_mm_zero(sizeof(gpio_c64x_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	dev->regs = regs;
	dev->regs->IODIR = 0xFFFF;
	dev->gpio_c64x_connector = NULL;
	dev->connector_iface = NULL;
	dev->gpio_c64x_gate_a = NULL;
	dev->gate_a_iface = NULL;
	dev->gpio_c64x_gate_b = NULL;
	dev->gate_b_iface = NULL;

	return dev->obj;
}
static void free_gpio_c64x(conf_object_t* conf_obj){
	gpio_c64x_device* dev = conf_obj->obj;
	skyeye_free(dev->obj);
	skyeye_free(dev);
	return;	
}

static attr_value_t get_attr_connector(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	gpio_c64x_device *dev = conf_obj->obj;
	attr_value_t gpio_c64x_connector = SKY_make_attr_object(dev->gpio_c64x_connector);
	return gpio_c64x_connector;
}
static exception_t set_attr_connector(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	gpio_c64x_device *dev = conf_obj->obj;
	dev->gpio_c64x_connector = SKY_attr_object(*value);
	dev->connector_iface = (skyeye_gpio_connector_intf *)SKY_get_iface(dev->gpio_c64x_connector, SKYEYE_GPIO_CONNECTOR);
	update_gpio_c64x_on_line(conf_obj, dev->regs->OUT_DATA);
	return No_exp;
}

static exception_t connector_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	gpio_c64x_device *dev = obj->obj;
	dev->gpio_c64x_connector = obj2;
	dev->connector_iface = (skyeye_gpio_connector_intf *)SKY_get_iface(dev->gpio_c64x_connector, SKYEYE_GPIO_CONNECTOR);
	if (dev->connector_iface == NULL){
		if (dev->gpio_c64x_connector == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SKYEYE_GPIO_CONNECTOR);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SKYEYE_GPIO_CONNECTOR, obj2->objname);

		return Not_found_exp;
	}
	update_gpio_c64x_on_line(obj, dev->regs->OUT_DATA);
	return No_exp;
}
 
static exception_t connector_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	gpio_c64x_device *dev = obj->obj;
	*obj2 = dev->gpio_c64x_connector;
	*port = NULL;
	return No_exp;
}

static exception_t gate_b_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	gpio_c64x_device *dev = obj->obj;
	dev->gpio_c64x_gate_b = obj2;
	dev->gate_b_iface = (skyeye_gate_b_intf*)SKY_get_iface(dev->gpio_c64x_gate_b, LOGIC_GATE_B);
	if (dev->gate_b_iface == NULL){
		if (dev->gpio_c64x_gate_b == NULL)
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
	gpio_c64x_device *dev = obj->obj;
	*obj2 = dev->gpio_c64x_gate_b;
	*port = NULL;
	return No_exp;
}

static exception_t gate_a_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	gpio_c64x_device *dev = obj->obj;
	dev->gpio_c64x_gate_a = obj2;
	dev->gate_a_iface = (skyeye_gate_a_intf*)SKY_get_iface(dev->gpio_c64x_gate_a, LOGIC_GATE_A);
	if (dev->gate_a_iface == NULL){
		if (dev->gpio_c64x_gate_a == NULL)
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
	gpio_c64x_device *dev = obj->obj;
	*obj2 = dev->gpio_c64x_gate_a;
	*port = NULL;
	return No_exp;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	gpio_c64x_device *dev = obj->obj;
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
	gpio_c64x_device *dev = obj->obj;
	*obj2 = dev->core;
	*port = NULL;
	return No_exp;
}
void init_gpio_c64x(){
	static skyeye_class_t class_data = {
		.class_name = "c64x_gpio",
		.class_desc = "c64x_gpio",
		.new_instance = new_gpio_c64x,
		.free_instance = free_gpio_c64x,
		.get_attr = NULL,
		.set_attr = NULL
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const skyeye_gpio_connector_intf iface = {
		.update =  update_gpio_c64x
	};
	SKY_register_iface(clss, SKYEYE_GPIO_CONNECTOR, &iface);

	static const memory_space_intf io_memory = {
		.read = gpio_c64x_read,
		.write = gpio_c64x_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = gpio_c64x_get_regvalue_by_id,
		.get_regname_by_id = gpio_c64x_get_regname_by_id,
		.set_regvalue_by_id = gpio_c64x_set_regvalue_by_id,
		.get_regid_by_name = gpio_c64x_get_regid_by_name,
		.get_regnum = gpio_c64x_get_regnum
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
		(struct ConnectDescription ){
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		},

	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}
