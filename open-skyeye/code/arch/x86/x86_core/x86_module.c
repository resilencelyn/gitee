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
* @file x86_module.cc
* @brief 
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2015-01-15
*/

#include <stdlib.h>
#include "skyeye_module.h"
#include "skyeye_mach.h"

extern void init_x86_bu61580();
extern void init_bu_61580_ram();
extern void init_kio_gpio();
const char* skyeye_module = "x86_core";
extern void init_x86_core();
void module_init(){
	init_x86_core();
	init_x86_bu61580();
	init_x86_sja1000();
	//init_kio_gpio();
	init_x86_gpio();
	init_x86_ad();
	init_x86_rs422();
	init_bu_61580_ram();

}

void module_fini(){
}
