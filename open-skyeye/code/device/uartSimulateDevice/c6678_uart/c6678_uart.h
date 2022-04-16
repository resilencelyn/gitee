#ifndef __C6678_UART_H__
#define __C6678_UART_H__

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
	"rbr",
	"thr",
	"dlb",
	"ier",
	"dmb",
	"iir",
	"fcr",
	"lcr",
	"mcr",
	"afr",
	"lsr",
	NULL
};


// 设备寄存器结构体
struct registers
{
	uint8 rbr;
	uint8 thr;
	uint8 dlb;
	/* id */
	uint8 ier;
	uint8 dmb;
	/* ifa */
	uint8 iir;
	uint8 fcr;
	uint8 lcr;
	uint8 mcr;
	uint8 afr;
	uint8 lsr;
};


// 中断管理器中断信号接口
struct general_signal_s
{
	conf_object_t* obj;
	general_signal_iface_t* iface;
};


// 串口设备设备双向传输接口
struct skyeye_uart_s
{
	conf_object_t* obj;
	skyeye_uart_intf* iface;
};


// 串口设备终端输出到数据文件接口
struct uart_file_s
{
	conf_object_t* obj;
	uart_file_intf* iface;
};


// 设备对象结构
struct c6678_uart_device_t
{
	conf_object_t* obj;
	// 接口对象和方法
	struct general_signal_s general_signal;
	struct skyeye_uart_s skyeye_uart;
	struct uart_file_s uart_file;
	conf_object_t *uart_com;
	uart_com_intf *uart_com_iface;
	struct registers regs;
	int event_number;
};
typedef struct c6678_uart_device_t c6678_uart_device;

/* mask define */
#define IER_EMSI   0x08
#define IER_ERLSI  0x04
#define IER_ETHREI 0x02
#define IER_ERDAI  0x01
#define IIR_FE     0xc0
#define IIR_NON    0x0f
#define IIR_NINT   0x01
#define IIR_RLS    0x06
#define IIR_RDA    0x04
#define IIR_CTO    0x0c
#define IIR_TE     0x02
#define IIR_MS     0x00
#define FCR_RTL    0xc0
#define FCR_DMS    0x08
#define FCR_TFR    0x04
#define FCR_RFR    0x02
#define FCR_FEN    0x01
#define LCR_DLAB   0x80
#define LCR_SB     0x40
#define LCR_SP     0x20
#define LCR_EPS    0x10
#define LCR_PEN    0x08
#define LCR_NTSB   0x04
#define LCR_WLS    0x03
#define MCR_LOOP   0x10
#define MCR_RTS    0x02
#define LSR_RFE    0x80
#define LSR_TEMT   0x40
#define LSR_THRE   0x20
#define LSR_BI     0x10
#define LSR_FE     0x08
#define LSR_PE     0x04
#define LSR_OE     0x02
#define LSR_DR     0x01
#define MSR_CTS    0x10
#define MSR_DCTS   0x01
#define AFR_BO     0x02
#define AFR_CW     0x01
#define DSR_TXRDY  0x02
#define DSR_RXRDY  0x01

// 寄存器偏移地址
#define THR  0
#define RBR  0
#define IER  1
#define IIR  2
#define FCR  2
#define LCR  3
#define MCR  4
#define LSR  5
#define MSR  6
#define SCR  7

#define SET_BIT(reg, mask) ((reg) |= (mask))
#define CLR_BIT(reg, mask) ((reg) &= ~(mask))
#define CHK_BIT(reg, mask) ((reg) & (mask))

uint32_t reg_off[] =
{
	THR,
	RBR,
	IER,
	IIR,
	FCR,
	LCR,
	MCR,
	LSR,
	MSR,
	SCR
};
#endif
