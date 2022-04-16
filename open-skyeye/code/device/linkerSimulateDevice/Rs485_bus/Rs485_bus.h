#ifndef __Rs485_bus_H
#define __Rs485_bus_H


#define DEV_MAX_COUNT  31
typedef struct Rs485_bus{
	conf_object_t *obj;                

	conf_object_t *device_list[DEV_MAX_COUNT];       /*all device link the fifo*/
	Rs485_emif_intf *device_iface_list[DEV_MAX_COUNT];              
	Rs485_emif_intf* emif_iface;
	conf_object_t *emif_obj;                
	char *send_buf[DEV_MAX_COUNT];
	int dev_cnt;
	char *recv_buf;
}Rs485_bus_t;

#define OK     0
#define Error -1
#endif
