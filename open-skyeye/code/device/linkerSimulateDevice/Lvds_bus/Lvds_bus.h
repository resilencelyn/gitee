#ifndef __LVDS_BUS_H
#define __LVDS_BUS_H


#define DEV_MAX_COUNT  31
typedef struct Lvds_bus{
	conf_object_t *obj;                

	conf_object_t *device_list[DEV_MAX_COUNT];       /*all device link the fifo*/
	Lvds_emif_intf *device_iface_list[DEV_MAX_COUNT];              
	Lvds_emif_intf* emif_iface;
	conf_object_t *emif_obj;                
	unsigned char *send_buf[DEV_MAX_COUNT];
	unsigned char *recv_buf;
	uint32_t fifo_cnt;
	uint32_t data_length;
}Lvds_bus_t;

#define OK     0
#define Error -1
#endif
