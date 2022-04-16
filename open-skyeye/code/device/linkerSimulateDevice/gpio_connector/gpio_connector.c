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
* @file gpio_connector.c
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
#include <skyeye_dev_intf.h>
#define DEBUG
#include <skyeye_log.h>

#include "gpio_connector.h"

static char *type[] = {
	"eq",
	"and",
	"or",
	"not",
	"nand",
	"nor",
	"xor",
	NULL
};

gate_type_t get_gate_type(char *type_string){
	int i;
	for(i = 0; type[i] != NULL; i++){
		if(!strcmp(type_string, type[i]))
			break;
	}
	return  (gate_type_t)i;
}

char *get_gate_string(gate_type_t type_id){
	int i;
	i = (int)type_id;
	return  type[i];
}


exception_t output_value(conf_object_t* obj){
	gpio_connector_device *dev = obj->obj;
	int i;
	if(dev->gpio_num > 0){
		for(i = 0; i < dev->gpio_num; i++){
			conf_object_t *gpio =  dev->gpio_group[i];
			skyeye_gpio_connector_intf *iface = (skyeye_gpio_connector_intf*)SKY_get_iface(gpio, SKYEYE_GPIO_CONNECTOR);
		if(iface != NULL)
			iface->update(gpio, dev->output_value);
		}
	}
	if(dev->gate_a != NULL){
		dev->gate_a_iface->update(dev->gate_a, dev->output_value);
	}
	if(dev->gate_b != NULL){
		dev->gate_b_iface->update(dev->gate_b, dev->output_value);
	}
	return No_exp;
}

exception_t logic_operation(conf_object_t *obj){
	gpio_connector_device *dev = obj->obj;
	switch(dev->gate_type){
		case EQ_GATE:
			OUTPUT = INPUT_UPDATE;
			break;
		case AND_GATE:
			OUTPUT = INPUT_A & INPUT_B;
			break;
		case OR_GATE:
			OUTPUT = INPUT_A | INPUT_B;
			break;
		case NOT_GATE:
			OUTPUT = ~INPUT_NOT;
			break;
		case NAND_GATE:
			OUTPUT = ~(INPUT_A & INPUT_B);
			break;
		case NOR_GATE:
			OUTPUT = ~(INPUT_A | INPUT_B);
			break;
		case XOR_GATE:
			OUTPUT = INPUT_A ^ INPUT_B;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Unknown logic gate type id %d.", dev->gate_type);
	}
	return  Not_found_exp;
}


exception_t  add_gpio(conf_object_t *obj, conf_object_t *gpio){
	gpio_connector_device *dev = obj->obj;
	dev->gpio_group = realloc(dev->gpio_group, sizeof(conf_object_t *) * (dev->gpio_num + 1));
	dev->gpio_group[dev->gpio_num] = gpio;
	dev->gpio_num++;
	return No_exp;
}

exception_t  update_gpio(conf_object_t *obj, uint32_t regvalue){
	gpio_connector_device *dev = obj->obj;
	dev->update_value = regvalue;
	logic_operation(obj);
	output_value(obj);
	return No_exp;
}


exception_t  update_gate_a(conf_object_t *obj, uint32_t regvalue){
	gpio_connector_device *dev = obj->obj;
	dev->gate_a_value = regvalue;
	if(dev->gate_type == NOT_GATE)
		dev->NOT_gate_value = regvalue;
	logic_operation(obj);
	output_value(obj);
	return No_exp;
}

exception_t  update_gate_b(conf_object_t *obj, uint32_t regvalue){
	gpio_connector_device *dev = obj->obj;
	dev->gate_b_value = regvalue;
	if(dev->gate_type == NOT_GATE)
		dev->NOT_gate_value = regvalue;
	logic_operation(obj);
	output_value(obj);

	return No_exp;
}
static conf_object_t* new_gpio_connector(char* obj_name){
	gpio_connector_device* dev = skyeye_mm_zero(sizeof(gpio_connector_device));
	dev->obj = new_conf_object(obj_name, dev);
	dev->gpio_group = NULL;
	dev->gpio_num = 0;
	dev->gate_type = EQ_GATE;
	dev->update_value = 0;
	dev->gate_a_value = 0;
	dev->gate_b_value = 0;
	return dev->obj;
}
static void free_gpio_connector(conf_object_t* conf_obj){
	gpio_connector_device *dev = conf_obj->obj;
    skyeye_free(dev->obj);
    skyeye_free(dev);
	return;	
}


static attr_value_t get_attr_gate_type(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	gpio_connector_device *dev = conf_obj->obj;
	char *type_string = get_gate_string(dev->gate_type);
	return SKY_make_attr_string(type_string);
}

static exception_t set_attr_gate_type(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	gpio_connector_device *dev = conf_obj->obj;
	char *type_string  = SKY_attr_string(*value);
	dev->gate_type = get_gate_type(type_string);
	logic_operation(conf_obj);
	output_value(conf_obj);
	return No_exp;
}
static exception_t gate_a_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	gpio_connector_device *dev = obj->obj;
	dev->gate_a = obj2;
	dev->gate_a_iface = (skyeye_gate_a_intf*)SKY_get_iface(dev->gate_a, LOGIC_GATE_A);
	if(dev->gate_a_iface == NULL)
		skyeye_log(Error_log, __FUNCTION__, "%s get %s interface fail\n", obj->objname, LOGIC_GATE_A);
	logic_operation(obj);
	output_value(obj);
	return No_exp;
}
 
static exception_t gate_a_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	gpio_connector_device *dev = obj->obj;
	*obj2 = dev->gate_a;
	*port = NULL;
	return No_exp;
}

static exception_t gate_b_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	gpio_connector_device *dev = obj->obj;
	dev->gate_b = obj2;
	dev->gate_b_iface = (skyeye_gate_b_intf*)SKY_get_iface(dev->gate_b, LOGIC_GATE_B);
	if(dev->gate_b_iface == NULL)
		skyeye_log(Error_log, __FUNCTION__, "%s get %s interface fail\n", obj->objname, LOGIC_GATE_B);
	logic_operation(obj);
	output_value(obj);
	return No_exp;
}
 
static exception_t gate_b_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	gpio_connector_device *dev = obj->obj;
	*obj2 = dev->gate_b;
	*port = NULL;
	return No_exp;
}

static exception_t gpio_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	gpio_connector_device *dev = obj->obj;
	add_gpio(obj, obj2);
	logic_operation(obj);
	output_value(obj);

	return No_exp;
}
 
static exception_t gpio_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	gpio_connector_device *dev = obj->obj;
	*obj2 = NULL;
	*port = NULL;
	return No_exp;
}





static void gpio_connector_register_attribute(conf_class_t* clss){

	SKY_register_attribute(clss, "gate_type", get_attr_gate_type, NULL, set_attr_gate_type, NULL, SKY_Attr_Optional, "string", "GPIO gate type");
}


void init_gpio_connector(){
	static skyeye_class_t class_data = {
		.class_name = "gpio_gate",
		.class_desc = "gpio_gate",
		.new_instance = new_gpio_connector,
		.free_instance = free_gpio_connector,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_OFFCHIP,
	};

	conf_class_t* clss = SKY_register_linker_class(class_data.class_name, &class_data);

	static const skyeye_gpio_connector_intf iface = {
		.update =  update_gpio
	};
	SKY_register_iface(clss, SKYEYE_GPIO_CONNECTOR, &iface);

	static const skyeye_gate_a_intf gate_a_iface = {
		.update = update_gate_a
	};
	SKY_register_iface(clss, LOGIC_GATE_A, &gate_a_iface);

	static const skyeye_gate_b_intf gate_b_iface = {
		.update = update_gate_b
	};
	SKY_register_iface(clss, LOGIC_GATE_B, &gate_b_iface);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = SKYEYE_GPIO_CONNECTOR,
			.iface = &iface,
		},
		(struct InterfaceDescription) {
			.name = LOGIC_GATE_A,
			.iface = &gate_a_iface,
		},
		(struct InterfaceDescription) {
			.name = LOGIC_GATE_B, 
			.iface = &gate_b_iface,
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
			.set = gpio_set,
			.get = gpio_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	gpio_connector_register_attribute(clss);
}
