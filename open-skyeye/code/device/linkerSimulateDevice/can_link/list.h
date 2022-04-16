/*
 * =====================================================================================
 *
 *       Filename:  list1.h
 *
 *    Description:  
 *
 *        Version:  1.0

 q*        Created:  2012年11月17日 15时08分04秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *
 * =====================================================================================
 */
#ifndef _LIST_H
#define _LIST_H
struct node
{
    void *date;
    struct node *next;
};

typedef struct node  *LIST_t;


extern void list_init(LIST_t *ls);
extern void list_insert(LIST_t *ls, void *date);
extern void list_insert_head(LIST_t *ls, void *date);
extern void *list_delete(LIST_t *ls, void *p, int (*compare)(void *, void *));
extern void *list_search(LIST_t *ls, void *p, int (*compare)(void *, void *));
extern void *list_search_all(LIST_t *ls, LIST_t *p);
extern void *list_retriver(LIST_t *ls, int idx);
extern void  list_sort_select(LIST_t *ls, int (*compare)(void *, void *));
extern void  list_destory(LIST_t *ls, void (*destory)(void*));

#endif
