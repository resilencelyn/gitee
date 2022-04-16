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
 * @file intc_c6678.c
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

#include "intc_c6678.h"

static exception_t intc_c6678_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct intc_c6678_device *dev = opaque->obj;
	intc_c6678_reg_t* regs = dev->regs;

	switch(offset) {
		//evtflag0-4 is read only
		case 0x0:
			*(uint32_t *)buf = regs->evtflag[0];
			break;
		case 0x4:
			*(uint32_t *)buf = regs->evtflag[1];
			break;
		case 0x8:
			*(uint32_t *)buf = regs->evtflag[2];
			break;
		case 0xc:
			*(uint32_t *)buf = regs->evtflag[3];
			break;
			//evtset is write only
		case 0x20:
		case 0x24:
		case 0x28:
		case 0x2c:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in intc_c6678\n", offset);
			return Invarg_exp;
			//evtclr is write only
		case 0x40:
		case 0x44:
		case 0x48:
		case 0x4c:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in intc_c6678\n", offset);
			return Invarg_exp;
			//evtmask0 bit0-4 is read only
		case 0x80:
			*(uint32_t *)buf = regs->evtmask[0];
			break;
		case 0x84:
			*(uint32_t *)buf = regs->evtmask[1];
			break;
		case 0x88:
			*(uint32_t *)buf = regs->evtmask[2];
			break;
		case 0x8c:
			*(uint32_t *)buf = regs->evtmask[3];
			break;
			//mevtflag is readonly
		case 0xa0:
			*(uint32_t *)buf = regs->mevtflag[0];
			break;
		case 0xa4:
			*(uint32_t *)buf = regs->mevtflag[1];
			break;
		case 0xa8:
			*(uint32_t *)buf = regs->mevtflag[2];
			break;
		case 0xac:
			*(uint32_t *)buf = regs->mevtflag[3];
			break;
			//intmux1-3
		case 0x104:
			*(uint32_t *)buf = regs->intmux[0];
			break;
		case 0x108:
			*(uint32_t *)buf = regs->intmux[1];
			break;
		case 0x10c:
			*(uint32_t *)buf = regs->intmux[2];
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in intc_c6678\n", offset);
			return Invarg_exp;
	}
	//DBG("In %s, offset = %x buf = %x\n", __func__, offset, *(uint32_t *)buf);
	return No_exp;
}

static void intc_callback(conf_object_t *opaque,int int_number, int core_number)
{
	struct intc_c6678_device *dev = opaque->obj;
	core_signal_intf* core_signal = dev->core_signal[core_number];
	intc_c6678_reg_t* regs = dev->regs;

	/* trigger the interrupt */
	interrupt_signal_t interrupt_signal;
	int i =0;
	for(; i < 16; i++)
		interrupt_signal.c6k_signal.ifr[i] = Prev_level;
	interrupt_signal.c6k_signal.ifr[int_number] = High_level;
	DBG("in %s, int_number=0x%x\n", __FUNCTION__, int_number);
	//	skyeye_log(Error_log, __FUNCTION__, "In %s, core %d interrupt %d is triggered, .\n", __FUNCTION__, core_number,int_number);

	if (core_signal)
	{
		core_signal->signal(dev->core[core_number], &interrupt_signal);
	}

	return;
}

static exception_t intc_c6678_write(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct intc_c6678_device *dev = opaque->obj;
	intc_c6678_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t*)buf;

	//DBG("In %s, offset = %x buf = %x\n", __func__, offset, *(uint32_t *)buf);
	switch(offset) {
		//evtflag is read only
		case 0x0:
		case 0x4:
		case 0x8:
		case 0xc:
			skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in intc_c6678\n", offset);
			return Invarg_exp;
		case 0x20:
			regs->evtset[0] = val;
			regs->evtflag[0] |=val;
			break;
		case 0x24:
			regs->evtset[1] = val;
			regs->evtflag[1] |=val;
			break;
		case 0x28:
			regs->evtset[2] = val;
			regs->evtflag[2] |=val;
			break;
		case 0x2c:
			regs->evtset[3] = val;
			regs->evtflag[3] |=val;
			break;
		case 0x40:
			regs->evtclr[0] = val;
			regs->evtflag[0] &=~val;
			regs->mevtflag[0] &=~val;
			break;
		case 0x44:
			regs->evtclr[1] = val;
			regs->evtflag[1] &=~val;
			regs->mevtflag[1] &=~val;
			break;
		case 0x48:
			regs->evtclr[2] = val;
			regs->evtflag[2] &=~val;
			regs->mevtflag[2] &= ~val;
			break;
		case 0x4c:
			regs->evtclr[3] = val;
			regs->evtflag[3] &=~val;
			regs->mevtflag[3] &=~val;
			break;
		case 0x80:
			regs->evtmask[0] = val | 0xf;
			break;
		case 0x84:
			regs->evtmask[1] = val;
			break;
		case 0x88:
			regs->evtmask[2] = val;
			break;
		case 0x8c:
			regs->evtmask[3] = val;
			break;
			//mevtflag is read only
		case 0xa0:
		case 0xa4:
		case 0xa8:
		case 0xac:
			skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in intc_c6678\n", offset);
			return Invarg_exp;
		case 0x104:
			regs->intmux[0] = val & 0x7f7f7f7f;
			break;
		case 0x108:
			regs->intmux[1] = val & 0x7f7f7f7f;
			break;
		case 0x10c:
			regs->intmux[2] = val & 0x7f7f7f7f;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in intc_c6678\n", offset);
			return Invarg_exp;
	}
	/*
	//set mevtflag by evtmask and evtflag
	int i = 0;
	for( ; i < 4; i++)
	{
		int j = 0;
		for(; j < 32; j++)
		{
			if((regs->evtmask[i] >> j) & 0x1 == 0)
				regs->mevtflag[i] |= (((regs->evtflag[i] >> j) & 0x1) << j);
			else
				regs->mevtflag[i] &= (~(0x1 << j));
		}

		if(regs->mevtflag[i] != 0)
		{
			DBG("mevtflag%d is %x,this should check evt%d\n", regs->mevtflag[i], i, i);
			//check mevtflag 
			//clear bit using evtclr reg

			//interrupt_signal(opaque,i,0);//use core 0 do interrupt

		}

	}

	*/
	return No_exp;
}

static char* intc_c6678_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t intc_c6678_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	intc_c6678_device *dev = conf_obj->obj;
	intc_c6678_reg_t* regs = dev->regs;
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static exception_t intc_c6678_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	intc_c6678_device *dev = conf_obj->obj;
	intc_c6678_reg_t* regs = dev->regs;
	*((uint32_t*)regs + id) = value;

	return No_exp;
}

static uint32_t intc_c6678_get_regnum(conf_object_t* conf_obj)
{
	intc_c6678_device *dev = conf_obj->obj;
	intc_c6678_reg_t* regs = dev->regs;
	return  sizeof(intc_c6678_reg_t) / 4;
}

static uint32_t intc_c6678_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	intc_c6678_device *dev = conf_obj->obj;
	intc_c6678_reg_t* regs = dev->regs;
	uint32_t regnum;
	regnum = sizeof(intc_c6678_reg_t)/ 4;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static int event_selector(conf_object_t* opaque,int line)
{
	intc_c6678_device *dev = opaque->obj;

	int i = 0;
	for(; i < 3; i++)
	{
		int j = 0;
		for(; j < 4; j++)
		{
			if(line == ((dev->regs->intmux[i] >> (j*8)) & 0xff))
			{
				return 4+i*4+j;
			}
		}
	}
	return -1;
}

int interrupt_signal(conf_object_t* opaque, int line, void* args)
{
	/* called in another thread */
	intc_c6678_device *dev = opaque->obj;
	RW_WRLOCK(dev->lock);
	int int_number = -1;

	/*EVT4-127*/
	if (line >= 4 && line < 128)
	{
		int index = line / 32;
		int offset = line % 32;
		//transform event num to interrupt num

		if (((dev->regs->evtmask[index] >> offset) & 0x1) == 0)
		{
			dev->regs->mevtflag[index] |= (0x1 << offset);
			int_number = event_selector(opaque, index);
		} else
		{
			int_number = event_selector(opaque, line);
		}

	}
	else{
		// something wrong?
	}

	DBG("in %s, int_number = %d event_num = %d\n", __FUNCTION__, int_number, line);
	if(int_number != -1)
	{
		intc_callback(opaque, int_number, *(uint32_t *)args);
	}

	RW_UNLOCK(dev->lock);

	return No_exp;
}

static conf_object_t* new_intc_c6678(const char* obj_name)
{
	intc_c6678_device* dev = skyeye_mm_zero(sizeof(intc_c6678_device));
	intc_c6678_reg_t* regs =  skyeye_mm_zero(sizeof(intc_c6678_reg_t));
	dev->obj = new_conf_object(obj_name, dev);

	/* init intc_c6678 regs */
	dev->regs = regs;
	dev->regs->evtmask[0] = 0xF;
	dev->regs->intmux[0] = 0x07060504;  //7f7f7f7f
	dev->regs->intmux[1] = 0x0b0a0908;
	dev->regs->intmux[2] = 0x0f0e0d0c;

	RWLOCK_INIT(dev->lock);

	return dev->obj;
}

static exception_t free_intc_c6678(conf_object_t* conf_obj)
{
	intc_c6678_device* dev = conf_obj->obj;
	RWLOCK_DESTROY(dev->lock);
	skyeye_free(dev->regs);
	skyeye_free(dev);
	skyeye_free(conf_obj);
	return No_exp;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	intc_c6678_device *dev = obj->obj;
	dev->core[index] = obj2;
	dev->core_signal[index] = (core_signal_intf *)SKY_get_iface(dev->core[index], CORE_SIGNAL_INTF_NAME);

	return No_exp;
}

static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
	intc_c6678_device *dev = obj->obj;
	*obj2 = dev->core[index];
	*port = NULL;
	return No_exp;
}

void init_intc_c6678(){

	static skyeye_class_t class_data =
	{
		.class_name = "c6678_intc",
		.class_desc = "c6678_intc",
		.new_instance = new_intc_c6678,
		.free_instance = free_intc_c6678,
		.get_attr = NULL,
		.set_attr = NULL
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory =
	{
		.read = intc_c6678_read,
		.write = intc_c6678_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const general_signal_intf general_signal =
	{
		.event_signal = interrupt_signal,
	};
	SKY_register_iface(clss, GENERAL_SIGNAL_INTF_NAME, &general_signal);

	static const skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = intc_c6678_get_regvalue_by_id,
		.get_regname_by_id = intc_c6678_get_regname_by_id,
		.set_regvalue_by_id = intc_c6678_set_regvalue_by_id,
		.get_regnum = intc_c6678_get_regnum,
		.get_regid_by_name = intc_c6678_get_regid_by_name,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const struct InterfaceDescription ifaces[] =
	{
		(struct InterfaceDescription
		 ) {
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf,
		},
		(struct InterfaceDescription)
		{
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		},

		(struct InterfaceDescription)
		{
			.name = GENERAL_SIGNAL_INTF_NAME,
			.iface = &general_signal,
		}
	};
	static const struct ConnectDescription connects[] =
	{
		(struct ConnectDescription )
		{
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		},

	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}
