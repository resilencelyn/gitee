/*
 * arm_module.c - the module function for arm simulation on SkyEye.
 * Copyright (C) 2003-2010 Skyeye Develop Group
 * for help please send mail to <skyeye-developer@lists.sf.linuxforum.net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */
/*
 * 01/16/2010   Michael.Kang  <blackfin.kang@gmail.com>
 */

#include "skyeye_module.h"
#include "skyeye_mach.h"
#include "skyeye_options.h"
#include "armdefs.h"
#include <stdlib.h>
#include <skyeye_log.h>
const char *skyeye_module = "arm";

extern void init_arm11_core();
extern void init_cortex_m3_core();
extern void init_cortex_a9_core();
extern void init_cortex_a8_core();
extern void init_arm920t_core();
extern void init_arm926ejs_core();
extern void init_cortex_r4_core();
extern void init_ftc662_core();

void module_init()
{
    /* register the arm core to the common library */
    init_arm11_core();
    init_cortex_m3_core();
    init_cortex_a9_core();
    init_cortex_a8_core();
    init_arm920t_core();
    init_arm926ejs_core();
    init_cortex_r4_core();
    init_ftc662_core();
}

void module_fini()
{
}
