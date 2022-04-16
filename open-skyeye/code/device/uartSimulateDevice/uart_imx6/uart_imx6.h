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
* @file uart_imx6.c
* @brief imx6 uart module
* @author Michael.Kang blackfin.kang@gmail.com
* @version 0.1
* @date 2011-07-13
*/
/* Description: update uart about improvement from mpc8641d
 * author: eJim Lee
 * date 2014-02-27
 */

#ifndef __UARTimx6_H__
#define __UARTimx6_H__

#include <skyeye_types.h>
#include <skyeye_signal.h>
#include <memory_space.h>
#include "uart_imx6_fifo.h" 

typedef struct reg_urxd {
	uint32 RX_DATA		:8;
	uint32 Reserved		:2;
	uint32 PRERR		:1;
	uint32 BRK		    :1;
	uint32 FRMERR		:1;
	uint32 OVRRUN		:1;
	uint32 ERR		    :1;
	uint32 CHARRDY		:1;
	uint32 Reserved_h	:16;

}urxd_t;
union urxd_reg{
    uint32              all;
    urxd_t              bit;
};


typedef struct reg_ucr1 {
	uint32 UARTEN		:1;
	uint32 DOZE		    :1;
	uint32 ATDMAEN		:1;
	uint32 TXDMAEN		:1;
	uint32 SNDBRK		:1;
	uint32 RTSDEN		:1;
	uint32 TXMPTYEN		:1;
	uint32 IREN		    :1;
	uint32 RXDMAEN		:1;
	uint32 RRDYEN		:1;
	uint32 ICD		    :2;
	uint32 IDEN		    :1;
	uint32 TRDYEN		:1;
	uint32 ADBR		    :1;
	uint32 ADEN		    :1;
	uint32 Reserved		:16;
}ucr1_t;
union ucr1_reg{
    uint32              all;
    ucr1_t              bit;
};


typedef struct reg_ucr2 {
	uint32 SRST		    :1;
	uint32 RXEN		    :1;
	uint32 TXEN		    :1;
	uint32 ATEN		    :1;
	uint32 RTSEN		:1;
	uint32 WS		    :1;
	uint32 STPB		    :1;
	uint32 PROE		    :1;
	uint32 PREN		    :1;
	uint32 RTEC		    :2;
	uint32 ESCEN		:1;
	uint32 CTS		    :1;
	uint32 CTSC		    :1;
	uint32 IRTS		    :1;
	uint32 ESCI		    :1;
	uint32 Reserved		:16;

}ucr2_t;
union ucr2_reg{
    uint32              all;
    ucr2_t              bit;
};

typedef struct reg_ucr3 {
	uint32 ACIEN		:1;
	uint32 INVT		    :1;
	uint32 RXDMUXSEL	:1;
	uint32 DTRDEN		:1;
	uint32 AWAKEN		:1;
	uint32 AIRINTEN		:1;
	uint32 RXDSEN		:1;
	uint32 ADNIMP		:1;
	uint32 RI		    :1;
	uint32 DCD		    :1;
	uint32 DSR		    :1;
	uint32 FRAERREN		:1;
	uint32 PARERREN		:1;
	uint32 DTREN		:1;
	uint32 DPEC		    :2;
	uint32 Reserved		:16;

}ucr3_t;
union ucr3_reg{
    uint32              all;
    ucr3_t              bit;
};


typedef struct reg_ucr4 {
	uint32 DREN		    :1;
	uint32 OREN		    :1;
	uint32 BKEN		    :1;
	uint32 TCEN		    :1;
	uint32 LPBYP		:1;
	uint32 IRSC		    :1;
	uint32 IDDMAEN		:1;
	uint32 WKEN		    :1;
	uint32 ENIRI		:1;
	uint32 INVR		    :1;
	uint32 CTSTL		:6;
	uint32 Reserved		:16;

}ucr4_t;
union ucr4_reg{
    uint32              all;
    ucr4_t              bit;
};


typedef struct reg_ufcr {
	uint32 RXTL		    :6;
	uint32 DCEDTE		:1;
	uint32 RFDIV		:3;
	uint32 TXTL		    :6;
	uint32 Reserved		:16;

}ufcr_t;
union ufcr_reg{
    uint32              all;
    ufcr_t              bit;
};


typedef struct reg_usr1 {
	uint32 Reserved		:3;
	uint32 SAD		    :1;
	uint32 AWAKE		:1;
	uint32 AIRINT		:1;
	uint32 RXDS		    :1;
	uint32 DTRD		    :1;
	uint32 AGTIM		:1;
	uint32 RRDY		    :1;
	uint32 FRAMERR		:1;
	uint32 ESCF		    :1;
	uint32 RTSD		    :1;
	uint32 TRDY		    :1;
	uint32 RTSS		    :1;
	uint32 PARITYERR	:1;
	uint32 Reserved_h	:16;

}usr1_t;
union usr1_reg{
    uint32              all;
    usr1_t              bit;
};


typedef struct reg_usr2 {
	uint32 RDR		    :1;
	uint32 ORE		    :1;
	uint32 BRCD		    :1;
	uint32 TXDC		    :1;
	uint32 RTSF		    :1;
	uint32 DCDIN		:1;
	uint32 DCDDELT		:1;
	uint32 WAKE	    	:1;
	uint32 IRNT		    :1;
	uint32 RIIN		    :1;
	uint32 RIDELT		:1;
	uint32 ACST		    :1;
	uint32 IDLE		    :1;
	uint32 DTRF		    :1;
	uint32 TXFE		    :1;
	uint32 ADET		    :1;
	uint32 Reserved		:16;

}usr2_t;
union usr2_reg{
    uint32              all;
    usr2_t              bit;
};


typedef struct reg_uts {
	uint32 SOFTRST		:1;
	uint32 Reserved_1	:2;
	uint32 RXFULL		:1;
	uint32 TXFULL		:1;
	uint32 RXEMPTY		:1;
	uint32 TXEMPTY		:1;
	uint32 Reserved_2	:2;
	uint32 RXDBG		:1;
	uint32 LOOPIR		:1;
	uint32 DBGEN		:1;
	uint32 LOOP		    :1;
	uint32 FRCPERR		:1;
	uint32 Reserved_3	:2;
	uint32 Reserved_4	:16;

}uts_t;
union uts_reg{
    uint32              all;
    uts_t               bit;
};


typedef struct reg_umcr {
	uint32 MDEN		    :1;
	uint32 SLAM		    :1;
	uint32 TXB8		    :1;
	uint32 SADEN		:1;
	uint32 Reserved_1	:4;
	uint32 SLADDR		:8;
	uint32 Reserved		:16;

}umcr_t;
union umcr_reg{
    uint32              all;
    umcr_t              bit;
};


typedef struct reg_uart {
	union urxd_reg      urxd;
    uint32              utxd;
    union ucr1_reg      ucr1;
	union ucr2_reg      ucr2;
	union ucr3_reg      ucr3;
	union ucr4_reg      ucr4;
	union ufcr_reg      ufcr;
	union usr1_reg      usr1;
	union usr2_reg      usr2;
	uint32              uesc;
	uint32              utim;
	uint32              ubir;
	uint32              ubmr;
	uint32              ubrc;
	uint32              onems;
	union uts_reg       uts;
	union umcr_reg      umcr;
}reg_imx6_t;

typedef struct uart_imx6 {
	conf_object_t *obj;
	reg_imx6_t    reg;
	/* internal implement */
	memory_space_intf           io_memory;
	struct {
		conf_object_t           *obj;
		skyeye_uart_intf        *intf;
	}term;
	struct {
		int                     num;
		conf_object_t           *obj;
		general_signal_intf     *intf;
	}signal;
	uart_file_intf *uart_file_iface;
	conf_object_t *uart_file;
	FIFO *read_fifo;
}uart_imx6_t;

static char* regs_name[] = {
	"URXD",			/*UART Receiver Register*/
	"UTXD",			/*UART Transmitter Register*/
	"UCR1",			/*UART Control Register 1*/
	"UCR2",			/*UART Control Register 2*/
	"UCR3",			/*UART Control Register 3*/
	"UCR4",			/*UART Control Register 4*/
	"UFCR",			/*UART FIFO Control Register*/
	"USR1",			/*UART Status Register 1*/
	"USR2",			/*UART Status Register 2*/
	"UESC",			/*UART Escape Character Register*/
	"UTIM",			/*UART Escape Timer Register*/
	"UBIR",			/*UART BRM Incremental Register*/
	"UBMR",			/*UART BRM Modulator Register*/
	"UBRC",			/*UART Baud Rate Count Register*/
	"ONEMS",		/*UART One Millisecond Register*/
	"UTS",			/*UART Test Register*/
	"UMCR",			/*Mode Control Register*/
	NULL
};
#define	URXD		0x000
#define	UTXD		0x040
#define	UCR1		0x080
#define	UCR2		0x084
#define	UCR3		0x088
#define	UCR4		0x08c
#define	UFCR		0x090
#define	USR1		0x094
#define	USR2		0x098
#define	UESC		0x09c
#define	UTIM		0x0a0
#define	UBIR		0x0a4
#define	UBMR		0x0a8
#define	UBRC		0x0ac
#define	ONEMS		0x0b0
#define	UTS		    0x0b4
#define	UMCR		0x0b8

#endif
