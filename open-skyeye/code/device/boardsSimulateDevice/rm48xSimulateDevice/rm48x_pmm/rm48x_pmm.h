#ifndef __RM48X_PMM_H__
#define __RM48X_PMM_H__

struct registers
{
	uint32_t logicpdpwrctrl0;
	uint32_t reserved0[3];
	uint32_t mempdpwrctrl0;
	uint32_t reserved1[3];
	uint32_t pdclkdisreg;
	uint32_t pdclkdissetreg;
	uint32_t pdclkdisclrreg;
	uint32_t reserved2[4];
	uint32_t logicpdpwrstat0;
	uint32_t logicpdpwrstat1;
	uint32_t logicpdpwrstat2;
	uint32_t logicpdpwrstat3;
	uint32_t reserved3[12];
	uint32_t mempdpwrstat0;
	uint32_t mempdpwrstat1;
	uint32_t mempdpwrstat2;
	uint32_t reserved4[5];
	uint32_t globalctrl1;
	uint32_t globalstat;
	uint32_t prckeyreg;
	uint32_t lpddcstat1;
	uint32_t lpddcstat2;
	uint32_t mpddcstat1;
	uint32_t mpddcstat2;
	uint32_t isodiagstat;
};

struct rm48x_pmm_device_t
{
	conf_object_t* obj;
	core_signal_intf* signal_iface;
	conf_object_t* signal;
	struct registers regs;
};
typedef struct rm48x_pmm_device_t rm48x_pmm_device;

static char* regs_name[] =
{
	"LOGICPDPWRCTRL0",
	"Reserved00",
	"Reserved01",
	"Reserved02",
	"MEMPDPWRCTRL0",
	"Reserved10",
	"Reserved11",
	"Reserved12",
	"PDCLKDISREG",
	"PDCLKDISSETREG",
	"PDCLKDISCLRREG",
	"Reserved20",
	"Reserved21",
	"Reserved22",
	"Reserved23",
	"LOGICPDPWRSTAT0",
	"LOGICPDPWRSTAT1",
	"LOGICPDPWRSTAT2",
	"LOGICPDPWRSTAT3",
	"Reserved30",
	"Reserved31",
	"Reserved32",
	"Reserved33",
	"Reserved34",
	"Reserved35",
	"Reserved36",
	"Reserved37",
	"Reserved38",
	"Reserved39",
	"Reserved310",
	"Reserved311",
	"MEMPDPWRSTAT0",
	"MEMPDPWRSTAT1",
	"MEMPDPWRSTAT2",
	"Reserved40",
	"Reserved41",
	"Reserved42",
	"Reserved43",
	"Reserved44",
	"GLOBALCTRL1",
	"GLOBALSTAT",
	"PRCKEYREG",
	"LPDDCSTAT1",
	"LPDDCSTAT2",
	"MPDDCSTAT1",
	"MPDDCSTAT2",
	"ISODIAGSTAT",
	NULL
};

#define LOGICPDPWRCTRL0 0x00
#define RESERVED0       0x04
#define MEMPDPWRCTRL0   0x10
#define RESERVED1       0x14
#define PDCLKDISREG     0x20
#define PDCLKDISSETREG  0x24
#define PDCLKDISCLRREG  0x28
#define RESERVED2       0x30
#define LOGICPDPWRSTAT0 0x40
#define LOGICPDPWRSTAT1 0x44
#define LOGICPDPWRSTAT2 0x48
#define LOGICPDPWRSTAT3 0x4c
#define RESERVED3       0x50
#define MEMPDPWRSTAT0   0x80
#define MEMPDPWRSTAT1   0x84
#define MEMPDPWRSTAT2   0x88
#define RESERVED4       0x8c
#define GLOBALCTRL1     0xa0
#define GLOBALSTAT      0xa8
#define PRCKEYREG       0xac
#define LPDDCSTAT1      0xb0
#define LPDDCSTAT2      0xb4
#define MPDDCSTAT1      0xb8
#define MPDDCSTAT2      0xbc
#define ISODIAGSTAT     0xc0

#endif
