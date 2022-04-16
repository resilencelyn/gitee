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
* @file c55_cpu.h
* @brief the core definition of c55
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-07-31
*/

#ifndef __C55_CPU_H__
#define __C55_CPU_H__
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
    uint32_t x;
    uint32_t x1;
    uint32_t x2;
    uint32_t f;
    uint32_t f1;
    uint32_t fs;
    uint32_t fd;
    uint32_t fdst;
    uint32_t fsrc;
    uint32_t c;
    uint32_t eq;
    uint32_t m1;
    uint32_t a1;
    uint32_t xdst;
    uint32_t smem;
    uint32_t lmem;
    uint32_t ymem;
    uint32_t addr;
    uint32_t src;
    uint32_t dst;
    uint32_t acx;
    uint32_t acy;
    uint32_t dst1;
    uint32_t shift;
    uint32_t k16;
    uint32_t bk16;
    uint32_t k4;
    uint32_t k8;
    uint32_t p24;
    uint32_t l8;
    uint32_t l7;
    uint32_t l16;
    uint32_t I4;
} opcode_fields_t;

typedef int (*insn_action_t) (conf_object_t * conf, void *core, uint64_t insn, opcode_fields_t * fields);
struct instruction_set_encoding_item
{
    const char *name;
    int attribute_value;
    int version;
    int content[21];
    int insn_lenth;
    int PEB;                            //Parallel Enable Bit
};
typedef struct instruction_set_encoding_item ISEITEM;

typedef struct _insn_cache
{
    uint64_t insn;
    insn_action_t action;
    uint32_t addr;
    opcode_fields_t fields;
} insn_cache_t;

typedef union
{
    uint32_t xar;
    struct
    {
        uint16_t ar;
        uint16_t arh;                   /* only use 7 bit */
    };
} xar_t;
typedef struct c55_core
{
    conf_object_t *obj;
    /* General Register */
    uint64_t ac[4];

    xar_t xar[8];                       /* only use 23 bit */

    union
    {
        uint32_t xcdp;                  /* only use 23 bit */
        struct
        {
            uint16_t cdp;
            uint16_t cdph;              /* only use 7 bit */
        };
    };

    union
    {
        uint32_t xdp;                   /* only use 23 bit */
        struct
        {
            uint16_t dp;
            uint16_t dph;               /* only use 7 bit */
        };
    };

    /* data stack pointer */
    union
    {
        uint32_t xsp;                   /* only use 23 bit */
        struct
        {
            uint16_t sp;
            uint16_t sph;               /* only use 7 bit */
        };
    };

    /* system stack pointer */
    union
    {
        uint32_t xssp;                  /* only use 23 bit */
        struct
        {
            uint16_t ssp;
            uint16_t ssph;              /* only use 7 bit */
        };
    };

    uint16_t bk03;                      /* Circular buffer size registers */
    uint16_t bk47;
    uint16_t bkc;

    uint16_t brs1;

    uint16_t brc0;                      /* Block-repeat counters 0 and 1 */
    uint16_t brc1;

    uint16_t bsa01;
    uint16_t bsa23;
    uint16_t bsa45;
    uint16_t bsa67;
    uint16_t bsac;

    uint8_t cfct;
    uint16_t csr;

    union
    {
        uint32_t dbier;
        struct
        {
            uint16_t dbier0;
            uint16_t dbier1;
        };
    };
    uint16_t dbstat;

    union
    {
        uint32_t ier;
        struct
        {
            uint16_t ier0;
            uint16_t ier1;
        };
    };

    union
    {
        uint32_t ifr;
        struct
        {
            uint16_t ifr0;
            uint16_t ifr1;
        };
    };

    uint16_t ivpd;
    uint16_t ivph;

    uint32_t pc;                        /* only use 24 bit */
    uint16_t pdp;                       /*only use 9 bit */
    uint32_t rea0;                      /*only use 24 bit */
    uint32_t rea1;                      /*only use 24 bit */
    uint32_t reta;                      /*only use 24 bit */
    uint16_t rptc;

    uint32_t rsa0;                      /*only use 24 bit */
    uint32_t rsa1;                      /*only use 24 bit */
    uint32_t rptb_enable;

    uint16_t st0_55;
    uint16_t st1_55;
    uint16_t st2_55;
    uint16_t st3_55;
    uint16_t t[4];
    uint16_t trn0;
    uint16_t trn1;

    uint32_t pce1;
    uint32_t vector_baseaddr;
    uint64_t stop_step;
    uint64_t insn_num;
    uint32_t insn_length;
    uint32_t intr_flag;
    RWLOCK_T ifr_lock;
    uint32_t PEB;                       //Parallel Enable Bit
    int init_memory_flag;
    int exec_enable;
    insn_cache_t insn_cache[INSN_CACHE_SIZE];
    addr_space_t *addr_space;
    memory_space_intf *mem_iface;
    memory_space_intf *memory_space_iface;
    conf_object_t *memory_space;
    memory_space_intf *c55_memory_intf;
    breakpoint_mgt_t bp_manager;
    char exception_name[100];
} c55_core_t;

#define INTM_BIT (0x1 << 11)
#define FRCT_BIT (0x1 << 6)
/* Memory map register */
#define ST0_ADDR  	0x2
#define ST1_ADDR 	0x3
#define ST2_ADDR 	0x4b
#define ST3_ADDR 	0x4

#define SSP_ADDR 	0x4c
#define SP_ADDR  	0x4d

#define IVPD_ADDR  	0x49
#define IVPH_ADDR  	0x4a
#define IER_ADDR 	0x0

#define AC0_ADDR  	0x8
#define AC0G_ADDR  	0xa
#define AC1_ADDR  	0xb
#define AC1G_ADDR  	0xd
#define AC2_ADDR  	0x24
#define AC2G_ADDR  	0x26
#define AC3_ADDR  	0x28
#define AC3G_ADDR  	0x2a

#define BKC_ADDR  	0x31
#define BK03_ADDR  	0x19
#define BK47_ADDR  	0x30
#define DPH_ADDR  	0x2b
#define BRC0_ADDR  	0x1a
#define RSA0L_ADDR  0x3d
#define RSA0H_ADDR  0x3c
#define REA0L_ADDR  0x3f
#define REA0H_ADDR  0x3e
#define BRS1_ADDR  	0x3a
#define BRC1_ADDR  	0x39
#define RSA1L_ADDR  0x41
#define RSA1H_ADDR  0x40
#define REA1L_ADDR  0x43
#define REA1H_ADDR  0x42
#define CSR_ADDR  	0x3b
#define RPTC_ADDR  	0x44
#define TRN0_ADDR  	0xf
#define TRN1_ADDR  	0x38
#define BSA01_ADDR 	0x32
#define BSA23_ADDR 	0x33
#define BSA45_ADDR 	0x34
#define BSA67_ADDR 	0x35
#define BSAC_ADDR  	0x36

enum MMAP_ARX
{
    AR0_ADDR = 0x10,
    AR1_ADDR,
    AR2_ADDR,
    AR3_ADDR,
    AR4_ADDR,
    AR5_ADDR,
    AR6_ADDR,
    AR7_ADDR
};

void c55x_set_pc(conf_object_t * opaque, generic_address_t addr);
generic_address_t c55x_get_pc(conf_object_t * opaque);
uint64_t c55x_get_steps(conf_object_t * obj);
endian_t c55x_get_endian(conf_object_t * obj);
align_t c55x_get_alignment(conf_object_t * obj);
void c55x_reset(conf_object_t * conf_obj);
uint32_t c55x_get_address_width(conf_object_t * obj);
char *c55x_disassemble(conf_object_t * obj, generic_address_t addr);
char *c55x_get_architecture(conf_object_t * obj);
void per_cpu_step(conf_object_t * conf_object);
uint32_t c55x_step_once(conf_object_t * conf_obj);
void c55x_stepi(conf_object_t * obj, uint64_t steps);
exception_t c55x_signal(conf_object_t * conf_obj, interrupt_signal_t * signal);

int c55x_insert_bp(conf_object_t * conf_obj, generic_address_t addr);
int c55x_delete_bp(conf_object_t * conf_obj, generic_address_t addr);
int c55x_get_bp_numbers(conf_object_t * conf_obj);
uint32_t c55x_get_bp_addr_by_id(conf_object_t * conf_obj, int id);
int c55x_get_trigger(conf_object_t * conf_obj);
bool_t c55x_check_bp_trigger(conf_object_t * conf_obj);
int c55x_clear_bp_trigger(conf_object_t * conf_obj);
exception_t memory_space_set(conf_object_t * obj, conf_object_t * obj2, const char *port, int index);
exception_t memory_space_get(conf_object_t * obj, conf_object_t ** obj2, const char **port, int index);
exception_t c55x_space_read(conf_object_t * obj, generic_address_t addr, void *buf, size_t count);
exception_t c55x_space_write(conf_object_t * obj, generic_address_t addr, void *buf, size_t count);

//const char* c55_get_regname_by_id(conf_object_t* conf_obj, int id);
//uint32 c55_get_regvalue_by_id(conf_object_t* conf_obj, int id);
//exception_t c55_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, int id);
//uint32 c55_get_regid_by_name(conf_object_t* opaque, char* name);
//uint32 c55_get_regnum(conf_object_t* conf_obj);
//void c55_run(conf_object_t* conf_object);
//void c55_stop(conf_object_t* conf_obj);

int c55x_get_cpu_exception_num(conf_object_t * obj);
void c55x_set_cpu_exception_by_id(conf_object_t * obj, int exception_id);
char *c55x_get_cpu_exception_name_by_id(conf_object_t * obj, int exception_id);

#define C55X_IRQ_NUM 32
#endif
