#ifndef __SPI_XDHT_H
#define __SPI_XDHT_H

#include <skyeye_dev_bus_intf.h>

typedef struct _FIFO
{
    char    *pFirst;
    char    *pLast; 
    char   *pIn;
    char    *pOut;
    uint32  Length;
    uint32  Enteres;

}FIFO;

typedef struct SPIState
{
	conf_object_t *obj;
	struct registers
	{	
		uint16 spi_cr;  
		uint16 spi_rsr; 
		uint16 spi_csr; 
		uint16 spi_stwr;
		uint16 spi_sr;  
		uint16 spi_mrr;
		uint16 spi_rfor;
		uint16 spi_crfr;
		uint16 spi_tfor;
		uint16 spi_dlr;
		uint16 spi_rf;
		uint16 spi_tf;
	}regs;
	FIFO rx_fifo;
	FIFO tx_fifo;
	conf_object_t* spi_bus_obj;
	spi_bus_intf *spi_bus_iface;

	conf_object_t* signal_obj;
	general_signal_intf *signal;
	uint32_t interrupt_number;
	int StartFlag;
	int write_num;
	int MASTER_SEND_FLAG;
}xdht_spi_dev;


/*======REGISTER  OFFSET=================================*/
#define SPI_CR      0x0        //Control Register(w&r)                                                
#define SPI_RSR     0x2        //Rate Set Register(w)
#define SPI_CSR     0x4      //Chip Select Register(w)
#define SPI_STWR    0x6       //Start Work Register(w)
#define SPI_SR      0x6      //Status Register(r)
#define SPI_MRR     0x8        //SPI Modulel Reset Register(w) 
#define SPI_RFOR    0x8        //Receiver FIFO Occupancy Register(r) 
#define SPI_CRFR    0xa       //Clear Receiver FIFO Register(w) 
#define SPI_TFOR    0xa       //Transmit FIFO Occupancy Register(r)
#define SPI_DLR     0xc        //Data Length Register(w)
#define SPI_RF      0xe        //Receiver FIFO(r)
#define SPI_TF      0xe        //Transmit FIFO(w)
#endif
