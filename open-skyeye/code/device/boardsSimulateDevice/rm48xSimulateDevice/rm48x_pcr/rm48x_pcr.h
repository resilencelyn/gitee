#ifndef __RM48X_PCR_H__ 
#define __RM48X_PCR_H__ 
struct registers 
{
	
	uint32_t pmprotset0;
	
	uint32_t pmprotset1;
	
	uint32_t pmprotclr0;
	
	uint32_t pmprotclr1;
	
	uint32_t pprotset0;
	
	uint32_t pprotset1;
	
	uint32_t pprotset2;
	
	uint32_t pprotset3;
	
	uint32_t pprotclr0;
	
	uint32_t pprotclr1;
	
	uint32_t pprotclr2;
	
	uint32_t pprotclr3;
	
	uint32_t pcspwrdwnset0;
	
	uint32_t pcspwrdwnset1;
	
	uint32_t pcspwrdwnclr0;
	
	uint32_t pcspwrdwnclr1;
	
	uint32_t pspwrdwnset0;
	
	uint32_t pspwrdwnset1;
	
	uint32_t pspwrdwnset2;
	
	uint32_t pspwrdwnset3;
	
	uint32_t pspwrdwnclr0;
	
	uint32_t pspwrdwnclr1;
	
	uint32_t pspwrdwnclr2;
	
	uint32_t pspwrdwnclr3;
	
};


struct rm48x_pcr_device_t 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;
	
	struct registers regs;
	
};
typedef struct rm48x_pcr_device_t rm48x_pcr_device;
static char* regs_name[] = 
{
	"PMPROTSET0",
	"PMPROTSET1",
	"PMPROTCLR0",
	"PMPROTCLR1",
	"PPROTSET0",
	"PPROTSET1",
	"PPROTSET2",
	"PPROTSET3",
	"PPROTCLR0",
	"PPROTCLR1",
	"PPROTCLR2",
	"PPROTCLR3",
	"PCSPWRDWNSET0",
	"PCSPWRDWNSET1",
	"PCSPWRDWNCLR0",
	"PCSPWRDWNCLR1",
	"PSPWRDWNSET0",
	"PSPWRDWNSET1",
	"PSPWRDWNSET2",
	"PSPWRDWNSET3",
	"PSPWRDWNCLR0",
	"PSPWRDWNCLR1",
	"PSPWRDWNCLR2",
	"PSPWRDWNCLR3",
	NULL
};
#define PMPROTSET0 0x00
#define PMPROTSET1 0x04
#define PMPROTCLR0 0x10
#define PMPROTCLR1 0x14
#define PPROTSET0 0x20
#define PPROTSET1 0x24
#define PPROTSET2 0x28
#define PPROTSET3 0x2c
#define PPROTCLR0 0x40
#define PPROTCLR1 0x44
#define PPROTCLR2 0x48
#define PPROTCLR3 0x4c
#define PCSPWRDWNSET0 0x60
#define PCSPWRDWNSET1 0x64
#define PCSPWRDWNCLR0 0x70
#define PCSPWRDWNCLR1 0x74
#define PSPWRDWNSET0 0x80
#define PSPWRDWNSET1 0x84
#define PSPWRDWNSET2 0x88
#define PSPWRDWNSET3 0x8c
#define PSPWRDWNCLR0 0xa0
#define PSPWRDWNCLR1 0xa4
#define PSPWRDWNCLR2 0xa8
#define PSPWRDWNCLR3 0xac
#endif  
