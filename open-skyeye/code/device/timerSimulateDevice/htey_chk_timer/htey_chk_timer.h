#ifndef __HTEY_CHK_TIMER_H__ 
#define __HTEY_CHK_TIMER_H__ 
struct registers 
{
	
	uint32_t chktimcountbyte0;
	
	uint32_t chktimcountbyte1;
	
	uint32_t chktimcountbyte2;
	
	uint32_t chktimcountbyte3;
	
	uint32_t gndchktimen;
	
	uint32_t gpschktimen;
	
	uint32_t chktimcountrdflag;
	
	uint32_t chktimcountclrflag;
	
};


struct chk_timer_device 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;
	
	struct registers regs;
	
};
typedef struct chk_timer_device htey_chk_timer_device;
static char* regs_name[] = 
{
	"chktimcountbyte0",
	"chktimcountbyte1",
	"chktimcountbyte2",
	"chktimcountbyte3",
	"gndchktimen",
	"gpschktimen",
	"chktimcountrdflag",
	"chktimcountclrflag",
	NULL
};
#define CHKTIMCOUNTBYTE0 0x0
#define CHKTIMCOUNTBYTE1 0x10
#define CHKTIMCOUNTBYTE2 0x20
#define CHKTIMCOUNTBYTE3 0x30
#define GNDCHKTIMEN 0x40
#define GPSCHKTIMEN 0x50
#define CHKTIMCOUNTRDFLAG 0x60
#define CHKTIMCOUNTCLRFLAG 0x70
#endif  
