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
* @file timer_am335x.c
* @brief The implementation of system controller
* @author Kewei Yu: keweihk@gmail.com
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
#include <skyeye_attribute.h>
#include <skyeye_iface.h>
#define DEBUG
#include <skyeye_log.h>

#include "timer_am335x.h"

void reset_timer_am335x(conf_object_t *opaque, const char* parameters);

static char* timer_attr[] = {"signal", "int_number"};
static exception_t timer_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	struct timer_am335x_device *dev = opaque->obj;
	int index;
	for(index = 0; index < (sizeof(timer_attr)/sizeof(timer_attr[0])); index++){
		if(!strncmp(attr_name, timer_attr[index], strlen(timer_attr[index])))
			break;
	}
	switch(index){
		case 0:
			dev->signal = (general_signal_intf*)SKY_get_interface(value->u.object, GENERAL_SIGNAL_INTF_NAME);
			break;
		case 1:
			dev->interrupt_number  = value->u.uinteger;
			break;
		default:
			printf("parameter error\n");
			return Invarg_exp;
	}
	return No_exp;
}

static exception_t timer_am335x_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct timer_am335x_device *dev = opaque->obj;
	timer_am335x_reg_t regs = dev->regs;
	switch(offset) {
		case DMTIMER_TIDR:
			*(uint32_t*)buf = regs.tidr;
			break;
		case DMTIMER_TIOCP_CFG:
			*(uint32_t*)buf = regs.tiocp_cfg;
			break;
		case DMTIMER_IRQSTATUS:
			*(uint32_t*)buf = regs.irqstatus;
			break;
		case DMTIMER_IRQENABLE_CLR:
			*(uint32_t*)buf = regs.irqenable_clr;
			break;
		case DMTIMER_TCLR:
			*(uint32_t*)buf = regs.tclr;
			break;
		case DMTIMER_TCRR:
			*(uint32_t*)buf = regs.tcrr;
			break;
		case DMTIMER_TLDR:
			*(uint32_t*)buf = regs.tldr;
			break;
		case DMTIMER_TTGR:
			*(uint32_t*)buf = regs.ttgr;
			break;
		default:
			printf("Can not read the register at 0x%x in timer_am335x\n", offset);
			return Invarg_exp;
	}
	return No_exp;

}

static exception_t timer_am335x_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct timer_am335x_device *dev = opaque->obj;
	//timer_am335x_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t*)buf;
	switch(offset) {
		case DMTIMER_TIDR:
			dev->regs.tidr = *buf;
			break;
		case DMTIMER_TIOCP_CFG:
			dev->regs.tiocp_cfg = *buf;
			if((*buf) & DMTIMER_TIOCP_CFG_SOFTRESET == DMTIMER_TIOCP_CFG_SOFTRESET_INITIATE){
				dev->regs.tiocp_cfg &= ~DMTIMER_TIOCP_CFG_SOFTRESET_ONGOING;
				/* initiate software reset */
				reset_timer_am335x(opaque, NULL);

				dev->regs.tiocp_cfg |=DMTIMER_TIOCP_CFG_SOFTRESET_DONE;
			}
			if(((*buf) & DMTIMER_TIOCP_CFG_EMUFREE) >> DMTIMER_TIOCP_CFG_EMUFREE_SHIFT
				       	== DMTIMER_TIOCP_CFG_EMUFREE_TIMER_FREE){
				/* The timer runs free, regardless of PINSUSPENDN value */ 
			}else{
				/*  The timer is frozen in emulation mode */
			}
			uint8_t idlemode = ((*buf) & DMTIMER_TIOCP_CFG_IDLEMODE)
			       	>> DMTIMER_TIOCP_CFG_IDLEMODE_SHIFT;
			if(idlemode == DMTIMER_TIOCP_CFG_IDLEMODE_NOIDLE){
				/* No-idle mode */
			}else if(idlemode == DMTIMER_TIOCP_CFG_IDLEMODE_SMART){
				/* Smart-idle mode */
			}else if(idlemode == DMTIMER_TIOCP_CFG_IDLEMODE_WAKEUP){
				/* Smart-idle wakeup-capable mode */ 
			}else{
				/* Force-idle mode */
			}
			break;
		case DMTIMER_TCLR:
			dev->regs.tclr = *buf;
			if(((*buf) & DMTIMER_TCLR_ST) >> DMTIMER_TCLR_ST_SHIFT == DMTIMER_TCLR_ST_START){
				/* Start timer */
			}else{
				/* Stop timeOnly the counter is frozen */
			}
			if(((*buf) & DMTIMER_TCLR_AR) >> DMTIMER_TCLR_AR_SHIFT == DMTIMER_TCLR_AR_AUTO){
				/* Auto-reload timer */
			}else{
				/* One shot timer */
			}
			if(((*buf) & DMTIMER_TCLR_CE) >> DMTIMER_TCLR_CE_SHIFT == DMTIMER_TCLR_CE_ENABLE){
				/* Compare mode is enabled */
			}else{
				/* Compare mode is disabled */
			}
			break;
		case DMTIMER_IRQSTATUS:
			if((*buf & DMTIMER_IRQSTATUS_MAT_IT_FLAG)
					>> DMTIMER_IRQSTATUS_MAT_IT_FLAG_SHIFT ==
					DMTIMER_IRQSTATUS_MAT_IT_FLAG_CLEAR){
				/* Clear pending event */
			}
			if((*buf & DMTIMER_IRQSTATUS_OVF_IT_FLAG)
					>> DMTIMER_IRQSTATUS_OVF_IT_FLAG_SHIFT ==
					DMTIMER_IRQSTATUS_OVF_IT_FLAG_CLEAR){
				/* Clear pending event */
			}
			if((*buf & DMTIMER_IRQSTATUS_TCAR_IT_FLAG)
					>> DMTIMER_IRQSTATUS_TCAR_IT_FLAG_SHIFT ==
					DMTIMER_IRQSTATUS_TCAR_IT_FLAG_CLEAR){
				/* Clear pending event */
			}
			break;
		case DMTIMER_IRQENABLE_CLR:
			/* writes 1 to clear irq enable */
			if((*buf & DMTIMER_IRQENABLE_CLR_MAT_EN_FLAG) 
					>> DMTIMER_IRQENABLE_CLR_MAT_EN_FLAG_SHIFT ==
				       	DMTIMER_IRQENABLE_CLR_MAT_EN_FLAG_DISABLE){
				/* Clear IRQ enable */ 
			}
			if((*buf & DMTIMER_IRQENABLE_CLR_OVF_EN_FLAG) 
					>> DMTIMER_IRQENABLE_CLR_OVF_EN_FLAG_SHIFT ==
					DMTIMER_IRQENABLE_CLR_OVF_EN_FLAG_ENABLED){
				/* Clear IRQ enable */ 
			}
			if((*buf & DMTIMER_IRQENABLE_CLR_TCAR_EN_FLAG)
					>> DMTIMER_IRQENABLE_CLR_TCAR_EN_FLAG_SHIFT ==
					DMTIMER_IRQENABLE_CLR_TCAR_EN_FLAG_DISABLE){
				/* Clear IRQ enable */ 
			}
			break;
		case DMTIMER_TCRR:
			dev->regs.tclr = *buf;
			break;
		case DMTIMER_TLDR:
			dev->regs.tldr = *buf;
			break;
		case DMTIMER_TTGR:
			dev->regs.tcrr = dev->regs.tldr;
			break;
		case DMTIMER_TMAR:
			dev->regs.tldr = *buf;
			break;
		default:
			printf("Can not write the register at 0x%x in timer_am335x\n", offset);
			return Invarg_exp;
	}
	return No_exp;

}
static conf_object_t* new_timer_am335x(char* obj_name){
	timer_am335x_device* dev = skyeye_mm_zero(sizeof(timer_am335x_device));
	//timer_am335x_reg_t* regs =  skyeye_mm_zero(sizeof(timer_am335x_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init timer_am335x regs */
	//dev->regs = regs;

	/* Register io function to the object */
	memory_space_intf* io_memory = skyeye_mm_zero(sizeof(memory_space_intf));
	io_memory->conf_obj = dev->obj;
	io_memory->read = timer_am335x_read;
	io_memory->write = timer_am335x_write;
	SKY_register_interface(io_memory, obj_name, MEMORY_SPACE_INTF_NAME);	
	return dev->obj;
}

void reset_timer_am335x(conf_object_t *opaque, const char* parameters)
{
	struct timer_am335x_device *dev = opaque->obj;
	//timer_am335x_reg_t* regs = dev->regs;
	dev->regs.tidr = 0x40000100;
}

void free_timer_am335x(conf_object_t* dev){
	
}

#define REGISTER_ATTR_GET_FUNC_DEF(name) \
	attr_value_t name##_get_attr(void* arg, conf_object_t* obj, attr_value_t* idx) \
	{\
		uint32_t data; \
		timer_am335x_device_t* dev = (timer_am335x_device_t*)(obj->obj); \
		data = dev->regs.name; \
		attr_value_t value = SKY_make_attr_integer(data); \
						\
		return value; \
	}

#define REGISTER_ATTR_SET_FUNC_DEF(name) \
	exception_t name##_set_attr(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx) { \
		timer_am335x_device_t* dev = (timer_am335x_device_t*)(obj->obj); \
		uint32_t data = SKY_attr_integer(*value);\
		dev->regs.name = data; \
								\
		return No_exp;	\
	}

#define COMMON_ATTR_GET_FUNC_DEF(name) \
	attr_value_t name##_get_attr(void* arg, conf_object_t* obj, attr_value_t* idx) \
	{\
		uint32_t data; \
		timer_am335x_device_t* dev = (timer_am335x_device_t*)(obj->obj); \
		data = dev->name; \
		attr_value_t value = SKY_make_attr_integer(data); \
						\
		return value; \
	}

#define COMMON_ATTR_SET_FUNC_DEF(name) \
	exception_t name##_set_attr(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx) { \
		timer_am335x_device_t* dev = (timer_am335x_device_t*)(obj->obj); \
		uint32_t data = SKY_attr_integer(*value);\
		dev->name = data; \
								\
		return No_exp;	\
	}

exception_t sig_set_attr(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx) {
	struct timer_am335x_device* dev = (struct timer_am335x_device*)(obj->obj);
	conf_object_t* connect_obj = NULL;
	exception_t status = No_exp;
	if (SKY_attr_is_object(*value)) {
		connect_obj = SKY_attr_object(*value);
	} else if (SKY_attr_is_list(*value)) {
		/* TODO: connect interface in port */
		printf("PAY attention: should implementation the content of port interface\n");
	} else {
		fprintf(stderr, "should connect object interface, but not object\n");
		return Not_found_exp;
	}

	if (dev->sig.obj == connect_obj) {
		return No_exp;
	}
	dev->sig.obj = connect_obj;
	interface_t* iface = SKY_get_iface(connect_obj, GENERAL_SIGNAL_INTF_NAME);
	if (iface != NULL) {
		dev->sig.general_signal_iface = iface;
		status = No_exp;
	} else {
		status = Not_found_exp;
	}

	return status;
}

attr_value_t sig_get_attr(void* arg, conf_object_t* obj, attr_value_t* idx) {
	struct timer_am335x_device* dev = (struct timer_am335x_device*)(obj->obj);
	attr_value_t value;
	memset(&value, 0, sizeof(value));
	if (dev->sig.port != NULL) {
		/* TODO: implementation the interface in port */
		printf("Pay attention: not implement the sig_get_attr of interface with port\n");
	} else {
		value = SKY_make_attr_object(dev->sig.obj);
	}
	return value;
}

REGISTER_ATTR_GET_FUNC_DEF(tidr)
REGISTER_ATTR_SET_FUNC_DEF(tidr)
REGISTER_ATTR_GET_FUNC_DEF(tiocp_cfg)
REGISTER_ATTR_SET_FUNC_DEF(tiocp_cfg)
REGISTER_ATTR_GET_FUNC_DEF(irqstatus_raw)
REGISTER_ATTR_SET_FUNC_DEF(irqstatus_raw)
REGISTER_ATTR_GET_FUNC_DEF(irqstatus)
REGISTER_ATTR_SET_FUNC_DEF(irqstatus)
REGISTER_ATTR_GET_FUNC_DEF(irqenable_set)
REGISTER_ATTR_SET_FUNC_DEF(irqenable_set)
REGISTER_ATTR_GET_FUNC_DEF(irqenable_clr)
REGISTER_ATTR_SET_FUNC_DEF(irqenable_clr)
REGISTER_ATTR_GET_FUNC_DEF(irqwakeen)
REGISTER_ATTR_SET_FUNC_DEF(irqwakeen)
REGISTER_ATTR_GET_FUNC_DEF(tclr)
REGISTER_ATTR_SET_FUNC_DEF(tclr)
REGISTER_ATTR_GET_FUNC_DEF(tcrr)
REGISTER_ATTR_SET_FUNC_DEF(tcrr)
REGISTER_ATTR_GET_FUNC_DEF(tldr)
REGISTER_ATTR_SET_FUNC_DEF(tldr)
REGISTER_ATTR_GET_FUNC_DEF(ttgr)
REGISTER_ATTR_SET_FUNC_DEF(ttgr)
REGISTER_ATTR_GET_FUNC_DEF(twps)
REGISTER_ATTR_SET_FUNC_DEF(twps)
REGISTER_ATTR_GET_FUNC_DEF(tmar)
REGISTER_ATTR_SET_FUNC_DEF(tmar)
REGISTER_ATTR_GET_FUNC_DEF(tcar1)
REGISTER_ATTR_SET_FUNC_DEF(tcar1)
REGISTER_ATTR_GET_FUNC_DEF(tsicr)
REGISTER_ATTR_SET_FUNC_DEF(tsicr)
REGISTER_ATTR_GET_FUNC_DEF(tcar2)
REGISTER_ATTR_SET_FUNC_DEF(tcar2)

COMMON_ATTR_GET_FUNC_DEF(interrupt_number)
COMMON_ATTR_SET_FUNC_DEF(interrupt_number)

#define REGISTER_ATTRIBUTE(name, type, desc) \
	SKY_register_attribute(clss, #name, name##_get_attr, NULL, name##_set_attr, NULL, SKY_Attr_Optional, #type, desc)

static void timer_am335x_register_attribute(conf_class_t* clss) {
	init_class_attribute_list(clss);
	REGISTER_ATTRIBUTE(tidr, i, NULL);
	REGISTER_ATTRIBUTE(tiocp_cfg, i, NULL);
	REGISTER_ATTRIBUTE(irqstatus_raw, i, NULL);
	REGISTER_ATTRIBUTE(irqstatus, i, NULL);
	REGISTER_ATTRIBUTE(irqenable_set, i, NULL);
	REGISTER_ATTRIBUTE(irqenable_clr, i, NULL);
	REGISTER_ATTRIBUTE(irqwakeen, i, NULL);
	REGISTER_ATTRIBUTE(tclr, i, NULL);
	REGISTER_ATTRIBUTE(tcrr, i, NULL);
	REGISTER_ATTRIBUTE(tldr, i, NULL);
	REGISTER_ATTRIBUTE(ttgr, i, NULL);
	REGISTER_ATTRIBUTE(twps, i, NULL);
	REGISTER_ATTRIBUTE(tmar, i, NULL);
	REGISTER_ATTRIBUTE(tcar1, i, NULL);
	REGISTER_ATTRIBUTE(tsicr, i, NULL);
	REGISTER_ATTRIBUTE(tcar2, i, NULL);
	REGISTER_ATTRIBUTE(sig, o, NULL);
	REGISTER_ATTRIBUTE(interrupt_number, i, NULL);

	return;
}

static void timer_am335x_register_interface(conf_class_t* clss) {
	init_class_interface_list(clss);
	static const memory_space_iface_t mem_iface = {
		.read = timer_am335x_read,
		.write = timer_am335x_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &mem_iface);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &mem_iface,
		}
 	};
	class_register_ifaces(clss, ifaces);

	return;
}

void init_timer_am335x(){
	static skyeye_class_t class_data = {
		.class_name = "am335x_timer",
		.class_desc = "am335x_timer",
		.new_instance = new_timer_am335x,
		.free_instance = free_timer_am335x,
		.get_attr = NULL,
		.set_attr = timer_set_attr,
		.module_type = SKYML_ONCHIP(SC_am335x SC_am3359),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	timer_am335x_register_attribute(clss);
	timer_am335x_register_interface(clss);
	//SKY_print_attribute(clss);
	//SKY_print_class_iface(clss);
}
