#include "page_ctrl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "page_logo/page_logo.h"
#include "part_head/part_head.h"
#include "page_dev/page_dev_ui.h"
#include "page1/page1.h"
#include "page2/page.h"
#include "page_set/page_set.h"
#include "esp_log.h"
#include "button.h"
#include "lv_port_indev.h"
#define TAG "PAGE"
#define PAGE_NEXT 35
#define PAGE_LAST 14
#define PAGE_SET_ON 51
#define PAGE_SET_OFF 52
#define KEY_UP 26
#define KEY_DOWN 0
#define KEY_LEFT 14
#define KEY_RIGHT 35
#define KEY_ENTER 33
extern struct _blbl_sys_t blbltv_t;

page_t *page_head;
void page_ctrl();
void blbltv_init()
{
    sys_index_init();
    sys_head_ico_init();
}
void blbl_tv_start()
{
    blbltv_init();//结构体初始化
    button_init();//按键初始化
    sys_logo_satrt();//开启logo
    sys_dev_init();
	page_ctrl();
	blbltv_t.key_use=1;
}
page_t * page_menu_init()
{
    page_head=add_page(page_head);//建立链表头部
    page_head->start=page1_start;
    page_head->end=page1_end;

	static page_t * page2;
	page2=add_page(page_head);
	page2->start=page2_start;
	page2->end=page2_end;

    return page_head;
}
void page_switch()
{
	uint32_t user_data;
	user_data=blbltv_t.user_data;
    if(user_data)
    {
        blbltv_t.user_data=0;
		if(blbltv_t.key_use){
        	if(user_data==PAGE_LAST)//往前
        	{
        	    if(blbltv_t.page->last!=NULL)
        	    {  
					blbltv_t.page->end();
        	        blbltv_t.page=blbltv_t.page->last;  
        	        ESP_LOGI("SYSTEM","界面%d",blbltv_t.page->index);   
        	        blbltv_t.page->start();           
					ESP_LOGI("SYSTEM","esp_get_free_heap_size : %d ", esp_get_free_heap_size());
        	    }
        	    else
        	    {
        	        ESP_LOGI("SYSTEM","已到界面尽头");
        	        return;                
        	    }
        	}else if (user_data==PAGE_NEXT)//向后
        	{
        	    if(blbltv_t.page->next!=NULL)
        	    {
					blbltv_t.page->end();
        	        ESP_LOGI("SYSTEM","切换菜单");
        	        ESP_LOGI("SYSTEM","界面%d",blbltv_t.page->index); 
					ESP_LOGI("SYSTEM","esp_get_free_heap_size : %d ", esp_get_free_heap_size());
        	        blbltv_t.page=blbltv_t.page->next;      
        	        blbltv_t.page->start();				            
        	    }
        	    else
        	    {
        	        ESP_LOGI("SYSTEM","已到界面尽头");
        	        return;                   
        	    }
        	}
		}
		static uint8_t set=0;
		if(user_data==PAGE_SET_ON){
			if(!set) {
				ESP_LOGI("SYSTEM","进入设置");
				blbltv_t.key_use=0;
				blbltv_t.page->end();
				page_set_start();
				set=1;
			}else{
				ESP_LOGI("SYSTEM","退出设置");
				blbltv_t.page->start();
				blbltv_t.key_use=1;
				page_set_end();
				set=0;
			}
		}
		
   	}
}
void send_lvgl_key(uint32_t key)
{
	if(!blbltv_t.key_use)
	{
		keypad_send_key(key);
	}
}
void button_ctrl(void *arg)
{
	static TickType_t startTick=0, endTick, diffTick;
	static uint8_t io_num=0,last_key=0,key=88;
	const uint8_t button_arr[]={26,0,14,33,35};
	while(1)
	{
		for(uint8_t i=0;i<5;i++){
			if(gpio_get_level(button_arr[i]) == 0){
				key=button_arr[i];
				if(io_num==0){
					ESP_LOGI("button", "按键%dstart",key);
				}else{
					if(io_num%10==0){
						send_lvgl_key(key);//解码按键
						ESP_LOGI("button", "按键%d长按中",key);
					}
				}
				io_num++;
				break;
			}else
			{
				if(io_num&&(button_arr[i]==key))
				{

					ESP_LOGI("button", "按键%dend",button_arr[i]);
					if(io_num>=10){//长按
						ESP_LOGI("button", "按键%d长按结束",key);
						goto PASS;
					}
					if(startTick&&(last_key==key)){//检测双击
						endTick = xTaskGetTickCount();
		   				diffTick = endTick - startTick;
						if(diffTick<30){//双击
							ESP_LOGI("button", "按键%d双击",button_arr[i]);
							if (button_arr[i]==KEY_ENTER){
								blbltv_t.user_data=PAGE_SET_ON;
								goto PASS;

							}
						}
						ESP_LOGI("SYSTEM","esp_get_free_heap_size : %d ", esp_get_free_heap_size());
						startTick=0;
					}
					io_num=0;
					last_key=key;
					startTick = xTaskGetTickCount();//开始计时
					ESP_LOGI("button", "开始计时%d",startTick);
					break;
				PASS:
					io_num=0;
					startTick=0;
					last_key=key;
					break;
				}
			}
		}
		if(startTick){
			endTick = xTaskGetTickCount();
		   	diffTick = endTick - startTick;
			if(diffTick>20){
				blbltv_t.user_data=key;
				send_lvgl_key(key);//解码按键
				startTick=0;
			}
		}
		vTaskDelay(50/portTICK_RATE_MS);
	}
	vTaskDelete(NULL);
}
void page_ctrl()
{
    blbltv_t.page=page_menu_init();
    ESP_LOGI(TAG,"dev init ok");
	ESP_LOGI("SYSTEM","esp_get_free_heap_size : %d ", esp_get_free_heap_size());
    blbltv_t.page->start();
	xTaskCreate(&button_ctrl, "button_ctrl", 1048*5, NULL, 7, NULL);
}