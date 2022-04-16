#ifndef __SKYEYE_DDS_INTF__
#define __SKYEYE_DDS_INTF__
typedef struct dds_interface{
	int (*start_transfer)(conf_object_t *obj);
	int (*is_transfered)(conf_object_t *obj);
}dds_intf;

#define DDS_INTF_NAME "dds_intf"

typedef struct {
	int(*send_data)(conf_object_t *dds_can_linker, void *buf, unsigned int len);
}dds_can_linker_intf;
#define DDS_CAN_LINKER_INTF "dds_can_linker_intf"

#endif 
