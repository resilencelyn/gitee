/*
	skyeye_mach_am3359.c - define machine AM3359 for skyeye
	Copyright (C) 2010 Skyeye Develop Group
	for help please send mail to <skyeye-developer@lists.gro.clinux.org>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
/**
* @file skyeye_mach_am3359.c
* @date 2015-11-02
*/


#include <skyeye_config.h>
#include <skyeye_core.h>
#include <skyeye_sched.h>
#include <skyeye_lock.h>
#include <skyeye_class.h>
#include <skyeye_addr_space.h>
#include <skyeye_android_intf.h>
#include "skyeye_internal.h"
#include "skyeye_checkpoint.h"
#include <skyeye_interface.h>
#include <skyeye_lcd_intf.h>
#include <skyeye_keypad_intf.h>
#include <skyeye_io.h>
#include <skyeye_log.h>
#include <skyeye_uart.h>
#include <skyeye_mm.h>
#include <skyeye_signal.h>
#include <skyeye_attr.h>
#include <skyeye_attribute.h>
#include <skyeye_iface.h>
#include <skyeye_core_intf.h>
#include <skyeye_system.h>

#ifdef __CYGWIN__
#include <time.h>
#endif

typedef struct am3359_mach{
	conf_object_t* obj;
	common_machine_attr_t attr;
	addr_space_t* space;
	conf_object_t* core;
	conf_object_t *signal;
	general_signal_intf  *signal_iface;
}am3359_mach_t;


int am3359_update_boot_info(conf_object_t *obj)
{
	int i;
	am3359_mach_t *mach = obj->obj;
	sys_soc_t *soc = obj->sys_struct;

	for(i = 0; i < soc->cpu_cnt;i++){
		sys_cpu_t *cpu = soc->cpus + i;
		//skyeye_reg_intf* reg_iface = (skyeye_reg_intf*)SKY_get_iface(cpu->cpu, SKYEYE_REG_INTF);
		/* The environment for boot linux */
		//reg_iface->set_regvalue_by_id(cpu->cpu, 0, 0);
		/* machine ID for am3359 */
		//reg_iface->set_regvalue_by_id(cpu->cpu, 1, 0x2c);
		/* The atag or dtb address for linux booting */
		//reg_iface->set_regvalue_by_id(cpu->cpu, 2, 0x85000000);
	}
	return 0;
}

static conf_object_t* new_am3359_mach(char* obj_name){
	am3359_mach_t* mach = skyeye_mm_zero(sizeof(am3359_mach_t));
	mach->obj = new_conf_object(obj_name, mach);
	mach->attr.core_num = 0;
	return  mach->obj;
}

static void am3359_register_attribute(conf_class_t* clss){
}

static void am3359_register_interface(conf_class_t* clss){
	static const skyeye_machine_intf inface = {
		.update_boot_info = am3359_update_boot_info
	};
	SKY_register_iface(clss, MACHINE_INTF_NAME, &inface);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MACHINE_INTF_NAME,
			.iface = &inface,
		},
	};
	class_register_ifaces(clss, ifaces);
	return;
}
static void free_am3359_mach(conf_object_t *mach)
{
	am3359_mach_t *am3359_mach = mach->obj;
	skyeye_free(am3359_mach->obj);
	skyeye_free(am3359_mach);
	return;
}
void init_am3359_mach(){
	static skyeye_class_t class_data = {
		.class_name = "am3359",
		.class_desc = "am3359 machine",
		.new_instance = new_am3359_mach,
		.free_instance = free_am3359_mach,
		.get_attr = NULL,
		.set_attr = NULL
	};
	conf_class_t* class = SKY_register_soc_class(class_data.class_name, &class_data);
	am3359_register_attribute(class);
	am3359_register_interface(class);
}
