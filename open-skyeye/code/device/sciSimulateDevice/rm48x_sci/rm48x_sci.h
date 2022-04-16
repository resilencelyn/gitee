#ifndef __rm48x_sci_H__
#define __rm48x_sci_H__
#include <skyeye_system.h>
#include "skyeye_lock.h"
#include "rm48x_sci_fifo.h"

struct registers
{
	uint32_t scigcr0;
	uint32_t scigcr1;
	uint32_t scigcr2;
	uint32_t scisetint;
	uint32_t sciclearint;
	uint32_t scisetintlvl;
	uint32_t sciclearintlvl;
	uint32_t sciflr;
	uint32_t sciintvect0;
	uint32_t sciintvect1;
	uint32_t sciformat;
	uint32_t brs;
	uint32_t scied;
	uint32_t scird;
	uint32_t scitd;
	uint32_t scipio0;
	uint32_t scipio1;
	uint32_t scipio2;
	uint32_t scipio3;
	uint32_t scipio4;
	uint32_t scipio5;
	uint32_t scipio6;
	uint32_t scipio7;
	uint32_t scipio8;
	uint32_t lincompare;
	uint32_t linrd0;
	uint32_t linrd1;
	uint32_t linmask;
	uint32_t linid;
	uint32_t lintd0;
	uint32_t lintd1;
	uint32_t mbrs;
	uint32_t iodftctrl;
};

struct rm48x_sci_device_t
{
	conf_object_t* obj;
	conf_object_t* core;

	general_signal_intf* signal_iface;
	conf_object_t* signal;

	conf_object_t* dmc_linker;
	dmc_linker_intf *dmc_linker_iface;

	conf_object_t* bus_obj;
	Rs232_bus_intf *bus_iface;

	struct registers regs;
	time_handle_t *handle;
	int int_number;
	int Tx_int_number;
	int Rx_int_number;
	int t_flag;
	uint8_t t_data[8];
	unsigned char SCITXSHF[8];
	unsigned char SCIRXSHF[8];
	uint8_t TX_CNT;
	uint8_t RX_CNT;
	FIFO* recv_fifo;
	RWLOCK_T lock;
};
typedef struct rm48x_sci_device_t rm48x_sci_device;

static char* regs_name[] =
{
	"SCIGCR0",
	"SCIGCR1",
	"SCIGCR2",
	"SCISETINT",
	"SCICLEARINT",
	"SCISETINTLVL",
	"SCICLEARINTLVL",
	"SCIFLR",
	"SCIINTVECT0",
	"SCIINTVECT1",
	"SCIFORMAT",
	"BRS",
	"SCIED",
	"SCIRD",
	"SCITD",
	"SCIPIO0",
	"SCIPIO1",
	"SCIPIO2",
	"SCIPIO3",
	"SCIPIO4",
	"SCIPIO5",
	"SCIPIO6",
	"SCIPIO7",
	"SCIPIO8",
	"LINCOMPARE",
	"LINRD0",
	"LINRD1",
	"LINMASK",
	"LINID",
	"LINTD0",
	"LINTD1",
	"MBRS",
	"IODFTCTRL",
	NULL
};

#define SCIGCR0        0x00
#define SCIGCR1        0x04
#define SCIGCR2        0x08
#define SCISETINT      0x0c
#define SCICLEARINT    0x10
#define SCISETINTLVL   0x14
#define SCICLEARINTLVL 0x18
#define SCIFLR         0x1c
#define SCIINTVECT0    0x20
#define SCIINTVECT1    0x24
#define SCIFORMAT      0x28
#define BRS            0x2c
#define SCIED          0x30
#define SCIRD          0x34
#define SCITD          0x38
#define SCIPIO0        0x3c
#define SCIPIO1        0x40
#define SCIPIO2        0x44
#define SCIPIO3        0x48
#define SCIPIO4        0x4c
#define SCIPIO5        0x50
#define SCIPIO6        0x54
#define SCIPIO7        0x58
#define SCIPIO8        0x5c
#define LINCOMPARE     0x60
#define LINRD0         0x64
#define LINRD1         0x68
#define LINMASK        0x6c
#define LINID          0x70
#define LINTD0         0x74
#define LINTD1         0x78
#define MBRS           0x7c
#define IODFTCTRL      0x90

#define LOOPBK_MODE   1
#define NOLOOPBK_MODE 0
#define CHAR_LENGTH ((dev->regs.sciformat & 0x7) + 1)

#endif
