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

#include "tsi578_link.h"

/* initlize device list */
static exception_t init_tsi578_device_list(tsi578_link_device_t *dev){
	dev->device_list.device_num = 0;
	return No_exp;
}

/* initlize tsi578 register */
static exception_t init_tsi578_regs(tsi578_link_device_t *tsi578_device){
	tsi578_device->rio_regs.rio_dev_id				= r_RIO_DEV_ID;
	tsi578_device->rio_regs.rio_dev_info			= r_RIO_DEV_INFO;
	tsi578_device->rio_regs.rio_asbly_id			= r_RIO_ASBLY_ID;
	tsi578_device->rio_regs.rio_asbly_info			= r_RIO_ASBLY_INFO;
	tsi578_device->rio_regs.rio_pe_feat				= r_RIO_PE_FEAT;
	tsi578_device->rio_regs.rio_sw_port				= r_RIO_SW_PORT;
	tsi578_device->rio_regs.rio_src_op				= r_RIO_SRC_OP;
	tsi578_device->rio_regs.rio_pe_mc_feat			= r_RIO_PE_MC_FEAT;
	tsi578_device->rio_regs.rio_lut_size			= r_RIO_LUT_SIZE;
	tsi578_device->rio_regs.rio_sw_mc_info			= r_RIO_SW_MC_INFO;
	tsi578_device->rio_regs.rio_host_base_id_lock	= r_RIO_HOST_BASE_ID_LOCK;
	tsi578_device->rio_regs.rio_comp_tag			= r_RIO_COMP_TAG;
	tsi578_device->rio_regs.rio_route_cfg_destid	= r_RIO_ROUTE_CFG_DESTID;
	tsi578_device->rio_regs.rio_route_cfg_port		= r_RIO_ROUTE_CFG_PORT;
	tsi578_device->rio_regs.rio_LUT_attr			= r_RIO_LUT_ATTR;
	tsi578_device->rio_regs.rio_mc_mask_cfg			= r_RIO_MC_MASK_CFG;
	tsi578_device->rio_regs.rio_mc_destid_cfg		= r_RIO_MC_DESTID_CFG;
	tsi578_device->rio_regs.rio_mc_destid_assoc		= r_RIO_MC_DESTID_ASSOC;

	return No_exp;
}

/* new tsi578 device */
static conf_object_t* tsi578_link_new(char* obj_name)
{
	tsi578_link_device_t *tsi578_device = skyeye_mm_zero(sizeof(tsi578_link_device_t));
	tsi578_device->obj = new_conf_object(obj_name, tsi578_device);
	init_tsi578_device_list(tsi578_device);
	init_tsi578_regs(tsi578_device);

	return tsi578_device->obj;
}

/* free tsi578 device */
static exception_t tsi578_link_free(conf_object_t* dev)
{
	return No_exp;
}

/* reset tsi578 device */
static exception_t tsi578_link_reset(conf_object_t* obj)
{
	return No_exp;
}

static exception_t tsi578_link_set_attr(conf_object_t* obj,
		const char* attr_name, attr_value_t* value)
{
	return No_exp;
}

static attr_value_t* tsi578_link_get_attr(const char* attr_name,
		conf_object_t* obj)
{
	return NULL;
}

/* tsi578 register write */
static exception_t tsi578_regs_write(conf_object_t *opaque, generic_address_t offset, uint32_t *buf, size_t count){
	return No_exp;
}

/* tsi578 register read */
static exception_t tsi578_regs_read(conf_object_t *opaque, generic_address_t offset, uint32_t *buf, size_t count){
	tsi578_link_device_t *dev = opaque->obj;
	switch(offset){
		case RIO_ROUTE_CFG_PORT:
			*(uint32_t *)buf = dev->rio_regs.rio_route_cfg_port;
			break;
		default:
			break;
	}

	return No_exp;
}

/* send message */
int tsi578_linker_send_msg(conf_object_t *tsi578_linker, conf_object_t *tsi578_device, tsi578_msg_t *msg){
	tsi578_link_device_t *tsi578_link = tsi578_linker->obj;
	rio_device_list_t *list = &(tsi578_link->device_list);
	conf_object_t *device;

	int i;
	for(i = 0; i < list->device_num; i++){
		device = list->device[i].device_obj;
		if(device != tsi578_device){
			(list->device[i].iface)->receive_msg(device, msg);
		}
	}

	return 0;
}

/* tsi578_dev_set */
static exception_t tsi578_dev_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	tsi578_link_device_t *tsi578_link = obj->obj;
	rio_device_list_t *list =  &(tsi578_link->device_list);
	int i;
	for(i = 0; i < list->device_num; i++){
		if (list->device[i].device_obj == obj2){
			return Unknown_exp;
		}
	}
	list->device[list->device_num].device_obj = obj2;
	list->device[list->device_num].iface = (tsi578_dev_intf *)SKY_get_iface(obj2, TSI578_DEV_INTF);

	if (list->device[list->device_num].iface == NULL){
		if (list->device[list->device_num].device_obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", TSI578_DEV_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", TSI578_DEV_INTF, obj2->objname);

		return Not_found_exp;
	}
	list->device_num++;
	return No_exp;
}

static exception_t tsi578_dev_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	tsi578_link_device_t *tsi578_link = obj->obj;
	rio_device_list_t *list =  &(tsi578_link->device_list);

	list->device_num --;
	*obj2 = list->device[list->device_num].device_obj;
	*port = NULL;
	return No_exp;
}

/* initlize tsi578 device */
void init_tsi578_link(){
	static skyeye_class_t class_data = {
		.class_name = "tsi578_linker",
		.class_desc = "tsi578_linker",
		.new_instance = tsi578_link_new,
		.free_instance = tsi578_link_free,
		.reset_instance = tsi578_link_reset,
		.get_attr = NULL,
		.set_attr = NULL
	};
	conf_class_t *clss = SKY_register_linker_class(class_data.class_name, &class_data);

//interface
	static const memory_space_intf io_memory = {
		.read = tsi578_regs_read,
		.write = tsi578_regs_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const tsi578_linker_intf linker_iface ={
		.send_msg = tsi578_linker_send_msg,
	};
	SKY_register_iface(clss, TSI578_LINKER_INTF, &linker_iface);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = TSI578_LINKER_INTF, 
			.iface = &linker_iface, 
		},
	};

//connect
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription ){
			.name = TSI578_DEV_INTF,
			.set = tsi578_dev_set,
			.get = tsi578_dev_get,
		},
	};


	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}
