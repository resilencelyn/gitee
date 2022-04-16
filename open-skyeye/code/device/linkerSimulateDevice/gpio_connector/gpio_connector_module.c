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
* @file timer_6713_module.c
* @brief timer_6713 module for
* @author 
* @version 78.77
*/

#include <skyeye_module.h>

const char* skyeye_module = "gpio_gate";

extern void init_gpio_connector();

void module_init(){
	init_gpio_connector();
}

void module_fini(){
}

const char* ModuleAuthor = "www.dijiproto.com";
const char* ModuleVersion = "20141103";
const char* MOduleDestription = "Gpio connector is the connector between two gpio devices. A gpio port value change , at the same time, port on\
				 the other side of the connector is changing through Gpio connector.";

