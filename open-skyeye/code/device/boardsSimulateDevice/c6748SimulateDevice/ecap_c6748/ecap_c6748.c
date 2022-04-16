/* Copyright (C) 
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See th
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
* 
*/
/**
* @file ecap_c6748.c
* @brief The implementation of system controller
* @author Kewei Yu keweihk@gmail.com
* @version 78.77
*/

#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h> 
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_uart_ops.h>
#include <skyeye_iface.h>
#define DEBUG
#include <skyeye_log.h>

#include "ecap_c6748.h"

void ecap_interrupt_callback(ecap_c6748_device *dev){
	ecap_c6748_reg_t* regs = dev->regs;
	if (regs->ecctl2.bit.CAP_APWM){
		if (regs->ecctl2.bit.TSCTRSTOP){
			regs->tsctr.all ++;
			if (regs->tsctr.all == regs->cap4.all && regs->ecflg.bit.CTR_EQ_CMP == 0){ //CTR = CPM
				regs->ecflg.bit.CTR_EQ_CMP = 1;
				if (regs->eceint.bit.CTR_EQ_CMP){
					if (dev->signal_iface)
						dev->signal_iface->raise_signal(dev->signal, dev->int_number);	
				}	
			}
			if (regs->tsctr.all == regs->cap3.all && regs->ecflg.bit.CTR_EQ_PRD == 0){ //CTR = PDR
				regs->ecflg.bit.CTR_EQ_PRD = 1;
				regs->tsctr.all = 0;
				if (regs->eceint.bit.CTR_EQ_PRD){
					if (dev->signal_iface)
						dev->signal_iface->raise_signal(dev->signal, dev->int_number);	
				}	
			}
			if (regs->ecfrc.bit.CTR_EQ_CMP && regs->eceint.bit.CTR_EQ_CMP){
				if (dev->signal_iface)
					dev->signal_iface->raise_signal(dev->signal, dev->int_number);	
			}
			if (regs->ecfrc.bit.CTR_EQ_PRD && regs->eceint.bit.CTR_EQ_PRD){
				if (dev->signal_iface)
					dev->signal_iface->raise_signal(dev->signal, dev->int_number);	
			}
		}
	}else {
		if (regs->ecfrc.bit.CEVT1 && regs->eceint.bit.CEVT1){
			if (dev->signal_iface)
				dev->signal_iface->raise_signal(dev->signal, dev->int_number);	
		}
		if (regs->ecfrc.bit.CEVT2 && regs->eceint.bit.CEVT2){
			if (dev->signal_iface)
				dev->signal_iface->raise_signal(dev->signal, dev->int_number);	
		}
		if (regs->ecfrc.bit.CEVT3 && regs->eceint.bit.CEVT3){
			if (dev->signal_iface)
				dev->signal_iface->raise_signal(dev->signal, dev->int_number);	
		}
		if (regs->ecfrc.bit.CEVT4 && regs->eceint.bit.CEVT4){
			if (dev->signal_iface)
				dev->signal_iface->raise_signal(dev->signal, dev->int_number);	
		}
		if ((regs->ecfrc.bit.CTROVF || (regs->tsctr.all == OVER_FLOW))
		       		&& regs->eceint.bit.CTROVF){
			if (dev->signal_iface)
				dev->signal_iface->raise_signal(dev->signal, dev->int_number);	
		}
	}

	return ;
}
static exception_t ecap_c6748_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct ecap_c6748_device *dev = opaque->obj;
	ecap_c6748_reg_t* regs = dev->regs;
	switch(offset)
	{
		case TSCTR:
			*(uint32_t *)buf = regs->tsctr.all;
			break;
		case CTRPHS:
			*(uint32_t *)buf = regs->ctrphs.all;
			break;
		case CAP1:
			*(uint32_t *)buf = regs->cap1.all;
			break;
		case CAP2:
			*(uint32_t *)buf = regs->cap2.all;
			break;
		case CAP3:
			*(uint32_t *)buf = regs->cap3.all;
			break;
		case CAP4:
			*(uint32_t *)buf = regs->cap4.all;
			break;
		case ECCTL1:
			*(uint16_t *)buf = regs->ecctl1.all;
			break;
		case ECCTL2:
			*(uint16_t *)buf = regs->ecctl2.all;
			break;
		case ECEINT:
			*(uint16_t *)buf = regs->eceint.all;
			break;
		case ECFLG:
			*(uint16_t *)buf = regs->ecflg.all;
			break;
		case ECCLR:
			*(uint16_t *)buf = regs->ecclr.all;
			break;
		case ECFRC:
			*(uint16_t *)buf = regs->ecfrc.all;
			break;
        case REVID:
			*(uint32_t *)buf = regs->revid;
		default:
			printf("In %s, read error register offset %d\n", __FUNCTION__, offset);
			break;
	}

	return No_exp;
}

static exception_t ecap_c6748_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	ecap_c6748_device *dev = opaque->obj;
	ecap_c6748_reg_t* regs = dev->regs;
	uint32_t value = *(uint32_t *)buf;
	int i;
	switch(offset)
	{
		case TSCTR:
			regs->tsctr.all = value;
			break;
		case CTRPHS:
			regs->ctrphs.all = value;
			break;
		case CAP1:
			regs->cap1.all = value;
			if (regs->ecctl2.bit.CAP_APWM)
				regs->cap3.all = regs->cap1.all;
			//APWM mode Resets TSCTR on CTR = PRD event (period boundary
			// Permits shadow loading on CAP1 and 2 registers ;
			// Disables loading of time-stamps into CAP1-4 registers;
			// CAPx/APWMx pin operates as a APWM output)
			break;
		case CAP2:
			regs->cap2.all = value;
			if (regs->ecctl2.bit.CAP_APWM)
				regs->cap4.all = regs->cap2.all;
			break;
		case CAP3:
			regs->cap3.all = value;
			if (regs->ecctl2.bit.CAP_APWM)
				regs->cap1.all = regs->cap3.all;
			break;
		case CAP4:
			regs->cap4.all = value;
			if (regs->ecctl2.bit.CAP_APWM)
				regs->cap2.all = regs->cap4.all;
			break;
		case ECCTL1:
			regs->ecctl1.all = value;
			break;
		case ECCTL2:
			regs->ecctl2.all = value;
			if (regs->ecctl2.bit.REARM){
				dev->mod4_count = 0;
			}
			break;
		case ECEINT:
			regs->eceint.all = value;
			break;
		case ECFLG:
			regs->ecflg.all = value;
			break;
		case ECCLR:
			regs->ecclr.all = value;
			regs->ecflg.all &= ~value;
			break;
		case ECFRC:
			regs->ecfrc.all = value;
			break;
		default:
			printf("In %s, write error register offset %d\n", __FUNCTION__, offset);
			break;
	}
	return No_exp;
}
static char * ecap_c6748_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

uint32_t ecap_c6748_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id)
{
	ecap_c6748_device *dev= conf_obj->obj;
	ecap_c6748_reg_t* regs = dev->regs;
	uint16_t* regs_value = (uint16_t*)regs + id;
	return *regs_value;
}

static char* ecap_c6748_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	ecap_c6748_device *dev = conf_obj->obj;
	ecap_c6748_reg_t* regs = dev->regs;
	*((uint16_t*)regs + id) = value;

	return NULL;
}

static uint32_t ecap_c6748_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	ecap_c6748_device *dev = conf_obj->obj;
	ecap_c6748_reg_t* regs = dev->regs;
	//return  sizeof(ecap_c6748_reg_t) / 2;
	return 13;
}

static uint32_t ecap_c6748_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	ecap_c6748_device *dev = conf_obj->obj;
	ecap_c6748_reg_t* regs = dev->regs;
	uint32_t regnum;
	regnum = sizeof(ecap_c6748_reg_t)/ 2;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}

	return regnum;
}

static uint32_t ecap_c6748_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

static conf_object_t* new_ecap_c6748(char* obj_name){
	ecap_c6748_device* dev = skyeye_mm_zero(sizeof(ecap_c6748_device));
	dev->obj = new_conf_object(obj_name, dev);
	/* init ecap_c6748 regs */
	ecap_c6748_reg_t* regs =  skyeye_mm_zero(sizeof(ecap_c6748_reg_t));
	dev->regs = regs;
	dev->timer_sched_id = -1;
	dev->mod4_count = 0;
	dev->stop = 0;
	dev->sync_flag = 0;
	create_thread_scheduler((unsigned int)1000, Periodic_sched, ecap_interrupt_callback, dev, &dev->timer_sched_id);

	return dev->obj;
}

exception_t reset_ecap_c6748(conf_object_t* opaque, const char* parameters){
	
	return No_exp;
}

exception_t free_ecap_c6748(conf_object_t* opaque){
	ecap_c6748_device *dev = opaque->obj;
	skyeye_free(dev->obj);
	skyeye_free(dev->regs);
	skyeye_free(dev);

	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	ecap_c6748_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->int_number);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	ecap_c6748_device *dev = conf_obj->obj;
	dev->int_number = SKY_attr_uinteger(*value);
	return No_exp;
}
static ecap_c6748_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of ecap interrupt");
}

static exception_t set_attr_signal(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	ecap_c6748_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);

	return No_exp;
}
 
static exception_t get_attr_signal(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	ecap_c6748_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

void init_ecap_c6748(){
	static skyeye_class_t class_data = {
		.class_name = "c6748_ecap",
		.class_desc = "c6748_ecap",
		.new_instance = new_ecap_c6748,
		.free_instance = free_ecap_c6748,
		.reset_instance = reset_ecap_c6748,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_c6748),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = ecap_c6748_read,
		.write = ecap_c6748_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = ecap_c6748_get_regvalue_by_id,
		.get_regname_by_id = ecap_c6748_get_regname_by_id,
		.set_regvalue_by_id = ecap_c6748_set_regvalue_by_id,
		.get_regnum = ecap_c6748_get_regnum,
		.get_regid_by_name = ecap_c6748_get_regid_by_name,
		.get_regoffset_by_id = ecap_c6748_get_regoffset_by_id,
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
			.set = set_attr_signal,
			.get = get_attr_signal,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	ecap_c6748_register_attribute(clss);
}
