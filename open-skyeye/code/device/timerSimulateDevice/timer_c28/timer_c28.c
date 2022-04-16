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
 * @file timer_c28.c
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

#include "timer_c28.h"

#define TIMER_CPU0_INT 7
#define TIMER_CPU1_INT 13
#define TIMER_CPU2_INT 14

void count_timer_func(conf_object_t *opaque){
	struct timer_c28_device *dev = opaque->obj;
	timer_c28_reg_t* regs = dev->regs;
	int int_number = dev->interrupt_number;
	//interrupt_signal_t interrupt_signal;
	c28_signal_t c28_signal;

	/* trigger the interrupt */
	if (regs->TCR.bit.TIE && regs->TCR.bit.TSS == 0){
		if (int_number == TIMER_CPU0_INT){
			if (dev->signal_iface)
				dev->signal_iface->raise_signal(dev->signal, int_number);
		}else{
			if (dev->signal_iface)
				dev->signal_iface->raise_signal(dev->signal, int_number*8);
#if 0
			if (dev->core_iface){
				int i = 1;
				for(; i < 16; i++){
					c28_signal.cpu_intr[i] = Prev_level;
				}
				//c28_signal.cpu_intr[int_number - 1] = High_level;
				c28_signal.pie_group = int_number - 1;
				c28_signal.irq_type = Non_Periph_Type;

				dev->core_iface->signal(dev->core_signal, &c28_signal);
			}
#endif
		}
	}

	return;
}

static exception_t timer_c28_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct timer_c28_device *dev = opaque->obj;
	timer_c28_reg_t* regs = dev->regs;
	switch(offset) {
		case TIM_LSW:
			*(uint16_t *)buf = regs->TIM.half.LSW;
			break;
		case TIM_MSW:
			*(uint16_t *)buf = regs->TIM.half.MSW;
			break;
		case PRD_LSW:
			*(uint16_t *)buf = regs->PRD.half.LSW;
			break;
		case PRD_MSW:
			*(uint16_t *)buf = regs->PRD.half.MSW;
			break;
		case TCR_REG:
			*(uint16_t *)buf = regs->TCR.all;
			break;
		case TPR_LSW:
			*(uint16_t *)buf = regs->TPR.all;
			break;
		case TPRH_MSW:
			*(uint16_t *)buf = regs->TPRH.all;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in timer_c28\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

static exception_t timer_c28_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct timer_c28_device *dev = opaque->obj;
	timer_c28_reg_t* regs = dev->regs;
	uint16_t val = *(uint16_t*)buf;
	switch(offset) {
		case TIM_LSW:
			regs->TIM.half.LSW = val;
			break;
		case TIM_MSW:
			regs->TIM.half.MSW = val;
			break;
		case PRD_LSW:
			regs->PRD.half.LSW = val;
			break;
		case PRD_MSW:
			regs->PRD.half.MSW = val;
			break;
		case TCR_REG:
			regs->TCR.all = val;
			if (regs->TCR.bit.TSS == 0){
				double period = regs->PRD.all / CPU_FREQ;
				if (period <= 0.0){
					period = 1000.0;
				}
				if (dev->handle == NULL)
					dev->handle = system_register_timer_handler(opaque, (int)period, (time_sched_t)count_timer_func, SCHED_US|SCHED_NORMAL);

			}
			// Reload Timer With period Value:
			if (regs->TCR.bit.TRB)
				regs->TIM.all = regs->PRD.all;
			break;
		case TPR_LSW:
			regs->TPR.all = val;
			break;
		case TPRH_MSW:
			regs->TPRH.all = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in timer_c28\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

static char* timer_c28_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t timer_c28_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	timer_c28_device *dev = conf_obj->obj;
	timer_c28_reg_t* regs = dev->regs;
	uint16_t* regs_value = (uint16_t*)regs + id;
	return *regs_value;
}

static char* timer_c28_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	timer_c28_device *dev = conf_obj->obj;
	timer_c28_reg_t* regs = dev->regs;
	*((uint16_t*)regs + id) = value;

	return NULL;
}

static uint32_t timer_c28_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	timer_c28_device *dev = conf_obj->obj;
	timer_c28_reg_t* regs = dev->regs;

	return REG_NUM;
}

static uint32_t timer_c28_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	timer_c28_device *dev = conf_obj->obj;
	timer_c28_reg_t* regs = dev->regs;
	uint32_t regnum;
	regnum = REG_NUM;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}

	return regnum;
}

static uint32_t timer_c28_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

static conf_object_t* new_timer_c28(char* obj_name){
	timer_c28_device* dev = skyeye_mm_zero(sizeof(timer_c28_device));
	timer_c28_reg_t* regs =  skyeye_mm_zero(sizeof(timer_c28_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init timer_c28 regs */
	dev->regs = regs;

	return dev->obj;
}

static void free_timer_c28(conf_object_t* conf_obj){
	timer_c28_device* dev = conf_obj->obj;
	skyeye_free(dev->obj);
	skyeye_free(dev->regs);
	skyeye_free(dev);

	return;	
}

static void config_timer_c28(conf_object_t* conf_obj){
	timer_c28_device* dev = conf_obj->obj;
	//system_register_timer_handler(conf_obj, 1, (time_sched_t)count_timer_func, SCHED_MS|SCHED_NORMAL);
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	timer_c28_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	timer_c28_device *dev = conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static timer_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of timer");
}

static exception_t set_attr_general_signal(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	timer_c28_device *dev = obj->obj;
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
	timer_c28_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

static exception_t set_attr_core_signal(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	timer_c28_device *dev = obj->obj;
	dev->core_signal = obj2;
	dev->core_iface = (core_signal_intf *)SKY_get_iface(dev->core_signal, CORE_SIGNAL_INTF_NAME);

	if (dev->core_iface == NULL){
		if (dev->core_signal == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", CORE_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", CORE_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t get_attr_core_signal(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	timer_c28_device *dev = obj->obj;
	*obj2 = dev->core_signal;
	*port = NULL;
	return No_exp;
}

void init_timer_c28(){
	static skyeye_class_t class_data = {
		.class_name = "c28_timer",
		.class_desc = "c28_timer",
		.new_instance = new_timer_c28,
		.free_instance = free_timer_c28,
		.config_instance = config_timer_c28,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_c2812 SC_f28335),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = timer_c28_read,
		.write = timer_c28_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = timer_c28_get_regvalue_by_id,
		.get_regname_by_id = timer_c28_get_regname_by_id,
		.set_regvalue_by_id = timer_c28_set_regvalue_by_id,
		.get_regnum = timer_c28_get_regnum,
		.get_regid_by_name = timer_c28_get_regid_by_name,
		.get_regoffset_by_id = timer_c28_get_regoffset_by_id,
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
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = set_attr_general_signal,
			.get = get_attr_general_signal,
		},
		(struct ConnectDescription ){
			.name = CORE_SIGNAL_INTF_NAME,
			.set = set_attr_core_signal,
			.get = get_attr_core_signal,
		},

	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	timer_register_attribute(clss);
}
