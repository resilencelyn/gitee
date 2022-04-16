#ifndef __AT697F_SPI_H__
#define __AT697F_SPI_H__
#include <skyeye_lock.h>
// All equipment register names, the list order and struct registers registers inside order is kept consistent
static char* regs_name[] =
{
	"ctrl",
	"intmsk",
	"data_rev",
	"data_send",
	"cmd",
	"status",
	"chips",
	"atx",
	NULL
};

// device register structure
struct registers
{
	uint32_t ctrl;
	uint32_t intmsk;
	uint32_t data_rev;
	uint32_t data_send;
	uint32_t cmd;
	uint32_t status;
	uint32_t chips;
	uint32_t atx;
};

#define MAX_SLAVES 8
#define HEADERSIZE 6
#define MAX_MSG 2054
// Device object structure
struct at697f_spi_device_t
{
	
	conf_object_t* obj;
	
	// Interface objects and methods;
	FIFO *spi_fifo;
	unsigned char tx_fifo[MAX_MSG];
	int index;
	RWLOCK_T lock;
	struct {
		int                     num;
		conf_object_t          *obj;
		general_signal_intf    *intf;
	}signal;
	struct {
		conf_object_t *slave_obj;
		spi_device_intf *spi_dev_iface;
	} slaves[MAX_SLAVES];
	struct registers regs;
	int slave_index;
	int recv_num;
	int len;
	
};
typedef struct at697f_spi_device_t at697f_spi_device;

// Register offset
#define CTRL 0x00
#define INTMSK 0x04
#define DATA_REV 0x08
#define DATA_SEND 0x0c
#define CMD 0x1c
#define STATUS 0x20
#define CHIPS 0x24
#define ATX 0x28

/*控制器域定义*/
#define SPI_CTR_ENABLE (0x01)
#define SPI_CTR_ROLE   (0x02)

/* SPI状态位定义,高有效 */
#define SPI_ST_SENDING		(0x80)
#define SPI_ST_TXFIFO_FULL	(0x08)
#define SPI_ST_RXFIFO_EMT	(0x20)
#define SPI_ST_RXTX_END		(0x02)

//SPI选通定义
#define SPI_CS_SHUGUAN		(0x01)
#define SPI_CS_JH1			(0x04)
#define SPI_CS_JH2			(0x08)
#define SPI_CS_CHONGGOU		(0x80)

/* 当前全发送模式使用 */
#define SPI_CS_NONE			(0x00)
#define SPI_CS_ALL			(0xFF)

#endif
