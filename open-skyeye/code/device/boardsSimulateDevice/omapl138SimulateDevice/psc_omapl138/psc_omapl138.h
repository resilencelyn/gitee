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
* @file psc_omapl138.h
* @brief The definition of system controller for omapl138
* @author Michael.Kang blackfin.kang@gmail.com
* @version 78.77
* @date 2011-12-12
*/

#ifndef __PSC_omapl138_H__
#define __PSC_omapl138_H__
#include <stdint.h>

typedef struct psc_reg{
	uint32_t revid;
	uint32_t inteval;
	uint32_t merrpr0;
	uint32_t merrcr0;
	uint32_t perrpr;
	uint32_t perrcr;

	/* I can't find this register on datasheet
	 * but linux kernel used this register */
	uint32_t epcpr;

	uint32_t ptcmd;
	uint32_t ptstat;
	uint32_t pdstat0;
	uint32_t pdstat1;
	uint32_t pdctl0;
	uint32_t pdctl1;
	uint32_t pdcfg0;
	uint32_t pdcfg1;
	uint32_t mdstat[32];
	uint32_t mdctl[32];
}psc_reg_t;

static char *regs_name[] = {
	"revid",
	"inteval",
	"merrpr0",
	"merrcr0",
	"perrpr",
	"perrcr",

	"epcpr",

	"ptcmd",
	"ptstat",
	"pdstat0",
	"pdstat1",
	"pdctl0",
	"pdctl1",
	"pdcfg0",
	"pdcfg1",
	"mdstat[0]",
	"mdstat[1]",
	"mdstat[2]",
	"mdstat[3]",
	"mdstat[4]",
	"mdstat[5]",
	"mdstat[6]",
	"mdstat[7]",
	"mdstat[8]",
	"mdstat[9]",
	"mdstat[10]",
	"mdstat[11]",
	"mdstat[12]",
	"mdstat[13]",
	"mdstat[14]",
	"mdstat[15]",
	"mdstat[16]",
	"mdstat[17]",
	"mdstat[18]",
	"mdstat[19]",
	"mdstat[20]",
	"mdstat[21]",
	"mdstat[22]",
	"mdstat[23]",
	"mdstat[24]",
	"mdstat[25]",
	"mdstat[26]",
	"mdstat[27]",
	"mdstat[28]",
	"mdstat[29]",
	"mdstat[30]",
	"mdstat[31]",
	"mdctl[0]",
	"mdctl[1]",
	"mdctl[2]",
	"mdctl[3]",
	"mdctl[4]",
	"mdctl[5]",
	"mdctl[6]",
	"mdctl[7]",
	"mdctl[8]",
	"mdctl[9]",
	"mdctl[10]",
	"mdctl[11]",
	"mdctl[12]",
	"mdctl[13]",
	"mdctl[14]",
	"mdctl[16]",
	"mdctl[17]",
	"mdctl[18]",
	"mdctl[19]",
	"mdctl[20]",
	"mdctl[21]",
	"mdctl[22]",
	"mdctl[23]",
	"mdctl[24]",
	"mdctl[25]",
	"mdctl[26]",
	"mdctl[27]",
	"mdctl[28]",
	"mdctl[29]",
	"mdctl[30]",
	"mdctl[31]",
	NULL
};

typedef struct omapl138_psc_device{
	conf_object_t* obj;
	psc_reg_t* regs;
	general_signal_intf *signal_iface;

	core_exec_intf *exec_iface;
	conf_object_t *c6k_obj;

	conf_object_t *signal;
	char **regs_name;
	//uint32_t sig_no;
	uint32_t psc_inter;
}omapl138_psc_device;

#define REVID		0x000
#define INTEVAL		0x018
#define MERRPR0		0x040
#define MERRCR0		0x050
#define PERRPR		0x060
#define PERRCR		0x068

/* I can't find this register on datasheet
 * but linux kernel used this register */
#define EPCPR		0x070

#define PTCMD		0x120
#define PTSTAT		0x128
#define PDSTAT0		0x200
#define PDSTAT1		0x204
#define PDCTL0		0x300
#define PDCTL1		0x304
#define PDCFG0		0x400
#define PDCFG1		0x404
#define MDSTAT		0x800
#define MDCTL		0xa00
#define MDEND		0xa7c
#endif
