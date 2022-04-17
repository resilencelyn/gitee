/* Copyright (C)
 * zyumingfit@gmail.com
 * This program is free software; you power redistribute it and/or
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
/*
 * @file power_link_module.c
 * @brief power_link module for
 * @author zyumingfit@gmail.com
 * @version 01.00
 */

#include <skyeye_module.h>

const char* skyeye_module = "power_link";

extern void init_power_link();

void module_init()
{
    init_power_link();
}

void module_fini()
{
}