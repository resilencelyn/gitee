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
* @file uart_c5510.c
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

#include "uart_c5510.h"
#include "c5510_fifo.h"

static char* c5510_uart_attr[] = {"term", "signal"};

void uart_do_cycle(conf_object_t* opaque)
{
	uart_c5510_device *dev = opaque->obj;
	uart_c5510_reg_t* regs = dev->regs;
	exception_t ret = 0;
	struct timeval tv;
	unsigned char buf;

	if (dev->dev_enable == 0)
		return;
	if (ETBEI && THRE){
		regs->iir &= ~(0x1);
		regs->iir |= 0x1;

		tv.tv_sec = 0;
		tv.tv_usec = 0;

		if ((ret = (dev->term_iface->read(dev->term, &buf, 1)) != 0)) {
			if(dev->read_fifo && (regs->fcr & 0x1))
				WriteFIFO(dev->read_fifo, &buf, 1);
			else
				regs->rbr = buf;

			if(dev->signal_iface != NULL)
				dev->signal_iface->raise_signal(dev->signal, dev->int_number);
		}
	}
}

static exception_t uart_c5510_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	uart_c5510_device *dev = opaque->obj;
	uart_c5510_reg_t* regs = dev->regs;

	switch(offset) {
		case RBR:
			if(dev->read_fifo) {
				if(CheckCanReadNum(dev->read_fifo) > 0) {
					uint8_t ch;
					ReadFIFO(dev->read_fifo, &ch, 1);
					*(uint16_t*)buf = ch;
					if(!CheckCanReadNum(dev->read_fifo)) {
						regs->lsr |= (0x3 << 5);
					}
				} else {
					*(uint32_t*)buf = regs->rbr;
				}
			} 
			break;
		case IER:
			*(uint16_t*)buf = regs->ier;
			break;
		case IIR:
			*(uint16_t*)buf = regs->iir;
			if (FIFOEN == 3){
				regs->fcr |= 0x1;
			}
			break;
		case LCR:
			*(uint16_t*)buf = regs->lcr;
			break;
		case MCR:
			*(uint16_t*)buf = regs->mcr;
			break;
		case LSR:
			*(uint16_t*)buf = regs->lsr;
			break;
		case SCR:
			*(uint16_t*)buf = regs->scr;
			break;
		case DLL:
			*(uint16_t*)buf = regs->dll;
			break;
		case DLH:
			*(uint16_t*)buf = regs->dlh;
			break;
		case PWREMU_MGMT:
			*(uint16_t*)buf = regs->pwremu_mgmt;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__,"Can not read the register at 0x%x in uart_c5510\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

static exception_t uart_c5510_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	uart_c5510_device *dev = opaque->obj;
	uart_c5510_reg_t* regs = dev->regs;
	uint16_t data = *buf;
	char c;

	switch(offset) {
		case THR:
			regs->thr = data;
			regs->lsr &= ~(0x3 << 5);
			c = data;
			if(dev->term_iface != NULL){
				dev->term_iface->write(dev->term, &c, 1);
				regs->lsr |= (0x1 << 5);
			}
			if (dev->uart_file){
				dev->uart_file_iface->write(dev->uart_file, &c, 1);
				regs->lsr |= (0x1 << 5);
			}
			break;
		case IER:
			regs->ier = data;
			break;
		case FCR:
			regs->fcr |= 0x1;
			regs->fcr = data;
			break;
		case LCR:
			regs->lcr = data;
			break;
		case MCR:
			regs->mcr = data;
			break;
		case LSR:
			regs->lsr = data;
			break;
		case SCR:
			regs->scr = data;
			break;
		case DLL:
			regs->dll = data;
			break;
		case DLH:
			regs->dlh = data;
			break;
		case PWREMU_MGMT:
			regs->pwremu_mgmt = data;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__,"Can not write the register at 0x%x in uart_c5510\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}
static char * uart_c5510_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	uart_c5510_device *dev= conf_obj->obj;
	return dev->regs_name[id];
}

uint32_t uart_c5510_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id)
{
	uart_c5510_device *dev= conf_obj->obj;
	uint16_t* regs_value = (uint16_t*)(dev->regs) + id;
	return *regs_value;
}

static char* uart_c5510_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	uart_c5510_device *dev = conf_obj->obj;
	uart_c5510_reg_t *regs = (dev->regs);
	*((uint16_t*)regs + id) = value;

	return NULL;
}

static uint32_t uart_c5510_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	uart_c5510_device *dev = conf_obj->obj;
	uart_c5510_reg_t *regs = (dev->regs);
	return  sizeof(uart_c5510_reg_t) / 2;
}

static uint32_t uart_c5510_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	uart_c5510_device *dev = conf_obj->obj;
	uart_c5510_reg_t *regs = (dev->regs);
	uint32_t regnum;
	regnum = sizeof(uart_c5510_reg_t)/ 2;
	int i;
	for(i =0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t uart_c5510_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	uart_c5510_device* dev = (uart_c5510_device*)(conf_obj->obj);
	attr_value_t int_number = SKY_make_attr_uinteger(dev->int_number);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	uart_c5510_device* dev = (uart_c5510_device*)(conf_obj->obj);
	dev->int_number = SKY_attr_uinteger(*value);
	return No_exp;
}
static conf_object_t* new_uart_c5510(char* obj_name) {
	uart_c5510_device* dev = skyeye_mm_zero(sizeof(uart_c5510_device));
	uart_c5510_reg_t* regs =  skyeye_mm_zero(sizeof(uart_c5510_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init uart_c5510 regs */
	dev->regs = regs;
	dev->regs_name = &regs_name;
	dev->regs->iir = 1;	
	dev->read_fifo = NULL;

	int  pthread_id;
	create_thread_scheduler(1000, Periodic_sched, uart_do_cycle, dev->obj, &pthread_id);
	dev->dev_enable = 1;
	return dev->obj;
}

exception_t reset_uart_c5510(conf_object_t* opaque, const char* parameters) {
	uart_c5510_device *dev = opaque->obj;
	uart_c5510_reg_t* regs = dev->regs; 
	
	return No_exp;
}

exception_t free_uart_c5510(conf_object_t* opaque) {
	uart_c5510_device *dev = opaque->obj;
	if(dev->read_fifo)
		FreeFIFO(dev->read_fifo);

	skyeye_free(dev->obj);
	skyeye_free(dev);
	return No_exp;
}

static attr_value_t get_attr_rfifo(void* arg, conf_object_t* conf_obj, attr_value_t* idx) {
	uart_c5510_device *dev = conf_obj->obj;
	return SKY_make_attr_object(dev->signal);
}
static exception_t set_attr_rfifo(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx) {
	unsigned int length;
	uart_c5510_device *dev = conf_obj->obj;
	length = SKY_attr_uinteger(*value);
	dev->read_fifo = CreateFIFO(length);
	if(dev->read_fifo == NULL)
		skyeye_log(Error_log, __FUNCTION__, "Create Read Fifo Fail.");
	return No_exp;
}

static exception_t term_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	uart_c5510_device *dev = obj->obj;
	dev->term = obj2;
	dev->term_iface = (memory_space_intf*)SKY_get_iface(dev->term, SKYEYE_UART_INTF);

	if (dev->term_iface == NULL){
		if (dev->term == NULL)
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

static exception_t term_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	uart_c5510_device *dev = obj->obj;
	*obj2 = dev->term;
	*port = NULL;
	return No_exp;
}

static exception_t uart_file_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	uart_c5510_device *dev = obj->obj;
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
	uart_c5510_device *dev = obj->obj;
	*obj2 = dev->uart_file;
	*port = NULL;
	return No_exp;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	uart_c5510_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (general_signal_intf **)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);

if (dev->signal_iface == NULL){
		if (dev->signal == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
			dev->dev_enable = 0;
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	dev->dev_enable = 1;
	return No_exp;
}
 
static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	uart_c5510_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

void uart_c5510_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "rfifo", get_attr_rfifo, NULL, set_attr_rfifo, NULL, SKY_Attr_Optional, "uinteger", "fifo length for s3c2410x uart");
}

void uart_c5510_register_interface(conf_class_t* clss) {
	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = uart_c5510_get_regvalue_by_id,
		.get_regname_by_id = uart_c5510_get_regname_by_id,
		.set_regvalue_by_id = uart_c5510_set_regvalue_by_id,
		.get_regnum = uart_c5510_get_regnum,
		.get_regid_by_name = uart_c5510_get_regid_by_name,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);
}
void init_uart_c5510(){
	static skyeye_class_t class_data = {
		.class_name = "c5510_uart",
		.class_desc = "c5510_uart",
		.new_instance = new_uart_c5510,
		.free_instance = free_uart_c5510,
		.reset_instance = reset_uart_c5510,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_c5510),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = uart_c5510_read,
		.write = uart_c5510_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = uart_c5510_get_regvalue_by_id,
		.get_regname_by_id = uart_c5510_get_regname_by_id,
		.set_regvalue_by_id = uart_c5510_set_regvalue_by_id,
		.get_regnum = uart_c5510_get_regnum,
		.get_regid_by_name = uart_c5510_get_regid_by_name,
		.get_regoffset_by_id = uart_c5510_get_regoffset_by_id
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		},
		(struct InterfaceDescription) {
			.name = SKYEYE_REG_INTF, 
			.iface = &reg_intf,
		},
 	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription ){
			.name = SKYEYE_UART_INTF,
			.set = term_set,
			.get = term_get,
		},
		(struct ConnectDescription ){
			.name = UART_FILE_INTF,
			.set = uart_file_set,
			.get = uart_file_get,
		},
		(struct ConnectDescription) {
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = general_signal_set,
			.get = general_signal_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	uart_c5510_register_attribute(clss);
}
