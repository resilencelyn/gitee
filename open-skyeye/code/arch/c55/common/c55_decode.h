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
* @file c55_decode.h
* @brief some definition for decoder
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-07-31
*/

#ifndef __DECODER_H__
#define __DECODER_H__

#define BITS(a,b) ((insn >> (a)) & ((1 << (1+(b)-(a)))-1))
#define BIT(n) ((instr >> (n)) & 1)
#define SIGN_EXTEND(cst, n) ((cst & (1 << (n - 1))) ? (cst | (0xFFFFFFFF << n)) : cst)
#define DSZ(header) ((header >> 16) & 0x7)
#define SET(m, n) (m = (m | (0x1 << n)))
#define CLEAR(m, n) (m = m & (~(0x1 << n)))
#define bit(m, n) ((m>>n) & 1)
#define SET_BIT(m, b, n) (m = (m | (b << n)))
#define SET_LOW_WORD(m, b) (m = ((m & 0xFFFF0000) | b))

enum DECODE_STATUS
{
    DECODE_SUCCESS,
    DECODE_FAILURE
};

int trans_opcode_fields(uint64_t insn, opcode_fields_t * fields);
insn_action_t get_insn_action(c55_core_t * core, uint64_t insn);
uint32_t exec_insn_cache(conf_object_t * conf, c55_core_t * core, insn_cache_t * cache);
int c55_memory_read(c55_core_t * core, uint32_t addr);
void c55_memory_write(c55_core_t * core, uint32_t addr, uint16_t buffer);
int *get_xadst_register(c55_core_t * core, uint64_t insn, opcode_fields_t * fields);
uint16_t bit16_reverse(uint16_t src);
void mov_k4_dst(c55_core_t * core, uint64_t insn, opcode_fields_t * fields);
void mov_negk4_dst(c55_core_t * core, uint64_t insn, opcode_fields_t * fields);
void mov_k8_dst(c55_core_t * core, uint64_t insn, opcode_fields_t * fields);
void mov_lmem_acx(c55_core_t * core, uint64_t insn, opcode_fields_t * fields);
void neg_src_dst(c55_core_t * core, uint64_t insn, opcode_fields_t * fields);
void add_k4_dst(c55_core_t * core, uint64_t insn, opcode_fields_t * fields);
void sfts_dst_right(c55_core_t * core, uint64_t insn, opcode_fields_t * fields);
void sfts_dst_left(c55_core_t * core, uint64_t insn, opcode_fields_t * fields);
void sftl_dst_right(c55_core_t * core, uint64_t insn, opcode_fields_t * fields);
void sftl_dst_left(c55_core_t * core, uint64_t insn, opcode_fields_t * fields);
void sftl_acx_shift_acy(c55_core_t * core, uint64_t insn, opcode_fields_t * fields);
void mov_k16_dst(c55_core_t * core, uint64_t insn, opcode_fields_t * fields);
void sub_k4_dst(c55_core_t * core, uint64_t insn, opcode_fields_t * fields);
void mov_k16_acx(c55_core_t * core, uint64_t insn, opcode_fields_t * fields);

void Overflow_check(c55_core_t * core, uint64_t insn, uint32_t fdst);
int check_cond_l4(c55_core_t * core, int cond, int src, int fs);
exception_t check_parallel_insn(conf_object_t * conf_obj, c55_core_t * core);

uint16_t read_val_from_direct_smem_address(c55_core_t * core, uint64_t insn);
uint16_t read_val_from_indirect_smem_address(c55_core_t * core, uint64_t insn);
uint32_t read_val_from_direct_lmem_address(c55_core_t * core, uint64_t insn);
uint32_t read_val_from_indirect_lmem_address(c55_core_t * core, uint64_t insn);

void write_val_to_direct_smem_address(c55_core_t * core, uint64_t insn, uint16_t mem16_val);
void write_val_to_indirect_smem_address(c55_core_t * core, uint64_t insn, uint16_t mem16_val);
void write_val_to_direct_lmem_address(c55_core_t * core, uint64_t insn, uint32_t mem16_val);
void write_val_to_indirect_lmem_address(c55_core_t * core, uint64_t insn, uint32_t mem16_val);
#endif

#define XSP  4
#define XSSP 5
#define XDP  6
#define XCDP 7

/*ST0_55 status bit */

#define TC1    13
#define TC2    12
#define CARRY  11
#define ACOV0  10
#define ACOV1  9
#define ACOV2  15
#define ACOV3  14

/*ST1_55 status bit */
#define FRCT   6
#define C16    7
#define SXMD   8
#define SATD   9
#define M40    10
#define INTM   11
#define HM     12
#define XF     13
#define CPL    14
#define BRAF   15

/*ST2_55 status bit */
#define ARMS    15

/*ST3_55 status bit */
#define SST    0
#define SMUL   0x1
#define CLK    0x2
#define SATA   0x5
#define MPNMC  0x6
#define CBERR  0x7
#define HINT   0xc
#define CACLR  0xd
#define CAEN   0xe
#define CAFRZ  0xf

#define TRUE  1
#define FALSE 0
