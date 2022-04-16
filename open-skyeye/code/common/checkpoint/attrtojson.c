#include <stdio.h>
#include "portable/json.h"
#include "skyeye_types.h"
#include "skyeye_class.h"
#include "skyeye_mm.h"
#include "skyeye_attribute.h"

/* compiled by c90 */
#ifdef json_object_object_foreach
#undef json_object_object_foreach
#define json_object_object_foreach(obj, key, val) \
	char *key; struct json_object *val; struct lh_entry *entry; \
	for(entry = json_object_get_object(obj)->head; ({ if(entry) { key = (char*)entry->k; val = (struct json_object*)entry->v; } ; entry; }); entry = entry->next )
#endif

static struct json_object *integer_attr_to_json(attr_value_t * attr)
{
    struct josn_object *json_attr = json_object_new_object();
    struct josn_object *tmp = json_object_new_int(attr->u.integer);

    json_object_object_add(json_attr, "type", json_object_new_int(Val_Integer));
    json_object_object_add(json_attr, "value", tmp);
    return json_attr;
}

static struct json_object *uinteger_attr_to_json(attr_value_t * attr)
{
    struct josn_object *json_attr = json_object_new_object();
    struct josn_object *tmp = json_object_new_int(attr->u.uinteger);

    json_object_object_add(json_attr, "type", json_object_new_int(Val_UInteger));
    json_object_object_add(json_attr, "value", tmp);
    return json_attr;
}

static struct json_object *string_attr_to_json(attr_value_t * attr)
{
    struct josn_object *json_attr = json_object_new_object();
    struct josn_object *tmp = json_object_new_string(attr->u.string.str);

    json_object_object_add(json_attr, "type", json_object_new_int(Val_String));
    json_object_object_add(json_attr, "value", tmp);
    return json_attr;
}
static struct json_object *list_item_to_json(attr_value_t * attr)
{
    struct json_object *tmp;

    switch (attr->type)
    {
        case Val_Integer:
            tmp = json_object_new_int(attr->u.integer);
            break;
        case Val_UInteger:
            tmp = json_object_new_int(attr->u.uinteger);
            break;
        case Val_Floating:
            tmp = json_object_new_double(attr->u.floating);
            break;
        case Val_ptr:
            break;
        case Val_String:
            tmp = json_object_new_string(attr->u.string.str);
            break;
        default:
            tmp = NULL;
    }
    return tmp;
}

static struct json_object *list_attr_to_json(attr_value_t * attr)
{
    int count, i;
    attr_value_t item;
    struct json_object *tmp;
    struct json_object *json_attr = json_object_new_object();
    struct json_object *list = json_object_new_array();

    count = SKY_attr_list_size(*attr);
    for (i = 0; i < count; i++)
    {
        item = SKY_attr_list_item(*attr, i);
        tmp = list_item_to_json(&item);
        if (tmp)
            json_object_array_add(list, tmp);
    }
    if (count >= 1)
    {
        item = SKY_attr_list_item(*attr, 0);
        json_object_object_add(json_attr, "item_type", json_object_new_int(item.type));
    }
    json_object_object_add(json_attr, "type", json_object_new_int(Val_List));
    json_object_object_add(json_attr, "value", list);
    return json_attr;
}

static struct json_object *data_attr_to_json(attr_value_t * attr)
{
    int count, i;
    attr_value_t item;
    struct json_object *tmp;
    struct json_object *json_attr = json_object_new_object();
    struct json_object *list = json_object_new_array();
    char *data;

    count = SKY_attr_data_size(*attr);
    data = SKY_attr_data(*attr);
    for (i = 0; i < count; i++)
    {
        tmp = json_object_new_int(*(data + i));
        if (tmp)
            json_object_array_add(list, tmp);
    }
    json_object_object_add(json_attr, "type", json_object_new_int(Val_Data));
    json_object_object_add(json_attr, "value", list);
    return json_attr;
}
static struct json_object *attr_to_json(attr_value_t * attr)
{
    struct json_object *tmp;

    switch (attr->type)
    {
        case Val_UInteger:
            tmp = uinteger_attr_to_json(attr);
            break;
        case Val_Integer:
            tmp = integer_attr_to_json(attr);
            break;
        case Val_List:
            tmp = list_attr_to_json(attr);
            break;
        case Val_String:
            tmp = string_attr_to_json(attr);
            break;
        case Val_Data:
            tmp = data_attr_to_json(attr);
            break;
        default:
            tmp = NULL;
    }
    return tmp;
}

struct json_object *object_to_json(conf_object_t * obj)
{
    struct json_object *json_obj = NULL, *tmp;
    attr_value_t attr;
    char **attrs;

    json_obj = json_object_new_object();
    conf_class_t *cls = SKY_get_class(obj->class_name);

    if (cls == NULL)
        return NULL;
    attrs = get_class_attributes(cls);
    while (*attrs)
    {
        attr = SKY_get_attribute(obj, *attrs);
        tmp = attr_to_json(&attr);
        if (tmp)
            json_object_object_add(json_obj, *attrs, tmp);
        else
            printf("%s attibuate convert json object fail\n", *attrs);
        attrs++;
    }
    return json_obj;
}

exception_t store_to_configure(char *filename)
{
    struct josn_object *json_root, *tmp;
    conf_object_t *obj;
    uint32_t count, i;

    json_root = json_object_new_object();
    count = SKY_get_obj_count();
    for (i = 0; i < count; i++)
    {
        obj = SKY_get_obj_index(i);
        tmp = object_to_json(obj);
        if (tmp)
            json_object_object_add(json_root, obj->objname, tmp);
        else
            printf("%s convert json object fail\n", obj->objname);
    }
    json_object_to_file(filename, json_root);
    return No_exp;
}

attr_value_t json_value_to_uinteger_attr(struct json_object * json_value)
{
    uint32_t val = json_object_get_int(json_value);

    return SKY_make_attr_uinteger(val);
}

attr_value_t json_value_to_integer_attr(struct json_object * json_value)
{
    uint32_t val = json_object_get_int(json_value);

    return SKY_make_attr_integer(val);
}

attr_value_t json_value_to_string_attr(struct json_object * json_value)
{
    char *val = json_object_get_string(json_value);

    return SKY_make_attr_string(val);
}

attr_value_t json_value_to_list_attr(value_type_t type, struct json_object * json_value)
{
    int count, i;
    struct json_object *item;
    attr_value_t value;

    count = json_object_array_length(json_value);
    attr_value_t list = SKY_alloc_attr_list(count);

    for (i = 0; i < count; i++)
    {
        item = json_object_array_get_idx(json_value, i);
        switch (type)
        {
            case Val_UInteger:
                value = json_value_to_uinteger_attr(item);
                break;
            case Val_Integer:
                value = json_value_to_integer_attr(item);
                break;
            case Val_String:
                value = json_value_to_string_attr(item);
                break;
              defalut:
                printf("Unknow attribue type!");
        }
        SKY_attr_list_set_item(&list, i, value);
    }
    return list;
}

attr_value_t json_value_to_data_attr(struct json_object * json_value)
{
    int count, i;
    char *data_ptr;
    struct json_object *item;
    attr_value_t value;

    count = json_object_array_length(json_value);
    data_ptr = skyeye_mm_zero(count);
    for (i = 0; i < count; i++)
    {
        item = json_object_array_get_idx(json_value, i);
        *(data_ptr + i) = json_object_get_int(item);
    }
    attr_value_t data = SKY_make_attr_data(count, data_ptr);

    skyeye_free(data_ptr);
    return data;
}

attr_value_t json_value_to_attr(struct json_object * attr_obj)
{
    struct josn_object *type_obj;
    struct josn_object *value_obj;
    struct josn_object *item_type_obj;
    attr_value_t value;

    type_obj = json_object_object_get(attr_obj, "type");
    value_obj = json_object_object_get(attr_obj, "value");
    value_type_t type;
    value_type_t item_type;

    type = (value_type_t) json_object_get_int(type_obj);
    switch (type)
    {
        case Val_UInteger:
            value = json_value_to_uinteger_attr(value_obj);
            break;
        case Val_Integer:
            value = json_value_to_integer_attr(value_obj);
            break;
        case Val_String:
            value = json_value_to_string_attr(value_obj);
            break;
        case Val_List:
            item_type_obj = json_object_object_get(attr_obj, "item_type");
            item_type = (value_type_t) json_object_get_int(item_type_obj);
            value = json_value_to_list_attr(item_type, value_obj);
            break;
        case Val_Data:
            value = json_value_to_data_attr(value_obj);
            break;
        default:
            printf("Unknow attribue type!");
    }
    return value;
}

exception_t json_to_attr(char *objname, char *attrname, struct json_object * attr_obj)
{
    attr_value_t val = json_value_to_attr(attr_obj);
    conf_object_t *obj = get_conf_obj(objname);

    if (obj == NULL)
    {
        printf("Can not get obj for %s\n", objname);
        return Not_found_exp;
    }
    SKY_set_attribute(obj, attrname, &val);
    return No_exp;
}

exception_t json_to_object(char *objname, struct json_object * json_obj)
{
    json_object_object_foreach(json_obj, key, attr_obj)
    {
        json_to_attr(objname, key, attr_obj);
    }
    return No_exp;
}

exception_t recover_from_configure(char *filename)
{
    struct json_object *json_root;

    json_root = json_object_from_file(filename);
    json_object_object_foreach(json_root, key, obj)
    {
        json_to_object(key, obj);
    }
    return No_exp;
}
