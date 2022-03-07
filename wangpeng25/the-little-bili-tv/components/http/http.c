/*
mode by xiaoxiaoyudu
github :https://github.com/xiaoxiaoyudu
qq :1907383069
blbl:两位真实好友
*/
#include "http.h"
#include "updata.h"
#define WEB_PORT 80
//#define WEB_URL "/x/relation/stat?vmid=138699039"
blbl_follow_t wp_fans;
static const char *TAG_HTTP = "HTTP";
//static const char *HTTP_TAG = "WEATHER";
const char * BILIBILI_WEB_SERVER="bilibili.com";
const char * XINZHI_WEB_SERVER="api.seniverse.com" ;
const char *BILIBILI_REQUEST1="GET /x/relation/stat?vmid=";
const char *BILIBILI_REQUEST2=" HTTP/1.1\r\n"
                "Host: api.bilibili.com\r\n"
                "Connection: close\r\n"
                "\r\n\r\n";
const char *XINZHI_REQUEST1="GET /v3/weather/daily.json?key=";
const char *XINZHI_REQUEST2="&location=";
const char *XINZHI_REQUEST3="&language=zh-Hans&start=0&days=3 HTTP/1.1\r\n"
                "Host: api.seniverse.com\r\n"
                "Connection: close\r\n"
                "\r\n";

sys_weather_t weather_data[3];
char city[20];
char *get_city()
{
    return city;
}
esp_err_t http_get_task(const char *REQUEST,const char * WEB_SERVER,esp_err_t (*fun)(char *text))
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    int s, r;
    char recv_buf[128];
    char * get_http=(char*)malloc(3*1024);
     //dns解析
    int err = getaddrinfo(WEB_SERVER, "80", &hints, &res);

    if(err != 0 || res == NULL)
    {
        ESP_LOGE(TAG_HTTP, "DNS lookup failed err=%d res=%p", err, res);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        free(get_http);
        return ESP_FAIL;
    }
    //获取ip地址
    //addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
    //ESP_LOGI(TAG_HTTP, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));
    //建立socket
    s = socket(res->ai_family, res->ai_socktype, 0);
    if(s < 0) {
        ESP_LOGE(TAG_HTTP, "... Failed to allocate socket.");
        freeaddrinfo(res);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
          free(get_http);
         return ESP_FAIL;
    }
    //ESP_LOGI(TAG_HTTP, "... allocated socket");
    //建立连接
    if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
        ESP_LOGE(TAG_HTTP, "... socket connect failed errno=%d", errno);
        close(s);
        freeaddrinfo(res);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
            free(get_http);
        return ESP_FAIL;
    }
    //ESP_LOGI(TAG_HTTP, "... connected");
    freeaddrinfo(res);
    //发送http请求
    if (write(s, REQUEST, strlen(REQUEST)) < 0) {
        ESP_LOGE(TAG_HTTP, "... socket send failed");
        close(s);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
            free(get_http);
        return ESP_FAIL;
    }
    //ESP_LOGI(TAG_HTTP, "... socket send success");
    struct timeval receiving_timeout;
    receiving_timeout.tv_sec = 5;
    receiving_timeout.tv_usec = 0;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
        sizeof(receiving_timeout)) < 0) {
        ESP_LOGE(TAG_HTTP, "... failed to set socket receiving timeout");
        close(s);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
            free(get_http);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG_HTTP, "... set socket receiving timeout success");
     /* Read HTTP response */
    do {
        bzero(recv_buf, sizeof(recv_buf));
        r = read(s, recv_buf, sizeof(recv_buf)-1);
        strcat(get_http,recv_buf);

        } while(r > 0);
    //printf("%s\n",get_http);
    if(fun(get_http)==ESP_OK)
    {
       
        //ESP_LOGI(TAG_HTTP, "... done reading from socket. Last read return=%d errno=%d.", r, errno);
        close(s);
        free(get_http);
        return ESP_OK;        
    }
    else
    {
        close(s);
        free(get_http);
        return ESP_FAIL;
    } 
    if(get_http!=NULL)
    free(get_http);
    close(s);
    return ESP_OK; 

}
esp_err_t cjson_bilibili(char *text)
{
     cJSON *root_blbl,*psub_blbl;
    char *index=strchr(text,'{');
    if(index==NULL)
    {
        return ESP_FAIL;
    }else
    {
        strcpy(text,index);
        root_blbl = cJSON_Parse(text);
        if(root_blbl!=NULL)
        {
            psub_blbl = cJSON_GetObjectItem(root_blbl, "data");
            if(psub_blbl!=NULL)
            {
                cJSON *follower=cJSON_GetObjectItem( psub_blbl, "follower");
                wp_fans.follower=follower->valueint;
                cJSON *following=cJSON_GetObjectItem( psub_blbl, "following");
                wp_fans.following=following->valueint;
                ESP_LOGI(TAG_HTTP,"follower:%d  following:%d\n",wp_fans.follower,wp_fans.following);
            }
            else
            {
                ESP_LOGI(TAG_HTTP,"获取粉丝失败，请检查uid");
            }
        }else{
            return ESP_FAIL;
        }
        cJSON_Delete(root_blbl);  
    }
    return ESP_OK;

}
esp_err_t cjson_weather(char *text)
{
    cJSON *root,*psub;
    cJSON *arrayItem;
  
    char *index=strchr(text,'{');
    if(index==NULL)
    {
        return ESP_FAIL;
    }else
    {
        strcpy(text,index);
        root = cJSON_Parse(text);
        if(root!=NULL)
        {
            psub = cJSON_GetObjectItem(root, "results");
            if(psub!=NULL)
            {
            arrayItem = cJSON_GetArrayItem(psub,0);
            cJSON *now = cJSON_GetObjectItem(arrayItem, "daily");
            //ESP_LOGI(HTTP_TAG,"code_day:%s",now->valuestring);
            for(uint8_t i=0;i<3;i++)
            {
                cJSON *array2=cJSON_GetArrayItem(now,i);            
                cJSON *code_day = cJSON_GetObjectItem(array2, "code_day");
                //sprintf(weather_data[i].code_day,"%d",atoi(code_day->valuestring));
                weather_data[i].code_day=atoi(code_day->valuestring);         
                cJSON *text_day = cJSON_GetObjectItem(array2, "text_day");
                strcpy(weather_data[i].text_day,text_day->valuestring);
                cJSON *high = cJSON_GetObjectItem(array2, "high");
                weather_data[i].high=atoi(high->valuestring);         
                cJSON *low = cJSON_GetObjectItem(array2, "low");
                weather_data[i].low=atoi(low->valuestring);           
                cJSON *rainfall = cJSON_GetObjectItem(array2, "rainfall");
                weather_data[i].rainfall=atof(rainfall->valuestring);           
                cJSON *wind_direction_degree = cJSON_GetObjectItem(array2, "wind_direction_degree");
                weather_data[i].wind_direction_degree=atoi(wind_direction_degree->valuestring);
                cJSON *wind_direction = cJSON_GetObjectItem(array2, "wind_direction");
                strcpy(weather_data[i].wind_direction,wind_direction->valuestring);            
                cJSON *wind_speed = cJSON_GetObjectItem(array2, "wind_speed");
                weather_data[i].wind_speed=atof(wind_speed->valuestring);
                cJSON *humidity = cJSON_GetObjectItem(array2, "humidity");
                weather_data[i].humidity=atoi(humidity->valuestring);                   
            }
            cJSON *location = cJSON_GetObjectItem(arrayItem, "location");//城市
            cJSON *text_city = cJSON_GetObjectItem(location, "name");
            strcpy(city,text_city->valuestring);
            }else
            {
                ESP_LOGI("HTTP","获取天气失败，请检测是否有改接口权力");
            }
        }else
        {
            return ESP_FAIL;
        }
        cJSON_Delete(root);
    }
    return ESP_OK;
}
sys_weather_t * get_weather()
{
    char str[300];
    char city[30];
    char key[30];
    if(read_nvs("city",city))//获取哔哩哔哩uid
        ESP_LOGI(TAG_HTTP,"获取到城市");
    else
    {
        sprintf(city,"nanjing");
        ESP_LOGI(TAG_HTTP,"没有获取到城市，默认使用南京");
    }
    if(read_nvs("pass",key))//获取哔哩哔哩uid
        ESP_LOGI(TAG_HTTP,"获取到心知密匙");
    else
    {
        sprintf(key,"SSuB-x5pvaLBddTr_");
        ESP_LOGI(TAG_HTTP,"没有获取心知密匙，使用默认");
    }
    sprintf(str,"%s%s%s%s%s",XINZHI_REQUEST1,key,XINZHI_REQUEST2,city,XINZHI_REQUEST3);
    //ESP_LOGI(TAG_HTTP,"%s",str);
    while (http_get_task((const char *)str,XINZHI_WEB_SERVER,cjson_weather)==ESP_FAIL)
    {
        vTaskDelay(10);
    }
    return weather_data;
}
esp_err_t get_fans()
{
    char str[200];
    char uid[15];
    if(read_nvs("uid",uid))//获取哔哩哔哩uid
    {
        sprintf(str,"%s%s%s",BILIBILI_REQUEST1,uid,BILIBILI_REQUEST2);
        ESP_LOGI(TAG_HTTP,"获取到uid");
    }
    else
    {
        sprintf(str,"%s138699039%s",BILIBILI_REQUEST1,BILIBILI_REQUEST2);
        ESP_LOGI(TAG_HTTP,"没有uid，使用默认uid");
    }
    while (http_get_task((const char *)str,BILIBILI_WEB_SERVER,cjson_bilibili)==ESP_FAIL)
    {
        vTaskDelay(10);
    }
    return ESP_OK;

}
uint16_t get_value_blblfs(uint8_t i)
{
    if(i)
    return wp_fans.follower;
    else
    {
    return wp_fans.following;
    }
    
}
