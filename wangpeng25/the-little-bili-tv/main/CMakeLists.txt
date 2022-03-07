set(COMPONENT_SRCS main.c blbl_sys.c page_ctrl.c 
    part_head/part_head.c page_logo/page_logo.c 
    page_dev/page_dev_ui.c page_dev/page_dev_fun.c 
    lv_lib_qrcode/lv_qrcode.c page_dev/fat.c
    page1/page1.c page1/time_font.c
    page2/page.c page2/fly.c page2/humi.c page2/temp.c
    page_set/page_set.c
                    INCLUDE_DIRS "."
                    )
set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()
spiffs_create_partition_image(storage ../html FLASH_IN_PROJECT)