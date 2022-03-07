/**
 * @file lv_port_indev_templ.c
 *
 */

 /*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev.h"
#include "lvgl.h"
#define KEY_UP 26
#define KEY_DOWN 0
#define KEY_LEFT 14
#define KEY_RIGHT 35
#define KEY_ENTER 33


// static void button_init(void);
// static bool button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
// static int8_t button_get_pressed_id(void);
// static bool button_is_pressed(uint8_t id);

/**********************
 *  STATIC VARIABLES
 **********************/



/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

uint8_t value=0;
uint32_t keypad_send_key(uint8_t key)
{
    /*Your code comes here*/

    switch(key){
        case KEY_UP:
            value= 2;
            break;
        case KEY_DOWN:
            value= 1;
            break;
        case KEY_LEFT:
            value=3;
            break;
        case KEY_RIGHT:
            value=4;
            break;
        case KEY_ENTER:
            value=5;
            break;
        default:
            value=0;
            break;
    }
    return value;
}
/*Will be called by the library to read the mouse*/
static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static uint32_t last_key = 0;

    /*Get the current x and y coordinates*/
    //mouse_get_xy(&data->point.x, &data->point.y);

    /*Get whether the a key is pressed and save the pressed key*/
    uint32_t act_key = value;
    if(act_key != 0) {
        data->state = LV_INDEV_STATE_PR;

        /*Translate the keys to LVGL control characters according to your key definitions*/
        switch(act_key) {
        case 1:
            act_key = LV_KEY_NEXT;
            break;
        case 2:
            act_key = LV_KEY_PREV;
            break;
        case 3:
            act_key = LV_KEY_LEFT;
            break;
        case 4:
            act_key = LV_KEY_RIGHT;
            break;
        case 5:
            act_key = LV_KEY_ENTER;
            break;
        }

        last_key = act_key;
            value=0;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }

    data->key = last_key;

}
lv_group_t * g ;
lv_group_t * get_key_g()
{
    return g;
}
void lv_port_indev_init(void)
{
    g= lv_group_create();
    static lv_indev_drv_t indev_drv;
    /*Initialize your encoder if you have*/
    /*Register a encoder input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv.read_cb = keypad_read;
    lv_indev_t * indev_encoder = lv_indev_drv_register(&indev_drv);
    lv_indev_set_group(indev_encoder, g);
}
#else /* Enable this file at the top */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
#endif
