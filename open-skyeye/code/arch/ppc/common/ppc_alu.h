/*
 *      PearPC
 *      ppc_alu.h
 *
 *      Copyright (C) 2003 Sebastian Biallas (sb@biallas.net)
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License version 2 as
 *      published by the Free Software Foundation.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __PPC_ALU_H__
#define __PPC_ALU_H__

#include "ppc_cpu.h"
extern uint32 ppc_cmp_and_mask[];

void ppc_opc_addx(e500_core_t *current_core);
void ppc_opc_addox(e500_core_t *current_core);
void ppc_opc_addcx(e500_core_t *current_core);
void ppc_opc_addcox(e500_core_t *current_core);
void ppc_opc_addex(e500_core_t *current_core);
void ppc_opc_addeox(e500_core_t *current_core);
void ppc_opc_addi(e500_core_t *current_core);
void ppc_opc_addic(e500_core_t *current_core);
void ppc_opc_addic_(e500_core_t *current_core);
void ppc_opc_addis(e500_core_t *current_core);
void ppc_opc_addmex(e500_core_t *current_core);
void ppc_opc_addmeox(e500_core_t *current_core);
void ppc_opc_addzex(e500_core_t *current_core);
void ppc_opc_addzeox(e500_core_t *current_core);

void ppc_opc_andx(e500_core_t *current_core);
void ppc_opc_andcx(e500_core_t *current_core);
void ppc_opc_andi_(e500_core_t *current_core);
void ppc_opc_andis_(e500_core_t *current_core);

void ppc_opc_cmp(e500_core_t *current_core);
void ppc_opc_cmpi(e500_core_t *current_core);
void ppc_opc_cmpl(e500_core_t *current_core);
void ppc_opc_cmpli(e500_core_t *current_core);

void ppc_opc_cntlzwx(e500_core_t *current_core);

void ppc_opc_crand(e500_core_t *current_core);
void ppc_opc_crandc(e500_core_t *current_core);
void ppc_opc_creqv(e500_core_t *current_core);
void ppc_opc_crnand(e500_core_t *current_core);
void ppc_opc_crnor(e500_core_t *current_core);
void ppc_opc_cror(e500_core_t *current_core);
void ppc_opc_crorc(e500_core_t *current_core);
void ppc_opc_crxor(e500_core_t *current_core);

void ppc_opc_divwx(e500_core_t *current_core);
void ppc_opc_divwux(e500_core_t *current_core);
void ppc_opc_divwox(e500_core_t *current_core);
void ppc_opc_divwuox(e500_core_t *current_core);

void ppc_opc_eqvx(e500_core_t *current_core);

void ppc_opc_extsbx(e500_core_t *current_core);
void ppc_opc_extshx(e500_core_t *current_core);

void ppc_opc_mulhwx(e500_core_t *current_core);
void ppc_opc_mulhwux(e500_core_t *current_core);
void ppc_opc_mulli(e500_core_t *current_core);
void ppc_opc_mullwx(e500_core_t *current_core);
void ppc_opc_mullwox(e500_core_t *current_core);

void ppc_opc_nandx(e500_core_t *current_core);

void ppc_opc_negx(e500_core_t *current_core);
void ppc_opc_negox(e500_core_t *current_core);
void ppc_opc_norx(e500_core_t *current_core);

void ppc_opc_orx(e500_core_t *current_core);
void ppc_opc_orcx(e500_core_t *current_core);
void ppc_opc_ori(e500_core_t *current_core);
void ppc_opc_oris(e500_core_t *current_core);

void ppc_opc_rlwimix(e500_core_t *current_core);
void ppc_opc_rlwinmx(e500_core_t *current_core);
void ppc_opc_rlwnmx(e500_core_t *current_core);

void ppc_opc_slwx(e500_core_t *current_core);
void ppc_opc_srawx(e500_core_t *current_core);
void ppc_opc_srawix(e500_core_t *current_core);
void ppc_opc_srwx(e500_core_t *current_core);

void ppc_opc_subfx(e500_core_t *current_core);
void ppc_opc_subfox(e500_core_t *current_core);
void ppc_opc_subfcx(e500_core_t *current_core);
void ppc_opc_subfcox(e500_core_t *current_core);
void ppc_opc_subfex(e500_core_t *current_core);
void ppc_opc_subfeox(e500_core_t *current_core);
void ppc_opc_subfic(e500_core_t *current_core);
void ppc_opc_subfmex(e500_core_t *current_core);
void ppc_opc_subfmeox(e500_core_t *current_core);
void ppc_opc_subfzex(e500_core_t *current_core);
void ppc_opc_subfzeox(e500_core_t *current_core);

void ppc_opc_xorx(e500_core_t *current_core);
void ppc_opc_xori(e500_core_t *current_core);
void ppc_opc_xoris(e500_core_t *current_core);

void ppc_opc_iseleq(e500_core_t *current_core);
void ppc_opc_iselgt(e500_core_t *current_core);
void ppc_opc_isel(e500_core_t *current_core);
void ppc_opc_tlbsx(e500_core_t *current_core);
#endif
