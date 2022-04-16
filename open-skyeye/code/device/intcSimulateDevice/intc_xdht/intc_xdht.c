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
* @file intc_xdht.c
* @brief The implementation of system controller
* @author Kewei Yu: keweihk@gmail.com
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
#include <skyeye_attribute.h>
#define DEBUG
#include <skyeye_log.h>

#include "intc_xdht.h"

void reset_intc_xdht(conf_object_t* opaque, const char* parameter);
#if 0
static exception_t xdht_intc_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{     
      xdht_intc_dev *dev = opaque->obj;
       if(!strncmp(attr_name, "signal", strlen("signal"))){                                                                          
              dev->signal = (general_signal_intf *)SKY_get_iface(value->u.object, GENERAL_SIGNAL_INTF_NAME);
       }else if(!strncmp(attr_name, "int_number", strlen("int_number"))){                                       
	       dev->interrupt_number = value->u.uinteger;
       }else{
              printf("parameter error\n");
              return Invarg_exp;
       }
       return No_exp;
}
#endif
static int xdht_intc_raise_signal(conf_object_t *opaque, int line){
	 xdht_intc_dev *dev = opaque->obj;

	if(line < 0 || line > 1024){
		return Not_found_exp;
	}
	uint32 index = line / REG_SIZE;
	uint32 reg_off = line % REG_SIZE;

	if(dev->regs.ier != 0){
		dev->regs.ifr[index] |= (1 << reg_off);
		dev->signal->raise_signal(dev->signal_obj, dev->interrupt_number);
	}

	return No_exp;
}

static int xdht_intc_lower_signal(conf_object_t *opaque, int line){

	return No_exp;
}

static exception_t intc_xdht_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	xdht_intc_dev *dev = opaque->obj;
	uint32 index;
	
	if(REG_ADDR >= REG_IFR0_BASE && REG_ADDR < REG_IFCR0_BASE){
		index = (REG_ADDR - REG_IFR0_BASE);
		*(uint16 *)buf = dev->regs.ifr[index];
	}else if(REG_ADDR == REG_IER0_BASE){
		*(uint16 *)buf = dev->regs.ier;
	}else{
		printf("Can not read the register at 0x%x in intc_xdht\n", offset);
	}
	return No_exp;

}

static exception_t intc_xdht_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	xdht_intc_dev *dev = opaque->obj;
	uint32 index;
	if(REG_ADDR >= REG_IFCR0_BASE && REG_ADDR < REG_IER0_BASE){
		index = (REG_ADDR - REG_IFCR0_BASE);
		dev->regs.ifcr[index] = *(uint16 *)buf;
		dev->regs.ifr[index] &= (~(dev->regs.ifcr[index]));
	}else if(REG_ADDR == REG_IER0_BASE){
		dev->regs.ier = *(uint16 *)buf;
	}else{
		printf("Can not write the register at 0x%x in %s\n", offset, __func__);
	}
	return No_exp;

}
static conf_object_t* new_intc_xdht(char* obj_name){
	xdht_intc_dev* dev = skyeye_mm_zero(sizeof(xdht_intc_dev));
	dev->obj = new_conf_object(obj_name, dev);
#if 0
	/* Register io function to the object */
	memory_space_intf* io_memory = skyeye_mm_zero(sizeof(memory_space_intf));
	io_memory->conf_obj = dev->obj;
	io_memory->read = intc_xdht_read;
	io_memory->write = intc_xdht_write;
	SKY_register_interface((void *)io_memory, obj_name, MEMORY_SPACE_INTF_NAME);
#endif

#if 0
	general_signal_intf* signal = skyeye_mm_zero(sizeof(general_signal_intf));
	signal->conf_obj = dev->obj;
	signal->raise_signal = xdht_intc_raise_signal;
	signal->lower_signal = xdht_intc_lower_signal;
	SKY_register_interface(signal, obj_name, GENERAL_SIGNAL_INTF_NAME);
#endif
	return dev->obj;
}

void reset_intc_xdht(conf_object_t* opaque, const char* parameter)
{
	xdht_intc_dev *dev = opaque->obj;
	return;
}

void free_intc_xdht(conf_object_t* dev){
	
}
static void intc_xdht_register_interface(conf_class_t* class){
	/* Register io function to the object */
	static const memory_space_intf io_memory = {
		.read = intc_xdht_read,
		.write = intc_xdht_write,
	};
	SKY_register_iface(class, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const general_signal_intf signal = {
		.raise_signal = xdht_intc_raise_signal,
		.lower_signal = xdht_intc_lower_signal,
	};
	SKY_register_iface(class, GENERAL_SIGNAL_INTF_NAME, &signal);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		},
		(struct InterfaceDescription) {
			.name = GENERAL_SIGNAL_INTF_NAME,
			.iface = &signal,
		},
 	};
	class_register_ifaces(class, ifaces);
	return;
}
static attr_value_t get_attr_cpu(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	xdht_intc_dev *dev = conf_obj->obj;
	attr_value_t signal = SKY_make_attr_object(dev->signal_obj);
	return signal;
}
static exception_t set_attr_cpu(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	xdht_intc_dev *dev = conf_obj->obj;
	dev->signal_obj = SKY_attr_object(*value);
	dev->signal = (general_signal_intf *)SKY_get_iface(dev->signal_obj, GENERAL_SIGNAL_INTF_NAME);
	return No_exp;
}

static void intc_xdht_register_attribute(conf_class_t* class){
	SKY_register_attribute(class, "signal", get_attr_cpu, NULL, set_attr_cpu, NULL, SKY_Attr_Optional, "object", " interrupt controler  connected");
}

void init_intc_xdht(){
	static skyeye_class_t class_data = {
		.class_name = "xdht_intc",
		.class_desc = "xdht_intc",
		.new_instance = new_intc_xdht,
		.free_instance = free_intc_xdht,
		.reset_instance = reset_intc_xdht,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_OFFCHIP,
	};

	conf_class_t* class = SKY_register_device_class(class_data.class_name, &class_data);
	intc_xdht_register_interface(class);
	intc_xdht_register_attribute(class);
}
