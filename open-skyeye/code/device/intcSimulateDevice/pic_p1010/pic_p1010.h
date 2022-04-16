#ifndef __PIC_P1010_H__
#define __PIC_P1010_H__
struct registers {
	uint32_t brr1;
	uint32_t brr2;
	uint32_t ipidr0;
	uint32_t ipidr1;
	uint32_t ipidr2;
	uint32_t ipidr3;
	uint32_t ctpr;
	uint32_t whoami;
	uint32_t iack;
	uint32_t eoi;
	uint32_t frr;
	uint32_t gcr;
	uint32_t svr;
};

struct p1010_device {
	conf_object_t* obj;
	core_signal_intf* signal_iface;
	conf_object_t* signal;
	struct registers regs;
	uint8_t iivpr_dr[1400];
};
typedef struct p1010_device pic_p1010_device;
static char* regs_name[] = {
	"brr1",
	"brr2",
	"ipidr0",
	"ipidr1",
	"ipidr2",
	"ipidr3",
	"ctpr",
	"whoami",
	"iack",
	"eoi",
	"frr",
	"gcr",
	"svr",
	NULL
};
#define BRR1 0x00
#define BRR2 0x10
#define IPIDR0 0x40
#define IPIDR1 0x50
#define IPIDR2 0x60
#define IPIDR3 0x70
#define CTPR 0x80
#define WHOAMI 0x90
#define IACK 0xa0
#define EOI 0xb0
#define FRR 0x1000
#define GCR 0x1020

#define SVR 0x10e0

#if 0
#define GTVPRA0 0x1120
#define GTDRA0 0x1130

#define GTVPRA1 0x1160
#define GTVPRA2 0x11a0
#define GTVPRA3 0x11e0
#endif

#endif
