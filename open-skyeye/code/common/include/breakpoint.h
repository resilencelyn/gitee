/*
        breakpoint.h - definition for breakpoint related operation
        Copyright (C) 2009 Michael.Kang
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
 * 05/16/2009   Michael.Kang  <blackfin.kang@gmail.com>
 */
#ifndef __COMMON_BREAKPOINT_H__
#define __COMMON_BREAKPOINT_H__

#include "skyeye_types.h"

typedef enum{
	SIM_access_read = 1,
	SIM_access_write = 2,
	SIM_access_execute = 4
}access_t;

typedef enum{
	/* the breakpoint on physical address */
	SIM_Break_Physical = 0,
	/* the breakpoint on virtual address */
	SIM_Break_Virtual = 1,
	SIM_Break_Linear = 2 /* x86 only */
}breakpoint_kind_t;

typedef int breakpoint_id_t;

#define MAX_BP_NUMBER 255
typedef struct breakpoint_s{
	breakpoint_id_t id;
	access_t access_type;
	breakpoint_kind_t address_type;
	generic_address_t address;
	uint32 hits;
	uint32 skiped;
	generic_address_t stop_address;
}breakpoint_t;

typedef struct breakpoint_mgt{
	breakpoint_t breakpoint[MAX_BP_NUMBER];
	int bp_number;
	breakpoint_t *breakpoint_hit;
	uint64_t last_bp_step;
}breakpoint_mgt_t;

#ifdef __cplusplus
 extern "C" {
#endif

breakpoint_t* get_bp_by_id(breakpoint_mgt_t *break_manager, breakpoint_id_t id);
breakpoint_t* get_bp_by_addr(breakpoint_mgt_t *break_manager, generic_address_t addr);
int insert_bp(breakpoint_mgt_t *break_manager, access_t access_type, breakpoint_kind_t address_type, generic_address_t addr);
int remove_bp(breakpoint_mgt_t *break_manager, breakpoint_id_t id);
int remove_bp_by_addr(breakpoint_mgt_t *break_manager, generic_address_t addr);
int init_breakpoint_manager(breakpoint_mgt_t *break_manager);
int create_a_breakpoint(breakpoint_mgt_t *break_manager, generic_address_t addr);
int get_bp_numbers(breakpoint_mgt_t *break_manager);
int check_breakpoint(breakpoint_mgt_t *break_manager, generic_address_t pc, uint64_t steps);
int check_pc_hit(breakpoint_mgt_t *break_manager, generic_address_t pc, uint64_t steps);

typedef void (*breakpoint_iterator_fn)(breakpoint_t *bp, void *arg);
void iterate_breakpoints(breakpoint_mgt_t *break_manager, breakpoint_iterator_fn fn, void *arg);

int clear_breakpoint_hit_flag(breakpoint_mgt_t *break_manager);
int get_breakpoint_hit_id(breakpoint_mgt_t *break_manager);
int check_breakpoint_hit_flag(breakpoint_mgt_t *break_manager);

#ifdef __cplusplus
}
#endif

#endif
