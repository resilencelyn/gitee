#ifndef __RM48X_TIMER_H__ 
#define __RM48X_TIMER_H__ 

struct registers 
{
	
	uint32_t rtigctrl;
	
	uint32_t rtitbctrl;
	
	uint32_t rticapctrl;
	
	uint32_t rticompctrl;
	
	uint32_t rtifrc0;
	
	uint32_t rtiuc0;
	
	uint32_t rticpuc0;
	
	uint32_t rticafrc0;
	
	uint32_t rticauc0;
	
	uint32_t rtifrc1;
	
	uint32_t rtiuc1;
	
	uint32_t rticpuc1;
	
	uint32_t rticafrc1;
	
	uint32_t rticauc1;
	
	uint32_t rticomp0;
	
	uint32_t rtiudcp0;
	
	uint32_t rticomp1;
	
	uint32_t rtiudcp1;
	
	uint32_t rticomp2;
	
	uint32_t rtiudcp2;
	
	uint32_t rticomp3;
	
	uint32_t rtiudcp3;
	
	uint32_t rtitblcomp;
	
	uint32_t rtitbhcomp;
	
	uint32_t rtisetintena;
	
	uint32_t rticlearintena;
	
	uint32_t rtiintflag;
	
	uint32_t rtidwdctrl;
	
	uint32_t rtidwdprld;
	
	uint32_t rtiwdstatus;
	
	uint32_t rtiwdkey;
	
	uint32_t rtidwdcntr;
	
	uint32_t rtiwwdrxnctrl;
	
	uint32_t rtiwwdsizectrl;
	
	uint32_t rtiintclrenable;
	
	uint32_t rticomp0clr;
	
	uint32_t rticomp1clr;
	
	uint32_t rticomp2clr;
	
	uint32_t rticomp3clr;
	
};


struct rm48x_timer_device_t 
{
	
	conf_object_t* obj;
	
	conf_object_t* core;
	core_signal_intf* signal_iface;
	conf_object_t* signal;;
	
	struct registers regs;
	
	int32_t sched_id0;
	int32_t sched_id1;
	
	int32_t frc0;
	int32_t frc1;
	
	time_handle_t* handle;
	uint32_t old_wdkey;
	uint32_t test_value;

};
typedef struct rm48x_timer_device_t rm48x_timer_device;



#define CLOCK_NTU 	0
#define CLOCK_RTIUC0	0


#define RTIINTFLAG_OVL1INT		1<<18
#define RTIINTFLAG_OVEL0INT		1<<17
#define RTIINTFLAG_TBINT		1<<16
#define RTIINTFLAG_INT3		 	1<<3
#define RTIINTFLAG_INT2			1<<2
#define RTIINTFLAG_INT1			1<<1
#define RTIINTFLAG_INT0			1<<0


#define RTIDWDCTRL_ENABLE 	0xad9559da


#define RTIGCTRL_COS_MASK	1<<15
#define RTIGCTRL_COS_RUN	1<<15
#define RTIGCTRL_CNT1EN		1<<1
#define RTIGCTRL_CNT0EN		1<<0	


#define RTITBCTRL_MASK		1<<1
#define RTITBCTRL_INC		1<<1
#define RTITBCTRL_TBEXT		1<<0



#define RTIWDSTATUS_DWWD_ST			1<<5
#define RTIWDSTATUS_END_TIME_VIOL	1<<4
#define RTIWDSTATUS_START_TIME_VIOL	1<<3	
#define RTIWDSTATUS_KEY_ST			1<<2
#define RTIWDSTATUS_DWD_ST			1<<1


#define COMPSEL3 	1<<12
#define COMPSEL2	1<<8
#define COMPSEL1	1<<4
#define COMPSEL0	1<<0


static char* regs_name[] = 
{
	"rtigctrl",
	"rtitbctrl",
	"rticapctrl",
	"rticompctrl",
	"rtifrc0",
	"rtiuc0",
	"rticpuc0",
	"rticafrc0",
	"rticauc0",
	"rtifrc1",
	"rtiuc1",
	"rticpuc1",
	"rticafrc1",
	"rticauc1",
	"rticomp0",
	"rtiudcp0",
	"rticomp1",
	"rtiudcp1",
	"rticomp2",
	"rtiudcp2",
	"rticomp3",
	"rtiudcp3",
	"rtitblcomp",
	"rtitbhcomp",
	"rtisetintena",
	"rticlearintena",
	"rtiintflag",
	"rtidwdctrl",
	"rtidwdprld",
	"rtiwdstatus",
	"rtiwdkey",
	"rtidwdcntr",
	"rtiwwdrxnctrl",
	"rtiwwdsizectrl",
	"rtiintclrenable",
	"rticomp0clr",
	"rticomp1clr",
	"rticomp2clr",
	"rticomp3clr",
	NULL
};
#define RTIGCTRL 0x0
#define RTITBCTRL 0x4
#define RTICAPCTRL 0x8
#define RTICOMPCTRL 0xc
#define RTIFRC0 0x10
#define RTIUC0 0x14
#define RTICPUC0 0x18
#define RTICAFRC0 0x20
#define RTICAUC0 0x24
#define RTIFRC1 0x30
#define RTIUC1 0x34
#define RTICPUC1 0x38
#define RTICAFRC1 0x40
#define RTICAUC1 0x44
#define RTICOMP0 0x50
#define RTIUDCP0 0x54
#define RTICOMP1 0x58
#define RTIUDCP1 0x5c
#define RTICOMP2 0x60
#define RTIUDCP2 0x64
#define RTICOMP3 0x68
#define RTIUDCP3 0x6c
#define RTITBLCOMP 0x70
#define RTITBHCOMP 0x74
#define RTISETINTENA 0x80
#define RTICLEARINTENA 0x84
#define RTIINTFLAG 0x88
#define RTIDWDCTRL 0x90
#define RTIDWDPRLD 0x94
#define RTIWDSTATUS 0x98
#define RTIWDKEY 0x9c
#define RTIDWDCNTR 0xa0
#define RTIWWDRXNCTRL 0xa4
#define RTIWWDSIZECTRL 0xa8
#define RTIINTCLRENABLE 0xac
#define RTICOMP0CLR 0xb0
#define RTICOMP1CLR 0xb4
#define RTICOMP2CLR 0xb8
#define RTICOMP3CLR 0xbc
#endif  
