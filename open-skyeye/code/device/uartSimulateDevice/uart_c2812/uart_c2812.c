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
* @file uart_c2812.c
* @brief The implementation of system controller
* @author Kewei Yu keweihk@gmail.com
* @version 78.77
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
#include <skyeye_uart_ops.h>
#include <skyeye_iface.h>
#define DEBUG
#include <skyeye_log.h>

#include "uart_c2812.h"

static char* c2812_uart_attr[] = {"term"};

static exception_t c2812_uart_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	uart_c2812_device *dev = opaque->obj;
        int index;
        //parse the parameter
        for(index = 0; index < sizeof(c2812_uart_attr)/sizeof(c2812_uart_attr[0]);
		index++){
                if(!strncmp(attr_name, c2812_uart_attr[index],
			 strlen(c2812_uart_attr[index])))
                        break;
        }
        switch(index){
                case 0:
			dev->term = (skyeye_uart_intf*)SKY_get_interface(value->u.object,
					SKYEYE_UART_INTF);
			                        break;
                default:
                        skyeye_log(Error_log, __FUNCTION__, "parameter error\n");
                        return Invarg_exp;
        }
        return No_exp;
}

static exception_t uart_c2812_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct uart_c2812_device *dev = opaque->obj;
	switch(offset)
	{
		case UART_DATA_REGISTER:
			if(dev->regs.control.flag.receiver_enable == 1){
				*(uint32*)buf = dev->fifo.buf[dev->fifo.start++];
			}
			if(dev->fifo.start >= dev->fifo.end){
				dev->regs.status.flag.data_ready = 0;
				dev->fifo.start = 0;
				dev->fifo.end = 0;
			}
			*(uint32*)buf = dev->regs.data;
			break;
		case UART_STATUS_REGISTER:
			*(uint32*)buf =dev->regs.status.value;
			break;
		case UART_CONTROL_REGISTER:
			*(uint32*)buf =dev->regs.control.value;
			break;
		case UART_SCALER_REGISTER:
			*(uint32*)buf =dev->regs.scaler;
			break;
		default:
			break;
	}

	return No_exp;
}

static exception_t uart_c2812_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct uart_c2812_device *dev = opaque->obj;
	switch(offset)
	{
		case UART_DATA_REGISTER:
			dev->regs.data = *buf;
            if (dev->term_dev.obj) {
                dev->term_dev.term->write(dev->term_dev.obj, buf, 1);
            }
	    if (dev->uart_file)
		dev->uart_file_iface->write(dev->uart_file, buf, 1);
			break;
		case UART_STATUS_REGISTER:
			break;
		case UART_CONTROL_REGISTER:
			dev->regs.control.value = *buf;
			break;
		case UART_SCALER_REGISTER:
			dev->regs.scaler = *buf;
			break;
		default:
			printf("write %s error offset %d : 0x%x\n",
					dev->obj->objname, offset, *buf);
			break;
	}
	return No_exp;
}
static char * uart_c2812_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	uart_c2812_device *dev= conf_obj->obj;
	if (id < sizeof(regs_name) / 4 - 1){
		return dev->regs_name[id];
	}
}


uint32_t uart_c2812_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id)
{
	uart_c2812_device *dev= conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)&(dev->regs) + id;
	return *regs_value;
}

static char* uart_c2812_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	uart_c2812_device *dev = conf_obj->obj;
	struct registers* regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;

	return NULL;
}

static uint32_t uart_c2812_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	uart_c2812_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	return  sizeof(struct registers) / 4;


}

static uint32_t uart_c2812_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	uart_c2812_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t regnum;
	regnum = sizeof(struct registers)/ 4;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;


}



static conf_object_t* new_uart_c2812(char* obj_name){
	uart_c2812_device* dev = skyeye_mm_zero(sizeof(uart_c2812_device));
	dev->regs_name = &regs_name;
	dev->obj = new_conf_object(obj_name, dev);
	dev->fifo.start = 0;
	dev->fifo.end = 0;
	/* init uart_c2812 regs */
	//struct registers* regs =  skyeye_mm_zero(sizeof(struct registers));

	return dev->obj;
}

exception_t reset_uart_c2812(conf_object_t* opaque, const char* parameters){
	struct uart_c2812_device *dev = opaque->obj;
	memset(&(dev->regs), 0, sizeof(dev->regs));
	
	return No_exp;
}

exception_t free_uart_c2812(conf_object_t* opaque){
	struct uart_c2812_device *dev = opaque->obj;
	//skyeye_free(dev->regs);
	skyeye_free(dev->obj);
	skyeye_free(dev);
}


void uart_c2812_register_attribute(conf_class_t* clss){
	return;
}

static exception_t uart_term_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	struct uart_c2812_device *dev = obj->obj;
	dev->term_dev.obj = obj2;
	dev->term_dev.term = (skyeye_uart_intf*)SKY_get_iface(dev->term_dev.obj, SKYEYE_UART_INTF);

	if (dev->term_dev.term == NULL){
		if (dev->term_dev.obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SKYEYE_UART_INTF);
			dev->dev_enable = 0;
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SKYEYE_UART_INTF, obj2->objname);

		return Not_found_exp;
	}

	dev->dev_enable = 1;
	return No_exp;
}

static exception_t uart_term_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	struct uart_c2812_device *dev = obj->obj;
	*obj2 = dev->term_dev.obj;
	*port = NULL;
	return No_exp;
}

static exception_t uart_file_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	struct uart_c2812_device *dev = obj->obj;
	dev->uart_file = obj2;
	dev->uart_file_iface = (uart_file_intf*)SKY_get_iface(obj2, UART_FILE_INTF);
	if (dev->uart_file_iface == NULL){
		if (dev->uart_file == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", UART_FILE_INTF);
			dev->dev_enable = 0;
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", UART_FILE_INTF, obj2->objname);

		return Not_found_exp;
	}
	dev->dev_enable = 1;
	return No_exp;
}

static exception_t uart_file_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	struct uart_c2812_device *dev = obj->obj;
	*obj2 = dev->uart_file;
	*port = NULL;
	return No_exp;
}

void init_uart_c2812(){
	static skyeye_class_t class_data = {
		.class_name = "c2812_uart",
		.class_desc = "c2812_uart",
		.new_instance = new_uart_c2812,
		.free_instance = free_uart_c2812,
		.reset_instance = reset_uart_c2812,
		.get_attr = NULL,
		.set_attr = c2812_uart_set_attr,
		.module_type = SKYML_ONCHIP(SC_c2812),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = uart_c2812_read,
		.write = uart_c2812_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = uart_c2812_get_regvalue_by_id,
		.get_regname_by_id = uart_c2812_get_regname_by_id,
		.set_regvalue_by_id = uart_c2812_set_regvalue_by_id,
		.get_regnum = uart_c2812_get_regnum,
		.get_regid_by_name = uart_c2812_get_regid_by_name,
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
 	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription ){
			.name = SKYEYE_UART_INTF,
			.set = uart_term_set,
			.get = uart_term_get,
		},
		(struct ConnectDescription ){
			.name = UART_FILE_INTF,
			.set = uart_file_set,
			.get = uart_file_get,
		}
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	uart_c2812_register_attribute(clss);
}
