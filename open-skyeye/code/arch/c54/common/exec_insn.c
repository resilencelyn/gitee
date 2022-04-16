/* Copyright (C) 
*2013 - Michael.Kang blackfin.kang@gmail.com
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
* @file exec_insn.c
* @brief The instruction interpreter
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-07-31
*/

#include "c54x_cpu.h"
#include "c54x_decode.h"
#include <stdio.h>
#include <stdlib.h>
#include "skyeye_types.h"
#include "skyeye_bus.h"
#include "skyeye_uart_ops.h"
#include "skyeye_interface.h"
#include <skyeye_swapendian.h>

#define DEBUG
#include <skyeye_log.h>

#ifdef DBG
#undef DBG
#define DBG
#endif

#define NOT_IMP printf("In %s:%d, not implement at 0x%x\n", __FUNCTION__, __LINE__, core->pc);skyeye_exit(-1)

int check_cond(c54x_core_t* core, int cond){
	int st0 = core->MMR_BUFFER[CPU_ST0];
	int tc = bit(st0, TC);
	int c = bit(st0, C);
	uint64_t A = core->acc_a;
	uint64_t B = core->acc_b;
	int ova = bit(st0, OVA);
	int ovb = bit(st0, OVB);
	int flag = FALSE;
	uint32_t A_F = (A >> 31) & 0x1;
	uint32_t B_F = (B >> 31) & 0x1;

	switch(cond){
		case 0x0: //UNC
			flag = TRUE;
			break;
		case 0x8: //C = 0
			if (c == 0)
				flag = TRUE;
			break;
		case 0xc: //C = 1
			if (c)
				flag = TRUE;
			break;
		case 0x20: //TC = 0
			if (tc == 0)
				flag = TRUE;
			break;
		case 0x30: //TC = 1
			if (tc)
				flag = TRUE;
			break;
		case 0x45: //AEQ A = 0
			if (A == 0)
				flag = TRUE;
			break;
		case 0x44: //A != 0
			if (A != 0)
				flag = TRUE;
			break;
		case 0x46: //AGT A > 0
			if (A_F == 0)
				flag = TRUE;
			break;
		case 0x42: //AGEQ A >= 0
			if (A_F == 0 || A == 0)
				flag = TRUE;
			break;
		case 0x43: //ALT A < 0
			if (A_F == 1)
				flag = TRUE;
			break;
		case 0x47: //ALEQ A <= 0
			if (A_F == 1 || A == 0)
				flag = TRUE;
			break;
		case 0x70: //AOV A overflow
			if (ova)
				flag = TRUE;
			break;
		case 0x60: //ANOV A no overflow
			if (ova == 0)
				flag = TRUE;
			break;
		case 0x4d: //BEQ B = 0
			if (B == 0)
				flag = TRUE;
			break;
		case 0x4c: //B != 0
			if (B != 0)
				flag = TRUE;
			break;
		case 0x4e: //BGT B > 0
			if (B_F == 0)
				flag = TRUE;
			break;
		case 0x4a: //BGEQ B >= 0
			if (B_F == 0 || B == 0)
				flag = TRUE;
			break;
		case 0x4b: //BLT B < 0
			if (B_F == 1)
				flag = TRUE;
			break;
		case 0x4f: //BLEQ B <= 0
			if (B_F == 1 || B == 0)
				flag = TRUE;
			break;
		case 0x78: //BOV B overflow
			if (ovb)
				flag = TRUE;
			break;
		case 0x68: //BNOV B no overflow
			if (ovb == 0)
				flag = TRUE;
			break;
		defaule:
			printf("In %s, this cond not implement at line :%d\n", __func__, __LINE__);
			skyeye_exit(-1);
	}

	return flag;
}
static void exec_ssbx(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int n = fields->n;
		int sbit = fields->sbit;
		
		if (n)
			SET(core->MMR_BUFFER[CPU_ST1], sbit);
		else 
			SET(core->MMR_BUFFER[CPU_ST0], sbit);
	}

	core->pc += ONE_WORD;
}
static void exec_rsbx(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int sbit = fields->sbit;
		int n = fields->n;
		
		if (n)
			CLEAR(core->MMR_BUFFER[CPU_ST1], sbit);
		else 
			CLEAR(core->MMR_BUFFER[CPU_ST0], sbit);
	}

	core->pc += ONE_WORD; 
}

static void exec_ld_k3_arp(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int k3 = fields->k3;
		int st0;

	//	SET_BIT(core->MMR_BUFFER[CPU_ST0], k3, ARP, 3);
		st0 = (core->MMR_BUFFER[CPU_ST0] & 0x1FFF) | k3;
		c54x_memory_write(core, CPU_ST0, st0);
	}
	core->pc += ONE_WORD; 
}
static void exec_stm(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int mmr = fields->mmr;
		uint16_t lk = c54x_memory_read(core, (core->pc + 1));

		c54x_memory_write(core, mmr, lk);
	}
	core->pc += TWO_WORD;
}

static void exec_addm(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int IA = fields->IA;
		int I = fields->I;
		uint32_t smem, result;
		int mmr, lk;

		if (IA == 0xF){ 
			mmr = c54x_memory_read(core, (core->pc + 1));
			core->pc += 1;
		}else if (IA == 0x9){  
			mmr = fields->mmr;
		}else {
			NOT_IMP;
		}

		lk = c54x_memory_read(core, (core->pc + 1)); //lk
		//READ_MMR(core, mmr, value); //read from smem
		
		smem = c54x_memory_read(core, mmr);

		result = smem + lk;
		int x = bit(smem, 15);
		int y = bit(lk, 15);
		int z = bit(result, 15);

		//Overflow condition
		if (result == HALF_OVERFLOW){
			SET(core->MMR_BUFFER[CPU_ST0], OVA);
		}
		//Carry condition
		if (((!x && y) || (!y && x)) && z == 0)
			SET(core->MMR_BUFFER[CPU_ST0], C);
		else 
			CLEAR(core->MMR_BUFFER[CPU_ST0], C);


		c54x_memory_write(core, mmr, result); //write value to smem
	}
	core->pc += TWO_WORD;
}
static void exec_andm(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int IA = fields->IA;
		int I = fields->I;
		int addr, src, dst, lk;
		int ARx = BITS(0, 2) + 16;
		int addr_offset = BITS(0, 6);

		if (IA == 0xF){ 
			addr = c54x_memory_read(core, (core->pc + 1));
			core->pc += 1;
		}else if (IA == 0x9){ 
			addr = fields->mmr;
		}else if (IA == 0x8){ 
			READ_MMR(core, ARx, addr);
		}else if (I == 0x0){ 
			READ_MMR(core, CPU_SP, addr);
			addr = addr + addr_offset;
		}else {
			NOT_IMP;
		}

		lk = c54x_memory_read(core, (core->pc + 1)); //lk
		src = c54x_memory_read(core, addr);//read from smem

		//READ_MMR(core, mmr, dst); 
		dst = src & lk;

		c54x_memory_write(core, addr, dst);//write value to smem
		//c54x_memory_write(core, mmr, dst); 
	}
	core->pc += TWO_WORD;
}
static void exec_ld_lk_shift16_dst(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int D = fields->D;
		int s;
		uint64_t result;

		uint32_t lk = c54x_memory_read(core, (core->pc + 1)); //#lk
		result = lk << 16;
		s = (result >> 31) & 0x1;
		if (s)
			result |= 0xFF00000000;

		//SIGN_EXTEND(result, 32);

		WRITE_ACC_REG(core, result, D); //write result to mmr acc_a or acc_b register
	}
	core->pc += TWO_WORD;
}

static void exec_or_lk_shft_src_dst(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int S = fields->S;
		int D = fields->D;
		int shft = fields->shft;
		uint64_t src, dst;

		if (S)
			src = core->acc_b;
		else 
			src = core->acc_a;
		uint32_t lk = c54x_memory_read(core, (core->pc + 1)); //#lk
		lk <<= shft;

		dst = lk | src;

		WRITE_ACC_REG(core, dst, D);
	}
	core->pc += TWO_WORD;
}
static void exec_add_lk_shft_src_dst(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int S = fields->S;
		int D = fields->D;
		int shft = fields->shft;
		int sxm = bit(core->MMR_BUFFER[CPU_ST1], 8);
		int ovm = bit(core->MMR_BUFFER[CPU_ST1], 9);
		uint64_t src, dst;

		if (S)
			src = core->acc_b;
		else 
			src = core->acc_a;

		uint32_t lk = c54x_memory_read(core, (core->pc + 1)); //#lk
		lk <<= shft;

		dst = (lk + src) & 0xFFFFFFFFFF;
		int x = (src >> 31) & 0x1;
		int z = (dst >> 31) & 0x1;

		//Overflow condition
		if (dst == OVERFLOW){
			if (D)
				SET(core->MMR_BUFFER[CPU_ST0], OVB);
			else 
				SET(core->MMR_BUFFER[CPU_ST0], OVA);
		}

		if (dst == 0xFF80000000){
			if (ovm)
				dst = 0x7FFFFFFF;
			else 
				dst = 0x80000000;
		}

		WRITE_ACC_REG(core, dst, D);
	}

	core->pc += TWO_WORD;
}
static void exec_bc(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int Z = fields->Z; //check delay  
		int cond = fields->cond;

		int flag = check_cond(core, cond);
		if (flag){
			uint16_t target_addr = c54x_memory_read(core, (core->pc + 1));

			if (Z){
					//dealy 2 cycles
				core->delay_enable = 1;
				core->pc += TWO_WORD;
				core->target_addr = target_addr;
				core->delay_slot_cycle = 2;
			}else 
				core->pc = target_addr;
		}else 
			core->pc += TWO_WORD;
	}else 
		core->pc += TWO_WORD;
}
static void exec_b(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int Z = fields->Z; //check delay  
		uint16_t target_addr = c54x_memory_read(core, (core->pc + 1));

		if (Z){
					//dealy 2 cycles
			core->delay_enable = 1;
			core->pc += TWO_WORD;
			core->target_addr = target_addr;
			core->delay_slot_cycle = 2;
		}else 
			core->pc = target_addr;
	}else 
		core->pc += TWO_WORD;
}
static void exec_reada(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int IA = fields->IA;
		int paddr = core->acc_a & 0xFFFF;
		int mmr, value, smem_addr;

		if (IA == 0xF){
			core->pc += 1;
			mmr = c54x_memory_read(core, core->pc);
		} else if (IA == 0x9){
			mmr = fields->mmr;
		} else {
			NOT_IMP;	
		}
	
		if (core->rptc == -1){ //repeat count = 0	
			value = c54x_memory_read(core, paddr);
			c54x_memory_write(core, mmr, value);
		}else { //repeat count != 0
			do{
				value = c54x_memory_read(core, paddr);
				smem_addr = c54x_memory_read(core, mmr);
				//READ_MMR(core, mmr, smem_addr);
				c54x_memory_write(core, smem_addr, value);
				c54x_memory_write(core, mmr, smem_addr + 1);
				paddr++;
				if (core->rptc == -1)
					break;
			}while(core->rptc--);
		}
	}
	core->pc += ONE_WORD;
}
static void exec_banz(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int Z = fields->Z;
		int IA = fields->IA;
		int op = BITS(3, 3);
		int ARx = BITS(0, 2) + 16;
		uint16_t value, off_val, result;

		READ_MMR(core, ARx, value);

		if (IA == 0xE){
			core->pc += 1;
			off_val = c54x_memory_read(core, core->pc);
			if ((off_val >> 15) & 0x1){
				off_val = 0xFFFF - off_val + 1;
				value = value - off_val;
			}else 
				value = value + off_val;
		}else if (IA == 0x8){
			if (op)
				result = value - 1;
			else 
				result = value;
			c54x_memory_write(core, ARx, result);
		}else {
			NOT_IMP;
		}

		uint16_t target_addr = c54x_memory_read(core, core->pc + 1);
		if (value != 0){
			if (Z){
				//delay 2 cycles
				core->delay_enable = 1;
				core->pc += TWO_WORD;
				core->target_addr = target_addr;
				core->delay_slot_cycle = 2;
			}else 
				core->pc = target_addr;
		}else
			core->pc += TWO_WORD;
	}else 
		core->pc += TWO_WORD;
}

static void exec_rpt(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int op = BITS(8, 15);
		int IA = fields->IA;
		int mmr;

		if (op == 0x47){
			if (IA == 0xF){
				core->pc += 1;
				mmr = c54x_memory_read(core, core->pc);
			}else if (IA == 0x9){ 
				mmr = fields->mmr;
			}else {
				NOT_IMP;
			}

			core->rptc = c54x_memory_read(core, mmr);
		}else if (op == 0xec){
			int K = fields->K;
			core->rptc = K;
		
		}else if (op == 0xf0){
			core->pc += 1;
			int lk = c54x_memory_read(core, core->pc);
			core->rptc = lk;
		}else {
			NOT_IMP;
		}
	}
	core->pc += ONE_WORD;
}
static void exec_add_smem_src(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int S = fields->D;
		int IA = fields->IA;
		uint64_t result, src;
		int mmr, value, x, z;

		if (IA == 0xF){
		    mmr = c54x_memory_read(core, core->pc + 1);
	    	core->pc += 1;
		} else if (IA == 0x9){ 
	    	mmr = fields->mmr;
		}else {
			NOT_IMP;
		}

		//READ_MMR(core, mmr, value);
		value = c54x_memory_read(core, mmr);
		if (S)
			src = core->acc_b;
		else 
			src = core->acc_a;

		result = src + value; 
		WRITE_ACC_REG(core, result, S);

//		uint64_t x = bit(src, 31);
//		uint64_t z = bit(result, 31);
		x = (src >> 31) & 0x1;
		z = (result >> 31) & 0x1;

		//Overflow condition
		if (result == OVERFLOW){
			if (S)
				SET(core->MMR_BUFFER[CPU_ST0], OVB);
			else 
				SET(core->MMR_BUFFER[CPU_ST0], OVA);
		}

		//Carry condition
		if (x && z == 0)
			SET(core->MMR_BUFFER[CPU_ST0], C);
		else 
			CLEAR(core->MMR_BUFFER[CPU_ST0], C);
	}

	core->pc += ONE_WORD;
}

static void exec_frame(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		uint8_t K = fields->K;
		int sp;
		int cpl = bit(core->MMR_BUFFER[CPU_ST1], CPL);

		READ_MMR(core, CPU_SP, sp);
		if ((K >> 7) & 0x1){
			K = 0xFF - K + 1;
			sp -= K; 
		}else 
			sp += K;

		c54x_memory_write(core, CPU_SP, sp);
	}
	core->pc += ONE_WORD;
}

static void exec_fcall(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int Z = fields->Z;
		int tos, xpc;
		int pmad_0_22 = BITS(0, 6);
		int pmad_0_15 = c54x_memory_read(core, core->pc + 1);

		if (Z){
			//dealy 2 cycles
			core->delay_enable = 1;
			core->MMR_BUFFER[CPU_SP] -= 1;
			tos = core->pc + 4; //PC
			c54x_memory_write(core, core->MMR_BUFFER[CPU_SP], tos);
			core->MMR_BUFFER[CPU_SP] -= 1;
			xpc = pmad_0_22; //XPC
			c54x_memory_write(core, core->MMR_BUFFER[CPU_SP], xpc);

			core->pc += TWO_WORD;
			core->target_addr = pmad_0_15;
			core->delay_slot_cycle = 2;
		}else{ 
			core->MMR_BUFFER[CPU_SP] -= 1;
			tos = core->pc + 2; //PC
			c54x_memory_write(core, core->MMR_BUFFER[CPU_SP], tos);
			core->MMR_BUFFER[CPU_SP] -= 1;
			xpc = pmad_0_22; //XPC
			core->pc = pmad_0_15;
		}
	}else 
		core->pc += TWO_WORD;
}

static void exec_pshm(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
 		core->MMR_BUFFER[CPU_SP] -= 1;
		int mmr = fields->mmr;
		uint16_t value;

		READ_MMR(core, mmr, value);
		
		c54x_memory_write(core, core->MMR_BUFFER[CPU_SP], value);
	}

	core->pc += ONE_WORD;	
}
static void exec_ld_k_dst(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		uint64_t K = fields->K; 
		int D = fields->D;

		WRITE_ACC_REG(core, K, D);
	}
		core->pc += ONE_WORD;
}
static void exec_stl_src_smem(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int S = fields->D;
		int IA = fields->IA;
		int I = fields->I;
		int addr, src, off_val;
		int addr_offset = BITS(0, 6);
		int mmr = BITS(0, 2) + 16;

		if (I == 0x0){
			READ_MMR(core, CPU_SP, addr);
			addr = addr + addr_offset;
		}else if (IA == 0xF){ 
			addr = c54x_memory_read(core, core->pc + 1);
			core->pc += 1;
		}else if (IA == 0x8){ 
			READ_MMR(core, mmr, addr);
		}else if (IA == 0xE){
			core->pc += 1;
			off_val = c54x_memory_read(core, core->pc);
			READ_MMR(core, mmr, addr);
			if ((off_val >> 15) & 0x1){
				off_val = 0xFFFF - off_val + 1;
				addr -= off_val;
			}else 
				addr += off_val;
		}else if (IA == 0x9){
			mmr = fields->mmr;
			READ_MMR(core, mmr, addr);
			c54x_memory_write(core, mmr, addr + 1);
		}else {
			NOT_IMP;
		}

		if (S)
				src = core->acc_b & 0xFFFF;
		else 
				src = core->acc_a & 0xFFFF;

		c54x_memory_write(core, addr, src);
	}
	core->pc += ONE_WORD;
}

static void exec_ld_lk_shft_dst(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int D = fields->D;
		int shft = fields->shft;
		uint64_t result;

		uint32_t lk = c54x_memory_read(core, (core->pc + 1)); //#lk
		result = lk << shft;
		//SIGN_EXTEND(result, 16+shft);
	//	if ((result >> 15) & 0x1)
	//		result |= 0xFFFFFF0000;

		WRITE_ACC_REG(core, result, D); //write result to mmr acc_a or acc_b register
	}
	core->pc += TWO_WORD;
}
static void exec_nop(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){

		core->pc += ONE_WORD;
}
static void exec_stlm_src_mmr(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int I = fields->I;
		int S = fields->D;
		int mmr, src;

		if (I){
			NOT_IMP;
		}else {
			mmr = fields->mmr;
		}

		if (S)
				src = core->acc_b & 0xFFFF;
		else 
				src = core->acc_a & 0xFFFF;

		c54x_memory_write(core, mmr, src);
	}
		core->pc += ONE_WORD;	
}
static void exec_ld_smem_dst(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int IA = fields->IA;
		int I = fields->I;
		int D = fields->D;
		int ARx = BITS(0, 2) + 16;
		int addr_offset = BITS(0, 6); 
		int smem, mmr, off_val;
		uint64_t dst;

		if (IA == 0x8){
			READ_MMR(core, ARx, smem);	
		}else if (IA == 0x9){
			READ_MMR(core, ARx, smem);	
			c54x_memory_write(core, ARx, smem + 1);
		}else if (I == 0x0){
			READ_MMR(core, CPU_SP, smem);
			smem += addr_offset;
		}else if (IA == 0xF){
			core->pc += 1;	
			//mmr = c54x_memory_read(core, core->pc);
			smem = c54x_memory_read(core, core->pc);
			//READ_MMR(core, mmr, smem);
		}else if (IA == 0xE){
			core->pc += 1;
			READ_MMR(core, ARx, smem);	
			off_val = c54x_memory_read(core, core->pc);
			if ((off_val >> 15) & 0x1){
				off_val = 0xFFFF - off_val + 1;
				smem -= off_val;
			}else 
				smem += off_val;
		}else {
			NOT_IMP;
		}

		dst = c54x_memory_read(core, smem);

		WRITE_ACC_REG(core, dst, D);
	}
		core->pc += ONE_WORD;
}
static void exec_mvkd_dmad_smem(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int dmad, src, smem;
		int IA = fields->IA;
		int I = fields->I;
		int addr_offset = BITS(0, 6);
		int ARx = BITS(0, 2) + 16;

		if (I == 0){
			if (core->rptc == -1){
				READ_MMR(core, CPU_SP, smem);	
				smem += addr_offset;
			}else { //repeat count != 0
				NOT_IMP;
			}
		}else if (IA == 0x8){
			READ_MMR(core, ARx, smem);
		}else {
			NOT_IMP;
		}
	    dmad = c54x_memory_read(core, core->pc + 1); 
		src = c54x_memory_read(core, dmad);

		c54x_memory_write(core, smem, src);
	}

	core->pc += TWO_WORD;
}
static void exec_popm_mmr(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int addr, value;
		int mmr = fields->mmr;
		READ_MMR(core, CPU_SP, addr);

		value = c54x_memory_read(core, addr);
 		core->MMR_BUFFER[CPU_SP] += 1;
		c54x_memory_write(core, mmr, value);
	}
		core->pc += ONE_WORD;
}
static void exec_fret(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int Z = fields->Z; 
		int xpc, pc;

		xpc = c54x_memory_read(core, core->MMR_BUFFER[CPU_SP]);
		core->MMR_BUFFER[CPU_SP] += 1;
		pc = c54x_memory_read(core, core->MMR_BUFFER[CPU_SP]);
		core->MMR_BUFFER[CPU_SP] += 1;

		if (Z){
				//delay 4 cycles
			core->pc += ONE_WORD;
			core->delay_enable = 1;
			core->target_addr = pc;
			core->delay_slot_cycle = 4;
		}else {
			core->pc = pc;		
		}
	}else 
		core->pc += ONE_WORD;
}
static void exec_mvdk_smem_dmad(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int dmad, src, smem, offset_addr;
		int I = fields->I;
		int IA = fields->IA;
		int addr_offset = BITS(0, 6);
		int mmr = BITS(0, 2) + 16; 

		if (I == 0){
			if (core->rptc == -1){
				READ_MMR(core, CPU_SP, smem);	
				smem += addr_offset;
				src = c54x_memory_read(core, smem);
			}else { //repeat count != 0
				NOT_IMP;
			}
		}else if (IA == 0x8){
			READ_MMR(core, mmr, smem);
			src = c54x_memory_read(core, smem);
		}else if (IA == 0xE){
			core->pc += 1;
			READ_MMR(core, mmr, smem);
			offset_addr = c54x_memory_read(core, core->pc);
			if ((offset_addr >> 15) & 0x1){
				offset_addr = 0xFFFF - offset_addr + 1;
				smem -= offset_addr;
			}else 
				smem += offset_addr;

			src = c54x_memory_read(core, smem);
		}else {
			NOT_IMP;
		}

	    dmad = c54x_memory_read(core, core->pc + 1); 
		if (dmad >= CPU_IMR && dmad <= CPU_TEST5){
			c54x_memory_write(core, dmad, src);
		}else 
			c54x_memory_write(core, dmad, src);
	}

	core->pc += TWO_WORD;
}
static void exec_st_lk_smem(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int IA = fields->IA;
		int I = fields->I;
		int mmr = BITS(0, 2) + 16;
		int addr_offset = BITS(0, 6); 
		int addr, lk, off_val;

		if (IA == 0x8){
			READ_MMR(core, mmr, addr);
		}else if (I == 0x0){
			READ_MMR(core, CPU_SP, addr);
			addr += addr_offset;
		}else if (IA == 0xE){
			READ_MMR(core, mmr, addr);
			core->pc += 1;
			off_val = c54x_memory_read(core, core->pc);
			if ((off_val >> 15) & 0x1){
				off_val = 0xFFFF - off_val + 1;
				addr -= off_val;
			}else 
				addr += off_val;
		}else {
			NOT_IMP;
		}

		lk = c54x_memory_read(core, core->pc + 1);
		c54x_memory_write(core, addr, lk);
	}
		core->pc += TWO_WORD;
}
static void exec_or_smem_src(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int IA = fields->IA;
		int S = fields->D;
		int ARx = BITS(0, 2) + 16;
		int smem, src, offset_addr;
		uint64_t dst;

		if (S)
			src = core->acc_b & 0xFFFF;
		else 
			src = core->acc_a & 0xFFFF;

		if (IA == 0xF){
			smem = c54x_memory_read(core, core->pc + 1);
			core->pc += 1;
		}else if (IA == 0xE){
			READ_MMR(core, ARx, smem);
			offset_addr = c54x_memory_read(core, core->pc + 1);
			if ((offset_addr >> 15) & 0x1){
				offset_addr = 0xFFFF - offset_addr + 1;
				smem -= offset_addr;
			}else 
				smem += offset_addr;

			core->pc += 1;
		}else if (IA == 0x8){
			READ_MMR(core, ARx, smem);	
		}else {
			NOT_IMP;	
		}

		dst = c54x_memory_read(core, smem);
		dst |= src;
		WRITE_ACC_REG(core, dst, S);
	}

	core->pc += ONE_WORD;
}
static void exec_cmpm_smem_lk(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int IA = fields->IA;
		int I = fields->I;
		int ARx = BITS(0, 2) + 16;
		int addr_offset = BITS(0, 6); 
		int smem, src;
		int lk = c54x_memory_read(core, core->pc + 1);
		
		if (IA == 0x8){
			READ_MMR(core, ARx, smem);	
		}else if (I == 0x0){
			READ_MMR(core, CPU_SP, smem);
			smem += addr_offset;
		}else {
			NOT_IMP;
		}

		src = c54x_memory_read(core, smem);
		if (src == lk)
			SET(core->MMR_BUFFER[CPU_ST0], TC);
		else 
			CLEAR(core->MMR_BUFFER[CPU_ST0], TC);
	}

	core->pc += TWO_WORD;
}
static void exec_ldm_mmr_dst(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int D = fields->D;
		int mmr = fields->mmr;
		uint64_t src;
		READ_MMR(core, mmr, src);

		WRITE_ACC_REG(core, src, D);
	}
	core->pc += ONE_WORD;
}
static void exec_dst_src_lmem(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int S = fields->D;
		int IA = fields->IA;
		int I = fields->I;
		int addr_offset = BITS(0, 6);
		int lmem, mmr;
		uint64_t src;

		if (S)
			src = core->acc_b; 
		else 
			src = core->acc_a; 

		if (I == 0x0){
			READ_MMR(core, CPU_SP, lmem);	
			lmem += addr_offset;
		}else if (IA == 0x9){
			mmr = fields->mmr;
			READ_MMR(core, mmr, lmem);
			c54x_memory_write(core, mmr, lmem + 2);
		}else {
			NOT_IMP;
		}

		c54x_memory_write(core, lmem, src >> 16);
		c54x_memory_write(core, lmem + 1, src & 0xFFFF);
	}

		core->pc += ONE_WORD;
}
static void exec_dld_lmem_dst(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int D = fields->D;
		int I = fields->I;
		int IA = fields->IA;
		int addr_offset = fields->mmr;
		int addr, lmem_lsb, lmem_msb;
		uint64_t result;
		int c16 = bit(core->MMR_BUFFER[CPU_ST1], C16);

		if (I == 0x0){
			READ_MMR(core, CPU_SP, addr);
			addr += addr_offset;	
		}else if (IA == 0xF){
			core->pc += 1;
			addr = c54x_memory_read(core, core->pc);
		}else {
			NOT_IMP;
		}

		lmem_lsb = c54x_memory_read(core, addr);
		lmem_msb = c54x_memory_read(core, addr + 1);

		if (c16){
			SIGN_EXTEND(lmem_lsb, 16);
			result = (lmem_lsb << 16) | lmem_msb;
		}else {
			result = (lmem_lsb << 16) | lmem_msb;
		}

		WRITE_ACC_REG(core, result, D);
	}
		core->pc += ONE_WORD;
}
static void exec_sub_lk_shft_src_dst(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int S = fields->S;
		int D = fields->D;
		int shft = fields->shft;
		int sxm = bit(core->MMR_BUFFER[CPU_ST1], 8);
		uint64_t src, dst;

		if (S)
			src = core->acc_b;
		else 
			src = core->acc_a;

		uint32_t lk = c54x_memory_read(core, (core->pc + 1)); //#lk
		lk <<= shft;

		dst = (src - lk) & 0xFFFFFFFFFF;
		//uint32_t x = bit(src, 31);
		//uint32_t z = bit(dst, 31);
		int x = (src >> 31) & 0x1;
		int z = (dst >> 31) & 0x1;

		WRITE_ACC_REG(core, dst, D);
		//Overflow condition
		if (dst == OVERFLOW){
			if (D)
				SET(core->MMR_BUFFER[CPU_ST0], OVB);
			else 
				SET(core->MMR_BUFFER[CPU_ST0], OVA);
		}

		//Carry condition
		if (x == 0 && z)
			CLEAR(core->MMR_BUFFER[CPU_ST0], C);
		else 
			SET(core->MMR_BUFFER[CPU_ST0], C);
	}

	core->pc += TWO_WORD;
}
static void exec_fcala_src(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int Z = fields->Z; //check delay  
		int S = fields->D;
		int tos, xpc;
		uint16_t target_addr;
		
		if (S){
				target_addr = core->acc_b & 0xFFFF;
				xpc = (core->acc_b >> 16) & 0x7F;
		}else{
				target_addr = core->acc_a & 0xFFFF;
				xpc = (core->acc_a >> 16) & 0x7F;
		} 
		if (Z){
			//dealy 4 cycles
			core->delay_enable = 1;
			core->MMR_BUFFER[CPU_SP] -= 1;
			tos = core->pc + 3; //PC
			c54x_memory_write(core, core->MMR_BUFFER[CPU_SP], tos);
			core->MMR_BUFFER[CPU_SP] -= 1;
			c54x_memory_write(core, core->MMR_BUFFER[CPU_SP], xpc);
			core->target_addr = target_addr;
			core->delay_slot_cycle = 4;
			core->pc += ONE_WORD;
		}else { 
			core->MMR_BUFFER[CPU_SP] -= 1;
			tos = core->pc + 1; //PC
			c54x_memory_write(core, core->MMR_BUFFER[CPU_SP], tos);
			core->MMR_BUFFER[CPU_SP] -= 1;
			c54x_memory_write(core, core->MMR_BUFFER[CPU_SP], xpc);
			core->pc = target_addr;
		}
	}
}
static void exec_mvdm_dmad_mmr(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int mmr = fields->mmr;
		int value, smem_addr;
		int paddr = c54x_memory_read(core, core->pc + 1);

		if (core->rptc == -1){ //repeat count = 0	
			value = c54x_memory_read(core, paddr);
			c54x_memory_write(core, mmr, value);
		}else { //repeat count != 0
			do{
				value = c54x_memory_read(core, paddr);
				READ_MMR(core, mmr, smem_addr);
				c54x_memory_write(core, smem_addr, value);
				c54x_memory_write(core, mmr, smem_addr + 1);
				paddr++;
				if (core->rptc == -1)
					break;
			}while(core->rptc--);
		}
	}

	core->pc += TWO_WORD;
}
static void exec_xc_n_cond(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
		int N = fields->S;
		int cond = fields->cond;
		int nops, flag;

		if (N)
				core->pend_nops = 2;
		else 
				core->pend_nops = 1;
		flag = check_cond(core, cond);

		if (flag == 0){
			core->pend_nops_flag = 0;	
		}
		core->pc += ONE_WORD;
}
static void exec_sfta_src_shift_dst(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int shift = fields->shift;
		int S = fields->S;
		int D = fields->D;
		uint64_t c, src, dst;
		int sxm = BITS(core->MMR_BUFFER[CPU_ST1], SXM);

		if (S)
				src = core->acc_b;
		else 
				src = core->acc_a;

		if (shift > 0xF){
			shift -= 0x20;
			c = bit(src, -shift - 1); 
			if (c)
				SET(core->MMR_BUFFER[CPU_ST0], C);
			else 
				CLEAR(core->MMR_BUFFER[CPU_ST0], C);

			dst = src >> -shift;
		}else {
			c = bit(src, 39 - shift); 
			if (c)
				SET(core->MMR_BUFFER[CPU_ST0], C);
			else 
				CLEAR(core->MMR_BUFFER[CPU_ST0], C);

			dst = src << shift;
		}

		WRITE_ACC_REG(core, dst, D);
	}

	core->pc += ONE_WORD; 
}
static void exec_and_lk_shft_src_dst(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int S = fields->S;
		int D = fields->D;
		int shft = fields->shft;
		uint64_t src, dst;

		if (S)
			src = core->acc_b;
		else 
			src = core->acc_a;

		int lk = c54x_memory_read(core, core->pc + 1); 
		lk <<= shft;

		dst = src & lk;
		WRITE_ACC_REG(core, dst, D);
	}
	core->pc += TWO_WORD;
}
static void exec_sub_smem_src(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int S = fields->D;
		int IA = fields->IA;
		int mmr, x, y, z;
		uint64_t src, smem;

		if (S)
			src = core->acc_b;
		else 
			src = core->acc_a;

		if (IA == 0xF){
			core->pc += 1;
			mmr = c54x_memory_read(core, core->pc);
		}else{
			NOT_IMP;	
		}

		if (mmr == 0xb){
			smem = core->acc_b;
		}else if (mmr == 0x8){
			smem = core->acc_a;
		}else{
		//	READ_MMR(core, mmr, smem);
			smem = c54x_memory_read(core, mmr);
		}

		x = (smem >> 31) & 0x1;
		y = (src >> 31) & 0x1;
		src -= smem;
		z = (src >> 31) & 0x1;
		WRITE_ACC_REG(core, src, S);

		//Overflow condition
		if (src == OVERFLOW){
			if (S)
				SET(core->MMR_BUFFER[CPU_ST0], OVB);
			else 
				SET(core->MMR_BUFFER[CPU_ST0], OVA);
		}

		//Carry condition
		if (((!x && y) || (!y && x)) && z == 0)
			CLEAR(core->MMR_BUFFER[CPU_ST0], C);

	}
	core->pc += ONE_WORD;
}
static void exec_ld_smem_asm(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int IA = fields->IA;
		int mmr, smem, asm_val, st1;

		if (IA == 0xF){
			core->pc += 1;
			mmr = c54x_memory_read(core, core->pc);
		}else if (IA == 0x8){
			mmr = BITS(0, 2) + 16;
		}else {
			NOT_IMP;
		}

		smem = c54x_memory_read(core, mmr);
		asm_val = smem & 0x1F;

		st1 = (core->MMR_BUFFER[CPU_ST1] & 0xFFE0) | asm_val;
		c54x_memory_write(core, CPU_ST1, st1);
	}
	core->pc += ONE_WORD;
}
static void exec_ld_src_asm_dst(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int S = fields->S;
		int D = fields->D;
		uint64_t src, dst;
		int asm_val = core->MMR_BUFFER[CPU_ST1] & 0x1F;

		if (S)
			src = core->acc_b;
		else 
			src = core->acc_a;

		dst = src << asm_val;
		//Overflow condition
		if (dst == OVERFLOW){
			if (D)
				SET(core->MMR_BUFFER[CPU_ST0], OVB);
			else 
				SET(core->MMR_BUFFER[CPU_ST0], OVA);
		}

		WRITE_ACC_REG(core, dst, D);
	}
	core->pc += ONE_WORD;
}
static void exec_and_smem_src(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int S = fields->D;
		int IA = fields->IA;
		int mmr, off_val;
		uint64_t src, smem;
		int ARx = BITS(0, 2) + 16;

		if (S)
			src = core->acc_b;
		else 
			src = core->acc_a;

		if (IA == 0xF){
			core->pc += 1;
			mmr = c54x_memory_read(core, core->pc);
		}else if (IA == 0xE){
			core->pc += 1;
			off_val = c54x_memory_read(core, core->pc);
			READ_MMR(core, ARx, mmr);
			if ((off_val >> 15) & 0x1){
				off_val = 0xFFFF - off_val + 1;
				mmr -= off_val;
			}else 
				mmr += off_val;
		}else if (IA == 0x8){
			READ_MMR(core, ARx, mmr);
		}else {
			NOT_IMP;	
		}

		if (mmr == 0xb){
			smem = core->acc_b;
		}else if (mmr == 0x8){
			smem = core->acc_a;
		}else
			smem = c54x_memory_read(core, mmr);

		src &= smem;
		WRITE_ACC_REG(core, src, S);
	}
	core->pc += ONE_WORD;
}
static void exec_mac_smem_src(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int S = fields->D;
		int R = fields->S;
		int IA = fields->IA;
		int mmr, smem;
		uint64_t src, dst;

		if (S)
			src = core->acc_b;
		else 
			src = core->acc_a;

		if (IA == 0xF){
			core->pc += 1;
			mmr = c54x_memory_read(core, core->pc);
		}else {
			NOT_IMP;
		}

	//	READ_MMR(core, mmr, smem);
		smem = c54x_memory_read(core, mmr);

		dst = src + smem * core->MMR_BUFFER[CPU_TREG];
		WRITE_ACC_REG(core, dst, S);

		//Overflow condition
		if (dst == OVERFLOW){
			if (S)
				SET(core->MMR_BUFFER[CPU_ST0], OVB);
			else 
				SET(core->MMR_BUFFER[CPU_ST0], OVA);
		}
	}
	core->pc += ONE_WORD;
}
static void exec_bitf_smem_lk(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int IA = fields->IA;
		int lk, smem, mmr;

		if (IA == 0xF){
			core->pc += 1;
			mmr = c54x_memory_read(core, core->pc);
		}else {
			NOT_IMP;
		}

	//	READ_MMR(core, mmr, smem);
		smem = c54x_memory_read(core, mmr);
		lk = c54x_memory_read(core, core->pc + 1);

		if (smem & lk)
			SET(core->MMR_BUFFER[CPU_ST0], TC);
		else 
			CLEAR(core->MMR_BUFFER[CPU_ST0], TC);
		
	}
	core->pc += TWO_WORD;	
}
static void exec_cmpr_cc_arx(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int cc = BITS(8, 9);
		int ARx = BITS(0, 2);
		int flag = 0, src, dst, x;

		src = core->MMR_BUFFER[CPU_AR0];
		READ_MMR(core, ARx + 16, dst);
		x = ((dst - src) >> 31) & 0x1;
		switch (cc){
			case 0:
				if (dst == src)
					flag = 1;
				break;
			case 1:
				if (x)
					flag = 1;
				break;
			case 2:
				if (x == 0)
					flag = 1;
				break;
			case 3:
				if (src != dst)
					flag = 1;
				break;
			default:
				break;
		}

		if (flag)
			SET(core->MMR_BUFFER[CPU_ST0], TC);
		else 
			CLEAR(core->MMR_BUFFER[CPU_ST0], TC);
	}
	core->pc += ONE_WORD;	
}
static void exec_orm_lk_smem(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int IA = fields->IA;
		int ARx = BITS(0, 2) + 16;
		int addr, smem, dst, lk;

		if (IA == 0x8){
			READ_MMR(core, ARx, addr);	
		}else if (IA == 0xF){
			core->pc += 1;
			addr = c54x_memory_read(core, core->pc);
		}else{
			NOT_IMP;
		}

		smem = c54x_memory_read(core, addr);
		lk = c54x_memory_read(core, core->pc + 1);
		dst = lk | smem;

		c54x_memory_write(core, addr, dst);
	}
	core->pc += TWO_WORD;	
}
static void exec_mpy_smem_lk_dst(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int D = fields->D;
		int IA = fields->IA;
		int ARx = BITS(0, 2) + 16;
		int addr, src, lk;
		uint64_t dst;

		if (IA == 0x8){ 
			READ_MMR(core, ARx, addr);
		}else {
			NOT_IMP;
		}

		lk = c54x_memory_read(core, (core->pc + 1)); //lk
		src = c54x_memory_read(core, addr);//read from smem
		core->MMR_BUFFER[CPU_TREG] = src;

		dst = src * lk;
		WRITE_ACC_REG(core, dst, D);

		//Overflow condition
		if (dst == OVERFLOW){
			if (D)
				SET(core->MMR_BUFFER[CPU_ST0], OVB);
			else 
				SET(core->MMR_BUFFER[CPU_ST0], OVA);
		}
	}
	core->pc += TWO_WORD;
}
static void exec_ldu_smem_dst(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int D = fields->D;
		int IA = fields->IA;
		int ARx = BITS(0, 2) + 16;
		int addr;
		uint64_t smem;

		if (IA == 0x8){
			READ_MMR(core, ARx, addr);	
		}else {
			NOT_IMP;
		}

		smem = c54x_memory_read(core, addr);
		WRITE_ACC_REG(core, smem, D);
	}
	core->pc += 1;	
}
static void exec_sftl_src_shift_dst(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int shift = fields->shift;
		int S = fields->S;
		int D = fields->D;
		uint64_t c, src, dst;
		int sxm = BITS(core->MMR_BUFFER[CPU_ST1], SXM);

		if (S)
				src = core->acc_b;
		else 
				src = core->acc_a;

		if (shift > 0xF){
			shift -= 0x20;
			c = bit(src, -shift - 1); 
			if (c)
				SET(core->MMR_BUFFER[CPU_ST0], C);
			else 
				CLEAR(core->MMR_BUFFER[CPU_ST0], C);

			dst = src >> -shift;
		}else if (shift == 0){
			CLEAR(core->MMR_BUFFER[CPU_ST0], C);
		}else {
			c = bit(src, 31 - (shift - 1)); 
			if (c)
				SET(core->MMR_BUFFER[CPU_ST0], C);
			else 
				CLEAR(core->MMR_BUFFER[CPU_ST0], C);

			dst = src << shift;
		}

		WRITE_ACC_REG(core, dst, D);
	}

	core->pc += ONE_WORD; 
}
static void exec_or_src_shift_dst(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
		if (core->pend_nops_flag){
		int shift = fields->shift;
		int S = fields->S;
		int D = fields->D;
		uint64_t c, src, dst;
		int sxm = BITS(core->MMR_BUFFER[CPU_ST1], SXM);

		if (S)
			src = core->acc_b;
		else 
			src = core->acc_a;

		if (D)
			dst = core->acc_b;
		else 
			dst = core->acc_a;

		if (shift > 0xF){
			shift -= 0x20;
			dst |= (src >> -shift);
		}else {
			dst |= (src << shift);
		}

		WRITE_ACC_REG(core, dst, D);
	}

	core->pc += ONE_WORD; 
}
static void exec_stl_src_shift_smem(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int I = fields->I;
		int sxm = BITS(core->MMR_BUFFER[CPU_ST1], SXM);
		int addr_offset = BITS(0, 6);
		uint64_t src;

		int op = c54x_memory_read(core, core->pc + 1);
		int S = (op >> 8) & 0x1;
		int shift = op & 0x1f;
		int addr, smem;

		if (S)
			src = core->acc_b;
		else 
			src = core->acc_a;

		if (shift > 0xF){
			shift -= 0x20;
			src = (src >> -shift);
		}else {
			src = (src << shift);
		}

		if (I == 0x0){
			READ_MMR(core, CPU_SP, addr);
			addr = addr + addr_offset;
		}else {
			NOT_IMP;
		}
		
		smem = src & 0xFFFF;
		c54x_memory_write(core, addr, smem);
	}

	core->pc += TWO_WORD; 
}
static void exec_mvmm_mmrx_mmry(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int mmrx = BITS(4, 7);
		int mmry = BITS(0, 3);
		int src;

		READ_MMR(core, mmrx + 16, src);
		c54x_memory_write(core, mmry + 16, src);
	}

	core->pc += ONE_WORD; 
}
static void exec_mar_smem(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int IA = fields->IA;
		int ARx = BITS(0, 2) + 16;
		int cmpt = bit(core->MMR_BUFFER[CPU_ST1], CMPT);
		int arp = (core->MMR_BUFFER[CPU_ST0] >> 13) & 0x7;
		int addr, off_val, dst, st0;

		if (IA == 0xE){
			READ_MMR(core, ARx, addr);
			core->pc += 1;
			off_val = c54x_memory_read(core, core->pc);
			if ((off_val >> 15) & 0x1){
				off_val = 0xFFFF - off_val + 1;
				addr -= off_val;
			}else 
				addr += off_val;
		}else {
			NOT_IMP;
		}

		if (cmpt){
			if (ARx == CPU_AR0)
			{
				c54x_memory_write(core, arp + 16, addr);	
			}else{
				c54x_memory_write(core, ARx, addr);	
				arp = ARx - 16;
				//SET_BIT(core->MMR_BUFFER[CPU_ST0], arp, ARP, 3);
				st0 = (core->MMR_BUFFER[CPU_ST0] & 0x1FFF) | arp;
				c54x_memory_write(core, CPU_ST0, st0);
			} 
		}else {
			c54x_memory_write(core, ARx, addr);	
		}
	}
	core->pc += ONE_WORD; 
}
static void exec_ld_src_shift_dst(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int shift = fields->shift;
		int S = fields->S;
		int D = fields->D;
		uint64_t c, src, dst;
		int sxm = BITS(core->MMR_BUFFER[CPU_ST1], SXM);
		int ovm = bit(core->MMR_BUFFER[CPU_ST1], 9);

		if (S)
			src = core->acc_b;
		else 
			src = core->acc_a;

		if (shift > 0xF){
			shift -= 0x20;
			dst = (src >> -shift);
		}else {
			dst = (src << shift);
		}

		WRITE_ACC_REG(core, dst, D);

		//Overflow condition
		if (dst == OVERFLOW){
			if (D)
				SET(core->MMR_BUFFER[CPU_ST0], OVB);
			else 
				SET(core->MMR_BUFFER[CPU_ST0], OVA);
		}
	}

	core->pc += ONE_WORD; 
}
static void exec_or_lk_shift16_src_dst(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int S = fields->S;
		int D = fields->D;
		int lk;
		uint64_t src, dst;
		if (S)
			src = core->acc_b;
		else 
			src = core->acc_a;

		lk = c54x_memory_read(core, core->pc + 1);
		lk <<= 16;

		dst = src | lk;
		WRITE_ACC_REG(core, dst, D);
	}

	core->pc += TWO_WORD;
}
static void exec_writa_smem(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int IA = fields->IA;
		int paddr = core->acc_a & 0xFFFF;
		int mmr, value, addr;

		if (IA == 0xF){
			core->pc += 1;
			mmr = c54x_memory_read(core, core->pc);
		} else if (IA == 0x9){
			mmr = fields->mmr;
		} else {
			NOT_IMP;	
		}
	
		if (core->rptc == -1){ //repeat count = 0	
			addr = c54x_memory_read(core, mmr);
			value = c54x_memory_read(core, addr);
			c54x_memory_write(core, paddr, value);
		}else { //repeat count != 0
			do{
				addr = c54x_memory_read(core, mmr);
				value = c54x_memory_read(core, addr);
				c54x_memory_write(core, paddr, value);
				c54x_memory_write(core, mmr, addr + 1);
			//	paddr++;
				if (core->rptc == -1)
					break;
			}while(core->rptc--);
		}
		
	}
	core->pc += ONE_WORD;
}
static void exec_neg_src_dst(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int S = fields->S;
		int D = fields->D;
		int ovm = bit(core->MMR_BUFFER[CPU_ST1], OVM);
		uint64_t src, dst;

		if (S)
			src = core->acc_b;
		else 
			src = core->acc_a;

		dst = -src;
		dst = SIGN_EXTEND_40BIT(dst);

		//Overflow condition
		if (dst == OVERFLOW){
			if (S)
				SET(core->MMR_BUFFER[CPU_ST0], OVB);
			else 
				SET(core->MMR_BUFFER[CPU_ST0], OVA);
		}

		//Carry condition
		if (dst == 0)
			SET(core->MMR_BUFFER[CPU_ST0], C);
		else 
			CLEAR(core->MMR_BUFFER[CPU_ST0], C);

		if (dst == 0xFF80000000){
			if (ovm)
				dst = 0x7FFFFFFF;
			else 
				dst = 0x80000000;
		}

		WRITE_ACC_REG(core, dst, D);
	}
	core->pc += ONE_WORD;
}
static void exec_mac_smem_lk_src_dst(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int S = fields->S;
		int D = fields->D;
		int IA = fields->IA;
		int ARx = BITS(0, 2) + 16;
		int ovm = bit(core->MMR_BUFFER[CPU_ST1], OVM);
		int addr, lk, smem;
		uint64_t src, dst;

		if (S)
			src = core->acc_b;
		else 
			src = core->acc_a;

		if (IA == 0x8){ 
			READ_MMR(core, ARx, addr);
		}else {
			NOT_IMP;
		}

		lk = c54x_memory_read(core, (core->pc + 1)); //lk
		smem = c54x_memory_read(core, addr);//read from smem
		
		dst = src + smem * lk; 
		//Overflow condition
		if (dst == OVERFLOW){
			if (S)
				SET(core->MMR_BUFFER[CPU_ST0], OVB);
			else 
				SET(core->MMR_BUFFER[CPU_ST0], OVA);
		}

		if (dst == 0xFF80000000){
			if (ovm)
				dst = 0x7FFFFFFF;
			else 
				dst = 0x80000000;
		}
		WRITE_ACC_REG(core, dst, D);
	}
	core->pc += TWO_WORD;
}
static void exec_frete(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int Z = fields->Z; //check delay  
		int xpc, pc;
		int sp = core->MMR_BUFFER[CPU_SP];

		xpc = c54x_memory_read(core, sp);
		c54x_memory_write(core, CPU_XPC, xpc);
		sp += 1;

		pc = c54x_memory_read(core, sp);
		sp += 1;
		
		c54x_memory_write(core, CPU_SP, sp);
		CLEAR(core->MMR_BUFFER[CPU_ST1], INTM);
		core->intr_flag = 0;

		if (Z){
			//dealy 4 cycles
			core->delay_enable = 1;
			core->pc += ONE_WORD;
			core->target_addr = pc;
			core->delay_slot_cycle = 4;
		}else {
			core->pc = pc;	
		}
	}
}
static void exec_cmpl_src_dst(c54x_core_t* core, uint16_t insn, opcode_fields_t* fields){
	if (core->pend_nops_flag){
		int S = fields->S;
		int D = fields->D;
		uint64_t src, dst;

		if (S)
			src = core->acc_b;
		else 
			src = core->acc_a;

		dst = 0xFFFFFFFFFF - src;
		WRITE_ACC_REG(core, dst, D);
	}
	core->pc += ONE_WORD;
}
insn_action_t insn_action[] = {
	exec_ssbx,
	exec_rsbx,
	exec_ld_k3_arp,
	exec_stm,
	exec_addm,
	exec_andm,
	exec_ld_lk_shift16_dst,
	exec_or_lk_shft_src_dst,
	exec_add_lk_shft_src_dst,
	exec_bc,
	exec_b,
	exec_reada,
	exec_banz,
	exec_rpt,
	exec_rpt,
	exec_rpt,
	exec_add_smem_src,
	exec_frame,
	exec_fcall,
	exec_pshm,
	exec_ld_k_dst,
	exec_stl_src_smem,
	exec_ld_lk_shft_dst,
	exec_nop,
	exec_stlm_src_mmr,
	exec_ld_smem_dst,
	exec_mvkd_dmad_smem,
	exec_popm_mmr,
	exec_fret,
	exec_mvdk_smem_dmad,
	exec_st_lk_smem,
	exec_or_smem_src,
	exec_cmpm_smem_lk,
	exec_ldm_mmr_dst,
	exec_dst_src_lmem,
	exec_dld_lmem_dst,
	exec_sub_lk_shft_src_dst,
	exec_fcala_src,
	exec_mvdm_dmad_mmr,
	exec_xc_n_cond,
	exec_sfta_src_shift_dst,
	exec_and_lk_shft_src_dst,
	exec_sub_smem_src,
	exec_ld_smem_asm,
	exec_ld_src_asm_dst,
	exec_and_smem_src,
	exec_mac_smem_src,
	exec_bitf_smem_lk,
	exec_cmpr_cc_arx,
	exec_orm_lk_smem,
	exec_mpy_smem_lk_dst,
	exec_ldu_smem_dst,
	exec_sftl_src_shift_dst,
	exec_or_src_shift_dst,
	exec_stl_src_shift_smem,
	exec_mvmm_mmrx_mmry,
	exec_mar_smem,
	exec_ld_src_shift_dst,
	exec_or_lk_shift16_src_dst,
	exec_writa_smem,
	exec_neg_src_dst,
	exec_mac_smem_lk_src_dst,
	exec_frete,
	exec_cmpl_src_dst,
	NULL,
};
const ISEITEM insn32_decode_table[] = {
	{"SSBX", 2, { 4, 8, 0x1b, 10, 15, 0x3d}, 1, 1, "class_1"},
	{"RSBX", 2, { 4, 8, 0xb, 10, 15, 0x3d}, 1, 1, "class_1"},
	{"LD_K3_ARP", 1, { 3, 15, 0x1e94}, 1, 1, "class_1"},
	{"STM", 1, { 8, 15, 0x77}, 2, 2, "class_12A_12B"},
	{"ADDM", 1, { 8, 15, 0x6b}, 2, 2, "class_18A_18B"},
	{"ANDM", 1, { 8, 15, 0x68}, 2, 2, "class_18A_18B"},
	{"LD_LK_SHIFT16_DST", 2, { 0, 7, 0x62, 9, 15, 0x78}, 2, 2, "class_2"},
	{"OR_LK_SHFT_SRC_DST", 2, { 4, 7, 0x4, 10, 15, 0x3c}, 2, 2, "class_2"},
	{"ADD_LK_SHFT_SRC_DST", 2, { 4, 7, 0x0, 10, 15, 0x3c}, 2, 2, "class_2"},
	{"BC", 2, { 8, 8, 0x0, 10, 15, 0x3e}, 2, 5, "class_31A"},//true:5 cycle false:3 cycle delay: 3 cycle
	{"B", 2, { 0, 8, 0x73, 10, 15, 0x3c}, 2, 4, "class_29A"},//true:5 cycle false:3 cycle delay: 3 cycle
	{"READA", 1, { 8, 15, 0x7e}, 1, 5, "class_25A_25B"},
	{"BANZ", 2, { 8, 8, 0x0, 10, 15, 0x1b}, 2, 4, "class_29A"},
	{"RPT_SMEM", 1, { 8, 15, 0x47}, 1, 3, "class_5A_5B"},
	{"RPT_K", 1, { 8, 15, 0xec}, 1, 1, "class_1"},
	{"RPT_LK", 1, { 0, 15, 0xf070}, 2, 2, "class_2"},
	{"ADD_SMEM_SRC", 1, { 9, 15, 0x0}, 1, 1, "class_3A_3B"},
	{"FRAME", 1, { 8, 15, 0xee}, 1, 1, "class_1"},
	{"FCALL", 2, { 7, 8, 0x3, 10, 15, 0x3e}, 2, 4, "class_29B"},
	{"PSHM", 1, { 8, 15, 0x4a}, 1, 1, "class_16A"},
	{"LD_K_DST", 1, { 9, 15, 0x74}, 1, 1, "class_1"},
	{"STL_SRC_SMEM", 1, { 9, 15, 0x40}, 1, 1, "class_10A_10B"},
	{"LD_LK_SHFT_DST", 2, { 4, 7, 0x2, 9, 15, 0x78}, 2, 2, "class_2"},
	{"NOP", 1, { 0, 15, 0xf495}, 1, 1, "class_1"},
	{"STLM_SRC_MMR", 1, { 9, 15, 0x44}, 1, 1, "class_10A"},
	{"LD_SMEM_DST", 1, { 9, 15, 0x8}, 1, 1, "class_3A_3B"},
	{"MVKD_DMAD_SMEM", 1, { 8, 15, 0x70}, 2, 2, "class_19A_19B"},
	{"POPM_MMR", 1, { 8, 15, 0x8a}, 1, 1, "class_17A"},
	{"FRET", 2, { 0, 8, 0xe4, 10, 15, 0x3d}, 1, 6, "class_34"},
	{"MVDK_SMEM_DMAD", 1, { 8, 15, 0x71}, 2, 2, "class_19A_19B"},
	{"ST_LK_SMEM", 1, { 8, 15, 0x76}, 2, 2, "class_12A_12B"},
	{"OR_SMEM_SRC", 1, { 9, 15, 0xd}, 1, 1, "class_3A_3B"},
	{"CMPM_SMEM_LK", 1, { 8, 15, 0x60}, 2, 2, "class_6A_6B"},
	{"LDM_MMR_DST", 1, { 9, 15, 0x24}, 1, 1, "class_3A"},
	{"DST_SRC_LMEM", 1, { 9, 15, 0x27}, 1, 2, "class_13A_13B"},
	{"DLD_LMEM_DST", 1, { 9, 15, 0x2b}, 1, 1, "class_9A_9B"},
	{"SUB_LK_SHFT_SRC_DST", 2, { 4, 7, 0x1, 10, 15, 0x3c}, 2, 2, "class_2"},
	{"FCALA_SRC", 2, {0, 7, 0xe7,  10, 15, 0x3d}, 1, 6, "class_30B"},
	{"MVDM_DMAD_MMR", 1, {8, 15, 0x72}, 2, 2, "class_19A"},
	{"XC_N_COND", 2, {10, 15, 0x3f, 8, 8, 0x1}, 1, 1, "class_1"},
	{"SFTA_SRC_SHIFT_DST", 2, {10, 15, 0x3d, 5, 7, 0x3}, 1, 1, "class_1"},
	{"AND_LK_SHFT_SRC_DST", 2, { 4, 7, 0x3, 10, 15, 0x3c}, 2, 2, "class_2"},
	{"SUB_SMEM_SRC", 1, { 9, 15, 0x4}, 1, 1, "class_3A_3B"},
	{"LD_SMEM_ASM", 1, { 8, 15, 0x32}, 1, 1, "class_3A_3B"},
	{"LD_SRC_ASM_DST", 2, { 0, 7, 0x82, 10, 15, 0x3d}, 1, 1, "class_1"},
	{"AND_SMEM_SRC", 1, { 9, 15, 0xc}, 1, 1, "class_3A_3B"},
	{"MAC_SMEM_SRC", 1, { 10, 15, 0xa}, 1, 1, "class_3A_3B"},
	{"BITF_SMEM_LK", 1, { 8, 15, 0x61}, 2, 2, "class_6A_6B"},
	{"CMPR_CC_ARX", 2, { 10, 15, 0x3d, 3, 7, 0x15}, 1, 1, "class_1"},
	{"ORM_LK_SMEM", 1, { 8, 15, 0x69}, 2, 2, "class_18A_18B"},
	{"MPY_SMEM_LK_DST", 1, { 9, 15, 0x31}, 2, 2, "class_6A_6B"},
	{"LDU_SMEM_DST", 1, { 9, 15, 0x9}, 1, 1, "class_3A_3B"},
	{"SFTL_SRC_SHIFT_DST", 2, {10, 15, 0x3c, 5, 7, 0x7}, 1, 1, "class_1"},
	{"OR_SRC_SHIFT_DST", 2, {10, 15, 0x3c, 5, 7, 0x5}, 1, 1, "class_1"},
	{"STL_SRC_SHIFT_SMEM", 1, {8, 15, 0x6f}, 2, 2, "class_11A_11B"},
	{"MVMM_MMRX_MMRY", 1, {8, 15, 0xe7}, 1, 1, "class_1"},
	{"MAR_SMEM", 1, {8, 15, 0x6d}, 1, 1, "class_1"},
	{"LD_SRC_SHIFT_DST", 2, {10, 15, 0x3d, 5, 7, 0x2}, 1, 1, "class_1"},
	{"OR_LK_SHIFT16_SRC_DST", 2, {10, 15, 0x3c, 0, 7, 0x64}, 2, 2, "class_2"},
	{"WRITA_SMEM", 1, {8, 15, 0x7f}, 1, 5, "class_26A_26B"},
	{"NEG_SRC_DST", 2, {10, 15, 0x3d, 0, 7, 0x84}, 1, 1, "class_1"},
	{"MAC_SMEM_LK_SRC_DST", 1, {10, 15, 0x19}, 2, 2, "class_6A_6B"},
	{"FRETE", 2, {10, 15, 0x3d, 0, 8, 0xe5}, 1, 6, "class_34"},
	{"CMPL_SRC_DST", 2, {10, 15, 0x3d, 0, 7, 0x93}, 1, 1, "class_1"},
};

uint32_t exec_insn_cache(c54x_core_t* core, insn_cache_t* cache){
	uint16_t insn = cache->insn;
	cache->action(core, insn, &(cache->fields));

	if (core->delay_enable)
		CYCLE_DEC(core);

	if (core->pend_nops_flag == 0)
		PENDING_NOPS(core);
#if 0	
	printf("PC:%x, ACC_A:%x, ACC_B:%x, SP:%x, ST0 :%x, ST1:%x\n",  core->pc,core->MMR_BUFFER[CPU_AL] | (core->MMR_BUFFER[CPU_AH] << 16), core->MMR_BUFFER[CPU_BL] | (core->MMR_BUFFER[CPU_BH] << 16),  core->MMR_BUFFER[CPU_SP], core->MMR_BUFFER[CPU_ST0], core->MMR_BUFFER[CPU_ST1]);
	int i = 16;
	for (i ;i < 24; i++)
		printf("AR%d: %x\n", i - 16, core->MMR_BUFFER[i]);

	printf("T :%x\n", core->MMR_BUFFER[CPU_TREG]);
#endif	
	core->insn_num ++;
	return 0;
}

int decode_instr(c54x_core_t* core, uint16_t insn, int32_t *idx, ISEITEM* table, int table_len)
{
	int n = 0;
	int base = 0;
	int ret = DECODE_FAILURE;
	int i = 0;
	int instr_slots = table_len;
	for (i = 0; i < instr_slots; i++)
	{
		ISEITEM* slot = &table[i];
		n = slot->attribute_value;

		/* if not content, we ignore */
		if(n == 0)
			continue;
		base = 0;
		while (n) {
			if (BITS(slot->content[base], slot->content[base + 1]) != slot->content[base + 2]) {
				break;
			}
			base += 3;
			n --;
		}
		//All conditions is satisfied.
		if (n == 0)
			ret = DECODE_SUCCESS;

		if (ret == DECODE_SUCCESS) {
			core->insn_length = slot->words;
			if (core->delay_enable)
				core->delay_slot_cycle -= slot->cycles; 
		//	printf("In %s, insn :%llx, words:%d, cycles:%d, class_name:%s\n", __FUNCTION__, insn, slot->words, slot->cycles, slot->class_mode);
			*idx = i;
			return ret;
		}
	}
	return ret;
}
insn_action_t get_insn_action(c54x_core_t* core, uint16_t insn){
	int index;
	insn_action_t ret = NULL;
	if((ret = decode_instr(core, insn, &index, insn32_decode_table, sizeof(insn32_decode_table)/sizeof(ISEITEM))) == DECODE_SUCCESS){
		DBG("In %s, instr=0x%x, index=0x%x\n", __FUNCTION__, insn, index);
		return insn_action[index];
	}
	else{
		printf("In %s, decode error for 0x%x at 0x%x\n", __FUNCTION__, insn, core->pc);
		skyeye_exit(-1);
	}
}
int trans_opcode_fields(uint16_t insn, opcode_fields_t* fields) {
    fields->k3 = BITS(0, 2);
    fields->sbit = BITS(0, 3);
	fields->mmr = BITS(0, 6);
	fields->n = BITS(9, 9);
	fields->I = BITS(7, 7); //I = 0 Direct address mode; I = 1 Indirect address mode 
	fields->IA = BITS(4, 7);
	fields->S = BITS(9, 9); //S = 0 ACC_A S = 1 ACC_B
	fields->D = BITS(8, 8); //D = 0 ACC_A D = 1 ACC_B
	fields->Z = BITS(9, 9); //Z = 0 excute not delay Z = 1 excute delay
	fields->K = BITS(0, 7);
	fields->shft = BITS(0, 3);
	fields->shift = BITS(0, 4);
	fields->cond = BITS(0, 7);
	//fields->pmad_0_22 = BITS(0, 6);

	return 0;
}

uint16_t c54x_memory_read(c54x_core_t* core, uint32_t addr){
    uint16_t data;

    memory_space_intf * io_memory = core->c54x_memory_intf;

	if (addr >= CPU_IMR && addr <= CPU_TEST5){
		data = core->MMR_BUFFER[addr];
	}else {
		if (io_memory != NULL)
    		io_memory->read(core->obj, addr, &data, 2);
	}

	//printf("In %s, addr:%x, data:%x\n", __func__, addr, data);
    return data;
}

void c54x_memory_write(c54x_core_t* core, uint32_t addr, uint16_t buffer){
    memory_space_intf * io_memory = core->c54x_memory_intf;

	if (addr >= CPU_IMR && addr <= CPU_TEST5)
		core->MMR_BUFFER[addr] = buffer;
	//printf("In %s, addr:%x, buffer:%x\n", __func__, addr, buffer);
	if (io_memory != NULL){
		if (io_memory->write(core->obj, addr, &buffer, 2) != 0){
			skyeye_log(Error_log, __func__, "Write %x to physical address %x error!\n", buffer , addr);
		}
	}
    return ;
 }

