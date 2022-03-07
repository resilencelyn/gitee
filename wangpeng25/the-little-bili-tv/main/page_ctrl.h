#ifndef _CTRL_H_
#define _CTRL_H_
#include "../../lvgl/lvgl.h"
#include "blbl_sys.h"
void blbl_tv_start();
void page_ctrl();
void page_switch();
void encoder_read(lv_indev_drv_t * drv, lv_indev_data_t*data);
#endif // 
