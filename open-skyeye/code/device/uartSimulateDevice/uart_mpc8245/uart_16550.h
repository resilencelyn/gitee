/* Copyright (C)
* 2011 - Michael.Kang blackfin.kang@gmail.com
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*/
/**
* @file uart_16550.c
* @brief 16550 uart module
* @author Michael.Kang blackfin.kang@gmail.com
* @version 0.1
* @date 2011-07-13
*/
/* Description: update uart about improvement from mpc8641d
 * author: eJim Lee
 * date 2014-02-27
 */

#ifndef __UART16550_H__
#define __UART16550_H__

#include <skyeye_types.h>
#include <skyeye_signal.h>
#include <memory_space.h>

#define REGS_NAME(x) #x
#define REGS_NAME_LEN(reg, len) sizeof(REGS_NAME(reg[len]))
#define REGS_V_NAME_FUNC(reg, off) \
	list[major++] = REGS_NAME(reg);
#define REGS_A_NAME_FUNC(reg, off, n, size) \
	for(minor = 0; minor < n; minor++) { \
		list[major + minor] = malloc(REGS_NAME_LEN(reg, n)); \
		sprintf(list[major + minor], "%s[%d]", #reg, minor); \
	} \
	major += n;
#define REGS_NAME_INIT(LIST, name_list) \
	do{ \
		int major = 0; \
		int minor; \
		char **list = (char**) name_list; \
		LIST(REGS_V_NAME_FUNC, REGS_A_NAME_FUNC) \
	}while(0)

#define REGS_V_NUM(reg, off) 1 +
#define REGS_A_NUM(reg, off, n, intv) (n) +
#define REGS_NUM(LIST) (LIST(REGS_V_NUM, REGS_A_NUM) + 0)

/* data structure related to diu */

/* uart 16550 registers define list
 * contain improvement from mpc8641d
 * f(reg, offset) for variable
 * x(reg, offset, num, interval) for array
 */
#define UART_16550_REGS_LIST(f, x) \
	f(rtd, 0x0)  /* rbr/thr/dlb -> rtd
	                rbr LCR[DLAB] = 0 receiver buffer register (R)
	                thr LCR[DLAB] = 0 transmitter holding register (W)
	                dlb LCR[DLAB] = 1 divisor least significant byte register (RW) */ \
	f(id, 0x1)   /* ier/dmb -> id
	                ier LCR[DLAB] = 0 interrupt enable register (RW)
	                dmb LCR[DLAB] = 1 divisor most significant byte register (RW) */  \
	f(ifa, 0x2)  /* iir/fcr/afr -> ifa
	                iir LCR[DLAB] = 0 interrupt ID register (R)
	                fcr LCR[DLAB] = 0 FIFO control register (W)
	                afr LCR[DLAB] = 1 alternate function register (RW) */             \
	f(lcr, 0x3)  /* line control register (RW) */                                     \
	f(mcr, 0x4)  /* modem control register (RW) */                                    \
	f(lsr, 0x5)  /* line status register (R) */                                       \
	f(msr, 0x6)  /* modem status register (R) */                                      \
	f(scr, 0x7)  /* scratch register (RW) */                                          \
	f(dsr, 0x10) /* DMA status register (R) */

#define UART_16550_BANK_LEN 0x11

#define REGS_V_DEF(reg, off) uint8 reg;
#define REGS_A_DEF(reg, off, n, intv) uint8 reg[n];
typedef struct reg_16550 {
	UART_16550_REGS_LIST(REGS_V_DEF, REGS_A_DEF)
	/* rtd */
	uint8 rbr;
	uint8 thr;
	uint8 dlb;
	/* id */
	uint8 ier;
	uint8 dmb;
	/* ifa */
	uint8 iir;
	uint8 fcr;
	uint8 afr;
}reg_16550_t;

/* define register offset */
#define REGS_V_MACRO(reg, off) reg##_off = off,
#define REGS_A_MACRO(reg, off, n, intv) reg##_off = off,
enum {
	UART_16550_REGS_LIST(REGS_V_MACRO, REGS_A_MACRO)
};

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

/* define register name string list for gui */
const char *uart_16550_reg_name[REGS_NUM(UART_16550_REGS_LIST)];

typedef struct uart_16550 {
	conf_object_t *obj;
	reg_16550_t    reg;
	/* internal implement */
	memory_space_intf           io_memory;
	struct {
		conf_object_t          *obj;
		skyeye_uart_intf *intf;
	}term;
    conf_object_t *uart_file;
	uart_file_intf *uart_file_iface;
	struct {
		int                     num;
		conf_object_t          *obj;
		general_signal_intf    *intf;
	}signal;
	int dev_enable;
}uart_16550_t;

#endif
