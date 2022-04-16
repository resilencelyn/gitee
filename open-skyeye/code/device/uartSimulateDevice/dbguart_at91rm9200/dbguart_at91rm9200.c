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
 * @file dbguart_at91rm9200.c
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

#include "dbguart_at91rm9200.h"
#include "at91rm9200_dbgfifo.h"

#define ARCH_ID_AT91RM9200  0x09290780

static char* at91rm9200_dbguart_attr[] = {"term", "signal"};

static void dbguart_do_cycle(conf_object_t* opaque)
{
	struct dbguart_at91rm9200_device *dev = opaque->obj;
	exception_t ret = 0;
	unsigned char buf;

	if (dev->term_iface && dev->dev_enable)
	{
		if ((ret = (dev->term_iface->read(dev->term, &buf, 1)) != 0)) {
			if (((dev->regs.dbg_cr & 0x30) >> 4) != 0x1) {
				if (dev->regs.dbg_imr & DBG_INT_RXRDY) {
					if (dev->read_fifo)
						WriteFIFO(dev->read_fifo, &buf, 1);
					else
						dev->regs.dbg_rhr = (int)buf;
					dev->regs.dbg_csr |= DBG_INT_RXRDY;
					if(dev->signal_iface != NULL)
						dev->signal_iface->raise_signal(dev->signal, dev->sig_no);
				}
			}
		}
	}
}

static exception_t at91rm9200_dbguart_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	struct dbguart_at91rm9200_device *dev = opaque->obj;
	int index;
	for(index = 0; index < sizeof(at91rm9200_dbguart_attr) / sizeof(at91rm9200_dbguart_attr[0]); index++) {
		if(!strncmp(attr_name, at91rm9200_dbguart_attr[index], strlen(at91rm9200_dbguart_attr[index])))
			break;
	}

	switch(index) {
		case 0:
			dev->term = (skyeye_uart_intf*)SKY_get_interface(value->u.object, SKYEYE_UART_INTF);
			break;
		case 1:
			dev->signal = (general_signal_intf*)SKY_get_interface(value->u.object, GENERAL_SIGNAL_INTF_NAME);
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "parameter error\n");
			return Invarg_exp;
	}

	return No_exp;
}

static exception_t dbguart_at91rm9200_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct dbguart_at91rm9200_device *dev = opaque->obj;
	uint32_t *data = (uint32_t *)buf;
	char ch;

	switch (offset) {
		case DBG_MR:
			*data = dev->regs.dbg_mr;
			break;
		case DBG_IMR:
			*data = dev->regs.dbg_imr;
			break;
		case DBG_CSR:
			*data = dev->regs.dbg_csr;
			break;
		case DBG_RHR:
			if (dev->read_fifo) {
				if (CheckCanReadNum(dev->read_fifo) > 0) {
					ReadFIFO(dev->read_fifo, &ch, 1);
					*data = ch;
				}
			} else {
				*data = dev->regs.dbg_rhr;
			}
			dev->regs.dbg_csr &= ~DBG_INT_RXRDY;
			break;
		case DBG_BRGR:
			*data = dev->regs.dbg_brgr;
			break;
		case DBG_CIDR:
			*data = dev->regs.dbg_cidr;
			break;
		case DBG_EXID:
			*data = dev->regs.dbg_exid;
			break;
		case DBG_FNR:
			*data = dev->regs.dbg_fnr;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__,"Can not read the register at 0x%x in dbguart_at91rm9200\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

static exception_t dbguart_at91rm9200_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct dbguart_at91rm9200_device *dev = opaque->obj;
	uint32_t data = *buf;
	char c;

	switch (offset) {
		case DBG_CR:
			dev->regs.dbg_cr = data;
			break;
		case DBG_MR:
			dev->regs.dbg_mr = data;
			break;
		case DBG_IER:
			dev->regs.dbg_ier = data;
			dev->regs.dbg_imr |= data;
			break;
		case DBG_IDR:
			dev->regs.dbg_idr = data;
			dev->regs.dbg_imr &= (~data);
			break;
		case DBG_THR:
			c = data;
			if (dev->term_iface != NULL)
				dev->term_iface->write(dev->term, &c, 1);
			if (dev->uart_file != NULL)
				dev->uart_file_iface->write(dev->uart_file,&c, 1);
			dev->regs.dbg_csr |= DBG_INT_TXEMPTY;
			break;
		case DBG_BRGR:
			dev->regs.dbg_brgr = data;
			break;
		case DBG_FNR:
			dev->regs.dbg_fnr = data;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__,"Can not write the register at 0x%x in dbguart_at91rm9200\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}
const char *dbguart_at91rm9200_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	dbguart_at91rm9200_device *dev= conf_obj->obj;
	return dev->regs_name[id];
}

static uint32_t dbguart_at91rm9200_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id)
{
	dbguart_at91rm9200_device *dev = conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)&(dev->regs) + id;
	return *regs_value;
}

static exception_t dbguart_at91rm9200_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	dbguart_at91rm9200_device *dev = conf_obj->obj;
	dbguart_at91rm9200_reg_t *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;

	return No_exp;
}

static uint32_t dbguart_at91rm9200_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	dbguart_at91rm9200_device *dev = conf_obj->obj;
	return  sizeof(dev->regs) / 4;
}

static uint32_t dbguart_at91rm9200_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	dbguart_at91rm9200_device *dev = conf_obj->obj;
	uint32_t regnum = sizeof(dev->regs) / 4;
	int i;

	for(i = 0; i < regnum; i++) {
		if(strcmp(name, dev->regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_dbguart_at91rm9200(char* obj_name) {
	dbguart_at91rm9200_device* dev = skyeye_mm_zero(sizeof(dbguart_at91rm9200_device));
	dev->obj = new_conf_object(obj_name, dev);

	dev->regs.dbg_mr	= 0x000;
	dev->regs.dbg_imr	= 0x000;
	dev->regs.dbg_rhr 	= 0x000;
	dev->regs.dbg_brgr 	= 0x000;
	dev->regs.dbg_cidr	= ARCH_ID_AT91RM9200;
	dev->regs.dbg_exid	= 0x000;
	dev->regs.dbg_fnr	= 0x000;

	dev->regs.dbg_csr |= DBG_INT_TXRDY;

	dev->regs_name = &regs_name;
	dev->read_fifo = NULL;
	dev->sig_no = 1;
	dev->dev_enable = 0;

	return dev->obj;
}

static exception_t reset_dbguart_at91rm9200(conf_object_t* opaque, const char* parameters) {
	struct dbguart_at91rm9200_device *dev = opaque->obj;

	dev->regs.dbg_mr	= 0x000;
	dev->regs.dbg_imr	= 0x000;
	dev->regs.dbg_rhr 	= 0x000;
	dev->regs.dbg_brgr 	= 0x000;
	dev->regs.dbg_cidr	= ARCH_ID_AT91RM9200;
	dev->regs.dbg_exid	= 0x000;
	dev->regs.dbg_fnr	= 0x000;

	dev->sig_no = 1;
	return No_exp;
}

static exception_t free_dbguart_at91rm9200(conf_object_t* opaque) {
	struct dbguart_at91rm9200_device *dev = opaque->obj;
	if(dev->read_fifo)
		FreeFIFO(dev->read_fifo);

	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static attr_value_t get_attr_rfifo(void* arg, conf_object_t* conf_obj, attr_value_t* idx) {
	struct dbguart_at91rm9200_device *dev = conf_obj->obj;
	return SKY_make_attr_object(dev->signal);
}
static exception_t set_attr_rfifo(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx) {
	unsigned int length;
	struct dbguart_at91rm9200_device *dev = conf_obj->obj;
	length = SKY_attr_uinteger(*value);
	dev->read_fifo = CreateFIFO(length);
	if(dev->read_fifo == NULL)
		skyeye_log(Error_log, __FUNCTION__, "Create Read Fifo Fail.");
	return No_exp;
}

static exception_t term_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	struct dbguart_at91rm9200_device *dev = obj->obj;
	dev->term = obj2;
	dev->term_iface = (memory_space_intf*)SKY_get_iface(dev->term, SKYEYE_UART_INTF);
	if (dev->term_iface == NULL){
		if (dev->term == NULL)
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SKYEYE_UART_INTF);
		else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SKYEYE_UART_INTF, obj2->objname);
	}
	int  pthread_id;
	create_thread_scheduler(1000, Periodic_sched, dbguart_do_cycle, dev->obj, &pthread_id);

	if (dev->term)
		dev->dev_enable = 1;
	else
		dev->dev_enable = 0;
	return No_exp;
}

static exception_t term_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	struct dbguart_at91rm9200_device *dev = obj->obj;
	*obj2 = dev->term;
	*port = NULL;
	return No_exp;
}

static exception_t uart_file_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	struct dbguart_at91rm9200_device *dev = obj->obj;
	dev->uart_file = obj2;
	dev->uart_file_iface = (uart_file_intf*)SKY_get_iface(obj2, UART_FILE_INTF);
	if (dev->uart_file_iface == NULL){
		if (dev->uart_file == NULL)
			skyeye_log(Error_log, __FUNCTION__, "Get 'skyeye_uart_intf' interface from NULL object fail.");
		else
			skyeye_log(Error_log, __FUNCTION__, "Get 'skyeye_uart_intf' interface from %s fail.", obj2->objname);
		return Not_found_exp;
	}
	return No_exp;
}

static exception_t uart_file_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	struct dbguart_at91rm9200_device *dev = obj->obj;
	*obj2 = dev->uart_file;
	*port = NULL;
	return No_exp;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	struct dbguart_at91rm9200_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (general_signal_intf **)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);
	if (dev->signal_iface == NULL){
		if (dev->signal == NULL)
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
		else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);
	}
	return No_exp;
}

static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	struct dbguart_at91rm9200_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

void dbguart_at91rm9200_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "rfifo", get_attr_rfifo, NULL, set_attr_rfifo, NULL, SKY_Attr_Optional, "uinteger", "fifo length for at91rm9200 dbguart");
}

void init_dbguart_at91rm9200(){
	static skyeye_class_t class_data = {
		.class_name = "at91rm9200_dbguart",
		.class_desc = "at91rm9200_dbguart",
		.new_instance = new_dbguart_at91rm9200,
		.free_instance = free_dbguart_at91rm9200,
		.reset_instance = reset_dbguart_at91rm9200,
		.get_attr = NULL,
		.set_attr = at91rm9200_dbguart_set_attr,
		.module_type = SKYML_ONCHIP(SC_at91rm9200),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = dbguart_at91rm9200_read,
		.write = dbguart_at91rm9200_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = dbguart_at91rm9200_get_regvalue_by_id,
		.get_regname_by_id = dbguart_at91rm9200_get_regname_by_id,
		.set_regvalue_by_id = dbguart_at91rm9200_set_regvalue_by_id,
		.get_regnum = dbguart_at91rm9200_get_regnum,
		.get_regid_by_name = dbguart_at91rm9200_get_regid_by_name,
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

	dbguart_at91rm9200_register_attribute(clss);
}
