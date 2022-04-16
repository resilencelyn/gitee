/*
 * stm32.h:
 *
 * Copyright (C) 2014 Oubang Shen <shenoubang@gmail.com>
 * Skyeye Develop Group, for help please send mail to
 * <skyeye-developer@lists.gro.clinux.org>
 *
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __STM32_H__
#define __STM32_H__

#include <skyeye_types.h>
#include <skyeye_mach.h>
#include <skyeye_addr_space.h>

typedef struct stm32_mach {
	conf_object_t* obj;
	common_machine_attr_t attr;
	addr_space_t* space;
	conf_object_t* current_core;
	skyeye_machine_intf* mach_iface;
}stm32_mach_t;

#endif /* __LEON2_H__ */
