#ifndef __IIC_AM335X_H
#define __IIC_AM335X_H
#include <skyeye_dev_bus_intf.h>

#define  I2C_REVNB_LO                0x00      //Module Revision Register (LOW BYTES) 
#define  I2C_REVNB_HI                0x04      //Module Revision Register (HIGH BYTES) 
#define  I2C_SYSC                    0x10      //System Configuration Register
#define  I2C_IRQSTATUS_RAW           0x24      //I2C Status Raw Register 
#define  I2C_IRQSTATUS	             0x28      //I2C Status Register 
#define  I2C_IRQENABLE_SET           0x2C      //I2C Interrupt Enable Set Register 
#define  I2C_IRQENABLE_CLR           0x30      //I2C Interrupt Enable Clear Register 
#define  I2C_WE                      0x34      //I2C Wakeup Enable Register 
#define  I2C_DMARXENABLE_SET         0x38      //Receive DMA Enable Set Register 
#define  I2C_DMATXENABLE_SET         0x3C      //Transmit DMA Enable Set Register 
#define  I2C_DMARXENABLE_CLR         0x40      //Receive DMA Enable Clear Register 
#define  I2C_DMATXENABLE_CLR         0x44      //Transmit DMA Enable Clear Register 
#define  I2C_DMARXWAKE_EN            0x48      //Receive DMA Wakeup Register 
#define  I2C_DMATXWAKE_EN            0x4C      //Transmit DMA Wakeup Register 
#define  I2C_SYSS                    0x90      //System Status Register 
#define  I2C_BUF                     0x94      //Buffer Configuration Register 
#define  I2C_CNT                     0x98      //Data Counter Register 
#define  I2C_DATA                    0x9C      //Data Access Register 
#define  I2C_CON                     0xA4      //I2C Configuration Register 
#define  I2C_OA 		     0xA8      //I2C  Own Address Register 
#define  I2C_SA 		     0xAC      //I2C Slave Address Register 
#define  I2C_PSC 		     0xB0      //I2C Clock Prescaler Register 
#define  I2C_SCLL 		     0xB4      //I2C SCL Low Time Register 
#define  I2C_SCLH 		     0xB8      //I2C SCL High Time Register 
#define  I2C_SYSTEST 		     0xBC      //System Test Register 
#define  I2C_BUFSTAT 		     0xC0      //I2C Buffer Status Register 
#define  I2C_OA1 		     0xC4      //I2C Own Address 1 Register 
#define  I2C_OA2  		     0xC8      //I2C Own Address 2 Register 
#define  I2C_OA3 		     0xCC      //I2C Own Address 3 Register 
#define  I2C_ACTOA 		     0xD0      //Active Own Address Register 
#define  I2C_SBLOCK      	     0xD4      //I2C Clock Blocking Enable Register 



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
		uint32_t i2c_revnb_lo;
                uint32_t i2c_revnb_hi;
                uint32_t i2c_sysc;
                uint32_t i2c_irqstatus_raw;
                uint32_t i2c_irqstatus;
                uint32_t i2c_irqenable_set;
                uint32_t i2c_irqenable_clr;
                uint32_t i2c_we;
                uint32_t i2c_dmarxenable_set;
                uint32_t i2c_dmatxenable_set;
                uint32_t i2c_dmarxenable_clr;
                uint32_t i2c_dmatxenable_clr;
                uint32_t i2c_dmarxwake_en;
                uint32_t i2c_dmatxwake_en;
                uint32_t i2c_syss;
                uint32_t i2c_buf;
                uint32_t i2c_cnt;
                uint32_t i2c_data;
                uint32_t i2c_con;
                uint32_t i2c_oa;
                uint32_t i2c_sa;
                uint32_t i2c_psc;
                uint32_t i2c_scll;
                uint32_t i2c_sclh;
                uint32_t i2c_systest;
                uint32_t i2c_bufstat;
                uint32_t i2c_oa1;
                uint32_t i2c_oa2;
                uint32_t i2c_oa3;
                uint32_t i2c_actoa;
                uint32_t i2c_sblock;
	}regs;
	conf_object_t* signal_obj;
	general_signal_intf *signal;

	conf_object_t* i2c_bus_obj;
	i2c_bus_intf *i2c_bus_iface;
	uint32_t interrupt_number;
	uint32_t write_irq_count;
	uint32_t read_irq_count;
}am335x_iic_dev;
#endif
