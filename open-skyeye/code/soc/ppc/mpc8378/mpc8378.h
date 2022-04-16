#ifndef ___MPC8378_H__ 
#define ___MPC8378_H__ 
struct registers 
{
	
};


struct mpc8378_device 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;
	
	struct registers regs;
	
};
typedef struct mpc8378_device mpc8378_device;
static char* regs_name[] = 
{
	NULL
};
#endif  
