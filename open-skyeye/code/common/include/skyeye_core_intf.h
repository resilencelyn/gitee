/* Copyright (C) 
* 2011 - Michael.Kang blackfin.kang@gmail.com
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
* @file skyeye_core_intf.h
* @brief The running interface for a processor core
* @author Michael.Kang blackfin.kang@gmail.com
* @version 78.77
* @date 2011-12-20
*/

#ifndef __SKYEYE_CORE_INFT_H__
#define __SKYEYE_CORE_INFT_H__
#ifdef __cplusplus
 extern "C" {
#endif



typedef struct core_run{
	conf_object_t* conf_obj;
	void (*set_pc)(conf_object_t* opaque, generic_address_t addr);
	generic_address_t (*get_pc)(conf_object_t* opaque);
	uint64_t (*get_steps)(conf_object_t* obj);
	endian_t (*get_endian)(conf_object_t* obj);
	align_t (*get_alignment)(conf_object_t* obj);
	char* (*get_architecture)(conf_object_t* conf_obj);
	void (*reset) (conf_object_t* opaque);
	char *(*disassemble)(conf_object_t *obj, generic_address_t addr);
    uint64_t (*get_usec)(conf_object_t * obj);
    void (*set_x86_timer_low)(conf_object_t* obj, int flag);
    	void (*get_cpu_run_status)(conf_object_t* obj, uint32_t* cpu_freq, uint64_t* cpu_insn_num);
	uint32_t (*get_address_width)(conf_object_t *obj);
	void (*set_textsection_info)(conf_object_t* obj, generic_address_t start_addr,generic_address_t end_addr);
    void (*do_pre_translate)(conf_object_t* obj);
    void (*set_memory_watch_status)(conf_object_t* obj, uint32_t addr, int status);
	int (*get_cpu_exception_num)(conf_object_t* obj);
	void (*set_cpu_exception_by_id)(conf_object_t* obj, int exception_id);
	char *(*get_cpu_exception_name_by_id)(conf_object_t* obj, int exception_id);
}core_info_intf;
#define CORE_INFO_INTF_NAME "core_info_intf"

typedef struct x86_core_info_iface{
       void (*x86_reset) (conf_object_t* opaque);
}x86_core_intf;
#define X86_CORE_INTF_NAME "x86_core_intf"

typedef struct core_exec{
	conf_object_t* conf_obj;
	uint32_t (*run)(conf_object_t *obj);
	void (*stop)(conf_object_t *obj);
	void (*stepi)(conf_object_t *obj, uint64_t steps);
	void (*power)(conf_object_t *opaque, int run);
}core_exec_intf;
#define CORE_EXEC_INTF_NAME "core_exec_intf"

typedef struct{
	exception_t (*set)(conf_object_t *obj, size_t sev, int cpu_id);
	exception_t (*get)(conf_object_t *obj, size_t *sev, int cpu_id);
}multcore_event_signal_intf;
#define MULTCORE_EVENT_SIGNAL_INTF "multcore_event_signal_intf"

typedef struct core_update_memory_interface_cache{
	exception_t (*update_memiface_cache)(conf_object_t *obj);
}core_update_memiface_cache;
#define CORE_UPDATE_MEMIFACE_CACHE_NAME "core_update_memiface_cache_intf"

typedef struct cpre_breakpoint_op{
	int (*insert_bp)(conf_object_t *obj, generic_address_t addr);
	int (*delete_bp)(conf_object_t *obj, generic_address_t addr);
	uint32_t (*get_bp_numbers)(conf_object_t *obj);
	uint32_t (*get_bp_addr_by_id)(conf_object_t *obj, int id);
	int (*get_bp_trigger)(conf_object_t *obj);
	bool_t (*check_bp_trigger)(conf_object_t *obj);
	int (*clear_bp_trigger)(conf_object_t *obj);
	
}core_breakpoint_op_intf;
#define CORE_BREAKPOINT_OP_INTF_NAME "core_breakpoint_op_intf"

typedef struct sparc_reg_window{
	uint32_t *(*get_reg_window)(conf_object_t *obj);
}sparc_reg_window_intf;
#define SPARC_REG_WINDOW_INTF_NAME "sparc_reg_window_op_intf"

#ifdef __cplusplus
}
#endif

#endif
