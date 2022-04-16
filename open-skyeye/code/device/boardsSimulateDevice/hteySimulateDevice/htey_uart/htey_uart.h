#ifndef __HTEY_UART_H__ 
#define __HTEY_UART_H__ 
#include "htey_fifo.h" 
struct registers 
{
	
	uint32_t uart_tx_rx;
	
	uint32_t uart_ier;
	
	uint32_t uart_ifr;
	
	uint32_t uart_lcr;
	
	uint32_t uart_fcr;
	
	uint32_t uart_lsr;
	
	uint32_t uart_bdrh;
	
	uint32_t uart_bdrl;
	
};


struct uart_device 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;
	
	struct registers regs;

	FIFO *read_fifo;

	conf_object_t *serial;
	skyeye_serial_device_t *serial_iface;
	
	conf_object_t *client;
	htey_client_intf *client_iface;
};
typedef struct uart_device htey_uart_device;
static char* regs_name[] = 
{
	"uart_tx_rx",
	"uart_ier",
	"uart_ifr",
	"uart_lcr",
	"uart_fcr",
	"uart_lsr",
	"uart_bdrh",
	"uart_bdrl",
	NULL
};
#define UART_TX_RX 0x0
#define UART_IER 0x10
#define UART_IFR 0x20
#define UART_LCR 0x30
#define UART_FCR 0x40
#define UART_LSR 0x50
#define UART_BDRH 0x60
#define UART_BDRL 0x70
#endif  
