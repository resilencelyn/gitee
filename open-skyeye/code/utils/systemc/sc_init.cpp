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
* @file sc_init.cpp
* @brief the systemc initialization
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-05-07
*/

#include "lt_top.h"
#include "sc_init.h"
#include <skyeye_bus.h>
#include "skyeye_addr_space.h"
#include "skyeye_class.h"
#include "skyeye_types.h"

Lt_top *top_ptr;

extern void init_skyeye_device();

void bus_dispatch(tlm::tlm_generic_payload & payload, sc_core::sc_time & delay_time)
{

}

void init_sc_device()
{
    init_skyeye_device();

}

void init_systemc_class()
{
    init_sc_device();

    top_ptr = new Lt_top("lt_top");
    return;
}
