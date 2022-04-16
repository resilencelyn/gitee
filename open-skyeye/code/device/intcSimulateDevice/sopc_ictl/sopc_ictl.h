#ifndef __SOPC_ICTL_H__
#define __SOPC_ICTL_H__

struct registers
{
	uint32_t int_enb;      //interrupt enable register
	uint32_t int_enb_ex;   //interrupt enable register
	uint32_t int_sta;      //interrupt status register
	uint32_t int_sta_ex;   //interrupt status register
	uint32_t int_msk;      //interrupt mask register
	uint32_t int_msk_ex;   //interrupt mask register
	uint32_t int_pr;       //interrupt priority register
	uint32_t int_pr_ex;    //interrupt priority register
	uint32_t int_pol;      //interrupt polarity register
	uint32_t int_edg;      //interrupt EDG select register
	uint32_t ints_sta;     //soft set interrupt register
	uint32_t ints_sta_ex;  //soft set interrupt register
	uint32_t intr_sta;     //soft clear interrupt register
	uint32_t intr_sta_ex;  //soft clear interrupt register
	uint32_t irq_vec;      //IRQ vector register
	uint32_t fiq_vec;      //FIQ vector register
};

struct sopc_ictl_device_t
{
	conf_object_t* obj;
	struct {
		int                     num;
		conf_object_t          *obj;
		core_signal_intf       *intf;
	}signal;
	struct registers regs;
	FIFO *ictl_fifo;
};

typedef struct sopc_ictl_device_t sopc_ictl_device;

/*ICTL Registers Base--0x6a200000 Offset*/
#define INT_ENB        0x00
#define INT_ENB_EX     0x04
#define INT_STA        0x08
#define INT_STA_EX     0x0c
#define INT_MSK        0x10
#define INT_MSK_EX     0x14

#define INT_PR         0x18
#define INT_PR_EX      0x1c
#define INT_POL        0x20
#define INT_EDG        0x24
#define INTS_STA       0x28
#define INTS_STA_EX    0x2c
#define INTR_STA       0x30
#define INTR_STA_EX    0x34
#define IRQ_VEC        0x38
#define FIQ_VEC        0x3c

static char* regs_name[] =
{
	"int_enb",
	"int_enb_ex",
	"int_sta",
	"int_sta_ex",
	"int_msk",
	"int_msk_ex",
	"int_pr",
	"int_pr_ex",
	"int_pol",
	"int_edg",
	"ints_sta",
	"ints_sta_ex",
	"intr_sta",
	"intr_sta_ex",
	"irq_vec",
	"fiq_vec",
	NULL
};

static int regs_offset[] =
{
	INT_ENB,
	INT_ENB_EX,
	INT_STA,
	INT_STA_EX,
	INT_MSK,
	INT_MSK_EX,
	INT_PR,
	INT_PR_EX,
	INT_POL,
	INT_EDG,
	INTS_STA,
	INTS_STA_EX,
	INTR_STA,
	INTR_STA_EX,
	IRQ_VEC,
	FIQ_VEC,
};

#endif
