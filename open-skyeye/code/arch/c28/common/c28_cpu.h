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
* @file c28_cpu.h
* @brief the core definition of c28
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-07-31
*/

#ifndef __C28_CPU_H__
#define __C28_CPU_H__
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
typedef struct _opcode_fields
{
    uint32_t op0_15;
    uint32_t op1_15;
    uint32_t op3_15;
    uint32_t op4_15;
    uint32_t op5_15;
    uint32_t op6_15;
    uint32_t op8_15;
    uint32_t op9_15;
    uint32_t op10_15;
    uint32_t op11_15;
    uint32_t op12_15;

    uint32_t mode0_2;
    uint32_t mode3_5;
    uint32_t mode6_7;
    uint32_t mode0_3;
    uint32_t mode4_7;
    uint32_t mode8_10;
    uint32_t bit6;
    uint32_t imm16;

    uint32_t cond8_11;
    uint32_t cond0_7;
    uint32_t offset0_7;
    uint32_t offset24_31;
    uint32_t offset0_6;
    uint32_t offset0_9;
    uint32_t shift0_3;
    uint32_t shift8_11;
    uint32_t shift24_27;
    uint32_t a8;
    uint32_t a0;
    uint32_t a4;
    uint32_t b;
} opcode_fields_t;

typedef int (*insn_action_t) (void *core, uint32_t insn, opcode_fields_t * fields);
struct instruction_set_encoding_item
{
    const char *name;
    int attribute_value;
    int version;
    int content[21];
    insn_action_t action;
};
typedef struct instruction_set_encoding_item ISEITEM;

typedef struct _insn_cache
{
    uint32_t insn;
    insn_action_t action;
    uint32_t addr;
    opcode_fields_t fields;
} insn_cache_t;

struct RB_BITS
{
    uint16_t RC:16;                     //Repeat Count
    uint16_t RE:7;                      //Repeat block end address
    uint16_t RSIZE:7;                   //Repeat block size 
    uint16_t RA:1;                      //repeat Block Active Bit
    uint16_t RAS:1;                     //Repeat Block Active Shadow Bit
};
typedef struct c28_core
{
    conf_object_t *obj;
    /* General Register */
    union
    {
        uint32_t acc;
        struct
        {
            union
            {
                uint16_t al;
                struct
                {
                    uint8_t al_LSB;
                    uint8_t al_MSB;
                };
            };
            union
            {
                uint16_t ah;
                struct
                {
                    uint8_t ah_LSB;
                    uint8_t ah_MSB;
                };
            };
        };
    };
    union
    {
        uint32_t p;
        struct
        {
            uint16_t pl;
            uint16_t ph;
        };
    };
    struct
    {
        union
        {
            uint32_t xar;
            struct
            {
                uint16_t ar;
                uint16_t arh;
            };
        };
    } xar[8];
    union
    {
        uint32_t xt;
        struct
        {
            uint16_t tl;
            uint16_t th;
        };
    };
    uint32_t dp;
    uint32_t data_addr;
    uint32_t sp_addr;
    uint32_t dp_addr;
    uint32_t xar_addr;
    uint16_t ifr;
    uint16_t ier;
    uint16_t dbgier;
    uint16_t dbgstat;
    uint32_t ic;
    uint32_t pc;                        /* only use 22 bit */
    uint32_t rpc;                       /* return pc, only use 22 bit */
    struct
    {
        union
        {
            uint32_t Ra;
            struct
            {
                uint16_t Ral;
                uint16_t Rah;
            };
        };
    } fp_regs[8];                       /* Floating-point result Register */
    uint32_t stf;                       /* Floating-point status Register */
    /* Repeat block Register */
    union
    {
        uint32_t rb;
        struct
        {
            uint16_t rbl;
            uint16_t rbh;
        };

        struct RB_BITS rb_bit;
    };
    int rb_ret;                         //Repeat block end address
    uint16_t sp;
    uint32_t sp_count;
    uint16_t st0;
    uint16_t st1;
    uint16_t rptc;
    uint16_t rptc_ret;

    uint32_t pce1;
    uint64_t stop_step;
    uint32_t pie_group_num;
    uint32_t pie_intr_num[12][8];
    uint32_t signal_type;
    uint64_t insn_num;
    uint16_t insn_length;
    uint32_t freq_hz;
    insn_cache_t insn_cache[INSN_CACHE_SIZE];
    RWLOCK_T ifr_lock;
    memory_space_intf *memory_space_iface;
    conf_object_t *memory_space;
    memory_space_intf *c28_memory_intf;
    breakpoint_mgt_t bp_manager;
    char exception_name[100];
} c28_core_t;

void c28x_set_pc(conf_object_t * opaque, generic_address_t addr);
generic_address_t c28x_get_pc(conf_object_t * opaque);
uint64_t c28x_get_steps(conf_object_t * obj);
endian_t c28x_get_endian(conf_object_t * obj);
align_t c28x_get_alignment(conf_object_t * obj);
void c28x_reset(conf_object_t * conf_obj);
uint32_t c28x_get_address_width(conf_object_t * obj);
char *c28x_disassemble(conf_object_t * obj, generic_address_t addr);
char *c28x_get_architecture(conf_object_t * obj);
void c28x_get_cpu_run_status(conf_object_t * obj, uint32_t * cpu_freq, uint64_t * cpu_insn_num);
void c28x_set_memory_watch_status(conf_object_t * obj, uint32_t addr, int status);
int c28x_get_cpu_exception_num(conf_object_t * obj);
void c28x_set_cpu_exception_by_id(conf_object_t * obj, int exception_id);
char *c28x_get_cpu_exception_name_by_id(conf_object_t * obj, int exception_id);

int per_cpu_step(conf_object_t * conf_object);
int c28x_step_once(conf_object_t * conf_obj);
void c28x_stepi(conf_object_t * obj, uint64_t steps);
exception_t c28x_signal(conf_object_t * conf_obj, c28_signal_t * signal);

int c28x_insert_bp(conf_object_t * conf_obj, generic_address_t addr);
int c28x_delete_bp(conf_object_t * conf_obj, generic_address_t addr);
int c28x_get_bp_numbers(conf_object_t * conf_obj);
uint32_t c28x_get_bp_addr_by_id(conf_object_t * conf_obj, int id);
int c28x_get_trigger(conf_object_t * conf_obj);
bool_t c28x_check_bp_trigger(conf_object_t * conf_obj);
int c28x_clear_bp_trigger(conf_object_t * conf_obj);
exception_t memory_space_set(conf_object_t * obj, conf_object_t * obj2, const char *port, int index);
exception_t memory_space_get(conf_object_t * obj, conf_object_t ** obj2, const char **port, int index);
exception_t c28x_space_read(conf_object_t * obj, generic_address_t addr, void *buf, size_t count);
exception_t c28x_space_write(conf_object_t * obj, generic_address_t addr, void *buf, size_t count);
exception_t c28x_space_read_hl(conf_object_t * obj, generic_address_t addr, void *buf, size_t count, int hl_flag);
exception_t c28x_space_write_hl(conf_object_t * obj, generic_address_t addr, void *buf, size_t count, int hl_flag);

#define PIE_VECTOR_BASE_ADDR 0xd00
#define PIE_VECTOR_GROUP_BASE_ADDR 0xd40
#define PIE_GROUP_NUM 12
#define PIE_INT_NUM 8

#define C28X_IRQ_NUM 16
#endif
