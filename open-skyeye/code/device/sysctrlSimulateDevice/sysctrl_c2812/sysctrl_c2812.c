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
* @file sysctrl_c2812.c
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
#define DEBUG
#include <skyeye_log.h>

#include "sysctrl_c2812.h"

#define WAKEINT 8

static void watchdog_callback(conf_object_t *opaque)
{
	sysctrl_c2812_device *dev = opaque->obj;
	sysctrl_c2812_reg_t* regs = dev->regs;
	
    	if (WDDIS == 0)
	    regs->WDCNTR ++;
	if (regs->WDCNTR == 512 && regs->SCSR.bit.WDENINT){
		if (dev->signal_iface)
			dev->signal_iface->raise_signal(dev->signal, WAKEINT);
	}
}

static exception_t sysctrl_c2812_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	sysctrl_c2812_device *dev = opaque->obj;
	sysctrl_c2812_reg_t* regs = dev->regs;
	switch(offset) {
		case HISPCP_REG:
			*(uint16_t *)buf = regs->HISPCP.all;
			break;
		case LOSPCP_REG:
			*(uint16_t *)buf = regs->LOSPCP.all;
			break;
		case PCLKCR_REG:
			*(uint16_t *)buf = regs->PCLKCR.all;
			break;
		case LPMCR0_REG:
			*(uint16_t *)buf = regs->LPMCR0.all;
			break;
		case LPMCR1_REG:
			*(uint16_t *)buf = regs->LPMCR1.all;
			break;
		case PLLCR_REG:
			*(uint16_t *)buf = regs->PLLCR.all;
			break;
		case SCSR_REG:
			*(uint16_t *)buf = regs->SCSR.all;
			break;
		case WDCNTR_REG:
			*(uint16_t *)buf = regs->WDCNTR;
			break;
		case WDKEY_REG:
			*(uint16_t *)buf = regs->WDKEY;
			break;
		case WDCR_REG:
			*(uint16_t *)buf = regs->WDCR;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, " Can not read  the register at 0x%x in sysctrl_c2812\n", offset);
			return Invarg_exp;
	}

	return No_exp;
}

static exception_t sysctrl_c2812_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	sysctrl_c2812_device *dev = opaque->obj;
	sysctrl_c2812_reg_t* regs = dev->regs;
	uint16_t val = *(uint16_t*)buf;

	switch(offset) {
		case HISPCP_REG:
			regs->HISPCP.all = val;
			break;
		case LOSPCP_REG:
			regs->LOSPCP.all = val;
			break;
		case PCLKCR_REG:
			regs->PCLKCR.all = val;
			break;
		case LPMCR0_REG:
			regs->LPMCR0.all = val;
			break;
		case LPMCR1_REG:
			regs->LPMCR1.all = val;
			break;
		case PLLCR_REG:
			regs->PLLCR.all = val;
			dev->sysclk = dev->oscclk * (regs->PLLCR.bit.DIV)/ 4;
			break;
		case SCSR_REG:
			regs->SCSR.all = val;
			break;
		case WDCNTR_REG:
			regs->WDCNTR = val;
			break;
		case WDKEY_REG:
			dev->old_key = regs->WDKEY;
			regs->WDKEY = val;
			if (dev->old_key == 0x55 && regs->WDKEY == 0xAA)
				regs->WDCNTR = 0;
			break;
		case WDCR_REG:
			regs->WDCR = val;
			switch(regs->WDCR & 0x7){
				case 0:
				case 1:
					dev->wd_scale = 1;
					break;
				case 2:
					dev->wd_scale = 2;
					break;
				case 3:
					dev->wd_scale = 4;
					break;
				case 4:
					dev->wd_scale = 8;
					break;
				case 5:
					dev->wd_scale = 16;
					break;
				case 6:
					dev->wd_scale = 32;
					break;
				case 7:
					dev->wd_scale = 64;
					break;
				default:
					break;
			}
			dev->wdclk = dev->oscclk / 512 / dev->wd_scale; 
            		if (dev->handle == NULL){
                		dev->handle = system_register_timer_handler(opaque, dev->wdclk * 1000, 
						(time_sched_t)watchdog_callback, SCHED_US|SCHED_NORMAL);
           	 	}
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in sysctrl_c2812\n", offset);
			return Invarg_exp;
	}
	return No_exp;

}

static char* sysctrl_c2812_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t sysctrl_c2812_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	sysctrl_c2812_device *dev = conf_obj->obj;
	sysctrl_c2812_reg_t* regs = dev->regs;
	uint16_t* regs_value = (uint16_t*)regs + id;
	return *regs_value;
}

static char* sysctrl_c2812_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	sysctrl_c2812_device *dev = conf_obj->obj;
	sysctrl_c2812_reg_t* regs = dev->regs;
	*((uint16_t*)regs + id) = value;

	return NULL;
}

static uint32_t sysctrl_c2812_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	sysctrl_c2812_device *dev = conf_obj->obj;
	sysctrl_c2812_reg_t* regs = dev->regs;

	return  sizeof(sysctrl_c2812_reg_t) / 2;
}

static uint32_t sysctrl_c2812_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	sysctrl_c2812_device *dev = conf_obj->obj;
	sysctrl_c2812_reg_t* regs = dev->regs;
	uint32_t regnum;
	regnum = sizeof(sysctrl_c2812_reg_t)/ 2;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t sysctrl_c2812_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

static conf_object_t* new_sysctrl_c2812(char* obj_name){
	sysctrl_c2812_device* dev = skyeye_mm_zero(sizeof(sysctrl_c2812_device));
	sysctrl_c2812_reg_t* regs =  skyeye_mm_zero(sizeof(sysctrl_c2812_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init sysctrl_c2812 regs */
	dev->regs = regs;
	regs->SCSR.all = 0x5;
	regs->HISPCP.all = 0x1;
	regs->LOSPCP.all = 0x2;
	dev->oscclk = 150; //F2812: 150MHZ
	dev->sched_id = -1;

	return dev->obj;
}

void free_sysctrl_c2812(conf_object_t* conf_obj){
	sysctrl_c2812_device* dev = conf_obj->obj;
	skyeye_free(dev->obj);
	skyeye_free(dev->regs);
	skyeye_free(dev);

	return;
}

void config_sysctrl_c2812(conf_object_t* conf_obj){
	sysctrl_c2812_device* dev = conf_obj->obj;
}

static exception_t set_attr_general_signal(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	sysctrl_c2812_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);

	if (dev->signal_iface == NULL){
		if (dev->signal == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t get_attr_general_signal(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	sysctrl_c2812_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

static sysctrl_c2812_register_attribute(conf_class_t* clss){
}

void init_sysctrl_c2812(){
	static skyeye_class_t class_data = {
		.class_name = "c2812_sysctrl",
		.class_desc = "c2812_sysctrl",
		.new_instance = new_sysctrl_c2812,
		.free_instance = free_sysctrl_c2812,
		.config_instance = config_sysctrl_c2812,
		.get_attr = NULL,
		.set_attr = NULL
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = sysctrl_c2812_read,
		.write = sysctrl_c2812_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = sysctrl_c2812_get_regvalue_by_id,
		.get_regname_by_id = sysctrl_c2812_get_regname_by_id,
		.set_regvalue_by_id = sysctrl_c2812_set_regvalue_by_id,
		.get_regnum = sysctrl_c2812_get_regnum,
		.get_regid_by_name = sysctrl_c2812_get_regid_by_name,
		.get_regoffset_by_id = sysctrl_c2812_get_regoffset_by_id,
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
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = set_attr_general_signal,
			.get = get_attr_general_signal,
		},
	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	sysctrl_c2812_register_attribute(clss);
}
