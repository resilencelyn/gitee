#ifndef __SPI_OMAPL138_H__
#define __SPI_OMAPL138_H__
#include <stdint.h>
#include <skyeye_lock.h>
#include "omapl138_spi_fifo.h"

#define MAX_SLAVES 8

struct registers
{
	uint32_t spigcr0;
	uint32_t spigcr1;
	uint32_t spiint0;
	uint32_t spilvl;
	uint32_t spiflg;
	uint32_t spipc0;
	uint32_t spipc1;
	uint32_t spipc2;
	uint32_t spipc3;
	uint32_t spipc4;
	uint32_t spipc5;
	uint32_t spidat0;
	uint32_t spidat1;
	uint32_t spibuf;
	uint32_t spiemu;
	uint32_t spidelay;
	uint32_t spidef;
	uint32_t spifmt0;
	uint32_t spifmt1;
	uint32_t spifmt2;
	uint32_t spifmt3;
	uint32_t intvec1;
};

struct omapl138_device
{
	conf_object_t* obj;
	struct {
		conf_object_t *slave_obj;
		spi_device_intf *spi_dev_iface;
	} slaves[MAX_SLAVES];
	conf_object_t *signal;
	general_signal_intf *signal_iface;
	struct registers regs;
	int interrupt_number;
	int index;
	FIFO *spi_fifo;
	RWLOCK_T lock;
};

typedef struct omapl138_device spi_omapl138_device;

static char* regs_name[] =
{
	"spigcr0",
	"spigcr1",
	"spiint0",
	"spilvl",
	"spiflg",
	"spipc0",
	"spipc1",
	"spipc2",
	"spipc3",
	"spipc4",
	"spipc5",
	"spidat0",
	"spidat1",
	"spibuf",
	"spiemu",
	"spidelay",
	"spidef",
	"spifmt0",
	"spifmt1",
	"spifmt2",
	"spifmt3",
	"intvec1",
	NULL
};

#define SPIGCR0 0x000
#define SPIGCR1 0x004
#define SPIINT0 0x008
#define SPILVL 0x00c
#define SPIFLG 0x010
#define SPIPC0 0x014
#define SPIPC1 0x018
#define SPIPC2 0x01c
#define SPIPC3 0x020
#define SPIPC4 0x024
#define SPIPC5 0x028
#define SPIDAT0 0x038
#define SPIDAT1 0x03c
#define SPIBUF 0x040
#define SPIEMU 0x044
#define SPIDELAY 0x048
#define SPIDEF 0x04c
#define SPIFMT0 0x050
#define SPIFMT1 0x054
#define SPIFMT2 0x058
#define SPIFMT3 0x05c
#define INTVEC1 0x064
#endif
