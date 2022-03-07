#include "part_head.h"
#include "../blbl_sys.h"
#include "esp_log.h"
extern struct _blbl_sys_t blbltv_t;
//----------------------------------
//通知栏显示控制
//0：不显示；1：显示
//----------------------------------
void sys_src_head_show(int8_t flag)
{
    if(!flag)
    lv_obj_add_flag(blbltv_t.scr.head, LV_OBJ_FLAG_HIDDEN);
    else
    lv_obj_clear_flag(blbltv_t.scr.head, LV_OBJ_FLAG_HIDDEN);
}
//----------------------------------
//通知栏初始化
//----------------------------------
void sys_head_ico_init()
{
        //屏幕通知栏
    static lv_style_t style_head;
    blbltv_t.scr_style.head=&style_head;

    lv_style_set_radius(&style_head, 0);//无圆角
    lv_style_set_border_width(&style_head, 0);//无边框
    blbltv_t.scr.head = lv_obj_create(blbltv_t.scr.index);//屏幕主体
    lv_obj_set_size(blbltv_t.scr.head,SCR_W-30,20);
    lv_obj_set_pos(blbltv_t.scr.head,30,0);

    lv_style_set_pad_column(&style_head,5);//间距
    lv_style_set_pad_top(&style_head,0);
    lv_style_set_pad_left(&style_head,0);

    lv_obj_clear_flag(blbltv_t.scr.head, LV_OBJ_FLAG_SCROLLABLE);//不可滚动

    lv_style_set_bg_opa(&style_head, LV_OPA_TRANSP);//通知栏透明
    lv_obj_add_style(blbltv_t.scr.head,&style_head , LV_PART_MAIN);
    lv_obj_set_flex_flow(blbltv_t.scr.head, LV_FLEX_FLOW_ROW );

    //通知栏图标初始化
    blbltv_t.head_t.wifi.str=LV_SYMBOL_WIFI;
    blbltv_t.head_t.mqtt.str=LV_SYMBOL_GPS;
    blbltv_t.head_t.temp.str=LV_SYMBOL_SHUFFLE;
    blbltv_t.head_t.web.str=LV_SYMBOL_UPLOAD;
}
//----------------------------------
//通知栏图标显示
//----------------------------------
int8_t sys_head_ico_show(int8_t flag,lv_obj_t ** label)
{
    if(flag)//创建图标
    {
        if(*label!=NULL)//图标已存在
            return 1;
        else
        {
            *label = lv_label_create(blbltv_t.scr.head);

        }
    }else//删除图标
    {
        if(*label==NULL)//图标不存在
            return 2;
        else
        {
            lv_obj_del(*label);
        }
    }
    return 0;
}
//----------------------------------
//wifi图标显示
//----------------------------------
static void lv_ico_move(lv_obj_t **ico_p)
{
    lv_anim_t move_ico;
    lv_anim_init(&move_ico);
    lv_anim_set_var(&move_ico,*ico_p);
    lv_anim_set_values(&move_ico, -16, 0);
    lv_anim_set_time(&move_ico, 1000);
    lv_anim_set_path_cb(&move_ico, lv_anim_path_ease_in_out);
    lv_anim_set_exec_cb(&move_ico, anim_y_cb);
    lv_anim_start(&move_ico);
}
int8_t lv_ico_show(ico_t *ico,int8_t flag)
{
    lv_obj_t **wifi=&ico->obj;
    int8_t re=sys_head_ico_show(flag,wifi);
    if(!re)
    {
        lv_label_set_text(ico->obj,"");
        lv_obj_t *label = lv_label_create(ico->obj);
        lv_label_set_text(label, ico->str);
        lv_ico_move(&label);
    }
    else
    {
        if(re==1)
            ESP_LOGI("ICO","wifi图标已存在");
        else
            ESP_LOGI("ICO","wifi图标不存在");
        return -1;
    }
    return 0;
}

