/* Copyright (C) 
* 2014 - Michael.Kang blackfin.kang@gmail.com
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
* @file skyeye_internal.h
* @brief some internal API
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2014-12-04
*/

#ifndef __SKYEYE_INTERNAL_H__
#define __SKYEYE_INTERNAL_H__
#include "skyeye_config.h"

#ifdef __cplusplus
 extern "C" {
#endif

int skyeye_option_init (skyeye_config_t * config);
void init_callback();
void init_command_list();
void init_stepi();
exception_t init_module_list();
void SKY_load_all_modules(char* lib_dir, char* suffix);
void SKY_unload_all_modules();
int init_thread_scheduler();
int init_timer_scheduler();
void init_arch();
void init_bus();
void init_mach();
int init_bp();
void init_conf_obj();
void stop_all_cell();
void start_all_cell();
void destroy_threads(void);
void init_threads(void);
exception_t mem_reset ();
void io_reset (void * state);
int register_arch_option (const char *arch_name);
char* SetSKYEYEBIN();
int RunCommand(char *cmdstr);
int RunAutotest();
exception_t init_common_class();
char* get_bin_env_path(char *path_name);
char* get_ws_path();
#ifdef __cplusplus
}
#endif

#endif
