#include "page_logo.h"
#include "../blbl_sys.h"
LV_IMG_DECLARE(blbl22);
LV_IMG_DECLARE(blbl33);
LV_FONT_DECLARE(chinese);
extern struct _blbl_sys_t blbltv_t;
static char *get_welcome_str()
{
    char *hello="#0000ff 你好##ff00ff 世界#";
    return hello;
}
static void logo_font()
{
    lv_obj_t * label=lv_label_create(blbltv_t.scr.index);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -40);
    lv_label_set_recolor(label, true);//颜色重绘
    lv_obj_set_style_text_font(label,&chinese,0);
    lv_label_set_text(label,get_welcome_str());

    lv_anim_t afont;
    lv_anim_init(&afont);
    lv_anim_set_var(&afont, label);
    lv_anim_set_values(&afont, 0, 180);
    lv_anim_set_time(&afont, 1000);
    lv_anim_set_playback_delay(&afont, 800);
    lv_anim_set_playback_time(&afont, 300);

    lv_anim_set_path_cb(&afont, lv_anim_path_ease_in_out);

    lv_anim_set_exec_cb(&afont, anim_width_cb);
    lv_anim_start(&afont);
}
void sys_logo_satrt()
{    
    lv_obj_t * img22 = lv_img_create(blbltv_t.scr.index);
    lv_img_set_src(img22,&blbl22);
    lv_obj_set_pos(img22,-blbl22.header.w,SCR_H/2-20);

    lv_obj_t * img33 = lv_img_create(blbltv_t.scr.index);
    lv_img_set_src(img33,&blbl33);
    lv_obj_set_pos(img33,SCR_H,SCR_H/2-20);

    lv_anim_t a22;
    lv_anim_init(&a22);
    lv_anim_set_var(&a22, img22);
    lv_anim_set_values(&a22, -blbl22.header.w, SCR_W/2-blbl22.header.w);
    lv_anim_set_time(&a22, 1000);
    lv_anim_set_playback_delay(&a22, 1000);
    lv_anim_set_playback_time(&a22, 300);

    lv_anim_set_path_cb(&a22, lv_anim_path_ease_in_out);

    lv_anim_set_exec_cb(&a22, anim_x_cb);
    lv_anim_start(&a22);

    lv_anim_t a33;
    lv_anim_init(&a33);
    lv_anim_set_var(&a33, img33);
    lv_anim_set_values(&a33, SCR_W+blbl22.header.w, SCR_W/2);
    lv_anim_set_time(&a33, 1000);
    lv_anim_set_playback_delay(&a33, 1000);
    lv_anim_set_playback_time(&a33, 300);

    lv_anim_set_path_cb(&a33, lv_anim_path_ease_in_out);

    lv_anim_set_exec_cb(&a33, anim_x_cb);
    lv_anim_start(&a33);
    logo_font();
    ANIEND

}
