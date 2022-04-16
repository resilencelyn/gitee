
#ifndef _SKYEYE_DEVICE_H_
#define _SKYEYE_DEVICE_H_

typedef struct State
{
    conf_object_t *obj;
    conf_object_t *signal;
    general_signal_intf *signal_iface;
    int int_number;
} skyeye_device_dev;
#endif
