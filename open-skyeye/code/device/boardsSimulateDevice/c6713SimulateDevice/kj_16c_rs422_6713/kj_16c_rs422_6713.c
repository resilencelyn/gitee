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
 * @file rs422_6713.c
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
#include <skyeye_data_intf.h>
#define DEBUG
#include <skyeye_log.h>
#include <skyeye_uart.h>
#include <skyeye_emif.h>

#include "kj_16c_rs422_6713.h"

static int rs422_6713_set_state(conf_object_t* obj, char *buf, int Length)
{
	rs422_6713_device *dev = obj->obj;
	rs422_6713_reg_t* regs = dev->regs;

	memcpy(dev->fifo.recv_buf, buf, Length);
	regs->recv_status = 1; //接收完毕

	return OK;
}
static exception_t rs422_6713_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct rs422_6713_device *dev = opaque->obj;
	rs422_6713_reg_t* regs = dev->regs;
	switch(offset) {
		case BUFFER_CACHE:
			*(uint32_t *)buf = dev->fifo.recv_buf[dev->fifo.recv_cnt];
			dev->fifo.recv_cnt += 1;

			break;
		case CHECK:
			*(uint32_t *)buf = regs->check;
			break;
		case BRSR:
			*(uint32_t *)buf = regs->brsr;
			break;
		case RECV_FORMAT:
			*(uint32_t *)buf = regs->recv_format;
			break;
		case RECV_HEADER:
			*(uint32_t *)buf = dev->recv_header[dev->recv_head_cnt];
			dev->recv_head_cnt ++;
			if (dev->recv_head_cnt == 2)
				dev->recv_head_cnt = 0;
			break;
		case RECV_TAIL:
			*(uint32_t *)buf = dev->recv_tail[dev->recv_tail_cnt];
			dev->recv_tail_cnt ++;
			if (dev->recv_tail_cnt == 2)
				dev->recv_tail_cnt = 0;
			break;
		case SEND_FORMAT:
			*(uint32_t *)buf = regs->send_format;
			break;
		case SEND_HEADER:
			*(uint32_t *)buf = dev->send_header[dev->send_head_cnt];
			dev->send_head_cnt ++;
			if (dev->send_head_cnt == 2)
				dev->send_head_cnt = 0;
			break;
		case SEND_TAIL:
			*(uint32_t *)buf = dev->send_tail[dev->send_tail_cnt];
			dev->send_tail_cnt ++;
			if (dev->send_tail_cnt == 2)
				dev->send_tail_cnt = 0;
			break;
		case RECV_STATUS:
			*(uint32_t *)buf = regs->recv_status;
			break;
		case SEND_STATUS:
			*(uint32_t *)buf = regs->send_status;
			break;
		case MODE_CTRL:
			*(uint32_t *)buf = regs->mode_ctrl;
			break;
		case CMD:
			*(uint32_t *)buf = regs->cmd;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in rs422_6713\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

static exception_t rs422_6713_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	//	skyeye_log(Error_log, __FUNCTION__, "offset :%x, buf :%x\n", offset, *(uint32_t *)buf);
	struct rs422_6713_device *dev = opaque->obj;
	rs422_6713_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t*)buf;
	switch(offset) {
		case BUFFER_CACHE:
			regs->buffer_cache = val;
			dev->fifo.send_buf[dev->fifo.send_cnt] = val;
			dev->fifo.send_cnt += 1;

			break;
		case CHECK:
			regs->check = val;
			break;
		case BRSR:
			regs->brsr = val;
			break;
		case RECV_FORMAT:
			regs->recv_format = val;
			break;
		case RECV_HEADER:
			regs->recv_header = val;
			dev->recv_header[dev->recv_head_cnt] = val;
			dev->recv_head_cnt ++;
			if (dev->recv_head_cnt == 2)
				dev->recv_head_cnt = 0;
			break;
		case RECV_TAIL:
			regs->recv_tail = val;
			dev->recv_tail[dev->recv_tail_cnt] = val;
			dev->recv_tail_cnt ++;
			if (dev->recv_tail_cnt == 2)
				dev->recv_tail_cnt = 0;
			break;
		case SEND_FORMAT:
			regs->send_format = val;
			break;
		case SEND_HEADER:
			regs->send_header = val;
			dev->send_header[dev->send_head_cnt] = val;
			dev->send_head_cnt ++;
			if (dev->send_head_cnt == 2)
				dev->send_head_cnt = 0;
			break;
		case SEND_TAIL:
			regs->send_tail = val;
			dev->send_tail[dev->send_tail_cnt] = val;
			dev->send_tail_cnt ++;
			if (dev->send_tail_cnt == 2)
				dev->send_tail_cnt = 0;
			break;
		case RECV_STATUS:
			regs->recv_status = val;
			break;
		case SEND_STATUS:
			regs->send_status = val;
			break;
		case MODE_CTRL:
			regs->mode_ctrl = val;
			break;
		case CMD:
			regs->cmd = val;
			if (regs->cmd == 1){ //start send 
				if (dev->bus_iface != NULL){
					dev->bus_iface->write_data(dev->bus_obj, opaque, dev->fifo.send_buf, dev->fifo.send_cnt);
				}

				dev->fifo.send_cnt = 0;
			}else if (regs->cmd == 3){ //clear recv
				memset(dev->fifo.recv_buf, 0, dev->fifo.recv_cnt);
				dev->fifo.recv_cnt = 0;
				regs->recv_status = 0; //接收完毕
			}
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in rs422_6713\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

static char* rs422_6713_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t rs422_6713_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	rs422_6713_device *dev = conf_obj->obj;
	rs422_6713_reg_t* regs = dev->regs;
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static char* rs422_6713_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	rs422_6713_device *dev = conf_obj->obj;
	rs422_6713_reg_t* regs = dev->regs;
	*((uint32_t*)regs + id) = value;

	return NULL;
}

static uint32_t rs422_6713_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	rs422_6713_device *dev = conf_obj->obj;
	rs422_6713_reg_t* regs = dev->regs;
	return  sizeof(rs422_6713_reg_t) / 4;

}

static uint32_t rs422_6713_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	rs422_6713_device *dev = conf_obj->obj;
	rs422_6713_reg_t* regs = dev->regs;
	uint32_t regnum;
	regnum = sizeof(rs422_6713_reg_t)/ 4;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;

}

static conf_object_t* new_rs422_6713(char* obj_name){
	rs422_6713_device* dev = skyeye_mm_zero(sizeof(rs422_6713_device));
	rs422_6713_reg_t* regs =  skyeye_mm_zero(sizeof(rs422_6713_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init rs422_6713 regs */
	dev->regs = regs;
	dev->fifo.send_cnt = 0;
	dev->fifo.recv_cnt = 0;

	dev->recv_head_cnt = 0;
	dev->recv_tail_cnt = 0;
	dev->send_head_cnt = 0;
	dev->send_tail_cnt = 0;

	return dev->obj;
}
static void free_rs422_6713(conf_object_t* conf_obj){
	rs422_6713_device *dev = conf_obj->obj;
	skyeye_free(dev->regs);
	skyeye_free(dev->obj);
	skyeye_free(dev);
	return;	
}

static exception_t term_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	struct rs422_6713_device *dev =  obj->obj;
	dev->term.obj = obj2;
	dev->term.intf = (skyeye_uart_intf*)SKY_get_iface(dev->term.obj, SKYEYE_UART_INTF);

	if (dev->term.intf == NULL){
		if (dev->term.obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SKYEYE_UART_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SKYEYE_UART_INTF, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t term_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	struct rs422_6713_device *dev =  obj->obj;
	*obj2 = dev->term.obj;
	*port = NULL;
	return No_exp;
}

static exception_t file_data_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{

	struct rs422_6713_device *dev = obj->obj;
	dev->intf_obj = obj2;
	dev->uart_intf = (skyeye_uart_data_intf *)SKY_get_iface(dev->intf_obj, SKYEYE_FILE_DATA_INTF_NAME);

	if (dev->uart_intf == NULL){
		if (dev->intf_obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SKYEYE_FILE_DATA_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SKYEYE_FILE_DATA_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t file_data_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	struct rs422_6713_device *dev = obj->obj;
	*obj2 = dev->intf_obj;
	*port = NULL;
	return No_exp;
}

static exception_t rs422_6713_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	rs422_6713_device* dev = (rs422_6713_device*)(obj->obj);
	dev->bus_obj = obj2;
	dev->bus_iface = (Rs422_bus_intf*)SKY_get_iface(dev->bus_obj, RS422_BUS_INTF);

	if (dev->bus_iface == NULL){
		if (dev->bus_obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", RS422_BUS_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", RS422_BUS_INTF, obj2->objname);

		return Not_found_exp;
	}

	if (Error == dev->bus_iface->register_device(obj2, obj)){
		printf("In %s, Register rs422_6713 device Error.\n", __FUNCTION__);
	}
	return No_exp;

}

static exception_t rs422_6713_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	rs422_6713_device* dev = (rs422_6713_device*)(obj->obj);
	*obj2 = dev->bus_obj;
	*port = NULL;
	return No_exp;
}

static rs422_6713_register_attribute(conf_class_t* clss){
}

void init_rs422_6713(){
	static skyeye_class_t class_data = {
		.class_name = "rs422_6713",
		.class_desc = "rs422_6713",
		.new_instance = new_rs422_6713,
		.free_instance = free_rs422_6713,
		.get_attr = NULL,
		.set_attr = NULL 
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = rs422_6713_read,
		.write = rs422_6713_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const Rs422_emif_intf rs422_emif_interface = {
		.set_state = rs422_6713_set_state,
	};
	SKY_register_iface(clss, RS422_EMIF_INTF, &rs422_emif_interface);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = rs422_6713_get_regvalue_by_id,
		.get_regname_by_id = rs422_6713_get_regname_by_id,
		.set_regvalue_by_id = rs422_6713_set_regvalue_by_id,
		.get_regnum = rs422_6713_get_regnum,
		.get_regid_by_name = rs422_6713_get_regid_by_name,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf,
		},
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME, 
			.iface = &io_memory,
		},
		(struct InterfaceDescription) {
			.name = RS422_EMIF_INTF,
			.iface =  &rs422_emif_interface,
		},
	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription ){
			.name = SKYEYE_UART_INTF,
			.set = term_set,
			.get = term_get,
		},
		(struct ConnectDescription ){
			.name = RS422_BUS_INTF,
			.set = rs422_6713_set,
			.get = rs422_6713_get,
		},
		(struct ConnectDescription) {
			.name = SKYEYE_FILE_DATA_INTF_NAME,
			.set = file_data_set,
			.get = file_data_get,
		}

	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	rs422_6713_register_attribute(clss);
}
