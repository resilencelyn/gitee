#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "page_set.h"
#include "../blbl_sys.h"
#include "../lv_lib_qrcode/lv_qrcode.h"
#include "esp_log.h"
#include "lv_port_indev.h"
#include "wificonnet.h"
LV_FONT_DECLARE(myFont);
LV_FONT_DECLARE(chinese);
lv_obj_t *tabview;
lv_obj_t *tab1,*tab2,*tab3;

uint8_t clock_change=0;//时间是否发生改变
extern struct _blbl_sys_t blbltv_t;
struct {
    lv_obj_t* labe_mqtt;
    lv_obj_t* labe_wifi;
    lv_obj_t* labe;
    lv_obj_t* sw_mqtt;
    lv_obj_t * btn;
    lv_obj_t* page_info;
    lv_obj_t* labe_info;
    lv_obj_t* list;
}tab1_t;
struct {
    lv_obj_t* label_time;
    lv_obj_t* sw_time;
    lv_obj_t* cont;
    lv_obj_t* table;
}tab2_t;
struct {
    lv_obj_t* label_temp;
    lv_obj_t* sw_temp;
    lv_obj_t* page;
    lv_obj_t* set_info;
}tab3_t;
static void creat_mqtt()
{

    lv_obj_clean(tab1_t.page_info);
    tab1_t.labe_mqtt=lv_label_create(tab1_t.page_info);
    lv_label_set_text(tab1_t.labe_mqtt,"mqtt");
    lv_obj_set_pos(tab1_t.labe_mqtt,10,8);
    tab1_t.btn = lv_switch_create(tab1_t.page_info);
    lv_obj_add_state(tab1_t.btn, LV_STATE_CHECKED);
    lv_obj_set_size(tab1_t.btn,40,20);
    lv_obj_set_pos(tab1_t.btn,80,10);
    //lv_obj_add_event_cb(tab1_t.btn, event_handler, LV_EVENT_ALL, NULL);
}
lv_obj_t * qr;
static void creat_web()
{
    lv_obj_clean(tab1_t.page_info);
    //tab1_t.labe_wifi=lv_obj_create(tab1_t.page_info);
    //二维码显示
    char label_str[100];
    lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_YELLOW, 4);

    lv_obj_t * label_qr = lv_label_create(tab1_t.page_info);
    qr = lv_qrcode_create(label_qr, 80, fg_color, bg_color);
    sprintf(label_str,"http://%s",get_ip());
    lv_qrcode_update(qr, label_str, strlen(label_str));
    lv_obj_align(label_qr, LV_ALIGN_CENTER, 0, -40);
    //文字
    tab1_t.labe_wifi=lv_label_create(tab1_t.page_info);
    lv_label_set_long_mode(tab1_t.labe_wifi, LV_LABEL_LONG_WRAP);
    //lv_obj_set_style_text_font(tab1_t.labe_wifi,&chinese,LV_PART_MAIN);

    lv_obj_set_width(tab1_t.labe_wifi, 140);
    lv_obj_align(tab1_t.labe_wifi, LV_ALIGN_CENTER, 0, 25);
    lv_label_set_recolor(tab1_t.labe_wifi, true);                      /*Enable re-coloring by commands in the text*/
    
    sprintf(label_str,"%s\nhttp://+ip",get_ip());
    lv_label_set_text(tab1_t.labe_wifi,label_str);
    //文字
    tab1_t.labe=lv_label_create(tab1_t.page_info);
    lv_label_set_long_mode(tab1_t.labe, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(tab1_t.labe,&chinese,LV_PART_MAIN);

    lv_obj_set_width(tab1_t.labe, 140);
    lv_obj_align(tab1_t.labe, LV_ALIGN_CENTER, 0, 70);
    lv_label_set_recolor(tab1_t.labe, true);                      /*Enable re-coloring by commands in the text*/
    lv_label_set_text(tab1_t.labe, "电脑访问地址↑\n");
}
static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    //printf("Clicked: %s", lv_list_get_btn_text(tab1_t.list, obj));
    if(code == LV_EVENT_CLICKED) {
        if(!strcmp(lv_list_get_btn_text(tab1_t.list, obj),"mqtt")){
            creat_mqtt();
        }else if(!strcmp(lv_list_get_btn_text(tab1_t.list, obj),"web")){
            creat_web();
        }
    }
}
lv_obj_t  *sw2;
void tab_wifi_init(lv_obj_t * obj)
{
    /*************列表*************/
    //  风格
    static lv_style_t style;
    //lv_style_set_pad_column(&style,0);//间距
    lv_style_set_pad_left(&style,0);
    lv_style_set_pad_right(&style,0);
    lv_style_set_pad_top(&style,0);
    //容器
    tab1_t.list = lv_list_create(obj);
    lv_obj_set_size(tab1_t.list,100,180);
    lv_obj_center(tab1_t.list);
    lv_obj_align(tab1_t.list, LV_ALIGN_CENTER, -87, 0);
    lv_obj_add_style(tab1_t.list,&style , LV_PART_MAIN );
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);//不可滚动
    
    //内容
    //lv_obj_set_style_text_font(tab1_t.list,&chinese,LV_PART_MAIN);
    lv_list_add_text(tab1_t.list, LV_SYMBOL_WIFI"  wifi");
    tab1_t.btn = lv_list_add_btn(tab1_t.list, LV_SYMBOL_DIRECTORY, "mqtt");
    lv_obj_add_event_cb(tab1_t.btn, event_handler, LV_EVENT_CLICKED, NULL);
    lv_group_add_obj(get_key_g(), tab1_t.btn);
    tab1_t.btn = lv_list_add_btn(tab1_t.list, LV_SYMBOL_DIRECTORY, "web");
    lv_obj_add_event_cb(tab1_t.btn, event_handler, LV_EVENT_CLICKED, NULL);
    lv_group_add_obj(get_key_g(), tab1_t.btn);
    //详细信息
    tab1_t.page_info = lv_obj_create(obj);
    lv_obj_set_size(tab1_t.page_info,170,180);
    //lv_obj_clear_flag(tab1_t.page_info, LV_OBJ_FLAG_SCROLLABLE);//不可滚动
    lv_obj_align(tab1_t.page_info, LV_ALIGN_CENTER, 50, 0);



    tab1_t.labe_wifi=lv_label_create(tab1_t.page_info);
    lv_label_set_long_mode(tab1_t.labe_wifi, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(tab1_t.labe_wifi,&chinese,LV_PART_MAIN);
    lv_obj_set_width(tab1_t.labe_wifi, 145);
    lv_label_set_recolor(tab1_t.labe_wifi, true);                      /*Enable re-coloring by commands in the text*/
    char str[200];
    sprintf(str,"wifi名称:\n %s \n ip:\n %s",get_ssid(),get_ip());
    lv_label_set_text(tab1_t.labe_wifi,str);
}

/****
*
*
    温湿度上传间隔
    简介：使用方法 开源地址
*
***/

void tab_set_init(lv_obj_t * obj)
{
    /*********temp**********/
    tab3_t.label_temp = lv_label_create(obj);
    //lv_obj_set_style_local_text_font(tab3_t.label_temp,LV_LABEL_PART_MAIN,LV_STATE_DEFAULT,&myFont);
    lv_label_set_text(tab3_t.label_temp, "温湿度上报");
    lv_obj_set_pos(tab3_t.label_temp,30,5);

    tab3_t.sw_temp = lv_switch_create(obj);
    if(blbltv_t.state_t.upcloud)
   // lv_switch_on(tab3_t.sw_temp, LV_ANIM_ON);
//    lv_obj_set_event_cb(tab3_t.sw_temp, event_handler);
    lv_obj_set_size(tab3_t.sw_temp,30,16);
    lv_obj_set_pos(tab3_t.sw_temp,150,5);
    lv_group_add_obj(get_key_g(), tab3_t.sw_temp);
    /*******page*********/
//    tab3_t.page = lv_page_create(obj, NULL);
//    lv_obj_set_size(tab3_t.page, 200, 100);
//    lv_obj_set_pos(tab3_t.page,20,30);
//
//    lv_obj_t * label = lv_label_create(tab3_t.page);
////    lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);            /*Automatically break long lines*/
//    lv_obj_set_width(label, lv_page_get_width_fit(tab3_t.page));          /*Set the label width to max value to not show hor. scroll bars*/
////    lv_obj_set_style_local_text_font(label,LV_LABEL_PART_MAIN,LV_STATE_DEFAULT,&myFont);
//    lv_label_set_text(label, "这是一个开源项目\n"
//                             "硬件开源网址\n"
//                             "http://balabala.com\n"
//                             "软件开源网址\n"
//                             "巴拉巴拉\n");
}
void lv_tabview_init(void)
{
    /*Create a Tab view object*/
    tabview = lv_tabview_create(blbltv_t.scr.index,LV_DIR_TOP, 50);
 
    tab1 = lv_tabview_add_tab(tabview, LV_SYMBOL_WIFI);
    //tab2 = lv_tabview_add_tab(tabview, LV_SYMBOL_BELL);
    tab3 = lv_tabview_add_tab(tabview, LV_SYMBOL_SETTINGS);


    tab_wifi_init(tab1);
    tab_set_init(tab3);
    lv_group_add_obj(get_key_g(), lv_tabview_get_tab_btns(tabview));
}

void page_set_end()
{
    lv_group_remove_all_objs(get_key_g());
    if(qr!=NULL)
        lv_qrcode_delete(qr);
    lv_obj_del(tabview);
    ANIEND
}
void page_set_start()
{
    lv_tabview_init();
}
