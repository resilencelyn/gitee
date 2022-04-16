#ifndef ___T2080_H__ 
#define ___T2080_H__ 

struct t2080_device 
{
	conf_object_t* obj;
	core_signal_intf* signal_iface;
	conf_object_t* signal;
};

typedef struct t2080_device t2080_device;

#endif  
