#ifndef __LOONGSON_SPI_H__
#define __LOONGSON_SPI_H__

// All equipment register names, the list order and struct registers registers inside order is kept consistent
static char* regs_name[] =
{
	"spcr",
	"spsr",
	"data",
	"sper",
	"param",
	"softcs",
	"timing",
	NULL
};

// Register offset
#define SPCR     0x0
#define SPSR     0x1
#define DATA     0x2
#define SPER     0x3
#define PARAM    0x4
#define SOFTCS   0x5
#define TIMING   0x6

// device register structure
struct registers
{
	uint8_t spcr;
	uint8_t spsr;
	uint8_t data;
	uint8_t sper;
	uint8_t param;
	uint8_t softcs;
	uint8_t timing;
};

#define MAX_SLAVES 4
// Device object structure
struct loongson_spi_device_t
{
	conf_object_t* obj;
	// Interface objects and methods;
	struct {
		conf_object_t *slave_obj;
		spi_device_intf *spi_dev_iface;
	} slaves[MAX_SLAVES];

	int slave_index;
	struct registers regs;
	FIFO *spi_fifo;
};
typedef struct loongson_spi_device_t loongson_spi_device;

#define RD_FIFO_EMPTY     0x1
#define RD_FIFO_FULL      0x2
#define WR_FIFO_EMPTY     0x4
#define WR_FIFO_FULL      0x8

static uint32_t spi_regs_offset[] =
{
	0x0000,
	0x0001,
	0x0002,
	0x0003,
	0x0004,
	0x0005,
	0x0006
};

#undef DBG
#ifdef DBG
#define debug_printf printf
#else
#define debug_printf
#endif
#endif
