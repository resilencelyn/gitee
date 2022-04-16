#ifndef __SKYEYE_FIFO_OPS_H__
#define __SKYEYE_FIFO_OPS_H__
#include <sys/time.h>
#include "skyeye_types.h"
#include "skyeye_config.h"

typedef struct skyeye_twoway_fifo_left{
	conf_object_t* conf_obj;
	exception_t (*read) (conf_object_t *conf_obj,  void *buf, size_t count);
	exception_t (*write) (conf_object_t *conf_obj,  void *buf, size_t count);
}skyeye_twoway_fifo_left_intf;
#define SKYEYE_TWOWAY_FIFO_LEFT_INTF_NAME "fifo_left"

typedef struct skyeye_twoway_fifo_right{
	conf_object_t* conf_obj;
	exception_t (*read) (conf_object_t *conf_obj,  void *buf, size_t count);
	exception_t (*write) (conf_object_t *conf_obj,  void *buf, size_t count);
}skyeye_twoway_fifo_right_intf;
#define SKYEYE_TWOWAY_FIFO_RIGHT_INTF_NAME "fifo_right"

typedef struct skyeye_twoway_fifo{
	conf_object_t* conf_obj;
	exception_t (*read) (conf_object_t *conf_obj, conf_object_t *device_obj, void *buf, size_t count);
	exception_t (*write) (conf_object_t *conf_obj, conf_object_t *device_obj,  void *buf, size_t count);
}skyeye_twoway_fifo_intf;
#define SKYEYE_TWOWAY_FIFO_INTF_NAME "skyeye_twoway_fifo_intf_name"


typedef struct c6713_fifo_interface{
	conf_object_t *conf_obj;
	int (*read_data)(conf_object_t* conf_obj, conf_object_t* device, void* recv_buf, uint32 dev_id, uint32 length);
	void (*write_data)(conf_object_t* conf_obj, conf_object_t* device, void *buf, uint32 dev_id, uint32 length);
	int (*register_device)(conf_object_t* conf_obj, conf_object_t* device, uint8 dev_id);
	void (*unregister_device)(conf_object_t* conf_obj, conf_object_t* device, uint8 dev_id);
}c6713_fifo_intf;

#define C6713_FIFO_INTF "c6713_fifo_intf"
#endif
