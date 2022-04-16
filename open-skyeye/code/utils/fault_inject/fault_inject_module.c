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
* @file fault_inject_module.c
* @brief the module of fault injection
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-10-13
*/

#include <stdio.h>
#include <errno.h>
#include "skyeye_types.h"
#include "skyeye_arch.h"
#include "skyeye_module.h"

int fault_inject_init();
int fault_inject_fini();

/* module name */
const char *skyeye_module = "fault_inject";

/* module initialization and will be executed automatically when loading. */
void module_init()
{
    fault_inject_init();
}

/* module destruction and will be executed automatically when unloading */
void module_fini()
{
    fault_inject_fini();
}
