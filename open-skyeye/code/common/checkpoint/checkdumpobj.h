#ifndef __CHECKDUMP_H__
#define __CHECKDUMP_H__

#include <vector>
#include <string>
#include "portable/json.h"
#include "skyeye_attr.h"

struct AttrPair
{
    std::string key;
    attr_value_t attr;
};
typedef std::vector < AttrPair > AttrList_t;

extern const char *type_str[];
extern const char *DefaultDumpObjectName;

class DumpObject
{
  public:
    DumpObject(void)
    {
        Children.clear();
        AttrList.clear();
        name = DefaultDumpObjectName;
    }
    DumpObject(const char *objname, struct json_object *json_obj = NULL);
    ~DumpObject(void);

    void addChild(const DumpObject & child)
    {
        Children.push_back(child);
    }
    void addAttr(std::string key, attr_value_t attr)
    {
        AttrPair pair = { key, SKY_attr_copy(attr) };
        AttrList.push_back(pair);
    }
    size_t getAttrs(AttrPair * &list);
    int getAttr(std::string key, attr_value_t & attr);
    size_t getChildren(DumpObject * &list);
    int getChild(std::string n, DumpObject & obj);

    void setName(std::string n)
    {
        name = n;
    }
    std::string getName(void)
    {
        return name;
    }

    struct json_object *toJson(void);
    int fromJson(struct json_object *json);

  private:
    std::string name;
    AttrList_t AttrList;

    std::vector < DumpObject > Children;
};

#endif
