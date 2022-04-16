/*
 * =====================================================================================
 *
 *       Filename:  leon2_timer_unit.c
 *
 *    Description:  This file implementes the LEON2 processor timer unit
 *
 *        Version:  1.0
 *        Created:  24/06/08 10:31:42
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aitor Viana Sanchez (avs), aitor.viana.sanchez@esa.int
 *        Company:  European Space Agency (ESA-ESTEC)
 *
 * =====================================================================================
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

#include "io_leon2.h"

#define PIO_IRQ0  4
#define PIO_IRQ1  5
#define PIO_IRQ2  6
#define PIO_IRQ3  7

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  leon2_io_write
 *  Description:  TBD
 * =====================================================================================
 */

static exception_t io_leon2_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	
	//skyeye_log(Error_log, __FUNCTION__, "write offset :%x, %s, value:%x\n", offset, opaque->objname, *buf);
	struct io_leon2_device *dev = opaque->obj;
	io_leon2_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t*)buf;
	switch(offset) 
	{
		case PIO_DATA:
			regs->piodata = val;
			if ((regs->piodata & 0xe0 == 0xe0) && (regs->piodata & 0x1f)){
			    if (dev->signal_iface){
					dev->signal_iface->raise_signal(dev->signal, PIO_IRQ0);
				}	
			}else if ((regs->piodata & 0xe000 == 0xe000) && (regs->piodata & 0x1f00)){
			    if (dev->signal_iface){
					dev->signal_iface->raise_signal(dev->signal, PIO_IRQ1);
				}	
			}else if ((regs->piodata & 0xe00000 == 0xe00000) && (regs->piodata & 0x1f0000)){
			    if (dev->signal_iface){
					dev->signal_iface->raise_signal(dev->signal, PIO_IRQ2);
				}	
			}else if ((regs->piodata & 0xe0000000 == 0xe0000000) && (regs->piodata & 0x1f000000)){
			    if (dev->signal_iface){
					dev->signal_iface->raise_signal(dev->signal, PIO_IRQ3);
				}	
			}
			break;
		case PIO_DIR:
			regs->piodir = val;
			break;
		case PIO_IRQ:
			regs->pioirq = val;
			break;
		default:
			printf("Can not write the register at 0x%x in io_leon2\n", offset);
		return Invarg_exp;
	}
	return No_exp;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  io_leon2_read
 *  Description:  TBD
 * =====================================================================================
 */
static exception_t io_leon2_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct io_leon2_device *dev = opaque->obj;
	io_leon2_reg_t* regs = dev->regs;

	//skyeye_log(Error_log, __FUNCTION__, "read offset :%x, %s\n", offset, opaque->objname);
	switch(offset){
		case PIO_DATA:
			*(uint32_t *)buf = regs->piodata;
			break;
		case PIO_DIR:
			*(uint32_t *)buf = regs->piodir;
			break;
		case PIO_IRQ:
			*(uint32_t *)buf = regs->pioirq;
			break;
		default:
			printf("Can not read the register at 0x%x in io_leon2\n", offset);
			return Invarg_exp;
	}

    return No_exp;
}

static char* io_leon2_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t io_leon2_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	io_leon2_device *dev = conf_obj->obj;
	io_leon2_reg_t* regs = dev->regs;
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static exception_t io_leon2_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	io_leon2_device *dev = opaque->obj;
}


static conf_object_t* new_io_leon2(char* obj_name){
	io_leon2_device* dev = skyeye_mm_zero(sizeof(io_leon2_device));
	io_leon2_reg_t* regs =  skyeye_mm_zero(sizeof(io_leon2_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init io_leon2 regs */
	dev->regs = regs;

	return dev->obj;
}

static void free_io_leon2(conf_object_t* dev){
	return;	
}

static void io_register_interface(conf_class_t* clss){
	/* Register io function to the object */
	static const memory_space_intf io_memory = {
		.read = io_leon2_read,
		.write = io_leon2_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = io_leon2_get_regvalue_by_id,
		.get_regname_by_id = io_leon2_get_regname_by_id,
		.set_regvalue_by_id = NULL,
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
	class_register_ifaces(clss, ifaces);
	return;
}

static attr_value_t get_attr_signal(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	io_leon2_device* dev = (io_leon2_device*)(conf_obj->obj);
	attr_value_t signal = SKY_make_attr_object(dev->signal);
	return signal;
}

static exception_t set_attr_signal(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	io_leon2_device* dev = (io_leon2_device*)(conf_obj->obj);
	dev->signal = SKY_attr_object(*value);
	dev->signal_iface = (general_signal_intf *)SKY_get_iface(value->u.object, GENERAL_SIGNAL_INTF_NAME);
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	io_leon2_device* dev = (io_leon2_device*)(conf_obj->obj);
	attr_value_t int_number = SKY_make_attr_uinteger(dev->int_number);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	io_leon2_device* dev = (io_leon2_device*)(conf_obj->obj);
	dev->int_number = SKY_attr_uinteger(*value);
	return No_exp;
}


static io_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "signal", get_attr_signal, NULL, set_attr_signal, NULL, SKY_Attr_Optional, "object", " interrupt controler  connected");
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of timer");

}

void init_io_leon2(){
	static skyeye_class_t class_data = {
		.class_name = "leon2_io",
		.class_desc = "leon2_io",
		.new_instance = new_io_leon2,
		.free_instance = free_io_leon2,
		.get_attr = NULL,
		.set_attr = io_leon2_set_attr 
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	io_register_interface(clss);
	io_register_attribute(clss);
}

