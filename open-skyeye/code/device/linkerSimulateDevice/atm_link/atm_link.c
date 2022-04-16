/* Copyright (C)
 * zyumingfit@gmail.com
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
/*
 * @file atm_link.c
 * @brief The implementation of device
 * @author zyumingfit@gmail.com
 * @version 01.00
 */

#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_iface.h>
#include <pthread.h>
#define DEBUG
#include <skyeye_log.h>
#include <skyeye_dev_bus_intf.h>
#include <skyeye_dev_intf.h>
#include <skyeye_dds.h>

#include "atm_link.h"

#define BUF(num)  ((unsigned long long)(buf[num]))  

static exception_t init_atm_device_list(atm_link_device_t *dev){
	dev->device_list.device_num = 0;
	return No_exp;
}

static conf_object_t* atm_link_new(char* obj_name)
{
	atm_link_device_t* dev = skyeye_mm_zero(sizeof(atm_link_device_t));
	dev->obj = new_conf_object(obj_name, dev);
	init_atm_device_list(dev);

	int id;
	//create_thread_scheduler((unsigned int)1000, Periodic_sched, callback, dev->obj, &id);
	return dev->obj;
}

static exception_t atm_link_free(conf_object_t* obj)
{
	skyeye_free(obj->obj);
	return No_exp;
}

static exception_t atm_link_reset(conf_object_t* obj)
{
	return No_exp;
}

static exception_t atm_link_set_attr(conf_object_t* obj,
		const char* attr_name, attr_value_t* value)
{
	return No_exp;
}

static attr_value_t* atm_link_get_attr(const char* attr_name,
		conf_object_t* obj)
{
	return NULL;
}


int atm_linker_send_msg(conf_object_t *atm_linker, conf_object_t *atm_device, atm_msg_t *msg){
	atm_link_device_t *atm_link = atm_linker->obj;
	atm_device_list_t *list =  &(atm_link->device_list);
	conf_object_t *device;
	
	//printf("in %s, 1\n", __FUNCTION__);
	int i;
	for(i = 0; i < list->device_num; i++){
		device = list->device[i].device_obj;
		if (device != atm_device){
			//printf("in %s, 2\n", __FUNCTION__);
			if (list->device[i].iface)
				(list->device[i].iface)->receive_msg(device, msg);
		}
	}
	
	return 0;
}

static exception_t atm_dev_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	atm_link_device_t *atm_link = obj->obj;
	atm_device_list_t *list =  &(atm_link->device_list);
	int i;
	for(i = 0; i < list->device_num; i++){
		if (list->device[i].device_obj == obj2){
			return Unknown_exp;
		}
	}
	list->device[list->device_num].device_obj = obj2;
	list->device[list->device_num].iface =  (atm_dev_intf *)SKY_get_iface(obj2, ATM_DEV_INTF);
	list->device_num++;
	return No_exp;
}
 
static exception_t atm_dev_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	atm_link_device_t *atm_link = obj->obj;
	atm_device_list_t *list =  &(atm_link->device_list);
	list->device_num --;
	*obj2 = list->device[list->device_num].device_obj;
	*port = NULL;
	return No_exp;
}

void init_atm_link(){
	static skyeye_class_t class_data = {
		.class_name = "atm_linker",
		.class_desc = "atm_linker",
		.new_instance = atm_link_new,
		.free_instance = atm_link_free,
		.reset_instance = atm_link_reset,
		.get_attr = atm_link_get_attr,
		.set_attr = atm_link_set_attr
	};
	conf_class_t *clss = SKY_register_linker_class(class_data.class_name, &class_data);

	static const atm_linker_intf linker_iface ={
		.send_msg = atm_linker_send_msg,
		.send_data = NULL,
	};
	SKY_register_iface(clss, ATM_LINKER_INTF, &linker_iface);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = ATM_LINKER_INTF,
			.iface = &linker_iface,
		},
 	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription ){
			.name = ATM_DEV_INTF,
			.set = atm_dev_set,
			.get = atm_dev_get,
		},
	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}
