#ifndef __C6678_I2C_H__
#define __C6678_I2C_H__

#define I2COAR 	0x0
#define I2CIMR 	0x4
#define I2CSTR 	0x8
#define I2CCLKL 0xC
#define I2CCLKH 0x10
#define I2CCNT 	0x14
#define I2CDRR 	0x18
#define I2CSAR 	0x1C
#define I2CDXR 	0x20
#define I2CMDR 	0x24
#define I2CIVR 	0x28
#define I2CEMDR 0x2C
#define I2CPSC 	0x30
#define I2CPID1 0x34
#define I2CPID2 0x38

struct I2CMDR_BITS
{                 					 // bits   description
   uint16_t BC      :3;              // 2:0    Bit count
   uint16_t FDF     :1;              // 3      Free data format
   uint16_t STB     :1;              // 4      Start byte
   uint16_t IRS     :1;              // 5      I2C Reset not
   uint16_t DLB     :1;              // 6      Digital loopback
   uint16_t RM      :1;              // 7      Repeat mode
   uint16_t XA      :1;              // 8      Expand address
   uint16_t TRX     :1;              // 9      Transmitter/reciever
   uint16_t MST     :1;              // 10     Master/slave
   uint16_t STP     :1;              // 11     Stop condition
   uint16_t rsvd1   :1;              // 12     reserved
   uint16_t STT     :1;              // 13     Start condition
   uint16_t FREE    :1;              // 14     Emulation mode
   uint16_t NACKMOD :1;              // 15     No Ack mode
};

struct I2CSTR_BITS
{                 					 // bits   description
   uint16_t AL			:1;          // 0    	Bit count
   uint16_t NACK    	:1;          // 1     Free data format
   uint16_t ARDY    	:1;          // 2      Start byte
   uint16_t ICRRDY		:1;          // 3      I2C Reset not
   uint16_t ICXRDY  	:1;          // 4      Digital loopback
   uint16_t SCD     	:1;          // 5      Repeat mode
   uint16_t Reserved1	:2;          // 7:6      Expand address
   uint16_t AD0     	:1;          // 8      Transmitter/reciever
   uint16_t AAS     	:1;          // 9     Master/slave
   uint16_t XSMT     	:1;          // 10     Stop condition
   uint16_t RSFULL   	:1;          // 11     reserved
   uint16_t BB     		:1;          // 12     Start condition
   uint16_t NACKSNT    	:1;          // 13     Emulation mode
   uint16_t SDIR 		:1;          // 14     No Ack mode
   uint16_t Reserved2 	:1;			 // 15     No Ack mode
};

typedef union I2CMDR_REG_t 
{
   uint32_t                 all;
   struct I2CMDR_BITS       bit;

}I2CMDR_REG;

typedef union I2CSTR_REG_t 
{
   uint32_t                 all;
   struct I2CSTR_BITS       bit;

}I2CSTR_REG;


// All equipment register names, the list order and struct registers registers inside order is kept consistent
static char* regs_name[] =
{
	"I2C Own Address Register",
	"I2C Interrupt Mask Register",
	"I2C Interrupt Status Register",
	"I2C Clock Low-Time Divider Register",
	"I2C Clock High-Time Divider Register",
	"I2C Data Count Register",
	"I2C Data Receive Register",
	"I2C Slave Address Register",
	"I2C Data Transmit Register",
	"I2C Mode Register",
	"I2C Interrupt Vector Register",
	"I2C Extended Mode Register",
	"I2C Prescaler Register",
	"I2C Peripheral Identification Register 1",
	"I2C Peripheral Identification Register 2",
	NULL
};

// Register offset

// device register structure
struct registers
{
	uint32_t i2coar;
	uint32_t i2cimr;
	I2CSTR_REG i2cstr;
	uint32_t i2cclkl;
	uint32_t i2cclkh;
	uint32_t i2ccnt;
	uint32_t i2cdrr;
	uint32_t i2csar;
	uint32_t i2cdxr;
	I2CMDR_REG i2cmdr;
	uint32_t i2civr;
	uint32_t i2cemdr;
	uint32_t i2cpsc;
	uint32_t i2cpid1;
	uint32_t i2cpid2;
};


// Device object structure
struct c6678_I2C_device_t
{
	conf_object_t* obj;
	conf_object_t* i2c_bus_obj;
	i2c_bus_intf* i2c_bus_iface;
//	conf_object_t* i2c_dev_obj;
//	i2c_device_intf* i2c_dev_iface;
	uint32_t u32i2c_bus_flag;
	// Interface objects and methods;
	struct registers regs;
	int i2c_module;
	uint32_t own_slave_addr;
	uint32_t slave_addr;
};
typedef struct c6678_I2C_device_t c6678_I2C_device;

#endif
