/* Copyright (C) 
* 
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
* @file fight_control_sparc_module.c
* @brief fight_control_sparc module for
* @author 
* @version 78.77
*/

#include <skyeye_module.h>

const char* skyeye_module = "fight_control_sparc";

extern void init_fight_control_sparc();

void module_init()
{
    init_fight_control_sparc();
}

void module_fini()
{
}
