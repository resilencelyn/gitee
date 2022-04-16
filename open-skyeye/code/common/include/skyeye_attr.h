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
* @file skyeye_attr.h
* @brief The definition of attribute system
* @author Michael.Kang blackfin.kang@gmail.com
* @version 0.1
* @date 2011-09-22
*/

#ifndef __SKYEYE_ATTR_H__
#define __SKYEYE_ATTR_H__
#include <skyeye_types.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VALUE_TYPE_LIST(f) \
	f(Invalid,  invalid) \
	f(String,   string) \
	f(Integer,  integer) \
	f(UInteger, uinteger) \
	f(Floating, floating) \
	f(List,     list) \
	f(Data,     data) \
	f(Object,   object) \
	f(Nil,      nil) \
	f(Dict,     dict) \
	f(Boolean,  boolean)

#define VALUE_TYPE_DEF(old, alias) \
	Val_##old, Val_##alias = Val_##old,
typedef enum {
	VALUE_TYPE_LIST(VALUE_TYPE_DEF)
	Val_ptr,
	Sim_Val_Unresolved_Object
} value_type_t;

typedef struct attr_value {
	value_type_t type;
	union {
		integer_t integer;              /* Sim_Val_Integer */
		uinteger_t uinteger;            /* Sim_Val_Integer */
		bool_t boolean;                 /* Sim_Val_Boolean */
		double floating;                /* Sim_Val_Floating */

		/* string */
		struct {
			bool_t adopt;
			const char *str;             /* Sim_Val_String */
		} string;

		/* Sim_Val_List */
		struct {
			size_t size;
			struct attr_value *vector;  /* [size] */
		} list;

		/* data array */
		struct {
			bool_t adopt;
			size_t size;
			uint8 *data;                /* [size] */
		} data;

		/* dict */
		struct {
			size_t size;
			struct attr_value *key;     /* binding with val */
			struct attr_value *val;     /* binding with key */
		} dict;

		conf_object_t* object;          /* Sim_Val_Object */
		void* ptr;
	} u;
} attr_value_t;

attr_value_t SKY_alloc_attr_dict(unsigned length);
attr_value_t SKY_alloc_attr_list(unsigned length);
void SKY_attr_dict_resize(attr_value_t *attr, unsigned newsize);
void SKY_attr_dict_set_item(attr_value_t *attr, unsigned index,
		attr_value_t key, attr_value_t value);
void SKY_attr_list_resize(attr_value_t *attr, unsigned newsize);
void SKY_attr_list_set_item(attr_value_t *attr, unsigned index,
		attr_value_t elem);

/* not implement */
//bool SKY_ascanf(attr_value_t *list, const char *fmt, ...);

/* deep copy */
attr_value_t SKY_attr_copy(attr_value_t val);

/* pick value */
integer_t      SKY_attr_integer(attr_value_t attr);
uinteger_t     SKY_attr_uinteger(attr_value_t attr);
bool_t         SKY_attr_boolean(attr_value_t attr);
const char*    SKY_attr_string(attr_value_t attr);
char*          SKY_attr_string_detach(attr_value_t *attr);
double         SKY_attr_floating(attr_value_t attr);
conf_object_t* SKY_attr_object(attr_value_t attr);
conf_object_t* SKY_attr_object_or_nil(attr_value_t attr);
void*          SKY_attr_data(attr_value_t attr);
unsigned       SKY_attr_data_size(attr_value_t attr);
attr_value_t*  SKY_attr_list(attr_value_t attr);
unsigned       SKY_attr_list_size(attr_value_t attr);
attr_value_t   SKY_attr_list_item(attr_value_t attr, unsigned index);
attr_value_t   SKY_attr_dict_key(attr_value_t attr, unsigned index);
attr_value_t   SKY_attr_dict_value(attr_value_t attr, unsigned index);
unsigned       SKY_attr_dict_size(attr_value_t attr);

/* determine type */
bool_t SKY_attr_is_integer(attr_value_t attr);
bool_t SKY_attr_is_uinteger(attr_value_t attr);
bool_t SKY_attr_is_boolean(attr_value_t attr);
bool_t SKY_attr_is_string(attr_value_t attr);
bool_t SKY_attr_is_floating(attr_value_t attr);
bool_t SKY_attr_is_object(attr_value_t attr);
bool_t SKY_attr_is_invalid(attr_value_t attr);
bool_t SKY_attr_is_nil(attr_value_t attr);
bool_t SKY_attr_is_data(attr_value_t attr);
bool_t SKY_attr_is_list(attr_value_t attr);
bool_t SKY_attr_is_dict(attr_value_t attr);

/* make new attr */
attr_value_t SKY_make_attr_integer(integer_t i);
attr_value_t SKY_make_attr_uinteger(uinteger_t ui);
attr_value_t SKY_make_attr_boolean(bool_t b);
attr_value_t SKY_make_attr_string(const char *str);
attr_value_t SKY_make_attr_string_adopt(const char *str);
attr_value_t SKY_make_attr_floating(double d);
attr_value_t SKY_make_attr_object(conf_object_t *obj);
attr_value_t SKY_make_attr_invalid(void);
attr_value_t SKY_make_attr_nil(void);
attr_value_t SKY_make_attr_data(size_t size, const void *data);
attr_value_t SKY_make_attr_data_adopt(size_t size, void *data);
attr_value_t SKY_make_attr_list(unsigned length, ...);
attr_value_t SKY_make_attr_list_vararg(unsigned length, va_list va);

void SKY_attr_free(attr_value_t *value);
void SKY_free_attribute(attr_value_t value);

typedef enum{
	Set_ok = 0,
	Set_invalid_type,
	Set_error,
}set_attr_error_t;

set_attr_error_t SKY_set_attr(conf_object_t* obj, const char* attr_name, attr_value_t* attr);
attr_value_t* SKY_get_attr(conf_object_t* conf_obj, const char* attr_name);

void SKY_register_attr(conf_object_t* obj, const char*  attr_name, attr_value_t* attr);
attr_value_t* make_new_attr(value_type_t type, void* value);
exception_t set_conf_attr(conf_object_t* obj, char* attr_name, attr_value_t* value);

#define DEF_UINTEGER_ATTR_FUNC(type, name); \
	attr_value_t name##_get_attr(void* arg, conf_object_t* obj, attr_value_t* idx) \
	{\
		uint32_t data; \
		type *dev = obj->obj; \
		data = dev->name; \
		attr_value_t value = SKY_make_attr_uinteger(data); \
		return value; \
	} \
	exception_t name##_set_attr(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx) { \
		type *dev = obj->obj; \
		uint32_t data = SKY_attr_uinteger(*value);\
		dev->name = data; \
		return No_exp;	\
	}


#define DEF_LIST_UINTEGER_ATTR_FUNC(type, name, num); \
	attr_value_t name##_get_attr(void* arg, conf_object_t* obj, attr_value_t* idx) { \
		attr_value_t value = SKY_alloc_attr_list(num); \
		attr_value_t item; \
		type* dev = obj->obj;	\
		uint32_t i;	\
		for (i = 0; i < num; i++) { \
			uint32_t data = dev->name[i]; \
			item = SKY_make_attr_uinteger(data); \
			SKY_attr_list_set_item(&value, i, item); \
		} \
			\
		return value; \
	} \
	exception_t name##_set_attr(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx) {	\
		type* dev = obj->obj; \
		uint32_t i = 0;	\
		uint32_t data = 0; \
		attr_value_t item; \
		for (i = 0; i < num; i++) { \
			item = SKY_attr_list_item(*value, i); \
			data = SKY_attr_uinteger(item); \
			dev->name[i] = data; \
		} \
			\
		return No_exp; \
	}
#define DEF_DOUBLE_ARRAY_UINTEGER_ATTR_FUNC(type, name, num, x_len, y_len); \
	attr_value_t name##_get_attr(void* arg, conf_object_t* obj, attr_value_t* idx) { \
		attr_value_t value = SKY_alloc_attr_list(num); \
		attr_value_t item; \
		type* dev = obj->obj;	\
		uint32_t i, j, index = 0;	\
		for (i = 0; i < x_len; i++) { \
			for (j = 0; j < y_len;j++){ \
				uint32_t data = dev->name[i][j]; \
				item = SKY_make_attr_uinteger(data); \
				SKY_attr_list_set_item(&value, index, item); \
				index++; \
			}\
		} \
			\
		return value; \
	} \
	exception_t name##_set_attr(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx) {	\
		type* dev = obj->obj; \
		uint32_t i = 0;	\
		uint32_t j = 0;	\
		uint32_t index = 0;	\
		uint32_t data = 0; \
		attr_value_t item; \
		for (i = 0; i < x_len; i++) { \
			for (j = 0; j < y_len;j++){ \
				item = SKY_attr_list_item(*value, index); \
				data = SKY_attr_uinteger(item); \
				dev->name[i][j] = data; \
				index++; \
			}\
		} \
			\
		return No_exp; \
	}
#define DEF_ATTR(name, type, desc) \
	SKY_register_attribute(clss, #name, name##_get_attr, NULL, name##_set_attr, NULL, SKY_Attr_Optional, #type, desc)

#define DEF_DISABLE_SHOW_ATTR(name, type, desc) \
	SKY_register_attribute(clss, #name, name##_get_attr, NULL, name##_set_attr, NULL, SKY_Attr_Optional|SKY_Attr_Type_disable_show, #type, desc)

#ifdef __cplusplus
}
#endif
#endif
