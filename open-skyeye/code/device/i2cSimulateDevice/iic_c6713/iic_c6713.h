#ifndef __IIC_C6713_H__ 
#define __IIC_C6713_H__ 
struct registers 
{
	
	uint32_t i2coar;
	
	uint32_t i2cier;
	
	uint32_t i2cstr;
	
	uint32_t i2cclkl;
	
	uint32_t i2cclkh;
	
	uint32_t i2ccnt;
	
	uint32_t i2cdrr;
	
	uint32_t i2csar;
	
	uint32_t i2cdxr;
	
	uint32_t i2cmdr;
	
	uint32_t i2cisrc;
	
	uint32_t i2cpsc;
	
	uint32_t i2cpid1;
	
	uint32_t i2cpid2;
	
};

#define FIFO_SIZE 1024
struct c6713_device 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;

    conf_object_t* i2c_bus_obj;

    i2c_bus_intf* i2c_bus_iface;
	
	struct registers regs;

    int i2c_module;

    uint32_t own_slave_addr;

    uint32_t slave_addr;

    uint32_t data_count;

    char fifo_buff[FIFO_SIZE];
	
};
typedef struct c6713_device iic_c6713_device;
static char* regs_name[] = 
{
	"I2COAR",
	"I2CIER",
	"I2CSTR",
	"I2CCLKL",
	"I2CCLKH",
	"I2CCNT",
	"I2CDRR",
	"I2CSAR",
	"I2CDXR",
	"I2CMDR",
	"I2CISRC",
	"I2CPSC",
	"I2CPID1",
	"I2CPID2",
	NULL
};
#define I2COAR 0x0
#define I2CIER 0x4
#define I2CSTR 0x8
#define I2CCLKL 0xC
#define I2CCLKH 0x10
#define I2CCNT 0x14
#define I2CDRR 0x18
#define I2CSAR 0x1C
#define I2CDXR 0x20
#define I2CMDR 0x24
#define I2CISRC 0x28
#define I2CPSC 0x30
#define I2CPID1 0x34
#define I2CPID2 0x38

#define I2C_INTCODE_AL  0x1
#define I2C_INTCODE_NACK  0x2
#define I2C_INTCODE_ARDY  0x3
#define I2C_INTCODE_ICRRDY  0x4
#define I2C_INTCODE_ICXRDY  0x5
#endif  
