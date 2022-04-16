/* Copyright (C) 
* 2013 - David Yu keweihk@gmail.com
* This progc6713_flash is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
* 
* This progc6713_flash is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this progc6713_flash; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
* 
*/
/**
* @file c6713_flash_module.c
* @brief c6713_flash module 
* @author David Yu keweihk@gmail.com
* @version 78.77
* @date 2013-04-08
*/

#include "skyeye_module.h"

const char* skyeye_module = "c6713_flash";

extern void init_c6713_flash();

void module_init(){
	init_c6713_flash();
}

void module_fini(){
}
