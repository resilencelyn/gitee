#ifndef __C687_TIMER_H__ 
#define __C687_TIMER_H__ 
struct registers 
{
	uint32_t  ir;
	uint32_t  mr;
	uint32_t  t_count;
	uint32_t  t_pause;
};

struct c687_timer_device_t 
{
	
	conf_object_t* obj;
	
	general_signal_intf* signal_iface;
	
	conf_object_t* signal;
	
	time_handle_t* handle;
	
	struct registers regs;
	
	uint32_t ir_num; 
	uint32_t time_cnt;
	int flag ;
	uint32_t int_number;
};

typedef struct c687_timer_device_t c687_timer_device;
static char* regs_name[] = 
{
	"ir",
	"mr",
	"t_count",
	"t_pause",
	NULL
};  
#define  IR         0x0
#define  MR         0x04
#define  T_COUNT    0x08
#define  T_PAUSE    0x0C

#endif  
