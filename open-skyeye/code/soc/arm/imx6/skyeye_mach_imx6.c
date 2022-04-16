/*
 * skyeye_mach_imx6.c:
 *
 * Copyright (C) 2014 Oubang Shen <shenoubang@gmail.com>
 * Skyeye Develop Group, for help please send mail to
 * <skyeye-developer@lists.gro.clinux.org>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
/**
* @file skyeye_mach_imx6.c
* @brief imx6 machine implemention
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2014-07-11
*/

#include <skyeye_class.h>
#include <skyeye_log.h>
#include <skyeye_mm.h>
#include <skyeye_iface.h>
#include <skyeye_attribute.h>
#include <skyeye_mach.h>

#include "imx6.h"

static conf_object_t* new_imx6_mach(char* obj_name) {
	imx6_mach_t* mach = skyeye_mm_zero(sizeof(imx6_mach_t));
	mach->obj = new_conf_object(obj_name, mach);
	mach->attr.core_num = 0;

	return mach->obj;
}

static void free_imx6_mach(conf_object_t* mach) {
	imx6_mach_t* imx6_mach = mach->obj;
	skyeye_free(imx6_mach->obj);
	skyeye_free(imx6_mach);
}

static exception_t reset_imx6_mach(conf_object_t* mach) {
	imx6_mach_t* imx6_mach = mach->obj;
	addr_space_t* phys_mem = imx6_mach->attr.space;
	space_obj_reset(phys_mem, NULL);
	exception_t ret = reset_conf_obj(imx6_mach->attr.core);
	return No_exp;
}

static int imx6_set_space(conf_object_t* obj, addr_space_t* space) {
	imx6_mach_t* mach = obj->obj;
	mach->attr.space = space;

	return 0;
}

static int imx6_set_current_core(conf_object_t* obj, conf_object_t* core) {
	imx6_mach_t* mach = obj->obj;
	mach->current_core = core;

	return 0;
}

static conf_object_t* imx6_get_current_core(conf_object_t* obj) {
	imx6_mach_t* mach = obj->obj;
	assert(mach->current_core != NULL);

	return mach->current_core;
}

static int imx6_set_core(conf_object_t* obj, conf_object_t* core) {
	imx6_mach_t* mach = obj->obj;
	mach->attr.core[mach->attr.core_num++] = core;
	if (mach->attr.core_num >= MAX_CORE_NUM) {
		SKY_log_error(obj, 1, "This machine support to 8-cores\n");
	}
	return 0;
}

static addr_space_t* imx6_get_space(conf_object_t* obj) {
	imx6_mach_t* mach = (imx6_mach_t*)(obj->obj);
	return  mach->attr.space;
}

static exception_t imx6_set_addr(conf_object_t* obj, conf_object_t* device, generic_address_t base_addr, generic_address_t len, generic_address_t start, memory_space_intf* mem_iface, int priority, int swap_endian) {

	return No_exp;
}

static conf_object_t*imx6_get_core_by_id(conf_object_t* obj, int id) {
	imx6_mach_t* mach = obj->obj;
	return mach->attr.core[id];
}


static void imx6_register_interface(conf_class_t* clss) {
	static const skyeye_machine_intf mach_intf = {
		.set_space = imx6_set_space,
		.set_core = imx6_set_core,
		.set_current_core = imx6_set_current_core,
		.get_current_core = imx6_get_current_core,
		.get_core_by_id = imx6_get_core_by_id,
		.get_space = imx6_get_space,
	};
	SKY_register_iface(clss, MACHINE_INTF_NAME, &mach_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MACHINE_INTF_NAME,
			.iface = &mach_intf,
		}
 	};
	class_register_ifaces(clss, ifaces);

	return;
}

void init_imx6_mach() {
	static skyeye_class_t class_data = {
		.class_name = "imx6",
		.class_desc = "imx6_machine",
		.new_instance = new_imx6_mach,
		.reset_instance = reset_imx6_mach,
		.free_instance = free_imx6_mach,
		.get_attr = NULL,
		.set_attr = NULL
	};

	conf_class_t* clss = SKY_register_soc_class(class_data.class_name, &class_data);
	imx6_register_interface(clss);

	return;
}

