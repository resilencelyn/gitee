#include <vector>
#include <algorithm>

#include "skyeye_class.h"
#include "skyeye_attr.h"
#include "skyeye_log.h"

typedef std::vector < const char *>StrList;
struct log_group
{
    StrList list;
};

static attr_value_t get_log_group(void *_, conf_class_t * cls, attr_value_t * idx)
{
    log_group_t *group = cls->log_group;

    if (!group)
        return SKY_make_attr_nil();

    /* n = {
     *   [0] for global
     *   ...
     * }
     */
    int n = SKY_attr_integer(*idx) - 1;

    if (n >= group->list.size())
    {
        return SKY_make_attr_nil();
    }
    return SKY_make_attr_string_adopt(group->list[n]);
}

void init_log_groups(conf_class_t * cls)
{
    cls->log_group = new log_group_t;
    /* TODO: register log_groups attribute */
    /*
     * SKY_register_typed_class_attribute(cls, "log_groups",
     * get_log_groups, NULL, NULL, NULL, (attr_attr_t) 0, "s|n", "i", NULL);
     */
}

void SKY_log_register_groups(conf_class_t * cls, const char *const *names)
{
    if (!cls)
    {
        SKY_log_error(NULL, 0, "%s: conf_class_t* is null", __func__);
        return;
    }
    cls->log_group->list.clear();
    int i;

    for (i = 0; names[i]; i++)
    {
        cls->log_group->list.push_back(names[i]);
    }
}

const char *SKY_log_get_group(conf_class_t * cls, int idx)
{
    if (idx <= 0)
        return NULL;
    /* n = {
     *   [0] for global
     *   ...
     * }
     */
    idx--;
    if (idx >= cls->log_group->list.size())
    {
        return NULL;
    }
    return cls->log_group->list[idx];
}
