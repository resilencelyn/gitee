
#ifndef _PAGE1_
#define _PAGE1_

#ifdef __cplusplus
extern "C" {
#endif
/*********************
* INCLUDES
*********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "../../lvgl/lvgl.h"
#include "../../lv_ex_conf.h"
#endif
#ifdef __cplusplus
} /* extern "C" */
#endif

void page1_start(void);
void page1_end(void);
void page1_time();
void update_time();
void page_del_time();
typedef struct _page1_time_t
{
    lv_obj_t *dian;
    lv_obj_t *shi;
    lv_obj_t *fen;
    lv_obj_t *date;
    lv_anim_t *anim;
}lv_page_time_t;
#endif // _TEST_
