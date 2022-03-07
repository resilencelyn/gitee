#ifndef _PART_DEV_FUN_H_
#define _PART_DEV_FUN_H_
#include "../../lvgl/lvgl.h"
void start_rate();
void wifi_rate();
void time_rate();
void weather_rate();
void fans_rate();
void web_rate();
void mqtt_rate();
void all_init();
void time_task(void *arg);
#endif // _LOGO_H_
