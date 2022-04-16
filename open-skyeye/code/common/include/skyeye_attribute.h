/*
 * skyeye_attribute.h:
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

#ifndef __SKYEYE_ATTRIBUTE_H__
#define __SKYEYE_ATTRIBUTE_H__  1
#include <skyeye_attr.h>
#include <skyeye_class.h>

#ifdef __cplusplus
extern "C" {
#endif

/* <add-type id="conf attr_attr_t"><ndx>attr_attr_t</ndx></add-type>
 * the following content is copy form simics, please refer to configuration.h of simics*/
#define SKY_ATTR_GROUP_NUMBER(number) (number<<20)
#define SKY_ATTR_ELEMENT_NUMBER(number) (number<<4)
typedef enum {
        SKY_Attr_Required         = 0,
        SKY_Attr_Optional         = 1,

        SKY_Attr_Session          = 3,
        SKY_Attr_Pseudo           = 4,

#ifndef DOC
        /* These members are for internal use only */
        //SKY_Attr_Flag_Mask        = 0xff,

        //SKY_Init_Phase_Shift      = 8,
        //SKY_Init_Phase_0          = 0 << SKY_Init_Phase_Shift,
        //SKY_Init_Phase_1          = 1 << SKY_Init_Phase_Shift,
        //SKY_Init_Phase_Bits       = 2,
        //SKY_Init_Phase_Mask       = (1 << SKY_Init_Phase_Bits) - 1,

        //SKY_Init_Phase_Pre1       =
        //                    (-1 & SKY_Init_Phase_Mask) << SKY_Init_Phase_Shift,
        //SKY_Attr_Class            = 0x8000,

        /* By default, attributes are only documented online (at the
           command-prompt).

           - to prevent an attribute from being visible in all documentation,
             use SKY_Attr_Internal.

           - to include an attribute in the offline documentation (help system,
             pdf, ...), use SKY_Attr_Doc.
        */
        //SKY_Attr_Internal         = 0x10000,
        //SKY_Attr_Doc              = 0x100000,
#endif /* not DOC */

        SKY_Attr_Integer_Indexed  = 0x1000,
        SKY_Attr_String_Indexed   = 0x2000,
        SKY_Attr_List_Indexed     = 0x4000,

        SKY_Attr_Persistent       = 0x20000,

#ifndef DOC
        /* set by SKY_register_*_attribute() */
        SKY_Attr_Read_Only        = 0x40000,
        SKY_Attr_Write_Only       = 0x80000,
#endif
        /*additional type info of skyeye Attrs*/
        SKY_Attr_Type_cpuname               =0x1000000,
        SKY_Attr_Type_memmoryspace_name     =0x2000000,
        SKY_Attr_Type_file_path             =0x3000000,
        SKY_Attr_Type_sharememmory_name     =0x4000000,
        SKY_Attr_Type_net_ip                =0x5000000,
        SKY_Attr_Type_net_port              =0x6000000,
        SKY_Attr_Type_group                 =0x7000000,
        SKY_Attr_Type_disable_show          =0x10000000



} attr_attr_t;

typedef attr_value_t (*get_attr_t)(void* arg, conf_object_t* obj, attr_value_t* idx);
typedef exception_t (*set_attr_t)(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx);

typedef attr_value_t (*get_class_attr_t)(void* arg, conf_class_t* obj, attr_value_t* idx);
typedef exception_t (*set_class_attr_t)(void* arg, conf_class_t* obj, attr_value_t* value, attr_value_t* idx);

typedef struct attribute {
	const char* name;
	attr_attr_t attr;
	const char* type;
	union {
		get_attr_t get;
		get_class_attr_t class_get;
	};
	void* set_args;
	union {
		set_attr_t set;
		set_class_attr_t class_set;
	};
	void* get_args;
	const char* desc;
}attribute_t;

void init_class_attribute_list(conf_class_t* clss);
int SKY_class_has_attribute(conf_class_t* clss, const char* attr_name);
int SKY_print_attribute(conf_class_t* clss);
exception_t SKY_change_attribute_attribute(conf_class_t* clss, const char* name, attr_attr_t attr);
int SKY_register_attribute(conf_class_t* clss, const char* name, get_attr_t get_attr, void* get_args, set_attr_t set_attr, void* set_args, attr_attr_t attr, const char* type, const char* desc);
int SKY_register_class_attribute(conf_class_t* clss, const char* name, get_class_attr_t get_attr, void* get_args, set_class_attr_t set_attr, void* set_args, attr_attr_t attr, const char* type, const char* desc);
attr_value_t SKY_get_attribute(conf_object_t* obj, const char* name);
exception_t SKY_set_attribute(conf_object_t* obj, const char* name, attr_value_t* value);
attr_value_t SKY_get_attribute_idx(conf_object_t* obj, const char* name, attr_value_t* index);
exception_t SKY_set_attribute_idx(conf_object_t* obj, const char* name, attr_value_t* index, attr_value_t* value);
attr_value_t SKY_get_class_attribute(conf_class_t* clss, const char* name);
exception_t SKY_set_class_attribute(conf_class_t* clss, const char* name, attr_value_t* value);
attr_value_t SKY_get_class_attribute_idx(conf_class_t* clss, const char* name, attr_value_t* index);
exception_t SKY_set_class_attribute_idx(conf_class_t* clss, const char* name, attr_value_t* index, attr_value_t* value);
const char** get_attributes(conf_object_t *obj);
const char** get_class_attrinfo(conf_class_t* clss, const char* attrname);
const char** get_class_attributes(conf_class_t *clss);
exception_t SKY_has_attribute(conf_object_t *obj, char *name);

/**************************attribute type********************************
 "iface"
 "string"
 "integer"
 "uinteger"
 "floating"
 "list"
 "object"
 "internal"
*************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __SKYEYE_ATTRIBUTE_H__ */
