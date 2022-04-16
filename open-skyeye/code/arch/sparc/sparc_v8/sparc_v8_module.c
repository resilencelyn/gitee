/* Copyright (C) 
 * 2014 - Michael.Kang blackfin.kang@gmail.com
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
* @file sparc_module.c
* @brief The module of sparc processor
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2014-03-21
*/

#include <stdlib.h>
#include "skyeye_module.h"
#include "skyeye_mach.h"

const char *skyeye_module = "sparc";

extern void init_sparc_dyncom_arch();
extern machine_config_t sparc_machines[];
extern void init_sparc_v8();
void module_init()
{
    //init_sparc_arch ();
#ifdef LLVM_EXIST
    //init_sparc_dyncom_arch();
#endif
#if 0
    /*
     * register all the supported mach to the common library.
     */
    int i = 0;

    while (sparc_machines[i].machine_name != NULL)
    {
        register_mach(sparc_machines[i].machine_name, sparc_machines[i].mach_init);
        i++;
    }
#endif
    init_sparc_v8();
}

void module_fini()
{
}
