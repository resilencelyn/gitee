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
* @file c28_decode.h
* @brief some definition for decoder
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-07-31
*/

#ifndef __DECODER_H__
#define __DECODER_H__

enum DECODE_STATUS
{
    DECODE_SUCCESS,
    DECODE_FAILURE
};

#define BITS(a,b) ((insn >> (a)) & ((1 << (1+(b)-(a)))-1))
#define BIT(n) ((instr >> (n)) & 1)
#define SIGN_EXTEND(cst, n) ((cst & (1 << (n - 1))) ? (cst | (0xFFFFFFFF << n)) : cst)
#define DSZ(header) ((header >> 16) & 0x7)
#define bit(m,n) ((m >> (n)) & 1)

#define SET(m, n) (m = (m | (0x1 << n)))
#define SET_BIT(m, data) (m = (m | data))
#define CLEAR_BIT(m, data) (m = (m & (~data)))
#define SET_AR(m, n) (m = ((m & 0x0fff) | (n << 13)))
#define CLEAR(m, n) (m = m & (~(0x1 << n)))

int trans_opcode_fields(uint32_t insn, opcode_fields_t * fields);
insn_action_t get_insn_action(c28_core_t * core, uint32_t insn);
uint32_t exec_insn_cache(c28_core_t * core, insn_cache_t * cache);
int check_insn_address_mode(c28_core_t * core, uint32_t insn, opcode_fields_t * fields, uint32_t loc32);
void check_insn_flag_cond(c28_core_t * core, uint32_t insn, opcode_fields_t * fields);
int check_float_insn_flag_cond(c28_core_t * core, int cond);
void check_insn_status_cond(c28_core_t * core, uint32_t insn, opcode_fields_t * fields);
int check_xar_increment_mode(c28_core_t * core, uint32_t insn, uint32_t loc32);
int check_xar_decrement_mode(c28_core_t * core, uint32_t insn, uint32_t loc32);
int check_xar_add_ar_mode(c28_core_t * core, uint32_t insn);
int check_register_mode(c28_core_t * core, uint32_t insn);
int *get_register(c28_core_t * core, uint32_t insn, uint32_t loc);
void judge_flags_modes(c28_core_t * core, uint32_t insn);
int c28_memory_read(c28_core_t * core, uint32_t addr);
int32_t sign_extend(int32_t a);
void c28_memory_write(c28_core_t * core, uint32_t addr, uint16_t buffer);
int c28_memory_read_byte_hl(c28_core_t * core, uint32_t addr, int hl_flag);
void c28_memory_write_byte_hl(c28_core_t * core, uint32_t addr, uint8_t buffer, int hl_flag);
int fpu_get_single(c28_core_t * core, uint16_t Ra);
void fpu_put_single_bit16(c28_core_t * core, uint32_t val, uint16_t Ra);
void fpu_put_single_bit32(c28_core_t * core, uint32_t val, uint16_t Ra);

typedef union
{
    float value;
    uint32_t data;
} s_precision;

/* Register Addressing Modes */

#define AR0 0x0
#define AR1 0x1
#define AR2 0x2
#define AR3 0x3
#define AR4 0x4
#define AR5 0x5
#define AR6 0x6
#define AR7 0x7
#define AH  0x8
#define AL  0x9
#define PH  0xa
#define PL  0xb
#define TH  0xc
#define SP  0xd
#define DP  0xe

#define TRUE  1
#define FALSE 0
/* Set the specified status bits */
/* ST0 */
#define SXM 0x1
#define OVM 0x2
#define TC  0x4
#define C   0x8
#define Z   0x10
#define N   0x20
#define V   0x40

#define SXM_BIT    0
#define OVM_BIT    1
#define TC_BIT     2
#define C_BIT      3
#define Z_BIT      4
#define N_BIT      5
#define V_BIT      6
#define PM_BIT     7
/* ST1 */
#define INTM      0x10
#define DBGM      0x20
#define PAGE0     0x40
#define VMAP      0x80
#define OBJMODE   0x200

#define INTM_BIT      0
#define DBGM_BIT      1
#define PAGE0_BIT     2
#define VMAP_BIT      3
#define SPA_BIT       4
#define EALLOW_BIT    6
#define IDLESTAT_BIT  7
#define AMODE_BIT     8
#define OBJMODE_BIT   9
#define M0M1MAP_BIT  11

/* STF */
#define LVF         0x1                 /* Latched Overflow */
#define LUF         0x2                 /* Latched Underflow */
#define NF  	    0x4                 /* Negative floating-point */
#define ZF  	    0x8                 /* Zero floating-point */
#define NI  	    0x10                /* Negative integer */
#define ZI  	    0x20                /* Zero integer */
#define TEST_FLAG  	0x40            /* Test */
#define RND32       0x200

#define LVF_BIT     0
#define LUF_BIT     1
#define NF_BIT      2
#define ZF_BIT      3
#define NI_BIT      4
#define ZI_BIT      5
#define TF_BIT      6
#define RND32_BIT   9

/* COND */
#define NEQ 	0x0
#define EQ 		0x1
#define GT 		0x2
#define GEQ 	0x3
#define LT 		0x4
#define LEQ 	0x5
#define HI 		0x6
#define HIS_C 	0x7
#define LO_NC  	0x8
#define LOS  	0x9
#define NOV  	0xa
#define OV  	0xb
#define NTC  	0xc
#define TC_C  	0xd
#define NBIO  	0xe
#define UNC  	0xf

#define TF  	0xa
#define NTF  	0xb
#define LU  	0xc
#define LV  	0xd
#define UNCD  	0xe
#define UNCF  	0xf

#define POSITIVE_OVERFLOW 0x7FFFFFFF
#define FLOAT_POSITIVE_OVERFLOW 0x7F800000
#define NEGATIVE_OVERFLOW 0x80000000
#define NEGATIVE_UNDERFLOW 0xffffffff
#define NEGATIVE_OVERFLOW_BIT16 0x8000

#define LSB_ADDR 0
#define MSB_ADDR 1
#endif
