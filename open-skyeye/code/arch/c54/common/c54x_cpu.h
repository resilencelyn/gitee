/* Copyright (C) 
* 2013 - Michael.Kang blackfin.kang@gmail.com
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
* @file c54x_cpu.h
* @brief the core definition of c54x
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-07-31
*/

#ifndef __C54X_CPU_H__
#define __C54X_CPU_H__
#include <stdint.h>
#include <skyeye_types.h>
#include <skyeye_lock.h>
#include <skyeye_addr_space.h>
#include "skyeye_types.h"
#include <skyeye_exec.h>
#include <skyeye_arch.h>
#include <skyeye_mm.h>
#include <skyeye_obj.h>
#include <skyeye_class.h>
#include <skyeye_iface.h>
#include <skyeye_core_intf.h>
#include <skyeye_int_register.h>
#include <sim_control.h>

#define INSN_CACHE_SIZE 4096
#define HASH(addr) (addr % INSN_CACHE_SIZE)

#define DELAY_SLOT_NUM 9
typedef struct _opcode_fields {
    uint32_t k3;
    uint32_t n;
    uint32_t sbit;
    uint32_t mmr;
    uint32_t I;
    uint32_t IA;
    uint32_t S;
    uint32_t D;
    uint32_t Z;
    uint32_t K;
    uint32_t shft;
    uint32_t shift;
    uint32_t cond;
}opcode_fields_t;

typedef void(*insn_action_t)(struct c54x_core* core, uint16_t insn, opcode_fields_t* fields);
struct instruction_set_encoding_item {
        const char *name;
        int attribute_value;
        int content[21];
		int words;
		int cycles;
		const char *class_mode;
};
typedef struct instruction_set_encoding_item ISEITEM;

typedef struct _insn_cache{
	uint16_t insn;
	insn_action_t action;
	uint32_t addr;
	opcode_fields_t fields;
}insn_cache_t;


typedef struct c54x_core{
	conf_object_t* obj;

	uint16_t pc;
   	uint16_t trn;
	uint16_t sp;	
	uint16_t st0;	
	uint16_t st1;	
	uint16_t ar[8];
	uint16_t treg;	
	uint64_t acc_a;
	uint64_t acc_b;
	uint16_t pmst;	
	uint16_t bk;	
	uint16_t brc;	
	uint16_t imr;	
	uint16_t rsa;	
	uint16_t ifr;	
	uint16_t rea;	
	uint16_t pmr_value;	
	uint16_t MMR_BUFFER[32];
	uint32_t insn_length;

	/* General Register */
	int rptc;
	uint64_t insn_num;
	uint64_t stop_step;
	uint32_t intr_flag;

	RWLOCK_T ifr_lock;
	int init_memory_flag;
	insn_cache_t insn_cache[INSN_CACHE_SIZE];

	uint32_t delay_enable;
	uint32_t delay_slot_cycle;
	uint32_t target_addr;

	uint32_t pend_nops_flag;
	uint32_t pend_nops;
	addr_space_t* addr_space;
	memory_space_intf* memory_space_iface;
	conf_object_t* memory_space;
	memory_space_intf* c54x_memory_intf;
	breakpoint_mgt_t bp_manager; 
	char exception_name[100];
}c54x_core_t;

//write value to memory-mapped register
#define WRITE_MMR(core, mmr, value) {\
		(core)->MMR_BUFFER[mmr] = value;\
		c54x_memory_write(core, mmr, value);}
#define READ_MMR(core, mmr, value) (value) = (core)->MMR_BUFFER[mmr]

#define WRITE_ACC_REG(core, reg_val, flag) {\
		if (flag) {\
			core->acc_b = reg_val;\
			core->MMR_BUFFER[CPU_BL] = reg_val & 0xFFFF;\
			core->MMR_BUFFER[CPU_BH] = (reg_val >> 16) & 0xFFFF;\
			core->MMR_BUFFER[CPU_BG] = (reg_val >> 32) & 0xFF;\
			c54x_memory_write(core, CPU_BL, reg_val & 0xFFFF);\
			c54x_memory_write(core, CPU_BH, (reg_val >> 16) & 0xFFFF);\
			c54x_memory_write(core, CPU_BG, (reg_val >> 32) & 0xFF);\
		}else {\
			core->acc_a = reg_val;\
			core->MMR_BUFFER[CPU_AL] = reg_val & 0xFFFF;\
			core->MMR_BUFFER[CPU_AH] = (reg_val >> 16) & 0xFFFF;\
			core->MMR_BUFFER[CPU_AG] = (reg_val >> 32) & 0xFF;\
			c54x_memory_write(core, CPU_AL, reg_val & 0xFFFF);\
			c54x_memory_write(core, CPU_AH, (reg_val >> 16) & 0xFFFF);\
			c54x_memory_write(core, CPU_AG, (reg_val >> 32) & 0xFF);\
		}\
	}

#define CYCLE_DEC(core) {\
		if (core->delay_slot_cycle == 0){\
				core->delay_enable = 0;\
				core->pc = core->target_addr;\
		}\
	}
#define PENDING_NOPS(core) {\
		if (core->pend_nops != 0){\
				core->pend_nops --;\
		}else {\
				core->pend_nops_flag = 1;\
		}\
	}
typedef enum {
	CPU_IMR = 0,
	CPU_IFR,
	CPU_TEST1,	
	CPU_TEST2,	
	CPU_TEST3,	
	CPU_TEST4,	
	CPU_ST0,	
	CPU_ST1,	
	CPU_AL,	
	CPU_AH,	
	CPU_AG,	
	CPU_BL,	
	CPU_BH,	
	CPU_BG,	
	CPU_TREG,	
	CPU_TRN,	
	CPU_AR0,	
	CPU_AR1,	
	CPU_AR2,	
	CPU_AR3,	
	CPU_AR4,	
	CPU_AR5,	
	CPU_AR6,	
	CPU_AR7,	
	CPU_SP,	
	CPU_BK,	
	CPU_BRC,	
	CPU_RSA,	
	CPU_REA,	
	CPU_PMST,	
	CPU_XPC,	
	CPU_TEST5	
#if 0
	CPU_DRR0,	
	CPU_DXR0,	
	CPU_SPC0,	
	CPU_TEST6,	
	CPU_TIM,	
	CPU_PRD, //timer period register
	CPU_TCR //timer control register	
#endif
}c54x_cpu_reg;
void c54x_set_pc(conf_object_t* opaque, generic_address_t addr);
generic_address_t c54x_get_pc(conf_object_t* opaque);
uint64_t c54x_get_steps(conf_object_t* obj);
endian_t c54x_get_endian(conf_object_t* obj);
align_t c54x_get_alignment(conf_object_t* obj);
void c54x_reset (conf_object_t* conf_obj);
uint32_t c54x_get_address_width(conf_object_t *obj);
char *c54x_disassemble(conf_object_t *obj, generic_address_t addr);
char *c54x_get_architecture(conf_object_t *obj);
void per_cpu_step(conf_object_t* conf_object);
void c54x_step_once(conf_object_t* conf_obj);
void  c54x_stepi(conf_object_t* obj, uint64_t steps);
exception_t c54x_signal(conf_object_t* conf_obj, interrupt_signal_t *signal);

int c54x_insert_bp(conf_object_t* conf_obj, generic_address_t addr);
int c54x_delete_bp(conf_object_t* conf_obj, generic_address_t addr);
int c54x_get_bp_numbers(conf_object_t *conf_obj);
uint32_t c54x_get_bp_addr_by_id(conf_object_t *conf_obj, int id);
int c54x_get_trigger(conf_object_t *conf_obj);
bool_t c54x_check_bp_trigger(conf_object_t *conf_obj);
int c54x_clear_bp_trigger(conf_object_t *conf_obj);
exception_t memory_space_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index);
exception_t memory_space_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index);
exception_t c54x_space_read(conf_object_t* obj, generic_address_t addr, void* buf, size_t count);
exception_t c54x_space_write(conf_object_t* obj, generic_address_t addr, void* buf, size_t count);
int c54x_get_cpu_exception_num(conf_object_t *obj);
void c54x_set_cpu_exception_by_id(conf_object_t *obj, int exception_id);
char* c54x_get_cpu_exception_name_by_id(conf_object_t *obj, int exception_id);

#define C54X_IRQ_NUM 16
#endif
