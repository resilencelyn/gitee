#ifndef _WIFISCH_H
#define _WIFISCH_H
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_smartconfig.h"


esp_err_t wifi_init(void);
esp_err_t initialise_wifi(void);
esp_err_t wifi_connet(void);
void smartconfig_example_task();
char * get_ip(void);
char * get_ssid(void);
#endif