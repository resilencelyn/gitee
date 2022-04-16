#ifndef __RS422_LINKER_H
#define __RS422_LINKER_H


#define DEV_MAX_COUNT  31
typedef struct rs422_linker{
	conf_object_t *obj;                

	conf_object_t *device_list[DEV_MAX_COUNT];       /*all device link the fifo*/
	rs422_dev_intf *device_iface_list[DEV_MAX_COUNT];              
	uint32_t dev_cnt;
}rs422_linker_t;

#define OK     0
#define Error -1
#endif
