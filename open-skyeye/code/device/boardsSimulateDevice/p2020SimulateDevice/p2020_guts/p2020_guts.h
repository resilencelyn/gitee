#ifndef __P2020_GUTS_H__
#define __P2020_GUTS_H__

struct registers
{
	uint32_t porpllsr;
	uint32_t porbmsr;
	uint32_t pordevsr;
	uint32_t pordbgmsr;
	uint32_t pordevsr2;
	uint32_t gpporcr;
	uint32_t pmuxcr;
	uint32_t devdisr;
	uint32_t powmgtcsr;
	uint32_t pmcdr;
	uint32_t mcpsumr;
	uint32_t rstrscr;
	uint32_t ectrstcr;
	uint32_t autorstsr;
	uint32_t pvr;
	uint32_t svr;
	uint32_t rstcr;
	uint32_t iovselsr;
	uint32_t ddrclkdr;
	uint32_t clkocr;
	uint32_t ecmcr;
	uint32_t sdhcdcr;
	uint32_t srdscr0;
	uint32_t srdscr1;
	uint32_t srdscr2;
	uint32_t srdscr4;
	uint32_t srdscr5;
	uint32_t srdscr6;
};

struct p2020_device {
	conf_object_t* obj;
	struct registers regs;
	struct
	{
		core_exec_intf *iface;
		conf_object_t *obj;
	}core_iface[2];
};
typedef struct p2020_device guts_p2020_device;

#define PORPLLSR     0x0
#define PORBMSR      0x4
#define PORDEVSR     0xc
#define PORDBGMSR    0x10
#define PORDEVSR2    0x14
#define GPPORCR      0x20
#define PMUXCR       0x60
#define DEVDISR      0x70
#define POWMGTCSR    0x80
#define PMCDR        0x8c
#define MCPSUMR      0x90
#define RSTRSCR      0x94
#define ECTRSTCR     0x98
#define AUTORSTSR    0x9c
#define PVR          0xa0
#define SVR          0xa4
#define RSTCR        0xb0
#define IOVSELSR     0xc0
#define DDRCLKDR     0xb28
#define CLKOCR       0xe00
#define ECMCR        0xe20
#define SDHCDCR      0xf64
#define SRDSCR0      0x3000
#define SRDSCR1      0x3004
#define SRDSCR2      0x3008
#define SRDSCR4      0x3010
#define SRDSCR5      0x3014
#define SRDSCR6      0x3018

#define E500_CORE0_DISABLE  0x10000
#define E500_CORE1_DISABLE  0x40000
#define TB0_DISABLE  0x20000
#define TB1_DISABLE  0x80000

static char* regs_name[] =
{
	"porpllsr",
	"porbmsr",
	"pordevsr",
	"pordbgmsr",
	"pordevsr2",
	"gpporcr",
	"pmuxcr",
	"devdisr",
	"powmgtcsr",
	"pmcdr",
	"mcpsumr",
	"rstrscr",
	"ectrstcr",
	"autorstsr",
	"pvr",
	"svr",
	"rstcr",
	"iovselsr",
	"ddrclkdr",
	"clkocr",
	"ecmcr",
	"sdhcdcr",
	"srdscr0",
	"srdscr1",
	"srdscr2",
	"srdscr3",
	"srdscr4",
	"srdscr5",
	"srdscr6",
	NULL
};
#endif
