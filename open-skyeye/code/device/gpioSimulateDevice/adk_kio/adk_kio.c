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
* @file adk_kio.c
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
#include <skyeye_data_intf.h>
#define DEBUG
#include <skyeye_log.h>
#include <skyeye_uart.h>
#include <skyeye_emif.h>

#include "adk_kio.h"

static exception_t adk_kio_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct adk_kio_device *dev = opaque->obj;
	adk_kio_reg_t* regs = dev->regs;

	switch(offset) {
	    case REG_KICTRL:
		    *(uint32_t *)buf = regs->ki_ctrl;
		    break;
        case REG_KOCTRL:
		    *(uint32_t *)buf = regs->ko_ctrl;
		    break;
	    case ADDR_DA_DATA:
		    *(uint32_t *)buf = regs->ad_data;
		    break;
	    case REG_DACTRL:
		    *(uint32_t *)buf = regs->da_ctrl;
		    break;
        case REG_DATA_CTRL:
		    *(uint32_t *)buf = regs->data_ctrl;
		    break;
	    case REG_KOCTRL1:
		    *(uint32_t *)buf = regs->ko_ctrl1;
		    break;
	    case REG_KOCTRL2:
		    *(uint32_t *)buf = regs->ko_ctrl2;
		    break;
	    default:
		    skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in adk_kio\n", offset);
		    return Invarg_exp;
	}

	return No_exp;
}

static exception_t adk_kio_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct adk_kio_device *dev = opaque->obj;
	adk_kio_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t*)buf;
    int da_channel, da_offset;

    if (offset >= ADDR_DA_DATA && offset < REG_DACTRL){
        da_channel = (offset - ADDR_DA_DATA) / 8;
        da_offset = (offset - ADDR_DA_DATA) % 8;
        if (da_offset == 0){
            dev->da_data[da_channel].da_lsb = val; 
        }else {
            dev->da_data[da_channel].da_msb = val; 
        }
        return No_exp;
    }
	switch(offset) {
	    case REG_KICTRL:
		    regs->ki_ctrl = val;
		    break;
         case REG_KOCTRL:
		    regs->ko_ctrl = val;
		    break;
	    case REG_DACTRL:
		    regs->da_ctrl = val;
		    break;
	    case REG_DATA_CTRL:
		    regs->data_ctrl = val;
		    break;
	    case REG_KOCTRL1:
		    regs->ko_ctrl1 = val;
		    break;
	    case REG_KOCTRL2:
		    regs->ko_ctrl2 = val;
		    break;
	default:
		skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in adk_kio\n", offset);
		return Invarg_exp;
	}
	return No_exp;
}

static char* adk_kio_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}

static uint32_t adk_kio_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	adk_kio_device *dev = conf_obj->obj;
	adk_kio_reg_t* regs = dev->regs;
	uint32_t* regs_value = (uint32_t*)regs + id;

	return *regs_value;
}

static char* adk_kio_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	adk_kio_device *dev = conf_obj->obj;
	adk_kio_reg_t* regs = dev->regs;
	*((uint32_t*)regs + id) = value;

	return NULL;
}

static uint32_t adk_kio_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	adk_kio_device *dev = conf_obj->obj;
	adk_kio_reg_t* regs = dev->regs;

	return  sizeof(adk_kio_reg_t) / 4;
}

static uint32_t adk_kio_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_offset[id];
}

static uint32_t adk_kio_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	adk_kio_device *dev = conf_obj->obj;
	adk_kio_reg_t* regs = dev->regs;
	uint32_t regnum;
	regnum = sizeof(adk_kio_reg_t)/ 4;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}

	return regnum;
}

static conf_object_t* new_adk_kio(char* obj_name){
	adk_kio_device* dev = skyeye_mm_zero(sizeof(adk_kio_device));
	adk_kio_reg_t* regs =  skyeye_mm_zero(sizeof(adk_kio_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init adk_kio regs */
	dev->regs = regs;
	dev->regs->ko_ctrl = 0;

	return dev->obj;
}

static void free_adk_kio(conf_object_t* conf_obj){
	adk_kio_device *dev = conf_obj->obj;
    skyeye_free(dev->regs);
    skyeye_free(dev->obj);
    skyeye_free(dev);
	return;	
}

static adk_kio_register_attribute(conf_class_t* clss){
}

void init_adk_kio(){
	static skyeye_class_t class_data = {
		.class_name = "adk_kio",
		.class_desc = "adk_kio",
		.new_instance = new_adk_kio,
		.free_instance = free_adk_kio,
		.get_attr = NULL,
		.set_attr = NULL 
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = adk_kio_read,
		.write = adk_kio_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = adk_kio_get_regvalue_by_id,
		.get_regname_by_id = adk_kio_get_regname_by_id,
		.set_regvalue_by_id = adk_kio_set_regvalue_by_id,
		.get_regnum = adk_kio_get_regnum,
		.get_regid_by_name = adk_kio_get_regid_by_name,
		.get_regoffset_by_id = adk_kio_get_regoffset_by_id,
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
	
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	adk_kio_register_attribute(clss);
}
