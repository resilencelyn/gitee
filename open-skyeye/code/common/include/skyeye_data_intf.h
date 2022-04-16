#ifndef __SKYEYE_DATA_INTF_OPS_H__
#define __SKYEYE_DATA_INTF_OPS_H__
#include <sys/time.h>
#include "skyeye_types.h"
#include "skyeye_config.h"

typedef struct skyeye_file_data_intf{
	conf_object_t* conf_obj;
	exception_t (*read) (conf_object_t *conf_obj, void *buf, size_t count);
	exception_t (*write) (conf_object_t *conf_obj, void *buf, size_t count);
}skyeye_uart_data_intf;
#define SKYEYE_FILE_DATA_INTF_NAME "skyeye_file_data_intf"

typedef struct skyeye_file_data_timer_intf{
	conf_object_t* conf_obj;
	exception_t (*set_timer)(conf_object_t *obj, conf_object_t *dev_obj, int ms); 
}skyeye_file_data_timer_intf;
#define SKYEYE_FD_TIMER_INTF_NAME "skyeye_fd_timer_intf"

typedef struct skyeye_ad_file_data_intf{
	conf_object_t* conf_obj;
	exception_t (*read) (conf_object_t *conf_obj, void *buf, size_t count);
	exception_t (*write) (conf_object_t *conf_obj, void *buf, size_t count);
}skyeye_ad_data_intf;
#define SKYEYE_AD_FILE_DATA_INTF_NAME "skyeye_ad_file_data_intf"

#endif
