idf_component_register(SRCS "file_server.c"
                    INCLUDE_DIRS "." "../read_nvs"
                    REQUIRES spiffs esp_https_server nvs_flash
                    EMBED_FILES "favicon.ico" "upload_script.html")

