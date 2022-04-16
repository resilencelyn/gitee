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
* @file sysctrl_28335.c
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

#include "sysctrl_28335.h"

#define WAKEINT 8

static void watchdog_callback(conf_object_t *opaque)
{
	sysctrl_28335_device *dev = opaque->obj;
	sysctrl_28335_reg_t* regs = dev->regs;
	
    if (WDDIS == 0)
	    regs->WDCNTR ++;
	if (regs->WDCNTR == 512 && WDENINT){
		if (dev->signal_iface)
			dev->signal_iface->raise_signal(dev->signal, WAKEINT);
	}
}

static exception_t sysctrl_28335_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	sysctrl_28335_device *dev = opaque->obj;
	sysctrl_28335_reg_t* regs = dev->regs;
	switch(offset) {
		case 0x0:
			*(uint16_t *)buf = regs->PLLSTS.all;
			break;
		case 0x9:
			*(uint16_t *)buf = regs->HISPCP.all;
			break;
		case 0xa:
			*(uint16_t *)buf = regs->LOSPCP.all;
			break;
		case 0xb:
			*(uint16_t *)buf = regs->PCLKCR0.all;
			break;
		case 0xc:
			*(uint16_t *)buf = regs->PCLKCR1.all;
			break;
		case 0xd:
			*(uint16_t *)buf = regs->LPMCR0.all;
			break;
		case 0xf:
			*(uint16_t *)buf = regs->PCLKCR3.all;
			break;
		case 0x10:
			*(uint16_t *)buf = regs->PLLCR.all;
			break;
		case 0x11:
			*(uint16_t *)buf = regs->SCSR;
			break;
		case 0x12:
			*(uint16_t *)buf = regs->WDCNTR;
			break;
		case 0x14:
			*(uint16_t *)buf = regs->WDCR;
			break;
		case 0x18:
			*(uint16_t *)buf = regs->WDCR;
			break;
		case 0x1d:
			*(uint16_t *)buf = regs->MAPCNF.all;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in sysctrl_28335\n", offset);
			return Invarg_exp;
	}
	return No_exp;

}

static exception_t sysctrl_28335_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	sysctrl_28335_device *dev = opaque->obj;
	sysctrl_28335_reg_t* regs = dev->regs;
	uint16_t val = *(uint16_t*)buf;
	switch(offset) {
		case 0x0:
			regs->PLLSTS.all = val;
			dev->divsel = regs->PLLSTS.bit.DIVSEL;
			break;
		case 0x9:
			regs->HISPCP.all = val;
			break;
		case 0xa:
			regs->LOSPCP.all = val;
			break;
		case 0xb:
			regs->PCLKCR0.all = val;
			break;
		case 0xc:
			regs->PCLKCR1.all = val;
			break;
		case 0xd:
			regs->LPMCR0.all = val;
			if (regs->LPMCR0.bit.LPM == 0x1){
			//LPM mode = Standby
			}else if (regs->LPMCR0.bit.LPM == 0x0){
			//LPM mode = Idle
			}else if (regs->LPMCR0.bit.LPM == 0x2){
			//LPM mode = Halt
			}
			break;
		case 0xf:
			regs->PCLKCR3.all = val;
			break;
		case 0x10:
			regs->PLLCR.all = val;
			dev->sysclk = dev->oscclk * (regs->PLLCR.bit.DIV)/ 4;
			break;
		case 0x11:
			regs->SCSR = val;
			break;
		case 0x12:
			regs->WDCNTR = val;
			break;
		case 0x14:
			dev->old_key = regs->WDKEY;
			regs->WDKEY = val;
			if (dev->old_key == 0x55 && regs->WDKEY == 0xAA)
				regs->WDCNTR = 0;

			break;
		case 0x18:
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
			if (dev->wdclk == 0){
				dev->wdclk = 1;
			}
            if (dev->handle == NULL){
                dev->handle = system_register_timer_handler(opaque, dev->wdclk * 1000, (time_sched_t)watchdog_callback, SCHED_US|SCHED_NORMAL);
            }
			//create_thread_scheduler(dev->wdclk, Periodic_sched, watchdog_callback, (void*)opaque, &dev->sched_id);
			break;
		case 0x1d:
			regs->MAPCNF.all = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in sysctrl_28335\n", offset);
			return Invarg_exp;
	}
	return No_exp;

}

static char* sysctrl_28335_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t sysctrl_28335_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	sysctrl_28335_device *dev = conf_obj->obj;
	sysctrl_28335_reg_t* regs = dev->regs;
	uint16_t* regs_value = (uint16_t*)regs + id;
	return *regs_value;
}

static char* sysctrl_28335_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	sysctrl_28335_device *dev = conf_obj->obj;
	sysctrl_28335_reg_t* regs = dev->regs;
	*((uint16_t*)regs + id) = value;

	return NULL;
}

static uint32_t sysctrl_28335_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	sysctrl_28335_device *dev = conf_obj->obj;
	sysctrl_28335_reg_t* regs = dev->regs;

	return  sizeof(sysctrl_28335_reg_t) / 2;
}

static uint32_t sysctrl_28335_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	sysctrl_28335_device *dev = conf_obj->obj;
	sysctrl_28335_reg_t* regs = dev->regs;
	uint32_t regnum;
	regnum = sizeof(sysctrl_28335_reg_t)/ 2;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t sysctrl_28335_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

static conf_object_t* new_sysctrl_28335(char* obj_name){
	sysctrl_28335_device* dev = skyeye_mm_zero(sizeof(sysctrl_28335_device));
	sysctrl_28335_reg_t* regs =  skyeye_mm_zero(sizeof(sysctrl_28335_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init sysctrl_28335 regs */
	dev->regs = regs;
	regs->PLLSTS.bit.PLLLOCKS = 1;
	regs->SCSR = 0x5;
	regs->HISPCP.all = 0x1;
	regs->LOSPCP.all = 0x2;
	dev->oscclk = 150; //F28335: 150MHZ
	dev->sched_id = -1;

	return dev->obj;
}
void free_sysctrl_28335(conf_object_t* conf_obj){
	sysctrl_28335_device* dev = conf_obj->obj;
	skyeye_free(dev->obj);
	skyeye_free(dev->regs);
	skyeye_free(dev);

	return;
}

void config_sysctrl_28335(conf_object_t* conf_obj){
	sysctrl_28335_device* dev = conf_obj->obj;
}

static exception_t set_attr_general_signal(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	sysctrl_28335_device *dev = obj->obj;
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
	sysctrl_28335_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

static sysctrl_28335_register_attribute(conf_class_t* clss){
}

void init_sysctrl_28335(){
	static skyeye_class_t class_data = {
		.class_name = "f28335_sysctrl",
		.class_desc = "f28335_sysctrl",
		.new_instance = new_sysctrl_28335,
		.free_instance = free_sysctrl_28335,
		.config_instance = config_sysctrl_28335,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_f28335),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = sysctrl_28335_read,
		.write = sysctrl_28335_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = sysctrl_28335_get_regvalue_by_id,
		.get_regname_by_id = sysctrl_28335_get_regname_by_id,
		.set_regvalue_by_id = sysctrl_28335_set_regvalue_by_id,
		.get_regnum = sysctrl_28335_get_regnum,
		.get_regid_by_name = sysctrl_28335_get_regid_by_name,
		.get_regoffset_by_id = sysctrl_28335_get_regoffset_by_id,
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

	sysctrl_28335_register_attribute(clss);
}
