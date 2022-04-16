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
* @file syscfg_omapl138.h
* @brief The definition of system controller for omapl138
* @author Michael.Kang blackfin.kang@gmail.com
* @version 78.77
* @date 2011-12-12
*/

#ifndef __SYSCFG_omapl138_H__
#define __SYSCFG_omapl138_H__
#include <stdint.h>

typedef struct syscfg_reg{
	uint32_t revid;
	uint32_t dieidr0;
	uint32_t dieidr1;
	uint32_t dieidr2;
	uint32_t dieidr3;
	uint32_t devidr0;
	uint32_t bootcfg;
	uint32_t kick0r;
	uint32_t kick1r;
	uint32_t host0cfg;
	uint32_t host1cfg;
	uint32_t irawstat;
	uint32_t ienstat;
	uint32_t ienset;
	uint32_t ienclr;
	uint32_t eoi;
	uint32_t fltaddr;
	uint32_t fltstat;
	uint32_t mstpri0;
	uint32_t mstpri1;
	uint32_t mstpri2;
	uint32_t pinmux[20];
	uint32_t suspsrc;
	uint32_t chipsig;
	uint32_t chipsig_clr;
	uint32_t cfgchip0;
	uint32_t cfgchip1;
	uint32_t cfgchip2;
	uint32_t cfgchip3;
	uint32_t cfgchip4;
	uint32_t vtpio_ctl;
	uint32_t ddr_slew;
	uint32_t deepsleep;
	uint32_t pupd_ena;
	uint32_t pupd_sel;
	uint32_t rxactive;
	uint32_t pwrdn;
}syscfg_reg_t;

static char *regs_name[] = {
	"revid",
	"dieidr0",
	"dieidr1",
	"dieidr2",
	"dieidr3",
	"devidr0",
	"bootcfg",
	"kick0r",
	"kick1r",
	"host0cfg",
	"host1cfg",
	"irawstat",
	"ienstat",
	"ienset",
	"ienclr",
	"eoi",
	"fltaddr",
	"fltstat",
	"mstpri0",
	"mstpri1",
	"mstpri2",
	"pinmux[0]",
	"pinmux[1]",
	"pinmux[2]",
	"pinmux[3]",
	"pinmux[4]",
	"pinmux[5]",
	"pinmux[6]",
	"pinmux[7]",
	"pinmux[8]",
	"pinmux[9]",
	"pinmux[10]",
	"pinmux[11]",
	"pinmux[12]",
	"pinmux[13]",
	"pinmux[14]",
	"pinmux[15]",
	"pinmux[16]",
	"pinmux[17]",
	"pinmux[18]",
	"pinmux[19]",
	"suspsrc",
	"chipsig",
	"chipsig_clr",
	"cfgchip0",
	"cfgchip1",
	"cfgchip2",
	"cfgchip3",
	"cfgchip4",
	"vtpio_ctl",
	"ddr_slew",
	"deepsleep",
	"pupd_ena",
	"pupd_sel",
	"rxactive",
	"pwrdn",
	NULL
};

typedef struct omapl138_syscfg_device{
	conf_object_t* obj;
	syscfg_reg_t* regs;
	struct {
		general_signal_intf *signal_iface;
		conf_object_t *signal;
	} arm_signal;
	struct {
		general_signal_intf *signal_iface;
		conf_object_t *signal;
	} c674x_signal;
	char **regs_name;
	uint32_t syscfg_inter;
}omapl138_syscfg_device;

#define REVID		0x000
#define DIEIDR0		0x008
#define DIEIDR1		0x00c
#define DIEIDR2		0x010
#define DIEIDR3		0x014
#define DEVIDR0		0x018
#define BOOTCFG		0x020
#define KICK0R		0x038
#define KICK1R		0x03c
#define HOST0CFG	0x040
#define HOST1CFG	0x044
#define IRAWSTAT	0x0e0
#define IENSTAT		0x0e4
#define IENSET		0x0e8
#define IENCLR		0x0ec
#define EOI		0x0f0
#define FLTADDR		0x0f4
#define FLTSTAT		0x0f8
#define MSTPRI0		0x110
#define MSTPRI1		0x114
#define MSTPRI2		0x118

#define PINMUX		0x120

#define SUSPSRC		0x170
#define CHIPSIG		0x174
#define CHIPSIG_CLR	0x178
#define CFGCHIP0	0x17c
#define CFGCHIP1	0x180
#define CFGCHIP2	0x184
#define CFGCHIP3	0x188
#define CFGCHIP4	0x18c

#define VTPIO_CTL	0x18000
#define DDR_SLEW	0x18004
#define DEEPSLEEP	0x18008
#define PUPD_ENA	0x1800c
#define PUPD_SEL	0x18010
#define RXACTIVE	0x18014
#define PWRDN		0x18018
#endif
