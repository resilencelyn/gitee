#ifndef __HTEY_HARD_TIMER_H__ 
#define __HTEY_HARD_TIMER_H__ 
struct registers 
{
	
	uint32_t timer_1s;
	
	uint32_t timer_2r5;
	
	uint32_t timer_1s_amount;
	
	uint32_t timer_1s1;
	
	uint32_t timer_1s1_amoun;
	
	uint32_t timer_2r5_lock;
	
	uint32_t timer_2r5_byte0;
	
	uint32_t timer_2r5_byte1;
	
	uint32_t timer_2r5_byte2;
	
};


struct hard_timer_device 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;

	conf_object_t *kgl;

	htey_kgl_intf *kgl_iface;
	
	struct registers regs;

	uint32_t irq_num;
	
};
typedef struct hard_timer_device htey_hard_timer_device;
static char* regs_name[] = 
{
	"timer_1s",
	"timer_2r5",
	"timer_1s_amount",
	"timer_1s1",
	"timer_1s1_amoun",
	"timer_2r5_lock",
	"timer_2r5_byte0",
	"timer_2r5_byte1",
	"timer_2r5_byte2",
	NULL
};
#define TIMER_1S 0x0
#define TIMER_2R5 0x10
#define TIMER_1S_AMOUNT 0x20
#define TIMER_1S1 0x30
#define TIMER_1S1_AMOUN 0x40
#define TIMER_2R5_LOCK 0x50
#define TIMER_2R5_BYTE0 0x60
#define TIMER_2R5_BYTE1 0x70
#define TIMER_2R5_BYTE2 0x80
#endif  
