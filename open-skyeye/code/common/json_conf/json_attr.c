/*
 * © Copyright 2014 eJim Lee. All Rights Reserved.
 * File:    json_attr.c
 * Email:   lyjforjob@gmail.com
 * Date:    2014-02-11
 */

#include <stdio.h>
#include "json_attr.h"
#include "json_parser.h"
#include "json_base.h"
#include <skyeye_types.h>
#include <skyeye_obj.h>
#include <skyeye_attr.h>
#include <skyeye_log.h>
#include <skyeye_pref.h>
#include <skyeye_config.h>
/* NOTE: you should add new special attribute info into attribute
 * list on json_attr.h and set function on this file
 */

#if 0
#define DBG(fmt, ...) fprintf(stdout, fmt, ##__VA_ARGS__)
#else
#define DBG(fmt, ...)
#endif

static int cpu_type_set(struct obj_desc *obj, struct attr_desc *attr, void *extra)
{
    DBG("%s\n", __func__);
    return 0;
}

static int device_address_set(struct obj_desc *obj, struct attr_desc *attr, void *extra)
{
    DBG("%s\n", __func__);
    return 0;
}

static int device_length_set(struct obj_desc *obj, struct attr_desc *attr, void *extra)
{
    DBG("%s\n", __func__);
    return 0;
}

#define CHECK_ATTR_TYPE(str) \
	if(strcmp(attr->type, str) == 0)
#define CHECK_ATTR_TYPE_NOT_IMPLEMENT(str) \
	do{ \
		fprintf(stderr, "attribute not implement type %s\n", str); \
		return -1; \
	}while(0)

static int obj_attr_set(struct obj_desc *obj, struct attr_desc *attr, void *extra)
{
    printf("set attr, name:%s, key: %s, type:%s, value:%s\n", obj->name, attr->key, attr->type, attr->val);
    skyeye_config_t *config = get_current_config();
    attr_value_t *attr_after_making;
    uint32_t attr_integer, base;
    char parity_val;

    if (!strncmp(attr->type, "string", strlen("string")))
    {
        attr_after_making = make_new_attr(Val_String, attr->val);
        if (!strncmp(attr->key, "parity", strlen("parity")))
        {
            attr_after_making = make_new_attr(Val_Integer, attr->val[0]);
        }
        if (!strncmp(attr->key, "elf", strlen("elf")))
        {
            conf_object_t *soc = get_conf_obj(obj->name);

            register_soc_elfname(soc, attr->val);
            return 0;
        }
        if (!strncmp(attr->key, "brate", strlen("brate")))
        {
            return 0;
        }
    } else if (!strncmp(attr->type, "uinteger", strlen("uinteger")))
    {
        if (!strncmp(attr->val, "0x", 2))
        {
            base = 16;
        } else
        {
            base = 10;
        }
        if (!strncmp(attr->key, "brate", strlen("brate")))
        {
            return 0;
        }
        attr_integer = strtol(attr->val, NULL, base);
        attr_after_making = make_new_attr(Val_UInteger, attr_integer);
    } else if (!strncmp(attr->type, "integer", strlen("integer")))
    {
    } else if (!strncmp(attr->type, "floating", strlen("floating")))
    {
    } else if (!strncmp(attr->type, "list", strlen("list")))
    {
    } else if (!strncmp(attr->type, "data", strlen("data")))
    {
    } else if (!strncmp(attr->type, "nil", strlen("nil")))
    {
    } else if (!strncmp(attr->type, "object", strlen("object")))
    {
        conf_object_t *obj = get_conf_obj(attr->val);

        attr_after_making = make_new_attr(Val_Object, obj);
    } else if (!strncmp(attr->type, "dict", strlen("dict")))
    {
    } else if (!strncmp(attr->type, "boolean", strlen("boolean")))
    {
    } else if (!strncmp(attr->type, "ptr", strlen("ptr")))
    {
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "Unsupport attr type: %s\n", attr->type);
        exit(0);
    }

    conf_object_t *device = get_conf_obj(obj->name);

    set_conf_attr(device, attr->key, attr_after_making);
    DBG("%s\n", __func__);
    return 0;
}

#define ATTR_SPEC_INTF(base, k, t) \
	{ \
		.key = #k, \
		.type = #t, \
		.set = base##_##k##_set, \
	},

#define ATTR_SPEC_LIST_DEF(base) \
	attr_spec_intf base##_attr_spec_list_in[] = { \
		base##_ATTR_SPEC_LIST(ATTR_SPEC_INTF) \
		{ \
			.key = NULL, \
			.type = NULL, \
			.set = obj_attr_set, \
		}, \
	}; \
	attr_spec_intf *base##_attr_spec_list = base##_attr_spec_list_in;

BASE_LIST(ATTR_SPEC_LIST_DEF)
