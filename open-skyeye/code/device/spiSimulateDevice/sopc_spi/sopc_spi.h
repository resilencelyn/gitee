#ifndef __SOPC_SPI_H__
#define __SOPC_SPI_H__

#include <skyeye_lock.h>
struct registers
{
	uint32_t ctrlr0;     //control register 0
	uint32_t ctrlr1;     //control register 1
	uint32_t spienr;     //spi enable register
	uint32_t spi_switch; //spi switch register
	uint32_t ser;        //slave allow register
	uint32_t baudr;      //baud register
	uint32_t txftlr;     //tx FIFO max number register
	uint32_t rxftlr;     //rx FIFO max number register
	uint32_t txflr;     //tx FIFO level register
	uint32_t rxflr;     //rx FIFO level register
	uint32_t sr;         //status register
	uint32_t imr;        //int mask register
	uint32_t isr;        //int status register
	uint32_t risr;       //Raw int status register
	uint32_t txoicr;     //clear send FIFO intr register
	uint32_t rxoicr;     //clear receive FIFO overFlow intr register
	uint32_t rxuicr;     //clear receive FIFO underFlow int register
	uint32_t icr;        //clear interrupt register
	uint32_t dmacr;      //DMA ctrl register
	uint32_t dmatdlr;    //DMA translate max number register
	uint32_t dmardlr;    //DMA receive max number register
	uint32_t dr[16];         //data register
};

#define MAX_SLAVES 8
struct sopc_spi_device_t
{
	conf_object_t* obj;
	struct {
		int                     num;
		conf_object_t          *obj;
		general_signal_intf    *intf;
	}signal;
	struct {
		conf_object_t *slave_obj;
		spi_device_intf *spi_dev_iface;
	} slaves[MAX_SLAVES];
	FIFO *spi_fifo;
	RWLOCK_T lock;
	struct registers regs;
	int slave_index;
	int recv_num;
};

typedef struct sopc_spi_device_t sopc_spi_device;

/*SPI0 Registers Base--0x68900000 Offset*/
/*SPI1 Registers Base--0x6a040000 Offset*/
#define CTRLR0        0x00
#define CTRLR1        0x04
#define SPIENR        0x08
#define SWITCH        0x0c
#define SER           0x10
#define BAUDR         0x14
#define TXFTLR        0x18

#define RXFTLR        0x1c
#define TXFLR         0x20
#define RXFLR         0x24
#define SR            0x28
#define IMR           0x2c
#define ISR           0x30
#define RISR          0x34
#define TXOICR        0x38
#define RXOICR        0x3c
#define RXUICR        0x40
#define ICR           0x48
#define DMACR         0x4c
#define DMATDLR       0x50
#define DMARDLR       0x54
#define DR            0x60
#define DR_END        0x15c

static char* regs_name[] =
{
	"ctrlr0",
	"ctrlr1",
	"spienr",
	"spi_switch",
	"ser",
	"baudr",
	"txftlr",
	"rxftlr",
	"txflr",
	"rxflr",
	"sr",
	"imr",
	"isr",
	"risr",
	"txoicr",
	"rxoicr",
	"rxuicr",
	"icr",
	"dmacr",
	"dmatdlr",
	"dmardlr",
	"dr",
	NULL
};

static int regs_offset[] =
{
	CTRLR0,
	CTRLR1,
	SPIENR,
	SWITCH,
	SER,
	BAUDR,
	TXFTLR,
	RXFTLR,
	TXFLR,
	RXFLR,
	SR,
	IMR,
	ISR,
	RISR,
	TXOICR,
	RXOICR,
	RXUICR,
	ICR,
	DMACR,
	DMATDLR,
	DMARDLR,
	DR
};

#define RXFIM (1 << 4)
#define RXOIM (1 << 3)
#define RXUIM (1 << 2)
#define TXOIM (1 << 1)
#define TXUIM (1 << 0)


#define RXFIS (1 << 4)
#define RXOIS (1 << 3)
#define RXUIS (1 << 2)
#define TXOIS (1 << 1)
#define TXEIS (1 << 0)

#define SR_BUSY (1 << 0)
#define SR_TFNF (1 << 1)
#define SR_TFE  (1 << 2)
#define SR_RFNE (1 << 3)
#define SR_RFF  (1 << 4)
#endif
