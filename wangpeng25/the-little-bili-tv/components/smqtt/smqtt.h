#ifndef _KMQTT_
#define _KMQTT_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "mqtt_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
esp_err_t cjson_mqtt(char *text,esp_mqtt_client_config_t *mqtt_cfg);
void mqtt_app_start(esp_mqtt_client_config_t *mqtt_cfg);
void mqtt_push_message(char * topic,char * str);
esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event);
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
#endif