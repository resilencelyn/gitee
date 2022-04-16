/* Copyright (C) 
* 2015 - Michael.Kang blackfin.kang@gmail.com
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
* @file skyeye_mach_pc.cc
* @brief 
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2015-01-15
*/

#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_mach.h>
#include <skyeye_class.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_internal.h>
#include <skyeye_addr_space.h>
#define DEBUG
#include <skyeye_log.h>
typedef struct skyeye_mach_pc_device{
	conf_object_t* obj;
	common_machine_attr_t attr;
}skyeye_mach_pc_device;


int skyeye_mach_pc_set_space(conf_object_t *obj, addr_space_t *space)
{
	skyeye_mach_pc_device* mach = (skyeye_mach_pc_device*)(obj->obj);
	mach->attr.space = space;
	return 0;
}

addr_space_t *skyeye_mach_pc_get_space(conf_object_t *obj)
{
	skyeye_mach_pc_device* mach = (skyeye_mach_pc_device *)(obj->obj);
	return  mach->attr.space;
}

int skyeye_mach_pc_set_core(conf_object_t *obj, conf_object_t *core)
{
	skyeye_mach_pc_device* mach = (skyeye_mach_pc_device *)(obj->obj);
	mach->attr.core[mach->attr.core_num++] = core;
	if(mach->attr.core_num >= MAX_CORE_NUM){
		skyeye_log(Error_log, __FUNCTION__, "This MACHINE supports up to 8-cores\n");
	}
	return 0;
}

conf_object_t *skyeye_mach_pc_get_core_by_id(conf_object_t *obj, int id){
	skyeye_mach_pc_device* mach = (skyeye_mach_pc_device *)(obj->obj);
	return mach->attr.core[id];
}


static conf_object_t* skyeye_mach_pc_new(char* obj_name)
{
	skyeye_mach_pc_device* mach = (skyeye_mach_pc_device*)skyeye_mm_zero(sizeof(skyeye_mach_pc_device));
	mach->obj = new_conf_object(obj_name, mach);
	mach->attr.core_num = 0;
	
	return mach->obj;
}
static void pc_register_interface(conf_class_t* clss) {
	static const skyeye_machine_intf mach_intf = {
		NULL,
		skyeye_mach_pc_set_space,
		skyeye_mach_pc_set_core,
		NULL,
		NULL,
		skyeye_mach_pc_get_core_by_id,
		skyeye_mach_pc_get_space
	};
	SKY_register_iface(clss, MACHINE_INTF_NAME, &mach_intf);
	return;
}

static exception_t skyeye_mach_pc_free(conf_object_t* dev)
{
	skyeye_mach_pc_device *mach = (skyeye_mach_pc_device *)(dev->obj);
	skyeye_free(mach->obj);
	skyeye_free(mach);
	return No_exp;
}

static exception_t skyeye_mach_pc_reset(conf_object_t* obj)
{
	return No_exp;
}

static exception_t skyeye_mach_pc_set_attr(conf_object_t* obj,
		const char* attr_name, attr_value_t* value)
{
	return No_exp;
}

static attr_value_t* skyeye_mach_pc_get_attr(const char* attr_name,
		conf_object_t* obj)
{
	return NULL;
}

void init_skyeye_mach_pc(){
	static skyeye_class_t class_data = {
		.class_name = "pc_mach",
		.class_desc = "skyeye_mach_pc",
		.new_instance = skyeye_mach_pc_new,
		.free_instance = skyeye_mach_pc_free,
		.reset_instance = skyeye_mach_pc_reset,
		.get_attr = skyeye_mach_pc_get_attr,
		.set_attr = skyeye_mach_pc_set_attr
	};
	conf_class_t* clss = SKY_register_soc_class(class_data.class_name, &class_data);
	pc_register_interface(clss);
}
