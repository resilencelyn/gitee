#ifndef __GLOBAL_SYMBOL_H__ 
#define __GLOBAL_SYMBOL_H__ 
struct registers 
{
	
};


struct global_symbol_device_t 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;
	
	struct registers regs;
	
};
typedef struct global_symbol_device_t global_symbol_device;
static char* regs_name[] = 
{
	NULL
};
#endif  
