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
* @file c54x_decode.h
* @brief some definition for decoder
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-07-31
*/

#ifndef __DECODER_H__
#define __DECODER_H__


#define BITS(a,b) ((insn >> (a)) & ((1 << (1+(b)-(a)))-1))
#define BIT(n) ((instr >> (n)) & 1)
#define SIGN_EXTEND(cst, n) ((cst & (1 << (n - 1))) ? (cst | (0xFFFFFFFFFF << n)) : cst)
#define SIGN_EXTEND_40BIT(cst) ((cst & (1 << 31)) ? (cst & (0xFFFFFFFFFF)) : cst)
#define DSZ(header) ((header >> 16) & 0x7)
#define SET(m, n) (m = (m | (0x1 << n)))
#define CLEAR(m, n) (m = m & (~(0x1 << n)))
#define bit(m, n) ((m>>n) & 1)
#define SET_BIT(m, b, n) (m = (m | (b << n)))
#define SET_LOW_WORD(m, b) (m = ((m & 0xFFFF0000) | b))

enum DECODE_STATUS {
	DECODE_SUCCESS,
	DECODE_FAILURE
};

int trans_opcode_fields(uint16_t insn, opcode_fields_t* fields);
insn_action_t get_insn_action(c54x_core_t* core, uint16_t insn);
uint32_t exec_insn_cache(c54x_core_t* core, insn_cache_t* cache);
uint16_t c54x_memory_read(c54x_core_t* core, uint32_t addr);
void c54x_memory_write(c54x_core_t* core, uint32_t addr, uint16_t buffer);

void write_acc_reg(c54x_core_t* core, uint64_t reg_val, int flag);
int check_cond(c54x_core_t* core, int cond);
void cycle_dec(c54x_core_t* core);

/*ST0 status bit*/
#define DP     0
#define OVB    9 
#define OVA    10 
#define C      11 
#define TC     12 
#define ARP    13 

/*ST1 status bit*/
#define ASM    0 
#define CMPT   5 
#define FRCT   6 
#define C16    7 
#define SXM    8 
#define OVM    9 
#define INTM   11 
#define HM     12 
#define XF     13 
#define CPL    14 
#define BRAF   15 

#define TRUE  1
#define FALSE 0

#define ONE_WORD  1
#define TWO_WORD  2

#define OVERFLOW 0xFF80000000
#define HALF_OVERFLOW 0x10000
#endif
