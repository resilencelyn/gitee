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
* @file skyeye_core.h
* @brief The core API
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2014-03-18
*/
#ifndef __SKYEYE_CORE_H__
#define __SKYEYE_CORE_H__

#include "skyeye_types.h"
#include "skyeye_config.h"
#include "skyeye_attr.h"
#include "skyeye_core_intf.h"
#ifdef __cplusplus
 extern "C" {
#endif

typedef enum{
	INTERPRET_RUN,
	DYNCOM_RUN,
	DYNCOM_RUN_SINGLE_STEP,
	HYBRID_RUN,
	RUN_MODE_NUM, /* also is a kind of invalid mode */
}core_running_mode_t;
conf_object_t* SIM_current_processor(void);
generic_address_t SIM_get_current_pc();
void SIM_set_current_pc(generic_address_t pc);
skyeye_reg_intf* SIM_get_current_reg_intf();
uint64_t SIM_get_current_steps();
uint64_t skyeye_get_core_steps(conf_object_t* core);
generic_address_t SIM_get_pc(conf_object_t* conf_obj);
void SIM_set_pc(conf_object_t* conf_obj, generic_address_t pc,generic_address_t code_start,generic_address_t code_end);

int SIM_step_count();
attr_value_t SIM_get_all_processors(void);
conf_object_t* SIM_get_processor(int proc_no);
int SIM_get_processor_number(const conf_object_t* cpu);

int SIM_number_processors(void);
void SIM_reset_prcessor(conf_object_t* cpu, int hard_reset);

//void SIM_register_core_signal(conf_obj_t* obj, char* obj_name, void (*signal)());

//void SIM_set_endianess(endian_t endian);
//endian_t SIM_get_endianess();
align_t SIM_get_current_align();
endian_t SIM_get_current_endianess();
char* SIM_get_current_arch();
core_running_mode_t SIM_get_core_mode(conf_object_t* conf_obj);
void SIM_set_core_mode(conf_object_t* conf_obj, core_running_mode_t mode);
int SIM_get_core_debug_mode(conf_object_t* conf_obj);
void SIM_set_core_debug_mode(conf_object_t* conf_obj);


#ifdef __cplusplus
}
#endif

#endif
