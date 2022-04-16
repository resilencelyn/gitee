#ifndef __SPI_XDHT_H
#define __SPI_XDHT_H

#include <skyeye_dev_bus_intf.h>

#define  IIC_IER     0x0   //r&w
#define  IIC_DLL     0x2   //w
#define  IIC_DLM     0x4   //w
#define  IIC_STWR    0x6   //w
#define  IIC_SR      0x6   //r
#define  IIC_MRR     0x8   //w
#define  IIC_RFOR    0x8   //r
#define  IIC_DLR     0xa   //w
#define  IIC_TFOR    0xa   //r
#define  IIC_SAR     0xc   //w
#define  IIC_RF      0xe   //r
#define  IIC_TF      0xe   //w
typedef struct _FIFO
{
    char    *pFirst;
    char    *pLast; 
    char   *pIn;
    char    *pOut;
    uint32  Length;
    uint32  Enteres;

}FIFO;

typedef struct I2CState
{
	conf_object_t *obj;
	struct registers
	{	
		uint16 iic_ier;      //r&w
		uint16 iic_dll;      //w
		uint16 iic_dlm;      //w
		uint16 iic_stwr;     //w
		uint16 iic_sr;       //r
		uint16 iic_mrr;      //w
		uint16 iic_rfor;     //r
		uint16 iic_dlr;      //w
		uint16 iic_tfor;     //r
		uint16 iic_sar;      //w
		uint16 iic_rf;       //r
		uint16 iic_tf;       //w
	}regs;
	FIFO rx_fifo;
	FIFO tx_fifo;
	conf_object_t* i2c_bus_obj;
	i2c_bus_intf *i2c_bus_iface;
	conf_object_t* signal_obj;
	general_signal_intf *signal;
	int MASTER_SEND_FLAG;
	int MASTER_RECEIVE_FLAG;
	int write_count;
	int read_count;
}xdht_iic_dev;




#endif
