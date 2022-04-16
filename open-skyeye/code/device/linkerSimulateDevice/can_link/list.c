/*
 * =====================================================================================
 *
 *       Filename:  list1.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年11月17日 15时07分11秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include"list.h"
#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>


void list_init(LIST_t *ls)
{
    LIST_t n;
    n = malloc(sizeof(struct node));
    if(!n)
    {
        printf("malloc fail\n");
        exit(0);
    }
    n->next = NULL;
    (*ls) = n;
    return;
}
struct node *make_node(void *date)
{
    struct node *n;
    n = malloc(sizeof(struct node));
    n->date = date;
    n->next = NULL;
    return n;
}

void list_insert(LIST_t *ls, void *date)
{
    struct node *n = *(ls), *t;
    t = make_node(date);
    while(n->next)
       n = n->next;
    n->next = t;
}

void list_insert_head(LIST_t *ls, void *date)
{
    struct node *n = (*ls), *t, *tmp;
    t = make_node(date);
    if(n->next == NULL)
        n->next = t;
    else
    {
       tmp = n->next;
       n->next = t;
       t->next = tmp;
    }
}
void *list_delete(LIST_t *ls, void *p, int (*compare)(void *, void *))
{
        struct node  *n = *(ls), *t;
        void *date = NULL;
        while(n->next) 
        {
            if(compare(n->next->date, p) == 0)
            {
                date = n->next->date;
                t = n->next;
                n->next = t->next;
                free(t);
                break;
            }
            n = n->next;
        }
        return date;
}


void *list_search(LIST_t *ls, void *p, int (*compare)(void *, void *))
{
        struct node *n = (*ls)->next; 
        while(n)
        {
            if(compare(n->date, p) == 0)
                return n->date;
            n = n->next;
        }
        return NULL;
}

void *list_search_all(LIST_t *ls, LIST_t *p)
{
   struct node *tmp;
   if((*p) == (*ls))
       tmp = (*p)->next;
   else
       tmp = (*p);
   (*p) = tmp->next;

   return  tmp->date;
}
void *list_retriver(LIST_t *ls, int idx)
{
    struct node *n = (*ls)->next;
    int i = 0;
    while(n && i < idx)
    {
        n = n->next;
        i++;
    }
    
    if(n)
        return n->date;
    else
        {
            return NULL;
        }
}

void list_sort_select(LIST_t *ls, int (*compare)(void *, void *))
{
    struct node *first, *p, *q, *t;
    first = (*ls)->next->next;
    (*ls)->next->next = NULL;
    
    while(first)
    {
        for(p = *ls; p->next != NULL; p = p->next)
            if(compare(first->date, p->next->date) < 0)
                break;
        t = first->next;
        if(p->next == NULL)
        {
            p->next = first;
            p->next->next = NULL;
        }
        else
          {
              q = p->next;
              p->next = first;
              p->next->next = q;
          }
        first = t;
    }
}

void list_destory(LIST_t *ls , void (*destory)(void *date))
{
    struct node *n = (*ls)->next, *t;
    free(*ls);
    while(n)
    {
        t = n->next; 
        if(destory)
           destory(n->date);
        free(n);
        n = t;
    }
    (*ls) = NULL;
}


