#ifndef __FT2000A_UART_H__
#define __FT2000A_UART_H__

#include <skyeye_uart_ops.h>
#include <stdint.h>

typedef union {
	uint32_t all;
	struct {
		uint8_t ERBFI	:1; //启用接收数据可用中断
		uint8_t ETBEI	:1; //启用 Transmit中断
		uint8_t ELSI	:1; //启用 Receiver Line 状态中断
		uint8_t EDSSI	:1; //启用 Modem 状态中断
		uint8_t res		:3;
		uint8_t PTIME	:1; //用来启用/禁用中断的产生
	}bit;
}ier_reg_t;

typedef union {
	uint32_t all;
	struct {
		uint8_t DLS		:2; //数据长度选择位
		uint8_t STOP	:1; //停止位个数选择位
		uint8_t PEN		:1; //校验使能位
		uint8_t EPS		:1; //偶校验选择位
		uint8_t res		:1; //校验使能位
		uint8_t BC		:1; //Break 控制位
		uint8_t DLAB	:1; //除数锁存访问位
	}bit;
}lcr_reg_t;

typedef union {
	uint32_t all;
	struct {
		uint8_t DR		:1; //数据长度选择位
		uint8_t OE		:1; //溢出错误位
		uint8_t PE		:1; //奇偶校验错误位
		uint8_t FE		:1; //帧错误位
		uint8_t BI		:1; //发送中断位
		uint8_t THRE	:1; //发送保持寄存器空标志位
		uint8_t TEMT	:1; //发送器空位
		uint8_t RFE		:1; //接收 FIFO 错误位
	}bit;
}lsr_reg_t;

typedef union {
	uint32_t all;
	struct {
		uint8_t BUSY	:1; //指示 UART 忙
		uint8_t TFNF	:1; //指示发送 FIFO 未满
		uint8_t TFE		:1; //指示发送 FIFO 空
		uint8_t RFNE	:1; //指示接收 FIFO 不为空
		uint8_t RFF		:1; //指示接收 FIFO 满
	}bit;
}usr_reg_t;

struct uart_registers
{
	uint32_t rbr;		//[RO]接收缓存寄存器
	uint32_t thr;		//[WO]发送保持寄存器
	uint32_t dll;		//[WR]波特率除数低位字节寄存器
	uint32_t dlh;		//[WR]波特率除数高位字节寄存器
	ier_reg_t ier;		//[RW]中断使能寄存器
	uint32_t iir;		//[RW]中断识别寄存器
	uint32_t fcr;		//[WO]FIFO控制寄存器
	lcr_reg_t lcr;		//[RW]行控制寄存器
	lsr_reg_t lsr;		//[RO]行状态寄存器
	usr_reg_t usr;		//串口状态寄存器
};

struct ft2000a_uart_device_t
{
	conf_object_t* obj;
	struct uart_registers regs;
	struct {
		conf_object_t *obj;
		skyeye_uart_intf *intf;
	}term;
	struct {
		int num;
		conf_object_t *obj;
		general_signal_intf *intf;
	}signal;
	conf_object_t *uart_file;
	uart_file_intf *uart_file_iface;
	uint8_t send_buf[1024];
	uint32_t send_buf_index;
	int isLock;
};

typedef struct ft2000a_uart_device_t ft2000a_uart_device;

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
	NULL
};

#define RBR		0x00
#define THR		0x00
#define DLL		0x00
#define DLH		0x04
#define IER		0x04
#define IIR		0x08
#define FCR		0x08
#define LCR		0x0c
#define LSR		0x14
#define USR		0x7c

static uint32_t regs_offset[] = {
	RBR,
	THR,
	DLL,
	DLH,
	IER,
	IIR,
	FCR,
	LCR,
	LSR,
	USR
};

#define FIFO_ENABLE 0x1

#define IIR_NINT	0x1
#define IIR_TE		0x2
#define IIR_RDA		0x4
#define IIR_NON		0xf

#define SET_BIT(reg, mask) ((reg) |= (mask))
#define CLEAR_BIT(reg, mask) ((reg) &= ~(mask))
#define CHECK_BIT(reg, mask) ((reg) & (mask))
#endif
