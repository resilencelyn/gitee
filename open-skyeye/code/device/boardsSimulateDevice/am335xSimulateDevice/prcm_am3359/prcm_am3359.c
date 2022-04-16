/* Copyright (C)
*
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
* @file prcm_am3359.c
* @brief The implementation of system controller
* @author
* @version 78.77
*/

#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#define DEBUG
#include <skyeye_log.h>

#include "prcm_am3359.h"

#define IDLEST (0x2 << 16)

#define CHANGE_CLKCTRL(val) { \
	int enable = val & 0x3; \
	val &= ~(0x3 << 16); \
	val = enable ? (val | IDLEST) : (val | (0x3 << 16));}
static void init_regs(prcm_am3359_device *dev)
{
	memset(dev->regs, 0, sizeof(prcm_am3359_reg_t));

	/* the corrent to set XXX_CLKCTRL IDLEST[16:17] is based on <module>_SYSCONFIG.SIDLEMODE bit.
	 * here, we force XXX_CLKCTRL IDLEST[16:17] is IDLEST status. Also XXX_CLKCTRL STBYST bit
	 * reference am3359 datasheet 8.1.3.2.1 and 8.1.3.2.2 */

	dev->regs->per.l4ls_clkstctrl			= 0x000c0102;
	dev->regs->per.l3s_clkstctrl			= 0x0000000a;
	dev->regs->per.l3_clkctrl			= 0x00030012 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.cpgmac0_clkctrl			= 0x00070000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.lcdc_clkctrl			= 0x00070000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.usb0_clkctrl			= 0x00070000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.tptc0_clkctrl			= 0x00070000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.emif_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.ocmcram_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.gpmc_clkctrl			= 0x00030002 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.mcasp0_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.uart5_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.mmc0_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.elm_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.i2c2_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.i2c1_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.spi0_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.spi1_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.l4ls_clkctrl			= 0x00000002 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.mcasp1_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.uart1_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.uart2_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.uart3_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.uart4_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.timer7_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.timer2_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.timer3_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.timer4_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.gpio1_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.gpio2_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.gpio3_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.tpcc_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.dcan0_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.dcan1_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.epwmss1_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.epwmss0_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.epwmss2_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.l3_instr_clkctrl			= 0x00000002 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.l3_clkctrl			= 0x00000002 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.ieee5000_clkctrl			= 0x00070002 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.pru_icss_clkctrl			= 0x00070000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.timer5_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.timer6_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.mmc1_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.mmc2_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.tptc1_clkctrl			= 0x00070000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.tptc2_clkctrl			= 0x00070000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.spinlock_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.mailbox0_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.l4hs_clkstctrl			= 0x0000007a;
	dev->regs->per.l4hs_clkctrl			= 0x00000002 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.ocpwp_l3_clkstctrl		= 0x00000002;
	dev->regs->per.ocpwp_clkctrl			= 0x00070002 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.pru_icss_clkstctrl		= 0x00000002;
	dev->regs->per.cpsw_clkstctrl			= 0x00000002;
	dev->regs->per.lcdc_clkstctrl			= 0x00000002;
	dev->regs->per.clkdiv32k_clkctrl		= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->per.clk_24mhz_clkstctrl		= 0x00000002;


	dev->regs->wkup.cm_wkup_clkstctrl			= 0x00000006;
	dev->regs->wkup.cm_wkup_control_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->wkup.cm_wkup_gpio0_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->wkup.cm_wkup_l4wkup_clkctrl			= 0x00000002 & ~(0x3 << 16) | IDLEST;
	dev->regs->wkup.cm_wkup_timer0_clkctrl			= 0x00030002 & ~(0x3 << 16) | IDLEST;
	dev->regs->wkup.cm_wkup_debugss_clkctrl			= 0x52580002 & ~(0x3 << 16) | IDLEST;
	dev->regs->wkup.cm_l3_aon_clkstctrl			= 0x0000001a;
	dev->regs->wkup.cm_div_m4_dpll_core			= 0x00000004;
	dev->regs->wkup.cm_div_m5_dpll_core			= 0x00000004;
	dev->regs->wkup.cm_clkmode_dpll_mpu			= 0x00000004;
	dev->regs->wkup.cm_clkmode_dpll_per			= 0x00000004;
	dev->regs->wkup.cm_clkmode_dpll_core			= 0x00000004;
	dev->regs->wkup.cm_clkmode_dpll_ddr			= 0x00000004;
	dev->regs->wkup.cm_clkmode_dpll_disp			= 0x00000004;
	dev->regs->wkup.cm_div_m2_dpll_ddr			= 0x00000001;
	dev->regs->wkup.cm_div_m2_dpll_disp			= 0x00000001;
	dev->regs->wkup.cm_div_m2_dpll_mpu			= 0x00000001;
	dev->regs->wkup.cm_div_m2_dpll_per			= 0x00000001;
	dev->regs->wkup.cm_wkup_wkup_m3_clkctrl			= 0x00000002 & ~(0x3 << 16) | IDLEST;
	dev->regs->wkup.cm_wkup_uart0_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->wkup.cm_wkup_i2c0_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->wkup.cm_wkup_adc_tsc_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->wkup.cm_wkup_smartreflex0_clkctrl		= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->wkup.cm_wkup_timer1_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->wkup.cm_wkup_smartreflex1_clkctrl		= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->wkup.cm_l4_wkup_aon_clkstctrl		= 0x00000006;
	dev->regs->wkup.cm_wkup_wdt1_clkctrl			= 0x00030002 & ~(0x3 << 16) | IDLEST;
	dev->regs->wkup.cm_div_m6_dpll_core			= 0x00000004;

	dev->regs->dpll.clksel_timer7_clk			= 0x00000001;
	dev->regs->dpll.clksel_timer2_clk			= 0x00000001;
	dev->regs->dpll.clksel_timer3_clk			= 0x00000001;
	dev->regs->dpll.clksel_timer4_clk			= 0x00000001;
	dev->regs->dpll.cm_mac_clksel				= 0x00000004;
	dev->regs->dpll.clksel_timer5_clk			= 0x00000001;
	dev->regs->dpll.clksel_timer6_clk			= 0x00000001;
	dev->regs->dpll.clksel_wdt1_clk				= 0x00000001;

	dev->regs->mpu.cm_mpu_clkstctrl				= 0x00000006;
	dev->regs->mpu.cm_mpu_mpu_clkctrl			= 0x00000002 & ~(0x3 << 16) | IDLEST;

	dev->regs->rtc.cm_rtc_rtc_clkctrl			= 0x00030002 & ~(0x3 << 16) | IDLEST;
	dev->regs->rtc.cm_rtc_clkstctrl				= 0x00000004;

	dev->regs->gfx.cm_gfx_l3_clkstctrl			= 0x00000002;
	dev->regs->gfx.cm_gfx_gfx_clkctrl			= 0x00070000 & ~(0x3 << 16) | IDLEST;
	dev->regs->gfx.cm_gfx_l4ls_gfx_clkstctrl		= 0x00000102;
	dev->regs->gfx.cm_gfx_mmucfg_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;
	dev->regs->gfx.cm_gfx_mmudata_clkctrl			= 0x00030000 & ~(0x3 << 16) | IDLEST;

	dev->regs->cefuse.cm_cefuse_clkstctrl			= 0x00000002;
	dev->regs->cefuse.cm_cefuse_cefuse_clkctrl		= 0x00030000 & ~(0x3 << 16) | IDLEST;

	dev->regs->prm_per.rm_per_rstctrl			= 0x00000002;
	dev->regs->prm_per.pm_per_pwrstst			= 0x01e60007;
	dev->regs->prm_per.pm_per_pwrstctrl			= 0xee0000eb;

	dev->regs->prm_wkup.rm_wkup_rstctrl			= 0x00000008;
	dev->regs->prm_wkup.pm_wkup_pwrstctrl			= 0x00000008;
	dev->regs->prm_wkup.pm_wkup_pwrstst			= 0x00060004;

	dev->regs->prm_mpu.pm_mpu_pwrstctrl			= 0x01ff0007;
	dev->regs->prm_mpu.pm_mpu_pwrstst			= 0x00000157;

	dev->regs->prm_device.prm_rsttime			= 0x00001006;
	dev->regs->prm_device.prm_rstst				= 0x00000001;
	dev->regs->prm_device.prm_sram_count			= 0x78000017;

	dev->regs->prm_rtc.pm_rtc_pwrstctrl			= 0x00000004;
	dev->regs->prm_rtc.pm_rtc_pwrstst			= 0x00000004;

	dev->regs->prm_gfx.pm_gfx_pwrstctrl			= 0x00060044;
	dev->regs->prm_gfx.rm_gfx_rstctrl			= 0x00000001;
	dev->regs->prm_gfx.pm_gfx_pwrstst			= 0x00000017;

	dev->regs->prm_cefuse.pm_cefuse_pwrstst			= 0x00000007;
}
static exception_t prcm_am3359_read(conf_object_t *opaque,
		generic_address_t offset, void* buf, size_t count)
{
	prcm_am3359_device *dev = opaque->obj;
	prcm_am3359_reg_t* regs = dev->regs;
	uint32_t *val = (uint32_t *)buf;

	switch (offset) {
		case CM_PER_L4LS_CLKSTCTRL:
			*val = regs->per.l4ls_clkstctrl;
			break;
		case CM_PER_L3S_CLKSTCTRL:
			*val = regs->per.l3s_clkstctrl;
			break;
#if 0
		case CM_PER_L4FW_CLKSTCTRL:
			*val = regs->per.l4fw_clkstctrl;
			break;
#endif
		case CM_PER_L3_CLKSTCTRL:
			*val = regs->per.l3_clkstctrl;
			break;
		case CM_PER_CPGMAC0_CLKCTRL:
			*val = regs->per.cpgmac0_clkctrl;
			break;
		case CM_PER_LCDC_CLKCTRL:
			*val = regs->per.lcdc_clkctrl;
			break;
		case CM_PER_USB0_CLKCTRL:
			*val = regs->per.usb0_clkctrl;
			break;
#if 0
		case CM_PER_MLB_CLKCTRL:
			*val = regs->per.mlb_clkctrl;
			break;
#endif
		case CM_PER_TPTC0_CLKCTRL:
			*val = regs->per.tptc0_clkctrl;
			break;
		case CM_PER_EMIF_CLKCTRL:
			*val = regs->per.emif_clkctrl;
			break;
		case CM_PER_OCMCRAM_CLKCTRL:
			*val = regs->per.ocmcram_clkctrl;
			break;
		case CM_PER_GPMC_CLKCTRL:
			*val = regs->per.gpmc_clkctrl;
			break;
		case CM_PER_MCASP0_CLKCTRL:
			*val = regs->per.mcasp0_clkctrl;
			break;
		case CM_PER_UART5_CLKCTRL:
			*val = regs->per.uart5_clkctrl;
			break;
		case CM_PER_MMC0_CLKCTRL:
			*val = regs->per.mmc0_clkctrl;
			break;
		case CM_PER_ELM_CLKCTRL:
			*val = regs->per.elm_clkctrl;
			break;
		case CM_PER_I2C2_CLKCTRL:
			*val = regs->per.i2c2_clkctrl;
			break;
		case CM_PER_I2C1_CLKCTRL:
			*val = regs->per.i2c1_clkctrl;
			break;
		case CM_PER_SPI0_CLKCTRL:
			*val = regs->per.spi0_clkctrl;
			break;
		case CM_PER_SPI1_CLKCTRL:
			*val = regs->per.spi1_clkctrl;
			break;
		case CM_PER_L4LS_CLKCTRL:
			*val = regs->per.l4ls_clkctrl;
			break;
#if 0
		case CM_PER_L4FW_CLKCTRL:
			*val = regs->per.l4fw_clkctrl;
			break;
#endif
		case CM_PER_MCASP1_CLKCTRL:
			*val = regs->per.mcasp1_clkctrl;
			break;
		case CM_PER_UART1_CLKCTRL:
			*val = regs->per.uart1_clkctrl;
			break;
		case CM_PER_UART2_CLKCTRL:
			*val = regs->per.uart2_clkctrl;
			break;
		case CM_PER_UART3_CLKCTRL:
			*val = regs->per.uart3_clkctrl;
			break;
		case CM_PER_UART4_CLKCTRL:
			*val = regs->per.uart4_clkctrl;
			break;
		case CM_PER_TIMER7_CLKCTRL:
			*val = regs->per.timer7_clkctrl;
			break;
		case CM_PER_TIMER2_CLKCTRL:
			*val = regs->per.timer2_clkctrl;
			break;
		case CM_PER_TIMER3_CLKCTRL:
			*val = regs->per.timer3_clkctrl;
			break;
		case CM_PER_TIMER4_CLKCTRL:
			*val = regs->per.timer4_clkctrl;
			break;
#if 0
		case CM_PER_RNG_CLKCTRL:
			*val = regs->per.rng_clkctrl;
			break;
		case CM_PER_AES0_CLKCTRL:
			*val = regs->per.aes0_clkctrl;
			break;
		case CM_PER_SHA0_CLKCTRL:
			*val = regs->per.sha0_clkctrl;
			break;
		case CM_PER_PKA_CLKCTRL:
			*val = regs->per.pka_clkctrl;
			break;
		case CM_PER_GPIO6_CLKCTRL:
			*val = regs->per.gpio6_clkctrl;
			break;
#endif
		case CM_PER_GPIO1_CLKCTRL:
			*val = regs->per.gpio1_clkctrl;
			break;
		case CM_PER_GPIO2_CLKCTRL:
			*val = regs->per.gpio2_clkctrl;
			break;
		case CM_PER_GPIO3_CLKCTRL:
			*val = regs->per.gpio3_clkctrl;
			break;
		case CM_PER_TPCC_CLKCTRL:
			*val = regs->per.tpcc_clkctrl;
			break;
		case CM_PER_DCAN0_CLKCTRL:
			*val = regs->per.dcan0_clkctrl;
			break;
		case CM_PER_DCAN1_CLKCTRL:
			*val = regs->per.dcan1_clkctrl;
			break;
		case CM_PER_EPWMSS1_CLKCTRL:
			*val = regs->per.epwmss1_clkctrl;
			break;
#if 0
		case CM_PER_EMIF_FW_CLKCTRL:
			*val = regs->per.emif_fw_clkctrl;
			break;
#endif
		case CM_PER_EPWMSS0_CLKCTRL:
			*val = regs->per.epwmss0_clkctrl;
			break;
		case CM_PER_EPWMSS2_CLKCTRL:
			*val = regs->per.epwmss2_clkctrl;
			break;
		case CM_PER_L3_INSTR_CLKCTRL:
			*val = regs->per.l3_instr_clkctrl;
			break;
		case CM_PER_L3_CLKCTRL:
			*val = regs->per.l3_clkctrl;
			break;
		case CM_PER_IEEE5000_CLKCTRL:
			*val = regs->per.ieee5000_clkctrl;
			break;
		case CM_PER_ICSS_CLKCTRL:
			*val = regs->per.pru_icss_clkctrl;
			break;
		case CM_PER_TIMER5_CLKCTRL:
			*val = regs->per.timer5_clkctrl;
			break;
		case CM_PER_TIMER6_CLKCTRL:
			*val = regs->per.timer6_clkctrl;
			break;
		case CM_PER_MMC1_CLKCTRL:
			*val = regs->per.mmc1_clkctrl;
			break;
		case CM_PER_MMC2_CLKCTRL:
			*val = regs->per.mmc2_clkctrl;
			break;
		case CM_PER_TPTC1_CLKCTRL:
			*val = regs->per.tptc1_clkctrl;
			break;
		case CM_PER_TPTC2_CLKCTRL:
			*val = regs->per.tptc2_clkctrl;
			break;
		case CM_PER_SPINLOCK_CLKCTRL:
			*val = regs->per.spinlock_clkctrl;
			break;
		case CM_PER_MAILBOX0_CLKCTRL:
			*val = regs->per.mailbox0_clkctrl;
			break;
		case CM_PER_L4HS_CLKSTCTRL:
			*val = regs->per.l4hs_clkstctrl;
			break;
		case CM_PER_L4HS_CLKCTRL:
			*val = regs->per.l4hs_clkctrl;
			break;
#if 0
		case CM_PER_MSTR_EXPS_CLKCTRL:
			*val = regs->per.mstr_exps_clkctrl;
			break;
		case CM_PER_SLV_EXPS_CLKCTRL:
			*val = regs->per.slv_exps_clkctrl;
			break;
#endif
		case CM_PER_OCPWP_L3_CLKSTCTRL:
			*val = regs->per.ocpwp_l3_clkstctrl;
			break;
		case CM_PER_OCPWP_CLKCTRL:
			*val = regs->per.ocpwp_clkctrl;
			break;
		case CM_PER_ICSS_CLKSTCTRL:
			*val = regs->per.pru_icss_clkstctrl;
			break;
		case CM_PER_CPSW_CLKSTCTRL:
			*val = regs->per.cpsw_clkstctrl;
			break;
		case CM_PER_LCDC_CLKSTCTRL:
			*val = regs->per.lcdc_clkstctrl;
			break;
		case CM_PER_CLKDIV32K_CLKCTRL:
			*val = regs->per.clkdiv32k_clkctrl;
			break;
		case CM_PER_CLK_24MHZ_CLKSTCTRL:
			*val = regs->per.clk_24mhz_clkstctrl;
			break;
		case CM_WKUP_CLKSTCTRL:
			*val = regs->wkup.cm_wkup_clkstctrl;
			break;
		case CM_WKUP_CONTROL_CLKCTRL:
			*val = regs->wkup.cm_wkup_control_clkctrl;
			break;
		case CM_WKUP_GPIO0_CLKCTRL:
			*val = regs->wkup.cm_wkup_gpio0_clkctrl;
			break;
		case CM_WKUP_L4WKUP_CLKCTRL:
			*val = regs->wkup.cm_wkup_l4wkup_clkctrl;
			break;
		case CM_WKUP_TIMER0_CLKCTRL:
			*val = regs->wkup.cm_wkup_timer0_clkctrl;
			break;
		case CM_WKUP_DEBUGSS_CLKCTRL:
			*val = regs->wkup.cm_wkup_debugss_clkctrl;
			break;
		case CM_L3_AON_CLKSTCTRL:
			*val = regs->wkup.cm_l3_aon_clkstctrl;
			break;
		case CM_AUTOIDLE_DPLL_MPU:
			*val = regs->wkup.cm_autoidle_dpll_mpu;
			break;
		case CM_IDLEST_DPLL_MPU:
			*val = regs->wkup.cm_idlest_dpll_mpu;
			break;
		case CM_SSC_DELTAMSTEP_DPLL_MPU:
			*val = regs->wkup.cm_ssc_deltamstep_dpll_mpu;
			break;
		case CM_SSC_MODFREQDIV_DPLL_MPU:
			*val = regs->wkup.cm_ssc_modfreqdiv_dpll_mpu;
			break;
		case CM_CLKSEL_DPLL_MPU:
			*val = regs->wkup.cm_clksel_dpll_mpu;
			break;
		case CM_AUTOIDLE_DPLL_DDR:
			*val = regs->wkup.cm_autoidle_dpll_ddr;
			break;
		case CM_IDLEST_DPLL_DDR:
			*val = regs->wkup.cm_idlest_dpll_ddr;
			break;
		case CM_SSC_DELTAMSTEP_DPLL_DDR:
			*val = regs->wkup.cm_ssc_deltamstep_dpll_ddr;
			break;
		case CM_SSC_MODFREQDIV_DPLL_DDR:
			*val = regs->wkup.cm_ssc_modfreqdiv_dpll_ddr;
			break;
		case CM_CLKSEL_DPLL_DDR:
			*val = regs->wkup.cm_clksel_dpll_ddr;
			break;
		case CM_AUTOIDLE_DPLL_DISP:
			*val = regs->wkup.cm_autoidle_dpll_disp;
			break;
		case CM_IDLEST_DPLL_DISP:
			*val = regs->wkup.cm_idlest_dpll_disp;
			break;
		case CM_SSC_DELTAMSTEP_DPLL_DISP:
			*val = regs->wkup.cm_ssc_deltamstep_dpll_disp;
			break;
		case CM_SSC_MODFREQDIV_DPLL_DISP:
			*val = regs->wkup.cm_ssc_modfreqdiv_dpll_disp;
			break;
		case CM_CLKSEL_DPLL_DISP:
			*val = regs->wkup.cm_clksel_dpll_disp;
			break;
		case CM_AUTOIDLE_DPLL_CORE:
			*val = regs->wkup.cm_autoidle_dpll_core;
			break;
		case CM_IDLEST_DPLL_CORE:
			*val = regs->wkup.cm_idlest_dpll_core;
			break;
		case CM_SSC_DELTAMSTEP_DPLL_CORE:
			*val = regs->wkup.cm_ssc_deltamstep_dpll_core;
			break;
		case CM_SSC_MODFREQDIV_DPLL_CORE:
			*val = regs->wkup.cm_ssc_modfreqdiv_dpll_core;
			break;
		case CM_CLKSEL_DPLL_CORE:
			*val = regs->wkup.cm_clksel_dpll_core;
			break;
		case CM_AUTOIDLE_DPLL_PER:
			*val = regs->wkup.cm_autoidle_dpll_per;
			break;
		case CM_IDLEST_DPLL_PER:
			*val = regs->wkup.cm_idlest_dpll_per;
			break;
		case CM_SSC_DELTAMSTEP_DPLL_PER:
			*val = regs->wkup.cm_ssc_deltamstep_dpll_per;
			break;
		case CM_SSC_MODFREQDIV_DPLL_PER:
			*val = regs->wkup.cm_ssc_modfreqdiv_dpll_per;
			break;
		case CM_CLKDCOLDO_DPLL_PER:
			*val = regs->wkup.cm_clkdcoldo_dpll_per;
			break;
		case CM_DIV_M4_DPLL_CORE:
			*val = regs->wkup.cm_div_m4_dpll_core;
			break;
		case CM_DIV_M5_DPLL_CORE:
			*val = regs->wkup.cm_div_m5_dpll_core;
			break;
		case CM_CLKMODE_DPLL_MPU:
			*val = regs->wkup.cm_clkmode_dpll_mpu;
			break;
		case CM_CLKMODE_DPLL_PER:
			*val = regs->wkup.cm_clkmode_dpll_per;
			break;
		case CM_CLKMODE_DPLL_CORE:
			*val = regs->wkup.cm_clkmode_dpll_core;
			break;
		case CM_CLKMODE_DPLL_DDR:
			*val = regs->wkup.cm_clkmode_dpll_ddr;
			break;
		case CM_CLKMODE_DPLL_DISP:
			*val = regs->wkup.cm_clkmode_dpll_disp;
			break;
		case CM_CLKSEL_DPLL_PERIPH:
			*val = regs->wkup.cm_clksel_dpll_periph;
			break;
		case CM_DIV_M2_DPLL_DDR:
			*val = regs->wkup.cm_div_m2_dpll_ddr;
			break;
		case CM_DIV_M2_DPLL_DISP:
			*val = regs->wkup.cm_div_m2_dpll_disp;
			break;
		case CM_DIV_M2_DPLL_MPU:
			*val = regs->wkup.cm_div_m2_dpll_mpu;
			break;
		case CM_DIV_M2_DPLL_PER:
			*val = regs->wkup.cm_div_m2_dpll_per;
			break;
		case CM_WKUP_WKUP_M3_CLKCTRL:
			*val = regs->wkup.cm_wkup_wkup_m3_clkctrl;
			break;
		case CM_WKUP_UART0_CLKCTRL:
			*val = regs->wkup.cm_wkup_uart0_clkctrl;
			break;
		case CM_WKUP_I2C0_CLKCTRL:
			*val = regs->wkup.cm_wkup_i2c0_clkctrl;
			break;
		case CM_WKUP_ADC_TSC_CLKCTRL:
			*val = regs->wkup.cm_wkup_adc_tsc_clkctrl;
			break;
		case CM_WKUP_SMARTREFLEX0_CLKCTRL:
			*val = regs->wkup.cm_wkup_smartreflex0_clkctrl;
			break;
		case CM_WKUP_TIMER1_CLKCTRL:
			*val = regs->wkup.cm_wkup_timer1_clkctrl;
			break;
		case CM_WKUP_SMARTREFLEX1_CLKCTRL:
			*val = regs->wkup.cm_wkup_smartreflex1_clkctrl;
			break;
		case CM_L4_WKUP_AON_CLKSTCTRL:
			*val = regs->wkup.cm_l4_wkup_aon_clkstctrl;
			break;
		case CM_WKUP_WDT1_CLKCTRL:
			*val = regs->wkup.cm_wkup_wdt1_clkctrl;
			break;
		case CM_DIV_M6_DPLL_CORE:
			*val = regs->wkup.cm_div_m6_dpll_core;
			break;
		case CM_DPLL_CLKSEL_TIMER7_CLK:
			*val = regs->dpll.clksel_timer7_clk;
			break;
		case CM_DPLL_CLKSEL_TIMER2_CLK:
			*val = regs->dpll.clksel_timer2_clk;
			break;
		case CM_DPLL_CLKSEL_TIMER3_CLK:
			*val = regs->dpll.clksel_timer3_clk;
			break;
		case CM_DPLL_CLKSEL_TIMER4_CLK:
			*val = regs->dpll.clksel_timer4_clk;
			break;
		case CM_DPLL_CM_MAC_CLKSEL:
			*val = regs->dpll.cm_mac_clksel;
			break;
		case CM_DPLL_CLKSEL_TIMER5_CLK:
			*val = regs->dpll.clksel_timer5_clk;
			break;
		case CM_DPLL_CLKSEL_TIMER6_CLK:
			*val = regs->dpll.clksel_timer6_clk;
			break;
		case CM_DPLL_CM_CPTS_RFT_CLKSEL:
			*val = regs->dpll.cm_cpts_rft_clksel;
			break;
		case CM_DPLL_CLKSEL_TIMER1MS_CLK:
			*val = regs->dpll.clksel_timer1ms_clk;
			break;
		case CM_DPLL_CLKSEL_GFX_FCLK:
			*val = regs->dpll.clksel_gfx_fclk;
			break;
		case CM_DPLL_CLKSEL_ICSS_OCP_CLK:
			*val = regs->dpll.clksel_pru_icss_ocp_clk;
			break;
		case CM_DPLL_CLKSEL_LCDC_PIXEL_CLK:
			*val = regs->dpll.clksel_lcdc_pixel_clk;
			break;
		case CM_DPLL_CLKSEL_WDT1_CLK:
			*val = regs->dpll.clksel_wdt1_clk;
			break;
		case CM_DPLL_CLKSEL_GPIO0_DBCLK:
			*val = regs->dpll.clksel_gpio0_dbclk;
			break;
		case CM_MPU_CLKSTCTRL:
			*val = regs->mpu.cm_mpu_clkstctrl;
			break;
		case CM_MPU_MPU_CLKCTRL:
			*val = regs->mpu.cm_mpu_mpu_clkctrl;
			break;
		case CM_CLKOUT_CTRL:
			*val = regs->device.cm_clkout_ctrl;
			break;
		case CM_GFX_L3_CLKSTCTRL:
			*val = regs->gfx.cm_gfx_l3_clkstctrl;
			break;
		case CM_GFX_GFX_CLKCTRL:
			*val = regs->gfx.cm_gfx_gfx_clkctrl;
			break;
		case CM_GFX_L4LS_GFX_CLKSTCTRL:
			*val = regs->gfx.cm_gfx_l4ls_gfx_clkstctrl;
			break;
		case CM_GFX_MMUCFG_CLKCTRL:
			*val = regs->gfx.cm_gfx_mmucfg_clkctrl;
			break;
		case CM_GFX_MMUDATA_CLKCTRL:
			*val = regs->gfx.cm_gfx_mmudata_clkctrl;
			break;
		case CM_CEFUSE_CLKSTCTRL:
			*val = regs->cefuse.cm_cefuse_clkstctrl;
			break;
		case CM_CEFUSE_CEFUSE_CLKCTRL:
			*val = regs->cefuse.cm_cefuse_cefuse_clkctrl;
			break;
		case REVISION_PRM:
			*val = regs->prm_irq.revision_prm;
			break;
		case PRM_IRQSTATUS_MPU:
			*val = regs->prm_irq.prm_irqstatus_mpu;
			break;
		case PRM_IRQENABLE_MPU:
			*val = regs->prm_irq.prm_irqenable_mpu;
			break;
		case PRM_IRQSTATUS_M3:
			*val = regs->prm_irq.prm_irqstatus_m3;
			break;
		case PRM_IRQENABLE_M3:
			*val = regs->prm_irq.prm_irqenable_m3;
			break;
		case RM_PER_RSTCTRL:
			*val = regs->prm_per.rm_per_rstctrl;
			break;
		case PM_PER_PWRSTST:
			*val = regs->prm_per.pm_per_pwrstst;
			break;
		case PM_PER_PWRSTCTRL:
			*val = regs->prm_per.pm_per_pwrstctrl;
			break;
		case RM_WKUP_RSTCTRL:
			*val = regs->prm_wkup.rm_wkup_rstctrl;
			break;
		case PM_WKUP_PWRSTCTRL:
			*val = regs->prm_wkup.pm_wkup_pwrstctrl;
			break;
		case PM_WKUP_PWRSTST:
			*val = regs->prm_wkup.pm_wkup_pwrstst;
			break;
		case RM_WKUP_RSTST:
			*val = regs->prm_wkup.rm_wkup_rstst;
			break;
		case PM_MPU_PWRSTCTRL:
			*val = regs->prm_mpu.pm_mpu_pwrstctrl;
			break;
		case PM_MPU_PWRSTST:
			*val = regs->prm_mpu.pm_mpu_pwrstst;
			break;
		case RM_MPU_RSTST:
			*val = regs->prm_mpu.rm_mpu_rstst;
			break;
		case PRM_RSTCTRL:
			*val = regs->prm_device.prm_rstctrl;
			break;
		case PRM_RSTTIME:
			*val = regs->prm_device.prm_rsttime;
			break;
		case PRM_RSTST:
			*val = regs->prm_device.prm_rstst;
			break;
		case PRM_SRAM_COUNT:
			*val = regs->prm_device.prm_sram_count;
			break;
		case PRM_LDO_SRAM_CORE_SETUP:
			*val = regs->prm_device.prm_ldo_sram_core_setup;
			break;
		case PRM_LDO_SRAM_CORE_CTRL:
			*val = regs->prm_device.prm_ldo_sram_core_ctrl;
			break;
		case PRM_LDO_SRAM_MPU_SETUP:
			*val = regs->prm_device.prm_ldo_sram_mpu_setup;
			break;
		case PRM_LDO_SRAM_MPU_CTRL:
			*val = regs->prm_device.prm_ldo_sram_mpu_ctrl;
			break;
		case PM_RTC_PWRSTCTRL:
			*val = regs->prm_rtc.pm_rtc_pwrstctrl;
			break;
		case PM_RTC_PWRSTST:
			*val = regs->prm_rtc.pm_rtc_pwrstst;
			break;
		case PM_GFX_PWRSTCTRL:
			*val = regs->prm_gfx.pm_gfx_pwrstctrl;
			break;
		case RM_GFX_RSTCTRL:
			*val = regs->prm_gfx.rm_gfx_rstctrl;
			break;
		case PM_GFX_PWRSTST:
			*val = regs->prm_gfx.pm_gfx_pwrstst;
			break;
		case RM_GFX_RSTST:
			*val = regs->prm_gfx.rm_gfx_rstst;
			break;
		case PM_CEFUSE_PWRSTCTRL:
			*val = regs->prm_cefuse.pm_cefuse_pwrstctrl;
			break;
		case PM_CEFUSE_PWRSTST:
			*val = regs->prm_cefuse.pm_cefuse_pwrstst;
			break;
		case CM_RTC_RTC_CLKCTRL:
			*val = regs->rtc.cm_rtc_rtc_clkctrl;
			break;
		case CM_RTC_CLKSTCTRL:
			*val = regs->rtc.cm_rtc_clkstctrl;
			break;
		default:
			skyeye_log(Warning_log, __FUNCTION__, "Can not read the register at 0x%x in %s\n", offset, __FILE__);
			break;
	}
}
static exception_t prcm_am3359_write(conf_object_t *opaque,
		generic_address_t offset, uint32_t* buf, size_t count)
{
	prcm_am3359_device *dev = opaque->obj;
	prcm_am3359_reg_t* regs = dev->regs;
	uint32_t val = *(uint32_t*)buf, olddiv, newdiv, oldack, enable;

	switch (offset) {
		case CM_PER_L4LS_CLKSTCTRL:
			regs->per.l4ls_clkstctrl = val;
			break;
		case CM_PER_L3S_CLKSTCTRL:
			regs->per.l3s_clkstctrl = val;
			break;
#if 0
		case CM_PER_L4FW_CLKSTCTRL:
			regs->per.l4fw_clkstctrl = val;
			break;
#endif
		case CM_PER_L3_CLKSTCTRL:
			regs->per.l3_clkstctrl = val;
			break;
		case CM_PER_CPGMAC0_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.cpgmac0_clkctrl = val;
			break;
		case CM_PER_LCDC_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.lcdc_clkctrl = val;
			break;
		case CM_PER_USB0_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.usb0_clkctrl = val;
			break;
#if 0
		case CM_PER_MLB_CLKCTRL:
			regs->per.mlb_clkctrl = val;
			break;
#endif
		case CM_PER_TPTC0_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.tptc0_clkctrl = val;
			break;
		case CM_PER_EMIF_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.emif_clkctrl = val;
			break;
		case CM_PER_OCMCRAM_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.ocmcram_clkctrl = val;
			break;
		case CM_PER_GPMC_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.gpmc_clkctrl = val;
			break;
		case CM_PER_MCASP0_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.mcasp0_clkctrl = val;
			break;
		case CM_PER_UART5_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.uart5_clkctrl = val;
			break;
		case CM_PER_MMC0_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.mmc0_clkctrl = val;
			break;
		case CM_PER_ELM_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.elm_clkctrl = val;
			break;
		case CM_PER_I2C2_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.i2c2_clkctrl = val;
			break;
		case CM_PER_I2C1_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.i2c1_clkctrl = val;
			break;
		case CM_PER_SPI0_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.spi0_clkctrl = val;
			break;
		case CM_PER_SPI1_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.spi1_clkctrl = val;
			break;
		case CM_PER_L4LS_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.l4ls_clkctrl = val;
			break;
#if 0
		case CM_PER_L4FW_CLKCTRL:
			regs->per.l4fw_clkctrl = val;
			break;
#endif
		case CM_PER_MCASP1_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.mcasp1_clkctrl = val;
			break;
		case CM_PER_UART1_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.uart1_clkctrl = val;
			break;
		case CM_PER_UART2_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.uart2_clkctrl = val;
			break;
		case CM_PER_UART3_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.uart3_clkctrl = val;
			break;
		case CM_PER_UART4_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.uart4_clkctrl = val;
			break;
		case CM_PER_TIMER7_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.timer7_clkctrl = val;
			break;
		case CM_PER_TIMER2_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.timer2_clkctrl = val;
			break;
		case CM_PER_TIMER3_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.timer3_clkctrl = val;
			break;
		case CM_PER_TIMER4_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.timer4_clkctrl = val;
			break;
#if 0
		case CM_PER_RNG_CLKCTRL:
			regs->per.rng_clkctrl = val;
			break;
		case CM_PER_AES0_CLKCTRL:
			regs->per.aes0_clkctrl = val;
			break;
		case CM_PER_SHA0_CLKCTRL:
			regs->per.sha0_clkctrl = val;
			break;
		case CM_PER_PKA_CLKCTRL:
			regs->per.pka_clkctrl = val;
			break;
		case CM_PER_GPIO6_CLKCTRL:
			regs->per.gpio6_clkctrl = val;
			break;
#endif
		case CM_PER_GPIO1_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.gpio1_clkctrl = val;
			break;
		case CM_PER_GPIO2_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.gpio2_clkctrl = val;
			break;
		case CM_PER_GPIO3_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.gpio3_clkctrl = val;
			break;
		case CM_PER_TPCC_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.tpcc_clkctrl = val;
			break;
		case CM_PER_DCAN0_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.dcan0_clkctrl = val;
			break;
		case CM_PER_DCAN1_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.dcan1_clkctrl = val;
			break;
		case CM_PER_EPWMSS1_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.epwmss1_clkctrl = val;
			break;
#if 0
		case CM_PER_EMIF_FW_CLKCTRL:
			regs->per.emif_fw_clkctrl = val;
			break;
#endif
		case CM_PER_EPWMSS0_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.epwmss0_clkctrl = val;
			break;
		case CM_PER_EPWMSS2_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.epwmss2_clkctrl = val;
			break;
		case CM_PER_L3_INSTR_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.l3_instr_clkctrl = val;
			break;
		case CM_PER_L3_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.l3_clkctrl = val;
			break;
		case CM_PER_IEEE5000_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.ieee5000_clkctrl = val;
			break;
		case CM_PER_ICSS_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.pru_icss_clkctrl = val;
			break;
		case CM_PER_TIMER5_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.timer5_clkctrl = val;
			break;
		case CM_PER_TIMER6_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.timer6_clkctrl = val;
			break;
		case CM_PER_MMC1_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.mmc1_clkctrl = val;
			break;
		case CM_PER_MMC2_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.mmc2_clkctrl = val;
			break;
		case CM_PER_TPTC1_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.tptc1_clkctrl = val;
			break;
		case CM_PER_TPTC2_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.tptc2_clkctrl = val;
			break;
		case CM_PER_SPINLOCK_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.spinlock_clkctrl = val;
			break;
		case CM_PER_MAILBOX0_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.mailbox0_clkctrl = val;
			break;
		case CM_PER_L4HS_CLKSTCTRL:
			regs->per.l4hs_clkstctrl = val;
			break;
		case CM_PER_L4HS_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.l4hs_clkctrl = val;
			break;
#if 0
		case CM_PER_MSTR_EXPS_CLKCTRL:
			regs->per.mstr_exps_clkctrl = val;
			break;
		case CM_PER_SLV_EXPS_CLKCTRL:
			regs->per.slv_exps_clkctrl = val;
			break;
#endif
		case CM_PER_OCPWP_L3_CLKSTCTRL:
			regs->per.ocpwp_l3_clkstctrl = val;
			break;
		case CM_PER_OCPWP_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.ocpwp_clkctrl = val;
			break;
		case CM_PER_ICSS_CLKSTCTRL:
			regs->per.pru_icss_clkstctrl = val;
			break;
		case CM_PER_CPSW_CLKSTCTRL:
			regs->per.cpsw_clkstctrl = val;
			break;
		case CM_PER_LCDC_CLKSTCTRL:
			regs->per.lcdc_clkstctrl = val;
			break;
		case CM_PER_CLKDIV32K_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->per.clkdiv32k_clkctrl = val;
			break;
		case CM_PER_CLK_24MHZ_CLKSTCTRL:
			regs->per.clk_24mhz_clkstctrl = val;
			break;
		case CM_WKUP_CLKSTCTRL:
			regs->wkup.cm_wkup_clkstctrl = val;
			break;
		case CM_WKUP_CONTROL_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->wkup.cm_wkup_control_clkctrl = val;
			break;
		case CM_WKUP_GPIO0_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->wkup.cm_wkup_gpio0_clkctrl = val;
			break;
		case CM_WKUP_L4WKUP_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->wkup.cm_wkup_l4wkup_clkctrl = val;
			break;
		case CM_WKUP_TIMER0_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->wkup.cm_wkup_timer0_clkctrl = val;
			break;
		case CM_WKUP_DEBUGSS_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->wkup.cm_wkup_debugss_clkctrl = val;
			break;
		case CM_L3_AON_CLKSTCTRL:
			regs->wkup.cm_l3_aon_clkstctrl = val;
			break;
		case CM_AUTOIDLE_DPLL_MPU:
			regs->wkup.cm_autoidle_dpll_mpu = val;
			break;
		case CM_IDLEST_DPLL_MPU:
			regs->wkup.cm_idlest_dpll_mpu = val;
			break;
		case CM_SSC_DELTAMSTEP_DPLL_MPU:
			regs->wkup.cm_ssc_deltamstep_dpll_mpu = val;
			break;
		case CM_SSC_MODFREQDIV_DPLL_MPU:
			regs->wkup.cm_ssc_modfreqdiv_dpll_mpu = val;
			break;
		case CM_CLKSEL_DPLL_MPU:
			regs->wkup.cm_clksel_dpll_mpu = val;
			break;
		case CM_AUTOIDLE_DPLL_DDR:
			regs->wkup.cm_autoidle_dpll_ddr = val;
			break;
		case CM_IDLEST_DPLL_DDR:
			regs->wkup.cm_idlest_dpll_ddr = val;
			break;
		case CM_SSC_DELTAMSTEP_DPLL_DDR:
			regs->wkup.cm_ssc_deltamstep_dpll_ddr = val;
			break;
		case CM_SSC_MODFREQDIV_DPLL_DDR:
			regs->wkup.cm_ssc_modfreqdiv_dpll_ddr = val;
			break;
		case CM_CLKSEL_DPLL_DDR:
			regs->wkup.cm_clksel_dpll_ddr = val;
			break;
		case CM_AUTOIDLE_DPLL_DISP:
			regs->wkup.cm_autoidle_dpll_disp = val;
			break;
		case CM_IDLEST_DPLL_DISP:
			regs->wkup.cm_idlest_dpll_disp = val;
			break;
		case CM_SSC_DELTAMSTEP_DPLL_DISP:
			regs->wkup.cm_ssc_deltamstep_dpll_disp = val;
			break;
		case CM_SSC_MODFREQDIV_DPLL_DISP:
			regs->wkup.cm_ssc_modfreqdiv_dpll_disp = val;
			break;
		case CM_CLKSEL_DPLL_DISP:
			regs->wkup.cm_clksel_dpll_disp = val;
			break;
		case CM_AUTOIDLE_DPLL_CORE:
			regs->wkup.cm_autoidle_dpll_core = val;
			break;
		case CM_IDLEST_DPLL_CORE:
			regs->wkup.cm_idlest_dpll_core = val;
			break;
		case CM_SSC_DELTAMSTEP_DPLL_CORE:
			regs->wkup.cm_ssc_deltamstep_dpll_core = val;
			break;
		case CM_SSC_MODFREQDIV_DPLL_CORE:
			regs->wkup.cm_ssc_modfreqdiv_dpll_core = val;
			break;
		case CM_CLKSEL_DPLL_CORE:
			regs->wkup.cm_clksel_dpll_core = val;
			break;
		case CM_AUTOIDLE_DPLL_PER:
			regs->wkup.cm_autoidle_dpll_per = val;
			break;
		case CM_IDLEST_DPLL_PER:
			regs->wkup.cm_idlest_dpll_per = val;
			break;
		case CM_SSC_DELTAMSTEP_DPLL_PER:
			regs->wkup.cm_ssc_deltamstep_dpll_per = val;
			break;
		case CM_SSC_MODFREQDIV_DPLL_PER:
			regs->wkup.cm_ssc_modfreqdiv_dpll_per = val;
			break;
		case CM_CLKDCOLDO_DPLL_PER:
			regs->wkup.cm_clkdcoldo_dpll_per = val;
			break;
		case CM_DIV_M4_DPLL_CORE:
			olddiv = regs->wkup.cm_div_m4_dpll_core & 0x1f;
			oldack = (regs->wkup.cm_div_m4_dpll_core >> 5) & 1;
			newdiv = val & 0x1f;
			regs->wkup.cm_div_m4_dpll_core = val;
			if (olddiv != newdiv) {
				if (oldack == 0)
					regs->wkup.cm_div_m4_dpll_core |= 1 << 5;
				else
					regs->wkup.cm_div_m4_dpll_core &= ~(1 << 5);
			}
			break;
		case CM_DIV_M5_DPLL_CORE:
			olddiv = regs->wkup.cm_div_m5_dpll_core & 0x1f;
			oldack = (regs->wkup.cm_div_m5_dpll_core >> 5) & 1;
			newdiv = val & 0x1f;
			regs->wkup.cm_div_m5_dpll_core = val;
			if (olddiv != newdiv) {
				if (oldack == 0)
					regs->wkup.cm_div_m5_dpll_core |= 1 << 5;
				else
					regs->wkup.cm_div_m5_dpll_core &= ~(1 << 5);
			}
			break;
		case CM_CLKMODE_DPLL_MPU:
			regs->wkup.cm_clkmode_dpll_mpu = val;
			if ((val & 0x7) == 0x4) {
				regs->wkup.cm_idlest_dpll_mpu |= 1 << 8;
				regs->wkup.cm_idlest_dpll_mpu &= ~1;
			} else if ((val & 0x7) == 0x7) {
				regs->wkup.cm_idlest_dpll_mpu &= ~(1 << 8);
				regs->wkup.cm_idlest_dpll_mpu |= 1;
			}
			break;
		case CM_CLKMODE_DPLL_PER:
			regs->wkup.cm_clkmode_dpll_per = val;
			if ((val & 0x7) == 0x4) {
				regs->wkup.cm_idlest_dpll_core |= 1 << 8;
				regs->wkup.cm_idlest_dpll_core &= ~1;
			} else if ((val & 0x7) == 0x7) {
				regs->wkup.cm_idlest_dpll_core &= ~(1 << 8);
				regs->wkup.cm_idlest_dpll_core |= 1;
			}
			break;
		case CM_CLKMODE_DPLL_CORE:
			regs->wkup.cm_clkmode_dpll_core = val;
			if ((val & 0x7) == 0x4) {
				regs->wkup.cm_idlest_dpll_core |= 1 << 8;
				regs->wkup.cm_idlest_dpll_core &= ~1;
			} else if ((val & 0x7) == 0x4) {
				regs->wkup.cm_idlest_dpll_core &= ~(1 << 8);
				regs->wkup.cm_idlest_dpll_core |= 1;
			}
			break;
		case CM_CLKMODE_DPLL_DDR:
			regs->wkup.cm_clkmode_dpll_ddr = val;
			if ((val & 0x7) == 0x4) {
				regs->wkup.cm_idlest_dpll_ddr |= 1 << 8;
				regs->wkup.cm_idlest_dpll_ddr &= ~1;
			} else if ((val & 0x7) == 0x7) {
				regs->wkup.cm_idlest_dpll_ddr &= ~(1 << 8);
				regs->wkup.cm_idlest_dpll_ddr |= 1;
			}
			break;
		case CM_CLKMODE_DPLL_DISP:
			regs->wkup.cm_clkmode_dpll_disp = val;
			if ((val & 0x7) == 0x4) {
				regs->wkup.cm_idlest_dpll_disp |= 1 << 8;
				regs->wkup.cm_idlest_dpll_disp &= ~1;
			} else if ((val & 0x7) == 0x7) {
				regs->wkup.cm_idlest_dpll_disp &= ~(1 << 8);
				regs->wkup.cm_idlest_dpll_disp |= 1;
			}
			break;
		case CM_CLKSEL_DPLL_PERIPH:
			regs->wkup.cm_clksel_dpll_periph = val;
			break;
		case CM_DIV_M2_DPLL_DDR:
			regs->wkup.cm_div_m2_dpll_ddr = val;
			break;
		case CM_DIV_M2_DPLL_DISP:
			regs->wkup.cm_div_m2_dpll_disp = val;
			break;
		case CM_DIV_M2_DPLL_MPU:
			regs->wkup.cm_div_m2_dpll_mpu = val;
			break;
		case CM_DIV_M2_DPLL_PER:
			olddiv = regs->wkup.cm_div_m2_dpll_per & 0x7f;
			oldack = (regs->wkup.cm_div_m2_dpll_per >> 7) & 1;
			newdiv = val & 0x7f;
			regs->wkup.cm_div_m2_dpll_per = val;
			if (olddiv != newdiv) {
				if (oldack == 0)
					regs->wkup.cm_div_m2_dpll_per |= 1 << 7;
				else
					regs->wkup.cm_div_m2_dpll_per &= ~(1 << 7);
			}
			break;
		case CM_WKUP_WKUP_M3_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->wkup.cm_wkup_wkup_m3_clkctrl = val;
			break;
		case CM_WKUP_UART0_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->wkup.cm_wkup_uart0_clkctrl = val;
			break;
		case CM_WKUP_I2C0_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->wkup.cm_wkup_i2c0_clkctrl = val;
			break;
		case CM_WKUP_ADC_TSC_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->wkup.cm_wkup_adc_tsc_clkctrl = val;
			break;
		case CM_WKUP_SMARTREFLEX0_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->wkup.cm_wkup_smartreflex0_clkctrl = val;
			break;
		case CM_WKUP_TIMER1_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->wkup.cm_wkup_timer1_clkctrl = val;
			break;
		case CM_WKUP_SMARTREFLEX1_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->wkup.cm_wkup_smartreflex1_clkctrl = val;
			break;
		case CM_L4_WKUP_AON_CLKSTCTRL:
			regs->wkup.cm_l4_wkup_aon_clkstctrl = val;
			break;
		case CM_WKUP_WDT1_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->wkup.cm_wkup_wdt1_clkctrl = val;
			break;
		case CM_DIV_M6_DPLL_CORE:
			olddiv = regs->wkup.cm_div_m6_dpll_core & 0x1f;
			oldack = (regs->wkup.cm_div_m6_dpll_core >> 5) & 1;
			newdiv = val & 0x1f;
			regs->wkup.cm_div_m6_dpll_core = val;
			if (olddiv != newdiv) {
				if (oldack == 0)
					regs->wkup.cm_div_m6_dpll_core |= 1 << 5;
				else
					regs->wkup.cm_div_m6_dpll_core &= ~(1 << 5);
			}
			break;
		case CM_DPLL_CLKSEL_TIMER7_CLK:
			regs->dpll.clksel_timer7_clk = val;
			break;
		case CM_DPLL_CLKSEL_TIMER2_CLK:
			regs->dpll.clksel_timer2_clk = val;
			break;
		case CM_DPLL_CLKSEL_TIMER3_CLK:
			regs->dpll.clksel_timer3_clk = val;
			break;
		case CM_DPLL_CLKSEL_TIMER4_CLK:
			regs->dpll.clksel_timer4_clk = val;
			break;
		case CM_DPLL_CM_MAC_CLKSEL:
			regs->dpll.cm_mac_clksel = val;
			break;
		case CM_DPLL_CLKSEL_TIMER5_CLK:
			regs->dpll.clksel_timer5_clk = val;
			break;
		case CM_DPLL_CLKSEL_TIMER6_CLK:
			regs->dpll.clksel_timer6_clk = val;
			break;
		case CM_DPLL_CM_CPTS_RFT_CLKSEL:
			regs->dpll.cm_cpts_rft_clksel = val;
			break;
		case CM_DPLL_CLKSEL_TIMER1MS_CLK:
			regs->dpll.clksel_timer1ms_clk = val;
			break;
		case CM_DPLL_CLKSEL_GFX_FCLK:
			regs->dpll.clksel_gfx_fclk = val;
			break;
		case CM_DPLL_CLKSEL_ICSS_OCP_CLK:
			regs->dpll.clksel_pru_icss_ocp_clk = val;
			break;
		case CM_DPLL_CLKSEL_LCDC_PIXEL_CLK:
			regs->dpll.clksel_lcdc_pixel_clk = val;
			break;
		case CM_DPLL_CLKSEL_WDT1_CLK:
			regs->dpll.clksel_wdt1_clk = val;
			break;
		case CM_DPLL_CLKSEL_GPIO0_DBCLK:
			regs->dpll.clksel_gpio0_dbclk = val;
			break;
		case CM_MPU_CLKSTCTRL:
			regs->mpu.cm_mpu_clkstctrl = val;
			break;
		case CM_MPU_MPU_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->mpu.cm_mpu_mpu_clkctrl = val;
			break;
		case CM_CLKOUT_CTRL:
			regs->device.cm_clkout_ctrl = val;
			break;
		case CM_RTC_RTC_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->rtc.cm_rtc_rtc_clkctrl = val;
			break;
		case CM_RTC_CLKSTCTRL:
			regs->rtc.cm_rtc_clkstctrl = val;
			break;
		case CM_GFX_L3_CLKSTCTRL:
			regs->gfx.cm_gfx_l3_clkstctrl = val;
			break;
		case CM_GFX_GFX_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->gfx.cm_gfx_gfx_clkctrl = val;
			break;
		case CM_GFX_L4LS_GFX_CLKSTCTRL:
			regs->gfx.cm_gfx_l4ls_gfx_clkstctrl = val;
			break;
		case CM_GFX_MMUCFG_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->gfx.cm_gfx_mmucfg_clkctrl = val;
			break;
		case CM_GFX_MMUDATA_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->gfx.cm_gfx_mmudata_clkctrl = val;
			break;
		case CM_CEFUSE_CLKSTCTRL:
			regs->cefuse.cm_cefuse_clkstctrl = val;
			break;
		case CM_CEFUSE_CEFUSE_CLKCTRL:
			CHANGE_CLKCTRL(val);
			regs->cefuse.cm_cefuse_cefuse_clkctrl = val;
			break;
		case REVISION_PRM:
			regs->prm_irq.revision_prm = val;
			break;
		case PRM_IRQSTATUS_MPU:
			regs->prm_irq.prm_irqstatus_mpu = val;
			break;
		case PRM_IRQENABLE_MPU:
			regs->prm_irq.prm_irqenable_mpu = val;
			break;
		case PRM_IRQSTATUS_M3:
			regs->prm_irq.prm_irqstatus_m3 = val;
			break;
		case PRM_IRQENABLE_M3:
			regs->prm_irq.prm_irqenable_m3 = val;
			break;
		case RM_PER_RSTCTRL:
			regs->prm_per.rm_per_rstctrl = val;
			break;
		case PM_PER_PWRSTST:
			regs->prm_per.pm_per_pwrstst = val;
			break;
		case PM_PER_PWRSTCTRL:
			regs->prm_per.pm_per_pwrstctrl = val;
			break;
		case RM_WKUP_RSTCTRL:
			regs->prm_wkup.rm_wkup_rstctrl = val;
			break;
		case PM_WKUP_PWRSTCTRL:
			regs->prm_wkup.pm_wkup_pwrstctrl = val;
			break;
		case PM_WKUP_PWRSTST:
			regs->prm_wkup.pm_wkup_pwrstst = val;
			break;
		case RM_WKUP_RSTST:
			regs->prm_wkup.rm_wkup_rstst = val;
			break;
		case PM_MPU_PWRSTCTRL:
			regs->prm_mpu.pm_mpu_pwrstctrl = val;
			break;
		case PM_MPU_PWRSTST:
			regs->prm_mpu.pm_mpu_pwrstst = val;
			break;
		case RM_MPU_RSTST:
			regs->prm_mpu.rm_mpu_rstst = val;
			break;
		case PRM_RSTCTRL:
			regs->prm_device.prm_rstctrl = val;
			break;
		case PRM_RSTTIME:
			regs->prm_device.prm_rsttime = val;
			break;
		case PRM_RSTST:
			regs->prm_device.prm_rstst = val;
			break;
		case PRM_SRAM_COUNT:
			regs->prm_device.prm_sram_count = val;
			break;
		case PRM_LDO_SRAM_CORE_SETUP:
			regs->prm_device.prm_ldo_sram_core_setup = val;
			break;
		case PRM_LDO_SRAM_CORE_CTRL:
			regs->prm_device.prm_ldo_sram_core_ctrl = val;
			break;
		case PRM_LDO_SRAM_MPU_SETUP:
			regs->prm_device.prm_ldo_sram_mpu_setup = val;
			break;
		case PRM_LDO_SRAM_MPU_CTRL:
			regs->prm_device.prm_ldo_sram_mpu_ctrl = val;
			break;
		case PM_RTC_PWRSTCTRL:
			regs->prm_rtc.pm_rtc_pwrstctrl = val;
			break;
		case PM_RTC_PWRSTST:
			regs->prm_rtc.pm_rtc_pwrstst = val;
			break;
		case PM_GFX_PWRSTCTRL:
			regs->prm_gfx.pm_gfx_pwrstctrl = val;
			break;
		case RM_GFX_RSTCTRL:
			regs->prm_gfx.rm_gfx_rstctrl = val;
			break;
		case PM_GFX_PWRSTST:
			regs->prm_gfx.pm_gfx_pwrstst = val;
			break;
		case RM_GFX_RSTST:
			regs->prm_gfx.rm_gfx_rstst = val;
			break;
		case PM_CEFUSE_PWRSTCTRL:
			regs->prm_cefuse.pm_cefuse_pwrstctrl = val;
			break;
		case PM_CEFUSE_PWRSTST:
			regs->prm_cefuse.pm_cefuse_pwrstst = val;
			break;
		default:
			skyeye_log(Warning_log, __FUNCTION__, "Can not write the register at 0x%x in %s\n", offset, __FILE__);
			break;
	}
}
static uint32_t prcm_am3359_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id) {
	prcm_am3359_device *dev = conf_obj->obj;
	struct prcm_am3359_reg *regs = dev->regs;
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static char* prcm_am3359_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	prcm_am3359_device *dev = conf_obj->obj;
	return regs_name[id];
}
static exception_t prcm_am3359_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	prcm_am3359_device *dev = conf_obj->obj;
	struct prcm_am3359_reg *regs = dev->regs;
	*((uint32_t*)regs + id) = value;

	return No_exp;
}
static uint32_t prcm_am3359_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	prcm_am3359_device *dev = conf_obj->obj;
	struct prcm_am3359_reg *regs = dev->regs;
	return  sizeof(struct prcm_am3359_reg) / 4;
}
static uint32_t prcm_am3359_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	prcm_am3359_device *dev = conf_obj->obj;
	struct prcm_am3359_reg *regs = dev->regs;
	uint32_t regnum;
	regnum = sizeof(struct prcm_am3359_reg) / 4;
	int i;

	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_prcm_am3359(char* obj_name)
{
	prcm_am3359_device* dev = skyeye_mm_zero(sizeof(prcm_am3359_device));
	prcm_am3359_reg_t* regs =  skyeye_mm_zero(sizeof(prcm_am3359_reg_t));

	dev->obj = new_conf_object(obj_name, dev);
	dev->regs = regs;

	init_regs(dev);
	return dev->obj;
}
static exception_t free_prcm_am3359(conf_object_t *opaque) {
	prcm_am3359_device *dev = opaque->obj;

	skyeye_free(dev->regs);
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}
static exception_t reset_prcm_am3359(conf_object_t *opaque, const char *args)
{
	prcm_am3359_device *dev = opaque->obj;
	init_regs(dev);

	return No_exp;
}
void init_prcm_am3359(void)
{
	static skyeye_class_t class_data = {
		.class_name = "am3359_prcm",
		.class_desc = "am3359_prcm",
		.new_instance = new_prcm_am3359,
		.free_instance = free_prcm_am3359,
		.reset_instance = reset_prcm_am3359,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_am335x SC_am3359),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read	= prcm_am3359_read,
		.write	= prcm_am3359_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = prcm_am3359_get_regvalue_by_id,
		.get_regname_by_id = prcm_am3359_get_regname_by_id,
		.set_regvalue_by_id = prcm_am3359_set_regvalue_by_id,
		.get_regnum = prcm_am3359_get_regnum,
		.get_regid_by_name = prcm_am3359_get_regid_by_name,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		},
		(struct InterfaceDescription) {
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf,
		}
 	};
	class_register_ifaces(clss, ifaces);
}
