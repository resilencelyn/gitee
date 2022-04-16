
#ifndef _SKYEYE_DEVICE_H_
#define _SKYEYE_DEVICE_H_

typedef struct State{
	conf_object_t *obj;
	conf_object_t *signal;
	general_signal_intf *signal_iface;
	int int_number;
	int target_id;
	unsigned int base_addr;
}skyeye_device_dev;
#endif
