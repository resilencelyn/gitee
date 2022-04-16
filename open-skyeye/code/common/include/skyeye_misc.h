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
* @file skyeye_misc.h
* @brief 
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2015-07-01
*/

#ifndef __SKYEYE_MISC_H__
#define __SKYEYE_MISC_H__
#include "skyeye_list.h"
#include "skyeye_queue.h"
#include "skyeye_lock.h"
#include "mm_monitor.h"
exception_t file_copy(const char* filename_src, const char* filename_dst);

#ifdef __cplusplus
 extern "C" {
#endif

void display_all_support(void);


typedef struct{
	uint32_t type;
	uint64_t t;
	uint64_t uint_v;
	int64_t int_v;
	double 	 double_v;
}var_t;

typedef struct {
	uint32_t count;
	var_t *var_list;
}var_py_t;

typedef struct{
	uint32_t id;
	uint32_t type;
	char     *name;
}vm_t;

typedef struct {
	uint32_t count;
	vm_t *vm_list;
}vm_py_t;

#define VAR_CNT 1000
#define NAME_LEN 256
#define EXP_LEN 256
typedef struct vm_point{
	bool_t          active;
	MMDataType_t 	type;
	char 	        name[NAME_LEN];
	char 	        exp[EXP_LEN];
	uint32_t 	exp_cnt;
	bool_t          s;
	uint64_t 	cur_var;
	uint64_t 	int_var[VAR_CNT];
	uint32_t	var_head; 
	uint64_t 	ms_head;
	uint32_t 	vm_point_id;
	RWLOCK_T 	vm_lock;
	bool_t 		confirmed;
	unsigned 	var_addr; //useful only when global variables are monitored 
	addr_info_t 	addr_info;//useful only when global variables are monitored 
        LIST_ENTRY (vm_point) list_entry;
	conf_object_t *idle_device;
}vm_point_t;


typedef struct var_monitor{
	uint32_t (*get_vms)(conf_object_t *obj, vm_t *vms, uint32_t cnt);
	uint32_t (*get_vars)(conf_object_t *obj, uint32_t id, var_t *vars, uint32_t cnt);
	uint32_t (*insert_exp)(conf_object_t *obj, uint32_t id, char *exp);
	uint32_t (*remove_exp)(conf_object_t *obj, uint32_t id);
}var_monitor_intf;
#define VAR_MONITOR_INTF_NAME "var_monitor"

typedef struct idle_api_intf_s {
	double (*get_curr_time)(conf_object_t *obj);
} idle_api_intf_t;
#define IDLE_DEV_API_INTF_NAME "idle_api_intf"

#ifdef __cplusplus
}
#endif

#endif

