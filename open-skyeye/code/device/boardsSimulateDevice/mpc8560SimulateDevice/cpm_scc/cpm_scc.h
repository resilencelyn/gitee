#ifndef __SCC_UART_H__
#define __SCC_UART_H__
#include <stdint.h>
struct registers {
	uint32_t gsmr_l;
	uint32_t gsmr_h;
	uint32_t psmr;
	uint32_t todr;
	uint32_t dsr;
	uint32_t scce;
	uint32_t sccm;
	uint32_t sccs;
};

typedef struct uart_device {
	conf_object_t* obj;
	core_signal_intf* signal_iface;
	conf_object_t* signal;
	struct registers regs;
} cpm_scc_device;

static char* regs_name[] = {
	"gsmr_l",
	"gsmr_h",
	"psmr",
	"todr",
	"dsr",
	"scce",
	"sccm",
	"sccs",
	NULL
};
#define GSMR_L 0x0
#define GSMR_H 0x4
#define PSMR 0x8
#define TODR 0xc
#define DSR 0xe
#define SCCE 0x10
#define SCCM 0x14
#define SCCS 0x17
#endif
