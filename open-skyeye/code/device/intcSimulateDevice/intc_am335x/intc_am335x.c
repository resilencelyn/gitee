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
* @file intc_am335x.c
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

#include "intc_am335x.h"
#define INTERRUTP	1
#define UINTERRUTP	0

void reset_intc_am335x(conf_object_t* opaque, const char* parameter);
#if 0
static char* intc_attr[] = {"cpu"};
static exception_t intc_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	struct intc_am335x_device *dev = opaque->obj;
        int index;
        for(index = 0; index < (sizeof(intc_attr)/sizeof(intc_attr[0])); index++){
                if(!strncmp(attr_name, intc_attr[index], strlen(intc_attr[index])))
                        break;
        }
        switch(index){
                case 0:
                        dev->core_signal = (core_signal_intf*)SKY_get_interface(value->u.object, CORE_SIGNAL_INTF_NAME);
			printf("in %s, dev->core_signal = 0x%x\n", __FUNCTION__, dev->core_signal);
                        break;
                default:
                        printf("parameter error\n");
                        return Invarg_exp;
        }
        return No_exp;
}
#endif
static int intc_am335x_raise_signal(conf_object_t *opaque, int line){
	struct intc_am335x_device *dev = opaque->obj;

	/* core interface should be set before */
	core_signal_intf* core_signal = dev->core_signal;
	//intc_am335x_reg_t* regs = dev->regs;
	uint32_t index = line / REG_SIZE;
	uint32_t reg_off = line % REG_SIZE;
	//printf("line:%d\n", line);
	/* 
	 * The current incoming interrupt status before masking is readable
	 * from the MPU_INTC.INTC_ITRn register.
	 */
	dev->regs.itr[index] |= reg_off;
	/* Judge the interrupt source is unmasked */
	if(((dev->regs.mir[index] >> reg_off) & 0x1)){
		return Not_found_exp;
	}
	/* Judge the priority threshold */
	//uint16_t threshold = (dev->regs.ilr[index] >> 1) & 0x3f;
	//if(threshold >= dev->regs.threshold){
	//	return Not_found_exp;
	//}

	interrupt_signal_t interrupt_signal;
	/*
	 * update sir_irq or sir_fiq 's bit ACTIVE
	 * before priority sorting is done, the interrupt status is readable
	 * from the PENDING_IRQn and PENDING_FIQn registers.
	 */
	if((dev->regs.ilr[line] & 0x1) == 0x0){
		/* IRQ */
		//printf("line:%d\n", line);
		if(dev->int_status == INTERRUTP){
			dev->regs.pending_irq[index] |= (0x1 << reg_off);
			dev->irq_pendinglist[dev->plist_tail++] = line;
			dev->current_pending_irq = 1;
			return No_exp;
		}
		dev->regs.sir_irq &= ~0x7f;
		dev->regs.sir_irq |= line;
		interrupt_signal.arm_signal.irq =  Low_level;
		interrupt_signal.arm_signal.firq = High_level;
		/* Clear the irq pending */
		dev->regs.pending_irq[index] = 0;
	}else{
		/* FIQ */
		if(dev->int_status == INTERRUTP){
			dev->regs.pending_irq[index] |= (0x1 << reg_off);
			dev->current_pending_irq = line;
			return No_exp;
		}
		interrupt_signal.arm_signal.irq = High_level;
		interrupt_signal.arm_signal.firq =  Low_level;
		dev->regs.sir_fiq &= ~0x7f;
		dev->regs.sir_fiq |= line;
		/* Clear the fiq pending */
		dev->regs.pending_fiq[index] = 0;
	}
	interrupt_signal.arm_signal.reset =  Prev_level;
	/* We don't need the mechanism of priority sorting temporarily */
	//conf_object_t* core = SIM_get_processor(0);
	core_signal->signal(dev->signal_obj, &interrupt_signal);
	dev->int_status = INTERRUTP;
	return No_exp;
}

static int intc_am335x_lower_signal(conf_object_t *opaque, int line)
{
	struct intc_am335x_device *dev = opaque->obj;
	core_signal_intf* core_signal = dev->core_signal;
	//intc_am335x_reg_t* regs = dev->regs;
	interrupt_signal_t interrupt_signal;

	if((dev->regs.ilr[line] & 0x1) == 0x0){
		/* IRQ */
		interrupt_signal.arm_signal.irq = High_level;
		interrupt_signal.arm_signal.firq = Prev_level;
		dev->regs.sir_irq &= ~0x7f;
	}else{
		/* FIQ */
		interrupt_signal.arm_signal.firq = High_level;
		interrupt_signal.arm_signal.irq = Prev_level;
		dev->regs.sir_fiq &= ~0x7f;
	}
	/*
	interrupt_signal_t interrupt_signal;
	interrupt_signal.arm_signal.irq =  High_level;
	interrupt_signal.arm_signal.firq = High_level;
	interrupt_signal.arm_signal.reset =  Prev_level;
	*/
	core_signal->signal(dev->signal_obj, &interrupt_signal);
	dev->int_status = UINTERRUTP;

	return No_exp;
}

static exception_t intc_am335x_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct intc_am335x_device *dev = opaque->obj;
	//intc_am335x_reg_t* regs = dev->regs;
	uint32_t index = 0;
	switch(offset) {
		case INTC_REVISION:
			*(uint32_t*)buf = dev->regs.revision;
			break;
		case INTC_SYSCONFIG:
			*(uint32_t*)buf = dev->regs.sysconfig;
			break;
		case INTC_SYSSTATUS:
			*(uint32_t*)buf = dev->regs.sysstatus;
			break;
		case INTC_SIR_IRQ: 
			*(uint32_t*)buf = dev->regs.sir_irq;
			//printf("read sir_irq:%d\n", *(char *)buf);
			break;
		case INTC_SIR_FIQ:
			*(uint32_t*)buf = dev->regs.sir_fiq;
			break;
		case INTC_CONTROL:
			//printf("In %s:%d register is write only\n", __func__, offset);
			*(uint32_t*)buf = dev->regs.control;
			break;
		case INTC_PROTECTION:
			*(uint32_t*)buf = dev->regs.protection;
			break;
		case INTC_IDLE: 
			*(uint32_t*)buf = dev->regs.idle;
			break;
		case INTC_IRQ_PRIORITY:
			*(uint32_t*)buf = dev->regs.irq_priority;
			break;
		case INTC_FIQ_PRIORITY:
			*(uint32_t*)buf = dev->regs.fiq_priority;
			break;
		case INTC_THRESHOLD: 
			*(uint32_t*)buf = dev->regs.threshold;
			break;
		case INTC_ITR(0):
		case INTC_ITR(1):
		case INTC_ITR(2):
		case INTC_ITR(3):
			index = (offset - 0x80) / 0x20;
			*(uint32_t*)buf = dev->regs.itr[index];
			break;
		case INTC_MIR(0):
		case INTC_MIR(1):
		case INTC_MIR(2):
		case INTC_MIR(3):
			index = (offset - 0x84) / 0x20;
			*(uint32_t*)buf = dev->regs.mir[index];
			break;
		case INTC_MIR_CLEAR(0):
		case INTC_MIR_CLEAR(1):
		case INTC_MIR_CLEAR(2):
		case INTC_MIR_CLEAR(3):
			/* reads return 0 */ 
			*(uint32_t*)buf = 0;
			break;
		case INTC_MIR_SET(0):
		case INTC_MIR_SET(1):
		case INTC_MIR_SET(2):
		case INTC_MIR_SET(3):
			/* reads return 0 */ 
			*(uint32_t*)buf = 0;
			break;
		case INTC_ISR_CLEAR(0):
		case INTC_ISR_CLEAR(1):
		case INTC_ISR_CLEAR(2):
		case INTC_ISR_CLEAR(3):
			/* reads return 0 */ 
			*(uint32_t*)buf = 0;
			break;
		case INTC_PENDING_IRQ(0):
		case INTC_PENDING_IRQ(1):
		case INTC_PENDING_IRQ(2):
		case INTC_PENDING_IRQ(3):
			index = (offset - 0x98) / 0x20;
			*(uint32_t*)buf = dev->regs.pending_irq[index];
			break;
		case INTC_PENDING_FIQ(0):
		case INTC_PENDING_FIQ(1):
		case INTC_PENDING_FIQ(2):
		case INTC_PENDING_FIQ(3):
			index = (offset - 0x9c) / 0x20;
			*(uint32_t*)buf = dev->regs.pending_fiq[index];
			break;
		default:
			if(offset >= INTC_ILR(0) && offset <= INTC_ILR(127)){
				index = (offset - 0x100) / 0x4;
				*(uint32_t*)buf = dev->regs.ilr[index];
				break;
			}else{

				printf("Can not read the register at 0x%x in intc_am335x\n", offset);
				return Invarg_exp;
			}
	}
	return No_exp;

}

static exception_t intc_am335x_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct intc_am335x_device *dev = opaque->obj;
	//intc_am335x_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t*)buf;
	uint32_t index = 0;
	switch(offset) {
		case INTC_REVISION:
			printf("In %s:%d register is read only\n", __func__, offset);
			break;
		case INTC_SYSCONFIG:
			if((*buf) & 0x2){
				reset_intc_am335x(opaque, NULL);
				dev->regs.sysstatus |= 0x1;
			}
			if((*buf) & 0x1)
				printf("Automatic OCP clock gating strategy is applied\n");
			break;
		case INTC_SYSSTATUS:
			printf("In %s:%x register is read only\n", __func__, offset);
			break;
		case INTC_SIR_IRQ: 
			dev->regs.sir_irq &= ~(*buf);
			break;
		case INTC_SIR_FIQ:
			dev->regs.sir_fiq &= ~(*buf);
			break;
		case INTC_CONTROL:
			if(*buf == 0x1){
				intc_am335x_lower_signal(opaque, dev->regs.sir_irq);
				if(dev->current_pending_irq >= 0){
					//dev->regs.ilr[dev->pending_fiq] &= ~0x1;
					intc_am335x_raise_signal(opaque, dev->irq_pendinglist[dev->plist_head++]);
					if(dev->plist_head == dev->plist_tail){
						dev->plist_tail = 0;
						dev->plist_head = 0;
						dev->current_pending_irq = -1;
					}
				}
			}
			if(*buf == 0x2){
				intc_am335x_lower_signal(opaque, dev->regs.sir_fiq);
				if(dev->current_pending_fiq >= 0){
					//dev->regs.ilr[dev->pending_fiq] |= 0x1;
					intc_am335x_raise_signal(opaque, dev->current_pending_fiq);
					dev->current_pending_fiq = -1;
				}
			}
			break;
		case INTC_PROTECTION:
			dev->regs.protection = *buf;
			break;
		case INTC_IDLE: 
			dev->regs.idle = *buf;
			break;
		case INTC_IRQ_PRIORITY:
			printf("In %s:%x register is read only\n", __func__, offset);
			break;
		case INTC_FIQ_PRIORITY:
			printf("In %s:%x register is read only\n", __func__, offset);
			break;
		case INTC_THRESHOLD:
			dev->regs.threshold = *buf;
			break;
		case INTC_ITR(0):
		case INTC_ITR(1):
		case INTC_ITR(2):
		case INTC_ITR(3):
			printf("In %s:%x register is read only\n", __func__, offset);
			break;
		case INTC_MIR(0):
		case INTC_MIR(1):
		case INTC_MIR(2):
		case INTC_MIR(3):
			index = (offset - 0x84) / 0x20;
			dev->regs.mir[index]= *buf;
			break;
		case INTC_MIR_CLEAR(0):
		case INTC_MIR_CLEAR(1):
		case INTC_MIR_CLEAR(2):
		case INTC_MIR_CLEAR(3):
			index = (offset - 0x88) / 0x20;
			/* Write 1 clears the mask bit to 0 */
			dev->regs.mir[index] &= ~(*buf);
			break;
		case INTC_MIR_SET(0):
		case INTC_MIR_SET(1):
		case INTC_MIR_SET(2):
		case INTC_MIR_SET(3):
			printf("set mir %x\n", offset);
			index = (offset - 0x8c) / 0x20;
			/* Write 1 sets the mask bit to 1 */
			dev->regs.mir[index] |= (*buf);
			break;
		case INTC_ISR_SET(0):
		case INTC_ISR_SET(1):
		case INTC_ISR_SET(2):
		case INTC_ISR_SET(3):
			index = (offset - 0x90) / 0x20;
			dev->regs.isr_set[index] = *buf;
			/* Writes 1, soft interrupt */
			break;
		case INTC_ISR_CLEAR(0):
		case INTC_ISR_CLEAR(1):
		case INTC_ISR_CLEAR(2):
		case INTC_ISR_CLEAR(3):
			index = (offset - 0x90) / 0x20;
			dev->regs.isr_clear[index] = *buf;
			/* Writes 1, clear soft interrupt */
			break;
		case INTC_PENDING_IRQ(0):
		case INTC_PENDING_IRQ(1):
		case INTC_PENDING_IRQ(2):
		case INTC_PENDING_IRQ(3):
		case INTC_PENDING_FIQ(0): 
		case INTC_PENDING_FIQ(1):
		case INTC_PENDING_FIQ(2):
		case INTC_PENDING_FIQ(3):
			break;
		default:
			if(offset >= INTC_ILR(0) && offset <= INTC_ILR(127)){
				index = (offset - 0x100) / 0x4;
				dev->regs.ilr[index] = *buf;
				break;
			}else{
				printf("Can not write the register at 0x%x in %s\n", offset, __func__);
				return Invarg_exp;
			}
	}
	return No_exp;

}

static conf_object_t* new_intc_am335x(char* obj_name){
	intc_am335x_device* dev = skyeye_mm_zero(sizeof(intc_am335x_device));
	//intc_am335x_reg_t* regs =  skyeye_mm_zero(sizeof(intc_am335x_reg_t));
	intc_am335x_reg_t regs = dev->regs;
	dev->obj = new_conf_object(obj_name, dev);
	//init_obj_with_class(dev->obj, &clss);
	dev->int_status = UINTERRUTP;
	memset(dev->irq_pendinglist, 0, sizeof(dev->irq_pendinglist));
	dev->plist_head = 0;
	dev->plist_tail = 0;
	dev->current_pending_irq = -1;
	dev->current_pending_fiq = -1;
	/* init intc_am335x regs */
	//dev->regs = regs;
#if 0
	/* Register io function to the object */
	memory_space_intf* io_memory = skyeye_mm_zero(sizeof(memory_space_intf));
	io_memory->conf_obj = dev->obj;
	io_memory->read = intc_am335x_read;
	io_memory->write = intc_am335x_write;
	SKY_register_interface(io_memory, obj_name, MEMORY_SPACE_INTF_NAME);

	general_signal_intf* signal = skyeye_mm_zero(sizeof(general_signal_intf));
	signal->conf_obj = dev->obj;
	signal->raise_signal = intc_am335x_raise_signal;
	signal->lower_signal = intc_am335x_lower_signal;
	SKY_register_interface(signal, obj_name, GENERAL_SIGNAL_INTF_NAME);
#endif
	//core_signal_intf* core_signal = skyeye_mm_zero(sizeof(core_signal_intf));
	//core_signal->obj = NULL;
	//core_signal->signal = NULL;

	return dev->obj;
}

void intc_am335x_register_interface(conf_class_t* clss) {
	static const memory_space_iface_t mem_iface = {
		.read = intc_am335x_read,
		.write = intc_am335x_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &mem_iface);

	static const general_signal_iface_t signal_iface = {
		.raise_signal = intc_am335x_raise_signal,
		.lower_signal = intc_am335x_lower_signal
	};
	SKY_register_iface(clss, GENERAL_SIGNAL_INTF_NAME, &signal_iface);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &mem_iface,
		},
		(struct InterfaceDescription) {
			.name = GENERAL_SIGNAL_INTF_NAME,
			.iface = &signal_iface,
		},
 	};
	class_register_ifaces(clss, ifaces);
	return;
}

void reset_intc_am335x(conf_object_t* opaque, const char* parameter)
{
	struct intc_am335x_device *dev = opaque->obj;
	intc_am335x_reg_t regs = dev->regs;
	//memset(regs, 0, sizeof(regs));
	memset(dev->irq_pendinglist, 0, sizeof(dev->irq_pendinglist));
	dev->plist_head = 0;
	dev->plist_tail = 0;
	dev->regs.revision = 0x50;
	dev->int_status = UINTERRUTP;
	dev->current_pending_irq = -1;
	dev->current_pending_fiq = -1;
	return;
}

void free_intc_am335x(conf_object_t* dev){
	
}

#define REGISTER_ATTR_GET_FUNC_DEF(name) \
	attr_value_t name##_get_attr(void* arg, conf_object_t* obj, attr_value_t* idx) \
	{\
		uint32_t data; \
		intc_am335x_device* dev = (intc_am335x_device*)(obj->obj); \
		data = dev->regs.name; \
		attr_value_t value = SKY_make_attr_integer(data); \
						\
		return value; \
	}

#define REGISTER_ATTR_SET_FUNC_DEF(name) \
	exception_t name##_set_attr(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx) { \
		intc_am335x_device* dev = (intc_am335x_device*)(obj->obj); \
		uint32_t data = SKY_attr_integer(*value);\
		dev->regs.name = data; \
								\
		return No_exp;	\
	}

#define REGISTER_ARR_ATTR_GET_FUNC_DEF(name, num) \
	attr_value_t name##_get_attr(void* arg, conf_object_t* obj, attr_value_t* idx) { \
		attr_value_t value = SKY_alloc_attr_list(num); \
		attr_value_t item; \
		intc_am335x_device* dev = obj->obj;	\
		uint32_t i;	\
		for (i = 0; i < num; i++) { \
			uint32_t data = dev->regs.name[i]; \
			item = SKY_make_attr_integer(data); \
			SKY_attr_list_set_item(&value, i, item); \
		} \
			\
		return value; \
	}

#define REGISTER_ARR_ATTR_SET_FUNC_DEF(name, num) \
	exception_t name##_set_attr(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx) {	\
		intc_am335x_device* dev = obj->obj; \
		uint32_t i = 0;	\
		uint32_t data = 0; \
		attr_value_t item; \
		for (i = 0; i < num; i++) { \
			item = SKY_attr_list_item(*value, i); \
			data = SKY_attr_integer(item); \
			dev->regs.name[i] = data; \
		} \
			\
		return No_exp; \
	}

#define COMMON_ATTR_GET_FUNC_DEF(name) \
	attr_value_t name##_get_attr(void* arg, conf_object_t* obj, attr_value_t* idx) \
	{\
		uint32_t data; \
		intc_am335x_device* dev = (intc_am335x_device*)(obj->obj); \
		data = dev->name; \
		attr_value_t value = SKY_make_attr_integer(data); \
						\
		return value; \
	}

#define COMMON_ATTR_SET_FUNC_DEF(name) \
	exception_t name##_set_attr(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx) { \
		intc_am335x_device* dev = (intc_am335x_device*)(obj->obj); \
		uint32_t data = SKY_attr_integer(*value);\
		dev->name = data; \
								\
		return No_exp;	\
	}

#define COMMON_ARR_ATTR_GET_FUNC_DEF(name, num) \
	attr_value_t name##_get_attr(void* arg, conf_object_t* obj, attr_value_t* idx) { \
		attr_value_t value = SKY_alloc_attr_list(num); \
		attr_value_t item; \
		intc_am335x_device* dev = obj->obj;	\
		uint32_t i;	\
		for (i = 0; i < num; i++) { \
			uint32_t data = dev->name[i]; \
			item = SKY_make_attr_integer(data); \
			SKY_attr_list_set_item(&value, i, item); \
		} \
			\
		return value; \
	}

#define COMMON_ARR_ATTR_SET_FUNC_DEF(name, num) \
	exception_t name##_set_attr(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx) {	\
		intc_am335x_device* dev = obj->obj; \
		uint32_t i = 0;	\
		uint32_t data = 0; \
		attr_value_t item; \
		for (i = 0; i < num; i++) { \
			item = SKY_attr_list_item(*value, i); \
			data = SKY_attr_integer(item); \
			dev->name[i] = data; \
		} \
			\
		return No_exp; \
	}

REGISTER_ATTR_GET_FUNC_DEF(revision)
//REGISTER_ATTR_SET_FUNC_DEF(revision)
REGISTER_ATTR_GET_FUNC_DEF(sysconfig)
REGISTER_ATTR_SET_FUNC_DEF(sysconfig)
REGISTER_ATTR_GET_FUNC_DEF(sysstatus)
REGISTER_ATTR_SET_FUNC_DEF(sysstatus)
REGISTER_ATTR_GET_FUNC_DEF(sir_irq)
REGISTER_ATTR_SET_FUNC_DEF(sir_irq)
REGISTER_ATTR_GET_FUNC_DEF(sir_fiq)
REGISTER_ATTR_SET_FUNC_DEF(sir_fiq)
REGISTER_ATTR_GET_FUNC_DEF(control)
REGISTER_ATTR_SET_FUNC_DEF(control)
REGISTER_ATTR_GET_FUNC_DEF(protection)
REGISTER_ATTR_SET_FUNC_DEF(protection)
REGISTER_ATTR_GET_FUNC_DEF(idle)
REGISTER_ATTR_SET_FUNC_DEF(idle)
REGISTER_ATTR_GET_FUNC_DEF(irq_priority)
REGISTER_ATTR_SET_FUNC_DEF(irq_priority)
REGISTER_ATTR_GET_FUNC_DEF(fiq_priority)
REGISTER_ATTR_SET_FUNC_DEF(fiq_priority)
REGISTER_ATTR_GET_FUNC_DEF(threshold)
REGISTER_ATTR_SET_FUNC_DEF(threshold)
REGISTER_ARR_ATTR_GET_FUNC_DEF(itr, 4)
REGISTER_ARR_ATTR_SET_FUNC_DEF(itr, 4)
REGISTER_ARR_ATTR_GET_FUNC_DEF(mir, 4)
REGISTER_ARR_ATTR_SET_FUNC_DEF(mir, 4)
REGISTER_ARR_ATTR_GET_FUNC_DEF(mir_clear, 4)
REGISTER_ARR_ATTR_SET_FUNC_DEF(mir_clear, 4)
REGISTER_ARR_ATTR_GET_FUNC_DEF(mir_set, 4)
REGISTER_ARR_ATTR_SET_FUNC_DEF(mir_set, 4)
REGISTER_ARR_ATTR_GET_FUNC_DEF(isr_set, 4)
REGISTER_ARR_ATTR_SET_FUNC_DEF(isr_set, 4)
REGISTER_ARR_ATTR_GET_FUNC_DEF(isr_clear, 4)
REGISTER_ARR_ATTR_SET_FUNC_DEF(isr_clear, 4)
REGISTER_ARR_ATTR_GET_FUNC_DEF(pending_irq, 4)
REGISTER_ARR_ATTR_SET_FUNC_DEF(pending_irq, 4)
REGISTER_ARR_ATTR_GET_FUNC_DEF(pending_fiq, 4)
REGISTER_ARR_ATTR_SET_FUNC_DEF(pending_fiq, 4)
REGISTER_ARR_ATTR_GET_FUNC_DEF(ilr, 128)
REGISTER_ARR_ATTR_SET_FUNC_DEF(ilr, 128)

COMMON_ATTR_GET_FUNC_DEF(int_status)
COMMON_ATTR_SET_FUNC_DEF(int_status)
COMMON_ATTR_GET_FUNC_DEF(current_pending_irq)
COMMON_ATTR_SET_FUNC_DEF(current_pending_irq)
COMMON_ATTR_GET_FUNC_DEF(current_pending_fiq)
COMMON_ATTR_SET_FUNC_DEF(current_pending_fiq)
COMMON_ARR_ATTR_GET_FUNC_DEF(irq_pendinglist, 128)
COMMON_ARR_ATTR_SET_FUNC_DEF(irq_pendinglist, 128)
COMMON_ATTR_GET_FUNC_DEF(plist_head)
COMMON_ATTR_SET_FUNC_DEF(plist_head)
COMMON_ATTR_GET_FUNC_DEF(plist_tail)
COMMON_ATTR_SET_FUNC_DEF(plist_tail)



#define REGISTER_READ_ONLY_ATTRIBUTE(name, type, desc) \
	SKY_register_attribute(clss, #name, name##_get_attr, NULL, NULL, NULL, SKY_Attr_Optional, #type, desc)
#define REGISTER_ATTRIBUTE(name, type, desc) \
	SKY_register_attribute(clss, #name, name##_get_attr, NULL, name##_set_attr, NULL, SKY_Attr_Optional, #type, desc)

static attr_value_t get_attr_cpu(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	intc_am335x_device *dev = conf_obj->obj;
	attr_value_t signal = SKY_make_attr_object(dev->signal_obj);
	return signal;
}
static exception_t set_attr_cpu(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	intc_am335x_device *dev = conf_obj->obj;
	dev->signal_obj = SKY_attr_object(*value);
	dev->core_signal = (core_signal_intf *)SKY_get_iface(dev->signal_obj, CORE_SIGNAL_INTF_NAME);
	return No_exp;
}

void intc_am335x_register_attribute(conf_class_t* clss) {
	SKY_register_attribute(clss, "cpu", get_attr_cpu, NULL, set_attr_cpu, NULL, SKY_Attr_Optional, "object", " interrupt controler  connected");
	//init_class_attribute_list(clss);
	//REGISTER_ATTRIBUTE(revision, i, NULL);
	REGISTER_READ_ONLY_ATTRIBUTE(revision, i, NULL);
	REGISTER_ATTRIBUTE(sysconfig, i, NULL);
	REGISTER_ATTRIBUTE(sysstatus, i, NULL);
	REGISTER_ATTRIBUTE(sir_irq, i, NULL);
	REGISTER_ATTRIBUTE(sir_fiq, i, NULL);
	REGISTER_ATTRIBUTE(control, i, NULL);
	REGISTER_ATTRIBUTE(protection, i, NULL);
	REGISTER_ATTRIBUTE(idle, i, NULL);
	REGISTER_ATTRIBUTE(irq_priority, i, NULL);
	REGISTER_ATTRIBUTE(fiq_priority, i, NULL);
	REGISTER_ATTRIBUTE(threshold, i, NULL);
	REGISTER_ATTRIBUTE(itr, [i{4}], NULL);
	REGISTER_ATTRIBUTE(mir, [i{4}], NULL);
	REGISTER_ATTRIBUTE(mir_clear, [i{4}], NULL);
	REGISTER_ATTRIBUTE(mir_set, [i{4}], NULL);
	REGISTER_ATTRIBUTE(isr_set, [i{4}], NULL);
	REGISTER_ATTRIBUTE(isr_clear, [i{4}], NULL);
	REGISTER_ATTRIBUTE(pending_irq, [i{4}], NULL);
	REGISTER_ATTRIBUTE(pending_fiq, [i{4}], NULL);
	REGISTER_ATTRIBUTE(ilr, [i{4}], NULL);
	REGISTER_ATTRIBUTE(int_status, i, NULL);
	REGISTER_ATTRIBUTE(current_pending_irq, i, NULL);
	REGISTER_ATTRIBUTE(current_pending_fiq, i, NULL);
	REGISTER_ATTRIBUTE(irq_pendinglist, [i{128}], NULL);
	REGISTER_ATTRIBUTE(plist_head, i, NULL);
	REGISTER_ATTRIBUTE(plist_tail, i, NULL);
	return;
}

void init_intc_am335x() {
	static skyeye_class_t class_data = {
		.class_name = "am335x_intc",
		.class_desc = "am335x_intc",
		.new_instance = new_intc_am335x,
		.free_instance = free_intc_am335x,
		.reset_instance = reset_intc_am335x,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_am335x SC_am3359),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	intc_am335x_register_attribute(clss);
	intc_am335x_register_interface(clss);
	//SKY_print_attribute(clss);
	//SKY_print_class_iface(clss);
}
