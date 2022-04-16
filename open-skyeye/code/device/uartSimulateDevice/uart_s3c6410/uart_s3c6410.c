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
* @file uart_s3c6410.c
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

#include "uart_s3c6410.h"
#include "fifo.h"

static char* s3c6410_uart_attr[] = {"term", "signal"};

void uart_do_cycle(conf_object_t* opaque)
{
	struct uart_s3c6410_device *dev = opaque->obj;
	uart_s3c6410_reg_t* regs = dev->regs;
	exception_t ret = 0;

	if (dev->dev_enable == 0)
		return;
	if (((regs->utrstat & 0x1) == 0x0) && ((regs->ucon & 0x3) == 0x1)) {
		struct timeval tv;
		unsigned char buf;

		tv.tv_sec = 0;
		tv.tv_usec = 0;

		if ((ret = (dev->term_iface->read(dev->term, &buf, 1)) != 0)){
			/* convert ctrl+c to ctrl+a. */
			if(dev->read_fifo)
				WriteFIFO(dev->read_fifo, &buf, 1);
			else{
				regs->urxh = buf;
			}

			/* Receiver Ready
			 * */
			regs->ufstat |= (0x1); /* 2007-02-09 by Anthony Lee : for 1 bytes */

			/* pending usart0 interrupt
			 * */
			regs->uintp |= (0x1 & ~(regs->uintm));
			regs->uintsp |= 0x1;
			if (regs->uintp) {
				if(dev->signal_iface)
					dev->signal_iface->raise_signal(dev->signal, dev->sig_no);
			}
		}
	}
}

static exception_t s3c6410_uart_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	struct uart_s3c6410_device *dev = opaque->obj;
	uart_s3c6410_reg_t* regs = dev->regs;
        int index;
        //parse the parameter
        for(index = 0; index < sizeof(s3c6410_uart_attr)/sizeof(s3c6410_uart_attr[0]);
		index++){
                if(!strncmp(attr_name, s3c6410_uart_attr[index],
			 strlen(s3c6410_uart_attr[index])))
                        break;
        }
        switch(index){
                case 0:
			dev->term = (skyeye_uart_intf*)SKY_get_interface(value->u.object,
					SKYEYE_UART_INTF);
			                        break;
                case 1:
			dev->signal = (general_signal_intf*)SKY_get_interface(value->u.object,
				GENERAL_SIGNAL_INTF_NAME);
			break;
                default:
                        skyeye_log(Error_log, __FUNCTION__, "parameter error\n");
                        return Invarg_exp;
        }
        return No_exp;
}

static exception_t uart_s3c6410_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct uart_s3c6410_device *dev = opaque->obj;
	uart_s3c6410_reg_t* regs = dev->regs;
	switch(offset) {
		case ULCON:
			*(uint32_t*)buf = regs->ulcon;
			break;
		case UCON:
			*(uint32_t*)buf = regs->ucon;
			break;
		case UFCON:
			*(uint32_t*)buf = regs->ufcon;
			break;
		case UMCON:
			*(uint32_t*)buf = regs->umcon;
			break;
		case UTRSTAT:
			*(uint32_t*)buf = regs->utrstat;
			break;
		case UERSTAT:
			*(uint32_t*)buf = regs->uerstat;
			break;
		case UFSTAT:
			*(uint32_t*)buf = regs->ufstat;
			break;
		case UMSTAT:
			*(uint32_t*)buf = regs->umstat;
			break;
		case URXH:
			if(dev->read_fifo){
				if(CheckCanReadNum(dev->read_fifo) > 0){
					uint8_t ch;
					ReadFIFO(dev->read_fifo, &ch, 1);
					*(uint32_t*)buf = ch;
					if(!CheckCanReadNum(dev->read_fifo)){
						regs->utrstat &= (~0x1);	/* clear strstat register bit[0] */
						regs->ufstat &= ~(0x1); /* 2007-02-09 by Anthony Lee : for 0 bytes */
					}
				}else{
					*(uint32_t*)buf = regs->urxh;
				}
			}else{
					*(uint32_t*)buf = regs->urxh;
					regs->utrstat &= (~0x1);	/* clear strstat register bit[0] */
					regs->ufstat &= ~(0x1); /* 2007-02-09 by Anthony Lee : for 0 bytes */
			}
			break;
		case UBRDIV:
			*(uint32_t*)buf = regs->ubrdiv;
			break;
		case UDIVSLOT:
			*(uint32_t*)buf = regs->ubrdivslot;
			break;
		case UINTP:
			*(uint32_t*)buf = regs->uintp;
			break;
		case UINTSP:
			*(uint32_t*)buf = regs->uintsp;
			break;
		case UINTM:
			*(uint32_t*)buf = regs->uintm;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__,"Can not read the register at 0x%x in uart_s3c6410\n", offset);
			return Invarg_exp;
	}

	return No_exp;

}

static exception_t uart_s3c6410_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct uart_s3c6410_device *dev = opaque->obj;
	uart_s3c6410_reg_t* regs = dev->regs;
	uint32_t data = *buf;
	switch(offset) {
		case ULCON:
			regs->ulcon = data;
			break;
		case UCON:
			regs->ucon = data;
			break;
		case UFCON:
			regs->ufcon = data;
			break;
		case UMCON:
			regs->umcon = data;
			break;
		case UMSTAT:
			regs->umstat = data;
			break;
		case UTXH:
			{
				char c = data;
				/* 2007-01-18 modified by Anthony Lee : for new uart device frame */
				if(dev->term_iface != NULL)
					dev->term_iface->write(dev->term, &c, 1);
				if(dev->uart_file != NULL)
					dev->uart_file_iface->write(dev->uart_file, &c, 1);
				regs->utrstat |= 0x6;	/* set strstat register bit[0] */
				regs->ufstat &= 0xff;	/* set strstat register bit[0] */

				regs->uintp |= (0x4 & ~(regs->uintm));
				regs->uintsp |= 0x4;
				if(dev->signal_iface != NULL)
					dev->signal_iface->raise_signal(dev->signal, dev->sig_no);
			}
			break;
		case UBRDIV:
			regs->ubrdiv = data;
			break;
		case UDIVSLOT:
			regs->ubrdivslot = data;
			break;
		case UINTP:
			regs->uintp &= ~data;
			break;
		case UINTSP:
			regs->uintsp &= ~data;
			break;
		case UINTM:
			regs->uintm = data;
			{
				regs->uintp |= (0x4 & ~(regs->uintm));
				regs->uintsp |= 0x4;
				dev->signal_iface->raise_signal(dev->signal, dev->sig_no);
			}
			break;

		default:
			skyeye_log(Error_log, __FUNCTION__,"Can not write the register at 0x%x in uart_s3c6410\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}
static char * uart_s3c6410_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	uart_s3c6410_device *dev= conf_obj->obj;
	return dev->regs_name[id];
}


uint32_t uart_s3c6410_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id)
{
	uart_s3c6410_device *dev= conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)(dev->regs) + id;
	return *regs_value;
}

static char* uart_s3c6410_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	uart_s3c6410_device *dev = conf_obj->obj;
	uart_s3c6410_reg_t *regs = (dev->regs);
	*((uint32_t*)regs + id) = value;

	return NULL;
}

static uint32_t uart_s3c6410_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	uart_s3c6410_device *dev = conf_obj->obj;
	uart_s3c6410_reg_t *regs = (dev->regs);
	return  sizeof(uart_s3c6410_reg_t) / 4;


}

static uint32_t uart_s3c6410_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	uart_s3c6410_device *dev = conf_obj->obj;
	uart_s3c6410_reg_t *regs = (dev->regs);
	uint32_t regnum;
	regnum = sizeof(uart_s3c6410_reg_t)/ 4;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;


}



static conf_object_t* new_uart_s3c6410(char* obj_name){
	uart_s3c6410_device* dev = skyeye_mm_zero(sizeof(uart_s3c6410_device));
	uart_s3c6410_reg_t* regs =  skyeye_mm_zero(sizeof(uart_s3c6410_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init uart_s3c6410 regs */
	dev->regs = regs;
	dev->regs_name = &regs_name;
	dev->read_fifo = NULL;
	regs->utrstat = UART_UTRSTAT_INIT;
	dev->sig_no = 37;

	int  pthread_id;
	create_thread_scheduler(1000, Periodic_sched, uart_do_cycle, dev->obj, &pthread_id);
	dev->dev_enable = 1;
	return dev->obj;
}

exception_t reset_uart_s3c6410(conf_object_t* opaque, const char* parameters){
	struct uart_s3c6410_device *dev = opaque->obj;
	uart_s3c6410_reg_t* regs = dev->regs;

	regs->utrstat = UART_UTRSTAT_INIT;
	dev->sig_no = 37;
	return No_exp;
}

exception_t free_uart_s3c6410(conf_object_t* opaque){
	struct uart_s3c6410_device *dev = opaque->obj;
	if(dev->read_fifo)
		FreeFIFO(dev->read_fifo);
    skyeye_free(dev->regs);
    skyeye_free(dev);
	return No_exp;
}


static attr_value_t get_attr_signal(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	struct uart_s3c6410_device *dev = conf_obj->obj;
	return SKY_make_attr_object(dev->signal);
}
static exception_t set_attr_signal(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	struct uart_s3c6410_device *dev = conf_obj->obj;
	conf_object_t* obj = SKY_attr_object(*value);
	dev->signal_iface = (general_signal_intf **)SKY_get_iface(obj, GENERAL_SIGNAL_INTF_NAME);
	dev->signal = obj;
	return No_exp;
}

static attr_value_t get_attr_rfifo(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	struct uart_s3c6410_device *dev = conf_obj->obj;
	return SKY_make_attr_object(dev->signal);
} static exception_t set_attr_rfifo(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	unsigned int length;
	struct uart_s3c6410_device *dev = conf_obj->obj;
	length = SKY_attr_uinteger(*value);
	dev->read_fifo = CreateFIFO(length);
	if(dev->read_fifo == NULL)
		skyeye_log(Error_log, __FUNCTION__, "Create Read Fifo Fail.");
	return No_exp;
}

static exception_t term_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	struct uart_s3c6410_device *dev = obj->obj;
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
	struct uart_s3c6410_device *dev = obj->obj;
	*obj2 = dev->term;
	*port = NULL;
	return No_exp;
}

static exception_t uart_file_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	struct uart_s3c6410_device *dev = obj->obj;
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
	struct uart_s3c6410_device *dev = obj->obj;
	*obj2 = dev->uart_file;
	*port = NULL;
	return No_exp;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	struct uart_s3c6410_device *dev = obj->obj;
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
	struct uart_s3c6410_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

void uart_s3c6410_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "rfifo", get_attr_rfifo, NULL, set_attr_rfifo, NULL, SKY_Attr_Optional, "uinteger", "fifo length for s3c6410 uart");
}


void uart_s3c6410_register_interface(conf_class_t* clss) {
	static const memory_space_intf io_memory = {
		.read = uart_s3c6410_read,
		.write = uart_s3c6410_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = uart_s3c6410_get_regvalue_by_id,
		.get_regname_by_id = uart_s3c6410_get_regname_by_id,
		.set_regvalue_by_id = uart_s3c6410_set_regvalue_by_id,
		.get_regnum = uart_s3c6410_get_regnum,
		.get_regid_by_name = uart_s3c6410_get_regid_by_name,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);
}
void init_uart_s3c6410(){
	static skyeye_class_t class_data = {
		.class_name = "s3c6410_uart",
		.class_desc = "s3c6410_uart",
		.new_instance = new_uart_s3c6410,
		.free_instance = free_uart_s3c6410,
		.reset_instance = reset_uart_s3c6410,
		.get_attr = NULL,
		.set_attr = s3c6410_uart_set_attr,
		.module_type = SKYML_ONCHIP(SC_s3c6410),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = uart_s3c6410_read,
		.write = uart_s3c6410_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = uart_s3c6410_get_regvalue_by_id,
		.get_regname_by_id = uart_s3c6410_get_regname_by_id,
		.set_regvalue_by_id = uart_s3c6410_set_regvalue_by_id,
		.get_regnum = uart_s3c6410_get_regnum,
		.get_regid_by_name = uart_s3c6410_get_regid_by_name,
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


	uart_s3c6410_register_attribute(clss);
}
