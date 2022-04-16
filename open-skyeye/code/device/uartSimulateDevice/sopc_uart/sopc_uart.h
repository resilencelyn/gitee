#ifndef __SOPC_UART_H__
#define __SOPC_UART_H__

#include <skyeye_uart_ops.h>
typedef union {
	uint32_t all;
	struct
	{
		uint32_t rx_threshold  :7;
		uint32_t tx_threshold  :7;
		uint32_t rx_en         :1;
		uint32_t tx_en         :1;
		uint32_t rx_int_en     :1;
		uint32_t tx_int_en     :1;
		uint32_t stp_len       :2;
		uint32_t data_len      :2;
		uint32_t sample_mode   :2;
		uint32_t check_mode    :2;
		uint32_t Parity_en     :1;
		uint32_t reset_rx_fifo :1;
		uint32_t reset_tx_fifo :1;
		uint32_t clr_int       :1;
		uint32_t Perr_en       :1;
		uint32_t res           :1;
	};
}uart_ctl_t;
/*
	[30]-Perr_en:		1-parity err data save enable,0;parity err data save disable
	[29]-CLR INT:       1-clear tx fifo empty int
	[28]-RESET TX FIFO:       1-reset tx fifo
	[27]-RESET RX FIFO:       1-reset rx fifo
	[26]-Parity EN:           1-check enable
	                          0:check disable
	[25:24]-check mode:    00:even
	                       01:odd
	                       10:check bit number is 0
	                       11:check bit number is 1
	[23:22]-sample mode:   00:one-time sample
	                       01:three-time sample
	[21:20]-data len:      00:5bits
	                       01:6bits
	                       10:7bits
	                       11:8bits
	[19:18]-stp len:       00:1bits
	                       01:2bits
	[17]-tx int en:        TX FIFO int en
	[16]-rx int en:        RX FIFO int en
	[15]-tx en:            1:tx enable
	[14]-rx en:            0:rx enable
	[13:7]: tx threshold                   threshold int of TX FIFO
	[6:0]: rx threshold                    threshold int of RX FIFO
*/
struct registers
{
	uint32_t  rbr;    //Data Receive Register
	uint32_t  thr;    //Data Transmit Register
	uint32_t  dll;    //division low  Reigster
	uint32_t  dlh;    //division high  Register
	uint32_t  ier;    //intr enable Register
	uint32_t  iir;    //intr status Register
	uint32_t  fcr;    //FIFO Control Register0
	uint32_t  lcr;    //Serial Control Register1
	uint32_t  lsr;    //Serial Status Register
	uint32_t  usr;    //UART Status Register
	uint32_t  tfl;    //Transimit FIFO counts Register
	uint32_t  rfl;    //Receive FIFO counts Register
	uint32_t  uart_st;    // uart status register
	uart_ctl_t  uart_ctl; // uart control register
	uint32_t  uart_baud;  // uart baud rate register
};



struct sopc_uart_device_t
{
	conf_object_t* obj;
	struct {
		conf_object_t          *obj;
		skyeye_uart_intf       *intf;
	}term;
	struct {
		conf_object_t          *obj;
		uart_file_intf         *intf;
	}uart_file;
	struct {
		int                     num;
		conf_object_t          *obj;
		general_signal_intf    *intf;
	}signal;
	struct registers regs;
	int tx_fifo_cnt;
	int rx_fifo_cnt;
	FIFO* recv_fifo;
};

typedef struct sopc_uart_device_t sopc_uart_device;

/*Uart Registers Base--0x6a060000 (uart0) Offset*/
#define RBR   0x00
#define THR   0x00
#define DLL   0x00
#define DLH   0x04
#define IER   0x04
#define IIR   0x08
#define FCR   0x08
#define LCR   0x0c
#define LSR   0x14
#define USR   0x7c
#define TFL   0x80
#define RFL   0x84

#define UART_TB   0x180  // uart TX FIFO
#define UART_RB   0x1A0  // uart RX FIFO
#define UART_ST   0x1C0  // uart status register
#define UART_CTL  0x1C4  // uart control register
#define UART_BAUD   0x1C8  // uart baud rate register

static char* regs_name[] =
{
	"rbr",
	"thr",
	"dll",
	"dlh",
	"ier",
	"iir",
	"fcr",
	"lcr",
	"lsr",
	"usr",
	"tfl",
	"rfl",
	"uart_st",
	"uart_ctl",
	"uart_baud",
	NULL
};

static int regs_offset[] =
{
	RBR,
	THR,
	DLL,
	DLH,
	IER,
	IIR,
	FCR,
	LCR,
	LSR,
	USR,
	TFL,
	RFL,
	UART_ST,
	UART_CTL,
	UART_BAUD
};

#define SET_BIT(reg, mask) ((reg) |= (mask))
#define CLR_BIT(reg, mask) ((reg) &= ~(mask))
#define CHK_BIT(reg, mask) ((reg) & (mask))

#define LSR_TEMT   0x40
#define LSR_THRE   0x20
#define LSR_DR     0x01

#define IER_ETHREI 0x02

#define IIR_NON    0x0f
#define IIR_NINT   0x01
#define IIR_TE     0x02
#define IIR_RDA    0x04

#define LCR_DLAB   0x80
#endif
