#ifndef __SOPC_WDT_H__
#define __SOPC_WDT_H__

struct registers
{
	uint32_t wdt_cr;     //control register
	uint32_t wdt_torr;   //time out range register
	uint32_t wdt_ccvr;   //current count value register
	uint32_t wdt_crr;    //count reset register
	uint32_t wdt_stat;   //interrupt status register
	uint32_t wdt_eoi;    //interrupt clear register
	uint32_t wdt_cp5;    //parameter5 register
	uint32_t wdt_cp4;    //parameter4 register
	uint32_t wdt_cp3;    //parameter3 register
	uint32_t wdt_cp2;    //parameter2 register
	uint32_t wdt_cp1;    //parameter1 register
	uint32_t wdt_ver;    //version register
	uint32_t wdt_type;   //wdt type register
};

struct sopc_wdt_device_t
{
	conf_object_t* obj;
	struct {
		int                     num;
		conf_object_t          *obj;
		general_signal_intf    *intf;
	}signal;
	struct registers regs;
};

typedef struct sopc_wdt_device_t sopc_wdt_device;

/*WDT Registers Base--0x68B00000 Offset*/
#define WDT_CR        0x00
#define WDT_TORR      0x04
#define WDT_CCVR      0x08
#define WDT_CRR       0x0c
#define WDT_STAT      0x10
#define WDT_EOI       0x14

#define WDT_CP5       0xe4
#define WDT_CP4       0xe8
#define WDT_CP3       0xec
#define WDT_CP2       0xf0
#define WDT_CP1       0xf4
#define WDT_VER       0xf8
#define WDT_TYPE      0xfc

static char* regs_name[] =
{
	"wdt_cr",
	"wdt_torr",
	"wdt_ccvr",
	"wdt_crr",
	"wdt_stat",
	"wdt_eoi",
	"wdt_cp5",
	"wdt_cp4",
	"wdt_cp3",
	"wdt_cp2",
	"wdt_cp1",
	"wdt_ver",
	"wdt_type",
	NULL
};

static int regs_offset[] =
{
	WDT_CR,
	WDT_TORR,
	WDT_CCVR,
	WDT_CRR,
	WDT_STAT,
	WDT_EOI,
	WDT_CP5,
	WDT_CP4,
	WDT_CP3,
	WDT_CP2,
	WDT_CP1,
	WDT_VER,
	WDT_TYPE,
};

#endif
