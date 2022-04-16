#ifndef __X86_DEVS_INTF__
#define __X86_DEVS_INTF__

typedef struct core_devices_fault_in{
	conf_object_t* conf_obj;
	void (*devs_save_fin)(conf_object_t *obj_conf, int did, unsigned int addr, int bitnum, int mode);
	fi_q_t* (*devs_get_fin)(conf_object_t *obj_conf, int did);
	void (*devs_clear_fin)(conf_object_t *obj_conf, int did, unsigned int addr, int bitnum, int mode);
}core_devices_fin;

#define DEVICE_FIN_INTF_NAME "device_fin_intf_name"

typedef struct core_devices{
	conf_object_t* conf_obj;
	char* (*get_devs_reg_name)(conf_object_t *conf, int did, int rid);
	unsigned int (*get_devs_reg_num)(conf_object_t *conf, int did);
	unsigned int (*get_devs_reg_width)(conf_object_t *conf, int did);
	unsigned int (*get_devs_reg_val)(conf_object_t *conf, int did, int rid);
	unsigned int (*get_devs_reg_large_val)(conf_object_t *conf, int did, int rid, int offset);
	char* (*get_devs_name)(conf_object_t *conf, int did);
	unsigned int (*get_devs_num)(conf_object_t *conf);
	unsigned int (*get_devs_reg_addr)(conf_object_t*conf, int did, int rid);
}core_devices_list;

#define DEVICE_LIST_INTF_NAME "device_list_intf_name"
#endif
