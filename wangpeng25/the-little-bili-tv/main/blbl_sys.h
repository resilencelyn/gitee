#ifndef _BLBL_SYS_T_
#define _BLBL_SYS_T_
#include "gui_time.h"
#include <stdlib.h>
#include <string.h>
#include "lvgl.h"
#include "http.h"
#include "sht30.h"

#define SCR_W 280
#define SCR_H 240
#define ANIEND  while(lv_anim_count_running()) {lv_task_handler();};//等待动画完成
typedef struct _page_t
{
    struct _page_t * last;
    unsigned char index;
    void (*start)();//功能
    void (*end)();//功能
    struct _page_t * next;
}page_t;
typedef struct _ico_t_
{
    lv_obj_t *obj;//wifi图标
    char * str;
}ico_t;
typedef struct
{
    ico_t wifi;//wifi图标
    ico_t mqtt; //mqtt图标
    ico_t web; //we图标
    ico_t temp; //温度图标
}blbl_ico_t;
struct _blbl_state
{
    /* data */
    uint8_t mqtt:2;//mqtt连接状态
    uint8_t wifi:4;//wifi连接情况
    uint8_t sys_button:6;//切换button使用权
    uint8_t clock:1;
    uint8_t web:1;
    uint8_t daiding:1;
    uint8_t upcloud:1;
};
struct _blbl_scr
{
    lv_obj_t * index ;//主体
    lv_obj_t * head;//通知栏
    lv_obj_t * body;//身体
    lv_obj_t * head_label;
};
struct _blbl_scr_style
{
    lv_style_t * index ;//主体
    lv_style_t * head;//通知栏
    lv_style_t * body;//身体
};
typedef struct _blbl_sys_t{

    page_t *               page;//当前页
    blbl_ico_t             head_t;//状态栏图标
    struct _blbl_state     state_t;//系统状态信息
    lv_obj_t *             lv_bsgbox; //信息通知盒
    uint32_t               user_data;//切换界面
    uint32_t               key;//切换界面
    uint32_t               key_use;
    lv_group_t *           group;//按键组
    struct tm              timeinfo_t;
    blbl_follow_t *        blbl_info_t;
    sht30_t*               temp_hmui_t;
    struct _blbl_scr       scr;
    struct _blbl_scr_style scr_style;
    sys_weather_t          (*weather_t)[3];
}blbl_sys_t;
page_t * add_page(page_t * page);
void sys_index_init();
void anim_x_cb(void * var, int32_t v);
void anim_y_cb(void * var, int32_t v);
void anim_width_cb(void * var, int32_t v);
#endif // _BLBL_SYS_
