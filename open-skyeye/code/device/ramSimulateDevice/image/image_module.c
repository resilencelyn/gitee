/* Copyright (C) 
* 2013 - David Yu keweihk@gmail.com
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
* @file image_module.c
* @brief image module for memory module
* @author David Yu keweihk@gmail.com
* @version 78.77
* @date 2013-04-08
*/

#include "skyeye_module.h"

const char* skyeye_module = "image";

extern void init_image();

void module_init(){
	init_image();
}

void module_fini(){
}
