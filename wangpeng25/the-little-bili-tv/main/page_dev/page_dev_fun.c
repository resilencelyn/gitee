#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "page_dev_fun.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_spiffs.h"
#include "esp_vfs.h"
#include "esp_err.h"
#include "wificonnet.h"
#include "page_dev_ui.h"
#include "../part_head/part_head.h"
#include "http.h"
#include "gui_time.h"
#include "updata.h"
#include "smqtt.h"
#include "file_server.h"
#include "esp_littlefs.h"
#include "sht30.h"
#include "fat.h"
#define TAG "DEV"
extern blbl_follow_t wp_fans;
extern sht30_t sht;
extern struct _blbl_sys_t blbltv_t;
extern sys_weather_t weather_data[3];
esp_mqtt_client_config_t mqtt_cfg;
static void SPIFFS_Directory(char * path) {
	DIR* dir = opendir(path);
	assert(dir != NULL);
	while (true) {
		struct dirent*pe = readdir(dir);
		if (!pe) break;
		ESP_LOGI(__FUNCTION__,"d_name=%s d_ino=%d d_type=%x", pe->d_name,pe->d_ino, pe->d_type);
	}
	closedir(dir);
}
void start_rate()
{
    ESP_LOGI(TAG,"SPIFFS INIT");
	esp_vfs_spiffs_conf_t conf = {
		.base_path = "/spiffs",
		.partition_label = NULL,
		.max_files = 8,
		.format_if_mount_failed =true
	};
	esp_err_t ret =esp_vfs_spiffs_register(&conf);
	if (ret != ESP_OK) 
	{
		if (ret == ESP_FAIL)
			ESP_LOGE(TAG, "Failed to mount or format filesystem");
		else if (ret == ESP_ERR_NOT_FOUND)
			ESP_LOGE(TAG, "Failed to find SPIFFS partition");
		else
			ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)",esp_err_to_name(ret));
		return;
	}
	SPIFFS_Directory("/spiffs/");
    ESP_LOGI(TAG,"NVS INIT");
	ESP_ERROR_CHECK( nvs_flash_init() );
	sht30_init();
	blbltv_t.blbl_info_t=&wp_fans;
	blbltv_t.temp_hmui_t=&sht;
	blbltv_t.weather_t=&weather_data;
    //adc_init();
}
void wifi_rate()
{
    if(wifi_init()!=ESP_OK)
        switch_wifi();
	else
    	lv_ico_show(&blbltv_t.head_t.wifi ,1);
}

void time_rate()
{
	vTaskDelay(10);
    time_init();
	ESP_LOGI(TAG, "获取到时间");
}
void weather_rate()
{
	get_weather();
	vTaskDelay(10);//此处玄学延迟，去掉后可能导致异常
	ESP_LOGI(TAG, "获取到天气");
}
void fans_rate()
{
	get_fans();
	vTaskDelay(10);
	ESP_LOGI(TAG, "获取到粉丝数");
}
void web_rate()
{
	
	if(start_file_server("/spiffs")!=ESP_OK)//启动文件系统
    {
        blbltv_t.state_t.web=0;    
        ESP_LOGI("WBE","web启动失败");     
    }else
	{
		lv_ico_show(&blbltv_t.head_t.web ,1);
		ESP_LOGI("WBE", "web初始化");
	}
	vTaskDelay(10);
}
void lv_blbl_mqtt_init()
{
    char info[250];
    if(read_nvs("mqtt",info))//读取mqtt状态
    {
        if(!strcmp("y",info))//开启mqtt
        {
            FILE* fp = fopen("/spiffs/mqtt.txt", "rb");
		    if (fp == NULL) {
				ESP_LOGI("MQTT", "mqtt文件找不到");
				blbltv_t.state_t.mqtt=0;
				return ;
		    }
            fgets( info, 250,fp );
            if(cjson_mqtt(info,&mqtt_cfg)==ESP_OK)
            {
               	ESP_LOGI("MQTT", "获取mqtt账号密码"); 
            }
            else
            {
            	blbltv_t.state_t.mqtt=0;
            }    
            fclose(fp);
            mqtt_app_start(&mqtt_cfg);
            blbltv_t.state_t.mqtt=1; 
			lv_ico_show(&blbltv_t.head_t.mqtt ,1);
        }
        else//关闭mqtt
        {
            blbltv_t.state_t.mqtt=0;
        }
    }
    else//第一次使用mqtt未选择
    {
        blbltv_t.state_t.mqtt=0;        
        ESP_LOGI("MQTT","mqtt未使用");
        save_nvs("mqtt","n");
    }
}
void mqtt_rate()
{
	vTaskDelay(10);
	lv_blbl_mqtt_init();
	ESP_LOGI(TAG, "mqtt初始化");
}
void time_task(void *arg)//时间任务，一直在lvgl后台运行
{
	static uint32_t t=1;
	while(1)
	{
    	get_time(&(blbltv_t.timeinfo_t));//获取时间
		if(t%(2*5)==0)//获取温湿度
			sht30_get_value();
		if(t%(2*60*10)==0)//10分钟//粉丝数
		{
			get_fans();
		}
		if(t==(2*60*35))//30分钟//天气
		{
			get_weather();
			t=0;
		}
		t++;
		vTaskDelay(500/portTICK_RATE_MS);
	}
}
void all_init()
{
	fsf_init();
	//xTaskCreate(&time_task, "time", 1048*4, NULL, 5, NULL);
	ESP_LOGI("SYSTEM","esp_get_free_heap_size : %d ", esp_get_free_heap_size());
}

