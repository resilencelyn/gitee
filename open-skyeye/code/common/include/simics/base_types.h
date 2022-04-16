/*
 * base_types.h: 
 *
 * Copyright (C) 2013 alloc <alloc.young@gmail.com>
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

#ifndef __BASE_TYPES_H__ 
#define __BASE_TYPES_H__  
#include "skyeye_types.h"
#include "memory_space.h"

//typedef int64_t integer_t;
//typedef uint64_t uinteger_t;
//typedef int8_t int8;
//typedef uint8_t uint8;
typedef int16_t int16;
//typedef uint16_t uint16;
//typedef int32_t int32;
//typedef uint32_t uint32;
typedef int64_t int64;
//typedef uint64_t uint64;
#ifndef __MINGW32__
//typedef uint64 uintptr_t;
#endif
typedef int set_error_t;
typedef int exception_type_t;
//typedef uint32 physical_address_t;
#define conf_object_register SIM_object_register
typedef physical_address_t logical_address_t;
typedef memory_space_intf MemoryRegion;
#define MM_STRDUP skyeye_strdup
#define MM_FREE skyeye_free
#define SIM_c_get_port_interface skyeye_get_port_interface
#define SIM_c_get_interface SKY_get_interface
#define UNUSED(x) (void)(x)
#define MASK(x) ((x) < 32 \
                 ? ((1 << (x)) - 1) \
                 : (x) < 64 \
                 ? ((UINT64_C(1) << (x)) - 1) \
                 : UINT64_C(0xffffffffffffffff))
#define MASK1(x) MASK((x) + 1)
#define MIX(x, y, mask) (((x) & ~(mask)) | ((y) & (mask)))

#endif /* __BASE_TYPES_H__ */
