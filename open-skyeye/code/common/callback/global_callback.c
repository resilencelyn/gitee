#include <stdio.h>
#include "skyeye_obj.h"
#include "skyeye_device.h"
#include "skyeye_class.h"
#include "skyeye_mm.h"
#include "skyeye_types.h"
#include "skyeye_list.h"
#include "skyeye_queue.h"
#include "skyeye_callback.h"

char *hap_name[] = {
    "SIM_LOG_MESSAGE",
    "SIM_PC",
    "SIM_BEFORE_INIT",
    NULL,
};

typedef enum
{
    Sim_Log_Message = 0,
    Sim_Pc,
    SIM_BEFORE_INIT,
    Max_hap
} hap_type_t;
static hap_handle_t handle_alloc_count = 0;

struct hap_node
{
    obj_hap_func_t hap_func;
    void *user_data;
    hap_handle_t handle;
    conf_object_t *obj;
      LIST_ENTRY(hap_node) list_entry;
};

LIST_HEAD(list_thread_head, hap_node) hap_head[Max_hap];

    static hap_type_t hap_name_to_hap_type(const char *hap)
{
    int i;

    for (i = 0; hap_name != NULL; i++)
    {
        if (!strcmp(hap_name[i], hap))
            return (hap_type_t) i;
    }
    return Max_hap;
}

void SKY_hap_init(void)
{
    int i;

    for (i = 0; i < Max_hap; i++)
    {
        LIST_INIT(&(hap_head[i]));
    }
    handle_alloc_count = 0;
    return;
}

static int free_hap_node(void *node)
{
    struct hap_node *hap_node = node;

    skyeye_free(hap_node);
    return 0;
}
void SKY_hap_free(void)
{
    int i;
    struct hap_node *tmp;

    for (i = 0; i < Max_hap; i++)
    {
        LIST_FOREACH(tmp, &(hap_head[1]), list_entry)
        {
            LIST_REMOVE(tmp, list_entry);
            skyeye_free(tmp);
        }
    }
    handle_alloc_count = 0;
    return;
}

hap_handle_t SKY_hap_add_callback(const char *hap, obj_hap_func_t func, void *user_data)
{
    hap_type_t type_index;

    type_index = hap_name_to_hap_type(hap);
    if (type_index == Max_hap)
    {
        return -1;
    }
    struct hap_node *hap_node = skyeye_mm(sizeof (struct hap_node));

    hap_node->hap_func = func;
    hap_node->user_data = user_data;
    hap_node->handle = handle_alloc_count++;
    LIST_INSERT_HEAD(&hap_head[(int) type_index], hap_node, list_entry);
    return hap_node->handle;
}

hap_handle_t SKY_hap_add_callback_obj(const char *hap, conf_object_t * obj, obj_hap_func_t func, void *user_data)
{
    hap_type_t type_index;

    type_index = hap_name_to_hap_type(hap);
    if (type_index == Max_hap)
    {
        return -1;
    }
    struct hap_node *hap_node = skyeye_mm(sizeof (struct hap_node));

    hap_node->hap_func = func;
    hap_node->user_data = user_data;
    hap_node->obj = obj;
    hap_node->handle = handle_alloc_count++;
    LIST_INSERT_HEAD(&hap_head[(int) type_index], hap_node, list_entry);
    return hap_node->handle;
}
void SKY_hap_delete_callback_id(const char *hap, hap_handle_t handle)
{

    struct hap_node *tmp;

    LIST_FOREACH(tmp, &(hap_head[Sim_Log_Message]), list_entry)
    {
        if (tmp->handle == handle)
        {
            LIST_REMOVE(tmp, list_entry);
            return;
        }
    }
    return;
}

void SKY_log_message_hap_occurred(void *callback_data, conf_object_t * trigger_obj, char *type, char *message)
{
    struct hap_node *tmp;

    LIST_FOREACH(tmp, &(hap_head[Sim_Log_Message]), list_entry)
    {
        (tmp->hap_func) (callback_data, trigger_obj, type, message);
    }
    return;
}

void SKY_pc_hap_occurred(void *callback_data, conf_object_t * trigger_obj, uint32_t pc)
{
    struct hap_node *tmp;

    LIST_FOREACH(tmp, &(hap_head[Sim_Pc]), list_entry)
    {
        if (tmp->obj == trigger_obj)
        {
            (tmp->hap_func) (callback_data, trigger_obj, tmp->user_data, pc);
        }
    }
    return;
}

void SKY_before_init_hap_occurred()
{
    struct hap_node *tmp;

    LIST_FOREACH(tmp, &(hap_head[SIM_BEFORE_INIT]), list_entry)
    {
        (tmp->hap_func) (tmp->user_data);
    }
    return;
}
