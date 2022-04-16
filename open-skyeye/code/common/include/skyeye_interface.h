/* Copyright (C) 
* 2011 - Michael.Kang blackfin.kang@gmail.com
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
* @file skyeye_interface.h
* @brief the interface definition
* @author Michael.Kang blackfin.kang@gmail.com
* @version 1.3.4
* @date 2011-08-15
*/
#ifndef __SKYEYE_INTERFACE_H__
#define __SKYEYE_INTERFACE_H__
#include <skyeye_types.h>
#ifdef __cplusplus
 extern "C" {
#endif

exception_t SKY_register_interface(void* intf_obj, const char* objname, const char* iface_name);
void* SKY_get_interface(conf_object_t* obj, const char* iface_name);
exception_t SKY_set_interface(void* from_obj, void* to_obj, const char* iface_name);

typedef struct skyeye_interface{
	void* obj;
	char* intf_name;
	char* class_name;
	int   registered;	// if No register: 0 else 1
}skyeye_intf_t;

void const *skyeye_get_interface(conf_object_t *obj, const char *iface);
void const *skyeye_get_port_interface(conf_object_t *obj, const char *iface, const char *port);
exception_t skyeye_connect(conf_object_t *obj, conf_object_t *obj2, const char *name, uint32_t index);

#ifdef __cplusplus
}
#endif

#endif
