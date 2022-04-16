#include <iostream>
#include <map>

#include <stdlib.h>
#include <string.h>

#include "checkdumpobj.h"
#include "attrjson.h"

using namespace std;

#define TYPE_NAME_DEF(old, alias) #alias,
const char *type_str[] = {
    VALUE_TYPE_LIST(TYPE_NAME_DEF)
};

#undef TYPE_NAME_DEF
const char *DefaultDumpObjectName = "Global";

DumpObject::DumpObject(const char *objname, struct json_object *json_obj)
{
    if (objname == NULL)
    {
        name = DefaultDumpObjectName;
    } else
    {
        name = objname;
    }
    if (json_obj)
    {
        fromJson(json_obj);
    } else
    {
        AttrList.clear();
        Children.clear();
    }
}

DumpObject::~DumpObject(void)
{
    int i;

    for (i = 0; i < AttrList.size(); i++)
    {
        SKY_free_attribute(AttrList[i].attr);
    }
    AttrList.clear();
    Children.clear();
}

size_t DumpObject::getAttrs(AttrPair * &list)
{
    size_t len = AttrList.size();
    AttrPair *tmplist;

    if (len > 0)
    {
        tmplist = new AttrPair[len];
        for (int i = 0; i < len; i++)
        {
            tmplist[i] = AttrList[i];
        }
        list = tmplist;
    }
    return len;
}

int DumpObject::getAttr(string key, attr_value_t & attr)
{
    AttrList_t::iterator it;
    for (it = AttrList.begin(); it != AttrList.end(); it++)
    {
        if (it->key == key)
        {
            break;
        }
    }
    if (it == AttrList.end())
    {
        return -1;
    }
    attr = it->attr;
    return 0;
}

size_t DumpObject::getChildren(DumpObject * &list)
{
    size_t len = Children.size();
    DumpObject *tmplist;

    if (len > 0)
    {
        tmplist = new DumpObject[len];
        for (int i = 0; i < len; i++)
        {
            tmplist[i] = Children[i];
        }
        list = tmplist;
    }
    return len;
}

int DumpObject::getChild(string n, DumpObject & obj)
{
    vector < DumpObject >::iterator it;
    for (it = Children.begin(); it != Children.end(); it++)
    {
        if (it->name == n)
        {
            break;
        }
    }
    if (it == Children.end())
    {
        return -1;
    }
    obj = *it;
    return 0;
}

struct json_object *DumpObject::toJson(void)
{
    struct json_object *root = json_object_new_object();
    struct json_object *attrsroot = json_object_new_object();
    struct json_object *objsroot = json_object_new_object();

    AttrList_t::iterator attrit;
    for (attrit = AttrList.begin(); attrit != AttrList.end(); attrit++)
    {
        json_object_object_add(attrsroot, attrit->key.c_str(), AttrToJson(attrit->attr));
    }
    vector < DumpObject >::iterator objit;
    for (objit = Children.begin(); objit != Children.end(); objit++)
    {
        json_object_object_add(objsroot, objit->name.c_str(), objit->toJson());
    }

    json_object_object_add(root, "objs", objsroot);
    json_object_object_add(root, "attrs", attrsroot);
    return root;
}

int DumpObject::fromJson(struct json_object *json_obj)
{
    if (!json_object_is_type(json_obj, json_type_object))
    {
        /* a DumpObject descripted by a json object */
        cerr << name << ": json format descripting DumpObject is error\n" << endl;
        return -1;
    }

    struct json_object *objs = json_object_object_get(json_obj, "objs");

    if (!json_object_is_type(objs, json_type_object))
    {
        /* a objs descripted by a json object */
        cerr << name << ": json format descripting objs is error\n" << endl;
        return -1;
    }
    json_object_object_foreach(objs, objkey, objval)
    {
        Children.push_back(DumpObject(objkey, objval));
    }

    struct json_object *attrs = json_object_object_get(json_obj, "attrs");

    if (!json_object_is_type(attrs, json_type_object))
    {
        /* a attrs descripted by a json object */
        cerr << name << ": json format descripting attrs is error\n" << endl;
        return -1;
    }
    json_object_object_foreach(attrs, attrkey, attrval)
    {
        attr_value_t attrval_val;

        if (JsonToAttr(attrval, attrval_val) < 0)
        {
            cerr << name << ": json format descripting " << attrkey << " is error\n" << endl;
            return -1;
        }
        addAttr(attrkey, attrval_val);
    }
    return 0;
}

#if 0
int main(int argc, const char *argv[])
{
    DumpObject dumpobj;
    attr_value_t a, b, c, d, e, f, g, h, i, j, k;

    a.type = Val_boolean;
    a.u.boolean = True;
    dumpobj.addAttr("testbool", a);

    b.type = Val_nil;
    dumpobj.addAttr("testnil", b);

    c.type = Val_floating;
    c.u.floating = 0.36;
    dumpobj.addAttr("testfloat", c);

    d.type = Val_uinteger;
    d.u.uinteger = 0x1000;
    dumpobj.addAttr("testuint", d);

    e.type = Val_integer;
    e.u.integer = -1000;
    dumpobj.addAttr("testint", e);

    f.type = Val_string;
    f.u.string.str = "你好hello world";
    dumpobj.addAttr("teststring", f);

    g.type = Val_invalid;
    dumpobj.addAttr("testinvalid", g);

    h.type = Val_data;
    const char teststring[] = "你好new world1234";

    h.u.data.data = (uint8_t *) teststring;
    h.u.data.size = sizeof (teststring);
    dumpobj.addAttr("testdata", h);

    i.type = Val_object;
    conf_object_t obj = {.objname = (char *) "uart" };
    i.u.object = &obj;
    dumpobj.addAttr("testobject", i);

    j.type = Val_list;
    j.u.list.size = 4;
    j.u.list.vector = new attr_value_t[4];
    j.u.list.vector[0] = i;
    j.u.list.vector[1] = h;
    j.u.list.vector[2] = g;
    j.u.list.vector[3] = f;
    dumpobj.addAttr("testlist", j);

    k.type = Val_dict;
    k.u.dict.size = 4;
    k.u.dict.key = new attr_value_t[4];
    k.u.dict.val = new attr_value_t[4];
    k.u.dict.key[0] = i;
    k.u.dict.val[0] = i;
    k.u.dict.key[1] = h;
    k.u.dict.val[1] = h;
    k.u.dict.key[2] = g;
    k.u.dict.val[2] = g;
    k.u.dict.key[3] = f;
    k.u.dict.val[3] = f;
    dumpobj.addAttr("testdict", k);

    dumpobj.addChild(dumpobj);
    struct json_object *root = dumpobj.toJson();

    cout << json_object_get_string(root) << endl;

    struct json_object *head = json_tokener_parse(json_object_get_string(root));
    DumpObject dp(NULL, head);

    cout << json_object_get_string(dp.toJson()) << endl;

    return 0;
}
#endif
