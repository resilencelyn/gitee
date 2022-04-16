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
* @file timer_c64x.c
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

#include "timer_c64x.h"
static void timer_callback(conf_object_t *opaque){
	struct timer_c64x_device *dev = opaque->obj;
	core_signal_intf* core_signal = dev->core_signal;
	timer_c64x_reg_t* regs = dev->regs;
	/* trigger the interrupt */
	interrupt_signal_t interrupt_signal;
	int i = 0;
	regs->cnt ++;
	if (regs->cnt == regs->prd){
		for(; i < 16; i++)
			interrupt_signal.c6k_signal.ifr[i] = Prev_level;

		interrupt_signal.c6k_signal.ifr[dev->interrupt_number] = High_level;
		core_signal->signal(dev->core, &interrupt_signal);
		regs->cnt = 0;
	}

	return;
}
static exception_t timer_c64x_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct timer_c64x_device *dev = opaque->obj;
	timer_c64x_reg_t* regs = dev->regs;
	switch(offset) {
		case 0x0:
			*(uint32_t *)buf = regs->ctrl;
			break;
		case 0x4:
			*(uint32_t *)buf = regs->prd;
			break;
		case 0x8:
			*(uint32_t *)buf = regs->cnt;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in timer_c64x\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

static exception_t timer_c64x_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct timer_c64x_device *dev = opaque->obj;
	timer_c64x_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t*)buf;
	switch(offset) {
	case 0x0:
		regs->ctrl = val;
		if((val & 0xc0) == 0xc0){ /* start timer */
			int freq = 720;
			int clksrc = 0;
			if ((val >> 9) & 0x1)
				clksrc = freq / 8;
			else 
				clksrc = freq;

			int ms = 1000 * regs->prd / (clksrc * 1000000); 
			ms = ms == 0 ? 1000: ms;
			int id = dev->timer_scheduler_id;

			/* get timer mode */
			/* check if a proper value */
			if(id == -1){
				create_thread_scheduler((unsigned int)ms, Periodic_sched, timer_callback, (void*)opaque, &id);
				dev->timer_scheduler_id = id;
			}else
				/* update timer */
				mod_thread_scheduler(dev->timer_scheduler_id,(unsigned int)ms,Periodic_sched);
		}else if ((val & 0xc0) == 0){ 
			/* stop timer */
			if (dev->timer_scheduler_id != -1) {
				del_thread_scheduler(dev->timer_scheduler_id);
				dev->timer_scheduler_id = -1;
			}
		}
		break;
	case 0x4:
		regs->prd = val;
		break;
	case 0x8:
		regs->cnt = val;
		break;
	default:
		skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in timer_c64x\n", offset);
		return Invarg_exp;
	}
	return No_exp;
}

static char* timer_c64x_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t timer_c64x_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	timer_c64x_device *dev = conf_obj->obj;
	timer_c64x_reg_t* regs = dev->regs;
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static char* timer_c64x_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	timer_c64x_device *dev = conf_obj->obj;
	timer_c64x_reg_t* regs = dev->regs;
	*((uint32_t*)regs + id) = value;

	return NULL;
}

static uint32_t timer_c64x_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	timer_c64x_device *dev = conf_obj->obj;
	timer_c64x_reg_t* regs = dev->regs;
	return  sizeof(timer_c64x_reg_t) / 4;
}

static uint32_t timer_c64x_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	timer_c64x_device *dev = conf_obj->obj;
	timer_c64x_reg_t* regs = dev->regs;
	uint32_t regnum;
	regnum = sizeof(timer_c64x_reg_t)/ 4;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;


}
static exception_t timer_c64x_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	timer_c64x_device *dev = opaque->obj;
	if(!strncmp(attr_name, "cpu", strlen("cpu"))){
		dev->core_signal = (core_signal_intf *)SKY_get_iface(value->u.object, CORE_SIGNAL_INTF_NAME);
	}else if(!strncmp(attr_name, "int_number", strlen("int_number"))){
		dev->interrupt_number = value->u.uinteger;
		skyeye_log(Error_log, __FUNCTION__, "in %s, set interrupt_number %d\n", __FUNCTION__, dev->interrupt_number);
	}
	else{
		skyeye_log(Error_log, __FUNCTION__, "No such attribute\n");
	}

}

static conf_object_t* new_timer_c64x(char* obj_name){
	timer_c64x_device* dev = skyeye_mm_zero(sizeof(timer_c64x_device));
	timer_c64x_reg_t* regs =  skyeye_mm_zero(sizeof(timer_c64x_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init timer_c64x regs */
	dev->regs = regs;

	dev->timer_scheduler_id = -1;
	return dev->obj;
}
static void free_timer_c64x(conf_object_t* conf_obj){
    	timer_c64x_device* dev = conf_obj->obj;
    	skyeye_free(dev->regs);
    	skyeye_free(dev);
    	skyeye_free(conf_obj);
	return;	
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	timer_c64x_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	timer_c64x_device *dev = conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static timer_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of timer");
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	timer_c64x_device *dev = obj->obj;
	dev->core = obj2;
	dev->core_signal = (core_signal_intf *)SKY_get_iface(dev->core, CORE_SIGNAL_INTF_NAME);

	if (dev->core_signal == NULL){
		if (dev->core == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", CORE_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", CORE_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}

	return No_exp;
}

static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	timer_c64x_device *dev = obj->obj;
	*obj2 = dev->core;
	*port = NULL;
	return No_exp;
}

void init_timer_c64x(){
	static skyeye_class_t class_data = {
		.class_name = "c64x_timer",
		.class_desc = "c64x_timer",
		.new_instance = new_timer_c64x,
		.free_instance = free_timer_c64x,
		.get_attr = NULL,
		.set_attr = timer_c64x_set_attr,
		.module_type = SKYML_ONCHIP(SC_c6416),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = timer_c64x_read,
		.write = timer_c64x_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = timer_c64x_get_regvalue_by_id,
		.get_regname_by_id = timer_c64x_get_regname_by_id,
		.set_regvalue_by_id = timer_c64x_set_regvalue_by_id,
		.get_regnum = timer_c64x_get_regnum,
		.get_regid_by_name = timer_c64x_get_regid_by_name,
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
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		},

	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	timer_register_attribute(clss);
}
