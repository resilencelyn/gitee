#ifndef __AINTC_c674x_H__
#define __AINTC_c674x_H__
#include <stdint.h>

#define EVTFLAG			0x000
#define EVTSET			0x020
#define EVTCLR			0x040
#define EVTMASK			0x080
#define MEVTFLAG		0x0a0

#define EXPMASK			0x0c0
#define MEXPFLAG		0x0e0

#define INTMUX1			0x104
#define INTMUX2			0x108
#define INTMUX3			0x10c

#define AEGMUX0			0x140
#define AEGMUX1			0x144

#define INTXSTAT		0x180
#define INTXCLR			0x184
#define INTDMASK		0x188

typedef struct c674x_device {
	conf_object_t *obj;
	struct registers{
		uint32_t evtflag[4];
		uint32_t evtset[4];
		uint32_t evtclr[4];
		uint32_t evtmask[4];
		uint32_t mevtflag[4];
		uint32_t expmask[4];
		uint32_t mexpflag[4];

		uint32_t intmux1;
		uint32_t intmux2;
		uint32_t intmux3;

		uint32_t aegmux0;
		uint32_t aegmux1;

		uint32_t intxstat;
		uint32_t intxclr;
		uint32_t intdmask;
	}regs;
	core_signal_intf *signal_iface;
	conf_object_t *signal;
    int EVT_intselect[128];
}aintc_c674x_device;

static char* regs_name[] = {
	"evtflag0",
	"evtflag1",
	"evtflag2",
	"evtflag3",

	"evtset0",
	"evtset1",
	"evtset2",
	"evtset3",

	"evtclr0",
	"evtclr1",
	"evtclr2",
	"evtclr3",

	"evtmask0",
	"evtmask1",
	"evtmask2",
	"evtmask3",

	"mevtflag0",
	"mevtflag1",
	"mevtflag2",
	"mevtflag3",

	"expmask0",
	"expmask1",
	"expmask2",
	"expmask3",

	"mexpflag0",
	"mexpflag1",
	"mexpflag2",
	"mexpflag3",

	"intmux1",
	"intmux2",
	"intmux3",

	"aegmux0",
	"aegmux1",

	"intxstat",
	"intxclr",
	"intdmask",
	NULL,
};
#endif
