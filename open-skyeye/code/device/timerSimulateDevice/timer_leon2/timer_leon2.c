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

#include "timer_leon2.h"

#define UNDERFLOW   (0xffffffff)
#define TIMER_CTRL_EN   0x1   //  Enable
#define TIMER_CTRL_RL   0x2   // Reload counter
#define TIMER_CTRL_LD   0x4   // Load counter

#define IRQ_TIMER1_MSK 0x100
#define IRQ_TIMER1_CLR  0x100

#define IRQ_TIMER2_MSK 0x200
#define IRQ_TIMER2_CLR 0x200

#define TIMER1_IRQ  8
#define TIMER2_IRQ  9
#define WDOG_IRQ  15

static void timer_callback(conf_object_t *opaque){
	timer_leon2_device* dev = opaque->obj;
	timer_leon2_reg_t* regs = dev->regs;
	regs->scalercnt--;

	if (regs->scalercnt == UNDERFLOW)
	{
		regs->scalercnt = regs->scalerload;
		if (regs->timerctrl1 & TIMER_CTRL_EN)
		{
			regs->timercnt1--;
			if (dev->signal_iface)
			{
				dev->signal_iface->raise_signal(dev->signal, TIMER1_IRQ);
			}
		}
#if  0
		if (regs->timercnt1 == UNDERFLOW)
		{
			if (regs->timerctrl1 & TIMER_CTRL_RL){
				regs->timercnt1 = regs->timerload1;
			}else {
				regs->timerctrl1 &= 0x6;
			}
			/*  Signal the trap 8*/
			if (dev->signal_iface)
				dev->signal_iface->raise_signal(dev->signal, TIMER1_IRQ);
		}
#endif
		if (regs->timerctrl2 & TIMER_CTRL_EN)
		{
			regs->timercnt2--;
		}

		if (regs->timercnt2 == UNDERFLOW)
		{
			if (regs->timerctrl2 & TIMER_CTRL_RL){
				regs->timercnt2 = regs->timerload2;
			}else
			{
				regs->timerctrl2 &= 0x6;
			}
			/*  Signal the trap 9*/
			if (dev->signal_iface)
				dev->signal_iface->raise_signal(dev->signal, TIMER2_IRQ);
		}

		if (regs->wdog == UNDERFLOW)
		{
			if (dev->signal_iface)
				dev->signal_iface->raise_signal(dev->signal, WDOG_IRQ);
		}
	}
	return;
}

static exception_t timer_leon2_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	//	skyeye_log(Error_log, __FUNCTION__, "write offset :%x, %s, value:%x\n", offset, opaque->objname, *buf);
	struct timer_leon2_device *dev = opaque->obj;
	timer_leon2_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t*)buf;
	val &= 0x00ffffff;

	switch(offset)
	{
		case TCNT1:
			regs->timercnt1 = val;
			break;
		case TLOAD1:
			regs->timerload1 = val;
			break;
		case TCTRL1:
			regs->timerctrl1 = val;
			if (val & TIMER_CTRL_EN){
				int mode = Periodic_sched;
				int us = 1000;
				int id = dev->timer_scheduler_id;

				if (id < 0){
					create_thread_scheduler((unsigned int )us, mode, timer_callback,  (void *)opaque, &id);
					DBG("In %s, thread_scheduler created, us=%d, id=%d\n", __FUNCTION__, us , id);	
					dev->timer_scheduler_id = id;
				}else{
					mod_thread_scheduler(dev->timer_scheduler_id, (unsigned int)us, mode);
				}
			}
			break;
		case WDOG:
			regs->wdog = val;
			break;
		case TCNT2:
			regs->timercnt2 = val;
			break;
		case TLOAD2:
			regs->timerload2 = val;
			break;
		case TCTRL2:
			regs->timerctrl2 = val;
			if (val & TIMER_CTRL_EN){
				int mode = Periodic_sched;
				int us = 100000;
				int id = dev->timer_scheduler_id;

				if (id < 0){
					create_thread_scheduler((unsigned int )us, mode, timer_callback,  (void *)opaque, &id);
					DBG("In %s, thread_scheduler created, us=%d, id=%d\n", __FUNCTION__, us , id);	
					dev->timer_scheduler_id = id;
				}else{
					mod_thread_scheduler(dev->timer_scheduler_id, (unsigned int)us, mode);
				}
			}
			break;
		case DMMY:
			regs->dummy8 = val;
			break;
		case SCNT:
			regs->scalercnt = val;
			break;
		case SLOAD:
			regs->scalerload = val;
			break;
		default:
			printf("Can not write the register at 0x%x in timer_leon2\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  leon2_timer_read
 *  Description:  TBD
 * =====================================================================================
 */
static exception_t timer_leon2_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct timer_leon2_device *dev = opaque->obj;
	timer_leon2_reg_t* regs = dev->regs;
	//	skyeye_log(Error_log, __FUNCTION__, "read offset :%x, %s\n", offset, opaque->objname);

	switch(offset){
		case TCNT1:
			*(uint32_t *)buf = regs->timercnt1;
			break;
		case TLOAD1:
			*(uint32_t *)buf = regs->timerload1;
			break;
		case TCTRL1:
			*(uint32_t *)buf = regs->timerctrl1;
			break;
		case WDOG:
			*(uint32_t *)buf = regs->wdog;
			break;
		case TCNT2:
			*(uint32_t *)buf = regs->timercnt2;
			break;
		case TLOAD2:
			*(uint32_t *)buf = regs->timerload2;
			break;
		case TCTRL2:
			*(uint32_t *)buf = regs->timerctrl2;
			break;
		case DMMY:
			*(uint32_t *)buf = regs->dummy8;
			break;
		case SCNT:
			*(uint32_t *)buf = regs->scalercnt;
			break;
		case SLOAD:
			*(uint32_t *)buf = regs->scalerload;
			break;
		default:
			printf("Can not read the register at 0x%x in timer_leon2\n", offset);
			return Invarg_exp;
	}

	return No_exp;
}

static char* timer_leon2_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t timer_leon2_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	timer_leon2_device *dev = conf_obj->obj;
	timer_leon2_reg_t* regs = dev->regs;
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t timer_leon2_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	return  sizeof(timer_leon2_reg_t) / 4;
}

static uint32_t timer_leon2_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	timer_leon2_device *dev = conf_obj->obj;
	timer_leon2_reg_t *regs = dev->regs;
	uint32_t regnum = sizeof(timer_leon2_reg_t) / 4;
	int i;
	for(i =0; i < regnum; i++)
	{
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;


}

static exception_t timer_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	timer_leon2_device* dev = (timer_leon2_device*)(obj->obj);
	dev->signal = obj2;
	dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);

	if (dev->signal_iface == NULL)
	{
		if (dev->signal == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
		} else
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);
		}

		return Not_found_exp;
	}

	return No_exp;
}

static exception_t timer_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	timer_leon2_device* dev = (timer_leon2_device*)(obj->obj);
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

static conf_object_t* new_timer_leon2(char* obj_name){
	timer_leon2_device* dev = skyeye_mm_zero(sizeof(timer_leon2_device));
	timer_leon2_reg_t* regs =  skyeye_mm_zero(sizeof(timer_leon2_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init timer_leon2 regs */
	dev->regs = regs;

	dev->timer_scheduler_id = -1;

	return dev->obj;
}

static void free_timer_leon2(conf_object_t* conf_obj){
	timer_leon2_device *dev = conf_obj->obj;
	skyeye_free(dev->regs);
	skyeye_free(dev->obj);
	skyeye_free(dev);
	return;
}

static void timer_register_interface(conf_class_t* clss){
	/* Register io function to the object */
	static const memory_space_intf io_memory = {
		.read = timer_leon2_read,
		.write = timer_leon2_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = timer_leon2_get_regvalue_by_id,
		.get_regname_by_id = timer_leon2_get_regname_by_id,
		.set_regvalue_by_id = NULL,
		.get_regnum = timer_leon2_get_regnum,
		.get_regid_by_name = timer_leon2_get_regid_by_name,
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

static void timer_register_connect (conf_class_t* clss)
{
	static const struct ConnectDescription connects[] =
	{
		(struct ConnectDescription)
		{
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = timer_signal_set,
			.get = timer_signal_get,
		},
	};
	class_register_connects(clss, connects);
	return;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	timer_leon2_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	timer_leon2_device *dev = conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static timer_register_attribute(conf_class_t* clss)
{
	SKY_register_attribute(clss, "irq_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of timer");
}

void init_timer_leon2()
{
	static skyeye_class_t class_data =
	{
		.class_name = "leon2_timer",
		.class_desc = "leon2_timer",
		.new_instance = new_timer_leon2,
		.free_instance = free_timer_leon2,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_leon2),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	timer_register_interface(clss);
	timer_register_connect(clss);
	timer_register_attribute(clss);
}

