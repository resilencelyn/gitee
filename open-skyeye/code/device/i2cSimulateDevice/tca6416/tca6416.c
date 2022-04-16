/* Copyright (C) 
* 2014 - Michael.Kang blackfin.kang@gmail.com
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
* @file tca6416.c
* @brief 
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2014-12-04
*/

#include <stdio.h>
#include <skyeye_config.h>
#include <skyeye_types.h>
#include <skyeye_sched.h> 
#include <skyeye_exec.h>
#include <skyeye_cell.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_dev_intf.h>
#include <skyeye_dev_bus_intf.h>
#include "skyeye_uart.h"
#include "skyeye_thread.h"
#include "tca6416.h"

static exception_t tca6416_read(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	tca6416_dev *dev=  (tca6416_dev *)opaque->obj;
	
	switch(offset)
	{
		case CMD_CONFIG_PORT :
			*(uint8_t *)buf = dev->regs.cmd_config_port;		 
			break;
		case CMD_WRITE_PORT :
			*(uint8_t *)buf = dev->regs.cmd_write_port;		 
			break;
		default:
			break;
	}       
	return  No_exp;
}

static exception_t tca6416_write(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{

	tca6416_dev *dev=  (tca6416_dev *)opaque->obj;
	uint8_t val = *(uint8_t*)buf;
	switch(offset)
	{
		case CMD_CONFIG_PORT :
			dev->regs.cmd_config_port = val;		 
			break;
		case CMD_WRITE_PORT :
			dev->regs.cmd_write_port = val;		 
			break;
		default:
			break;
	}
	return  No_exp;
}
static int tca6416_i2c_set_state(conf_object_t* opaque, i2c_device_state_t state, uint8_t address)
{
	tca6416_dev *dev=  (tca6416_dev *)opaque->obj;
	return 0;
}

static uint8_t tca6416_i2c_read_data(conf_object_t *opaque){
	tca6416_dev *dev=  (tca6416_dev *)opaque->obj;
	char regDate;
	int regAddr = dev->slave_buf[0];

	tca6416_read(opaque, regAddr, &regDate, 1);
	dev->cnt = 0;
	
	return regDate;
}

static uint8_t tca6416_i2c_write_data(conf_object_t *opaque, uint8_t value){
	tca6416_dev *dev=  (tca6416_dev *)opaque->obj;
	int regAddr, regDate;

	dev->slave_buf[dev->cnt] = value;
	dev->cnt ++;

	if (dev->cnt == 2){
		dev->cnt = 0;
		regAddr = dev->slave_buf[0];
		regDate = dev->slave_buf[1];
		tca6416_write(opaque, regAddr, &regDate, 1);
	}
		
	return 0;
}

static uint8_t tca6416_i2c_get_address(conf_object_t *opaque)
{
	tca6416_dev *dev=  (tca6416_dev *)opaque->obj;
	return 0;
}

static conf_object_t* new_tca6416(char* obj_name)
{
	tca6416_dev* tca6416 = skyeye_mm_zero(sizeof(tca6416_dev));

	tca6416->obj = new_conf_object(obj_name, tca6416);
	tca6416->cnt = 0;
	tca6416->slave_buf[0] = 0;
	tca6416->slave_buf[1] = 0;

	return tca6416->obj;
}

static exception_t reset_tca6416(conf_object_t* opaque, const char* args)
{
	tca6416_dev *dev = opaque->obj;
	memset(&(dev->regs), 0, sizeof(dev->regs));
	return No_exp;        
}


static exception_t free_tca6416(conf_object_t* opaque)
{
	tca6416_dev *dev = opaque->obj;
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}

static exception_t get_attr_i2c_master(conf_object_t *obj, conf_object_t **obj2, const char **port, int index){
	tca6416_dev *dev = obj->obj;
	*obj2 = dev->i2c_bus_obj;
	*port = NULL;
	return No_exp;
}
static exception_t set_attr_i2c_master(conf_object_t *obj, conf_object_t *obj2, const char *port, int index){
	tca6416_dev *dev = obj->obj;
	dev->i2c_bus_obj = obj2;
	dev->i2c_bus_iface = (i2c_bus_intf *)SKY_get_iface(dev->i2c_bus_obj, I2C_BUS_INTF_NAME);

	if (dev->i2c_bus_iface == NULL){
		if (dev->i2c_bus_obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", I2C_BUS_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", I2C_BUS_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

void init_tca6416(){
	static skyeye_class_t class_data = {
		.class_name = "tca6416",
		.class_desc = "tca6416",
		.new_instance = new_tca6416,
		.reset_instance = reset_tca6416,
		.free_instance = free_tca6416,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_OFFCHIP,
	};
	conf_class_t* class = SKY_register_device_class(class_data.class_name, &class_data);
	/* Register io function to the object */
	static const memory_space_intf io_memory = {
		.read = tca6416_read,
		.write = tca6416_write,
	};
	SKY_register_iface(class, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const i2c_device_intf i2c_dev_iface = {
		.set_state = tca6416_i2c_set_state,
		.write_data = tca6416_i2c_write_data,
		.read_data = tca6416_i2c_read_data,
		.get_address = tca6416_i2c_get_address,
	};
	SKY_register_iface(class, I2C_DEVICE_INTF_NAME, &i2c_dev_iface);

	static const struct InterfaceDescription ifaces[] =
	{
		(struct InterfaceDescription)
		{
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory
		},
		(struct InterfaceDescription)
		{
			.name = I2C_DEVICE_INTF_NAME,
			.iface = &i2c_dev_iface
		},
	};
	static const struct ConnectDescription connects[] =
	{
		/* ConnectDescription */
		(struct ConnectDescription) {
			.name = I2C_BUS_INTF_NAME,
			.set = set_attr_i2c_master,
			.get = get_attr_i2c_master,
		}
	};
	class_register_ifaces(class, ifaces);
	class_register_connects(class, connects);
}

