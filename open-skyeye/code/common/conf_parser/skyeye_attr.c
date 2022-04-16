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
* @file skyeye_attr.c
* @brief The attribute system for skyeye object
* @author Michael.Kang blackfin.kang@gmail.com
* @version 0.1
* @date 2011-09-22
*/

/**
 * all API description reference to simics-base
 */

#include <string.h>
#include <stdlib.h>

#include <skyeye_obj.h>
#include <skyeye_mm.h>

#include <skyeye_log.h>
#include <skyeye_attribute.h>
#include "skyeye_class.h"

#define ATTR_ASSERT(cond) assert(cond)

static void SKY_attr_array_init(attr_value_t * list, unsigned length, value_type_t type)
{
    unsigned i;

    for (i = 0; i < length; i++)
    {
        attr_value_t *attr = list + i;

        memset(attr, 0, sizeof (attr_value_t));
        attr->type = type;
    }
}

/* not free list */
static void SKY_attr_array_clean(attr_value_t * list, unsigned length)
{
    unsigned i;

    for (i = 0; i < length; i++)
    {
        SKY_free_attribute(list[i]);
    }
}

static void SKY_attr_array_free(attr_value_t * list, unsigned length)
{
    if (length == 0)
        return;
    SKY_attr_array_clean(list, length);
    skyeye_free(list);
}

attr_value_t SKY_alloc_attr_dict(unsigned length)
{
    attr_value_t attr;

    attr.type = Val_Dict;

    attr.u.dict.size = length;
    attr.u.dict.key = (attr_value_t *) skyeye_mm(sizeof (attr_value_t) * length);
    SKY_attr_array_init(attr.u.dict.key, length, Val_Invalid);
    attr.u.dict.val = (attr_value_t *) skyeye_mm(sizeof (attr_value_t) * length);
    SKY_attr_array_init(attr.u.dict.val, length, Val_Invalid);
    return attr;
}

attr_value_t SKY_alloc_attr_list(unsigned length)
{
    attr_value_t attr;

    attr.type = Val_List;

    attr.u.list.size = length;
    attr.u.list.vector = (attr_value_t *) skyeye_mm(sizeof (attr_value_t) * length);
    SKY_attr_array_init(attr.u.list.vector, length, Val_Invalid);
    return attr;
}

static attr_value_t *SKY_attr_array_strip(attr_value_t * array, unsigned newsize, unsigned oldsize)
{
    attr_value_t *newarray;

    SKY_attr_array_clean(array + newsize, oldsize - newsize);
    newarray = (attr_value_t *) skyeye_realloc(array, sizeof (attr_value_t) * newsize);
    return newarray;
}

static attr_value_t *SKY_attr_array_expand(attr_value_t * array, unsigned newsize, unsigned oldsize)
{
    attr_value_t *newarray;

    newarray = (attr_value_t *) skyeye_realloc(array, sizeof (attr_value_t) * newsize);
    SKY_attr_array_init(newarray + oldsize, newsize - oldsize, Val_Invalid);
    return newarray;
}

void SKY_attr_dict_resize(attr_value_t * attr, unsigned newsize)
{
    if (newsize < attr->u.dict.size)
    {
        attr->u.dict.key = SKY_attr_array_strip(attr->u.dict.key, newsize, attr->u.dict.size);
        attr->u.dict.val = SKY_attr_array_strip(attr->u.dict.val, newsize, attr->u.dict.size);
        attr->u.dict.size = newsize;
    } else if (newsize > attr->u.dict.size)
    {
        attr->u.dict.key = SKY_attr_array_expand(attr->u.dict.key, newsize, attr->u.dict.size);
        attr->u.dict.val = SKY_attr_array_expand(attr->u.dict.val, newsize, attr->u.dict.size);
        attr->u.dict.size = newsize;
    } else
    {
        /* nothing should be done */
    }
}

void SKY_attr_dict_set_item(attr_value_t * attr, unsigned index, attr_value_t key, attr_value_t value)
{
    ATTR_ASSERT(index < attr->u.dict.size);
    ATTR_ASSERT((key.type == Val_integer) ||
                (key.type == Val_uinteger) || (key.type == Val_string) || (key.type == Val_object));

    SKY_free_attribute(attr->u.dict.key[index]);
    SKY_free_attribute(attr->u.dict.val[index]);

    attr->u.dict.key[index] = key;
    attr->u.dict.val[index] = value;
}

void SKY_attr_list_resize(attr_value_t * attr, unsigned newsize)
{
    if (newsize < attr->u.list.size)
    {
        attr->u.list.vector = SKY_attr_array_strip(attr->u.list.vector, newsize, attr->u.list.size);
        attr->u.list.size = newsize;
    } else if (newsize > attr->u.dict.size)
    {
        attr->u.list.vector = SKY_attr_array_expand(attr->u.list.vector, newsize, attr->u.list.size);
        attr->u.list.size = newsize;
    } else
    {
        /* nothing should be done */
    }
}

void SKY_attr_list_set_item(attr_value_t * attr, unsigned index, attr_value_t elem)
{
    ATTR_ASSERT(index < attr->u.list.size);
    SKY_free_attribute(attr->u.list.vector[index]);
    attr->u.list.vector[index] = elem;
}

/* not implement */
//bool SKY_ascanf(attr_value_t *list, const char *fmt, ...);

static attr_value_t *SKY_attr_array_copy(attr_value_t * list, unsigned length)
{
    int i;
    attr_value_t *target = skyeye_mm(sizeof (attr_value_t) * length);

    for (i = 0; i < length; i++)
    {
        target[i] = SKY_attr_copy(list[i]);
    }
    return target;
}

/* deep copy */
attr_value_t SKY_attr_copy(attr_value_t val)
{
    attr_value_t target;

    target = val;

    switch (val.type)
    {
        case Val_data:
            if (val.u.data.adopt == False && val.u.data.size > 0)
            {
                target.u.data.data = skyeye_mm(val.u.data.size);
                memcpy(target.u.data.data, val.u.data.data, val.u.data.size);
            }
            break;
        case Val_string:
            if (val.u.string.adopt == False && val.u.string.str)
            {
                target.u.string.str = strdup(val.u.string.str);
            }
            break;
        case Val_list:
            val.u.list.vector = SKY_attr_array_copy(val.u.list.vector, val.u.list.size);
            break;
        case Val_dict:
            val.u.dict.key = SKY_attr_array_copy(val.u.dict.key, val.u.dict.size);
            val.u.dict.val = SKY_attr_array_copy(val.u.dict.val, val.u.dict.size);
            break;
        case Val_integer:
        case Val_uinteger:
        case Val_boolean:
        case Val_floating:
        case Val_object:
            break;
        case Val_nil:
        case Val_invalid:
            break;
        default:
            /* Wrong? ptr? */
            assert(0 && "wrong type");
            break;
    }
    return target;
}

/* pick value */
#define SKY_ATTR_COMMON(rettype, vtype) \
	rettype SKY_attr_##vtype(attr_value_t attr) { \
		ATTR_ASSERT(attr.type == Val_##vtype); \
		return attr.u.vtype; \
	}
SKY_ATTR_COMMON(integer_t, integer)
SKY_ATTR_COMMON(uinteger_t, uinteger)
SKY_ATTR_COMMON(bool_t, boolean) SKY_ATTR_COMMON(double, floating) SKY_ATTR_COMMON(conf_object_t *, object)
    const char *SKY_attr_string(attr_value_t attr)
{
    ATTR_ASSERT(attr.type == Val_string);
    return attr.u.string.str;
}

char *SKY_attr_string_detach(attr_value_t * attr)
{
    ATTR_ASSERT(attr->type == Val_string);
    const char *tmp = attr->u.string.str;

    *attr = SKY_make_attr_nil();
    return (char *) tmp;
}

conf_object_t *SKY_attr_object_or_nil(attr_value_t attr)
{
    ATTR_ASSERT((attr.type == Val_object) || (attr.type == Val_nil));
    if (attr.type == Val_object)
    {
        return attr.u.object;
    }
    return NULL;
}

void *SKY_attr_data(attr_value_t attr)
{
    ATTR_ASSERT(attr.type == Val_data);
    return attr.u.data.data;
}

unsigned SKY_attr_data_size(attr_value_t attr)
{
    ATTR_ASSERT(attr.type == Val_data);
    return attr.u.data.size;
}

attr_value_t *SKY_attr_list(attr_value_t attr)
{
    ATTR_ASSERT(attr.type == Val_list);
    return attr.u.list.vector;
}

unsigned SKY_attr_list_size(attr_value_t attr)
{
    ATTR_ASSERT(attr.type == Val_list);
    return attr.u.list.size;
}

attr_value_t SKY_attr_list_item(attr_value_t attr, unsigned index)
{
    ATTR_ASSERT((attr.type == Val_list) && (index < attr.u.list.size));
    return attr.u.list.vector[index];
}

attr_value_t SKY_attr_dict_key(attr_value_t attr, unsigned index)
{
    ATTR_ASSERT((attr.type == Val_dict) && (index < attr.u.dict.size));
    return attr.u.dict.key[index];
}

attr_value_t SKY_attr_dict_value(attr_value_t attr, unsigned index)
{
    ATTR_ASSERT((attr.type == Val_dict) && (index < attr.u.dict.size));
    return attr.u.dict.val[index];
}

unsigned SKY_attr_dict_size(attr_value_t attr)
{
    ATTR_ASSERT(attr.type == Val_dict);
    return attr.u.dict.size;
}

/* determine type */
#define SKY_ATTR_IS_COMMON(vtype) \
	bool_t SKY_attr_is_##vtype(attr_value_t attr) { \
		return (attr.type == Val_##vtype); \
	}
SKY_ATTR_IS_COMMON(integer)
SKY_ATTR_IS_COMMON(uinteger)
SKY_ATTR_IS_COMMON(boolean)
SKY_ATTR_IS_COMMON(string)
SKY_ATTR_IS_COMMON(floating)
SKY_ATTR_IS_COMMON(object)
SKY_ATTR_IS_COMMON(invalid)
SKY_ATTR_IS_COMMON(nil) SKY_ATTR_IS_COMMON(data) SKY_ATTR_IS_COMMON(list) SKY_ATTR_IS_COMMON(dict)
    /* make new attr */
#define SKY_MAKE_ATTR_COMMON(intype, vtype) \
	attr_value_t SKY_make_attr_##vtype(intype value) { \
		attr_value_t attr; \
		attr.type = Val_##vtype; \
		attr.u.vtype = value; \
		return attr; \
	}
#define SKY_MAKE_ATTR_COMMON_VOID(vtype) \
	attr_value_t SKY_make_attr_##vtype(void) { \
		attr_value_t attr; \
		attr.type = Val_##vtype; \
		return attr; \
	}
    SKY_MAKE_ATTR_COMMON(integer_t, integer)
SKY_MAKE_ATTR_COMMON(uinteger_t, uinteger)
SKY_MAKE_ATTR_COMMON(bool_t, boolean)
SKY_MAKE_ATTR_COMMON(double, floating) SKY_MAKE_ATTR_COMMON_VOID(nil) SKY_MAKE_ATTR_COMMON_VOID(invalid)
    attr_value_t SKY_make_attr_string(const char *str)
{
    if (str == NULL)
    {
        return SKY_make_attr_nil();
    }
    attr_value_t attr;

    attr.type = Val_string;
    attr.u.string.adopt = False;
    attr.u.string.str = strdup(str);
    return attr;
}

attr_value_t SKY_make_attr_string_adopt(const char *str)
{
    if (str == NULL)
    {
        return SKY_make_attr_nil();
    }
    attr_value_t attr;

    attr.type = Val_string;
    attr.u.string.adopt = True;
    attr.u.string.str = str;
    return attr;
}

attr_value_t SKY_make_attr_object(conf_object_t * obj)
{
    if (obj == NULL)
    {
        return SKY_make_attr_nil();
    }
    attr_value_t attr;

    attr.type = Val_object;
    attr.u.object = obj;
    return attr;
}

attr_value_t SKY_make_attr_data(size_t size, const void *data)
{
    ATTR_ASSERT(data != NULL);
    attr_value_t attr;

    attr.type = Val_data;
    attr.u.data.size = size;
    attr.u.data.data = (uint8 *) skyeye_mm(size);
    memcpy(attr.u.data.data, data, size);
    return attr;
}

attr_value_t SKY_make_attr_data_adopt(size_t size, void *data)
{
    ATTR_ASSERT(data != NULL);
    attr_value_t attr;

    attr.type = Val_data;
    attr.u.data.adopt = True;
    attr.u.data.size = size;
    attr.u.data.data = data;
    return attr;
}

attr_value_t SKY_make_attr_list(unsigned length, ...)
{
    va_list ap;
    attr_value_t list;

    va_start(ap, length);
    list = SKY_make_attr_list_vararg(length, ap);
    va_end(ap);
    return list;
}

attr_value_t SKY_make_attr_list_vararg(unsigned length, va_list va)
{
    unsigned i;
    attr_value_t list = SKY_alloc_attr_list(length);

    for (i = 0; i < length; i++)
    {
        attr_value_t attr = va_arg(va, attr_value_t);

        ATTR_ASSERT(SKY_attr_is_invalid(attr) == False);
        SKY_attr_list_set_item(&list, i, attr);
    }
    return list;
}

void SKY_attr_free(attr_value_t * value)
{
    switch (value->type)
    {
        case Val_data:
            if (value->u.data.adopt == False && value->u.data.size > 0)
            {
                skyeye_free(value->u.data.data);
                value->u.data.data = NULL;
                value->u.data.size = 0;
            }
            break;
        case Val_string:
            if (value->u.string.adopt == False && value->u.string.str)
            {
                skyeye_free((void *) value->u.string.str);
                value->u.string.str = NULL;
            }
            break;
        case Val_list:
            SKY_attr_array_free(value->u.list.vector, value->u.list.size);
            value->u.list.vector = NULL;
            value->u.list.size = 0;
            break;
        case Val_dict:
            SKY_attr_array_free(value->u.dict.key, value->u.dict.size);
            SKY_attr_array_free(value->u.dict.val, value->u.dict.size);
            value->u.dict.key = NULL;
            value->u.dict.val = NULL;
            value->u.dict.size = 0;
            break;
        case Val_integer:
            value->u.integer = 0;
            break;
        case Val_uinteger:
            value->u.uinteger = 0;
            break;
        case Val_boolean:
            value->u.boolean = False;
            break;
        case Val_floating:
            value->u.floating = 0.0;
            break;
        case Val_object:
            value->u.object = NULL;
            break;
        case Val_nil:
        case Val_invalid:
            break;
        default:
            /* Wrong? ptr? */
            assert(0 && "wrong type");
            break;
    }
}

void SKY_free_attribute(attr_value_t value)
{
    switch (value.type)
    {
        case Val_data:
            if (value.u.data.adopt == False && value.u.data.size > 0)
            {
                skyeye_free(value.u.data.data);
            }
            break;
        case Val_string:
            if (value.u.string.adopt == False && value.u.string.str)
            {
                skyeye_free((void *) value.u.string.str);
            }
            break;
        case Val_list:
            SKY_attr_array_free(value.u.list.vector, value.u.list.size);
            break;
        case Val_dict:
            SKY_attr_array_free(value.u.dict.key, value.u.dict.size);
            SKY_attr_array_free(value.u.dict.val, value.u.dict.size);
            break;
        case Val_integer:
        case Val_uinteger:
        case Val_boolean:
        case Val_floating:
        case Val_object:
            break;
        case Val_nil:
        case Val_invalid:
            break;
        default:
            /* Wrong? ptr? */
            assert(0 && "wrong type");
            break;
    }
}

/**
* @brief Register an attribute to the system
*
* @param obj
* @param attr_name
* @param lcd_ctrl
* @param type
*/
void SKY_register_attr(conf_object_t * obj, const char *attr_name, attr_value_t * attr)
{
    char attr_objname[MAX_OBJNAME];

    get_strcat_objname(attr_objname, obj->objname, attr_name);
    if (new_map_object(attr_objname, attr, MapObject) != NULL)
    {
        //return No_exp;
        return;
    } else
    {
        //return Invarg_exp;
        return;
    }
}

/**
* @brief Get an attribute by its name
*
* @param conf_obj
* @param attr_name
* @param attr
*
* @return 
*/
attr_value_t *SKY_get_attr(conf_object_t * conf_obj, const char *attr_name)
{
    char attr_objname[MAX_OBJNAME];

    if (strlen(conf_obj->objname) + strlen(attr_name) + 1 > MAX_OBJNAME)
    {
        return NULL;
    }
    get_strcat_objname(&attr_objname[0], conf_obj->objname, attr_name);
    conf_object_t *attr_obj = get_conf_obj(attr_objname);

    return attr_obj->obj;
}

/**
* @brief Set the value for the given attribute
*
* @param conf_obj
* @param attr_name
* @param attr
*
* @return 
*/

set_attr_error_t SKY_set_attr(conf_object_t * conf_obj, const char *attr_name, attr_value_t * attr)
{
    char attr_objname[MAX_OBJNAME];

    if (strlen(conf_obj->objname) + strlen(attr_name) + 1 > MAX_OBJNAME)
    {
        return Set_error;
    }
    get_strcat_objname(&attr_objname[0], conf_obj->objname, attr_name);
    conf_object_t *attr_obj = get_conf_obj(attr_objname);

    if (attr_obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "The object %s not exist.\n", attr_objname);
        return Set_error;

    }
    attr_value_t *current_attr = attr_obj->obj;

    if (current_attr->type != attr->type)
    {
        return Set_invalid_type;
    }
    if (current_attr->type == Val_ptr)
    {
        current_attr->u.ptr = attr->u.ptr;
        return Set_ok;
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "No such type\n");
        return Set_error;
    }
}

/*
 * attr_value_t* make_new_attr(value_type_t type){
 * attr_value_t* attr = skyeye_mm(sizeof(attr_value_t));
 * attr->type = type;
 * if(type == Val_ptr){
 * return attr; 
 * }
 * else{
 * return NULL;
 * }
 * }
 */
exception_t set_conf_attr(conf_object_t * obj, char *attr_name, attr_value_t * value)
{
    conf_object_t *class_obj = get_conf_obj(obj->class_name);

    if (class_obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not find the object %s\n", obj->class_name);
        return Invarg_exp;
    }
    skyeye_class_t *class_data = class_obj->obj;

    if (class_data == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not find the class %s\n", obj->class_name);
        return Invarg_exp;
    }
    //int ret = class_data->set_attr(obj, attr_name, value);
    exception_t ret = SKY_set_attribute(obj, attr_name, value);

    return ret;
}

attr_value_t *make_new_attr(value_type_t type, void *value)
{
    attr_value_t *attr = skyeye_mm(sizeof (attr_value_t));

    switch (type)
    {
        case Val_Invalid:
            break;
        case Val_String:
            attr->type = type;
            attr->u.string.str = (const char *) value;
            break;
        case Val_Integer:
            attr->type = type;
            attr->u.integer = (int32_t) (size_t) value;
            break;
        case Val_UInteger:
            attr->type = type;
            attr->u.uinteger = (uint32_t) (size_t) value;
            break;
        case Val_Floating:
            attr->type = type;
            char *floatstring = (char *) (size_t) value;

            attr->u.floating = atof(floatstring);
            break;
        case Val_List:
            break;
        case Val_Data:
            break;
        case Val_Nil:
            break;
        case Val_Object:
            attr->type = type;
            attr->u.object = (conf_object_t *) value;
            break;
        case Val_Dict:
            break;
        case Val_Boolean:
            attr->type = type;
            attr->u.boolean = (bool_t) value;
            break;
        case Val_ptr:
            attr->type = type;
            attr->u.ptr = (void *) value;
            break;
        default:
            break;
    }
    return attr;
}

exception_t get_uint_attrlist(conf_object_t * obj, char *attrname, int *ptr, unsigned int size)
{
    attr_value_t value, item;
    int count, i;
    unsigned int item_value;

    value = SKY_get_attribute(obj, attrname);
    count = SKY_attr_list_size(value);

    count = count > size ? size : count;
    for (i = 0; i < count; i++)
    {
        item = SKY_attr_list_item(value, i);
        item_value = SKY_attr_uinteger(item);
        *(ptr + i) = item_value;
    }
    return No_exp;
}

exception_t set_uint_attrlist(conf_object_t * obj, char *attrname, int *ptr, unsigned int size)
{
    attr_value_t value, item;
    int count, i;
    unsigned int item_value;

    value = SKY_get_attribute(obj, attrname);
    count = SKY_attr_list_size(value);

    count = count > size ? size : count;
    attr_value_t list_value = SKY_alloc_attr_list(count);

    for (i = 0; i < count; i++)
    {
        item = SKY_make_attr_uinteger(ptr[i]);
        SKY_attr_list_set_item(&value, i, item);

    }
    return SKY_set_attribute(obj, attrname, &value);

}

exception_t get_uint_attr(conf_object_t * obj, char *attrname, unsigned int *ptr)
{
    attr_value_t item;
    unsigned int value;

    if (SKY_has_attribute(obj, attrname) != No_exp)
        return Not_found_exp;
    item = SKY_get_attribute(obj, attrname);
    value = SKY_attr_uinteger(item);
    *ptr = value;
    return No_exp;
}

exception_t set_uint_attr(conf_object_t * obj, char *attrname, unsigned int value)
{
    attr_value_t item;

    item = SKY_make_attr_uinteger(value);
    if (SKY_has_attribute(obj, attrname) != No_exp)
        return Not_found_exp;
    return SKY_set_attribute(obj, attrname, &item);
}
