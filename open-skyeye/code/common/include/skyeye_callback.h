/*
        skyeye_callback.h - callback function definition for skyeye
        Copyright (C) 2003 Skyeye Develop Group
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
/*
 * 09/16/2009   Michael.Kang  <blackfin.kang@gmail.com>
 */

#ifndef __CALLBACK_H__
#define __CALLBACK_H__
#include "skyeye_arch.h"
typedef enum{
	Step_callback = 0, /* called when step running of core */
	Mem_read_callback, /* called when memory write */
	Mem_write_callback, /* called when memory read */
	Bus_read_callback, /* called when memory write */
	Bus_write_callback, /* called when memory read */
	Exception_callback, /* called when some exceptions are triggered. */
	Bootmach_callback, /* called when hard reset of machine */
	SIM_exit_callback, /* called when simulator exit */
	Max_callback
}callback_kind_t;
typedef void(*callback_func_t)(void* arg);
void register_callback(callback_func_t func, callback_kind_t kind);
int exec_callback(callback_kind_t kind, void* arg);


//new golbal callback API
 
typedef int hap_handle_t;
typedef void (*obj_hap_func_t)();


void SKY_hap_init(void);
void SKY_hap_free(void);

hap_handle_t 
SKY_hap_add_callback(const char *hap, obj_hap_func_t func, void *user_data);

hap_handle_t 
SKY_hap_add_callback_obj(const char *hap, conf_object_t *obj, 
		     obj_hap_func_t func, 
		     void *user_data);

void SKY_hap_delete_callback_id(const char *hap, hap_handle_t handle);

/*
 * SIM_Log_Message Callback Prototype 
 * */
typedef void (*sim_log_message_t)(void *callback_data, conf_object_t *trigger_obj, char *type, char *message);

/*
 * SIM_Log_Message Callback Trigger function 
 * */
void SKY_log_message_hap_occurred(void *callback_data, conf_object_t *trigger_obj, char *type, char *message);


/*
 * SIM_pc Callback Callback Prototype 
 * */
typedef struct {
    uint32_t startpc;
    int32_t cycle;
}sim_pc_data_t;

typedef void (*sim_pc_t)(void *callback_data, conf_object_t *trigger_obj, conf_object_t *user, uint32_t pc);
/*
 * SIM_pc_Message Callback Trigger function 
 * */
void SKY_pc_hap_occurred(void *callback_data, conf_object_t *trigger_obj, uint32_t pc);

/*
 * SIM_Before_Init Callback Prototype 
 * */
typedef void (*sim_before_init_t)(void *callback_data);

/*
 * SIM_Before_Init Callback Trigger function 
 * */
void SKY_before_init_hap_occurred();


#endif
