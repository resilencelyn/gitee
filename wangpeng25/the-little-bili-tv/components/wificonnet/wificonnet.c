/*
mode by xiaoxiaoyudu
github :https://github.com/xiaoxiaoyudu
qq :1907383069
blbl:两位真实好友
*/
#include "wificonnet.h"
#include "esp_err.h"
#include "../read_nvs/updata.h"
static const char *TAG = "WIFI";
const int CONNECTED_BIT = BIT0;
const int ESPTOUCH_DONE_BIT = BIT1;
const int WIFI_SMART = BIT2;
const int WIFI_CONNET_BIT=BIT3;//配网连接
const int MQTT_CONNET_BIT=BIT4;
char ip_adder[20];
char ssid[40];
EventGroupHandle_t s_wifi_event_group;//wifi事件组
/**********下面是wifi网络********/
/*******wifi回调函数**********/
void event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data)//网络回调函数
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)//开始配网
        ESP_LOGI(TAG,"开始连接");
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        esp_wifi_connect();
        xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);//清除连接标志
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) //获取ip
    {
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);//设置连接标志状态
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        sprintf(ip_adder,"%s",ip4addr_ntoa((const struct ip4_addr *)&event->ip_info.ip));
       //ESP_LOGI(TAG, "got ip: %s", ip4addr_ntoa(&event->ip_info.ip));
       //  mqtt_app_start();
    } 
    else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE)
        ESP_LOGI(TAG, "Scan done");
    else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL)
        ESP_LOGI(TAG, "Found channel");
    else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD) {//获取密码
        ESP_LOGI(TAG, "Got SSID and password");

        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
        wifi_config_t wifi_config;
        char password[65];

        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
        wifi_config.sta.bssid_set = evt->bssid_set;
        if (wifi_config.sta.bssid_set == true)
            memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
        memcpy(ssid, evt->ssid, sizeof(evt->ssid));
        memcpy(password, evt->password, sizeof(evt->password));
        ESP_LOGI(TAG, "SSID:%s", ssid);
        ESP_LOGI(TAG, "PASSWORD:%s", password);

        /*打开wifipass工作区并保存密码*/
        if(save_nvs("wifi_ssid",ssid)&&save_nvs("wifi_pass",password))
        ESP_LOGI(TAG,"保存密码成功");
        
        ESP_ERROR_CHECK( esp_wifi_disconnect() );
        ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
        ESP_ERROR_CHECK( esp_wifi_connect() );
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE) {
        xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
    }
}
/******wifi初始化*****/
esp_err_t initialise_wifi(void)
{
    esp_netif_init();//tcp初始化
    ESP_ERROR_CHECK(esp_event_loop_create_default());//创建循环事件
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();//定义配置文件
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );//配置文件初始化
    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );//任何一个时间都回调
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );//STA模式
    ESP_ERROR_CHECK( esp_wifi_start() );//wifi启动

    return wifi_connet();
}
//配网任务
void smartconfig_example_task()
{
	ESP_LOGI(TAG,"start smartconfig");
    //EventBits_t uxBits;
    
    ESP_ERROR_CHECK( esp_smartconfig_set_type(SC_TYPE_ESPTOUCH_AIRKISS) );//选择esptouch和airkiss配网
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_smartconfig_start(&cfg) );
}

esp_err_t wifi_init(void)
{
    s_wifi_event_group = xEventGroupCreate();
    return initialise_wifi();//配网
}
esp_err_t wifi_connet(void)
{

        char password[65];
        ESP_LOGI(TAG,"读取nvs"); 
        if( read_nvs("wifi_ssid",ssid) )//读取ssid
        ESP_LOGI(TAG,"get ssid %s",ssid);
        else
        {
        //xTaskCreate(smartconfig_example_task, "smartconfig_task", 4096, NULL, 3, NULL);
        return ESP_FAIL;
        }
        read_nvs("wifi_pass",password);//读取密码
        EventBits_t uxBits;
        wifi_config_t wifi_config ;
        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, password, sizeof(wifi_config.sta.password));
        for(uint8_t i=0;i<2;i++)//自动连接两次，连接阻塞时间5秒
        {
           /*连接wifi*/
            ESP_LOGI(TAG,"connet  %d",i);
            ESP_ERROR_CHECK( esp_wifi_disconnect() );
            ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
            ESP_ERROR_CHECK( esp_wifi_connect() );
            /*进入阻塞态等待连接*/
            uxBits = xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT, false, false, 10000/portTICK_PERIOD_MS); 
            if(uxBits & CONNECTED_BIT) 
            {
            ESP_LOGI(TAG,"WiFi Connected to ap");
            return ESP_OK;
            }
        }
        return ESP_FAIL;
        //xTaskCreate(smartconfig_example_task, "smartconfig_task", 4096, NULL, 3, NULL);  
}
char * get_ip(void)
{
    return ip_adder;
}
char * get_ssid(void)
{
    return ssid;
}
