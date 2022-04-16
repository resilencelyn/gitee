#ifndef __IIC_OMAPL138_H
#define __IIC_OMAPL138_H
#include <skyeye_dev_bus_intf.h>

#define  I2C_ICOAR                0x00      //I2C Own Address Register  
#define  I2C_ICIMR                0x04      //I2C Interrupt Mask Register  
#define  I2C_ICSTR                0x08      //I2c Interrupt Status Register
#define  I2C_ICCLKL               0x0c      //I2C Clock Low-Time Divider Register 
#define  I2C_ICCLKH	              0x10      //I2C Clock Low-Time Divider Register 
#define  I2C_ICCNT                0x14      //I2C Data Count Register 
#define  I2C_ICDRR                0x18      //I2C Data Receive Register 
#define  I2C_ICSAR                0x1c      //I2C Slave Address Register 
#define  I2C_ICDXR                0x20      //I2C Data Transmit Register 
#define  I2C_ICMDR                0x24      //I2C Mode Register 
#define  I2C_ICIVR                0x28      //I2C Interrupt Vector Register 
#define  I2C_ICEMDR               0x2C      //I2C Extended Mode Register 
#define  I2C_ICPSC                0x30      //I2C Prescaler Register 
#define  I2C_REVID1 		      0x34      //I2C Revision Identification Register 1
#define  I2C_REVID2 		      0x38      //I2C Revision Identification Register 2 
#define  I2C_ICDMAC 		      0x3c      //I2C DMA Control Register 
#define  I2C_ICPFUNC 		      0x48      //I2C Pin Function Register 
#define  I2C_ICPDIR 		      0x4c      //I2C Pin Direction Register 
#define  I2C_ICPDIN 		      0x50      //I2C Pin Data In Register 
#define  I2C_ICPDOUT 		      0x54      //I2C Pin Data Out Register 
#define  I2C_ICPDSET 		      0x58      //I2C Pin Data Set Register 
#define  I2C_ICPDCLR  		      0x5c      //I2C Pin Data Clear Register 



typedef struct _FIFO
{
    char    *pFirst;
    char    *pLast;
    char   *pIn;
    char    *pOut;
    uint32  Length;
    uint32  Enteres;

}FIFO;

typedef struct IICState
{
	conf_object_t *obj;
	struct registers
	{
     	uint32_t i2c_icoar;
        uint32_t i2c_icimr;
        uint32_t i2c_icstr;
        uint32_t i2c_icclkl;
        uint32_t i2c_icclkh;
        uint32_t i2c_iccnt;
        uint32_t i2c_icdrr;
        uint32_t i2c_icsar;
        uint32_t i2c_icdxr;
        uint32_t i2c_icmdr;
        uint32_t i2c_icivr;
        uint32_t i2c_icemdr;
        uint32_t i2c_icpsc;
        uint32_t i2c_revid1;
        uint32_t i2c_revid2;
        uint32_t i2c_icdmac;
        uint32_t i2c_icpfunc;
        uint32_t i2c_icpdir;
        uint32_t i2c_icpdin;
        uint32_t i2c_icpdout;
        uint32_t i2c_icpdset;
        uint32_t i2c_icpdclr;
	}regs;
	conf_object_t* signal_obj;
	general_signal_intf *signal;

	conf_object_t* i2c_bus_obj;
	i2c_bus_intf *i2c_bus_iface;
	FIFO *i2c_fifo;
	uint32_t data_count;
	uint32_t send_count;
	uint32_t own_slave_addr;
	uint32_t slave_addr;
	uint32_t interrupt_number;
	uint32_t write_irq_count;
	uint32_t read_irq_count;
}omapl138_iic_dev;

static char* regs_name[] =
{
	"i2c_icoar",
	"i2c_icimr",
	"i2c_icstr",
	"i2c_icclkl",
	"i2c_icclkh",
	"i2c_iccnt",
	"i2c_icdrr",
	"i2c_icsar",
	"i2c_icdxr",
	"i2c_icmdr",
	"i2c_icivr",
	"i2c_icemdr",
	"i2c_icpsc",
	"i2c_revid1",
	"i2c_revid2",
	"i2c_icdmac",
	"i2c_icpfunc",
	"i2c_icpdir",
	"i2c_icpdin",
	"i2c_icpdout",
	"i2c_icpdset",
	"i2c_icpdclr",
	NULL
};
#define ICMDR_STT ((dev->regs.i2c_icmdr >> 13) & 0x1)
#define ICMDR_STP ((dev->regs.i2c_icmdr >> 11) & 0x1)
#define ICMDR_MST ((dev->regs.i2c_icmdr >> 10) & 0x1)
#define ICMDR_TRX ((dev->regs.i2c_icmdr >> 9) & 0x1)
#define ICMDR_XA ((dev->regs.i2c_icmdr >> 8) & 0x1)
#define ICMDR_RM ((dev->regs.i2c_icmdr >> 7) & 0x1)
#define ICMDR_IRS ((dev->regs.i2c_icmdr >> 5) & 0x1)
#define ICMDR_FDF ((dev->regs.i2c_icmdr >> 3) & 0x1)

#define AAS ((dev->regs.i2c_icimr >> 6) & 0x1)
#define SCD ((dev->regs.i2c_icimr >> 5) & 0x1)
#define ICXRDY ((dev->regs.i2c_icimr >> 4) & 0x1)
#define ICRRDY ((dev->regs.i2c_icimr >> 3) & 0x1)
#define ARDY ((dev->regs.i2c_icimr >> 2) & 0x1)
#define NACK ((dev->regs.i2c_icimr >> 1) & 0x1)
#define AL (dev->regs.i2c_icimr & 0x1)

#define I2C_INTCODE_AL     0x1
#define I2C_INTCODE_NACK   0x2
#define I2C_INTCODE_ARDY   0x3
#define I2C_INTCODE_ICRRDY 0x4
#define I2C_INTCODE_ICXRDY 0x5
#define I2C_INTCODE_STOP   0x6
#define I2C_INTCODE_AAS    0x7

#define I2C_ICDMAC_TXDMAEN 0x2
#define I2C_ICDMAC_RXDMAEN 0x1

#define ICDMAC_TX ((dev->regs.i2c_icdmac >> 1) & 0x1)
#define ICDMAC_RX (dev->regs.i2c_icdmac & 0x1)
#endif
