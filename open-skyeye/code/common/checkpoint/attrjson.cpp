#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include <stdlib.h>
#include <string.h>

#include "checkdumpobj.h"
#include "attrjson.h"
#include "codebase64.h"

using namespace std;

#define JSONTOATTR_DECL(old, alias) \
	int JsonToAttr_##alias(struct json_object *json_attr, attr_value_t &attr);
VALUE_TYPE_LIST(JSONTOATTR_DECL)
#undef JSONTOATTR_DECL
    typedef int (*JsonToAttrFunc) (struct json_object *, attr_value_t &);
    typedef map < string, JsonToAttrFunc > JsonToAttrTypeMap;

/* define a vector table of jsontodump_type function */
#define JSONTOATTR_DEF(old, alias) pair<string, JsonToAttrFunc>(#alias, JsonToAttr_##alias),
    static pair < string, JsonToAttrFunc > JsonToAttrTypeList[] = {
        VALUE_TYPE_LIST(JSONTOATTR_DEF)
    };
JsonToAttrTypeMap JsonToAttrType(JsonToAttrTypeList,
                                 JsonToAttrTypeList + sizeof (JsonToAttrTypeList) / sizeof (JsonToAttrTypeList[0]));
#undef JSONTOATTR_DEF

int JsonToAttr_invalid(struct json_object *json_attr, attr_value_t & attr)
{
    attr = SKY_make_attr_invalid();
    return 0;
}

int JsonToAttr_string(struct json_object *json_attr, attr_value_t & attr)
{
    struct json_object *attrval = json_object_array_get_idx(json_attr, 1);

    if (!json_object_is_type(attrval, json_type_string))
    {
        cerr << "json string description is error, type of attr value not expect(string)" << endl;
        return -1;
    }
    const char *str = json_object_get_string(attrval);

    attr = SKY_make_attr_string(str);
    return 0;
}

int JsonToAttr_integer(struct json_object *json_attr, attr_value_t & attr)
{
    struct json_object *attrval = json_object_array_get_idx(json_attr, 1);

    if (!json_object_is_type(attrval, json_type_string))
    {
        cerr << "json integer description is error, type of attr value not expect(string)" << endl;
        return -1;
    }
    const char *str = json_object_get_string(attrval);

    attr = SKY_make_attr_integer(atoll(str));
    return 0;
}

int JsonToAttr_uinteger(struct json_object *json_attr, attr_value_t & attr)
{
    struct json_object *attrval = json_object_array_get_idx(json_attr, 1);

    if (!json_object_is_type(attrval, json_type_string))
    {
        cerr << "json uinteger description is error, type of attr value not expect(string)" << endl;
        return -1;
    }
    const char *str = json_object_get_string(attrval);

    attr = SKY_make_attr_uinteger(atoll(str));
    return 0;
}

int JsonToAttr_floating(struct json_object *json_attr, attr_value_t & attr)
{
    struct json_object *attrval = json_object_array_get_idx(json_attr, 1);

    if (!json_object_is_type(attrval, json_type_string))
    {
        cerr << "json floating description is error, type of attr value not expect(string)" << endl;
        return -1;
    }
    const char *str = json_object_get_string(attrval);

    attr = SKY_make_attr_floating(strtod(str, NULL));
    return 0;
}

int JsonToAttr_data(struct json_object *json_attr, attr_value_t & attr)
{
    struct json_object *attrval = json_object_array_get_idx(json_attr, 1);

    if (!json_object_is_type(attrval, json_type_string))
    {
        cerr << "json data description is error, type attr value not expect(string)" << endl;
        return -1;
    }

    const char *str = json_object_get_string(attrval);
    void *data;
    size_t size = decodeBase64(str, &data);

    attr = SKY_make_attr_data(size, data);
    if (size > 0)
    {
        delete(uint8_t *) data;
    }
    return 0;
}

int JsonToAttr_object(struct json_object *json_attr, attr_value_t & attr)
{
    struct json_object *attrval = json_object_array_get_idx(json_attr, 1);

    if (!json_object_is_type(attrval, json_type_string))
    {
        cerr << "json object description is error, type of attr value not expect(string)" << endl;
        return -1;
    }
#warning "object is not implement"
    const char *str = json_object_get_string(attrval);
    conf_object_t *tmp = new conf_object_t;

    tmp->objname = strdup(str);
    attr = SKY_make_attr_object(tmp);
    return 0;
}

int JsonToAttr_nil(struct json_object *json_attr, attr_value_t & attr)
{
    attr = SKY_make_attr_nil();
    return 0;
}

int JsonToAttr_list(struct json_object *json_attr, attr_value_t & attr)
{
    struct json_object *attrval = json_object_array_get_idx(json_attr, 1);

    if (!json_object_is_type(attrval, json_type_array))
    {
        cerr << "json list description is error, type of attr value not expect(array)" << endl;
        return -1;
    }

    /* setting list length */
    int array_len = json_object_array_length(attrval);

    attr = SKY_alloc_attr_list(array_len);

    /* setting list element */
    for (int i = 0; i < array_len; i++)
    {
        struct json_object *item = json_object_array_get_idx(attrval, i);
        attr_value_t itemval;

        if (JsonToAttr(item, itemval) < 0)
        {
            return -1;
        }
        SKY_attr_list_set_item(&attr, i, itemval);
    }
    return 0;
}

int JsonToAttr_dict(struct json_object *json_attr, attr_value_t & attr)
{
    struct json_object *attrval = json_object_array_get_idx(json_attr, 1);

    if (!json_object_is_type(attrval, json_type_array))
    {
        cerr << "json dict description is error, type of attr value not expect(array)" << endl;
        return -1;
    }

    int array_len = json_object_array_length(attrval);

    attr = SKY_alloc_attr_dict(array_len);
    for (int i = 0; i < array_len; i++)
    {
        struct json_object *item = json_object_array_get_idx(attrval, i);

        /* check item format */
        if (!json_object_is_type(item, json_type_array) || json_object_array_length(item) != 2)
        {
            cerr << "json dict description is error, type of dict item not expect(array[2])" << endl;
            return -1;
        }
        /* get item value */
        struct json_object *itemkey = json_object_array_get_idx(item, 0);
        struct json_object *itemval = json_object_array_get_idx(item, 1);
        attr_value_t itemkey_val, itemval_val;

        if (JsonToAttr(itemkey, itemkey_val) < 0 || JsonToAttr(itemval, itemval_val) < 0)
        {
            return -1;
        }
        SKY_attr_dict_set_item(&attr, i, itemkey_val, itemval_val);
    }
    return 0;
}

int JsonToAttr_boolean(struct json_object *json_attr, attr_value_t & attr)
{
    struct json_object *attrval = json_object_array_get_idx(json_attr, 1);

    if (!json_object_is_type(attrval, json_type_boolean))
    {
        cerr << "json attr description is error, type of attr value not expect(boolean)" << endl;
        return -1;
    }
    attr = SKY_make_attr_boolean(json_object_get_boolean(attrval) ? True : False);
    return 0;
}

int JsonToAttr(struct json_object *json_attr, attr_value_t & attr)
{
    if ((!json_object_is_type(json_attr, json_type_array)) || (json_object_array_length(json_attr) > 2))
    {
        /* attr descripted by json array like as ["type", value] or ["invalid"] */
        cerr << "format of json descripting attr is error" << endl;
        return -1;
    }

    /* get attr type */
    const char *type = json_object_get_string(json_object_array_get_idx(json_attr, 0));

    JsonToAttrTypeMap::iterator it = JsonToAttrType.find(type);
    if (it == JsonToAttrType.end())
    {
        cerr << "type format of json descripting attr is error, unknown attr type " << type << endl;
        return -1;
    }

    if (it->second(json_attr, attr) < 0)
    {
        return -1;
    }
}

struct json_object *AttrToJson(attr_value_t attr)
{
    struct json_object *type = json_object_new_string(type_str[attr.type]);
    struct json_object *value = NULL;
    stringstream ss;

    switch (attr.type)
    {
        case (Val_invalid):
            break;
        case (Val_string):
            value = json_object_new_string(attr.u.string.str);
            break;
        case (Val_integer):
            ss << attr.u.integer;
            value = json_object_new_string(ss.str().c_str());
            break;
        case (Val_uinteger):
            ss << attr.u.uinteger;
            value = json_object_new_string(ss.str().c_str());
            break;
        case (Val_floating):
            ss << attr.u.floating;
            value = json_object_new_string(ss.str().c_str());
            break;
        case (Val_list):
            {
                value = json_object_new_array();
                for (int i = 0; i < attr.u.list.size; i++)
                {
                    struct json_object *tmp = AttrToJson(attr.u.list.vector[i]);

                    json_object_array_add(value, tmp);
                }
                break;
            }
        case (Val_data):
            {
                char *tmp = encodeBase64(attr.u.data.data, attr.u.data.size);

                value = json_object_new_string(tmp);
                delete tmp;

                break;
            }
        case (Val_object):
            value = json_object_new_string(attr.u.object->objname);
            break;
        case (Val_nil):
            break;
        case (Val_dict):
            {
                value = json_object_new_array();
                for (int i = 0; i < attr.u.dict.size; i++)
                {
                    struct json_object *tmp = json_object_new_array();
                    struct json_object *key = AttrToJson(attr.u.dict.key[i]);
                    struct json_object *val = AttrToJson(attr.u.dict.val[i]);

                    json_object_array_add(tmp, key);
                    json_object_array_add(tmp, val);
                    json_object_array_add(value, tmp);
                }
                break;
            }
        case (Val_boolean):
            value = json_object_new_boolean(attr.u.boolean);
            break;
        default:
            cerr << "How fuck something happend is!" << endl;
            break;
    }
    struct json_object *pair = json_object_new_array();

    json_object_array_add(pair, type);
    if (value)
    {
        json_object_array_add(pair, value);
    }
    return pair;
}
