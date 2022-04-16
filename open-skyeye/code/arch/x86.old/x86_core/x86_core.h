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
* @file x86_core.h
* @brief 
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2015-01-15
*/

#ifndef __X86_CORE_H__
#define __X86_CORE_H__
#include "bochs.h"
typedef struct x86_core{
	conf_object_t* obj;
	BX_CPU_C * core;	
	uint64_t stop_step;
	uint64_t steps;
	breakpoint_mgt_t bp_manager;
}x86_core_t;

#ifdef __cplusplus
 extern "C" {
#endif

void init_x86_core();
#ifdef __cplusplus
}
#endif

#endif
