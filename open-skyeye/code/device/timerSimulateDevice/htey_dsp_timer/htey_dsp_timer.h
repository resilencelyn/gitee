#ifndef __HTEY_DSP_TIMER_H__ 
#define __HTEY_DSP_TIMER_H__ 
struct registers 
{
	
	uint32_t ctrl;
	
	uint32_t prd;
	
	uint32_t cnt;
	
};


struct timer_device 
{
	
	conf_object_t* obj;
	
	core_signal_intf* core_signal;
	
	conf_object_t* core;

	time_handle_t* handle;

	int scale;
	
	struct registers regs;

	int interrupt_number;
	
};
typedef struct timer_device htey_dsp_timer_device;
static char* regs_name[] = 
{
	"ctrl",
	"prd",
	"cnt",
	NULL
};
#define CTRL 0x0
#define PRD 0x4
#define CNT 0x8
#endif  
