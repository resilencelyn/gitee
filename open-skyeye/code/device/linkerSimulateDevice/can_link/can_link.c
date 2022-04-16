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
 * @file can_link.c
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

#include "can_link.h"

#define BUF(num)  ((unsigned long long)(buf[num]))  

int data_to_frame(char *buf, can_msg_t *msg)
{
	int i;
	if(((buf[1] >> 2) & 0x1) == 0){ //标准帧
	}else{ //扩展帧
		msg->id = ((BUF(0) & 0x7f) << 22) | (((BUF(1) & 0xf0) >> 4) << 18) | ((BUF(1) & 0x3) << 16) | ((BUF(2) & 0xff) << 8) | (BUF(3) & 0xff);
		msg->ide = (BUF(1) >> 2) & 0x1;
		msg->dlc = (BUF(4) >> 1) & 0xf;
		if(msg->dlc > 8)
			msg->dlc = 8;
		msg->data[0] = ((BUF(4) & 0x1) << 7) | ((BUF(5) & 0xfe) >> 1);
		msg->data[1] = ((BUF(5) & 0x1) << 7) | ((BUF(6) & 0xfe) >> 1);
		msg->data[2] = ((BUF(6) & 0x1) << 7) | ((BUF(7) & 0xfe) >> 1);
		msg->data[3] = ((BUF(7) & 0x1) << 7) | ((BUF(8) & 0xfe) >> 1);
		msg->data[4] = ((BUF(8) & 0x1) << 7) | ((BUF(9) & 0xfe) >> 1);
		msg->data[5] = ((BUF(9) & 0x1) << 7) | ((BUF(10) & 0xfe) >> 1);
		msg->data[6] = ((BUF(10) & 0x1) << 7) | ((BUF(11) & 0xfe) >> 1);
		msg->data[7] = ((BUF(11) & 0x1) << 7) | ((BUF(12) & 0xfe) >> 1);
	
	}
}


int frame_to_data(char *buf, can_msg_t *msg)
{
	if (msg->ide == 0){ //标准帧
	}else{//扩展帧
		buf[0] = (msg->id >> 22) & 0x7f;
		buf[1] = (((msg->id >> 18) & 0xf) << 4)|  ((msg->ide & 0x1) << 2) | (((msg->id >> 16) & 0x3));
		buf[2] = (msg->id >> 8) & 0xff;
		buf[3] = msg->id  & 0xff;
		buf[4] = ((msg->dlc & 0xf) << 1) | ((msg->data[0] >> 7) & 0x1);
		buf[5] = ((msg->data[0] & 0x7f) << 1)| ((msg->data[1] >> 7) & 0x1);
		buf[6] = ((msg->data[1] & 0x7f)<< 1) | ((msg->data[2] >> 7) & 0x1);
		buf[7] = ((msg->data[2] & 0x7f)<< 1)| ((msg->data[3] >> 7) & 0x1);
		buf[8] = ((msg->data[3] & 0x7f)<< 1) | ((msg->data[4] >> 7) & 0x1);
		buf[9] = ((msg->data[4] & 0x7f)<<1) | ((msg->data[5] >> 7) & 0x1);
		buf[10] =((msg->data[5] & 0x7f)<< 1) | ((msg->data[6] >> 7) & 0x1);
		buf[11] =((msg->data[6] & 0x7f)<< 1) | ((msg->data[7] >> 7) & 0x1);
		buf[12] =((msg->data[7] & 0x7f) << 1);
		buf[13] = 0;
		buf[14] = 0;
		buf[15] = 0;
	}
	return 0;
}

static exception_t init_can_device_list(can_link_device_t *dev){
	dev->device_list.device_num = 0;
	return No_exp;
}


void callback(conf_object_t *obj){
	can_link_device_t *can_link = obj->obj;
	can_msg_t mesg;
	conf_object_t *device;
	int i;
	static int id = 0;
	//just for test
#if 0
	mesg.ide = 0;
	mesg.id = id++;
	mesg.dlc = 8;
	for(i = 0; i < 8; i++) 
		(mesg.data)[i] = i;

	can_device_list_t *list =  &(can_link->device_list);
	for(i = 0; i < list->device_num; i++){
		device = list->device[i].device_obj;
		(list->device[i].iface)->receive_msg(device, &mesg);
	}
#endif
}


static conf_object_t* can_link_new(char* obj_name)
{
	can_link_device_t* dev = skyeye_mm_zero(sizeof(can_link_device_t));
	dev->obj = new_conf_object(obj_name, dev);
	init_can_device_list(dev);

	int id;
	//create_thread_scheduler((unsigned int)1000, Periodic_sched, callback, dev->obj, &id);
	return dev->obj;
}



static exception_t can_link_free(conf_object_t* dev)
{
	return No_exp;
}

static exception_t can_link_reset(conf_object_t* obj)
{
	return No_exp;
}

static exception_t can_link_set_attr(conf_object_t* obj,
		const char* attr_name, attr_value_t* value)
{
	return No_exp;
}

static attr_value_t* can_link_get_attr(const char* attr_name,
		conf_object_t* obj)
{
	return NULL;
}


int can_linker_send_msg(conf_object_t *can_linker, conf_object_t *can_device, can_msg_t *msg){
	can_link_device_t *can_link = can_linker->obj;
	can_device_list_t *list =  &(can_link->device_list);
	conf_object_t *device;
	
	int i;
	for(i = 0; i < list->device_num; i++){
		device = list->device[i].device_obj;
		if (device != can_device){
			if ((list->device[i].iface))
				(list->device[i].iface)->receive_msg(device, msg);
		}
	}
	if (can_link->dds_can_linker){	
		char buf[16];

		frame_to_data(buf, msg);	
		if (can_link->dds_can_linker_iface)
			can_link->dds_can_linker_iface->send_data(can_link->dds_can_linker, buf, 16);
	}
	return 0;
}

int can_linker_send_data(conf_object_t *can_linker, conf_object_t *can_device, void *buf, uint32_t len){
	can_link_device_t *can_link = can_linker->obj;
	can_msg_t msg;
	conf_object_t *device;
	static int j = 0;
	int i;
	can_device_list_t *list =  &(can_link->device_list);
	data_to_frame(buf, &msg);
	for(i = 0; i < list->device_num; i++){
		device = list->device[i].device_obj;
		if (device != can_device){
			if ((list->device[i].iface))
				(list->device[i].iface)->receive_msg(device, &msg);
		}
	}
	return 0;
}

static exception_t can_dev_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	can_link_device_t *can_link = obj->obj;
	can_device_list_t *list =  &(can_link->device_list);
	int i;
	for(i = 0; i < list->device_num; i++){
		if (list->device[i].device_obj == obj2){
			return Unknown_exp;
		}
	}
	list->device[list->device_num].device_obj = obj2;
	list->device[list->device_num].iface =  (can_dev_intf *)SKY_get_iface(obj2, CAN_DEV_INTF);
	list->device_num++;
	return No_exp;
}
 
static exception_t can_dev_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	can_link_device_t *can_link = obj->obj;
	can_device_list_t *list =  &(can_link->device_list);

	list->device_num --;
	*obj2 = list->device[list->device_num].device_obj;
	*port = NULL;
	return No_exp;
}

static exception_t dds_can_linker_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	can_link_device_t *can_link = obj->obj;
	can_link->dds_can_linker = obj2;
	can_link->dds_can_linker_iface = (dds_can_linker_intf *)SKY_get_iface(obj2, DDS_CAN_LINKER_INTF);
	return No_exp;
}
 
static exception_t dds_can_linker_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	can_link_device_t *can_link = obj->obj;
	*obj2 = can_link->dds_can_linker;
	*port = NULL;
	return No_exp;
}


void init_can_link(){
	static skyeye_class_t class_data = {
		.class_name = "can_linker",
		.class_desc = "can_linker",
		.new_instance = can_link_new,
		.free_instance = can_link_free,
		.reset_instance = can_link_reset,
		.get_attr = can_link_get_attr,
		.set_attr = can_link_set_attr
	};
	conf_class_t *clss = SKY_register_linker_class(class_data.class_name, &class_data);

	static const can_linker_intf linker_iface ={
		.send_msg = can_linker_send_msg,
		.send_data = can_linker_send_data,
	};
	SKY_register_iface(clss, CAN_LINKER_INTF, &linker_iface);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = CAN_LINKER_INTF,
			.iface = &linker_iface,
		},
 	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription ){
			.name = CAN_DEV_INTF,
			.set = can_dev_set,
			.get = can_dev_get,
		},
		(struct ConnectDescription ){
			.name = DDS_CAN_LINKER_INTF,
			.set = dds_can_linker_set,
			.get = dds_can_linker_get,
		},

	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}
