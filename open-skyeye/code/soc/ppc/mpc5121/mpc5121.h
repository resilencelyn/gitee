#ifndef ___MPC5121_H__ 
#define ___MPC5121_H__ 
struct registers 
{
	
};


struct mpc5121_device 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;
	
	struct registers regs;
	
};
typedef struct mpc5121_device mpc5121_device;
static char* regs_name[] = 
{
	NULL
};
#endif  
