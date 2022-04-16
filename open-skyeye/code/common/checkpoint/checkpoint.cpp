#include <iostream>
#include "portable/json.h"

#include "skyeye_types.h"
#include "skyeye_class.h"
#include "skyeye_attribute.h"
#include "skyeye_checkpoint.h"
#include "check.h"
#include "checkdumpobj.h"

using namespace std;

static DumpObject *configuration_to_object(const char *file)
{
    json_object *json = json_object_from_file((char *) file);

    if (json == NULL)
    {
        return NULL;
    }
    DumpObject *root = new DumpObject();

    root->fromJson(json);
    return root;
}

static void object_to_configuration(DumpObject & obj, const char *file)
{
    struct json_object *json = obj.toJson();

    json_object_to_file((char *) file, json);
}

static void restore_object_attributes(DumpObject & dumpobj)
{
    conf_object_t *obj = (conf_object_t *) get_conf_obj((char *) dumpobj.getName().c_str());

    if (obj == NULL)
    {
        cerr << "Can't find " << dumpobj.getName() << endl;
        return;
    }
    AttrPair *attrs;
    size_t attrslen = dumpobj.getAttrs(attrs);
    conf_class_t *clss = (conf_class_t *) obj->clss;

    if (attrslen > 0)
    {
        for (size_t i = 0; i < attrslen; i++)
        {
            /* FIXME: maybe we should check a attr key belong to object or class ??? */
            SKY_set_attribute(obj, attrs[i].key.c_str(), &attrs[i].attr);
            SKY_set_class_attribute(clss, attrs[i].key.c_str(), &attrs[i].attr);
        }
        delete[]attrs;
    }
}

static void restore_object(DumpObject & dumpobj)
{
    DumpObject *objs;
    size_t childrenlen = dumpobj.getChildren(objs);

    if (childrenlen > 0)
    {
        for (size_t i = 0; i < childrenlen; i++)
        {
            restore_object(objs[i]);
            restore_object_attributes(objs[i]);
        }
        delete[]objs;
    }
}

static DumpObject *snapshot_object_attributes(conf_object_t * srcobj)
{
    const char **attrs, *key;
    attr_value_t value;

    conf_class_t *clss = (conf_class_t *) srcobj->clss;
    DumpObject *dumpobj = new DumpObject(srcobj->objname);

    attrs = get_attributes(srcobj);
    if (attrs)
    {
        while (*attrs)
        {
            key = *attrs;
            value = SKY_get_attribute(srcobj, key);
            dumpobj->addAttr(key, value);
            SKY_free_attribute(value);
            attrs++;
        }
    }

    attrs = get_class_attributes(clss);
    if (attrs)
    {
        while (*attrs)
        {
            key = *attrs;
            value = SKY_get_class_attribute(clss, key);
            dumpobj->addAttr(key, value);
            SKY_free_attribute(value);
            attrs++;
        }
    }
    return dumpobj;
}

static exception_t get_conf_obj_attribute(conf_object_t * obj, void *extra)
{
    DumpObject *root = (DumpObject *) extra;
    conf_class_t *clss = (conf_class_t *) obj->clss;

    if (obj->type == ConfObject && clss->class_data.kind == Class_Persistent)
    {
        DumpObject *child = snapshot_object_attributes(obj);

        root->addChild(*child);
    }
    return No_exp;
}

static void snapshot_object(DumpObject & dumpobj)
{
    travel_conf_obj(get_conf_obj_attribute, (void *) &dumpobj);
}

void write_configuration_to_file(const char *file, save_flag_t flag)
{
    DumpObject *root = new DumpObject();

    snapshot_object(*root);
    object_to_configuration(*root, file);
}

void read_configuration(const char *file)
{
    DumpObject *root = configuration_to_object(file);

    if (root == NULL)
    {
        return;
    }
    restore_object(*root);
}
