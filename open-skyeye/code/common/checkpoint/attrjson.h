#ifndef __ATTRJSON_H__
#define __ATTRJSON_H__

#include "portable/json.h"
#include "skyeye_attr.h"

int JsonToAttr(struct json_object *json_attr, attr_value_t & attr);
struct json_object *AttrToJson(attr_value_t value);

#endif
