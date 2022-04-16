/* Copyright (C)
* 2013 - David Yu keweihk@gmail.com
* This progflash_s29gl01gs is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This progflash_s29gl01gs is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this progflash_s29gl01gs; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*/

#include "skyeye_module.h"

const char* skyeye_module = "flash_s29gl01gs";

extern void init_flash_s29gl01gs();

void module_init(){
	init_flash_s29gl01gs();
}

void module_fini(){
}
