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
* @file sysctrl_at91rm9200.h
* @brief The definition of system controller for at91rm9200
* @author Michael.Kang blackfin.kang@gmail.com
* @version 78.77
* @date 2011-12-12
*/

#ifndef __SYSCTRL_at91rm9200_H__
#define __SYSCTRL_at91rm9200_H__

typedef struct sysctrl_reg{
	uint32_t pmc_scer; /* enable system clk reg */
	uint32_t pmc_scdr; /* disable system clk reg */
	uint32_t pmc_scsr; /* system clk status reg */

	uint32_t pmc_pcer; /* peripheral clk enable */
	uint32_t pmc_pcdr; /* peripheral clk disable */
	uint32_t pmc_pcsr; /* peripheral clk status */

	uint32_t ckgr_mor; /* Main Oscillator Register */
	uint32_t ckgr_mcfr; /* Main Clock Frequency Register */
	uint32_t ckgr_pllar; /* PLL A Register */
	uint32_t ckgr_pllbr; /* PLL B Register */
	uint32_t pmc_mckr; /* Master Clock Register */

	uint32_t pmc_pck0;
	uint32_t pmc_pck1;
	uint32_t pmc_pck2;
	uint32_t pmc_pck3;

	uint32_t pmc_ier; /* Interrupt Enable Register */
	uint32_t pmc_idr; /* Interrupt Disable Register */
	uint32_t pmc_sr; /* Status Register */
	uint32_t pmc_imr; /* Interrupt Mask Register */
}sysctrl_reg_t;

static char *regs_name[] = {
	"pmc_scer",
	"pmc_scdr",
	"pmc_scsr",
	"pmc_pcer",
	"pmc_pcdr",
	"pmc_pcsr",
	"ckgr_mor",
	"ckgr_mcfr",
	"ckgr_pllar",
	"ckgr_pllbr",
	"pmc_mckr",
	"pmc_pck0",
	"pmc_pck1",
	"pmc_pck2",
	"pmc_pck3",
	"pmc_ier",
	"pmc_idr",
	"pmc_sr",
	"pmc_imr",
	NULL
};

typedef struct at91rm9200_sysctrl_device{
	conf_object_t* obj;
	sysctrl_reg_t* regs;
	general_signal_intf *signal_iface;
	conf_object_t *signal;
	char **regs_name;
	uint32_t sig_no;
}at91rm9200_sysctrl_device;

#define PMC_SCER 		(0x0000)
#define PMC_SCDR 		(0x0004)
#define PMC_SCSR 		(0x0008)
#define PMC_PCER 		(0x0010)
#define PMC_PCDR 		(0x0014)
#define PMC_PCSR 		(0x0018)

#define CKGR_MOR 		(0x0020)
#define CKGR_MCFR 		(0x0024)
#define CKGR_PLLAR 		(0x0028)
#define CKGR_PLLBR 		(0x002c)
#define PMC_MCKR 		(0x0030)

#define PMC_PCK0 		(0x0040)
#define PMC_PCK1 		(0x0044)
#define PMC_PCK2 		(0x0048)
#define PMC_PCK3 		(0x004c)

#define PMC_IER 		(0x0060)
#define PMC_IDR 		(0x0064)
#define PMC_SR 			(0x0068)
#define PMC_IMR 		(0x006c)
#endif
