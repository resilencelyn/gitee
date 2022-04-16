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
* @file Lvds_bus.c
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
#include "Lvds_bus.h"
#include <skyeye_dev_bus_intf.h>

#define Memory_Size 0x400

static int Lvds_bus_read_data(conf_object_t *opaque, conf_object_t* obj2, void* recv_buf, uint32_t dev_id, int length) {
	Lvds_bus_t *dev = opaque->obj;
	int i;
	if (obj2 == dev->device_list[dev_id]){
		for(i = 0; i < DEV_MAX_COUNT;i++){
			if (i == dev_id){
				if (dev->device_iface_list[i] != NULL){
					memcpy(recv_buf, (dev->send_buf[i] + dev->fifo_cnt), length);
				}
			}else{
				continue;
			}
		}
		dev->fifo_cnt++;
	}
	if (dev->fifo_cnt == (dev->data_length - 3))
	{
		dev->fifo_cnt = 1;
	}
	return 0; 
}

static void Lvds_bus_write_data(conf_object_t *opaque, conf_object_t* obj2, void *buf, uint32_t dev_id, int length) {
	Lvds_bus_t *dev = (Lvds_bus_t *)opaque->obj;
	int i;

	if (dev_id > DEV_MAX_COUNT){
		printf("In %s, dev_id %d is over the MAX_COUNT.", __FUNCTION__, dev_id);
		return;
	}

	if (obj2 == dev->device_list[dev_id]){
		for(i = 0; i < DEV_MAX_COUNT;i++){
			if (i == dev_id){
				continue;
			}else{
				if (dev->device_iface_list[i] != NULL){
					memcpy(dev->send_buf[i], buf, length);
					dev->device_iface_list[i]->set_state(dev->device_list[i], dev->send_buf[i]);
				}
			}
		}
	}
	return;
}


static int Lvds_bus_register_device(conf_object_t *opaque, conf_object_t *device, uint8_t dev_id) {
	Lvds_bus_t *dev = opaque->obj;
	if(dev_id > DEV_MAX_COUNT || dev_id < 0){
		return Error;
	}
	dev->device_list[dev_id] = device; 

	dev->device_iface_list[dev_id] = (Lvds_emif_intf*)SKY_get_iface(device, LVDS_EMIF_INTF);
	if (dev->device_iface_list[dev_id] == NULL){
		skyeye_log(Error_log, __FUNCTION__, "Get 'Lvds_emif_iface' interface from %s fail.", device->objname);
		return Error;
	}
	return OK;
}

static void Lvds_bus_unregister_device(conf_object_t *opaque, conf_object_t *device, uint8_t dev_id) {
	Lvds_bus_t *Lvds_bus = opaque->obj;
	return;
}

static conf_object_t* new_Lvds_bus(char* obj_name)
{
	int i;
	Lvds_bus_t *Lvds_bus = skyeye_mm_zero(sizeof(Lvds_bus_t));
	Lvds_bus->obj = new_conf_object(obj_name, Lvds_bus);
	Lvds_bus->recv_buf = (unsigned char *)skyeye_mm_zero(Memory_Size);
	Lvds_bus->fifo_cnt = 1;	
	Lvds_bus->data_length = 0;	
	for (i = 0; i < DEV_MAX_COUNT;i++)
		Lvds_bus->send_buf[i] = (unsigned char *)skyeye_mm_zero(Memory_Size);

	return Lvds_bus->obj;
}
static exception_t reset_Lvds_bus(conf_object_t* opaque, const char* args)
{
	Lvds_bus_t *Lvds_bus = opaque->obj;
	
	return No_exp;        
}

static exception_t free_Lvds_bus(conf_object_t* opaque)
{
	Lvds_bus_t *dev = opaque->obj;
	int i;
	for (i = 0; i < DEV_MAX_COUNT;i++)
		skyeye_free(dev->send_buf[i]);
	skyeye_free(dev->recv_buf);
	skyeye_free(dev->obj);
	skyeye_free(dev);
	return No_exp;
}
static exception_t Lvds_emif_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	Lvds_bus_t* dev = (Lvds_bus_t*)(obj->obj);
	dev->emif_obj = obj2;

	dev->emif_iface = (Lvds_emif_intf*)SKY_get_iface(dev->emif_obj, LVDS_EMIF_INTF);
	if (dev->emif_iface == NULL){
		if (dev->emif_obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", LVDS_EMIF_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", LVDS_EMIF_INTF, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;

}
 
static exception_t Lvds_emif_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	Lvds_bus_t* dev = (Lvds_bus_t*)(obj->obj);
	*obj2 = dev->emif_obj;
	*port = NULL;
	return No_exp;
}
void init_Lvds_bus(){
	static skyeye_class_t class_data = {
		.class_name = "Lvds_bus",
		.class_desc = "Lvds_bus",
		.new_instance = new_Lvds_bus,
		.reset_instance = reset_Lvds_bus,
		.free_instance = free_Lvds_bus,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_OFFCHIP,
	};
	conf_class_t* class = SKY_register_linker_class(class_data.class_name, &class_data);
	static const Lvds_bus_intf Lvds_bus_iface = {
		.read_data = Lvds_bus_read_data,
		.write_data = Lvds_bus_write_data,
		.register_device = Lvds_bus_register_device,
		.unregister_device = Lvds_bus_unregister_device,
	};
	SKY_register_iface(class, LVDS_BUS_INTF, &Lvds_bus_iface);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = LVDS_BUS_INTF,
			.iface = &Lvds_bus_iface,
		},
	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription ){
			.name = LVDS_EMIF_INTF,
			.set = Lvds_emif_set,
			.get = Lvds_emif_get,
		},
	};

	class_register_ifaces(class, ifaces);
	class_register_connects(class, connects);
}
