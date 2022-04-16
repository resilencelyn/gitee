#ifndef _BM3823_INTC_H__
#define _BM3823_INTC_H__

#include "bm3823_intc_fifo.h"
#include "skyeye_lock.h"


typedef union
{
    uint32_t all;
    struct
    {
        uint32_t ics      :2;
        uint32_t dcs      :2;
        uint32_t ifs      :1;
        uint32_t df       :1;
        uint32_t res0     :8;
        uint32_t dp       :1;
        uint32_t ip       :1;
        uint32_t ib       :1;
        uint32_t res1     :4;
        uint32_t fi       :1;
        uint32_t fd       :1;
        uint32_t ds       :1;
        uint32_t dsts     :2;
        uint32_t ists     :2;
        uint32_t irpl     :2;
        uint32_t drpl     :2;
    };
}cachectrl_reg_t;

typedef union
{
    uint32_t all;
    struct
    {
        uint32_t res0     :1;
        uint32_t imask    :15;
        uint32_t res1     :1;
        uint32_t ilevel   :15;
    };
}irq1mask_pri_reg_t;

typedef union
{
    uint32_t all;
    struct
    {
        uint32_t res0      :1;
        uint32_t ipend    :15;
        uint32_t res1      :1;
        uint32_t iforce   :15;
    };
}irq1force_pend_reg_t;

typedef union
{
    uint32_t all;
    struct
    {
        uint32_t INT1   :4;
        uint32_t INT2   :4;
        uint32_t INT3   :4;
        uint32_t INT4   :4;
        uint32_t INT5   :4;
        uint32_t INT6   :4;
        uint32_t res0   :8;
    };
}irq1subpri_reg_t;

typedef union
{
    uint32_t all;
    struct
    {
        uint32_t res0   :1;
        uint32_t iclear :15;
        uint32_t res1   :16;
    };
}irq1clear_reg_t;

typedef union
{
    uint32_t all;
    struct
    {
        uint32_t imask    :26;
        uint32_t res0     :6;
    };
}irq2mask_reg_t;

typedef union
{
    uint32_t all;
    struct
    {
        uint32_t ipend2    :26;
        uint32_t res0      :6;
    };
}irq2pend_reg_t;

typedef union
{
    uint32_t all;
    struct
    {
        uint32_t ilr       :5;
        uint32_t ip        :1;
        uint32_t res0      :26;
    }irq2state;
    struct
    {
        uint32_t iclear2   :26;
        uint32_t res0      :6;
    }irq2clear;
}irq2state_clear_reg_t;

typedef uint32_t exirgconfig_reg_t;

struct registers
{
    cachectrl_reg_t         cachectrl;
    irq1mask_pri_reg_t      irq1mask_pri;
    irq1force_pend_reg_t    irq1force_pend;
    irq1subpri_reg_t        irq1subpri;
    irq1clear_reg_t         irq1clear;
    irq2mask_reg_t          irq2mask;
    irq2pend_reg_t          irq2pend;
    irq2state_clear_reg_t   irq2state_clear;
    exirgconfig_reg_t       exirqconfig;
};

struct bm3823_intc_device_t
{

    conf_object_t* obj;
    struct registers regs;
    struct
    {
        int                    num;
        conf_object_t          *obj;
        core_signal_intf       *intf;
    }signal;

    FIFO *fifo;
    RWLOCK_T lock;
};

typedef struct bm3823_intc_device_t bm3823_intc_device;

/*Interrupt Registers Offset*/
#define     CACHECTRL           0x14
#define     IRQ1MASK_PRI        0x90
#define     IRQ1FORCE_PEND      0x94
#define     IRQ1SUBPRI          0x98
#define     IRQ1CLEAR           0x9C
#define     IRQ2MASK            0xB0
#define     IRQ2PEND            0xB4
#define     IRQ2STATE_CLEAR     0xB8
#define     EXIRQCONFIG        0x3C0

static char* regs_name[] =
{
    "cachectrl",
    "irq1mask_pri",
    "irq1force_pend",
    "irq1subpri",
    "irq1clear",
    "irq2mas",
    "irq2pend",
    "irq2state_clear",
    "exirqconfig",
    NULL
};

enum Interrupt_Number
{
    IRQ_NUM_HARDWARE_ERROR = 1,
    IRQ_NUM_UART2,
    IRQ_NUM_UART1,
    IRQ_NUM_EX0,
    IRQ_NUM_EX1,
    IRQ_NUM_EX2,
    IRQ_NUM_EX3,
    IRQ_NUM_EX4,
    IRQ_NUM_EX5,
    IRQ_NUM_INTC_II,
    IRQ_NUM_DSU_TR_BUF,
    IRQ_NUM_PCI,
    IRQ_NUM_TIMER2,
    IRQ_NUM_TIMER1,
    IRQ_NUM_NMI
};

#define IPEND(n)          ((dev->regs.irq1force_pend.ipend >> (n - 1)) & 0x1)
#define IMASK(n)          ((dev->regs.irq1mask_pri.imask >> (n - 1)) & 0x1)
#define IFORCE(n)         ((dev->regs.irq1force_pend.iforce >> (n - 1)) & 0x1)
#define ILEVEL(n)         ((dev->regs.irq1mask_pri.ilevel >> (n - 1)) & 0x1)

#define SET_PEND(n)       (dev->regs.irq1force_pend.ipend |= (1 << (n - 1)))
#define CLR_PEND(n)       (dev->regs.irq1force_pend.ipend &= ~(1 << (n - 1)))
#define SET_FORCE(n)       (dev->regs.irq1force_pend.iforce |= (1 << (n - 1)))
#define CLR_FORCE(n)       (dev->regs.irq1force_pend.iforce &= ~(1 << (n - 1)))

void signal_interrupt_to_cpu(conf_object_t *opaque);
void interrupt_request_handle(conf_object_t *opaque, int irq_num);

static int regs_offset[] =
{
    IRQ1MASK_PRI,
    IRQ1FORCE_PEND,
    IRQ1SUBPRI,
    IRQ1CLEAR,
    IRQ2MASK,
    IRQ2PEND,
    IRQ2STATE_CLEAR,
    EXIRQCONFIG
};

#define MAX_FIFO_NUM  0x40000

#endif

