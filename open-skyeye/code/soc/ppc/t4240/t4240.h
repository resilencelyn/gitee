#ifndef ___T4240_H__ 
#define ___T4240_H__ 

struct t4240_device_t 
{
	conf_object_t* obj;
	core_signal_intf* signal_iface;
	conf_object_t* signal;
};

typedef struct t4240_device_t t4240_device;

#endif  
