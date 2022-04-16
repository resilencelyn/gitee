/* Copyright (C) 
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
* @file i2c_bus.c
* @brief 
* @author zyuming zyumingfit@gmail.com
* @date 2013-11-27
*/
#include <stdio.h>
#include <skyeye_class.h>
#include <skyeye_iface.h>
#include <skyeye_types.h>
#include <skyeye_mm.h>
#include <skyeye_interface.h>
#include <skyeye_class.h>
#include <skyeye_dev_bus_intf.h>
#include <skyeye_dev_intf.h>
#include <skyeye_attribute.h>
#include "i2c_bus.h"
#define DEBUG
#include <skyeye_log.h>
#if 0
static exception_t i2c_bus_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	i2c_bus_t *i2c_bus = opaque->obj;
	uint8_t address;
	if(!strncmp(attr_name, "i2c_slave", strlen("i2c_slave"))){
		i2c_bus_intf *op_i2c_bus = (i2c_bus_intf *)SKY_get_interface(opaque, I2C_BUS_INTF_NAME);
		i2c_device_intf *op_i2c_dev = (i2c_device_intf *)SKY_get_interface(value->u.object, I2C_DEVICE_INTF_NAME);
		address = op_i2c_dev->get_address(value->u.object);
		op_i2c_bus->register_device(opaque, value->u.object, address, 0, 0);
	}else{
		printf("parameter error\n");
		return Invarg_exp;
	}
	return No_exp;
}
#endif
static int i2c_bus_start(conf_object_t *opaque, uint8_t address) {
	i2c_bus_t *i2c_bus = opaque->obj;
	int rw;
	if(i2c_bus->current_slave){
		return BUS_BUSY;
	}
	if(address > I2C_DEV_MAX_COUNT || address < 0){
		return ADDR_ERROR;
	}
	i2c_bus->current_slave = i2c_bus->slave_device_list[address & 0xfe];
	if(i2c_bus->current_slave == NULL){
		return SLAVE_ERROR;
	}
	i2c_bus->slave_iface = (i2c_device_intf *)SKY_get_iface(i2c_bus->current_slave, I2C_DEVICE_INTF_NAME);
	if (i2c_bus->slave_iface == NULL){
		skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", I2C_DEVICE_INTF_NAME, i2c_bus->current_slave->objname);
	}

	rw = address & 0x1;
	return i2c_bus->slave_iface->set_state(i2c_bus->current_slave, rw, (address & 0xfe));
}

static int i2c_bus_stop(conf_object_t *opaque) {
	i2c_bus_t *i2c_bus = (i2c_bus_t *)opaque->obj;

	if(!i2c_bus->current_slave) {
		return NO_COM;
	}

	i2c_bus->current_slave = NULL;
	i2c_bus->slave_iface = NULL;	
	return 0;
}

static uint8_t i2c_bus_read_data(conf_object_t *opaque) {
	i2c_bus_t *i2c_bus = (i2c_bus_t *)opaque->obj;

	if(i2c_bus->slave_iface) {
		return i2c_bus->slave_iface->read_data(i2c_bus->current_slave);
	}
	return 0;
}

static void i2c_bus_write_data(conf_object_t *opaque, uint8_t value) {
	i2c_bus_t *i2c_bus = (i2c_bus_t *)opaque->obj;

	if(i2c_bus->slave_iface) {
		i2c_bus->slave_iface->write_data(i2c_bus->current_slave, value);
	}
}

static int i2c_bus_register_device(conf_object_t *opaque, conf_object_t *device, uint8_t address, uint8_t mask, i2c_device_flag_t flags) {
	i2c_bus_t *i2c_bus = opaque->obj;
	if(address > I2C_DEV_MAX_COUNT || address < 0){
		return ADDR_ERROR;
	}
	i2c_bus->slave_device_list[address] = device; 
	return 0;
}

static void i2c_bus_unregister_device(conf_object_t *opaque, conf_object_t *device, uint8_t address, uint8_t mask) {
	i2c_bus_t *i2c_bus = opaque->obj;

}


static conf_object_t* new_i2c_bus(char* obj_name)
{
	i2c_bus_t *i2c_bus = skyeye_mm_zero(sizeof(i2c_bus_t));
	i2c_bus->obj = new_conf_object(obj_name, i2c_bus);
	i2c_bus->current_slave = NULL;
	i2c_bus->slave_iface = NULL;
	i2c_bus->slave_iface = 0;
#if 0
	i2c_bus_intf* i2c_iface = skyeye_mm_zero(sizeof(i2c_bus_intf));
	i2c_iface->conf_obj = i2c_bus->obj;
	i2c_iface->start = i2c_bus_start;
	i2c_iface->stop = i2c_bus_stop;
	i2c_iface->read_data = i2c_bus_read_data;
	i2c_iface->write_data = i2c_bus_write_data;
	i2c_iface->register_device = i2c_bus_register_device;
	i2c_iface->unregister_device = i2c_bus_unregister_device;

	SKY_register_interface((void*)i2c_iface, obj_name, I2C_BUS_INTF_NAME);
#endif
	return i2c_bus->obj;
}
static exception_t i2c_slave_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	i2c_bus_t *dev = obj->obj;
	dev->slave_device_list[index & 0xfe] = obj2;
	i2c_device_intf *op_i2c_dev = (i2c_device_intf *)SKY_get_iface(obj2, I2C_DEVICE_INTF_NAME);
	if (op_i2c_dev == NULL){
		if (obj2 == NULL)
			skyeye_log(Error_log, __func__, "Can't get %s interface from NULL object\n", I2C_DEVICE_INTF_NAME);
		else
			skyeye_log(Error_log, __func__, "Can't get %s interface from %s\n", I2C_DEVICE_INTF_NAME, obj2->objname);
	}
	return No_exp;
}

static exception_t i2c_slave_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	i2c_bus_t *dev = obj->obj;
	*obj2 = dev->slave_device_list[index & 0xfe];
	*port = NULL;
	return No_exp;
}
/*
static exception_t set_attr_i2c_slave(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	i2c_bus_t *i2c_bus = conf_obj->obj;
        uint8_t address;
	i2c_bus_intf *op_i2c_bus = (i2c_bus_intf *)SKY_get_iface(conf_obj, I2C_BUS_INTF_NAME);
	i2c_device_intf *op_i2c_dev = (i2c_device_intf *)SKY_get_iface(value->u.object, I2C_DEVICE_INTF_NAME);
	address = op_i2c_dev->get_address(value->u.object);
	op_i2c_bus->register_device(conf_obj, value->u.object, address, 0, 0);
        return No_exp;
}
*/
/*
static void i2c_bus_register_attribute(conf_class_t* class){
	SKY_register_attribute(class, "i2c_slave", NULL, NULL, set_attr_i2c_slave, NULL, SKY_Attr_Optional, "o", " i2c slave");
	return;
}
*/
static exception_t reset_i2c_bus(conf_object_t* opaque, const char* args)
{
	i2c_bus_t *i2c_bus = opaque->obj;
	
	return No_exp;        
}

static exception_t free_i2c_bus(conf_object_t* opaque)
{

	return No_exp;
}
void init_i2c_bus(){
	static skyeye_class_t class_data = {
		.class_name = "i2c_bus",
		.class_desc = "i2c_bus",
		.new_instance = new_i2c_bus,
		.reset_instance = reset_i2c_bus,
		.free_instance = free_i2c_bus,
		.get_attr = NULL,
		.set_attr = NULL,
	};

	conf_class_t* class = SKY_register_linker_class(class_data.class_name, &class_data);
	static const i2c_bus_intf i2c_iface = {
		.start = i2c_bus_start,
		.stop = i2c_bus_stop,
		.read_data = i2c_bus_read_data,
		.write_data = i2c_bus_write_data,
		.register_device = i2c_bus_register_device,
		.unregister_device = i2c_bus_unregister_device,
	};
	SKY_register_iface(class, I2C_BUS_INTF_NAME, &i2c_iface);

	static const struct InterfaceDescription ifaces[] =
	{
		(struct InterfaceDescription)
		{
			.name = I2C_BUS_INTF_NAME,
			.iface = &i2c_iface
		},
	};

	static const struct ConnectDescription connects[] =
	{
		/* ConnectDescription */
		(struct ConnectDescription) {
			.name = I2C_DEVICE_INTF_NAME,
			.set = i2c_slave_set,
			.get = i2c_slave_get,
		}
	};

	class_register_ifaces(class, ifaces);
	class_register_connects(class, connects);
//	i2c_bus_register_attribute(class);
}
