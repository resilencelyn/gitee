#include "blbl_sys.h"
struct _blbl_sys_t blbltv_t;
page_t *page_head;
void anim_x_cb(void * var, int32_t v)
{
    lv_obj_set_x(var, v);
}
void anim_y_cb(void * var, int32_t v)
{
    lv_obj_set_y(var, v);
}
void anim_width_cb(void * var, int32_t v)
{
    lv_obj_set_size(var, v,20);
    // lv_obj_set_height(var, v);
}
page_t * add_page(page_t * page)
{
    if(page==NULL)//页头初始化
    {
        page=(page_t *)malloc(sizeof(page_t));
        page->index=0;
        page->next=NULL;
        page->last=NULL;
     //   ESP_LOGI("SYSTEM","添加页面0");
        return page;
    }
    page_t *p,*now;//添加到page后
    now=page;
	while(now->next!= NULL)		//找到同级菜单最后的表
    {
        now =now->next;
    }
    p = (page_t *)malloc(sizeof(page_t));
    now->next=p;
    p->last=now;
    p->next=NULL;
    p->index=(now->index)+1;
//    ESP_LOGI("SYSTEM","添加页面%d",p->index);
    return p;
}
void sys_index_init()
{
    blbltv_t.scr.index = lv_scr_act();//获取当前活跃的屏幕对象
    // 屏幕主体界面
    static lv_style_t style_index;
    blbltv_t.scr_style.index=&style_index;
    //lv_style_set_bg_color(&style_index,lv_palette_main(LV_PALETTE_LIGHT_BLUE));//设置屏幕背景
    lv_obj_clear_flag(blbltv_t.scr.index, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(blbltv_t.scr.index,&style_index,LV_PART_MAIN);
}
/*
page_t * lv_page_menu_init()
{
    page_head=add_page(page_head);//建立链表头部
    page_head->start=page1_start;
    page_head->end=page1_end;
    static page_t *page2;
    page2=add_page(page_head);
    page2->start=page2_start;
    page2->end=page2_end;
    static page_t *page4;
    page4=add_page(page_head);
    page4->start=page4_start;
    page4->end=page4_end;
    static page_t *page5;
    page5=add_page(page_head);
    page5->start=page5_start;
    page5->end=page5_end;
    return page_head;
}*/
