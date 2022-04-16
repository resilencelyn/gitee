/*
 * sparc_v8.h:
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

#ifndef __SPARC_V8_H__
#define __SPARC_V8_H__
#include <skyeye_types.h>
#include <skyeye_addr_space.h>

#include "sparc.h"
#include "traps.h"
#include "iu.h"

typedef struct sparc_v8
{
    conf_object_t *obj;
    sparc_state_t *state;
    addr_space_t *space;
    core_running_mode_t running_mode;
    uint32_t load_mask;
    generic_address_t load_addr;
} sparc_v8_t;

#endif /* __SPARC_V8_H__ */
