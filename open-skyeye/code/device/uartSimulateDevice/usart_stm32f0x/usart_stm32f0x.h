#ifndef __USART_STM32F0X_H__ 
#define __USART_STM32F0X_H__ 

typedef struct reg_usart_cr1{
    uint32_t ue             :1;
    uint32_t usem           :1;
    uint32_t re             :1;
    uint32_t te             :1;
    uint32_t idleie         :1;
    uint32_t rxneie         :1;
    uint32_t tcie           :1;
    uint32_t txeie          :1;
    uint32_t peie           :1;
    uint32_t ps             :1;
    uint32_t pce            :1;
    uint32_t wake           :1;
    uint32_t m0             :1;
    uint32_t mme            :1;
    uint32_t cmie           :1;
    uint32_t over8          :1;
    uint32_t dedt           :5;
    uint32_t deat           :5;
    uint32_t rtoie          :1;
    uint32_t eobie          :1;
    uint32_t m1             :1;
    uint32_t reserved_1     :1;
}usart_cr1_t;
union usart_cr1_reg{
    uint32_t all;
    usart_cr1_t bit;
};

typedef struct reg_usart_isr{
    uint32_t pe             :1;
    uint32_t fe             :1;
    uint32_t nf             :1;
    uint32_t ore            :1;
    uint32_t idle           :1;
    uint32_t rxne           :1;
    uint32_t tc             :1;
    uint32_t txe            :1;
    uint32_t lbdf           :1;
    uint32_t ctsif          :1;
    uint32_t cts            :1;
    uint32_t rtof           :1;
    uint32_t eobf           :1;
    uint32_t reserved_1     :1;
    uint32_t abre           :1;
    uint32_t abrf           :1;
    uint32_t busy           :1;
    uint32_t cmf            :1;
    uint32_t sbkf           :1;
    uint32_t rwu            :1;
    uint32_t wuf            :1;
    uint32_t teack          :1;
    uint32_t reack          :1;
    uint32_t reserved_2     :9;
}usart_isr_t;
union usart_isr_reg{
    uint32_t all;
    usart_isr_t bit;
};

struct registers 
{
	union usart_cr1_reg     usart_cr1;
	uint32_t usart_cr2;
	uint32_t usart_cr3;
	uint32_t usart_brr;
	uint32_t usart_gtpr;
	uint32_t usart_rtor;
	uint32_t usart_rqr;
    union usart_isr_reg     usart_isr;
	uint32_t usart_icr;
	uint32_t usart_rdr;
	uint32_t usart_tdr;
};


struct stm32f0x_device 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;

    conf_object_t *term;
    skyeye_uart_intf *term_iface;

    uart_file_intf *uart_file_iface;
	conf_object_t *uart_file;

	struct registers regs;

	int rfifo;

    int sched_id;
	int dev_enable;
};
typedef struct stm32f0x_device usart_stm32f0x_device;
static char* regs_name[] = 
{
	"USART_CR1",
	"USART_CR2",
	"USART_CR3",
	"USART_BRR",
	"USART_GTPR",
	"USART_RTOR",
	"USART_RQR",
	"USART_ISR",
	"USART_ICR",
	"USART_RDR",
	"USART_TDR",
	NULL
};
#define USART_CR1 0x00
#define USART_CR2 0x04
#define USART_CR3 0x08
#define USART_BRR 0x0c
#define USART_GTPR 0x10
#define USART_RTOR 0x14
#define USART_RQR 0x18
#define USART_ISR 0x1c
#define USART_ICR 0x20
#define USART_RDR 0x24
#define USART_TDR 0x28
#endif  
