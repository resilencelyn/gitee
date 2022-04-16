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
* @file intc_am3359.c
* @brief The implementation of system controller
* @author Kewei Yu: keweihk@gmail.com
* @version 78.77
*/

#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_attribute.h>
#include <skyeye_iface.h>
#define DEBUG
#include <skyeye_log.h>
#include <skyeye_core.h>

#include "intc_am3359.h"
static exception_t reset_intc_am3359(conf_object_t* opaque, const char* args);
static int intc_am3359_raise_signal(conf_object_t *opaque, int line)
{
	intc_am3359_device *dev = opaque->obj;
	intc_am3359_reg_t *regs = &dev->regs;
	interrupt_signal_t interrupt_signal;
	int index = line / 32, bits = line % 32, priority, isfiq, threshold;

	regs->itr[index] |= 1 << bits;

	if (regs->mir[index] & (1 << bits))
		return Not_found_exp;

	threshold = regs->threshold & 0xff;
	isfiq = regs->ilr[line] & 0x1;
	priority = (regs->ilr[line] & (0x3f << 2)) >> 2;

	if (isfiq) regs->pending_fiq[index] |= 1 << bits;
	else regs->pending_irq[index] |= 1 << bits;

	if (priority > threshold || threshold == 0xff || priority == 0) {
		/* after priority sorting, here we ignore sorting step */
		if (isfiq) {
			regs->pending_fiq[index] |= 1 << bits;

			regs->sir_fiq = regs->fiq_priority;
			regs->sir_fiq &= ~0x7f;
			regs->sir_fiq |= line;

			regs->fiq_priority &= ~0x7f;
			regs->fiq_priority |= priority;

			/*firq*/
			interrupt_signal.arm_signal.firq = Low_level;
			interrupt_signal.arm_signal.irq = Prev_level;

			interrupt_signal.arm_signal.reset = Prev_level;
			if (dev->signal_iface)
				dev->signal_iface->signal(dev->signal, &interrupt_signal);
		} else {
			regs->pending_irq[index] |= 1 << bits;

			regs->sir_irq = regs->irq_priority;
			regs->sir_irq &= ~0x7f;
			regs->sir_irq |= line;

			regs->irq_priority &= ~0x7f;
			regs->irq_priority |= priority;

			/*irq*/
			interrupt_signal.arm_signal.firq = Prev_level;
			interrupt_signal.arm_signal.irq = Low_level;

			interrupt_signal.arm_signal.reset = Prev_level;
			if (dev->signal_iface)
				dev->signal_iface->signal(dev->signal, &interrupt_signal);
		}
	}
	return No_exp;
}
static int intc_am3359_lower_signal(conf_object_t *opaque, int line)
{
	intc_am3359_device *dev = opaque->obj;
	intc_am3359_reg_t *regs = &dev->regs;
	interrupt_signal_t interrupt_signal;

	if ((dev->regs.ilr[line] & 0x1) == 0x0) {
		/* irq */
		interrupt_signal.arm_signal.irq = High_level;
		interrupt_signal.arm_signal.firq = Prev_level;
	} else {
		/* fiq */
		interrupt_signal.arm_signal.firq = High_level;
		interrupt_signal.arm_signal.irq = Prev_level;
	}
}
static void handle_intc_control(conf_object_t *opaque, uint32_t val)
{
	intc_am3359_device *dev = opaque->obj;
	intc_am3359_reg_t* regs = &dev->regs;
	int line, index, bits, i;

	regs->control |= val;

	if (val == 0x1) {
		/* irq */
		line = regs->sir_irq & 0x7f;
		index = line / 32;
		bits = line % 32;

		regs->itr[index] &= ~(1 << bits);
		regs->pending_irq[index] &= ~(1 << bits);

	} else if (val == 0x2) {
		/* fiq */
		line = regs->sir_fiq & 0x7f;
		index = line / 32;
		bits = line % 32;

		regs->itr[index] &= ~(1 << bits);
		regs->pending_fiq[index] &= ~(1 << bits);
	}

	intc_am3359_lower_signal(opaque, line);
}
static exception_t intc_am3359_read(conf_object_t *opaque,
		generic_address_t offset, void* buf, size_t count)
{
	intc_am3359_device *dev = opaque->obj;
	intc_am3359_reg_t *regs = &dev->regs;
	uint32_t index = 0, *val = (uint32_t *)buf;

	switch(offset) {
		case INTC_REVISION:
			*val = regs->revision;
			break;
		case INTC_SYSCONFIG:
			*val = regs->sysconfig;
			break;
		case INTC_SYSSTATUS:
			*val = regs->sysstatus;
			break;
		/* currently active irq number */
		case INTC_SIR_IRQ:
			*val = regs->sir_irq;
			break;
		/* currently active fiq number */
		case INTC_SIR_FIQ:
			*val = regs->sir_fiq;
			break;
		case INTC_PROTECTION:
			*val = regs->protection;
			break;
		case INTC_IDLE:
			*val = regs->idle;
			break;
		case INTC_IRQ_PRIORITY:
			*val = regs->irq_priority;
			break;
		case INTC_FIQ_PRIORITY:
			*val = regs->fiq_priority;
			break;
		case INTC_THRESHOLD:
			*val = regs->threshold;
			break;
		/* raw interrupt input status before masking */
		case INTC_ITR(0):
		case INTC_ITR(1):
		case INTC_ITR(2):
		case INTC_ITR(3):
			index = (offset - 0x80) / 0x20;
			*val = regs->itr[index];
			break;
		case INTC_MIR(0):
		case INTC_MIR(1):
		case INTC_MIR(2):
		case INTC_MIR(3):
			index = (offset - 0x84) / 0x20;
			*val = regs->mir[index];
			break;
		case INTC_MIR_CLEAR(0):
		case INTC_MIR_CLEAR(1):
		case INTC_MIR_CLEAR(2):
		case INTC_MIR_CLEAR(3):
			*val = 0;
			break;
		case INTC_MIR_SET(0):
		case INTC_MIR_SET(1):
		case INTC_MIR_SET(2):
		case INTC_MIR_SET(3):
			*val = 0;
			break;
		case INTC_ISR_SET(0):
		case INTC_ISR_SET(1):
		case INTC_ISR_SET(2):
		case INTC_ISR_SET(3):
			index = (offset - 0x90) / 0x20;
			/* FIX ME:read return currently active software interrupts */
			break;
		case INTC_ISR_CLEAR(0):
		case INTC_ISR_CLEAR(1):
		case INTC_ISR_CLEAR(2):
		case INTC_ISR_CLEAR(3):
			*val = 0;
			break;
		/* register contains the irq status after masking */
		case INTC_PENDING_IRQ(0):
		case INTC_PENDING_IRQ(1):
		case INTC_PENDING_IRQ(2):
		case INTC_PENDING_IRQ(3):
			index = (offset - 0x98) / 0x20;
			*val = regs->pending_irq[index];
			break;
		/* register contains the fiq status after masking */
		case INTC_PENDING_FIQ(0):
		case INTC_PENDING_FIQ(1):
		case INTC_PENDING_FIQ(2):
		case INTC_PENDING_FIQ(3):
			index = (offset - 0x9c) / 0x20;
			*val = regs->pending_fiq[index];
			break;
		default:
			if(offset >= INTC_ILR(0) && offset <= INTC_ILR(127)) {
				index = (offset - 0x100) / 0x4;
				*val = regs->ilr[index];
				break;
			} else {
				skyeye_log(Warning_log, __FUNCTION__, "Can not read the register at 0x%x in %s\n", offset, __FILE__);
				return Invarg_exp;
			}
	}

	return No_exp;
}
static exception_t intc_am3359_write(conf_object_t *opaque,
		generic_address_t offset, uint32_t* buf, size_t count)
{
	intc_am3359_device *dev = opaque->obj;
	intc_am3359_reg_t* regs = &dev->regs;
	uint32_t val = *(uint32_t*)buf, line, index = 0;

	switch(offset) {
		case INTC_SYSCONFIG:
			if(val & 0x2){
				reset_intc_am3359(opaque, NULL);
				regs->sysstatus |= 0x1;
			}
			break;
		/* currently active irq number */
		case INTC_SIR_IRQ:
			regs->sir_irq = val;
			break;
		case INTC_CONTROL:
			handle_intc_control(opaque, val);
			break;
		case INTC_PROTECTION:
			regs->protection = val;
			break;
		case INTC_IDLE:
			regs->idle = val;
			break;
		case INTC_THRESHOLD:
			regs->threshold = val;
			break;
		case INTC_MIR(0):
		case INTC_MIR(1):
		case INTC_MIR(2):
		case INTC_MIR(3):
			index = (offset - 0x84) / 0x20;
			regs->mir[index]= val;
			break;
		case INTC_MIR_CLEAR(0):
		case INTC_MIR_CLEAR(1):
		case INTC_MIR_CLEAR(2):
		case INTC_MIR_CLEAR(3):
			index = (offset - 0x88) / 0x20;
			regs->mir[index] &= ~val;
			break;
		case INTC_MIR_SET(0):
		case INTC_MIR_SET(1):
		case INTC_MIR_SET(2):
		case INTC_MIR_SET(3):
			index = (offset - 0x8c) / 0x20;
			regs->mir[index] |= val;
			break;
		case INTC_ISR_SET(0):
		case INTC_ISR_SET(1):
		case INTC_ISR_SET(2):
		case INTC_ISR_SET(3):
			index = (offset - 0x90) / 0x20;
			regs->isr_set[index] = val;
			/* set the software interrupt bits */
			regs->itr[index] |= val;
			for (line = 0; line < 32; line++)
				if (val & (1 << line))
					break;
			line += index * 32;
			intc_am3359_raise_signal(opaque, line);
			break;
		case INTC_ISR_CLEAR(0):
		case INTC_ISR_CLEAR(1):
		case INTC_ISR_CLEAR(2):
		case INTC_ISR_CLEAR(3):
			index = (offset - 0x90) / 0x20;
			regs->isr_clear[index] = val;
			/* FIX ME: writes 1, clear soft interrupt bits */
			break;
		default:
			if(offset >= INTC_ILR(0) && offset <= INTC_ILR(127)){
				index = (offset - 0x100) / 0x4;
				regs->ilr[index] = val;
				break;
			}else{
				skyeye_log(Warning_log, __FUNCTION__, "Can not write the register at 0x%x in %s\n", offset, __FILE__);
				return Invarg_exp;
			}
	}
	return No_exp;
}
static void init_regs(intc_am3359_device *dev)
{
	memset(&dev->regs, 0, sizeof(dev->regs));

	dev->regs.revision	= 0x00000050;
	dev->regs.sir_irq	= 0xffffff80;
	dev->regs.sir_fiq	= 0xffffff80;
	dev->regs.irq_priority	= 0xffffffc0;
	dev->regs.fiq_priority	= 0xffffffc0;
	dev->regs.threshold	= 0x000000ff;

	memset(dev->regs.mir, 0xff, sizeof(dev->regs.mir));
}
static conf_object_t* new_intc_am3359(char* obj_name)
{
	intc_am3359_device *dev = skyeye_mm_zero(sizeof(intc_am3359_device));

	dev->obj = new_conf_object(obj_name, dev);
	init_regs(dev);

	return dev->obj;
}
static exception_t reset_intc_am3359(conf_object_t* opaque, const char* args)
{
	intc_am3359_device *dev = opaque->obj;
	init_regs(dev);

	return No_exp;
}
static exception_t free_intc_am3359(conf_object_t* opaque) {
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}
static uint32_t intc_am3359_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id) {
	intc_am3359_device *dev = conf_obj->obj;
	struct intc_am3359_reg *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static char* intc_am3359_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	intc_am3359_device *dev = conf_obj->obj;
	return regs_name[id];
}
static exception_t intc_am3359_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	intc_am3359_device *dev = conf_obj->obj;
	struct intc_am3359_reg *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;

	return No_exp;
}
static uint32_t intc_am3359_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	intc_am3359_device *dev = conf_obj->obj;
	struct intc_am3359_reg *regs = &(dev->regs);
	return  sizeof(struct intc_am3359_reg) / 4;
}
static uint32_t intc_am3359_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	intc_am3359_device *dev = conf_obj->obj;
	struct intc_am3359_reg *regs = &(dev->regs);
	uint32_t regnum;
	regnum = sizeof(struct intc_am3359_reg) / 4;
	int i;

	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	intc_am3359_device *dev = obj->obj;
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
	intc_am3359_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}
void init_intc_am3359()
{
	static skyeye_class_t class_data = {
		.class_name	= "am3359_intc",
		.class_desc 	= "am3359_intc",
		.new_instance	= new_intc_am3359,
		.free_instance	= free_intc_am3359,
		.reset_instance	= reset_intc_am3359,
		.set_attr	= NULL,
		.get_attr	= NULL,
		.module_type = SKYML_OFFCHIP,
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read	= intc_am3359_read,
		.write	= intc_am3359_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = intc_am3359_get_regvalue_by_id,
		.get_regname_by_id = intc_am3359_get_regname_by_id,
		.set_regvalue_by_id = intc_am3359_set_regvalue_by_id,
		.get_regnum = intc_am3359_get_regnum,
		.get_regid_by_name = intc_am3359_get_regid_by_name,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const general_signal_intf signal_intf = {
		.raise_signal = intc_am3359_raise_signal,
		.lower_signal = intc_am3359_lower_signal,
	};
	SKY_register_iface(clss, GENERAL_SIGNAL_INTF_NAME, &signal_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = GENERAL_SIGNAL_INTF_NAME,
			.iface = &signal_intf,
		},
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
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}
