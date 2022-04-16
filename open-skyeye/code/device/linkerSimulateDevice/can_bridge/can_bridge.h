#ifndef __CAN_BRIDGE_H__ 
#define __CAN_BRIDGE_H__ 
#include "can_bridge_fifo.h"
typedef struct input_device 
{
	conf_object_t* obj;
	FIFO *rec_fifo;

	conf_object_t* can_linker;
	can_linker_intf *can_linker_iface;
	int cnt;
	int timer_freq;
}can_bridge_device;

#define OK      0
#define Error  -1
#endif  
