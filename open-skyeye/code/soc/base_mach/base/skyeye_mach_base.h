#ifndef __SKYEYE_MACH_BASE_H__
#define __SKYEYE_MACH_BASE_H__

#include <skyeye_types.h>
#include <skyeye_mach.h>
#include <skyeye_addr_space.h>

typedef struct skyeye_base_mach {
	conf_object_t* obj;
	common_machine_attr_t attr;
	addr_space_t* space;
	conf_object_t* current_core;
	skyeye_machine_intf* mach_iface;
}skyeye_base_mach_t;

#endif
