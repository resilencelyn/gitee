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
* @file gpio_at91rm9200.h
* @brief The definition of system controller for at91rm9200
* @author Michael.Kang blackfin.kang@gmail.com
* @version 78.77
* @date 2011-12-12
*/

#ifndef __SYSCTRL_at91rm9200_H__
#define __SYSCTRL_at91rm9200_H__

#define PIONUMS 4
typedef struct gpio_reg{
	uint32_t pio_per;
	uint32_t pio_pdr;
	uint32_t pio_psr;

	uint32_t pio_oer;
	uint32_t pio_odr;
	uint32_t pio_osr;

	uint32_t pio_ifer;
	uint32_t pio_ifdr;
	uint32_t pio_ifsr;

	uint32_t pio_sodr;
	uint32_t pio_codr;
	uint32_t pio_odsr;
	uint32_t pio_pdsr;

	uint32_t pio_ier;
	uint32_t pio_idr;
	uint32_t pio_imr;
	uint32_t pio_isr;

	uint32_t pio_mder;
	uint32_t pio_mddr;
	uint32_t pio_mdsr;

	uint32_t pio_pudr;
	uint32_t pio_puer;
	uint32_t pio_pusr;

	uint32_t pio_asr;
	uint32_t pio_bsr;
	uint32_t pio_absr;

	uint32_t pio_ower;
	uint32_t pio_owdr;
	uint32_t pio_owsr;
}gpio_reg_t;

static char* regs_name[] = {
	"pioa_per",
	"pioa_pdr",
	"pioa_psr",

	"pioa_oer",
	"pioa_odr",
	"pioa_osr",

	"pioa_ifer",
	"pioa_ifdr",
	"pioa_ifsr",

	"pioa_sodr",
	"pioa_codr",
	"pioa_odsr",
	"pioa_pdsr",

	"pioa_ier",
	"pioa_idr",
	"pioa_imr",
	"pioa_isr",

	"pioa_mder",
	"pioa_mddr",
	"pioa_mdsr",

	"pioa_pudr",
	"pioa_puer",
	"pioa_pusr",

	"pioa_asr",
	"pioa_bsr",
	"pioa_absr",

	"pioa_ower",
	"pioa_owdr",
	"pioa_owsr",

	"piob_per",
	"piob_pdr",
	"piob_psr",

	"piob_oer",
	"piob_odr",
	"piob_osr",

	"piob_ifer",
	"piob_ifdr",
	"piob_ifsr",

	"piob_sodr",
	"piob_codr",
	"piob_odsr",
	"piob_pdsr",

	"piob_ier",
	"piob_idr",
	"piob_imr",
	"piob_isr",

	"piob_mder",
	"piob_mddr",
	"piob_mdsr",

	"piob_pudr",
	"piob_puer",
	"piob_pusr",

	"piob_asr",
	"piob_bsr",
	"piob_absr",

	"piob_ower",
	"piob_owdr",
	"piob_owsr",

	"pioc_per",
	"pioc_pdr",
	"pioc_psr",

	"pioc_oer",
	"pioc_odr",
	"pioc_osr",

	"pioc_ifer",
	"pioc_ifdr",
	"pioc_ifsr",

	"pioc_sodr",
	"pioc_codr",
	"pioc_odsr",
	"pioc_pdsr",

	"pioc_ier",
	"pioc_idr",
	"pioc_imr",
	"pioc_isr",

	"pioc_mder",
	"pioc_mddr",
	"pioc_mdsr",

	"pioc_pudr",
	"pioc_puer",
	"pioc_pusr",

	"pioc_asr",
	"pioc_bsr",
	"pioc_absr",

	"pioc_ower",
	"pioc_owdr",
	"pioc_owsr",

	"piod_per",
	"piod_pdr",
	"piod_psr",

	"piod_oer",
	"piod_odr",
	"piod_osr",

	"piod_ifer",
	"piod_ifdr",
	"piod_ifsr",

	"piod_sodr",
	"piod_codr",
	"piod_odsr",
	"piod_pdsr",

	"piod_ier",
	"piod_idr",
	"piod_imr",
	"piod_isr",

	"piod_mder",
	"piod_mddr",
	"piod_mdsr",

	"piod_pudr",
	"piod_puer",
	"piod_pusr",

	"piod_asr",
	"piod_bsr",
	"piod_absr",

	"piod_ower",
	"piod_owdr",
	"piod_owsr",
	 NULL
};

#define PIO_PER		(0x0000)
#define PIO_PDR 	(0x0004)
#define PIO_PSR 	(0x0008)

#define PIO_OER		(0x0010)
#define PIO_ODR		(0x0014)
#define PIO_OSR		(0x0018)

#define PIO_IFER	(0x0020)
#define PIO_IFDR	(0x0024)
#define PIO_IFSR	(0x0028)

#define PIO_SODR	(0x0030)
#define PIO_CODR	(0x0034)
#define PIO_ODSR	(0x0038)
#define PIO_PDSR	(0x003c)

#define PIO_IER		(0x0040)
#define PIO_IDR		(0x0044)
#define PIO_IMR		(0x0048)
#define PIO_ISR		(0x004c)

#define PIO_MDER	(0x0050)
#define PIO_MDDR	(0x0054)
#define PIO_MDSR	(0x0058)

#define PIO_PUDR	(0x0060)
#define PIO_PUER	(0x0064)
#define PIO_PUSR	(0x0068)

#define PIO_ASR		(0x0070)
#define PIO_BSR		(0x0074)
#define PIO_ABSR	(0x0078)

#define PIO_OWER	(0x00a0)
#define PIO_OWDR	(0x00a4)
#define PIO_OWSR	(0x00a8)



typedef struct at91rm9200_gpio_device{
	conf_object_t* obj;
	gpio_reg_t regs[PIONUMS];
	int line_no;
	conf_object_t* signal_target;
	general_signal_intf* master;
	char** regs_name;
}at91rm9200_gpio_device;

#endif
