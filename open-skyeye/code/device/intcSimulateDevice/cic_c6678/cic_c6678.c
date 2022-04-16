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
 * @file cic_c6678.c
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
//#define DEBUG
#include <skyeye_log.h>

#include "cic_c6678.h"

static exception_t cic_c6678_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct cic_c6678_device *dev = opaque->obj;
	cic_c6678_reg_t* regs = dev->regs;

	/* System Interrupt Status Raw/Set Registers */
	if (offset >= 0x200 && offset <= 0x27c)
	{
		int index = (offset - 0x200) / 4;
		*(uint32_t *)buf = regs->systemIntStRawOrSet[index];
		return No_exp;
	}

	/* System Interrupt Status Enabled/Clear Registers */
	if (offset >= 0x280 && offset <= 0x2fc)
	{
		int index = (offset - 0x280) / 4;
		*(uint32_t *)buf = regs->systemIntStEnableOrClr[index];
		DBG("in %s, read enabled status, index=%d\n, buf=0x%x\n", __FUNCTION__, index, *(uint32_t *)buf);
		return No_exp;
	}

	/* System Interrupt Enable Set Registers */
	if (offset >= 0x300 && offset <= 0x37c)
	{
		int index = (offset - 0x300) / 4;
		*(uint32_t *)buf = regs->systemIntEnableSet[index];
		return No_exp;
	}

	/* System Interrupt Enable Clear Registers */
	if (offset >= 0x380 && offset <= 0x3fc)
	{
		int index = (offset - 0x380) / 4;
		*(uint32_t *)buf = regs->systemIntEnableClr[index];
		return No_exp;
	}

	/* Channel Map Registers */
	if (offset >= 0x400 && offset <= 0x7fc)
	{
		int index = (offset - 0x400) / 4;
		*(uint32_t *)buf = regs->channelMap[index];
		return No_exp;
	}

	/* Host Interrupt Map Registers */
	if (offset >= 0x800 && offset <= 0x8fc)
	{
		int index = (offset - 0x800) / 4;
		*(uint32_t *)buf = regs->hostInterruptMap[index];
		return No_exp;
	}

	/* Host Interrupt Prioritized Index Registers */
	if (offset >= 0x900 && offset <= 0xcfc)
	{
		int index = (offset - 0x900) / 4;
		*(uint32_t *)buf = regs->hostInterruptPriIndex[index];
		return No_exp;
	}

	/* Host Interrupt Enable Registers */
	if (offset >= 0x1500 && offset <= 0x151c)
	{
		int index = (offset - 0x1500) / 4;
		*(uint32_t *)buf = regs->hostInterruptEnable[index];
		return No_exp;
	}

	switch(offset) {
		case 0x0000:
			*(uint32_t *)buf = regs->revision;
			break;
		case 0x0004:
			*(uint32_t *)buf = regs->ctrl;
			break;
		case 0x0010:
			*(uint32_t *)buf = regs->globalEnable;
			break;
		case 0x0020:
			*(uint32_t *)buf = regs->systemIntStIndexSet;
			break;
		case 0x0024:
			*(uint32_t *)buf = regs->systemIntStIndexClr;
			break;
		case 0x0028:
			*(uint32_t *)buf = regs->systemIntEnableIndexSet;
			break;
		case 0x002c:
			*(uint32_t *)buf = regs->systemIntEnableIndexClr;
			break;
		case 0x0034:
			*(uint32_t *)buf = regs->hostIntEnableIndexSet;
			break;
		case 0x0038:
			*(uint32_t *)buf = regs->hostIntEnableIndexClr;
			break;
		case 0x0080:
			*(uint32_t *)buf = regs->globalPriIndex;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in cic_c6678\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

static void cic_callback(conf_object_t *opaque,int int_number, int core_index)
{
	struct cic_c6678_device *dev = opaque->obj;
	cic_c6678_reg_t* regs = dev->regs;
	DBG("in %s, int_number=%d\n", __FUNCTION__, int_number);
	if(dev->general_signal.obj){
		dev->general_signal.iface->event_signal(dev->general_signal.obj, int_number, &core_index);
	}
	return;
}

static exception_t cic_c6678_write(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct cic_c6678_device *dev = opaque->obj;
	cic_c6678_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t*)buf;
	//DBG("In %s, offset = %x buf = %x\n", __func__, offset, *(uint32_t *)buf);

	/* System Interrupt Status Raw/Set Registers */
	if (offset >= 0x200 && offset <= 0x27c)
	{
		int index = (offset - 0x200) / 4;
		regs->systemIntStRawOrSet[index] = val;
		return No_exp;
	}

	/* System Interrupt Status Enabled/Clear Registers */
	if (offset >= 0x280 && offset <= 0x2fc)
	{
		int index = (offset - 0x280) / 4;
		/* w1c */
		regs->systemIntStEnableOrClr[index] &= ~val;
		DBG("in %s, clear interrupt by val=0x%x\n", __FUNCTION__, val);
		return No_exp;
	}

	/* System Interrupt Enable Set Registers */
	if (offset >= 0x300 && offset <= 0x37c)
	{
		int index = (offset - 0x300) / 4;
		regs->systemIntEnableSet[index] = val;
		return No_exp;
	}

	/* System Interrupt Enable Clear Registers */
	if (offset >= 0x380 && offset <= 0x3fc)
	{
		int index = (offset - 0x380) / 4;
		regs->systemIntEnableClr[index] = val;
		return No_exp;
	}

	/* Channel Map Registers */
	if (offset >= 0x400 && offset <= 0x7fc)
	{
		int index = (offset - 0x400) / 4;
		DBG("in %s ,channelMap index=%d, val=%d\n", __FUNCTION__, index, val);
		regs->channelMap[index] = val;
		return No_exp;
	}

	/* Host Interrupt Map Registers */
	if (offset >= 0x800 && offset <= 0x8fc)
	{
		int index = (offset - 0x800) / 4;
		regs->hostInterruptMap[index] = val;
		DBG("in %s ,host interrupt Map index=%d, val=%d\n", __FUNCTION__, index, val);
		return No_exp;
	}

	/* Host Interrupt Prioritized Index Registers */
	if (offset >= 0x900 && offset <= 0xcfc)
	{
		int index = (offset - 0x900) / 4;
		regs->hostInterruptPriIndex[index] = val;
		return No_exp;
	}

	/* Host Interrupt Enable Registers */
	if (offset >= 0x1500 && offset <= 0x151c)
	{
		int index = (offset - 0x1500) / 4;
		regs->hostInterruptEnable[index] = val;
		return No_exp;
	}

	switch(offset) {
		case 0x0000:
			regs->revision = val;
			break;
		case 0x0004:
			regs->ctrl = val;
			break;
		case 0x0010:
			regs->globalEnable = val;
			break;
		case 0x0020:
			regs->systemIntStIndexSet = val;
			break;
		case 0x0024:
			regs->systemIntStIndexClr = val;
			break;
		case 0x0028:
			regs->systemIntEnableIndexSet = val;
			break;
		case 0x002c:
			regs->systemIntEnableIndexClr = val;
			break;
		case 0x0034:
			regs->hostIntEnableIndexSet = val;
			break;
		case 0x0038:
			regs->hostIntEnableIndexClr = val;
			break;
		case 0x0080:
			regs->globalPriIndex = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in cic_c6678\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

static char* cic_c6678_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t cic_c6678_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	cic_c6678_device *dev = conf_obj->obj;
	cic_c6678_reg_t* regs = dev->regs;
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static exception_t cic_c6678_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	cic_c6678_device *dev = conf_obj->obj;
	cic_c6678_reg_t* regs = dev->regs;
	*((uint32_t*)regs + id) = value;

	return No_exp;
}

static uint32_t cic_c6678_get_regnum(conf_object_t* conf_obj)
{
	cic_c6678_device *dev = conf_obj->obj;
	cic_c6678_reg_t* regs = dev->regs;
	return  10;
}

static uint32_t cic_c6678_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	cic_c6678_device *dev = conf_obj->obj;
	cic_c6678_reg_t* regs = dev->regs;
	uint32_t regnum;
	regnum = sizeof(cic_c6678_reg_t)/ 4;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t cic_c6678_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

static int event_selector(conf_object_t* opaque,int line)
{
	cic_c6678_device *dev = opaque->obj;
	return 29;
}

int interrupt_signal(conf_object_t* opaque, int system_int, void* args)
{
	/* called in another thread */
	cic_c6678_device *dev = opaque->obj;
	RW_WRLOCK(dev->lock);

	//transform event num to interrupt num
	//int int_number = event_selector(opaque,line);
	int core_index = *(uint32_t *)args;
	int index = system_int / 32;
	int offset = system_int % 32;

	/* set the corresponding bit in intr status register */	
	DBG("in %s, system_int=%d, index=%d, offset=%d\n", __FUNCTION__, system_int, index, offset);
	dev->regs->systemIntStEnableOrClr[index] |= (1 << offset); 

	int channel_reg = system_int / 4 - 1;	
	int channel_offset = ((system_int - 1)/ 4);
	int ch_map = -1;
	if(channel_reg < MAX_CHANNEL_NUM){
		ch_map = 0xff & (dev->regs->channelMap[channel_reg] >> (channel_offset * 8)) ;
	}
	else{
		//something wrong , since max register number is 256
	}
	DBG("in %s,channel_reg=%d ch_map=%d, \n", __FUNCTION__, channel_reg, ch_map);
	int host_int = -1;
	if(ch_map < MAX_HOST_INT_MAP){
		int index = ch_map / 4;
		int offset = ch_map % 4;
		host_int = 0xff & (dev->regs->hostInterruptMap[index] >> (offset * 8));
	}
	else{
		//something wrong , since max register number is 64
	}
	DBG("in %s, ch_map=%d, host_int=%d\n", __FUNCTION__, ch_map, host_int);
	//int int_number = event_selector(opaque, system_int);	
	/* here we need to set */
	if(host_int != -1)
	{
		cic_callback(opaque, host_int, core_index);
	}

	RW_UNLOCK(dev->lock);

	return No_exp;
}

static conf_object_t* new_cic_c6678(const char* obj_name)
{
	cic_c6678_device* dev = skyeye_mm_zero(sizeof(cic_c6678_device));
	cic_c6678_reg_t* regs =  skyeye_mm_zero(sizeof(cic_c6678_reg_t));
	dev->obj = new_conf_object(obj_name, dev);

	/* init_cic_c6678 regs */
	dev->regs = regs;

	regs->revision = 0x4e820100;
	regs->ctrl = 0x10;
	RWLOCK_INIT(dev->lock);

	return dev->obj;
}

static exception_t free_cic_c6678(conf_object_t* conf_obj)
{
	cic_c6678_device* dev = conf_obj->obj;
	RWLOCK_DESTROY(dev->lock);
	skyeye_free(dev->regs);
	skyeye_free(dev);
	skyeye_free(conf_obj);
	return No_exp;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	cic_c6678_device *dev = obj->obj;
	if (obj2 == NULL) {
		skyeye_log(Error_log, __FUNCTION__, "Get %s interface from NULL object fail.\n", GENERAL_SIGNAL_INTF_NAME);
		return Not_found_exp;
	}
 
	dev->general_signal.obj = obj2;
	dev->general_signal.iface = (general_signal_iface_t *)SKY_get_iface(obj2, GENERAL_SIGNAL_INTF_NAME);
	if (dev->general_signal.iface == NULL) {
		skyeye_log(Error_log, __FUNCTION__, "Get %s interface from %s fail.\n", GENERAL_SIGNAL_INTF_NAME, obj2->objname);
		return Not_found_exp;
	}
	//DBG("in %s, dev->general_signal.iface=0x%x\n", __FUNCTION__, dev->general_signal.iface);
	return No_exp;
}

static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
	cic_c6678_device *dev = (cic_c6678_device *)obj->obj;
	*obj2 = dev->general_signal.obj;
	*port = NULL;
	return No_exp;
}

void init_cic_c6678(){

	static skyeye_class_t class_data =
	{
		.class_name = "c6678_cic",
		.class_desc = "c6678_cic",
		.new_instance = new_cic_c6678,
		.free_instance = free_cic_c6678,
		.get_attr = NULL,
		.set_attr = NULL
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory =
	{
		.read = cic_c6678_read,
		.write = cic_c6678_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const general_signal_intf general_signal =
	{
		.event_signal = interrupt_signal,
	};
	SKY_register_iface(clss, GENERAL_SIGNAL_INTF_NAME, &general_signal);

	static const skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = cic_c6678_get_regvalue_by_id,
		.get_regname_by_id = cic_c6678_get_regname_by_id,
		.set_regvalue_by_id = cic_c6678_set_regvalue_by_id,
		.get_regnum = cic_c6678_get_regnum,
		.get_regid_by_name = cic_c6678_get_regid_by_name,
		.get_regoffset_by_id = cic_c6678_get_regoffset_by_id,
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
		(struct ConnectDescription)
		{
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set  = general_signal_set,
			.get  = general_signal_get
		}

	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}
