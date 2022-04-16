#ifndef __SPI_AM335X_H
#define __SPI_AM335X_H

#include <skyeye_dev_bus_intf.h>

/****************/
/*    offset    */
/****************/
#define  MCSPI_REVISION           0x000                                //McSPI revision register 
#define  MCSPI_SYSCONFIG          0x110                                //McSPI system configuration register
#define  MCSPI_SYSSTATUS          0x114                                //McSPI system status register 
#define  MCSPI_IRQSTATUS          0x118                                //McSPI interrupt status register
#define  MCSPI_IRQENABLE          0x11C                                //McSPI interrupt enable register
#define  MCSPI_SYST               0x124                                //McSPI system register
#define  MCSPI_MODULCTRL          0x128                                //McSPI module control register 
#define  MCSPI_CH0CONF            0x12C                                //McSPI channel i configuration register 
#define  MCSPI_CH0STAT            0x130                                //McSPI channel i status register 
#define  MCSPI_CH0CTRL            0x134                                //McSPI channel i control register
#define  MCSPI_TX0                0x138                                //McSPI channel i FIFO transmit buffer register 
#define  MCSPI_RX0                0x13C                                //McSPI channel i FIFO receive buffer register 
#define  MCSPI_CH1CONF            0x140                                //McSPI channel i configuration register 
#define  MCSPI_CH1STAT            0x144                                //McSPI channel i status register 
#define  MCSPI_CH1CTRL            0x148                                //McSPI channel i control register
#define  MCSPI_TX1                0x14C                                //McSPI channel i FIFO transmit buffer register 
#define  MCSPI_RX1                0x150                                //McSPI channel i FIFO receive buffer register 
#define  MCSPI_CH2CONF            0x154                                //McSPI channel i configuration register 
#define  MCSPI_CH2STAT            0x158                                //McSPI channel i status register 
#define  MCSPI_CH2CTRL            0x15C                                //McSPI channel i control register 
#define  MCSPI_TX2                0x160                                //McSPI channel i FIFO transmit buffer register 
#define  MCSPI_RX2                0x164                                //McSPI channel i FIFO receive buffer register 
#define  MCSPI_CH3CONF            0x168                                //McSPI channel i configuration register 
#define  MCSPI_CH3STAT            0x16C                                //McSPI channel i status register register 
#define  MCSPI_CH3CTRL            0x170                                //McSPI channel i control register 
#define  MCSPI_TX3                0x174                                //McSPI channel i FIFO transmit buffer register 
#define  MCSPI_RX3                0x178                                //McSPI channel i FIFO receive buffer register 
#define  MCSPI_XFERLEVEL          0x17C                                //McSPI transfer levels register 
#define  MCSPI_DAFTX              0x180                                //McSPI DMA address aligned FIFO transmitter register 
#define  MCSPI_DAFRX              0x1A0                                //McSPI DMA address aligned FIFO receiver register 


/*SYSCONFIG*/
#define MCSPI_SYSCONFIG_SOFTRESET   (0x00000002u)                                                                  
#define MCSPI_SYSCONFIG_SOFTRESET_SHIFT   (0x00000001u)
#define MCSPI_SYSCONFIG_SOFTRESET_DONE   (0x0u)
#define MCSPI_SYSCONFIG_SOFTRESET_INITIATE   (0x1u)
#define MCSPI_SYSCONFIG_SOFTRESET_ONGOING   (0x1u)

/* SYSSTATUS */
#define MCSPI_SYSSTATUS_RESETDONE   (0x00000001u)
#define MCSPI_SYSSTATUS_RESETDONE_SHIFT   (0x00000000u)
#define MCSPI_SYSSTATUS_RESETDONE_COMPLETED   (0x1u)
#define MCSPI_SYSSTATUS_RESETDONE_ONGOING   (0x0u)

typedef struct _FIFO
{
    uint8    *pFirst;
    uint8    *pLast; 
    uint8    *pIn;
    uint8    *pOut;
    uint32  Length;
    uint32  Enteres;

}FIFO;


typedef struct SPIState
{
	conf_object_t *obj;
	struct registers
	{
		uint32_t mcspi_revision;                    //rw
                uint32_t mcspi_sysconfig;                   //rw
                uint32_t mcspi_sysstatus;                   //r
                uint32_t mcspi_irqstatus;                   //rw
                uint32_t mcspi_irqenable;                   //rw
                uint32_t mcspi_syst;                        //rw
                uint32_t mcspi_modulctrl;                   //rw
                uint32_t mcspi_ch0conf;                     //rw
                uint32_t mcspi_ch0stat;                     //r
                uint32_t mcspi_ch0ctrl;                     //rw
                uint32_t mcspi_tx0;                         //rw                  
                uint32_t mcspi_rx0;                         //rw
                uint32_t mcspi_ch1conf;                     //rw
                uint32_t mcspi_ch1stat;                     //r
                uint32_t mcspi_ch1ctrl;                     //rw
                uint32_t mcspi_tx1;                         //rw
                uint32_t mcspi_rx1;                         //rw
                uint32_t mcspi_ch2conf;                     //rw
                uint32_t mcspi_ch2stat;                     //r
                uint32_t mcspi_ch2ctrl;                     //rw
                uint32_t mcspi_tx2;                         //rw
                uint32_t mcspi_rx2;                         //rw
                uint32_t mcspi_ch3conf;                     //rw
                uint32_t mcspi_ch3stat;                     //r
                uint32_t mcspi_ch3ctrl;                     //rw
                uint32_t mcspi_tx3;                         //rw
                uint32_t mcspi_rx3;                         //rw
                uint32_t mcspi_xferlevel;                   //rw
                uint32_t mcspi_daftx;		            //rw
                uint32_t mcspi_dafrx;                       //r
	}regs;
	conf_object_t* signal_obj;
	general_signal_intf *signal;

	conf_object_t* spi_bus_obj;
	spi_bus_intf *spi_bus_iface;
	uint32_t interrupt_number;
	int read_num;
	int FirstFlag;
	int ReceiveLen;
	int ReadFlag;
}am335x_spi_dev;

#endif
