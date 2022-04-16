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
* @file mpc8641d_module.c
* @brief 
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2015-01-09
*/

#include <stdlib.h>
#include <stdio.h>

#include "skyeye_mach.h"
#include "skyeye_module.h"
#include "skyeye_log.h"

const char *skyeye_module = "mpc8641d_mach";

extern exception_t init_skyeye_mach_mpc8641d();

void module_init()
{
    init_skyeye_mach_mpc8641d();
    return;
}

void module_fini()
{
}
