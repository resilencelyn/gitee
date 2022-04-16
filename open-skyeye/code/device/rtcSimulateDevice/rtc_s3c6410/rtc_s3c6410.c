/* Copyright (C) 
* 2012 - xq2537@gmail.com
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
* @file rtc_s3c6410.c
* @brief The implementation of system controller
* @author xq2537@gmail.com
* @version 78.77
* @date 2012-1-8
*/

#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_attribute.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h> 
#include <memory_space.h>
#include <skyeye_device.h>
#define DEBUG
#include <skyeye_log.h>

#include "rtc_s3c6410.h"

static exception_t s3c6410_rtc_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct s3c6410_rtc_device *dev = opaque->obj;
	rtc_reg_t* regs = dev->regs;
	switch(offset) {
		case 0x30:
			*(uint32_t*)buf = regs->rtc_intp;
			break;
		case 0x40:
			*(uint32_t*)buf = regs->rtc_rtccon;
			break;
		case 0x44:
			*(uint32_t*)buf = regs->rtc_ticcnt;
			break;
		case 0x50:
			*(uint32_t*)buf = regs->rtc_rtcalm;
			break;
		case 0x54:
			*(uint32_t*)buf = regs->rtc_almsec;
			break;
		case 0x58:
			*(uint32_t*)buf = regs->rtc_almmin;
			break;
		case 0x5c:
			*(uint32_t*)buf = regs->rtc_almhour;
			break;

		case 0x60:
			*(uint32_t*)buf = regs->rtc_almdate;
			break;
		case 0x64:
			*(uint32_t*)buf = regs->rtc_almmon;
			break;
		case 0x68:
			*(uint32_t*)buf = regs->rtc_almyear;
			break;
		case 0x70:
			*(uint32_t*)buf = regs->rtc_bcdsec;
			break;
		case 0x74:
			*(uint32_t*)buf = regs->rtc_bcdmin;
			break;
		case 0x78:
			*(uint32_t*)buf = regs->rtc_bcdhour;
			break;
		case 0x7c:
			*(uint32_t*)buf = regs->rtc_bcddate;
			break;
		case 0x80:
			*(uint32_t*)buf = regs->rtc_bcdday;
			break;
		case 0x84:
			*(uint32_t*)buf = regs->rtc_bcdmon;
			break;
		case 0x88:
			*(uint32_t*)buf = regs->rtc_bcdyear;
			break;
		case 0x90:
			*(uint32_t*)buf = regs->rtc_curticcnt;
			break;
		default:
			printf("Can not read the register at 0x%x in rtc\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

static exception_t s3c6410_rtc_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct s3c6410_rtc_device *dev = opaque->obj;
	rtc_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t*)buf;
	switch(offset) {
		case 0x30:
			regs->rtc_intp = val;
			break;
		case 0x40:
			regs->rtc_rtccon = val;
			break;
		case 0x44:
			regs->rtc_ticcnt = val;
			break;
		case 0x50:
			regs->rtc_rtcalm = val;
			break;
		case 0x54:
			regs->rtc_almsec = val;
			break;
		case 0x58:
			regs->rtc_almmin = val;
			break;
		case 0x5c:
			regs->rtc_almhour = val;
			break;

		case 0x60:
			regs->rtc_almdate = val;
			break;
		case 0x64:
			regs->rtc_almmon = val;
			break;
		case 0x68:
			regs->rtc_almyear = val;
			break;
		case 0x70:
			regs->rtc_bcdsec = val;
			break;
		case 0x74:
			regs->rtc_bcdmin = val;
			break;
		case 0x78:
			regs->rtc_bcdhour = val;
			break;
		case 0x7c:
			regs->rtc_bcddate = val;
			break;
		case 0x80:
			regs->rtc_bcdday = val;
			break;
		case 0x84:
			regs->rtc_bcdmon = val;
			break;
		case 0x88:
			regs->rtc_bcdyear = val;
			break;
		case 0x90:
			regs->rtc_curticcnt = val;
			break;

		default:
			printf("Can not write the register at 0x%x in rtc\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

char* s3c6410_rtc_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	s3c6410_rtc_device *dev = conf_obj->obj;
	return dev->regs_name[id];
}

uint32_t s3c6410_rtc_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	s3c6410_rtc_device *dev = conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)(dev->regs) + id;
	return *regs_value;
}

static char* s3c6410_rtc_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	s3c6410_rtc_device *dev = conf_obj->obj;
	rtc_reg_t *regs = (dev->regs);
	*((uint32_t*)regs + id) = value;

	return NULL;
}
static uint32_t s3c6410_rtc_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	s3c6410_rtc_device *dev = conf_obj->obj;
	rtc_reg_t *regs = (dev->regs);
	return  sizeof(rtc_reg_t) / 4;


}

static uint32_t s3c6410_rtc_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	s3c6410_rtc_device *dev = conf_obj->obj;
	rtc_reg_t *regs = (dev->regs);
	uint32_t regnum;
	regnum = sizeof(rtc_reg_t)/ 4;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;


}


static conf_object_t* new_s3c6410_rtc(char* obj_name){
	s3c6410_rtc_device* dev = skyeye_mm_zero(sizeof(s3c6410_rtc_device));
	rtc_reg_t* regs =  skyeye_mm_zero(sizeof(rtc_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init rtc regs */
	dev->regs = regs;
	dev->regs_name = &regs_name;
	return dev->obj;

}
void free_s3c6410_rtc(conf_object_t* conf_obj){
	s3c6410_rtc_device *dev = conf_obj->obj;
    skyeye_free(dev->regs);
    skyeye_free(dev);
}

void s3c6410_rtc_register_attribute(conf_class_t* clss){
}


void init_s3c6410_rtc(){
	static skyeye_class_t class_data = {
		.class_name = "s3c6410_rtc",
		.class_desc = "s3c6410 rtc",
		.new_instance = new_s3c6410_rtc,
		.free_instance = free_s3c6410_rtc,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_s3c6410),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = s3c6410_rtc_read,
		.write = s3c6410_rtc_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);	

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = s3c6410_rtc_get_regvalue_by_id,
		.get_regname_by_id = s3c6410_rtc_get_regname_by_id,
		.set_regvalue_by_id = s3c6410_rtc_set_regvalue_by_id,
		.get_regnum = s3c6410_rtc_get_regnum,
		.get_regid_by_name = s3c6410_rtc_get_regid_by_name,
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
	static const struct ConnectDescription connects[] = {
	
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	s3c6410_rtc_register_attribute(clss);
}
