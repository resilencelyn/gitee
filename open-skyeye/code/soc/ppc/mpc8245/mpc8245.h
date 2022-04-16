#ifndef ___MPC8245_H__ 
#define ___MPC8245_H__ 
struct registers 
{
	
};


struct mpc8245_device 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;
	
	struct registers regs;
	
};
typedef struct mpc8245_device mpc8245_device;
static char* regs_name[] = 
{
	NULL
};
#endif  
