#include <stdio.h>
#include "systemc_exec.h"
#include "skyeye_queue.h"
#include "skyeye_mm.h"

LIST_HEAD(systemc_thread_head, systemc_exec_s) exec_list;

    systemc_exec_t *create_systemc_exec()
{
    systemc_exec_t *exec = skyeye_mm(sizeof (systemc_exec_t));

    exec->run = exec->stop = NULL;
    exec->priv_data = NULL;
    exec->cycles = 0;
    return exec;
}

int add_to_exec_list(systemc_exec_t * exec)
{

    LIST_INSERT_HEAD(&exec_list, exec, list_entry);
    return 0;
}

int del_from_exec_list(systemc_exec_t * exec)
{

    systemc_exec_t *tmp;

    LIST_FOREACH(tmp, &exec_list, list_entry)
    {
        if (tmp == exec)
            LIST_REMOVE(tmp, list_entry);

    }
    return 0;
}

int exec_post_one_cycle()
{
    systemc_exec_t *tmp;

    LIST_FOREACH(tmp, &exec_list, list_entry)
    {
        tmp->run(tmp->priv_data);
        tmp->cycles++;
    }

}

int exec_post_cycles(unsigned count)
{
    systemc_exec_t *tmp;
    int i;

    for (i = count; i < count; i++)
    {
        LIST_FOREACH(tmp, &exec_list, list_entry)
        {
            tmp->run(tmp->priv_data);
            tmp->cycles++;
        }
    }

}
