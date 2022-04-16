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
* @file spi_bus.c
* @brief 
* @author zyuming zyumingfit@gmail.com
* @date 2013-11-27
*/

#include <stdio.h>
#include <skyeye_class.h>
#include <skyeye_iface.h>
#include <skyeye_types.h>
#include <skyeye_mm.h>
#include <skyeye_class.h>
#include <skyeye_dev_bus_intf.h>
#include <skyeye_dev_intf.h>
#include <skyeye_attribute.h>
#include "spi_bus.h"
#if 0
static exception_t spi_bus_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	spi_bus_t *spi_bus = opaque->obj;
	if(!strncmp(attr_name, "spi_slave", strlen("spi_slave"))){
		spi_bus_intf *op_spi_bus = (spi_bus_intf *)SKY_get_interface(opaque, SPI_BUS_INTF_NAME);
		op_spi_bus->register_device(opaque, value->u.object);
	}else{
		printf("parameter error\n");
		return Invarg_exp;
	}
	return No_exp;
}
#endif
int spi_bus_connect_slave(conf_object_t *opaque, char *slave_name)
{
	spi_bus_t *spi_bus = opaque->obj;
	int i;
	if(spi_bus->current_slave){
		return BUS_BUSY;
	}
	for(i = 0; i < spi_bus->slave_count; i++){
		if(!strncmp(spi_bus->slave_device_list[i]->objname, slave_name, strlen(slave_name))){
			spi_bus->current_slave =  spi_bus->slave_device_list[i];
			if(spi_bus->current_slave == NULL){
				return SLAVE_ERROR;
			}
			spi_bus->slave_iface = (spi_device_intf *)SKY_get_iface(spi_bus->current_slave, SPI_DEVICE_INTF_NAME);
			break;
		}
		
	}
	return spi_bus->slave_iface->connect_master(spi_bus->current_slave);
}


int spi_bus_disconnect_slave(conf_object_t *opaque, char *slave_name)
{
	spi_bus_t *spi_bus = (spi_bus_t *)opaque->obj;
	if(!spi_bus->current_slave) {
		return NO_COM;
	}
	spi_bus->current_slave = NULL;
	spi_bus->slave_iface = NULL;	
	return 0;
}


void spi_slave_request(conf_object_t *opaque, int first, int last, char *buf, int len, char *feedback)
{
	spi_bus_t *spi_bus = (spi_bus_t *)opaque->obj;
	if(spi_bus->slave_iface) {
		spi_bus->slave_iface->spi_request(spi_bus->current_slave, first, last, buf, len, feedback);
	}
}

static int spi_bus_register_device(conf_object_t *opaque, conf_object_t *device) {
	spi_bus_t *spi_bus = opaque->obj;
	spi_bus->slave_device_list[spi_bus->slave_count] = device; 
	spi_bus->slave_count += 1;
	return 0;
}

static void spi_bus_unregister_device(conf_object_t *opaque, conf_object_t *device, uint8_t address, uint8_t mask) {
	spi_bus_t *spi_bus = opaque->obj;
}


static conf_object_t* new_spi_bus(char* obj_name)
{
	spi_bus_t *spi_bus = skyeye_mm_zero(sizeof(spi_bus_t));
	spi_bus->obj = new_conf_object(obj_name, spi_bus);
	spi_bus->current_slave = NULL;
	spi_bus->slave_iface = NULL;
	spi_bus->slave_count = 0;
#if 0
	spi_bus_intf* spi_iface = skyeye_mm_zero(sizeof(spi_bus_intf));
	spi_iface->conf_obj = spi_bus->obj;
	spi_iface->spi_slave_request = spi_slave_request;
	spi_iface->connect_slave = spi_bus_connect_slave;
	spi_iface->disconnect_slave = spi_bus_disconnect_slave;
	spi_iface->register_device = spi_bus_register_device;
	spi_iface->unregister_device = spi_bus_unregister_device;
	SKY_register_interface((void*)spi_iface, obj_name, SPI_BUS_INTF_NAME);
#endif
	return spi_bus->obj;
}

static exception_t reset_spi_bus(conf_object_t* opaque, const char* args)
{
	spi_bus_t *spi_bus = opaque->obj;
	
	return No_exp;        
}

static exception_t free_spi_bus(conf_object_t* opaque)
{
	return No_exp;
}
static exception_t set_attr_spi_slave(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){

	spi_bus_t *spi_bus = conf_obj->obj;
	spi_bus_intf *op_spi_bus = (spi_bus_intf *)SKY_get_iface(conf_obj, SPI_BUS_INTF_NAME);
	op_spi_bus->register_device(conf_obj, value->u.object);

	return No_exp;
}

static void spi_bus_register_attribute(conf_class_t* class){
	SKY_register_attribute(class, "spi_slave", NULL, NULL, set_attr_spi_slave, NULL, SKY_Attr_Optional, "object", "spi slave");
	return;
}
static void spi_bus_register_interface(conf_class_t* class){
	static const spi_bus_intf spi_iface = {
		.spi_slave_request = spi_slave_request,
		.connect_slave = spi_bus_connect_slave,
		.disconnect_slave = spi_bus_disconnect_slave,
		.register_device = spi_bus_register_device,
		.unregister_device = spi_bus_unregister_device,
	};
	SKY_register_iface(class, SPI_BUS_INTF_NAME, &spi_iface);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = SPI_BUS_INTF_NAME,
			.iface = &spi_iface,
		}
 	};
	class_register_ifaces(class, ifaces);
	return;
}
void init_spi_bus(){
	static skyeye_class_t class_data = {
		.class_name = "spi_bus",
		.class_desc = "spi_bus",
		.new_instance = new_spi_bus,
		.reset_instance = reset_spi_bus,
		.free_instance = free_spi_bus,
		.get_attr = NULL,
		.set_attr = NULL,
	};
	conf_class_t* class = SKY_register_linker_class(class_data.class_name, &class_data);
	spi_bus_register_interface(class);
	spi_bus_register_attribute(class);
}
