#ifndef __SKYEYE_EMIF_OPS_H__
#define __SKYEYE_EMIF_OPS_H__
#include <sys/time.h>
#include "skyeye_types.h"
#include "skyeye_config.h"

typedef struct skyeye_emif_interface{
	conf_object_t* conf_obj;
	exception_t (*set_r_status) (conf_object_t *opaque, int set_flag);
	exception_t (*set_w_status) (conf_object_t *opaque, int set_flag);
	exception_t (*frame_head) (conf_object_t *opaque, int set_flag);
	exception_t (*frame_tail) (conf_object_t *opaque, int set_flag);
	exception_t (*device_ready) (conf_object_t *opaque);
}skyeye_emif_intf;

#define SKYEYE_EMIF_INTF	"skyeye_emif_intf"


typedef struct c6713_emif_iface{
	conf_object_t* conf_obj;
	exception_t (*set_state) (conf_object_t *conf_obj);
}c6713_emif_intf;
#define C6713_EMIF_INTF "c6713_emif_intf"

typedef struct Rs485_emif_iface{
	conf_object_t* conf_obj;
	exception_t (*set_state) (conf_object_t *conf_obj, char *buf, int Length);
}Rs485_emif_intf;
#define RS485_EMIF_INTF "Rs485_emif_intf"

typedef struct Rs422_emif_iface{
	conf_object_t* conf_obj;
	exception_t (*set_state) (conf_object_t *conf_obj, char *buf, int Length);
}Rs422_emif_intf;
#define RS422_EMIF_INTF "Rs422_emif_intf"

typedef struct Rs232_emif_iface{
	conf_object_t* conf_obj;
	exception_t (*set_state) (conf_object_t *conf_obj, char *buf, int Length, int dev_id);
}Rs232_emif_intf;
#define RS232_EMIF_INTF "Rs232_emif_intf"

typedef struct Lvds_emif_iface{
	conf_object_t* conf_obj;
	exception_t (*set_state) (conf_object_t *conf_obj, unsigned char *buf);
}Lvds_emif_intf;
#define LVDS_EMIF_INTF "Lvds_emif_intf"

typedef struct arinc_emif_iface{
	conf_object_t* conf_obj;
	exception_t (*transmit_data) (conf_object_t *conf_obj, char *buf, int Length, int dev_id);
}arinc_emif_intf;
#define ARINC_EMIF_INTF "arinc_emif_intf"
#endif
