#ifndef __FT2000A_UART_H__
#define __FT2000A_UART_H__

#include <skyeye_uart_ops.h>
#include <stdint.h>

typedef union {
	uint32_t all;
	struct {
		uint8_t ERBFI	:1; //���ý������ݿ����ж�
		uint8_t ETBEI	:1; //���� Transmit�ж�
		uint8_t ELSI	:1; //���� Receiver Line ״̬�ж�
		uint8_t EDSSI	:1; //���� Modem ״̬�ж�
		uint8_t res		:3;
		uint8_t PTIME	:1; //��������/�����жϵĲ���
	}bit;
}ier_reg_t;

typedef union {
	uint32_t all;
	struct {
		uint8_t DLS		:2; //���ݳ���ѡ��λ
		uint8_t STOP	:1; //ֹͣλ����ѡ��λ
		uint8_t PEN		:1; //У��ʹ��λ
		uint8_t EPS		:1; //żУ��ѡ��λ
		uint8_t res		:1; //У��ʹ��λ
		uint8_t BC		:1; //Break ����λ
		uint8_t DLAB	:1; //�����������λ
	}bit;
}lcr_reg_t;

typedef union {
	uint32_t all;
	struct {
		uint8_t DR		:1; //���ݳ���ѡ��λ
		uint8_t OE		:1; //�������λ
		uint8_t PE		:1; //��żУ�����λ
		uint8_t FE		:1; //֡����λ
		uint8_t BI		:1; //�����ж�λ
		uint8_t THRE	:1; //���ͱ��ּĴ����ձ�־λ
		uint8_t TEMT	:1; //��������λ
		uint8_t RFE		:1; //���� FIFO ����λ
	}bit;
}lsr_reg_t;

typedef union {
	uint32_t all;
	struct {
		uint8_t BUSY	:1; //ָʾ UART æ
		uint8_t TFNF	:1; //ָʾ���� FIFO δ��
		uint8_t TFE		:1; //ָʾ���� FIFO ��
		uint8_t RFNE	:1; //ָʾ���� FIFO ��Ϊ��
		uint8_t RFF		:1; //ָʾ���� FIFO ��
	}bit;
}usr_reg_t;

struct uart_registers
{
	uint32_t rbr;		//[RO]���ջ���Ĵ���
	uint32_t thr;		//[WO]���ͱ��ּĴ���
	uint32_t dll;		//[WR]�����ʳ�����λ�ֽڼĴ���
	uint32_t dlh;		//[WR]�����ʳ�����λ�ֽڼĴ���
	ier_reg_t ier;		//[RW]�ж�ʹ�ܼĴ���
	uint32_t iir;		//[RW]�ж�ʶ��Ĵ���
	uint32_t fcr;		//[WO]FIFO���ƼĴ���
	lcr_reg_t lcr;		//[RW]�п��ƼĴ���
	lsr_reg_t lsr;		//[RO]��״̬�Ĵ���
	usr_reg_t usr;		//����״̬�Ĵ���
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
