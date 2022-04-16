#ifndef __RS422_BRIDGE_H__ 
#define __RS422_BRIDGE_H__ 
#include "rs422_bridge_fifo.h"
typedef struct input_device 
{
	conf_object_t* obj;
	FIFO *rec_fifo;

	conf_object_t* bus_obj;
	Rs422_bus_intf *bus_iface;

    char server_ip[16];
	int server_port;
	int sv_socket;
}rs422_bridge_device;

#define OK      0
#define Error  -1

#define IP_LENGTH       16 

#endif  
