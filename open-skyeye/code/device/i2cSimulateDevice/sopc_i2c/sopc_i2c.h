#ifndef __SOPC_I2C_H
#define __SOPC_I2C_H
#include <skyeye_dev_bus_intf.h>

#define GPIO_SWPORTA_DR		(0x68600000)
#define MACH_OUT_ENABLE		0x6/*设置 b0110 为使能板外*/
#define MACH_IN_ENABLE		0x9/*设置 b1001 为使能板内*/
#define MACH_OUT_DISABLE	0x1/*设置 b0001 为失能板外*/
#define MACH_IN_DISABLE		0x4/*设置 b0100 为失能板内*/

#define MACH_GET(val)	((val & 0x3C0000) >> 18)

#define M_PAGE_BYTE_NUM			64
#define M_PAGE_LIMIT			512

#define  I2C_CON                0x00      //I2C Control Register  
#define  I2C_TAR                0x04      //I2C target addr Register  
#define  I2C_SAR                0x08      //I2c slave addr Register
#define  I2C_HS_MADDR           0x0c      //I2c high speed mode addr Register
#define  I2C_DATA_CMD           0x10      //I2c data read/write and cmdRegister
#define  I2C_SS_SCL_HCNT         0x14      //I2c standard speed time cont high Register
#define  I2C_SS_SCL_LCNT         0x18      //I2c standard time cont low Register
#define  I2C_FS_SCL_HCNT         0x1c      //I2c fast speed time cont high Register
#define  I2C_FS_SCL_LCNT         0x20      //I2c fast speed time cont low Register
#define  I2C_HS_SCL_HCNT         0x24      //I2c high speed time cont high Register
#define  I2C_HS_SCL_LCNT         0x28      //I2c high speed time cont low Register
#define  I2C_INTR_STAT           0x2c      //I2c Interrupt Status Register
#define  I2C_INTR_MASK           0x30      //I2c Interrupt Mask Register
#define  I2C_RAW_INTR_STAT       0x34      //I2c RAW Interrupt Status Register
#define  I2C_RX_TL               0x38      //I2c rx FIFO threshold NUM Register
#define  I2C_TX_TL               0x3c      //I2c tx FIFO threshold NUM Register
#define  I2C_CLR_INTR            0x40      //I2c clear Interrupt Register
#define  I2C_CLR_RX_UNDER        0x44      //I2c clear RX_UNDER interrupt Register
#define  I2C_CLR_RX_OVER         0x48      //I2c clear RX_OVER interrupt Register
#define  I2C_CLR_TX_OVER         0x4c      //I2c clear TX_OVER interrupt Register
#define  I2C_CLR_RD_REQ          0x50      //I2c clear RD_REQ interrupt Register
#define  I2C_CLR_TX_ABRT         0x54      //I2c clear TX_ABRT interrupt Register
#define  I2C_CLR_RX_DONE         0x58      //I2c cleart RX Done interrupt Register
#define  I2C_CLR_ACTIVITY        0x5c      //I2c clear ACTIVITY interrupt Register
#define  I2C_CLR_STOP_DET        0x60      //I2c clear STOP_DET interrupt Register
#define  I2C_CLR_START_DET       0x64      //I2c clear START_DET interrupt Register
#define  I2C_ENABLE              0x6c      //I2c Enable Register
#define  I2C_STATUS              0x70      //I2c Status Register
#define  I2C_TXFLR               0x74      //I2c TX FIFO Register
#define  I2C_RXFLR               0x78      //I2c RX FIFO Register
#define	 I2C_TX_ABRT_SOURCE		 0x80	   //I2C Transmit Abort Source Register
#define  I2C_SLV_DATA_NACK_ONLY	 0x84	   //Generate Slave Data NACK Register
#define  I2C_DMA_CR				 0x88	   //DMA Control Register
#define  I2C_DMA_TDLR			 0x8c 	   //DMA Transmit Data Level Register
#define  I2C_DMA_RDLR			 0x90	   //I2C Receive Data Level Register
#define  I2C_SDA_SETUP			 0x94	   //I2C SDA Setup Register 
#define  I2C_ACK_GENERAL_CALL	 0x98	   //I2C ACK General Call Register
#define  I2C_ENABLE_STATUS	     0x9c	   //I2C Enable Status Register
#define  I2C_COMP_PARAM_1		 0xf4	   //Component Parameter Register 1	
#define  I2C_COMP_VERSION	     0xf8      //I2C Component Version Register 
#define  I2c_COMP_TYPE		     0xfc	   //I2C Component Type Register 

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
	uint32_t i2c_con;
	uint32_t i2c_tar;
	uint32_t i2c_sar;
	uint32_t i2c_hs_maddr;
	uint32_t i2c_data_cmd;
	uint32_t i2c_ss_scl_hcnt;
	uint32_t i2c_ss_scl_lcnt;
	uint32_t i2c_fs_scl_hcnt;
	uint32_t i2c_fs_scl_lcnt;
	uint32_t i2c_hs_scl_hcnt;
	uint32_t i2c_hs_scl_lcnt;
	uint32_t i2c_intr_stat;
	uint32_t i2c_intr_mask;
	uint32_t i2c_raw_intr_stat;
	uint32_t i2c_rx_tl;
	uint32_t i2c_tx_tl;
	uint32_t i2c_clr_intr;
	uint32_t i2c_clr_rx_under;
	uint32_t i2c_clr_rx_over;
	uint32_t i2c_clr_tx_over;
	uint32_t i2c_clr_rd_req;
	uint32_t i2c_clr_tx_abrt;
	uint32_t i2c_clr_rx_done;
	uint32_t i2c_clr_activity;
	uint32_t i2c_clr_stop_det;
	uint32_t i2c_clr_start_def;
	uint32_t i2c_enable;
	uint32_t i2c_status;
	uint32_t i2c_txflr;
	uint32_t i2c_rxflr;
	uint32_t i2c_tx_abrt_source;
	uint32_t i2c_slv_data_nack_only;
	uint32_t i2c_dma_cr;
	uint32_t i2c_dma_tdlr;
	uint32_t i2c_dma_rdlr;
	uint32_t i2c_sda_setup;
	uint32_t i2c_ack_general_call;
	uint32_t i2c_enable_status;
	uint32_t i2c_comp_param_1;
	uint32_t i2c_comp_version;
	uint32_t i2c_comp_type;
};

typedef struct IICState
{
		conf_object_t *obj;

		conf_object_t* signal_obj;
		general_signal_intf *signal;

		memory_space_intf *space_iface;
		conf_object_t     *space;

		conf_object_t* i2c_bus_obj;
		i2c_bus_intf *i2c_bus_iface;
		struct registers regs;

		char	*eeprom_filename;
		char	*in_eeprom_filename;
		char	*out_eeprom_filename;

		uint32_t page_num;
		uint32_t page_size;

		uint8_t w_out_count;
		uint8_t w_in_count;
		int w_out_mach_flag;
		int w_in_mach_flag;

		FIFO *i2c_fifo;
		FIFO *i2c_refifo;
		uint32_t interrupt_number;

		FILE *out_fp;
		FILE *in_fp;

		int write_flag;
		int out_file_exist;
		int in_file_exist;
		uint8_t read_count;
}sopc_i2c_dev;

static char* regs_name[] =
{
		"i2c_con",
		"i2c_tar",
		"i2c_sar",
		"i2c_hs_maddr",
		"i2c_data_cmd",
		"i2c_ss_scl_hcnt",
		"i2c_ss_scl_lcnt",
		"i2c_fs_scl_hcnt",
		"i2c_fs_scl_lcnt",
		"i2c_hs_scl_hcnt",
		"i2c_hs_scl_lcnt",
		"i2c_intr_stat",
		"i2c_intr_mask",
		"i2c_raw_intr_stat",
		"i2c_rx_tl",
		"i2c_tx_tl",
		"i2c_clr_intr",
		"i2c_clr_rx_under",
		"i2c_clr_rx_over",
		"i2c_clr_tx_over",
		"i2c_clr_rd_req",
		"i2c_clr_tx_abrt",
		"i2c_clr_rx_done",
		"i2c_clr_activity",
		"i2c_clr_stop_det",
		"i2c_clr_start_def",
		"i2c_enable",
		"i2c_status",
		"i2c_txflr",
		"i2c_rxflr",
		"i2c_tx_abrt_source",
		"i2c_slv_data_nack_only",
		"i2c_dma_cr",
		"i2c_dma_tdlr",
		"i2c_dma_rdlr",
		"i2c_sda_setup",
		"i2c_ack_general_call",
		"i2c_enable_status",
		"i2c_comp_param_1",
		"i2c_comp_version",
		"i2c_comp_type",
		NULL
};
#define I2C_DATA_READ_CMD	0x100

#define I2C_RAW_INTR_ACTIVITY (1 << 8)
#define WRITE_VAL(val)	(val & 0xff)
#define READ_VAL(dev)	(dev->regs.i2c_data_cmd & 0xff)

#define I2C_DMA_STATUS(val)	(val & 0xF)
#define I2C_DMA_ENABLE		0x2
#define I2C_DMA_DISABLE		0x0

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
