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
* @file prcm_am3359.h
* @brief The definition of system controller
* @author
* @version 78.77
*/

/* Autogenerated by SkyEye script */
#ifndef __prcm_am3359_H__
#define __prcm_am3359_H__

typedef struct prcm_am3359_reg{
	struct cm_per{
		uint32_t l4ls_clkstctrl;
		uint32_t l3s_clkstctrl;
		uint32_t l3_clkstctrl;
		uint32_t cpgmac0_clkctrl;
		uint32_t lcdc_clkctrl;
		uint32_t usb0_clkctrl;
		uint32_t tptc0_clkctrl;
		uint32_t emif_clkctrl;
		uint32_t ocmcram_clkctrl;
		uint32_t gpmc_clkctrl;
		uint32_t mcasp0_clkctrl;
		uint32_t uart5_clkctrl;
		uint32_t mmc0_clkctrl;
		uint32_t elm_clkctrl;
		uint32_t i2c2_clkctrl;
		uint32_t i2c1_clkctrl;
		uint32_t spi0_clkctrl;
		uint32_t spi1_clkctrl;
		uint32_t l4ls_clkctrl;
		uint32_t mcasp1_clkctrl;
		uint32_t uart1_clkctrl;
		uint32_t uart2_clkctrl;
		uint32_t uart3_clkctrl;
		uint32_t uart4_clkctrl;
		uint32_t timer7_clkctrl;
		uint32_t timer2_clkctrl;
		uint32_t timer3_clkctrl;
		uint32_t timer4_clkctrl;
		uint32_t gpio1_clkctrl;
		uint32_t gpio2_clkctrl;
		uint32_t gpio3_clkctrl;
		uint32_t tpcc_clkctrl;
		uint32_t dcan0_clkctrl;
		uint32_t dcan1_clkctrl;
		uint32_t epwmss1_clkctrl;
		uint32_t epwmss0_clkctrl;
		uint32_t epwmss2_clkctrl;
		uint32_t l3_instr_clkctrl;
		uint32_t l3_clkctrl;
		uint32_t ieee5000_clkctrl;
		uint32_t pru_icss_clkctrl;
		uint32_t timer5_clkctrl;
		uint32_t timer6_clkctrl;
		uint32_t mmc1_clkctrl;
		uint32_t mmc2_clkctrl;
		uint32_t tptc1_clkctrl;
		uint32_t tptc2_clkctrl;
		uint32_t spinlock_clkctrl;
		uint32_t mailbox0_clkctrl;
		uint32_t l4hs_clkstctrl;
		uint32_t l4hs_clkctrl;
		uint32_t ocpwp_l3_clkstctrl;
		uint32_t ocpwp_clkctrl;
		uint32_t pru_icss_clkstctrl;
		uint32_t cpsw_clkstctrl;
		uint32_t lcdc_clkstctrl;
		uint32_t clkdiv32k_clkctrl;
		uint32_t clk_24mhz_clkstctrl;

		//uint32_t l4fw_clkctrl;
	}per;
	struct cm_dpll{
		uint32_t clksel_timer7_clk;
		uint32_t clksel_timer2_clk;
		uint32_t clksel_timer3_clk;
		uint32_t clksel_timer4_clk;
		uint32_t cm_mac_clksel;
		uint32_t clksel_timer5_clk;
		uint32_t clksel_timer6_clk;
		uint32_t cm_cpts_rft_clksel;
		uint32_t clksel_timer1ms_clk;
		uint32_t clksel_gfx_fclk;
		uint32_t clksel_pru_icss_ocp_clk;
		uint32_t clksel_lcdc_pixel_clk;
		uint32_t clksel_wdt1_clk;
		uint32_t clksel_gpio0_dbclk;
	}dpll;
	struct cm_wkup{
		uint32_t cm_wkup_clkstctrl;
		uint32_t cm_wkup_control_clkctrl;
		uint32_t cm_wkup_gpio0_clkctrl;
		uint32_t cm_wkup_l4wkup_clkctrl;
		uint32_t cm_wkup_timer0_clkctrl;
		uint32_t cm_wkup_debugss_clkctrl;
		uint32_t cm_l3_aon_clkstctrl;
		uint32_t cm_autoidle_dpll_mpu;
		uint32_t cm_idlest_dpll_mpu;
		uint32_t cm_ssc_deltamstep_dpll_mpu;
		uint32_t cm_ssc_modfreqdiv_dpll_mpu;
		uint32_t cm_clksel_dpll_mpu;
		uint32_t cm_autoidle_dpll_ddr;
		uint32_t cm_idlest_dpll_ddr;
		uint32_t cm_ssc_deltamstep_dpll_ddr;
		uint32_t cm_ssc_modfreqdiv_dpll_ddr;
		uint32_t cm_clksel_dpll_ddr;
		uint32_t cm_autoidle_dpll_disp;
		uint32_t cm_idlest_dpll_disp;
		uint32_t cm_ssc_deltamstep_dpll_disp;
		uint32_t cm_ssc_modfreqdiv_dpll_disp;
		uint32_t cm_clksel_dpll_disp;
		uint32_t cm_autoidle_dpll_core;
		uint32_t cm_idlest_dpll_core;
		uint32_t cm_ssc_deltamstep_dpll_core;
		uint32_t cm_ssc_modfreqdiv_dpll_core;
		uint32_t cm_clksel_dpll_core;
		uint32_t cm_autoidle_dpll_per;
		uint32_t cm_idlest_dpll_per;
		uint32_t cm_ssc_deltamstep_dpll_per;
		uint32_t cm_ssc_modfreqdiv_dpll_per;
		uint32_t cm_clkdcoldo_dpll_per;
		uint32_t cm_div_m4_dpll_core;
		uint32_t cm_div_m5_dpll_core;
		uint32_t cm_clkmode_dpll_mpu;
		uint32_t cm_clkmode_dpll_per;
		uint32_t cm_clkmode_dpll_core;
		uint32_t cm_clkmode_dpll_ddr;
		uint32_t cm_clkmode_dpll_disp;
		uint32_t cm_clksel_dpll_periph;
		uint32_t cm_div_m2_dpll_ddr;
		uint32_t cm_div_m2_dpll_disp;
		uint32_t cm_div_m2_dpll_mpu;
		uint32_t cm_div_m2_dpll_per;
		uint32_t cm_wkup_wkup_m3_clkctrl;
		uint32_t cm_wkup_uart0_clkctrl;
		uint32_t cm_wkup_i2c0_clkctrl;
		uint32_t cm_wkup_adc_tsc_clkctrl;
		uint32_t cm_wkup_smartreflex0_clkctrl;
		uint32_t cm_wkup_timer1_clkctrl;
		uint32_t cm_wkup_smartreflex1_clkctrl;
		uint32_t cm_l4_wkup_aon_clkstctrl;
		uint32_t cm_wkup_wdt1_clkctrl;
		uint32_t cm_div_m6_dpll_core;
	}wkup;

	struct cm_rtc{
		uint32_t cm_rtc_rtc_clkctrl;
		uint32_t cm_rtc_clkstctrl;
	}rtc;

	struct cm_mpu {
		uint32_t cm_mpu_clkstctrl;
		uint32_t cm_mpu_mpu_clkctrl;
	}mpu;

	struct cm_device {
		uint32_t cm_clkout_ctrl;
	}device;

	struct cm_gfx {
		uint32_t cm_gfx_l3_clkstctrl;
		uint32_t cm_gfx_gfx_clkctrl;
		uint32_t cm_gfx_l4ls_gfx_clkstctrl;
		uint32_t cm_gfx_mmucfg_clkctrl;
		uint32_t cm_gfx_mmudata_clkctrl;
	}gfx;

	struct cm_cefuse {
		uint32_t cm_cefuse_clkstctrl;
		uint32_t cm_cefuse_cefuse_clkctrl;
	}cefuse;

	struct prm_irq {
		uint32_t revision_prm;
		uint32_t prm_irqstatus_mpu;
		uint32_t prm_irqenable_mpu;
		uint32_t prm_irqstatus_m3;
		uint32_t prm_irqenable_m3;
	} prm_irq;

	struct prm_per {
		uint32_t rm_per_rstctrl;
		uint32_t pm_per_pwrstst;
		uint32_t pm_per_pwrstctrl;
	} prm_per;

	struct prm_wkup {
		uint32_t rm_wkup_rstctrl;
		uint32_t pm_wkup_pwrstctrl;
		uint32_t pm_wkup_pwrstst;
		uint32_t rm_wkup_rstst;
	}prm_wkup;

	struct prm_mpu {
		uint32_t pm_mpu_pwrstctrl;
		uint32_t pm_mpu_pwrstst;
		uint32_t rm_mpu_rstst;
	}prm_mpu;

	struct prm_device {
		uint32_t prm_rstctrl;
		uint32_t prm_rsttime;
		uint32_t prm_rstst;
		uint32_t prm_sram_count;
		uint32_t prm_ldo_sram_core_setup;
		uint32_t prm_ldo_sram_core_ctrl;
		uint32_t prm_ldo_sram_mpu_setup;
		uint32_t prm_ldo_sram_mpu_ctrl;
	}prm_device;

	struct prm_rtc {
		uint32_t pm_rtc_pwrstctrl;
		uint32_t pm_rtc_pwrstst;
	}prm_rtc;

	struct prm_gfx {
		uint32_t pm_gfx_pwrstctrl;
		uint32_t rm_gfx_rstctrl;
		uint32_t pm_gfx_pwrstst;
		uint32_t rm_gfx_rstst;
	}prm_gfx;

	struct prm_cefuse {
		uint32_t pm_cefuse_pwrstctrl;
		uint32_t pm_cefuse_pwrstst;
	}prm_cefuse;

}prcm_am3359_reg_t;

typedef struct prcm_am3359_device{
	conf_object_t* obj;
	prcm_am3359_reg_t* regs;
}prcm_am3359_device;

#define CM_PER_L4LS_CLKSTCTRL   		(0x0)
#define CM_PER_L3S_CLKSTCTRL   			(0x4)
//#define CM_PER_L4FW_CLKSTCTRL   		(0x8)
#define CM_PER_L3_CLKSTCTRL   			(0xc)
#define CM_PER_CPGMAC0_CLKCTRL   		(0x14)
#define CM_PER_LCDC_CLKCTRL   			(0x18)
#define CM_PER_USB0_CLKCTRL   			(0x1c)
//#define CM_PER_MLB_CLKCTRL   			(0x20)
#define CM_PER_TPTC0_CLKCTRL   			(0x24)
#define CM_PER_EMIF_CLKCTRL   			(0x28)
#define CM_PER_OCMCRAM_CLKCTRL   		(0x2c)
#define CM_PER_GPMC_CLKCTRL   			(0x30)
#define CM_PER_MCASP0_CLKCTRL   		(0x34)
#define CM_PER_UART5_CLKCTRL   			(0x38)
#define CM_PER_MMC0_CLKCTRL   			(0x3c)
#define CM_PER_ELM_CLKCTRL   			(0x40)
#define CM_PER_I2C2_CLKCTRL   			(0x44)
#define CM_PER_I2C1_CLKCTRL   			(0x48)
#define CM_PER_SPI0_CLKCTRL   			(0x4c)
#define CM_PER_SPI1_CLKCTRL   			(0x50)
#define CM_PER_L4LS_CLKCTRL   			(0x60)
//#define CM_PER_L4FW_CLKCTRL   		(0x64)
#define CM_PER_MCASP1_CLKCTRL   		(0x68)
#define CM_PER_UART1_CLKCTRL   			(0x6c)
#define CM_PER_UART2_CLKCTRL   			(0x70)
#define CM_PER_UART3_CLKCTRL   			(0x74)
#define CM_PER_UART4_CLKCTRL   			(0x78)
#define CM_PER_TIMER7_CLKCTRL   		(0x7c)
#define CM_PER_TIMER2_CLKCTRL   		(0x80)
#define CM_PER_TIMER3_CLKCTRL   		(0x84)
#define CM_PER_TIMER4_CLKCTRL   		(0x88)
//#define CM_PER_RNG_CLKCTRL   			(0x90)
//#define CM_PER_AES0_CLKCTRL   		(0x94)
//#define CM_PER_SHA0_CLKCTRL   		(0xa0)
//#define CM_PER_PKA_CLKCTRL   			(0xa4)
//#define CM_PER_GPIO6_CLKCTRL   		(0xa8)
#define CM_PER_GPIO1_CLKCTRL   			(0xac)
#define CM_PER_GPIO2_CLKCTRL   			(0xb0)
#define CM_PER_GPIO3_CLKCTRL   			(0xb4)
#define CM_PER_TPCC_CLKCTRL   			(0xbc)
#define CM_PER_DCAN0_CLKCTRL   			(0xc0)
#define CM_PER_DCAN1_CLKCTRL   			(0xc4)
#define CM_PER_EPWMSS1_CLKCTRL   		(0xcc)
//#define CM_PER_EMIF_FW_CLKCTRL   		(0xd0)
#define CM_PER_EPWMSS0_CLKCTRL   		(0xd4)
#define CM_PER_EPWMSS2_CLKCTRL   		(0xd8)
#define CM_PER_L3_INSTR_CLKCTRL   		(0xdc)
#define CM_PER_L3_CLKCTRL   			(0xe0)
#define CM_PER_IEEE5000_CLKCTRL   		(0xe4)
#define CM_PER_ICSS_CLKCTRL   			(0xe8)
#define CM_PER_TIMER5_CLKCTRL   		(0xec)
#define CM_PER_TIMER6_CLKCTRL   		(0xf0)
#define CM_PER_MMC1_CLKCTRL   			(0xf4)
#define CM_PER_MMC2_CLKCTRL   			(0xf8)
#define CM_PER_TPTC1_CLKCTRL   			(0xfc)
#define CM_PER_TPTC2_CLKCTRL   			(0x100)
#define CM_PER_SPINLOCK_CLKCTRL   		(0x10c)
#define CM_PER_MAILBOX0_CLKCTRL   		(0x110)
#define CM_PER_L4HS_CLKSTCTRL   		(0x11c)
#define CM_PER_L4HS_CLKCTRL   			(0x120)
//#define CM_PER_MSTR_EXPS_CLKCTRL   		(0x124)
//#define CM_PER_SLV_EXPS_CLKCTRL   		(0x128)
#define CM_PER_OCPWP_L3_CLKSTCTRL   		(0x12c)
#define CM_PER_OCPWP_CLKCTRL   			(0x130)
#define CM_PER_ICSS_CLKSTCTRL   		(0x140)
#define CM_PER_CPSW_CLKSTCTRL   		(0x144)
#define CM_PER_LCDC_CLKSTCTRL   		(0x148)
#define CM_PER_CLKDIV32K_CLKCTRL   		(0x14c)
#define CM_PER_CLK_24MHZ_CLKSTCTRL		(0x150)

#define CM_WKUP_CLKSTCTRL		        (0x400)
#define CM_WKUP_CONTROL_CLKCTRL		        (0x404)
#define CM_WKUP_GPIO0_CLKCTRL		        (0x408)
#define  CM_WKUP_L4WKUP_CLKCTRL		        (0x40C)
#define  CM_WKUP_TIMER0_CLKCTRL		        (0x410)
#define  CM_WKUP_DEBUGSS_CLKCTRL	        (0x414)
#define  CM_L3_AON_CLKSTCTRL		        (0x418)
#define  CM_AUTOIDLE_DPLL_MPU		        (0x41C)
#define  CM_IDLEST_DPLL_MPU		        (0x420)
#define  CM_SSC_DELTAMSTEP_DPLL_MPU	        (0x424)
#define  CM_SSC_MODFREQDIV_DPLL_MPU	        (0x428)
#define  CM_CLKSEL_DPLL_MPU		        (0x42C)
#define  CM_AUTOIDLE_DPLL_DDR		        (0x430)
#define  CM_IDLEST_DPLL_DDR		        (0x434)
#define  CM_SSC_DELTAMSTEP_DPLL_DDR	        (0x438)
#define  CM_SSC_MODFREQDIV_DPLL_DDR	        (0x43C)
#define  CM_CLKSEL_DPLL_DDR		        (0x440)
#define  CM_AUTOIDLE_DPLL_DISP		        (0x444)
#define  CM_IDLEST_DPLL_DISP		        (0x448)
#define  CM_SSC_DELTAMSTEP_DPLL_DISP	        (0x44C)
#define  CM_SSC_MODFREQDIV_DPLL_DISP	        (0x450)
#define  CM_CLKSEL_DPLL_DISP		        (0x454)
#define  CM_AUTOIDLE_DPLL_CORE		        (0x458)
#define  CM_IDLEST_DPLL_CORE                    (0x45C)
#define  CM_SSC_DELTAMSTEP_DPLL_CORE            (0x460)
#define  CM_SSC_MODFREQDIV_DPLL_CORE            (0x464)
#define  CM_CLKSEL_DPLL_CORE                    (0x468)
#define  CM_AUTOIDLE_DPLL_PER                   (0x46C)
#define  CM_IDLEST_DPLL_PER                     (0x470)
#define  CM_SSC_DELTAMSTEP_DPLL_PER             (0x474)
#define  CM_SSC_MODFREQDIV_DPLL_PER             (0x478)
#define  CM_CLKDCOLDO_DPLL_PER                  (0x47C)
#define  CM_DIV_M4_DPLL_CORE                    (0x480)
#define  CM_DIV_M5_DPLL_CORE                    (0x484)
#define  CM_CLKMODE_DPLL_MPU                    (0x488)
#define  CM_CLKMODE_DPLL_PER                    (0x48C)
#define  CM_CLKMODE_DPLL_CORE                   (0x490)
#define  CM_CLKMODE_DPLL_DDR                    (0x494)
#define  CM_CLKMODE_DPLL_DISP                   (0x498)
#define  CM_CLKSEL_DPLL_PERIPH                  (0x49C)
#define  CM_DIV_M2_DPLL_DDR                     (0x4A0)
#define  CM_DIV_M2_DPLL_DISP                    (0x4A4)
#define  CM_DIV_M2_DPLL_MPU                     (0x4A8)
#define  CM_DIV_M2_DPLL_PER                     (0x4AC)
#define  CM_WKUP_WKUP_M3_CLKCTRL                (0x4B0)
#define  CM_WKUP_UART0_CLKCTRL                  (0x4B4)
#define  CM_WKUP_I2C0_CLKCTRL                   (0x4B8)
#define  CM_WKUP_ADC_TSC_CLKCTRL                (0x4BC)
#define  CM_WKUP_SMARTREFLEX0_CLKCTRL           (0x4C0)
#define  CM_WKUP_TIMER1_CLKCTRL                 (0x4C4)
#define  CM_WKUP_SMARTREFLEX1_CLKCTRL           (0x4C8)
#define  CM_L4_WKUP_AON_CLKSTCTRL               (0x4CC)
#define  CM_WKUP_WDT1_CLKCTRL                   (0x4D4)
#define  CM_DIV_M6_DPLL_CORE                    (0x4D8)

#define CM_DPLL_CLKSEL_TIMER7_CLK   		(0x504)
#define CM_DPLL_CLKSEL_TIMER2_CLK   		(0x508)
#define CM_DPLL_CLKSEL_TIMER3_CLK   		(0x50c)
#define CM_DPLL_CLKSEL_TIMER4_CLK   		(0x510)
#define CM_DPLL_CM_MAC_CLKSEL   		(0x514)
#define CM_DPLL_CLKSEL_TIMER5_CLK   		(0x518)
#define CM_DPLL_CLKSEL_TIMER6_CLK   		(0x51c)
#define CM_DPLL_CM_CPTS_RFT_CLKSEL   		(0x520)
#define CM_DPLL_CLKSEL_TIMER1MS_CLK   		(0x528)
#define CM_DPLL_CLKSEL_GFX_FCLK   		(0x52c)
#define CM_DPLL_CLKSEL_ICSS_OCP_CLK   		(0x530)
#define CM_DPLL_CLKSEL_LCDC_PIXEL_CLK   	(0x534)
#define CM_DPLL_CLKSEL_WDT1_CLK   		(0x538)
#define CM_DPLL_CLKSEL_GPIO0_DBCLK   		(0x53c)

#define CM_MPU_CLKSTCTRL			(0x600)
#define CM_MPU_MPU_CLKCTRL			(0x604)

#define CM_CLKOUT_CTRL				(0x700)

#define CM_RTC_RTC_CLKCTRL   			(0x800)
#define CM_RTC_CLKSTCTRL   			(0x804)

#define CM_GFX_L3_CLKSTCTRL			(0x900)
#define CM_GFX_GFX_CLKCTRL			(0x904)
#define CM_GFX_L4LS_GFX_CLKSTCTRL		(0x90c)
#define CM_GFX_MMUCFG_CLKCTRL			(0x910)
#define CM_GFX_MMUDATA_CLKCTRL			(0x914)

#define CM_CEFUSE_CLKSTCTRL			(0xa00)
#define CM_CEFUSE_CEFUSE_CLKCTRL		(0xa20)

#define REVISION_PRM				(0xb00)
#define PRM_IRQSTATUS_MPU			(0xb04)
#define PRM_IRQENABLE_MPU			(0xb08)
#define PRM_IRQSTATUS_M3			(0xb0c)
#define PRM_IRQENABLE_M3			(0xb10)

#define RM_PER_RSTCTRL				(0xc00)
#define PM_PER_PWRSTST				(0xc08)
#define PM_PER_PWRSTCTRL			(0xc0c)

#define RM_WKUP_RSTCTRL				(0xd00)
#define PM_WKUP_PWRSTCTRL			(0xd04)
#define PM_WKUP_PWRSTST				(0xd08)
#define RM_WKUP_RSTST				(0xd0c)

#define PM_MPU_PWRSTCTRL			(0xe00)
#define PM_MPU_PWRSTST				(0xe04)
#define RM_MPU_RSTST				(0xe08)

#define PRM_RSTCTRL				(0xf00)
#define PRM_RSTTIME				(0xf04)
#define PRM_RSTST				(0xf08)
#define PRM_SRAM_COUNT				(0xf0c)
#define PRM_LDO_SRAM_CORE_SETUP			(0xf10)
#define PRM_LDO_SRAM_CORE_CTRL			(0xf14)
#define PRM_LDO_SRAM_MPU_SETUP			(0xf18)
#define PRM_LDO_SRAM_MPU_CTRL			(0xf1c)

#define PM_RTC_PWRSTCTRL			(0x1000)
#define PM_RTC_PWRSTST				(0x1004)

#define PM_GFX_PWRSTCTRL			(0x1100)
#define RM_GFX_RSTCTRL				(0x1104)
#define PM_GFX_PWRSTST				(0x1110)
#define RM_GFX_RSTST				(0x1114)

#define PM_CEFUSE_PWRSTCTRL			(0x1200)
#define PM_CEFUSE_PWRSTST			(0x1204)

static char *regs_name[] = {
	"l4ls_clkstctrl",
	"l3s_clkstctrl",
	"l3_clkstctrl",
	"cpgmac0_clkctrl",
	"lcdc_clkctrl",
	"usb0_clkctrl",
	"tptc0_clkctrl",
	"emif_clkctrl",
	"ocmcram_clkctrl",
	"gpmc_clkctrl",
	"mcasp0_clkctrl",
	"uart5_clkctrl",
	"mmc0_clkctrl",
	"elm_clkctrl",
	"i2c2_clkctrl",
	"i2c1_clkctrl",
	"spi0_clkctrl",
	"spi1_clkctrl",
	"l4ls_clkctrl",
	"mcasp1_clkctrl",
	"uart1_clkctrl",
	"uart2_clkctrl",
	"uart3_clkctrl",
	"uart4_clkctrl",
	"timer7_clkctrl",
	"timer2_clkctrl",
	"timer3_clkctrl",
	"timer4_clkctrl",
	"gpio1_clkctrl",
	"gpio2_clkctrl",
	"gpio3_clkctrl",
	"tpcc_clkctrl",
	"dcan0_clkctrl",
	"dcan1_clkctrl",
	"epwmss1_clkctrl",
	"epwmss0_clkctrl",
	"epwmss2_clkctrl",
	"l3_instr_clkctrl",
	"l3_clkctrl",
	"ieee5000_clkctrl",
	"pru_icss_clkctrl",
	"timer5_clkctrl",
	"timer6_clkctrl",
	"mmc1_clkctrl",
	"mmc2_clkctrl",
	"tptc1_clkctrl",
	"tptc2_clkctrl",
	"spinlock_clkctrl",
	"mailbox0_clkctrl",
	"l4hs_clkstctrl",
	"l4hs_clkctrl",
	"ocpwp_l3_clkstctrl",
	"ocpwp_clkctrl",
	"pru_icss_clkstctrl",
	"cpsw_clkstctrl",
	"lcdc_clkstctrl",
	"clkdiv32k_clkctrl",
	"clk_24mhz_clkstctrl",
	"clksel_timer7_clk",
	"clksel_timer2_clk",
	"clksel_timer3_clk",
	"clksel_timer4_clk",
	"cm_mac_clksel",
	"clksel_timer5_clk",
	"clksel_timer6_clk",
	"cm_cpts_rft_clksel",
	"clksel_timer1ms_clk",
	"clksel_gfx_fclk",
	"clksel_pru_icss_ocp_clk",
	"clksel_lcdc_pixel_clk",
	"clksel_wdt1_clk",
	"clksel_gpio0_dbclk",
	"cm_wkup_clkstctrl",
	"cm_wkup_control_clkctrl",
	"cm_wkup_gpio0_clkctrl",
	"cm_wkup_l4wkup_clkctrl",
	"cm_wkup_timer0_clkctrl",
	"cm_wkup_debugss_clkctrl",
	"cm_l3_aon_clkstctrl",
	"cm_autoidle_dpll_mpu",
	"cm_idlest_dpll_mpu",
	"cm_ssc_deltamstep_dpll_mpu",
	"cm_ssc_modfreqdiv_dpll_mpu",
	"cm_clksel_dpll_mpu",
	"cm_autoidle_dpll_ddr",
	"cm_idlest_dpll_ddr",
	"cm_ssc_deltamstep_dpll_ddr",
	"cm_ssc_modfreqdiv_dpll_ddr",
	"cm_clksel_dpll_ddr",
	"cm_autoidle_dpll_disp",
	"cm_idlest_dpll_disp",
	"cm_ssc_deltamstep_dpll_disp",
	"cm_ssc_modfreqdiv_dpll_disp",
	"cm_clksel_dpll_disp",
	"cm_autoidle_dpll_core",
	"cm_idlest_dpll_core",
	"cm_ssc_deltamstep_dpll_core",
	"cm_ssc_modfreqdiv_dpll_core",
	"cm_clksel_dpll_core",
	"cm_autoidle_dpll_per",
	"cm_idlest_dpll_per",
	"cm_ssc_deltamstep_dpll_per",
	"cm_ssc_modfreqdiv_dpll_per",
	"cm_clkdcoldo_dpll_per",
	"cm_div_m4_dpll_core",
	"cm_div_m5_dpll_core",
	"cm_clkmode_dpll_mpu",
	"cm_clkmode_dpll_per",
	"cm_clkmode_dpll_core",
	"cm_clkmode_dpll_ddr",
	"cm_clkmode_dpll_disp",
	"cm_clksel_dpll_periph",
	"cm_div_m2_dpll_ddr",
	"cm_div_m2_dpll_disp",
	"cm_div_m2_dpll_mpu",
	"cm_div_m2_dpll_per",
	"cm_wkup_wkup_m3_clkctrl",
	"cm_wkup_uart0_clkctrl",
	"cm_wkup_i2c0_clkctrl",
	"cm_wkup_adc_tsc_clkctrl",
	"cm_wkup_smartreflex0_clkctrl",
	"cm_wkup_timer1_clkctrl",
	"cm_wkup_smartreflex1_clkctrl",
	"cm_l4_wkup_aon_clkstctrl",
	"cm_wkup_wdt1_clkctrl",
	"cm_div_m6_dpll_core",
	"cm_rtc_rtc_clkctrl",
	"cm_rtc_clkstctrl",
	"cm_mpu_clkstctrl",
	"cm_mpu_mpu_clkctrl",
	"cm_clkout_ctrl",
	"cm_gfx_l3_clkstctrl",
	"cm_gfx_gfx_clkctrl",
	"cm_gfx_l4ls_gfx_clkstctrl",
	"cm_gfx_mmucfg_clkctrl",
	"cm_gfx_mmudata_clkctrl",
	"cm_cefuse_clkstctrl",
	"cm_cefuse_cefuse_clkctrl",
	"revision_prm",
	"prm_irqstatus_mpu",
	"prm_irqenable_mpu",
	"prm_irqstatus_m3",
	"prm_irqenable_m3",
	"rm_per_rstctrl",
	"pm_per_pwrstst",
	"pm_per_pwrstctrl",
	"rm_wkup_rstctrl",
	"pm_wkup_pwrstctrl",
	"pm_wkup_pwrstst",
	"rm_wkup_rstst",
	"pm_mpu_pwrstctrl",
	"pm_mpu_pwrstst",
	"rm_mpu_rstst",
	"prm_rstctrl",
	"prm_rsttime",
	"prm_rstst",
	"prm_sram_count",
	"prm_ldo_sram_core_setup",
	"prm_ldo_sram_core_ctrl",
	"prm_ldo_sram_mpu_setup",
	"prm_ldo_sram_mpu_ctrl",
	"pm_rtc_pwrstctrl",
	"pm_rtc_pwrstst",
	"pm_gfx_pwrstctrl",
	"rm_gfx_rstctrl",
	"pm_gfx_pwrstst",
	"rm_gfx_rstst",
	"pm_cefuse_pwrstctrl",
	"pm_cefuse_pwrstst",
	NULL,
};
#endif
