#ifndef __SKYEYE_LIST__
#define __SKYEYE_LIST__

#ifdef __cplusplus
extern "C" {
#endif

SKY_list *SKY_create_list();
int SKY_list_push_back(SKY_list* list_p, void*node);
int SKY_list_clear(SKY_list* list_p);
int SKY_list_push_front(SKY_list* list_p, void*node);
void *SKY_list_search(SKY_list *list_p, int (*func)(void*));
void *SKY_list_traveral(SKY_list *list_p, int (*func)(void*));
void* SKY_list_get_node(SKY_list *list_p, unsigned int num);
int SKY_list_count(SKY_list *list_p);
#ifdef __cplusplus
}
#endif

#endif


