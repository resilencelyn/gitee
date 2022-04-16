#include <skyeye_class.h>
#include <skyeye_log.h>
#include <skyeye_mm.h>
#include <skyeye_iface.h>
#include <skyeye_attribute.h>
#include <skyeye_mach.h>

#include "skyeye_mach_base.h"

static conf_object_t* new_base_mach(char* obj_name) {
	skyeye_base_mach_t* mach = skyeye_mm_zero(sizeof(skyeye_base_mach_t));
	mach->obj = new_conf_object(obj_name, mach);
	mach->attr.core_num = 0;
	return mach->obj;
}

static void free_base_mach(conf_object_t* mach) {
	skyeye_base_mach_t* base_mach = mach->obj;
	skyeye_free(base_mach->obj);
	skyeye_free(base_mach);
}

static exception_t reset_base_mach(conf_object_t* mach) {
	skyeye_base_mach_t* base_mach = mach->obj;
	addr_space_t* phys_mem = base_mach->attr.space;
	space_obj_reset(phys_mem, NULL);
	exception_t ret = reset_conf_obj(base_mach->attr.core);
	return No_exp;
}

static int base_set_space(conf_object_t* obj, addr_space_t* space) {
	skyeye_base_mach_t* mach = obj->obj;
	mach->attr.space = space;
	return 0;
}

static int base_set_current_core(conf_object_t* obj, conf_object_t* core) {
	skyeye_base_mach_t* mach = obj->obj;
	mach->current_core = core;
	return 0;
}

static conf_object_t* base_get_current_core(conf_object_t* obj) {
	skyeye_base_mach_t* mach = obj->obj;
	assert(mach->current_core != NULL);
	return mach->current_core;
}

static int base_set_core(conf_object_t* obj, conf_object_t* core) {
	skyeye_base_mach_t* mach = obj->obj;
	mach->attr.core[mach->attr.core_num++] = core;
	if (mach->attr.core_num >= MAX_CORE_NUM) {
		SKY_log_error(obj, 1, "This machine support to 8-cores\n");
	}
	return 0;
}

static addr_space_t* base_get_space(conf_object_t* obj) {
	skyeye_base_mach_t* mach = (skyeye_base_mach_t*)(obj->obj);
	return  mach->attr.space;
}

static conf_object_t*base_get_core_by_id(conf_object_t* obj, int id) {
	skyeye_base_mach_t* mach = obj->obj;
	return mach->attr.core[id];
}

static void base_register_interface(conf_class_t* clss) {
	static const skyeye_machine_intf mach_intf = {
		.set_space = base_set_space,
		.set_core = base_set_core,
		.set_current_core = base_set_current_core,
		.get_current_core = base_get_current_core,
		.get_core_by_id = base_get_core_by_id,
		.get_space = base_get_space,
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

void init_base_mach() {
	static skyeye_class_t class_data = {
		.class_name = "base_mach",
		.class_desc = "base_machine",
		.new_instance = new_base_mach,
		.reset_instance = reset_base_mach,
		.free_instance = free_base_mach,
		.get_attr = NULL,
		.set_attr = NULL
	};

	conf_class_t* clss = SKY_register_soc_class(class_data.class_name, &class_data);
	base_register_interface(clss);

	return;
}

