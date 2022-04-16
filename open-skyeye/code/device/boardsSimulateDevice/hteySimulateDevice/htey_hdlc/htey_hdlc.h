#ifndef __HTEY_HDLC_H__ 
#define __HTEY_HDLC_H__ 
struct registers 
{
	
	uint32_t hdlc_stack;
	
	uint32_t hdlc_txlenl;
	
	uint32_t hdlc_txlenh;
	
	uint32_t hdlc_rx_tx_start_status;
	
	uint32_t hdlc_rxlenl;
	
	uint32_t hdlc_rxlenh;
	
	uint32_t hdlc_head;
	
	uint32_t hdlc_idle;
	
	uint32_t hdlc_bdrl;
	
	uint32_t hdlc_bdrh;
	
	uint32_t hdlc_periodl;
	
	uint32_t hdlc_periodh;
	
	uint32_t hdlc_if;
	
	uint32_t hdlc_ier;
	
	uint32_t hdlc_txbase;
	
	uint32_t hdlc_rxbase;
	
};


struct hdlc_device 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;

	conf_object_t* serial;
	skyeye_serial_device_t* serial_iface;
	
	struct registers regs;
	char *rbuf;
	char *wbuf;
	
	conf_object_t *kgl;
	htey_kgl_intf *kgl_iface;	

	conf_object_t *uart_file;
	uart_file_intf* uart_file_iface;

	conf_object_t* term;
	skyeye_uart_intf* term_iface;

	uint32_t frame_total_num;
	uint32_t execution_status;

	uint32_t frame_num;
	uint32_t frame_all_num;
};
typedef struct hdlc_device htey_hdlc_device;
static char* regs_name[] = 
{
	"hdlc_stack",
	"hdlc_txlenl",
	"hdlc_txlenh",
	"hdlc_rx_tx_start_status",
	"hdlc_rxlenl",
	"hdlc_rxlenh",
	"hdlc_head",
	"hdlc_idle",
	"hdlc_bdrl",
	"hdlc_bdrh",
	"hdlc_periodl",
	"hdlc_periodh",
	"hdlc_if",
	"hdlc_ier",
	"hdlc_txbase",
	"hdlc_rxbase",
	NULL
};
#define HDLC_STACK 0x0
#define HDLC_TXLENL 0x20
#define HDLC_TXLENH 0x30
#define HDLC_RX_TX_START_STATUS 0x40
#define HDLC_RXLENL 0x50
#define HDLC_RXLENH 0x60
#define HDLC_HEAD 0x70
#define HDLC_IDLE 0x80
#define HDLC_BDRL 0x90
#define HDLC_BDRH 0xa0
#define HDLC_PERIODL 0xb0
#define HDLC_PERIODH 0xc0
#define HDLC_IF 0xd0
#define HDLC_IER 0xe0
#define HDLC_TXBASE 0x8000
#define HDLC_TXTAIL 0xbfff
#define HDLC_RXBASE 0xc000
#define HDLC_RXTAIL 0xffff

#endif  
