/**
* @file skyeye_addr_space.h
* @brief The addr_space class
* @author Michael.Kang blackfin.kang@gmail.com
* @version 0.1
* @date 2011-07-11
*/

#ifndef __SKYEYE_ADDR_SPACE_H__
#define __SKYEYE_ADDR_SPACE_H__

#include <skyeye_types.h>
#include <memory_space.h>
#include "breakpoint.h"
#include <skyeye_dev_intf.h>
#include <skyeye_attribute.h>
#include <skyeye_lock.h>
#include "mm_monitor.h"
#include "memory_monitor.h"
#define MAX_MAP 16
#ifdef __cplusplus
 extern "C" {
#endif

typedef struct map_info{
	conf_object_t* obj;
	uint64_t base_addr;
	generic_address_t length;
	generic_address_t start;
	memory_space_intf* memory_space;
	int priority;
	int swap_endian;
	RWLOCK_T lock;
}map_info_t;

typedef struct ram_array{
	map_info_t *map;
	memory_space_intf *iface;
}ram_array_t;

typedef struct addr_space{
	conf_object_t* obj;
	map_info_t** map_array;
	uint32_t map_count;
	ram_array_t **ram_map_array;
	uint32_t ram_map_count;
	memory_space_intf* memory_space;
	breakpoint_mgt_t bp_manager;
	conf_object_t *extra_core;
	memory_space_intf *extra_iface;
	conf_object_t *dma;
	dma_intf *dma_iface;
	pgd_t mm_monitor;
	mm_fast_map memory_monitor;
}addr_space_t;
#define ADDR_SPACE_INTF_NAME	"addr_space_interface"

typedef exception_t(*memory_space_set_plug_t)(conf_object_t* target, generic_address_t addr, int bitnum, int mode);
typedef exception_t(*memory_space_clear_plug_t)(conf_object_t* target, generic_address_t addr, int bitnum, int mode);
typedef void*(*memory_space_get_plug_t)(conf_object_t* target, generic_address_t addr);
typedef struct memory_space_set_plug{
	conf_object_t* conf_obj;
	memory_space_set_plug_t set_plug;
	memory_space_get_plug_t get_plug;
	memory_space_clear_plug_t clear_plug;
}memory_space_set_plug_intf;
#define MEMORY_SPACE_SET_PLUG_INTF_NAME   "memory_space_set_plug_interface"


typedef exception_t(*set_core_t)(conf_object_t* target, conf_object_t *core, memory_space_intf *iface);
typedef exception_t(*remove_core_t)(conf_object_t* target);
typedef struct memory_space_extern_core{
	set_core_t set_core;
	remove_core_t remove_core;
}memory_space_extra_core_intf;
#define MEMORY_SPACE_EXTRA_CORE_NAME   "memory_space_extra_core"


typedef exception_t(*add_map_t)(conf_object_t* target, conf_object_t *core, uint64_t base_addr, generic_address_t length);
typedef exception_t(*add_map_group_t)(conf_object_t* target, conf_object_t *core, uint64_t base_addr, generic_address_t length, uint32_t index);
typedef struct memory_space_add_map{
	add_map_t add_map;
	add_map_group_t add_map_group;
	addr_space_t *(*get_addr_space)(conf_object_t *obj);	
}memory_space_add_map_intf;
#define MEMORY_SPACE_ADD_MAP_NAME   "memory_space_add_map"



static inline addr_space_t* get_space_from_attr(conf_object_t* conf_obj){
	if (!SKY_has_attribute(conf_obj, "addr_space")){
		attr_value_t space_attr = SKY_get_attribute(conf_obj, "addr_space");
		return (addr_space_t*) SKY_attr_object(space_attr);
	}else
		return NULL;
}

addr_space_t* new_addr_space(char* obj_name);
void init_addr_space(char* obj_name, addr_space_t* space);
conf_object_t* new_addr_space_obj(char* obj_name);
void free_addr_space(char* obj_name);
get_page_t* space_get_page(addr_space_t *a, generic_address_t p);

exception_t SKY_add_map(addr_space_t* space, conf_object_t* device, generic_address_t base_addr, generic_address_t length, generic_address_t start, memory_space_intf* memory_space, int priority, int swap_endian);
exception_t add_map(addr_space_t* space, generic_address_t base_addr, generic_address_t length, generic_address_t start, memory_space_intf* memory_space, int priority, int swap_endian);

get_page_t *space_get_page(addr_space_t* space, generic_address_t addr);

exception_t space_obj_free(addr_space_t* addr_space);
exception_t space_obj_reset(addr_space_t* addr_space, const char* obj_name);

exception_t SKY_space_read(conf_object_t* obj, generic_address_t addr, void* buf, size_t count);
exception_t SKY_space_write(conf_object_t* obj, generic_address_t addr, void* buf, size_t count);

int initialize_addr_space(addr_space_t* space); 
#ifdef __cplusplus
}
#endif

#endif
