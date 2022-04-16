#ifndef __rm48x_spi_H__
#define __rm48x_spi_H__
#include <skyeye_lock.h>
#include "rm48x_spi_fifo.h"
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
	uint32_t spipc6;
	uint32_t spipc7;
	uint32_t spipc8;
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
	uint32_t intvect0;
	uint32_t intvect1;
	uint32_t spipc9;
	uint32_t spipmctrl;
	uint32_t mibspie;
	uint32_t tgitenst;
	uint32_t tgitencr;
	uint32_t tgitlvst;
	uint32_t tgitlvcr;
	uint32_t tgintflg;
	uint32_t tickcnt;
	uint32_t ltgpend;
	uint32_t tgxctrl;
	uint32_t dmaxctrl;
	uint32_t icount;
	uint32_t dmacntlen;
	uint32_t uerrctrl;
	uint32_t uerrstat;
	uint32_t uerraddr1;
	uint32_t uerraddr0;
	uint32_t rxovrn_buf_addr;
	uint32_t iolpbktstcr;
	uint32_t extended_prescale1;
	uint32_t extended_prescale2;
};

typedef struct Multi_Buffer {
	uint32_t TxRAM[0x80];
	uint32_t RxRAM[0x80];
} Multi_Buffer_t;

struct rm48x_spi_device_t 
{
	conf_object_t* obj;

	conf_object_t* image;
	memory_space_intf* image_space;

	struct {
		conf_object_t *slave_obj;
		spi_device_intf *spi_dev_iface;
	} slaves[MAX_SLAVES];

	conf_object_t* signal;
	core_signal_intf* signal_iface;

	struct registers regs;
	int index;
	int interrupt_number;
	int MibSPI_RAM_BASE;
	Multi_Buffer_t MibSPI_RAM;
	RWLOCK_T lock;
	FIFO *spi_fifo;
	int t_flag;
};
typedef struct rm48x_spi_device_t rm48x_spi_device;

static char* regs_name[] =
{
	"SPIGCR0",
	"SPIGCR1",
	"SPIINT0",
	"SPILVL",
	"SPIFLG",
	"SPIPC0",
	"SPIPC1",
	"SPIPC2",
	"SPIPC3",
	"SPIPC4",
	"SPIPC5",
	"SPIPC6",
	"SPIPC7",
	"SPIPC8",
	"SPIDAT0",
	"SPIDAT1",
	"SPIBUF",
	"SPIEMU",
	"SPIDELAY",
	"SPIDEF",
	"SPIFMT0",
	"SPIFMT1",
	"SPIFMT2",
	"SPIFMT3",
	"INTVECT0",
	"INTVECT1",
	"SPIPC9",
	"SPIPMCTRL",
	"MIBSPIE",
	"TGITENST",
	"TGITENCR",
	"TGITLVST",
	"TGITLVCR",
	"TGINTFLG",
	"TICKCNT",
	"LTGPEND",
	"TGxCTRL",
	"DMAxCTRL",
	"ICOUNT",
	"DMACNTLEN",
	"UERRCTRL",
	"UERRSTAT",
	"UERRADDR1",
	"UERRADDR0",
	"RXOVRN_BUF_ADDR",
	"IOLPBKTSTCR",
	"EXTENDED_PRESCALE1",
	"EXTENDED_PRESCALE2",
	NULL
};

#define SPIGCR0            0x00
#define SPIGCR1            0x04
#define SPIINT0            0x08
#define SPILVL             0x0c
#define SPIFLG             0x10
#define SPIPC0             0x14
#define SPIPC1             0x18
#define SPIPC2             0x1c
#define SPIPC3             0x20
#define SPIPC4             0x24
#define SPIPC5             0x28
#define SPIPC6             0x2c
#define SPIPC7             0x30
#define SPIPC8             0x34
#define SPIDAT0            0x38
#define SPIDAT1            0x3c
#define SPIBUF             0x40
#define SPIEMU             0x44
#define SPIDELAY           0x48
#define SPIDEF             0x4c
#define SPIFMT0            0x50
#define SPIFMT1            0x54
#define SPIFMT2            0x58
#define SPIFMT3            0x5c
#define INTVECT0           0x60
#define INTVECT1           0x64
#define SPIPC9             0x68
#define SPIPMCTRL          0x6c
#define MIBSPIE            0x70
#define TGITENST           0x74
#define TGITENCR           0x78
#define TGITLVST           0x7c
#define TGITLVCR           0x80
#define TGINTFLG           0x84
#define TICKCNT            0x90
#define LTGPEND            0x94
#define TGXCTRL            0x98
#define DMAXCTRL           0xd8
#define ICOUNT             0xf8
#define DMACNTLEN          0x118
#define UERRCTRL           0x120
#define UERRSTAT           0x124
#define UERRADDR1          0x128
#define UERRADDR0          0x12c
#define RXOVRN_BUF_ADDR    0x130
#define IOLPBKTSTCR        0x134
#define EXTENDED_PRESCALE1 0x138
#define EXTENDED_PRESCALE2 0x13c

#endif  
