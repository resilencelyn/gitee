#ifndef __DCFG_T2080_H__
#define __DCFG_T2080_H__

struct registers {
	uint32_t porsr1;
        uint32_t pvr;//Processor Version Register
        uint32_t svr;//System Version Register
	uint32_t rcwsr[16];
};

typedef struct dcfg_device {
	conf_object_t* obj;
	struct registers regs;
}dcfg_t2080_device;

static char* regs_name[] = {
	"porsr1"
	"pvr",
	"svr",
	"rcwsr0",
	"rcwsr1",
	"rcwsr2",
	"rcwsr3",
	"rcwsr4",
	"rcwsr5",
	"rcwsr6",
	"rcwsr7",
	"rcwsr8",
	"rcwsr9",
	"rcwsr10",
	"rcwsr11",
	"rcwsr12",
	"rcwsr13",
	"rcwsr14",
	"rcwsr15",
	NULL
};
#define PORSR1	   0x0
#define PVR       0xA0
#define SVR       0xA4
#define RCWSR0     0x100
#define RCWSR1     0x104
#define RCWSR2     0x108
#define RCWSR3     0x10c
#define RCWSR4     0x110
#define RCWSR5     0x114
#define RCWSR6     0x118
#define RCWSR7     0x11c
#define RCWSR8     0x120
#define RCWSR9     0x124
#define RCWSR10    0x128
#define RCWSR11    0x12c
#define RCWSR12    0x130
#define RCWSR13    0x134
#define RCWSR14    0x138
#define RCWSR15    0x13c

#endif
