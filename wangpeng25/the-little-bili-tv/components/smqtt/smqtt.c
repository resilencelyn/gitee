#include "smqtt.h"
#include "esp_log.h"
esp_mqtt_client_handle_t client;
static const char *TAG = "mqtt";

extern const int MQTT_CONNET_BIT;
extern EventGroupHandle_t s_wifi_event_group;//wifi事件组
esp_err_t cjson_mqtt(char *text,esp_mqtt_client_config_t *mqtt_cfg)
{
    
    cJSON *root_mqtt;
    char *index=strchr(text,'{');
    if(index==NULL)
    {
        return ESP_FAIL;
    }else
    {
        strcpy(text,index);
        root_mqtt = cJSON_Parse(text);
        if(root_mqtt!=NULL)
        {
            static char host[30];
            static char id[30];
            static char name[30];
            static char pwd[30];
            cJSON *js_host=cJSON_GetObjectItem( root_mqtt, "host");
            if(js_host==NULL)
            {
                ESP_LOGI("BLBL_MQTT", "JSON读取mqtt失败");
                return ESP_FAIL;
            }
            strcpy(host,js_host->valuestring);
            ESP_LOGI("BLBL_MQTT", "********host:%s*******",host);

            cJSON *js_id=cJSON_GetObjectItem( root_mqtt, "id");
            if(js_id==NULL)
            {
                ESP_LOGI("BLBL_MQTT", "JSON读取id失败");
                return ESP_FAIL;
            }
            strcpy(id,js_id->valuestring);
            ESP_LOGI("BLBL_MQTT", "********id%s*******",id);   

            cJSON *js_name=cJSON_GetObjectItem( root_mqtt, "name");
            if(js_name==NULL)
            {
                ESP_LOGI("BLBL_MQTT", "JSON读取name失败");
                return ESP_FAIL;
            }
            strcpy(name,js_name->valuestring);
            ESP_LOGI("BLBL_MQTT", "********name:%s**********",name);  

            cJSON *js_pwd=cJSON_GetObjectItem( root_mqtt, "pwd");
            if(js_pwd==NULL)
            {
                ESP_LOGI("BLBL_MQTT", "JSON读取密码失败");
                return ESP_FAIL;
            }
            strcpy(pwd,js_pwd->valuestring);
            ESP_LOGI("BLBL_MQTT", "********密码:%s**********",pwd);

            cJSON *js_keep=cJSON_GetObjectItem( root_mqtt, "keep");
            if(js_keep==NULL)
            {
                ESP_LOGI("BLBL_MQTT", "JSON读取keep失败");
                return ESP_FAIL;
            }

            mqtt_cfg->host = host;
            mqtt_cfg->username=name;
            mqtt_cfg->password=pwd;
            mqtt_cfg->client_id=id;
            mqtt_cfg->keepalive=js_keep->valueint;

        }else{
            return ESP_FAIL;
        }
        cJSON_Delete(root_mqtt);  
    }
    return ESP_OK;

}
void ctrl_something(char *text)
{
    cJSON *root;
    //截取有效json
    char *index=strchr(text,'{');
    strcpy(text,index);

    root = cJSON_Parse(text); 
    if(root!=NULL)
    {
       // cJSON *c = cJSON_GetObjectItem(root,"id");
    }
    cJSON_Delete(root);
}
void mqtt_app_start(esp_mqtt_client_config_t *mqtt_cfg)
{
    mqtt_cfg->event_handle = mqtt_event_handler;
#if CONFIG_BROKER_URL_FROM_STDIN
    char line[128];

    if (strcmp(mqtt_cfg.uri, "FROM_STDIN") == 0) {
        int count = 0;
        printf("Please enter url of mqtt broker\n");
        while (count < 128) {
            int c = fgetc(stdin);
            if (c == '\n') {
                line[count] = '\0';
                break;
            } else if (c > 0 && c < 127) {
                line[count] = c;
                ++count;
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        mqtt_cfg.uri = line;
        printf("Broker url: %s\n", line);
    } else {
        ESP_LOGE(TAG, "Configuration mismatch: wrong broker url");
        abort();
    }
#endif /* CONFIG_BROKER_URL_FROM_STDIN */

    client = esp_mqtt_client_init(mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}
/******下面是mqtt回调函数******/
esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            xEventGroupSetBits(s_wifi_event_group, MQTT_CONNET_BIT);//设置连接标志状态
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_publish(client, "blbl", "hello", 0, 1, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            msg_id = esp_mqtt_client_subscribe(client, "temp", 0);
            break;
        case MQTT_EVENT_DISCONNECTED:
            xEventGroupClearBits(s_wifi_event_group, MQTT_CONNET_BIT);//设置连接标志状态
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
          //  ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
          //  ESP_LOGI(TAG, "MQTT_EVENT_DATA");
          //  printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
          //  printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}
void mqtt_push_message(char * topic,char * str)
{
    esp_mqtt_client_publish(client, (const char *)topic,  (const char *)str, 0, 1, 0);
}