/* Copyright (C) 
* 2015 - Michael.Kang blackfin.kang@gmail.com
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
* @file x86_arch_interface.h
* @brief 
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2015-01-16
*/
#ifndef __X86_ARCH_INTERFACE_H__
#define __X86_ARCH_INTERFACE_H__
generic_address_t x86_get_pc();
void x86_set_pc (generic_address_t addr);
void x86_step_once ();
uint64_t x86_get_cycles();
uint32 x86_get_regval_by_id(int id);
exception_t x86_set_register_by_id(int id, uint32 value);
char* x86_get_reg_name_by_id(int id);
char* x86_disassemble(generic_address_t offset);
int32_t x86_get_reg_id_by_name(char *name);
void x86_init_state();
#endif
