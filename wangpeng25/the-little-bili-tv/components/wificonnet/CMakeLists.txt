idf_component_register(SRCS "wificonnet.c"
                    INCLUDE_DIRS "." "../read_nvs/"
                    REQUIRES spiffs nvs_flash esp_wifi wpa_supplicant esp_netif read_nvs)

