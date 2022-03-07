#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "../blbl_sys.h"
#include "page.h"
#include "../font/myFont.h"
#include "../photo/weather.h"
#include "../page1/page1.h"
LV_IMG_DECLARE(blbl22);
LV_IMG_DECLARE(blbl33);
LV_FONT_DECLARE(chinese);
LV_FONT_DECLARE(time_font);
extern lv_page_time_t page1_time_t;
/*****主体****/
struct _page2_cont_t
{
    lv_obj_t *weather;
    lv_obj_t *sht30;
    lv_obj_t *cat;
}page2_cont_t;
/*****天气****/
struct _page1_weather_t
{
    lv_obj_t * label;
    lv_obj_t *imgtq;
}page1_weather_t;
struct _page2_sht30_t
{
    lv_obj_t *img_temp;
    lv_obj_t *img_humi;
    lv_obj_t *bar_temp;
    lv_obj_t *bar_humi;
    lv_obj_t *label_temp;
    lv_obj_t *label_humi;
}page2_sht30_t;
extern struct _blbl_sys_t blbltv_t;

lv_obj_t * label_temp_number;
static void update_temp()
{
    char str[40];
    sprintf(str,"%d℃",blbltv_t.temp_hmui_t->temp);
    lv_label_set_text(page2_sht30_t.label_temp, str);
    sprintf(str,"%d%%",blbltv_t.temp_hmui_t->humi);
    lv_label_set_text(page2_sht30_t.label_humi, str);
    lv_bar_set_value(page2_sht30_t.bar_temp, blbltv_t.temp_hmui_t->temp, LV_ANIM_ON);
    lv_bar_set_value(page2_sht30_t.bar_humi, blbltv_t.temp_hmui_t->humi, LV_ANIM_ON);
}
lv_obj_t *cont_temp;
extern const struct LV_WEATHER lv_weather[40];
lv_obj_t *imgtq;
static void update_weather()
{
    //天气图片
    char str[40];
    uint8_t i=blbltv_t.weather_t[0]->code_day;
    lv_img_dsc_t *p=(lv_img_dsc_t *)lv_weather[i].img_dsc; 
    lv_img_set_src(page1_weather_t.imgtq,p);
    sprintf(str,"%s %s",get_city(),lv_weather[i].weathername);
    lv_label_set_text(page1_weather_t.label,str);

}
static lv_style_t style_temp;
    lv_anim_t atemp;
static void page2_temp()
{
    //主cont体
    page2_cont_t.weather = lv_obj_create(blbltv_t.scr.index);
    lv_obj_set_size(page2_cont_t.weather,80,100);
    lv_obj_set_pos(page2_cont_t.weather,120,40);
    // 风格
    lv_obj_clear_flag(page2_cont_t.weather, LV_OBJ_FLAG_SCROLLABLE);//不可滚动
    lv_style_set_border_width(&style_temp, 0);//无边框
    lv_style_set_bg_opa(&style_temp, LV_OPA_TRANSP);//透明
    lv_obj_add_style(page2_cont_t.weather,&style_temp , LV_PART_MAIN);
    //天气部分
    page1_weather_t.imgtq = lv_img_create(page2_cont_t.weather);
    lv_obj_align(page1_weather_t.imgtq, LV_ALIGN_CENTER, 0, -20);

    page1_weather_t.label=lv_label_create(page2_cont_t.weather);
    lv_obj_set_style_text_font(page1_weather_t.label,&chinese,0);
    lv_label_set_text(page1_weather_t.label,"多云");
    lv_obj_align(page1_weather_t.label, LV_ALIGN_CENTER, 0, 20);


    //室内温度
    update_weather();

    lv_anim_init(&atemp);
    lv_anim_set_var(&atemp, page2_cont_t.weather);
    lv_anim_set_values(&atemp, SCR_W+blbl22.header.w, 180);
    lv_anim_set_time(&atemp, 1000);

    lv_anim_set_path_cb(&atemp, lv_anim_path_ease_in_out);

    lv_anim_set_exec_cb(&atemp, anim_x_cb);
    lv_anim_start(&atemp);

}
LV_IMG_DECLARE(b22c);
LV_IMG_DECLARE(fly);
static void page2_gif_move()
{
 
    page2_cont_t.cat = lv_gif_create(blbltv_t.scr.index);
    lv_gif_set_src(page2_cont_t.cat, &fly);
    lv_obj_align(page2_cont_t.cat, LV_ALIGN_LEFT_MID, 180, 60);

}
LV_IMG_DECLARE(temp);
LV_IMG_DECLARE(humi);
static lv_style_t style;
static void page_temp()
{
        // 风格

    lv_style_set_border_width(&style, 0);//无边框
    lv_style_set_bg_opa(&style, LV_OPA_TRANSP);//透明

    page2_cont_t.sht30= lv_obj_create(blbltv_t.scr.index);
    lv_obj_clear_flag(page2_cont_t.sht30, LV_OBJ_FLAG_SCROLLABLE);//不可滚动
    lv_obj_set_size(page2_cont_t.sht30,170,100);
    lv_obj_add_style(page2_cont_t.sht30,&style , LV_PART_MAIN);
    lv_obj_set_pos(page2_cont_t.sht30,-3,140);

    page2_sht30_t.img_temp = lv_img_create(page2_cont_t.sht30);
    lv_img_set_src(page2_sht30_t.img_temp,&temp);
    lv_obj_set_pos(page2_sht30_t.img_temp,-5,0);

    page2_sht30_t.img_humi = lv_img_create(page2_cont_t.sht30);
    lv_img_set_src(page2_sht30_t.img_humi,&humi);
    lv_obj_set_pos(page2_sht30_t.img_humi,-5,40);

    static lv_style_t style_indic;

    lv_style_init(&style_indic);
    lv_style_set_bg_opa(&style_indic, LV_OPA_COVER);
    lv_style_set_bg_color(&style_indic, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_bg_grad_color(&style_indic, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_bg_grad_dir(&style_indic, LV_GRAD_DIR_HOR);

    page2_sht30_t.bar_temp = lv_bar_create(page2_cont_t.sht30);
    lv_obj_add_style(page2_sht30_t.bar_temp, &style_indic, LV_PART_INDICATOR);
    lv_obj_set_size(page2_sht30_t.bar_temp, 80, 15);
    lv_bar_set_range(page2_sht30_t.bar_temp, -20, 40);
    lv_obj_set_pos(page2_sht30_t.bar_temp,30,10);

    page2_sht30_t.label_temp=lv_label_create(page2_cont_t.sht30);
    lv_label_set_text(page2_sht30_t.label_temp, "0℃");
    lv_obj_set_style_text_font(page2_sht30_t.label_temp,&chinese,0);
    lv_obj_set_pos(page2_sht30_t.label_temp,115,5);

    page2_sht30_t.bar_humi = lv_bar_create(page2_cont_t.sht30);
    lv_obj_add_style(page2_sht30_t.bar_humi, &style_indic, LV_PART_INDICATOR);
    lv_obj_set_size(page2_sht30_t.bar_humi, 80, 15);
    lv_obj_set_pos(page2_sht30_t.bar_humi,30,50);
    lv_bar_set_range(page2_sht30_t.bar_humi, 0, 100);

    page2_sht30_t.label_humi=lv_label_create(page2_cont_t.sht30);
    lv_label_set_text(page2_sht30_t.label_humi, "0%");
    lv_obj_set_style_text_font(page2_sht30_t.label_humi,&chinese,0);
    lv_obj_set_pos(page2_sht30_t.label_humi,115,45);

}
void page2_load()
{
    page2_gif_move();
    page2_temp();
    page1_time();
    page_temp();
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
            if(h==h*60*8)//8小时
            {
                update_weather();//更新天气
                h=0;
            }
        }
        i++;
}
static lv_timer_t  * task_time;
void page2_start()
{
    page2_load();
    task_time = lv_timer_create(page_task_time, 500,  NULL); //创建任务
}
void page2_end()
{
    lv_timer_del(task_time);
    lv_anim_del_all();
    lv_obj_del(page2_cont_t.weather);
    lv_obj_del(page2_cont_t.sht30);
    lv_obj_del(page2_cont_t.cat);
    page_del_time();
}
