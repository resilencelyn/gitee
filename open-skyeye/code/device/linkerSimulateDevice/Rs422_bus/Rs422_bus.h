#ifndef __Rs422_bus_H
#define __Rs422_bus_H


#define DEV_MAX_COUNT  31
typedef struct Rs422_bus{
	conf_object_t *obj;                

	conf_object_t *device_list[DEV_MAX_COUNT];       /*all device link the fifo*/
	Rs422_emif_intf *device_iface_list[DEV_MAX_COUNT];              
	Rs422_emif_intf* emif_iface;
	conf_object_t *emif_obj;                
	char *send_buf[DEV_MAX_COUNT];
	char *recv_buf;
	uint32_t dev_cnt;
}Rs422_bus_t;

#define OK     0
#define Error -1
#endif