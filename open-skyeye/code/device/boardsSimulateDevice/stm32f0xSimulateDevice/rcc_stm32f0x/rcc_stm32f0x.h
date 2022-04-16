#ifndef __RCC_STM32F0X_H__ 
#define __RCC_STM32F0X_H__ 

typedef struct reg_rcc_cr{
    uint32_t hsi_on          :1;
    uint32_t hsi_rdy         :1;
    uint32_t reserved_1     :1;
    uint32_t hsi_trim        :5;
    uint32_t hsi_cal         :8;
    uint32_t hse_on          :1;
    uint32_t hse_rdy         :1;
    uint32_t hse_byp         :1;
    uint32_t css_on          :1;
    uint32_t reserved_2     :4;
    uint32_t pll_on          :1;
    uint32_t pll_rdy         :1;
    uint32_t reserved_3     :6;
}rcc_cr_t;
union rcc_cr_reg{
    uint32_t all;
    rcc_cr_t bit;
};

typedef struct reg_rcc_cfgr {
    uint32_t sw             :2;
    uint32_t sws            :2;
    uint32_t hpre           :4;
    uint32_t ppre           :3;
    uint32_t reserved_1     :3;
    uint32_t adcpre         :1;
    uint32_t pll_src         :2;
    uint32_t pll_xtpre       :1;
    uint32_t pllmul         :4;
    uint32_t reserved_2     :2;
    uint32_t mco            :4;
    uint32_t mcopre         :3;
    uint32_t pll_nodiv       :1;
}rcc_cfgr_t;
union rcc_cfgr_reg{
    uint32_t all;
    rcc_cfgr_t bit;
};

typedef struct reg_rcc_cir {
    uint32_t lsi_rdyf        :1;
    uint32_t lse_rdyf        :1;
    uint32_t hsi_rdyf        :1;
    uint32_t hse_rdyf        :1;
    uint32_t pll_rdyf        :1;
    uint32_t hsi14_rdyf      :1;
    uint32_t hsi48_rdyf      :1;
    uint32_t cssf           :1;
    uint32_t lsi_rdyie       :1;
    uint32_t lse_rdyie       :1;
    uint32_t hsi_rdyie       :1;
    uint32_t hse_rdyie       :1;
    uint32_t pll_rdyie       :1;
    uint32_t hsi14_rdyie     :1;
    uint32_t hsi48_rdyie     :1;
    uint32_t reserved_1     :1;
    uint32_t lsi_rdyc        :1;
    uint32_t lse_rdyc        :1;
    uint32_t hsi_rdyc        :1;
    uint32_t hse_rdyc        :1;
    uint32_t pll_rdyc        :1;
    uint32_t hsi14_rdyc      :1;
    uint32_t hsi48_rdyc      :1;
    uint32_t cssc           :1;
    uint32_t reserved_2     :8;
}rcc_cir_t;
union rcc_cir_reg{
    uint32_t    all;
    rcc_cir_t   bit;
};

typedef struct reg_rcc_apb2rstr{
    uint32_t syscfg_rst     :1;
    uint32_t reserved_1     :4;
    uint32_t usart6_rst     :1;
    uint32_t usart7_rst     :1;
    uint32_t usart8_rst     :1;
    uint32_t reserved_2     :1;
    uint32_t adc_rst        :1;
    uint32_t reserved_3     :1;
    uint32_t tim1_rst       :1;
    uint32_t spi1_rst       :1;
    uint32_t usart1_rst     :1;
    uint32_t reserved_4     :1;
    uint32_t tim15_rst      :1;
    uint32_t tim16_rst      :1;
    uint32_t tim17_rst      :1;
    uint32_t reserved_5     :3;
    uint32_t sdgmcu_rst     :1;
    uint32_t reserved_6     :9;
}rcc_apb2rstr_t;
union rcc_apb2rstr_reg{
    uint32_t    all;
    rcc_apb2rstr_t   bit;
};

typedef struct reg_rcc_apb1rstr{
    uint32_t tim2_rst       :1;
    uint32_t tim3_rst       :1;
    uint32_t reserved_1     :2;
    uint32_t tim6_rst       :1;
    uint32_t tim7_rst       :1;
    uint32_t reserved_2     :2;
    uint32_t tim14_rst      :1;
    uint32_t reserved_3     :2;
    uint32_t wwdg_rst       :1;
    uint32_t reserved_4     :2;
    uint32_t spi2_rst       :1;
    uint32_t reserved_5     :2;
    uint32_t usart2_rst     :1;
    uint32_t usart3_rst     :1;
    uint32_t usart4_rst     :1;
    uint32_t usart5_rst     :1;
    uint32_t i2c1_rst       :1;
    uint32_t i2c2_rst       :1;
    uint32_t usb_rst        :1;
    uint32_t reserved_6     :1;
    uint32_t can_rst        :1;
    uint32_t reserved_7     :1;
    uint32_t crs_rst        :1;
    uint32_t pwr_rst        :1;
    uint32_t dac_rst        :1;
    uint32_t cec_rxt        :1;
    uint32_t reserved_8     :1;
}rcc_apb1rstr_t;
union rcc_apb1rstr_reg{
    uint32_t    all;
    rcc_apb1rstr_t  bit;
};

typedef struct reg_rcc_ahbenr{
    uint32_t dma_en         :1;
    uint32_t dma2_en        :1;
    uint32_t sram_en        :1;
    uint32_t reserved_1     :1;
    uint32_t flitf_en       :1;
    uint32_t reserved_2     :1;
    uint32_t crc_en         :1;
    uint32_t reserved_3     :10;
    uint32_t iopa_en        :1;
    uint32_t iopb_en        :1;
    uint32_t iopc_en        :1;
    uint32_t iopd_en        :1;
    uint32_t iope_en        :1;
    uint32_t iopf_en        :1;
    uint32_t reserved_4     :1;
    uint32_t tsc_en         :1;
    uint32_t reserved_5     :1;
}rcc_ahbenr_t;
union rcc_ahbenr_reg{
    uint32_t   all;
    rcc_ahbenr_t    bit;
};

typedef struct reg_rcc_bdcr{
    uint32_t lse_on         :1;
    uint32_t lse_rdy        :1;
    uint32_t lse_byp        :1;
    uint32_t lse_drv        :2;
    uint32_t reserved_1     :3;
    uint32_t rtcsel         :2;
    uint32_t reserved_2     :5;
    uint32_t rtc_en         :1;
    uint32_t bd_rst         :1;
    uint32_t reserved_3     :15;
}rcc_bdcr_t;
union rcc_bdcr_reg{
    uint32_t    all;
    rcc_bdcr_t  bit;
};

typedef struct reg_rcc_csr{
    uint32_t lsi_on         :1;
    uint32_t lsi_rdy        :1;
    uint32_t reserved_1     :21;
    uint32_t v18pwr_rstf    :1;
    uint32_t rmvf           :1;
    uint32_t ob_lrstf       :1;
    uint32_t pin_rstf       :1;
    uint32_t por_rstf       :1;
    uint32_t sft_rstf       :1;
    uint32_t iwdg_rstf      :1;
    uint32_t wwdg_rstf      :1;
    uint32_t lpwr_rstf      :1;
}rcc_csr_t;
union rcc_csr_reg{
    uint32_t    all;
    rcc_csr_t   bit;
};

typedef struct reg_rcc_ahbrstr{
    uint32_t reserved_1     :17;
    uint32_t iopa_rst       :1;
    uint32_t iopb_rst       :1;
    uint32_t iopc_rst       :1;
    uint32_t iopd_rst       :1;
    uint32_t iope_rst       :1;
    uint32_t iopf_rst       :1;
    uint32_t reserved_2     :1;
    uint32_t tsc_rst        :1;
    uint32_t reserved_3     :7;
}rcc_ahbrstr_t;
union rcc_ahbrstr_reg{
    uint32_t all;
    rcc_ahbrstr_t bit;
};

typedef struct reg_rcc_cfgr2{
    uint32_t prediv         :4;
    uint32_t reserved_1     :28;
}rcc_cfgr2_t;
union rcc_cfgr2_reg{
    uint32_t    all;
    rcc_cfgr2_t bit;
};

typedef struct reg_rcc_cfgr3{
    uint32_t usart1_sw      :2;
    uint32_t reserved_1     :2;
    uint32_t i2c1_sw        :1;
    uint32_t reserved_2     :1;
    uint32_t cec_sw         :1;
    uint32_t usb_sw         :1;
    uint32_t adc_sw         :1;
    uint32_t reserved_3     :7;
    uint32_t uart2_sw       :2;
    uint32_t uart3_sw       :2;
    uint32_t reserved_4     :12;
}rcc_cfgr3_t;
union rcc_cfgr3_reg{
    uint32_t    all;
    rcc_cfgr3_t bit;
};

typedef struct reg_rcc_cr2{
    uint32_t hsi14_on       :1;
    uint32_t hsi14_rdy      :1;
    uint32_t hsi14_dis      :1;
    uint32_t hsi14_trim     :5;
    uint32_t hsi14_cal      :8;
    uint32_t hsi48_on       :1;
    uint32_t hsi48_rdy      :1;
    uint32_t reserved_1     :6;
    uint32_t hsi48_cal      :8;
}rcc_cr2_t;
union rcc_cr2_reg{
    uint32_t    all;
    rcc_cr2_t   bit;
};

struct registers 
{
    union rcc_cr_reg        rcc_cr;
	union rcc_cfgr_reg      rcc_cfgr;
	union rcc_cir_reg       rcc_cir;
	union rcc_apb2rstr_reg  rcc_apb2rstr;
	union rcc_apb1rstr_reg  rcc_apb1rstr;
	union rcc_ahbenr_reg    rcc_ahbenr;
	union rcc_apb2rstr_reg  rcc_apb2enr;
	union rcc_apb1rstr_reg  rcc_apb1enr;
	union rcc_bdcr_reg      rcc_bdcr;
	union rcc_csr_reg       rcc_csr;
	union rcc_ahbrstr_reg   rcc_ahbrstr;
	union rcc_cfgr2_reg     rcc_cfgr2;
	union rcc_cfgr3_reg     rcc_cfgr3;
    union rcc_cr2_reg       rcc_cr2;
};

struct stm32f0x_device 
{
	conf_object_t* obj;
	core_signal_intf* signal_iface;
	conf_object_t* signal;
	struct registers regs;
    uint32_t hse_clock;
    uint32_t hsi_clock;
    uint32_t hsi48_clock;
    uint32_t lse_clock;
    uint32_t lsi_clock;
    uint32_t adc_clock;
    uint32_t rtc_clock;
    uint32_t sysclock;
};
typedef struct stm32f0x_device rcc_stm32f0x_device;
static char* regs_name[] = 
{
	"RCC_CR",
	"RCC_CFGR",
	"RCC_CIR",
	"RCC_APB2RSTR",
	"RCC_APB1RSTR",
	"RCC_AHBENR",
	"RCC_APB2ENR",
	"RCC_APB1ENR",
	"RCC_BDCR",
	"RCC_CSR",
	"RCC_AHBRSTR",
	"RCC_CFGR2",
	"RCC_CFGR3",
	"RCC_CR2",
	NULL
};
#define RCC_CR 0x00
#define RCC_CFGR 0x04
#define RCC_CIR 0x08
#define RCC_APB2RSTR 0x0c
#define RCC_APB1RSTR 0x10
#define RCC_AHBENR 0x14
#define RCC_APB2ENR 0x18
#define RCC_APB1ENR 0x1c
#define RCC_BDCR 0x20
#define RCC_CSR 0x24
#define RCC_AHBRSTR 0x28
#define RCC_CFGR2 0x2c
#define RCC_CFGR3 0x30
#define RCC_CR2 0x34
#endif  
