#ifndef __Bu61580_BRIDGE_H__ 
#define __BU61580_BRIDGE_H__ 
#include "bu61580_bridge_fifo.h"

#define DEV_MAX_COUNT  31
typedef struct input_device 
{
	conf_object_t* obj;
	FIFO *rec_fifo;
    
    	conf_object_t *bridge_device_list[DEV_MAX_COUNT];      
	bu61580_bridge_intf *bridge_device_iface_list[DEV_MAX_COUNT];              
	uint32_t dev_cnt;
    	char server_ip[16];
	int server_port;
	int sv_socket;
	RWLOCK_T lock;
}bu61580_bridge_device;

#define OK      0
#define Error  -1

#define IP_LENGTH       16 

#define BYTE_SIZE 	1
#define HALFWORD_SIZE 	2
#endif  
