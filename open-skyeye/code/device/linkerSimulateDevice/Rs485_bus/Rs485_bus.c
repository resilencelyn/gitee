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
* @file Rs485_bus.c
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
#include <skyeye_fifo.h>
#include <skyeye_interface.h>
#include <memory_space.h>
#include <skyeye_obj.h>
#include <skyeye_emif.h>
#include <skyeye_attribute.h>
#include <skyeye_core.h>
#include <skyeye_device.h>
#include "Rs485_bus.h"
#include <skyeye_dev_bus_intf.h>

static int Rs485_bus_read_data(conf_object_t *opaque, conf_object_t* obj2, void* recv_buf, int length) {
	Rs485_bus_t *dev = opaque->obj;
	int i;
	for(i = 0; i < dev->dev_cnt;i++){
		if (obj2 != dev->device_list[i]){
			if (dev->device_iface_list[i] != NULL){
				memcpy(recv_buf, dev->send_buf[i], length);
			}
		}else{
			continue;
		}
	}
	return 0; 
}

static void Rs485_bus_write_data(conf_object_t *opaque, conf_object_t* obj2, void *buf, int length) {
	Rs485_bus_t *dev = (Rs485_bus_t *)opaque->obj;
	int i;

	for(i = 0; i < dev->dev_cnt;i++){
		if (obj2 != dev->device_list[i]){
			if (dev->device_iface_list[i] != NULL){
				memcpy(dev->send_buf[i], buf, length);
				dev->device_iface_list[i]->set_state(dev->device_list[i], buf, length);
			}
		}
	}
	return;
}


static int Rs485_bus_register_device(conf_object_t *opaque, conf_object_t *device) {
	Rs485_bus_t *dev = opaque->obj;
	dev->device_list[dev->dev_cnt] = device; 

	dev->device_iface_list[dev->dev_cnt] = (Rs485_emif_intf*)SKY_get_iface(device, RS485_EMIF_INTF);
	if (dev->device_iface_list[dev->dev_cnt] == NULL){
		skyeye_log(Error_log, __FUNCTION__, "Get 'Rs485_emif_iface' interface from %s fail.", device->objname);
		return Error;
	}
	dev->dev_cnt ++;
	return OK;
}

static void Rs485_bus_unregister_device(conf_object_t *opaque, conf_object_t *device) {
	Rs485_bus_t *Rs485_bus = opaque->obj;
	return;
}

static conf_object_t* new_Rs485_bus(char* obj_name)
{
	int i;
	Rs485_bus_t *Rs485_bus = skyeye_mm_zero(sizeof(Rs485_bus_t));
	Rs485_bus->obj = new_conf_object(obj_name, Rs485_bus);
	Rs485_bus->recv_buf = (char *)skyeye_mm_zero(32*sizeof(int));
	Rs485_bus->dev_cnt = 0;
	for (i = 0; i < DEV_MAX_COUNT;i++)
		Rs485_bus->send_buf[i] = (char *)skyeye_mm_zero(32*sizeof(int));

	return Rs485_bus->obj;
}
static exception_t reset_Rs485_bus(conf_object_t* opaque, const char* args)
{
	Rs485_bus_t *Rs485_bus = opaque->obj;
	
	return No_exp;        
}

static exception_t free_Rs485_bus(conf_object_t* opaque)
{
	Rs485_bus_t *dev = opaque->obj;
	int i;
	for (i = 0; i < DEV_MAX_COUNT;i++)
		skyeye_free(dev->send_buf[i]);
	skyeye_free(dev->recv_buf);
	skyeye_free(dev->obj);
	skyeye_free(dev);
	return No_exp;
}
static exception_t Rs485_emif_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	Rs485_bus_t* dev = (Rs485_bus_t*)(obj->obj);
	dev->emif_obj = obj2;

	dev->emif_iface = (Rs485_emif_intf*)SKY_get_iface(dev->emif_obj, RS485_EMIF_INTF);

	if (dev->emif_iface == NULL){
		if (dev->emif_obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", RS485_EMIF_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", RS485_EMIF_INTF, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t Rs485_emif_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	Rs485_bus_t* dev = (Rs485_bus_t*)(obj->obj);
	*obj2 = dev->emif_obj;
	*port = NULL;
	return No_exp;
}
void init_Rs485_bus(){
	static skyeye_class_t class_data = {
		.class_name = "Rs485_bus",
		.class_desc = "Rs485_bus",
		.new_instance = new_Rs485_bus,
		.reset_instance = reset_Rs485_bus,
		.free_instance = free_Rs485_bus,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_OFFCHIP,
	};
	conf_class_t* class = SKY_register_linker_class(class_data.class_name, &class_data);
	static const Rs485_bus_intf Rs485_bus_iface = {
		.read_data = Rs485_bus_read_data,
		.write_data = Rs485_bus_write_data,
		.register_device = Rs485_bus_register_device,
		.unregister_device = Rs485_bus_unregister_device,
	};
	SKY_register_iface(class, RS485_BUS_INTF, &Rs485_bus_iface);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = RS485_BUS_INTF, 
			.iface = &Rs485_bus_iface,
		},
	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription ){
			.name = RS485_EMIF_INTF,
			.set = Rs485_emif_set,
			.get = Rs485_emif_get,
		},
	};

	class_register_ifaces(class, ifaces);
	class_register_connects(class, connects);
}