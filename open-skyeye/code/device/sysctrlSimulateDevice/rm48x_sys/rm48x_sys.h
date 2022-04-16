#ifndef __rm48x_sys_H__
#define __rm48x_sys_H__

struct registers
{
	uint32_t syspc1;
	uint32_t syspc2;
	uint32_t syspc3;
	uint32_t syspc4;
	uint32_t syspc5;
	uint32_t syspc6;
	uint32_t syspc7;
	uint32_t syspc8;
	uint32_t syspc9;
	uint32_t csdis;
	uint32_t csdisset;
	uint32_t csdisclr;
	uint32_t cddis;
	uint32_t cddisset;
	uint32_t cddisclr;
	uint32_t ghvsrc;
	uint32_t vclkasrc;
	uint32_t rclksrc;
	uint32_t csvstat;
	uint32_t mstgcr;
	uint32_t minitgcr;
	uint32_t msinena;
	uint32_t mstcgstat;
	uint32_t ministat;
	uint32_t pllctl1;
	uint32_t pllctl2;
	uint32_t syspc10;
	uint32_t dieidl;
	uint32_t dieidh;
	uint32_t lpomonctl;
	uint32_t clktest;
	uint32_t dftctrlreg;
	uint32_t dftctrlreg2;
	uint32_t gpreg1;
	uint32_t impfasts;
	uint32_t impftadd;
	uint32_t ssir1;
	uint32_t ssir2;
	uint32_t ssir3;
	uint32_t ssir4;
	uint32_t ramgcr;
	uint32_t bmmcr1;
	uint32_t reserve;
	uint32_t cpurstcr;
	uint32_t clkcntl;
	uint32_t ecpcntl;
	uint32_t devcr1;
	uint32_t sysecr;
	uint32_t sysesr;
	uint32_t systasr;
	uint32_t glbstat;
	uint32_t devid;
	uint32_t ssivec;
	uint32_t ssif;
};


struct rm48x_sys_device_t 
{
	conf_object_t* obj;

	struct registers regs;
};
typedef struct rm48x_sys_device_t rm48x_sys_device;

static char* regs_name[] =
{
	"SYSPC1",
	"SYSPC2",
	"SYSPC3",
	"SYSPC4",
	"SYSPC5",
	"SYSPC6",
	"SYSPC7",
	"SYSPC8",
	"SYSPC9",
	"CSDIS",
	"CSDISSET",
	"CSDISCLR",
	"CDDIS",
	"CDDISSET",
	"CDDISCLR",
	"GHVSRC",
	"VCLKASRC",
	"RCLKSRC",
	"CSVSTAT",
	"MSTGCR",
	"MINITGCR",
	"MSINENA",
	"MSTCGSTAT",
	"MINISTAT",
	"PLLCTL1",
	"PLLCTL2",
	"SYSPC10",
	"DIEIDL",
	"DIEIDH",
	"LPOMONCTL",
	"CLKTEST",
	"DFTCTRLREG",
	"DFTCTRLREG2",
	"GPREG1",
	"IMPFASTS",
	"IMPFTADD",
	"SSIR1",
	"SSIR2",
	"SSIR3",
	"SSIR4",
	"RAMGCR",
	"BMMCR1",
	"RESERVE"
	"CPURSTCR",
	"CLKCNTL",
	"ECPCNTL",
	"DEVCR1",
	"SYSECR",
	"SYSESR",
	"SYSTASR",
	"GLBSTAT",
	"DEVID",
	"SSIVEC",
	"SSIF",
	NULL
};

#define SYSPC1      0x00
#define SYSPC2      0x04
#define SYSPC3      0x08
#define SYSPC4      0x0c
#define SYSPC5      0x10
#define SYSPC6      0x14
#define SYSPC7      0x18
#define SYSPC8      0x1c
#define SYSPC9      0x20
#define CSDIS       0x30
#define CSDISSET    0x34
#define CSDISCLR    0x38
#define CDDIS       0x3c
#define CDDISSET    0x40
#define CDDISCLR    0x44
#define GHVSRC      0x48
#define VCLKASRC    0x4c
#define RCLKSRC     0x50
#define CSVSTAT     0x54
#define MSTGCR      0x58
#define MINITGCR    0x5c
#define MSINENA     0x60
#define MSTCGSTAT   0x68
#define MINISTAT    0x6c
#define PLLCTL1     0x70
#define PLLCTL2     0x74
#define SYSPC10     0x78
#define DIEIDL      0x7c
#define DIEIDH      0x80
#define LPOMONCTL   0x88
#define CLKTEST     0x8c
#define DFTCTRLREG  0x90
#define DFTCTRLREG2 0x94
#define GPREG1      0xa0
#define IMPFASTS    0xa8
#define IMPFTADD    0xac
#define SSIR1       0xb0
#define SSIR2       0xb4
#define SSIR3       0xb8
#define SSIR4       0xbc
#define RAMGCR      0xc0
#define BMMCR1      0xc4
#define RESERVE     0xc8
#define CPURSTCR    0xcc
#define CLKCNTL     0xd0
#define ECPCNTL     0xd4
#define DEVCR1      0xdc
#define SYSECR      0xe0
#define SYSESR      0xe4
#define SYSTASR     0xe8
#define GLBSTAT     0xec
#define DEVID       0xf0
#define SSIVEC      0xf4
#define SSIF        0xf8
#endif
