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
* @file c6k_module.c
* @brief The module of c6k
* @version 7849
* @date 2015-10-28
*/

#include <stdlib.h>
#include "skyeye_module.h"

const char *skyeye_module = "c64xplus_core";

extern void init_c64xplus_core();
void module_init()
{
    init_c64xplus_core();
}

void module_fini()
{
}
