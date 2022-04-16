
/*
 * debugger.h - necessary definition for skyeye debugger
 * Copyright (C) 2003 Skyeye Develop Group
 * for help please send mail to <skyeye-developer@lists.sf.linuxforum.net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */
/*
 * 12/04/2005   ksh  <blackfin.kang@gmail.com>
 * */

#ifndef __SKYEYE2GDB_H__
#define __SKYEYE2GDB_H__

#include <skyeye_types.h>
#include "gdb_tracepoint.h"
#include "skyeye_signal.h"

//chy 2006-04-12 add ICE Simulation
//chy 2006-04-12 define skyeye_ice struct
#define MAX_BREAKPOINTS 16
#define MAX_TRACEPOINTS 16
#define MAX_ACTION_LENGTH 80

struct SkyEye_ICE
{
    unsigned int bps[MAX_BREAKPOINTS];
    int num_bps;
    trace_status tps_status;            // is tracing enabled or disabled ?
    tracepoint_def tps[MAX_TRACEPOINTS];        // tracepoints array
    int num_tps;
    frame_buffer *fb;                   // frames linked list
    int num_fb;
    frame_buffer *selected_fb;
    ro_region *ro_region_head;
};
extern struct SkyEye_ICE skyeye_ice;

typedef struct register_defs
{
    char *name;
    int (*register_raw_size) (int x);
    int register_bytes;
    int (*register_byte) (int x);
    int num_regs;
    int max_register_raw_size;
    int endian_flag;
    int pc_regnum;
    int sp_regnum;
    int fp_regnum;
    int byte_width;                     // arch has `byte_width` * 8 bits in one byte
#define EIGHT_BITS_PER_BYTE	1
#define SIXTEEN_BITS_PER_BYTE	2
    int (*store_register) (conf_object_t * obj, int rn, unsigned char *memory);
    int (*fetch_register) (conf_object_t * obj, int rn, unsigned char *memory);
    int (*register_num) (conf_object_t * obj);
} register_defs_t;

#define MAX_SUPP_ARCH 9
int sim_debug();

//typedef struct register_defs register_defs_t;
int sim_read(conf_object_t * core, generic_address_t addr, unsigned char *buffer, int size, int byte_width);
int sim_write(conf_object_t * core, generic_address_t addr, unsigned char *buffer, int size, int byte_width);
int sim_ice_breakpoint_remove(conf_object_t * core, generic_address_t addr);
int sim_ice_breakpoint_insert(conf_object_t * core, generic_address_t addr);
void remote_interrupt(conf_object_t * conf_obj);

void gdbserver_cont(conf_object_t * conf_obj);
void gdbserver_step(conf_object_t * core);
signal_status_t sim_get_signal_status(conf_object_t * core);
void tomem(endian_t endian, unsigned char *memory, int val);
int frommem(endian_t endian, unsigned char *memory);
void register_reg_type(register_defs_t * reg_type);
#endif
