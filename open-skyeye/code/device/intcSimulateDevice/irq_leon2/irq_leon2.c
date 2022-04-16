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

#include "irq_leon2.h"
#include "../arch/sparc/common/bits.h"


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  leon2_irq_write
 *  Description:  TBD
 * =====================================================================================
 */

static exception_t irq_leon2_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	
//	skyeye_log(Error_log, __FUNCTION__, "write offset :%x, %s, value:%x\n", offset, opaque->objname, *buf);
	struct irq_leon2_device *dev = opaque->obj;
	irq_leon2_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t*)buf;
	switch(offset) 
	{
		case IRQ_MSK:
			regs->irqmask = val;
			break;
		case IRQ_PEND:
			regs->irqpend = val;
			break;
		case IRQ_FORCE:
			regs->irqforce = val;
			break;
		case IRQ_CLEAR:
			regs->irqclear &= ~val;
			break;
		default:
			printf("Can not write the register at 0x%x in irq_leon2\n", offset);
		return Invarg_exp;
	}
	return No_exp;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  irq_timer_read
 *  Description:  TBD
 * =====================================================================================
 */
static exception_t irq_leon2_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct irq_leon2_device *dev = opaque->obj;
	irq_leon2_reg_t* regs = dev->regs;

//	skyeye_log(Error_log, __FUNCTION__, "read offset :%x, %s\n", offset, opaque->objname);
	switch(offset){
		case IRQ_MSK:
			*(uint32_t *)buf = regs->irqmask;
			break;
		case IRQ_PEND:
			*(uint32_t *)buf = regs->irqpend;
			//regs->irqpend = 0;
			break;
		case IRQ_FORCE:
			*(uint32_t *)buf = regs->irqforce;
			//regs->irqforce = 0;
			break;
		case IRQ_CLEAR:
			*(uint32_t *)buf = regs->irqclear;
			break;
		default:
			printf("Can not read the register at 0x%x in irq_leon2\n", offset);
			return Invarg_exp;
	}

    return No_exp;

}

static char* irq_leon2_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t irq_leon2_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	irq_leon2_device *dev = conf_obj->obj;
	irq_leon2_reg_t* regs = dev->regs;
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static exception_t irq_leon2_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	irq_leon2_device *dev = opaque->obj;
}

static uint32_t irq_leon2_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	return  4;
}

static uint32_t irq_leon2_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	irq_leon2_device *dev = conf_obj->obj;
	irq_leon2_reg_t *regs = dev->regs;
	uint32_t regnum;
	regnum = 10;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}



static conf_object_t* new_irq_leon2(char* obj_name){
	irq_leon2_device* dev = skyeye_mm_zero(sizeof(irq_leon2_device));
	irq_leon2_reg_t* regs =  skyeye_mm_zero(sizeof(irq_leon2_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init irq_leon2 regs */
	dev->regs = regs;

	return dev->obj;
}

static void free_irq_leon2(conf_object_t* opaque){
	irq_leon2_device *dev = opaque->obj;
    skyeye_free(dev->regs);
    skyeye_free(dev->obj);
    skyeye_free(dev);
	return;	
}


static void leon2_update_int(irq_leon2_device *dev, int line)
{
	interrupt_signal_t interrupt_signal;
	if((PENDREG != 0) || (FORCEREG != 0)){
		interrupt_signal.sparc_signal.irq_no = line;
		if (MASKREG & 0xffff0000){
			interrupt_signal.sparc_signal.level = High_level;
		}else{
			interrupt_signal.sparc_signal.level = Low_level;
		}

		if (dev->core_iface){
			dev->core_iface->signal(dev->signal, &interrupt_signal);
		}
	}
}
static int irq_leon2_raise_signal(conf_object_t *opaque, int line){
	irq_leon2_device *dev = opaque->obj;
	MASKREG &= 0xfffffffe;
	if(line > 0 && line <= 15){
		PENDREG |= (1 << line) & 0xfffe & MASKREG;
		FORCEREG |= (1 << line) & 0xfffe & MASKREG;
		leon2_update_int(dev, line);
	}
	
	return No_exp;
}

static int irq_leon2_lower_signal(conf_object_t *opaque, int line)
{
	struct irq_leon2_device *dev = opaque->obj;

	return No_exp;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	irq_leon2_device* dev = (irq_leon2_device*)(obj->obj);
	dev->signal = obj2;
	dev->core_iface = (core_signal_intf *)SKY_get_iface(dev->signal, CORE_SIGNAL_INTF_NAME);
	if (dev->core_iface == NULL){
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
	irq_leon2_device* dev = (irq_leon2_device*)(obj->obj);
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}
void init_irq_leon2(){
	static skyeye_class_t class_data = {
		.class_name = "leon2_irq",
		.class_desc = "leon2_irq",
		.new_instance = new_irq_leon2,
		.free_instance = free_irq_leon2,
		.get_attr = NULL,
		.set_attr = irq_leon2_set_attr,
		.module_type = SKYML_ONCHIP(SC_leon2),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);


	static const memory_space_intf io_memory = {
		.read = irq_leon2_read,
		.write = irq_leon2_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = irq_leon2_get_regvalue_by_id,
		.get_regname_by_id = irq_leon2_get_regname_by_id,
		.set_regvalue_by_id = NULL,
		.get_regnum = irq_leon2_get_regnum,
		.get_regid_by_name = irq_leon2_get_regid_by_name,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const general_signal_iface_t signal_iface = {
		.raise_signal = irq_leon2_raise_signal,
		.lower_signal = irq_leon2_lower_signal
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
		},
		(struct InterfaceDescription) {
			.name = GENERAL_SIGNAL_INTF_NAME, 
			.iface = &signal_iface,
		},
 	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription) {
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		}
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	return;
}
