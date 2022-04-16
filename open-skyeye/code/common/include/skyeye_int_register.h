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
* @file skyeye_int_register.h
* @brief The interface for register
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-09-03
*/

#ifndef __SKYEYE_INT_REGISTER_H__
#define __SKYEYE_INT_REGISTER_H__
typedef struct skyeye_reg_intf{
	conf_object_t* conf_obj;
	uint32_t (*get_regvalue_by_id) (conf_object_t* conf_obj, uint32_t id);
	char* (*get_regname_by_id) (conf_object_t* conf_obj, uint32_t id);
	exception_t (*set_regvalue_by_id) (conf_object_t* conf_obj, uint32_t value, uint32_t id);

	uint32 (*get_regid_by_name)(conf_object_t* opaque, char* name);
	uint32 (*get_regnum)(conf_object_t* opaque);
	uint32_t (*get_regoffset_by_id) (conf_object_t* conf_obj, uint32_t id);
}skyeye_reg_intf;
#define SKYEYE_REG_INTF        "skyeye_register_intf"

typedef struct skyeye_reg64_intf{
	conf_object_t* conf_obj;
	uint64_t (*get_regvalue_by_id) (conf_object_t* conf_obj, uint32_t id);
	char* (*get_regname_by_id) (conf_object_t* conf_obj, uint32_t id);
	exception_t (*set_regvalue_by_id) (conf_object_t* conf_obj, uint64_t value, uint32_t id);

	uint32 (*get_regid_by_name)(conf_object_t* opaque, char* name);
	uint32 (*get_regnum)(conf_object_t* opaque);
	uint32_t (*get_regoffset_by_id) (conf_object_t* conf_obj, uint32_t id);
}skyeye_reg64_intf;
#define SKYEYE_REG64_INTF        "skyeye_register64_intf"
#endif
