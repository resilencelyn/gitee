/* Copyright (C) 
* 2011 - Michael.Kang blackfin.kang@gmail.com
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
* @file sysctrl_s3c6410.c
* @brief The implementation of system controller
* @author Michael.Kang blackfin.kang@gmail.com
* @version 78.77
* @date 2011-12-12
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
#include <skyeye_attribute.h>
#include <skyeye_device.h>
#define DEBUG
#include <skyeye_log.h>

#include "sysctrl_s3c6410.h"

static exception_t s3c6410_sysctrl_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct s3c6410_sysctrl_device *dev = opaque->obj;
	sysctrl_reg_t* regs = dev->regs;
	DBG("yukewei ######################### In %s, offset=0x%x\n", __FUNCTION__, offset);
	switch(offset) {
		case 0x0:
			*(uint32_t*)buf = regs->apll_lock;
			break;			
		case 0x4:
			*(uint32_t*)buf = regs->mpll_lock;
			break;
		case 0x8:
			*(uint32_t*)buf = regs->epll_lock;
			break; case 0xc:
			*(uint32_t*)buf = regs->apll_con;
			break;
		case 0x10:
			*(uint32_t*)buf = regs->mpll_con;
			break;
		case 0x14:
			*(uint32_t*)buf = regs->epll_con0;
			break;
		case 0x18:
			*(uint32_t*)buf = regs->epll_con1;
			break;
		case 0x1c:
			*(uint32_t*)buf = regs->clk_src;
			break;
		case 0x20:
			*(uint32_t*)buf = regs->clk_div0;
			break;
		case 0x24:
			*(uint32_t*)buf = regs->clk_div1;
			break;
		case 0x28:
			*(uint32_t*)buf = regs->clk_div2;
			break;
		case 0x30:
			*(uint32_t*)buf = regs->hclk_gate;
			break;
		case 0x34:
			*(uint32_t*)buf = regs->pclk_gate;
			break;
		case 0x38:
			*(uint32_t*)buf = regs->sclk_gate;
			break;

		case 0x804:
			*(uint32_t*)buf = regs->pwr_cfg; 
			break;
		case 0x110:
			*(uint32_t*)buf = regs->sdma_sel;
			break;
		case 0x118:	
			*(uint32_t*)buf = regs->sys_id;
			break;
		case 0x900:
			*(uint32_t*)buf = regs->others;
			break;
		default:
			printf("Can not read the register at 0x%x in sysctrl\n", offset);
			*(uint32_t*)buf = 0;
			return No_exp;
			//return Invarg_exp;
	}
	return No_exp;
}

static exception_t s3c6410_sysctrl_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct s3c6410_sysctrl_device *dev = opaque->obj;
	sysctrl_reg_t* regs = dev->regs;
	DBG("yukewei ######################### In %s, offset=0x%x\n", __FUNCTION__, offset);
	uint32_t val = *(uint32_t*)buf;
	switch(offset) {
		case 0x0:
			regs->apll_lock = val;
			break;			
		case 0x4:
			regs->mpll_lock = val;
			break;
		case 0x8:
			regs->epll_lock = val;
			break;
		case 0xc:
			regs->apll_con = val;
			break;
		case 0x10:
			regs->mpll_con = val;
			break;
		case 0x14:
			regs->epll_con0 = val;
			break;
		case 0x18:
			regs->epll_con1 = val;
			break;
		case 0x1c:
			regs->clk_src = val;
			break;
		case 0x20:
			regs->clk_div0 = val;
			break;
		case 0x24:
			regs->clk_div1 = val;
			break;
		case 0x28:
			regs->clk_div2 = val;
			break;
		case 0x30:
			regs->hclk_gate = val;
			break;
		case 0x34:
			regs->pclk_gate = val;
			break;
		case 0x38:
			regs->sclk_gate = val;
			break;

		case 0x804:
			regs->pwr_cfg = val; // 0x7E00_B000
			break;
		case 0x110:
			regs->sdma_sel = val;
			break;
		default:
			printf("Can not write the register at 0x%x in sysctrl\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}


char* s3c6410_sysctrl_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	        s3c6410_sysctrl_device *dev = conf_obj->obj;
	        return dev->regs_name[id];
}
	 

uint32_t s3c6410_sysctrl_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
	s3c6410_sysctrl_device *dev = conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)(dev->regs) + id;
	return *regs_value;
}

static char* s3c6410_sysctrl_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	s3c6410_sysctrl_device *dev = conf_obj->obj;
	sysctrl_reg_t *regs = (dev->regs);
	*((uint32_t*)regs + id) = value;

	return NULL;
}

static uint32_t s3c6410_sysctrl_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	s3c6410_sysctrl_device *dev = conf_obj->obj;
	sysctrl_reg_t *regs = (dev->regs);
	return  sizeof(sysctrl_reg_t) / 4;


}

static uint32_t s3c6410_sysctrl_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	s3c6410_sysctrl_device *dev = conf_obj->obj;
	sysctrl_reg_t *regs = (dev->regs);
	uint32_t regnum;
	regnum = sizeof(sysctrl_reg_t)/ 4;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;


}

static conf_object_t* new_s3c6410_sysctrl(char* obj_name){
	s3c6410_sysctrl_device* dev = skyeye_mm_zero(sizeof(s3c6410_sysctrl_device));
	sysctrl_reg_t* regs =  skyeye_mm_zero(sizeof(sysctrl_reg_t));
	dev->obj = new_conf_object(obj_name, dev);
	/* init sysctrl regs */
	regs->pwr_cfg = 0x1;
	regs->apll_lock = 0xFFFF;
	regs->mpll_lock = 0xFFFF;
	regs->epll_lock = 0xFFFF;
	regs->apll_con = 0x1900302;
	regs->mpll_con = 0x2140603;
	regs->epll_con0 = 0x00200102;
	regs->epll_con1 = 0x9111;
	regs->clk_div0 = 0x01051000;
	regs->hclk_gate = 0xFFFFFFFF;
	regs->pclk_gate = 0xFFFFFFFF;
	regs->sclk_gate = 0xFFFFFFFF;
	regs->ahb_con0 = 0x04000000;
	regs->sys_id = 0x36410101;
	regs->osc_freq = 0xd;
	regs->osc_stable = 0x1;
	regs->others = 0x801e;
	dev->regs = regs;
	dev->regs_name = &regs_name;

	return dev->obj;
}
void free_s3c6410_sysctrl(conf_object_t* conf_obj){
	s3c6410_sysctrl_device *dev = conf_obj->obj;
	skyeye_free(dev->regs);
	skyeye_free(dev);
}

void s3c6410_sysctrl_register_attribute(conf_class_t* clss){
}

void init_s3c6410_sysctrl(){
	static skyeye_class_t class_data = {
		.class_name = "s3c6410_sysctrl",
		.class_desc = "s3c6410 sysctrl",
		.new_instance = new_s3c6410_sysctrl,
		.free_instance = free_s3c6410_sysctrl,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_s3c6410),
	};
	conf_class_t *clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = s3c6410_sysctrl_read,
		.write = s3c6410_sysctrl_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);	

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = s3c6410_sysctrl_get_regvalue_by_id,
		.get_regname_by_id = s3c6410_sysctrl_get_regname_by_id,
		.set_regvalue_by_id = s3c6410_sysctrl_set_regvalue_by_id,
		.get_regnum = s3c6410_sysctrl_get_regnum,
		.get_regid_by_name = s3c6410_sysctrl_get_regid_by_name,
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

	s3c6410_sysctrl_register_attribute(clss);
}
