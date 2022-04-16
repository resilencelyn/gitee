#ifndef __LOONGSON_SPI_SLAVE_H__
#define __LOONGSON_SPI_SLAVE_H__

#define MAXSIZE        1024
#define RT3_BASE_ADDR   0x50000000
#define RT2_BASE_ADDR   0x50001000
#pragma pack(1)
typedef union
{
	uint32_t dwData;
	struct
	{
		uint32_t Addr			:12;
		uint32_t RegMemFlag		:1;
		uint32_t WRFlag			:1;
		uint32_t MemPri			:1;
		uint32_t Reserved		:1;
		uint32_t Data			:16;
	}st;
}RT_OPT;

typedef struct protocolformat
{
	uint16_t head;
	uint8_t  cmd;
	uint32_t addr;
	RT_OPT   data;
	uint8_t  cs;		/* check sum */
}stProtocolFormat;

#pragma pack()

// Device object structure
struct loongson_spi_slave_device_t
{
	conf_object_t* obj;
	// Interface objects and methods;
	stProtocolFormat sendProtocol;
	stProtocolFormat recvProtocol;
	memory_space_intf* memory_iface;
	conf_object_t* memory;
	conf_object_t *spi_bus_obj;
	spi_bus_intf *spi_bus_iface;
	unsigned char spiData[MAXSIZE];
	int spiIndex;
	int validFrame;
	int readAddr;

};
typedef struct loongson_spi_slave_device_t loongson_spi_slave_device;

#define SPI_HEAD			(0xEB)
#define SPI_READ			(0x00u)
#define SPI_WRITE			(0x01u)

#define RD_FIFO_EMPTY		(0x1)
#define RD_FIFO_FULL		(0x2)
#define WR_FIFO_EMPTY		(0x4)
#define WR_FIFO_FULL		(0x8)

#undef DBG
#ifdef DBG
#define debug_printf printf
#else
#define debug_printf
#endif
#endif
