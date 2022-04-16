#ifndef __IIC_F28335_H
#define __IIC_F28335_H
#include <skyeye_dev_bus_intf.h>

#define  I2COAR                0x00      //I2C Own Address Register  
#define  I2CIER                0x01      //I2C Interrupt Mask Register  
#define  I2CSTR                0x02      //I2c Interrupt Status Register
#define  I2CCLKL               0x03      //I2C Clock Low-Time Divider Register 
#define  I2CCLKH	       0x04      //I2C Clock Low-Time Divider Register 
#define  I2CCNT                0x05      //I2C Data Count Register 
#define  I2CDRR                0x06      //I2C Data Receive Register 
#define  I2CSAR                0x07      //I2C Slave Address Register 
#define  I2CDXR                0x08      //I2C Data Transmit Register 
#define  I2CMDR                0x09      //I2C Mode Register 
#define  I2CISRC               0x0a      //I2C Interrupt Source Register 
#define  I2CPSC                0x0c      //I2C Prescaler Register 
#define  I2CFFTX 	       0x20      //I2C FIFO transmit Register 
#define  I2CFFRX 	       0x21      //I2C FIFO receive Register  
//I2CRSR 	       0x58     I2C receive shift Register 
//I2CXSR               0x5c     I2C transmit shift Register 




// I2C interrupt vector register bit definitions */
struct I2CIER_BITS
{
	uint16_t ARBL :1; //Arbitration lost interrupt
	uint16_t NACK :1; //No ack interrupt
	uint16_t ARDY :1; //Register access ready interrupt
	uint16_t RRDY :1; //Recieve data ready interrupt
	uint16_t XRDY :1; //Transmit data ready interrupt
	uint16_t SCD  :1; //Stop condition detection
	uint16_t AAS  :1; //Address as slave
	uint16_t rsvd :9;
};

union I2CIER_REG{
	uint16_t           all;
	struct I2CIER_BITS bit;
};
// I2C status register bit definitions */
struct I2CSTR_BITS {                  // bits   description
   uint16_t ARBL      :1;             // 0      Arbitration lost interrupt
   uint16_t NACK      :1;             // 1      No ack interrupt
   uint16_t ARDY      :1;             // 2      Register access ready interrupt
   uint16_t RRDY      :1;             // 3      Recieve data ready interrupt
   uint16_t XRDY      :1;             // 4      Transmit data ready interrupt
   uint16_t SCD       :1;             // 5      Stop condition detection
   uint16_t rsvd1     :2;             // 7:6    reserved
   uint16_t AD0       :1;             // 8      Address Zero
   uint16_t AAS       :1;             // 9      Address as slave
   uint16_t XSMT      :1;             // 10     XMIT shift empty
   uint16_t RSFULL    :1;             // 11     Recieve shift full
   uint16_t BB        :1;             // 12     Bus busy
   uint16_t NACKSNT   :1;             // 13     A no ack sent
   uint16_t SDIR      :1;             // 14     Slave direction
   uint16_t rsvd2     :1;             // 15     reserved
};

union I2CSTR_REG {
   uint16_t               all;
   struct I2CSTR_BITS     bit;
};
// I2C mode control register bit definitions */
struct I2CMDR_BITS {                 // bits   description
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

union I2CMDR_REG {
   uint16_t                 all;
   struct I2CMDR_BITS       bit;
};

// I2C interrupt vector register bit definitions */
struct I2CISRC_BITS {         // bits   description
   uint16_t INTCODE:3;          // 2:0    Interrupt code
   uint16_t rsvd1:13;           // 15:3   reserved
};

union I2CISRC_REG {
   uint16_t               all;
   struct I2CISRC_BITS    bit;
};
// TX FIFO control register bit definitions */
struct I2CFFTX_BITS {         // bits   description
   uint16_t TXFFIL:5;           // 4:0    FIFO interrupt level
   uint16_t TXFFIENA:1;         // 5      FIFO interrupt enable/disable
   uint16_t TXFFINTCLR:1;       // 6      FIFO clear
   uint16_t TXFFINT:1;          // 7      FIFO interrupt flag
   uint16_t TXFFST:5;           // 12:8   FIFO level status
   uint16_t TXFFRST:1;          // 13     FIFO reset
   uint16_t I2CFFEN:1;          // 14     enable/disable TX & RX FIFOs
   uint16_t rsvd1:1;            // 15     reserved

};

union I2CFFTX_REG {
   uint16_t               all;
   struct I2CFFTX_BITS    bit;
};

//----------------------------------------------------
// RX FIFO control register bit definitions */
struct I2CFFRX_BITS {         // bits   description
   uint16_t RXFFIL:5;           // 4:0    FIFO interrupt level
   uint16_t RXFFIENA:1;         // 5      FIFO interrupt enable/disable
   uint16_t RXFFINTCLR:1;       // 6      FIFO clear
   uint16_t RXFFINT:1;          // 7      FIFO interrupt flag
   uint16_t RXFFST:5;           // 12:8   FIFO level
   uint16_t RXFFRST:1;          // 13     FIFO reset
   uint16_t rsvd1:2;            // 15:14  reserved
};

union I2CFFRX_REG {
   uint16_t               all;
   struct I2CFFRX_BITS    bit;
};

#define FIFO_SIZE 1024
typedef struct IICState
{
	conf_object_t *obj;
	struct registers
	{
     		uint16_t i2c_icoar;
        	union I2CIER_REG i2c_icier;
        	union I2CSTR_REG i2c_icstr;
      		uint16_t i2c_icclkl;
        	uint16_t i2c_icclkh;
        	uint16_t i2c_iccnt;
        	uint16_t i2c_icdrr;
        	uint16_t i2c_icsar;
        	uint16_t i2c_icdxr;
        	union I2CMDR_REG i2c_icmdr;
        	union I2CISRC_REG i2c_icisrc;
        	uint16_t i2c_icpsc;
        	union I2CFFTX_REG i2c_icfftx;
        	union I2CFFRX_REG i2c_icffrx;
	}regs;
	conf_object_t* signal_obj;
	general_signal_intf *signal;

	conf_object_t* i2c_bus_obj;
	i2c_bus_intf *i2c_bus_iface;
	int w_cnt;
	int r_cnt;
	char fifo_buffer[FIFO_SIZE];
	uint32_t data_count;
	uint32_t own_slave_addr;
	uint32_t slave_addr;
	uint32_t interrupt_number;
}f28335_iic_dev;

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
	"i2c_icisrc",
	"i2c_icpsc",
	"i2c_icfftx",
	"i2c_icffrx",
	NULL
};

static uint32_t regs_offset[] = {
	0x000,
	0x001,
	0x002,
	0x003,
	0x004,
	0x005,
	0x006,
	0x007,
	0x008,
	0x009,
	0x00a,
	0x00c,
	0x020,
	0x021,
};
#define ICMDR_STT (dev->regs.i2c_icmdr.bit.STT)
#define ICMDR_STP (dev->regs.i2c_icmdr.bit.STP)
#define ICMDR_MST (dev->regs.i2c_icmdr.bit.MST)
#define ICMDR_TRX (dev->regs.i2c_icmdr.bit.TRX)
#define ICMDR_XA  (dev->regs.i2c_icmdr.bit.XA)
#define ICMDR_RM  (dev->regs.i2c_icmdr.bit.RM)
#define ICMDR_IRS (dev->regs.i2c_icmdr.bit.IRS)
#define ICMDR_FDF (dev->regs.i2c_icmdr.bit.FDF)

#define ICAAS    (dev->regs.i2c_icier.bit.AAS)
#define ICSCD    (dev->regs.i2c_icier.bit.SCD)
#define ICXRDY (dev->regs.i2c_icier.bit.XRDY)
#define ICRRDY (dev->regs.i2c_icier.bit.RRDY)
#define ICARDY   (dev->regs.i2c_icier.bit.ARDY)
#define ICNACK   (dev->regs.i2c_icier.bit.NACK)
#define ICAL     (dev->regs.i2c_icier.bit.ARBL)

#define I2C_INTCODE_AL     0x1
#define I2C_INTCODE_NACK   0x2
#define I2C_INTCODE_ARDY   0x3
#define I2C_INTCODE_ICRRDY 0x4
#define I2C_INTCODE_ICXRDY 0x5
#define I2C_INTCODE_STOP   0x6
#define I2C_INTCODE_AAS    0x7

#endif
