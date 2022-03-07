#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "../blbl_sys.h"
#include "../photo/weather.h"
#include "page1.h"
LV_IMG_DECLARE(blbl22);
LV_IMG_DECLARE(blbl33);
LV_IMG_DECLARE(b22c);
LV_FONT_DECLARE(chinese);
LV_FONT_DECLARE(time_font);
/*****主体****/
typedef struct _page1_cont_t
{
    lv_obj_t *time;
    lv_obj_t *temp;
    lv_obj_t *fensi;
    lv_anim_t *anim;
}lv_page_cont;
lv_page_cont page1_cont_t;
/*****时间****/

lv_page_time_t page1_time_t;
/*****天气****/
typedef struct _page1_weather_t
{
    lv_obj_t * label;
    lv_obj_t *imgtq;
    lv_anim_t *anim;
}lv_page_weather_t;
lv_page_weather_t page1_weather_t;
/*****动画****/
struct _page1_anim_t
{
    lv_anim_t time;
    lv_anim_t weather;
    lv_anim_t fensi;
    lv_anim_t b22;
    lv_anim_t b33;
}page1_anim_t;
extern struct _blbl_sys_t blbltv_t;
lv_obj_t * label_fensi;
extern const struct LV_WEATHER lv_weather[40];
static lv_timer_t  * task_time;
const char *date[]={"日","一","二","三","四","五","六"};
void update_time()
{
    char now_time[40];
    sprintf((char *)now_time,"#FFA500 %02d#",blbltv_t.timeinfo_t.tm_hour);
    lv_label_set_text(page1_time_t.shi,(const char *)now_time );
    sprintf((char *)now_time,"#FFA500 %02d#",blbltv_t.timeinfo_t.tm_min);
    lv_label_set_text(page1_time_t.fen, (const char *)now_time);
    uint8_t tm_wday=blbltv_t.timeinfo_t.tm_wday;
    //星期与日历
    sprintf((char *)now_time,"#FFC0CB 星期%s%02d/%02d#",date[tm_wday],blbltv_t.timeinfo_t.tm_mon+1,blbltv_t.timeinfo_t.tm_mday);
    lv_label_set_text(page1_time_t.date, (const char *)now_time);
}
static void update_fensi()
{
    char info[40];
    sprintf((char *)info,"粉丝: %d\n关注: %d",blbltv_t.blbl_info_t->follower,blbltv_t.blbl_info_t->following);
    lv_label_set_text(label_fensi, (const char *)info);
}
static void update_temp()
{
    //更新温湿度
    char now_time[40];
    sprintf(now_time,"%d ℃\n%d %%",blbltv_t.temp_hmui_t->temp,blbltv_t.temp_hmui_t->humi);
    lv_label_set_text(page1_weather_t.label,(const char *) now_time);
}
static void update_weather()
{
    //天气图片
    uint8_t i=blbltv_t.weather_t[0]->code_day;
    lv_img_dsc_t *p=(lv_img_dsc_t *)lv_weather[i].img_dsc; 
    lv_img_set_src(page1_weather_t.imgtq,p);

}
void page1_time()
{
    // 风格
    static lv_style_t style;
    lv_style_set_border_width(&style, 0);//无边框
    lv_style_set_bg_opa(&style, LV_OPA_TRANSP);//透明

    page1_cont_t.time= lv_obj_create(blbltv_t.scr.index);
    lv_obj_clear_flag(page1_cont_t.time, LV_OBJ_FLAG_SCROLLABLE);//不可滚动

    lv_obj_add_style(page1_cont_t.time,&style , LV_PART_MAIN);

    lv_obj_set_size(page1_cont_t.time,180,120);
    lv_obj_set_pos(page1_cont_t.time,-10,30);
    //小时
    page1_time_t.shi=lv_label_create(page1_cont_t.time);
    lv_label_set_recolor(page1_time_t.shi, true);
    lv_obj_set_pos(page1_time_t.shi,0,0);
    lv_obj_set_style_text_font(page1_time_t.shi,&time_font,0);
    //点
    page1_time_t.dian=lv_label_create(page1_cont_t.time);
    lv_label_set_recolor(page1_time_t.dian, true);
    lv_obj_set_pos(page1_time_t.dian,67,-5);
    lv_obj_set_style_text_font(page1_time_t.dian,&time_font,0);
    lv_label_set_text(page1_time_t.dian, "#FFA500 :#");
    //分钟
    page1_time_t.fen=lv_label_create(page1_cont_t.time);
    lv_label_set_recolor(page1_time_t.fen, true);
    lv_obj_set_pos(page1_time_t.fen,80,0);
    lv_obj_set_style_text_font(page1_time_t.fen,&time_font,0);

    //星期及日期
    page1_time_t.date=lv_label_create(page1_cont_t.time);
    lv_label_set_recolor(page1_time_t.date, true);
    lv_obj_set_pos(page1_time_t.date,15,58);
    lv_obj_set_style_text_font(page1_time_t.date,&chinese,0);
    update_time();//更新数据

    // lv_anim_init(&page1_anim_t.time);
    // lv_anim_set_var(&page1_anim_t.time, page1_cont_t.time);
    // lv_anim_set_values(&page1_anim_t.time, -70, -10);
    // lv_anim_set_time(&page1_anim_t.time, 1000);
    // lv_anim_set_path_cb(&page1_anim_t.time, lv_anim_path_ease_in_out);
    // lv_anim_set_exec_cb(&page1_anim_t.time, anim_x_cb);
    // lv_anim_start(&page1_anim_t.time);
}
static void page1_fensi()
{
    page1_cont_t.fensi =lv_obj_create(blbltv_t.scr.index);
    lv_obj_clear_flag(page1_cont_t.fensi, LV_OBJ_FLAG_SCROLLABLE);//不可滚动
    // 风格
    static lv_style_t style;
    lv_style_set_border_width(&style, 0);//无边框
    lv_style_set_bg_opa(&style, LV_OPA_TRANSP);//透明
    lv_obj_add_style(page1_cont_t.fensi,&style , LV_PART_MAIN);

    lv_obj_set_size(page1_cont_t.fensi,100,60);
    lv_obj_set_pos(page1_cont_t.fensi,92,140);
    //blbl
    label_fensi=lv_label_create(page1_cont_t.fensi);
    lv_obj_set_style_text_font(label_fensi,&chinese,0);
    lv_obj_align(label_fensi,LV_ALIGN_CENTER,0,0);
    //风格
    update_fensi();
    lv_anim_init(&page1_anim_t.fensi);
    lv_anim_set_var(&page1_anim_t.fensi, page1_cont_t.fensi);
    lv_anim_set_values(&page1_anim_t.fensi, SCR_H, 145);
    lv_anim_set_time(&page1_anim_t.fensi, 1000);
    lv_anim_set_path_cb(&page1_anim_t.fensi, lv_anim_path_ease_in_out);
    lv_anim_set_exec_cb(&page1_anim_t.fensi, anim_y_cb);
    lv_anim_start(&page1_anim_t.fensi);
}

static void page1_temp()
{
    static lv_style_t style_temp;
    //主cont体
    page1_cont_t.temp = lv_obj_create(blbltv_t.scr.index);
    //lv_obj_set_size(page1_cont_t.temp,120,150);
    lv_obj_set_pos(page1_cont_t.temp,120,40);
    // 风格
    lv_obj_clear_flag(page1_cont_t.temp, LV_OBJ_FLAG_SCROLLABLE);//不可滚动
    lv_style_set_border_width(&style_temp, 0);//无边框
    lv_style_set_bg_opa(&style_temp, LV_OPA_TRANSP);//透明
    lv_obj_add_style(page1_cont_t.temp,&style_temp , LV_PART_MAIN);
    //天气部分
    page1_weather_t.imgtq = lv_img_create(page1_cont_t.temp);
    lv_obj_set_pos(page1_weather_t.imgtq,0,0);
   //百分比
    page1_weather_t.label=lv_label_create(page1_cont_t.temp);
    lv_obj_set_pos(page1_weather_t.label,0,0);
    lv_obj_set_style_text_font(page1_weather_t.label,&chinese,0);
    lv_obj_set_pos(page1_weather_t.label,50,0);
    //室内温度
    update_temp();
    update_weather();

    lv_anim_init(&page1_anim_t.weather);
    lv_anim_set_var(&page1_anim_t.weather, page1_cont_t.temp);
    lv_anim_set_values(&page1_anim_t.weather, SCR_W+blbl22.header.w, 150);
    lv_anim_set_time(&page1_anim_t.weather, 1000);

    lv_anim_set_path_cb(&page1_anim_t.weather, lv_anim_path_ease_in_out);

    lv_anim_set_exec_cb(&page1_anim_t.weather, anim_x_cb);
    lv_anim_start(&page1_anim_t.weather);

}
lv_obj_t * img22;
lv_obj_t * img33;
static void page1_blbl_move()
{

    img22 = lv_img_create(blbltv_t.scr.index);
    lv_img_set_src(img22,&blbl22);
    lv_obj_set_pos(img22,-blbl22.header.w,150);

    img33 = lv_img_create(blbltv_t.scr.index);
    lv_img_set_src(img33,&blbl33);
    lv_obj_set_pos(img33,SCR_H,150);

    lv_anim_init(&page1_anim_t.b22);
    lv_anim_set_var(&page1_anim_t.b22, img22);
    lv_anim_set_values(&page1_anim_t.b22, -blbl22.header.w, 10);
    lv_anim_set_time(&page1_anim_t.b22, 1000);
//    lv_anim_set_playback_delay(&page1_anim_t.b22, 1000);
//    lv_anim_set_playback_time(&page1_anim_t.b22, 300);

    lv_anim_set_path_cb(&page1_anim_t.b22, lv_anim_path_ease_in_out);

    lv_anim_set_exec_cb(&page1_anim_t.b22, anim_x_cb);
    lv_anim_start(&page1_anim_t.b22);

    lv_anim_init(&page1_anim_t.b33);
    lv_anim_set_var(&page1_anim_t.b33, img33);
    lv_anim_set_values(&page1_anim_t.b33, SCR_W+blbl22.header.w, SCR_W-blbl22.header.w-10);
    lv_anim_set_time(&page1_anim_t.b33, 1000);
//    lv_anim_set_playback_delay(&page1_anim_t.b33, 1000);
//    lv_anim_set_playback_time(&page1_anim_t.b33, 300);

    lv_anim_set_path_cb(&page1_anim_t.b33, lv_anim_path_ease_in_out);

    lv_anim_set_exec_cb(&page1_anim_t.b33, anim_x_cb);
    lv_anim_start(&page1_anim_t.b33);
//    logo_font();
//    ANIEND
}

static void page1_load()
{
    page1_blbl_move();
    page1_temp();
    page1_fensi();
    page1_time();
}
static void page_task_time()
{
    static uint8_t i=0,h=0;
        if(i%2==0)
            lv_obj_add_flag(page1_time_t.dian, LV_OBJ_FLAG_HIDDEN);
        else
            lv_obj_clear_flag(page1_time_t.dian, LV_OBJ_FLAG_HIDDEN);
        if(i%20==0)//10s
        {
         update_temp(); //更新温湿度         
         update_time();  //更新时间
        }
        if(i%120==0)//一分钟
        {
            i=0;
            h++;
            if(h%3==0)//3分钟，更新粉丝数
            update_fensi();
            if(h==h*60*8)//8小时
            {
                update_weather();//更新天气
                h=0;
            }
        }
        i++;
}
void page1_start()
{
    page1_load();
    task_time = lv_timer_create(page_task_time, 500,  NULL); //创建任务
}
void page_del_time()
{
    lv_obj_del(page1_cont_t.time);
}
void page1_end()
{
    lv_timer_del(task_time);
    lv_anim_del_all();
    lv_obj_del(page1_cont_t.fensi);
    lv_obj_del(page1_cont_t.temp);
    lv_obj_del(img22);
    lv_obj_del(img33);
    lv_obj_del(page1_cont_t.time);
}