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

#include "sysctrl_c5510.h"


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  sysctrl_c5510_write
 *  Description:  TBD
 * =====================================================================================
 */

static exception_t sysctrl_c5510_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
//	skyeye_log(Error_log, __FUNCTION__, "write offset :%x, %s, value:%x\n", offset, opaque->objname, *buf);
	struct sysctrl_c5510_device *dev = opaque->obj;
	sysctrl_c5510_reg_t* regs = dev->regs;
	uint8_t val = *(uint8_t*)buf;
	switch(offset) 
	{
		case 0x0:
			regs->memcntl = val;
			break;
		case 0x1:
			regs->syscntl = val;
			break;
		case 0x11:
			regs->wdog = val;
			break;
		case 0x2:
			regs->systate = val;
			break;
		default:
			printf("Can not write the register at 0x%x in sysctrl_c5510\n", offset);
		return Invarg_exp;
	}
	return No_exp;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: sysctrl_c5510_read 
 *  Description:  TBD
 * =====================================================================================
 */
static exception_t sysctrl_c5510_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct sysctrl_c5510_device *dev = opaque->obj;
	sysctrl_c5510_reg_t* regs = dev->regs;

//	skyeye_log(Error_log, __FUNCTION__, "read offset :%x, %s\n", offset, opaque->objname);
	switch(offset){
		case 0x0:
			*(uint8_t *)buf = regs->memcntl;
			break;
		case 0x1:
			*(uint8_t *)buf = SYSTATE0_REG;
			//regs->irqpend = 0;
			break;
		case 0x11:
			*(uint8_t *)buf = regs->wdog;
			//regs->irqforce = 0;
			break;
		case 0x2:
			*(uint8_t *)buf = regs->systate;
			break;
		default:
			printf("Can not read the register at 0x%x in sysctrl_c5510\n", offset);
			return Invarg_exp;
	}

    return No_exp;
}

static char* sysctrl_c5510_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t sysctrl_c5510_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	sysctrl_c5510_device *dev = conf_obj->obj;
	sysctrl_c5510_reg_t* regs = dev->regs;
	uint8_t* regs_value = (uint8_t*)regs + id;
	return *regs_value;
}

static char* sysctrl_c5510_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	sysctrl_c5510_device *dev = conf_obj->obj;
	sysctrl_c5510_reg_t* regs = dev->regs;
	*((uint8_t*)regs + id) = value;

	return NULL;
}
static uint32_t sysctrl_c5510_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	return  sizeof(sysctrl_c5510_reg_t);
}

static uint32_t sysctrl_c5510_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	sysctrl_c5510_device *dev = conf_obj->obj;
	sysctrl_c5510_reg_t* regs = dev->regs;
	uint32_t regnum;
	regnum = sizeof(sysctrl_c5510_reg_t);
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static uint32_t sysctrl_c5510_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

static conf_object_t* new_sysctrl_c5510(char* obj_name){
	sysctrl_c5510_device* dev = skyeye_mm_zero(sizeof(sysctrl_c5510_device));
	sysctrl_c5510_reg_t* regs =  skyeye_mm_zero(sizeof(sysctrl_c5510_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init sysctrl_c5510 regs */
	dev->regs = regs;

	return dev->obj;
}

static void free_sysctrl_c5510(conf_object_t* conf_obj){
	sysctrl_c5510_device* dev = conf_obj->obj;
	skyeye_free(dev->obj);
	skyeye_free(dev);
	return;	
}

static int sysctrl_c5510_raise_signal(conf_object_t *opaque, int int_number){
	sysctrl_c5510_device *dev = opaque->obj;
	interrupt_signal_t interrupt_signal;
	switch(int_number){
		case UARTINTB:
		case UARTINTA:
			interrupt_signal.c5k_signal.ifr[6] = High_level;
		case DBINT1:
		case DBINT2:
			break;
		case USBINT:
			interrupt_signal.c5k_signal.ifr[20] = High_level;
			break;
		default:
			break;
	}

	if (dev->signal_iface){
		dev->signal_iface->signal(dev->signal, &interrupt_signal);
	}
	return No_exp;
}

static int sysctrl_c5510_lower_signal(conf_object_t *opaque, int int_number)
{
	struct sysctrl_c5510_device *dev = opaque->obj;

	return No_exp;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	struct sysctrl_c5510_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (core_signal_intf *)SKY_get_iface(dev->signal, CORE_SIGNAL_INTF_NAME);

	if (dev->signal_iface == NULL){
		if (dev->signal == NULL)
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
	struct sysctrl_c5510_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

void init_sysctrl_c5510(){
	static skyeye_class_t class_data = {
		.class_name = "c5510_sysctrl",
		.class_desc = "c5510_sysctrl",
		.new_instance = new_sysctrl_c5510,
		.free_instance = free_sysctrl_c5510,
		.get_attr = NULL,
		.set_attr = NULL 
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = sysctrl_c5510_read,
		.write = sysctrl_c5510_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = sysctrl_c5510_get_regvalue_by_id,
		.get_regname_by_id = sysctrl_c5510_get_regname_by_id,
		.set_regvalue_by_id = sysctrl_c5510_set_regvalue_by_id,
		.get_regnum = sysctrl_c5510_get_regnum,
		.get_regid_by_name = sysctrl_c5510_get_regid_by_name,
		.get_regoffset_by_id = sysctrl_c5510_get_regoffset_by_id,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const general_signal_iface_t signal_iface = {
		.raise_signal = sysctrl_c5510_raise_signal,
		.lower_signal = sysctrl_c5510_lower_signal
	};
	SKY_register_iface(clss, GENERAL_SIGNAL_INTF_NAME, &signal_iface);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf,
		},
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME, 
			.iface = &io_memory,
		}
 	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription) {
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		},
	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}

