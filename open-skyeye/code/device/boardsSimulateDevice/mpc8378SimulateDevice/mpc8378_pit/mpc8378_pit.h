#ifndef __MPC8378_PIT_H__ 
#define __MPC8378_PIT_H__ 
struct registers 
{
	uint32_t ptcnr;
	uint32_t ptldr;
	uint32_t ptpsr;
	uint32_t ptctr;
	uint32_t ptevr;
};


struct mpc8378_pit_device_t 
{
	
	conf_object_t* obj;
	
	general_signal_intf* signal_iface;
	
	conf_object_t* signal;
	
	time_handle_t* handle;
	
	int scale;
	
	struct registers regs;
};

typedef struct mpc8378_pit_device_t mpc8378_pit_device;
static char* regs_name[] = 
{
	"ptcnr",
	"ptldr",
	"ptpsr",
	"ptctr",
	"ptevr",
	NULL
};
#define  PIT_PTCNR		0x00
#define  PIT_PTLDR		0x04
#define  PIT_PTPSR		0x08
#define  PIT_PTCTR		0x0c
#define  PIT_PTEVR		0x10


#define  IRQ_NUM      65


#endif  
