/*
 * =====================================================================================
 *
 *       Filename:  leon2_uart.h
 *
 *    Description:  UART variables
 *
 *        Version:  1.0
 *        Created:  20/05/08 15:53:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aitor Viana Sanchez (avs), aitor.viana.sanchez@esa.int
 *        Company:  European Space Agency (ESA-ESTEC)
 *
 * =====================================================================================
 */

#ifndef _LEON2_UART_H_
#define _LEON2_UART_H_

/*-----------------------------------------------------------------------------
 *  UART variables
 *-----------------------------------------------------------------------------*/
typedef struct reg_status
{
	uint32 data_ready                          :1;
	uint32 transmitter_shift_register_empty    :1;
	uint32 transmitter_hold_register_empty     :1;
	uint32 break_received                      :1;
	uint32 overrun                             :1;
	uint32 parity_error                        :1;
	uint32 framing_error                       :1;
	uint32 reserved                            :25;
}status_t;

typedef struct reg_control
{
	uint32 receiver_enable                     :1;
	uint32 transmitter_enable                  :1;
	uint32 receiver_interrupt_enable           :1;
	uint32 transmitter_interrupt_enable        :1;
	uint32 parity_select                       :1;
	uint32 parity_enable                       :1;
	uint32 flow_control                        :1;
	uint32 loop_back                           :1;
	uint32 external_clock                      :1;
	uint32 reserved                            :23;
}control_t;


#define FIFO_SIZE	1024
typedef struct UARTState
{
	conf_object_t *obj;
	struct registers
	{
		uint32 data;

		union{
			status_t flag;
			uint32 value;
		}status;

		union{
			control_t flag;
			uint32 value;
		}control;

		uint32 scaler;

		/* just for float format present */
		uint32 fp1;
		uint32 fp2;
	}regs;
	conf_object_t* term_obj;
	skyeye_uart_intf* term;

	conf_object_t *uart_file;
	uart_file_intf *uart_file_iface;
	conf_object_t *uart_com;
	uart_com_intf *uart_com_iface;
	char **regs_name;
	struct{
		conf_object_t* obj;
		skyeye_uart_intf* term;
	}term_dev;
	struct {
		conf_object_t *obj;
		char *port;
		const skyeye_serial_device_t* serial_dev;
	}uart_term;
	struct fifo{
		char buf[FIFO_SIZE];
		uint32 start;
		uint32 end;
	}fifo;
	conf_object_t *signal;
	general_signal_intf *signal_iface;
	int int_number;
	int timer_scheduler_id;
	int dev_enable;
}leon2_uart_dev;

static char* regs_name[] = {
	"Data_Reg",
	"Status_Reg",
	"Control_reg",
	"Scaler_reg",
	"fp1_reg",
	"fp2_reg",
	NULL
};

#define DBG_LEON2_U	0
#define DBG_LEON2_UART(fmt, ...)      if(DBG_LEON2_U){					\
	                                        printf(fmt, ## __VA_ARGS__);	\
	                                        }
#define UART_DATA_REGISTER      0x0
#define UART_STATUS_REGISTER    0x4
#define UART_CONTROL_REGISTER   0x8
#define UART_SCALER_REGISTER    0xc
#define UART_FP1_REGISTER    0x10
#define UART_FP2_REGISTER    0x14

#define UART_NREGS  6

//**************Uart Status Register Bits************
#define  TH  0x4
#define  TS  0x2
//**************Uart Ctrol Register Bits************
#define RE  0x1
#define TE  0x2
#define RI 0x4
#define TI 0x8
#define PS 0x10
#define PE 0x20
#define FL 0x40
#define LB 0x80
#define EC 0x100

//************************Registers Define*****
#define  UDATA    dev->regs.data
#define  USTATUS  dev->regs.status.value
#define  UCONTROL dev->regs.control.value
#define  USCALER  dev->regs.scaler


#endif
