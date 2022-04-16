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
* @file timer_1ms_am335x.c
* @brief The implementation of system controller
* @author zyuming: zyumingfit@gmail.com
* @version 78.77
*/

#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h> 
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_log.h>
#include <skyeye_attribute.h>
#include "skyeye_thread.h"
#include "timer_1ms_am335x.h"
#define DEBUG

void reset_timer_1ms_am335x(conf_object_t *opaque, const char* parameters);
#if 0
static char* timer_attr[] = {"signal", "int_number"};
static exception_t timer_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	struct timer_1ms_am335x_device *dev = opaque->obj;
	int index;
	for(index = 0; index < (sizeof(timer_attr)/sizeof(timer_attr[0])); index++){
		if(!strncmp(attr_name, timer_attr[index], strlen(timer_attr[index])))
			break;
	}
	switch(index){
		case 0:
			dev->signal = (general_signal_intf*)SKY_get_iface(value->u.object, GENERAL_SIGNAL_INTF_NAME);
			break;
		case 1:
			dev->interrupt_number = value->u.uinteger;
			break;
		default:
			printf("parameter error\n");
			return Invarg_exp;
	}
	return No_exp;
}
#endif
static void *time_delay(void *uart_dev)
{
	static int i = 0xff;
	timer_1ms_am335x_device* dev  = uart_dev;
	i--;
	if(i == 0 && dev){
		i = 0xff;
		if(((dev->regs->tier) & 0x07) == 0x2){
			dev->regs->tisr |= 0x2;
			dev->signal->raise_signal(dev->signal_obj, dev->interrupt_number);/*触发接收中断*/
		}
	}

	return NULL;
}
static exception_t timer_1ms_am335x_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct timer_1ms_am335x_device *dev = opaque->obj;
	timer_1ms_am335x_reg_t* regs = dev->regs;
	switch(offset) {
		case DMTIMER_1MS_TIDR:
			*(uint32_t*)buf = regs->tidr;
			break;
		case DMTIMER_1MS_TIOCP_CFG:
			*(uint32_t*)buf = regs->tidr;
			*(uint32_t*)buf = regs->tiocp_cfg;
			break;
		case DMTIMER_1MS_TISTAT:
			*(uint32_t*)buf = regs->tistat;
			break;
		case DMTIMER_1MS_TISR:
			*(uint32_t*)buf = regs->tisr;
			break;
		case DMTIMER_1MS_TIER:
			*(uint32_t*)buf = regs->tier;
			break;
		case DMTIMER_1MS_TWER:
			*(uint32_t*)buf = regs->twer;
			break;
		case DMTIMER_1MS_TCLR:
			*(uint32_t*)buf = regs->tclr;
			break;
		case DMTIMER_1MS_TCRR:
			*(uint32_t*)buf = regs->tcrr;
			break;
		case DMTIMER_1MS_TLDR:
			*(uint32_t*)buf = regs->tldr;
			break;
		case DMTIMER_1MS_TTGR:
			*(uint32_t*)buf = regs->ttgr;
			break;
		case DMTIMER_1MS_TWPS:
			*(uint32_t*)buf = regs->twps;
			break;
		case DMTIMER_1MS_TMAR:
			*(uint32_t*)buf = regs->tmar;
			break;
		case DMTIMER_1MS_TCAR1:
			*(uint32_t*)buf = regs->tcar1;
			break;
		case DMTIMER_1MS_TSICR:
			*(uint32_t*)buf = regs->tsicr;
			break;
		case DMTIMER_1MS_TCAR2:
			*(uint32_t*)buf = regs->tcar2;
			break;
		case DMTIMER_1MS_TPIR:
			*(uint32_t*)buf = regs->tpir;
			break;
		case DMTIMER_1MS_TNIR:
			*(uint32_t*)buf = regs->tnir;
			break;
		case DMTIMER_1MS_TCVR:
			*(uint32_t*)buf = regs->tcvr;
			break;
		case DMTIMER_1MS_TOCR:
			*(uint32_t*)buf = regs->tocr;
			break;
		case DMTIMER_1MS_TOWR:
			*(uint32_t*)buf = regs->towr;
			break;
		default:
			return Invarg_exp;
	}
	return No_exp;

}

static exception_t timer_1ms_am335x_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct timer_1ms_am335x_device *dev = opaque->obj;
	timer_1ms_am335x_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t*)buf;
	switch(offset) {
		case DMTIMER_1MS_TIOCP_CFG:
			regs->tiocp_cfg = *(uint32_t*)buf;
			break;
		case DMTIMER_1MS_TISR:
			regs->tisr &= ~(*(uint32_t*)buf);
			break;
		case DMTIMER_1MS_TIER:
			regs->tier = *(uint32_t*)buf;
			break;
		case DMTIMER_1MS_TWER:
			regs->twer = *(uint32_t*)buf;
			break;
		case DMTIMER_1MS_TCLR:
			regs->tclr = *(uint32_t*)buf;
			break;
		case DMTIMER_1MS_TCRR:
			regs->tcrr = *(uint32_t*)buf;
			break;
		case DMTIMER_1MS_TLDR:
			regs->tldr = *(uint32_t*)buf;
			break;
		case DMTIMER_1MS_TTGR:
			regs->ttgr = *(uint32_t*)buf;
			break;
		case DMTIMER_1MS_TMAR:
			regs->tmar = *(uint32_t*)buf;
			break;
		case DMTIMER_1MS_TSICR:
			regs->tsicr = *(uint32_t*)buf;
			break;
		case DMTIMER_1MS_TCAR2:
			regs->tcar2 = *(uint32_t*)buf;
			break;
		case DMTIMER_1MS_TPIR:
			regs->tpir = *(uint32_t*)buf;
			break;
		case DMTIMER_1MS_TNIR:
			regs->tnir = *(uint32_t*)buf;
			break;
		case DMTIMER_1MS_TCVR:
			regs->tcvr = *(uint32_t*)buf;
			break;
		case DMTIMER_1MS_TOCR:
			regs->tocr = *(uint32_t*)buf;
			break;
		case DMTIMER_1MS_TOWR:
			regs->towr = *(uint32_t*)buf;
			break;

		default:
			printf("Can not write the register at 0x%x in timer_1ms_am335x\n", offset);
			return Invarg_exp;
	}
	return No_exp;

}

static conf_object_t* new_timer_1ms_am335x(char* obj_name){
	timer_1ms_am335x_device* dev = skyeye_mm_zero(sizeof(timer_1ms_am335x_device));
	timer_1ms_am335x_reg_t* regs =  skyeye_mm_zero(sizeof(timer_1ms_am335x_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init timer_1ms_am335x regs */
	dev->regs = regs;
#if 0
	/* Register io function to the object */
	memory_space_intf* io_memory = skyeye_mm_zero(sizeof(memory_space_intf));
	io_memory->conf_obj = dev->obj;
	io_memory->read = timer_1ms_am335x_read;
	io_memory->write = timer_1ms_am335x_write;
	SKY_register_interface(io_memory, obj_name, MEMORY_SPACE_INTF_NAME);	
#endif
	int  pthread_id;
	create_thread_scheduler(1, Periodic_sched, time_delay, dev, &pthread_id);

	return dev->obj;
}

void reset_timer_1ms_am335x(conf_object_t *opaque, const char* parameters)
{
	struct timer_1ms_am335x_device *dev = opaque->obj;
	timer_1ms_am335x_reg_t* regs = dev->regs;
}

void free_timer_1ms_am335x(conf_object_t* dev){
	
}
static void timer_1ms_am335x_register_interface(conf_class_t* clss){
	/* Register io function to the object */
	static const memory_space_intf io_memory = {
		.read = timer_1ms_am335x_read,
		.write = timer_1ms_am335x_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		}
 	};
	class_register_ifaces(clss, ifaces);
	return;
}

static attr_value_t get_attr_signal(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	timer_1ms_am335x_device *dev = conf_obj->obj;
	attr_value_t signal_attr = SKY_make_attr_object(dev->signal_obj);
	return signal_attr;
}
static exception_t set_attr_signal(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	timer_1ms_am335x_device *dev = conf_obj->obj;
	dev->signal_obj = SKY_attr_object(*value);
	dev->signal = (general_signal_intf *)SKY_get_iface(dev->signal_obj, GENERAL_SIGNAL_INTF_NAME);
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	timer_1ms_am335x_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	timer_1ms_am335x_device *dev = conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static void timer_1ms_am335x_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "signal", get_attr_signal, NULL, set_attr_signal, NULL, SKY_Attr_Optional, "object", " interrupt controler  connected");
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of timer");
	return;
}

void init_timer_1ms_am335x(){
	static skyeye_class_t class_data = {
		.class_name = "am335x_timer_1ms",
		.class_desc = "am335x_timer_1ms",
		.new_instance = new_timer_1ms_am335x,
		.free_instance = free_timer_1ms_am335x,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_am335x SC_am3359),
	};

	conf_class_t* class = SKY_register_device_class(class_data.class_name, &class_data);
	timer_1ms_am335x_register_interface(class);
	timer_1ms_am335x_register_attribute(class);
}
