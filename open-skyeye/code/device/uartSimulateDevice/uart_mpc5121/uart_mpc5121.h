#ifndef __UART_MPC5121_H__ 
#define __UART_MPC5121_H__ 
struct registers 
{
	
	uint32_t mr1_mr2;
	
	uint32_t sr_csr;
	
	uint32_t cr;
	
	uint32_t rb_tb;
	
	uint32_t ipcr_acr;
	
	uint32_t isr_imr;
	
	uint32_t ctur;
	
	uint32_t ctlr;
	
	uint32_t ccr;
	
	uint32_t ac97slots;
	
	uint32_t ac97cmd;
	
	uint32_t ac97data;
	
	uint32_t ip;
	
	uint32_t op1;
	
	uint32_t op0;
	
	uint32_t sicr;
	
	uint32_t pscn_tx_cmd;
	
	uint32_t pscn_tx_alarm;
	
	uint32_t pscn_tx_sr;
	
	uint32_t pscn_tx_isr;
	
	uint32_t pscn_tx_imr;
	
	uint32_t pscn_tx_count;
	
	uint32_t pscn_tx_pointer;
	
	uint32_t pscn_tx_size;
	
	uint32_t pscn_tx_data;
	
	uint32_t pscn_rx_cmd;
	
	uint32_t pscn_rx_alarm;
	
	uint32_t pscn_rx_stat;
	
	uint32_t pscn_rx_isr;
	
	uint32_t pscn_rx_imr;
	
	uint32_t pscn_rx_count;
	
	uint32_t pscn_rx_pointer;
	
	uint32_t pscn_rx_size;
	
	uint32_t pscn_rx_data;
	
};

#define F_TRUE 1
#define F_FALSE 0

struct mpc5121_device 
{
	
	conf_object_t* obj;
	general_signal_intf* signal_iface;
	conf_object_t* signal;
	conf_object_t *term;
	skyeye_uart_intf *term_iface;
	conf_object_t *uart_file;
	uart_file_intf *uart_file_iface;
	conf_object_t *sfifoc;
	mpc5121_sfifoc_intf *sfifoc_iface;
	struct registers regs;
	uint32_t irq_num;
	uint32_t id;
	uint32_t tx_fifo_empty;
	uint32_t rx_fifo_full;
	int dev_enable;
};
typedef struct mpc5121_device uart_mpc5121_device;
static char* regs_name[] = 
{
	"mr1_mr2",
	"sr_csr",
	"cr",
	"rb_tb",
	"ipcr_acr",
	"isr_imr",
	"ctur",
	"ctlr",
	"ccr",
	"ac97slots",
	"ac97cmd",
	"ac97data",
	"ip",
	"op1",
	"op0",
	"sicr",
	"pscn_tx_cmd",
	"pscn_tx_alarm",
	"pscn_tx_sr",
	"pscn_tx_isr",
	"pscn_tx_imr",
	"pscn_tx_count",
	"pscn_tx_pointer",
	"pscn_tx_size",
	"pscn_tx_data",
	"pscn_rx_cmd",
	"pscn_rx_alarm",
	"pscn_rx_stat",
	"pscn_rx_isr",
	"pscn_rx_imr",
	"pscn_rx_count",
	"pscn_rx_pointer",
	"pscn_rx_size",
	"pscn_rx_data",
	NULL
};

#define 	 MR1_MR2		0x00	//Mode Register 1 (MR1) R/W 
#define 	 MR1_MR2		0x00	//Mode Register 2 (MR2) R/W
#define 	 SR_CSR			0x04	//Status Register (SR) R 0x00 
#define 	 SR_CSR			0x04	//Clock Select Register (CSR) R/W 0x00 
#define 	 CR			0x08	//Command Register (CR) W 0x00 
#define 	 RB_TB			0x0C	//RX Buffer Register (RB) R 0x0000_0000 
#define 	 RB_TB			0x0C	//TX Buffer Register (TB) W 0x0000_0000 
#define 	 IPCR_ACR		0x10	//Input Port Change Register (IPCR) R 0x00 
#define 	 IPCR_ACR		0x10	//Auxiliary Control Register (ACR) R/W 0x00 
#define 	 ISR_IMR		0x14	//Interrupt Status Register (ISR) R 0x0000_0000 
#define 	 ISR_IMR		0x14	//Interrupt Mask Register (IMR) R/W 0x0000_0000 
#define 	 CTUR			0x18	//Counter Timer Upper Register (CTUR) R/W 0x00 
#define 	 CTLR			0x1C	//Counter Timer Lower Register (CTLR) R/W 0x00 
#define 	 CCR			0x20	//Codec Clock Register (CCR) R/W 0x0001_0000 
#define 	 AC97SLOTS		0x24	//AC97 Slots Register (AC97Slots) R/W 0x0000_0000 
#define 	 AC97CMD		0x28	//AC97 Command Register (AC97CMD) R/W 0x0000_0000 
#define 	 AC97DATA		0x2C	//AC97 Status Data Register (AC97Data) R 0x0000_0000 
#define 	 IP			0x34	//Input Port Register (IP) R —5 30.2.1.18/30-31
#define 	 OP1			0x38	//Output Port 1 Bit Set Register (OP1) R/W 0xFC 
#define 	 OP0			0x3C	//Output Port 0 Bit Set Register (OP0) R/W 0x00 
#define 	 SICR			0x40	//Serial Interface Control Register (SICR) R/W 0x0000_0000 
#define 	 PSCN_TX_CMD		0x80	//Command register for PSCn TX slice (PSCn_TX_CMD) R/W 0x0000_0000 
#define 	 PSCN_TX_ALARM		0x84	//Alarm level for PSCn TX slice (PSCn_TX_ALARM) R/W 0x0000_0000 
#define 	 PSCN_TX_SR		0x88	//Status register for PSCn TX slice (PSCn_TX_SR) R 0x0000_0080 
#define 	 PSCN_TX_ISR		0x8C	//Interrupt status register for PSCn TX slice (PSCn_TX_ISR) R 0x0000_0000 
#define 	 PSCN_TX_IMR		0x90	//Interrupt mask register for PSCn TX slice (PSCn_TX_IMR) R/W 0x0000_0000 
#define 	 PSCN_TX_COUNT		0x94	//FIFO count for PSCn TX slice (PSCn_TX_COUNT) R 0x0000_0000 
#define 	 PSCN_TX_POINTER	0x98	//FIFO pointer for PSCn TX slice (PSCn_TX_POINTER) R 0x0000_0000 
#define 	 PSCN_TX_SIZE		0x9C	//FIFO size register for PSCn TX slice (PSCn_TX_SIZE) R/W 0x0000_0000 
#define 	 PSCN_TX_DATA		0xBC	//FIFO data register for PSCn TX slice (PSCn_TX_DATA) R/W 0x0000_0000 
#define 	 PSCN_RX_CMD		0xC0	//Command register for PSCn RX slice (PSCn_RX_CMD) R/W 0x0000_0000 
#define 	 PSCN_RX_ALARM		0xC4	//Alarm level for PSCn RX slice (PSCn_RX_ALARM) R/W 0x0000_0000 
#define 	 PSCN_RX_STAT		0xC8	//Status register for PSCn RX slice (PSCn_RX_STAT) R 0x0000_0080 
#define 	 PSCN_RX_ISR		0xCC	//Interrupt status register for PSCn RX slice (PSCn_RX_ISR) R 0x0000_0000 
#define 	 PSCN_RX_IMR		0xD0	//Interrupt mask register for PSCn RX slice (PSCn_RX_IMR) R/W 0x0000_0000 
#define 	 PSCN_RX_COUNT		0xD4	//FIFO count for PSCn RX slice (PSCn_RX_COUNT) R 0x0000_0000 
#define 	 PSCN_RX_POINTER	0xD8	//FIFO pointer for PSCn RX slice (PSCn_RX_POINTER) R 0x0000_0000 
#define 	 PSCN_RX_SIZE		0xDC	//FIFO size register for PSCn RX slice (PSCn_RX_SIZE) R/W 0x0000_0000 
#define 	 PSCN_RX_DATA		0xFC	//FIFO data register for PSCn RX slice (PSCn_RX_DATA) R/W 0x0000_0000 


#define EMPTY 	0x1
#define FULL  	0x2
#define ALARM 	0x4
#define URERR 	0x8
#define ORERR 	0x10
#define DATAREADY 	0x20
#define MEMERROR 	0x40

#endif  
