#ifndef __RM48X_IIC_H__ 
#define __RM48X_IIC_H__ 










typedef struct _FIFO
{
    char    *pFirst;
    char    *pLast;
    char   *pIn;
    char    *pOut;
    uint32  Length;
    uint32  Enteres;

}FIFO;



struct registers 
{
	
	uint32_t i2coar;
	
	uint32_t i2cimr;
	
	uint32_t i2cstr;
	
	uint32_t i2cckl;
	
	uint32_t i2cckh;
	
	uint32_t i2ccnt;
	
	uint32_t i2cdrr;
	
	uint32_t i2csar;
	
	uint32_t i2cdxr;
	
	uint32_t i2cmdr;
	
	uint32_t i2civr;
	
	uint32_t i2cemdr;
	
	uint32_t i2cpsc;
	
	uint32_t i2cpid1;
	
	uint32_t i2cpid2;
	
	uint32_t i2cdmacr;
	
	uint32_t i2cpfnc;
	
	uint32_t i2cpdir;
	
	uint32_t i2cdin;
	
	uint32_t i2cdout;
	
	uint32_t i2cdset;
	
	uint32_t i2cdclr;
	
	uint32_t i2cpdr;
	
	uint32_t i2cpdis;
	
	uint32_t i2cpsel;
	
	uint32_t i2csrs;
	
};


struct rm48x_iic_device_t 
{
	
	conf_object_t* obj;	
	conf_object_t* core;
	core_signal_intf* signal_iface;
	conf_object_t* signal;

	struct registers regs;
	
	conf_object_t* i2c_bus_obj;
	i2c_bus_intf *i2c_bus_iface;
	FIFO *i2cfifo;
	uint32_t data_count;
	uint32_t send_count;
	uint32_t own_slave_addr;
	uint32_t slave_addr;
	uint32_t interrupt_number;
	uint32_t write_irq_count;
	uint32_t read_irq_count;
	
};
typedef struct rm48x_iic_device_t rm48x_iic_device;
static char* regs_name[] = 
{
	"i2coar",
	"i2cimr",
	"i2cstr",
	"i2cckl",
	"i2cckh",
	"i2ccnt",
	"i2cdrr",
	"i2csar",
	"i2cdxr",
	"i2cmdr",
	"i2civr",
	"i2cemdr",
	"i2cpsc",
	"i2cpid1",
	"i2cpid2",
	"i2cdmacr",
	"i2cpfnc",
	"i2cpdir",
	"i2cdin",
	"i2cdout",
	"i2cdset",
	"i2cdclr",
	"i2cpdr",
	"i2cpdis",
	"i2cpsel",
	"i2csrs",
	NULL
};

#define ICMDR_STT ((dev->regs.i2cmdr >> 13) & 0x1)
#define ICMDR_STP ((dev->regs.i2cmdr >> 11) & 0x1)
#define ICMDR_MST ((dev->regs.i2cmdr >> 10) & 0x1)
#define ICMDR_TRX ((dev->regs.i2cmdr >> 9) & 0x1)
#define ICMDR_XA ((dev->regs.i2cmdr >> 8) & 0x1)
#define ICMDR_RM ((dev->regs.i2cmdr >> 7) & 0x1)
#define ICMDR_IRS ((dev->regs.i2cmdr >> 5) & 0x1)
#define ICMDR_FDF ((dev->regs.i2cmdr >> 3) & 0x1)


#define AAS ((dev->regs.i2cimr >> 6) & 0x1)
#define SCD ((dev->regs.i2cimr >> 5) & 0x1)
#define ICXRDY ((dev->regs.i2cimr >> 4) & 0x1)
#define ICRRDY ((dev->regs.i2cimr >> 3) & 0x1)
#define ARDY ((dev->regs.i2cimr >> 2) & 0x1)
#define NACK ((dev->regs.i2cimr >> 1) & 0x1)
#define AL (dev->regs.i2cimr & 0x1)


#define I2C_INTCODE_AL     0x1
#define I2C_INTCODE_NACK   0x2
#define I2C_INTCODE_ARDY   0x3
#define I2C_INTCODE_ICRRDY 0x4
#define I2C_INTCODE_ICXRDY 0x5
#define I2C_INTCODE_STOP   0x6
#define I2C_INTCODE_AAS    0x7


#define I2C_ICDMAC_TXDMAEN 0x2
#define I2C_ICDMAC_RXDMAEN 0x1


#define ICDMAC_TX ((dev->regs.i2cdmacr >> 1) & 0x1)
#define ICDMAC_RX (dev->regs.i2cdmacr & 0x1)


#define I2COAR 0x00
#define I2CIMR 0x04
#define I2CSTR 0x08
#define I2CCKL 0x0c
#define I2CCKH 0x10
#define I2CCNT 0x14
#define I2CDRR 0x18
#define I2CSAR 0x1c
#define I2CDXR 0x20
#define I2CMDR 0x24
#define I2CIVR 0x28
#define I2CEMDR 0x2c
#define I2CPSC 0x30
#define I2CPID1 0x34
#define I2CPID2 0x38
#define I2CDMACR 0x3c
#define I2CPFNC 0x48
#define I2CPDIR 0x4c
#define I2CDIN 0x50
#define I2CDOUT 0x54
#define I2CDSET 0x58
#define I2CDCLR 0x5c
#define I2CPDR 0x60
#define I2CPDIS 0x64
#define I2CPSEL 0x68
#define I2CSRS 0x6c
#endif  
