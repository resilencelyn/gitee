#ifndef __BMAN_T2080_H__ 
#define __BMAN_T2080_H__ 


struct BMan_T2080_device_t 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	conf_object_t* signal;
	
	memory_space_intf* image_space;
	conf_object_t* image;
	
	uint32_t regs[0x1000];
	uint8_t sp_regs[0x2000000];
	
	
};
typedef struct BMan_T2080_device_t BMan_T2080_device;
static char* regs_name[] = 
{
	NULL
};
#endif  
