/*
 * skyeye_iface.h:
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

#ifndef __SKYEYE_IFACE_H__
#define __SKYEYE_IFACE_H__  1
#include <skyeye_class.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void interface_t;

void init_class_interface_list(conf_class_t* clss);
void SKY_print_class_iface(conf_class_t* clss);
int SKY_unregister_iface(conf_class_t* clss, const char* name);
int SKY_register_iface(conf_class_t* clss, const char* name, const void* iface);
interface_t* SKY_get_class_iface(conf_class_t* clss, const char* name);
interface_t* SKY_get_iface(conf_object_t* obj, const char* name);
interface_t* SKY_get_port_iface(conf_object_t* obj, const char* name, const char* portname);
interface_t* SKY_get_class_port_iface(conf_class_t* clss, const char* name, const char* portname);
exception_t SKY_connect(conf_object_t* dest, conf_object_t* sr, const char* name);
char **SKY_get_ifacename_list(conf_class_t *clss);

#ifdef __cplusplus
}
#endif

#endif /* __SKYEYE_IFACE_H__ */
