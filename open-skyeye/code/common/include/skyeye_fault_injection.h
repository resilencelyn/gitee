#ifndef __SKYEYE_FAULT_INJECTION_H__
#define __SKYEYE_FAULT_INJECTION_H__

#include <skyeye_types.h>
#include <memory_space.h>
#include "breakpoint.h"
#include <skyeye_dev_intf.h>
#include <skyeye_attribute.h>
#ifdef __cplusplus
 extern "C" {
#endif

#define DEVICE_FI_NUM 20
typedef struct dev_finj_t{
	int count;
	unsigned int fi[DEVICE_FI_NUM][2];
}dev_fi_t;

typedef struct finj_t{
	int bitnum;
	int addr;
	int mode;
}fi_t;

typedef struct memory_space_plug{
	conf_object_t* obj;
	conf_object_t* dev_memory_space_obj;
 	memory_space_intf *memory_space;
	dev_fi_t fi_array;
	fi_t fi[DEVICE_FI_NUM];
}memory_space_plug_t;

typedef exception_t(*memory_space_save_plug_obj_t)(conf_object_t* target, conf_object_t *memory_space_obj, generic_address_t addr, int bitnum, int mode);
typedef int(*memory_space_clear_plug_obj_t)(conf_object_t* target, generic_address_t addr, int bitnum, int mode);
typedef void*(*memory_space_get_plug_obj_t)(conf_object_t* target);
typedef struct memory_space_plug_save{
	conf_object_t *conf_obj;
	memory_space_save_plug_obj_t save_plug_obj;
	memory_space_clear_plug_obj_t clear_plug_obj;
	memory_space_get_plug_obj_t get_plug_obj;
}memory_space_plug_save_intf;
#define MEMORY_SPACE_PLUG_INTF_NAME  "memory_space_plug_interface"
#define MEMORY_SPACE_PLUG_SAVE_INTF_NAME  "memory_space_plug_save_interface"

#ifdef __cplusplus
}
#endif

#endif
