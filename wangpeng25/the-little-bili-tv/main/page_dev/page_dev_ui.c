#include "page_dev_ui.h"
#include "page_dev_fun.h"
#include "../blbl_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wificonnet.h"
#include "../lv_lib_qrcode/lv_qrcode.h"
#include "../part_head/part_head.h"
#include "../page_ctrl.h"
#define QR_W 100
extern struct _blbl_sys_t blbltv_t;
LV_FONT_DECLARE(chinese);
extern EventGroupHandle_t s_wifi_event_group;//wifi事件组
extern const int CONNECTED_BIT;
typedef struct _dev_rate_
{
    void(*fun)(void);
    char *       str;
}blbl_dev_t;
struct _lv_dev_t
{
    lv_obj_t *bar_rate;
    lv_obj_t *bar_label;
}lv_dev_t;
void smartconfig_set()
{
    //二维码显示
    lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_YELLOW, 4);

    lv_obj_t * label_qr = lv_label_create(blbltv_t.scr.index);
    lv_obj_t * qr = lv_qrcode_create(label_qr, QR_W, fg_color, bg_color);
    /*Set data*/
    const char * data = "http://iot.espressif.cn/configWXDeviceWiFi.html";
    lv_qrcode_update(qr, data, strlen(data));
    lv_obj_center(qr);
    lv_obj_set_y(label_qr,SCR_W/6);
    lv_obj_set_style_border_color(qr, bg_color, 0);
    lv_obj_set_style_border_width(qr, 5, 0);
    lv_anim_t move_qr;
    lv_anim_init(&move_qr);
    lv_anim_set_var(&move_qr, label_qr);
    lv_anim_set_values(&move_qr, 0, SCR_W/2-50);
    lv_anim_set_time(&move_qr, 1000);
    lv_anim_set_path_cb(&move_qr, lv_anim_path_ease_in_out);

    lv_anim_set_exec_cb(&move_qr, anim_x_cb);
    lv_anim_start(&move_qr);

    lv_obj_t * label_smartconfig = lv_label_create(blbltv_t.scr.index);
    lv_label_set_long_mode(label_smartconfig, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    lv_label_set_recolor(label_smartconfig, true);
    lv_obj_set_style_text_font(label_smartconfig,&chinese,0);                   /*Enable re-coloring by commands in the text*/
    lv_label_set_text(label_smartconfig,"#0000ff 微信扫码或关注乐鑫信息科技\n商铺->airkiss设备#");
    lv_obj_set_width(label_smartconfig, SCR_W);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label_smartconfig, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label_smartconfig, LV_ALIGN_CENTER, 0, SCR_H/4);

    lv_anim_t move_label;
    lv_anim_init(&move_label);
    lv_anim_set_var(&move_label, label_smartconfig);
    lv_anim_set_values(&move_label, SCR_W, 0);
    lv_anim_set_time(&move_label, 1000);
    lv_anim_set_path_cb(&move_label, lv_anim_path_ease_in_out);

    lv_anim_set_exec_cb(&move_label, anim_x_cb);
    lv_anim_start(&move_label);
    //开启配网
    smartconfig_example_task();
    ANIEND
    EventBits_t uxBits;
    while (1) 
    {
        uxBits = xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT, false, false, portMAX_DELAY); //等待配网事件组
        if(uxBits & CONNECTED_BIT) 
        {
            ESP_LOGI("WIFI", "WiFi Connected to ap");
            esp_smartconfig_stop();
            break;
        }
    }

    //配网完成
    lv_anim_set_values(&move_label,0, -SCR_W);
    lv_anim_start(&move_label);
    lv_anim_set_values(&move_qr,SCR_W/2-QR_W/2,SCR_W);
    lv_anim_start(&move_qr);
    //wifi图标
    lv_ico_show(&blbltv_t.head_t.wifi ,1);
    ANIEND
    lv_obj_clear_flag(lv_dev_t.bar_rate, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(lv_dev_t.bar_label, LV_OBJ_FLAG_HIDDEN);
    
}
void switch_wifi()
{
    lv_obj_add_flag(lv_dev_t.bar_rate, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(lv_dev_t.bar_label, LV_OBJ_FLAG_HIDDEN);
    smartconfig_set();
    blbltv_t.state_t.wifi=3;
}
void sys_dev_lv_init()
{
    lv_dev_t.bar_rate = lv_bar_create(blbltv_t.scr.index);
    lv_obj_set_size(lv_dev_t.bar_rate, 200, 20);
    lv_obj_align(lv_dev_t.bar_rate, LV_ALIGN_CENTER, 0, -20);

    lv_dev_t.bar_label=lv_label_create(blbltv_t.scr.index);
    lv_obj_set_style_text_align(lv_dev_t.bar_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(lv_dev_t.bar_label, LV_ALIGN_CENTER, 0, 10);
    lv_label_set_recolor(lv_dev_t.bar_label, true);
    lv_obj_set_style_text_font(lv_dev_t.bar_label,&chinese,0);
    lv_bar_set_value(lv_dev_t.bar_rate, 0, LV_ANIM_ON);
};
void sys_dev_fun()
{
    uint8_t value;
     blbl_dev_t dev[]={
         {start_rate,"blbl初始化"},
         {wifi_rate,"wifi初始化"},
         {time_rate,"获取时间"},
         {weather_rate,"获取天气"},
         {fans_rate,"获取粉丝数"},
         {mqtt_rate,"mqtt连接"},
         {web_rate,"web初始化"},
         {all_init,"初始化完毕"}
     };
    uint8_t m=sizeof(dev)/sizeof(dev[0]);
    for(uint8_t i=0;i<m;i++)
    {
         value=100/m*(i+1);
         if(i==(m-1)) value=100;
         lv_label_set_text(lv_dev_t.bar_label,dev[i].str);
         lv_bar_set_value(lv_dev_t.bar_rate, value, LV_ANIM_ON);
         ANIEND
         dev[i].fun();
         
    }
    lv_obj_del(lv_dev_t.bar_label);
    lv_obj_del(lv_dev_t.bar_rate);
    //lv_timer_create(time_task, 500,  NULL); //创建任务
    xTaskCreate(&time_task, "time_task", 1048*5, NULL, 5, NULL);
}
void sys_dev_init()
{
    sys_dev_lv_init();
    sys_dev_fun();
}
