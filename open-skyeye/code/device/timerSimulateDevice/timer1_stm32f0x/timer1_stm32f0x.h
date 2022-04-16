#ifndef __TIMER1_STM32F0X_H__ 
#define __TIMER1_STM32F0X_H__ 

typedef struct reg_tim1_cr1{
    uint32_t cen            :1;
    uint32_t udis           :1;
    uint32_t urs            :1;
    uint32_t opm            :1;
    uint32_t dir            :1;
    uint32_t cms            :2;
    uint32_t arpe           :1;
    uint32_t ckd            :2;
    uint32_t reserved_1     :6;
}tim1_cr1_t;
union tim1_cr1_reg{
    uint32_t all;
    tim1_cr1_t bit;
};

typedef struct reg_tim1_cr2{
    uint32_t ccpc           :1;
    uint32_t reserved_1     :1;
    uint32_t ccus           :1;
    uint32_t ccds           :1;
    uint32_t mms            :2;
    uint32_t ti1s           :1;
    uint32_t ois1           :1;
    uint32_t ois1n          :1;
    uint32_t ois2           :1;
    uint32_t ois2n          :1;
    uint32_t ois3           :1;
    uint32_t ois3n          :1;
    uint32_t ois4           :1;
    uint32_t reserved_2     :1;  
}tim1_cr2_t;
union tim1_cr2_reg{
    uint32_t all;
    tim1_cr2_t bit;
};

typedef struct reg_tim1_egr{
    uint32_t ug             :1;
    uint32_t cc1g           :1;
    uint32_t cc2g           :1;
    uint32_t cc3g           :1;
    uint32_t cc4g           :1;
    uint32_t comg           :1;
    uint32_t tg             :1;
    uint32_t bg             :1;
    uint32_t reserved_1     :8;
}tim1_egr_t;
union tim1_egr_reg{
    uint32_t all;
    tim1_egr_t bit;
};

typedef struct reg_tim1_dier{
    uint32_t uie            :1;
    uint32_t cc1ie          :1;
    uint32_t cc2ie          :1;
    uint32_t cc3ie          :1;
    uint32_t cc4ie          :1;
    uint32_t comie          :1;
    uint32_t tie            :1;
    uint32_t bie            :1;
    uint32_t ude            :1;
    uint32_t cc1de          :1;
    uint32_t cc2de          :1;
    uint32_t cc3de          :1;
    uint32_t cc4de          :1;
    uint32_t comde          :1;
    uint32_t tde            :1;
    uint32_t reserved_1     :1;
}tim1_dier_t;
union tim1_dier_reg{
    uint32_t all;
    tim1_dier_t bit;
};

struct registers 
{
	union tim1_cr1_reg      tim1_cr1;
	union tim1_cr2_reg      tim1_cr2;
	uint32_t tim1_smcr;
	union tim1_dier_reg     tim1_dier;
	uint32_t tim1_sr;
	union tim1_egr_reg      tim1_egr;
	uint32_t tim1_ccmr1;
	uint32_t tim1_ccmr2;
	uint32_t tim1_ccer;
	uint32_t tim1_cnt;
	uint32_t tim1_psc;
	uint32_t tim1_arr;
	uint32_t tim1_rcr;
	uint32_t tim1_ccr1;
	uint32_t tim1_ccr2;
	uint32_t tim1_ccr3;
	uint32_t tim1_ccr4;
	uint32_t tim1_bdtr;
	uint32_t tim1_dcr;
	uint32_t tim1_dmar;
};


struct stm32f0x_device 
{
	conf_object_t* obj;
    conf_object_t* core;
	core_signal_intf* signal_iface;
	conf_object_t* signal;
    time_handle_t *handle;

    conf_object_t *clock;
    get_clock_frequency_t *clock_frequency;

	struct registers regs;

    uint32_t scale;
    uint32_t psc;
    int sched_id;
	
};
typedef struct stm32f0x_device timer1_stm32f0x_device;
static char* regs_name[] = 
{
	"TIM1_CR1",
	"TIM1_CR2",
	"TIM1_SMCR",
	"TIM1_DIER",
	"TIM1_SR",
	"TIM1_EGR",
	"TIM1_CCMR1",
	"TIM1_CCMR2",
	"TIM1_CCER",
	"TIM1_CNT",
	"TIM1_PSC",
	"TIM1_ARR",
	"TIM1_RCR",
	"TIM1_CCR1",
	"TIM1_CCR2",
	"TIM1_CCR3",
	"TIM1_CCR4",
	"TIM1_BDTR",
	"TIM1_DCR",
	"TIM1_DMAR",
	NULL
};
#define TIM1_CR1 0x00
#define TIM1_CR2 0x04
#define TIM1_SMCR 0x08
#define TIM1_DIER 0x0c
#define TIM1_SR 0x10
#define TIM1_EGR 0x14
#define TIM1_CCMR1 0x18
#define TIM1_CCMR2 0x1c
#define TIM1_CCER 0x20
#define TIM1_CNT 0x24
#define TIM1_PSC 0x28
#define TIM1_ARR 0x2c
#define TIM1_RCR 0x30
#define TIM1_CCR1 0x34
#define TIM1_CCR2 0x38
#define TIM1_CCR3 0x3c
#define TIM1_CCR4 0x40
#define TIM1_BDTR 0x44
#define TIM1_DCR 0x48
#define TIM1_DMAR 0x4c
#endif  
