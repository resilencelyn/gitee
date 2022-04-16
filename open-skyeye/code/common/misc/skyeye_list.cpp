#include <iostream>
#include<string>
#include<list>
#include<iostream>
#include <malloc.h>
#include <algorithm>
#include<string.h>
#include<skyeye_types.h>
#include<skyeye_list.h>

using namespace std;

typedef struct list_s
{
    list < void *>general_list;
} SKY_list;

SKY_list *SKY_create_list()
{
    SKY_list *list = new SKY_list;

    return list;
}

int SKY_list_clear(SKY_list * list_p)
{
    list_p->general_list.clear();
    return 0;
}

int SKY_list_push_back(SKY_list * list_p, void *node)
{
    list_p->general_list.push_back(node);
    return 0;
}

int SKY_list_push_front(SKY_list * list_p, void *node)
{
    list_p->general_list.push_front(node);
    return 0;
}

void *SKY_list_search(SKY_list * list_p, int (*func) (void *))
{
    list < void *>::iterator verator;

    for (verator = list_p->general_list.begin(); verator != list_p->general_list.end(); verator++)
    {
        if (!func(*verator))
        {
            return *verator;
        }
    }
    return NULL;
}

void *SKY_list_traveral(SKY_list * list_p, int (*func) (void *))
{
    for_each(list_p->general_list.begin(), list_p->general_list.end(), func);
}

int SKY_list_count(SKY_list * list_p)
{
    return list_p->general_list.size();
}

void *SKY_list_get_node(SKY_list * list_p, unsigned int num)
{
    list < void *>::iterator verator;
    unsigned int temp_n = 0;

    for (verator = list_p->general_list.begin(); verator != list_p->general_list.end(); verator++)
    {
        if (temp_n == num)
        {
            return *verator;
        }
        temp_n++;
    }
    return NULL;
}
