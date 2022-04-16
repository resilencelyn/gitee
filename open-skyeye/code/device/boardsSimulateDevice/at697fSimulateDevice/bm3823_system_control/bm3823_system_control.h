#ifndef __BM3823_SYSTEM_CONTROL_H__
#define __BM3823_SYSTEM_CONTROL_H__

#include <skyeye_uart_ops.h>
#include "skyeye_lock.h"

/*memory interface registers*/
typedef union
{
    uint32_t all;
    struct
    {
        uint32_t prrws    :5;
        uint32_t res0     :3;
        uint32_t prwdh    :2;
        uint32_t res1     :1;
        uint32_t prwen    :1;
        uint32_t prwws    :5;
        uint32_t spckpo   :1;
        uint32_t spen     :1;
        uint32_t Ioen     :1;
        uint32_t Iows     :5;
        uint32_t bexc     :1;
        uint32_t brdy     :1;
        uint32_t iowdh    :2;
        uint32_t fdiv     :3;
    };
}mcfg1_reg_t;

typedef union
{
    uint32_t all;
    struct
    {
        uint32_t srrws    :4;
        uint32_t srwdh    :2;
        uint32_t rmw      :1;
        uint32_t brdy     :1;
        uint32_t res      :1;
        uint32_t srbs     :4;
        uint32_t srdis    :1;
        uint32_t sdren    :1;
        uint32_t srwws    :4;
        uint32_t sdrcmd   :2;
        uint32_t sdrcls   :2;
        uint32_t sdrbs    :3;
        uint32_t sdrcas   :1;
        uint32_t trfc     :3;
        uint32_t trp      :1;
        uint32_t sdref    :1;
    };
}mcfg2_reg_t;

typedef union
{
    uint32_t all;
    struct
    {
        uint32_t iosn_oen_f_ws    :4;
        uint32_t oen_iosn_r_ws    :4;
        uint32_t iosn_writen_f_ws :4;
        uint32_t sdrrv            :15;
        uint32_t res0             :1;
        uint32_t writen_iosn_r_ws :4;
    };
}mcfg3_reg_t;

typedef uint32_t failar_reg_t;

typedef union
{
    uint32_t all;
    struct
    {
        uint32_t hsize     :3;
        uint32_t hmast     :4;
        uint32_t rw        :1;
        uint32_t ne        :1;
        uint32_t ce        :1;
        uint32_t res0      :22;
    };
}memstatus_reg_t;

typedef union
{
    uint32_t all;
    struct
    {
        uint32_t mask     :14;
        uint32_t res0      :1;
        uint32_t tag      :14;
        uint32_t res1      :1;
        uint32_t bp        :1;
        uint32_t en        :1;
    };
}wpr_reg_t;

typedef struct
{
    mcfg1_reg_t         mcfg1;
    mcfg2_reg_t         mcfg2;
    mcfg3_reg_t         mcfg3;
    failar_reg_t       failar;
    memstatus_reg_t memstatus;
    wpr_reg_t            wpr1;
    wpr_reg_t            wpr2;
}memory_reg_t;

/*Timer Registers*/
typedef uint32_t timercnt1_t;
typedef uint32_t timerload1_t;

typedef union
{
    uint32_t all;
    struct
    {
        uint32_t en0        :1;
        uint32_t rl0        :1;
        uint32_t ld0        :1;
        uint32_t res0       :1;
        uint32_t wtden0     :1;
        uint32_t res1      :27;
    };
}timerctrl1_reg_t;

typedef uint32_t wdog_reg_t;

typedef uint32_t timercnt2_reg_t;
typedef uint32_t timerload2_t;

typedef union
{
    uint32_t all;
    struct
    {
        uint32_t en1        :1;
        uint32_t rl1        :1;
        uint32_t ld1        :1;
        uint32_t res0       :29;
    };
}timerctrl2_reg_t;

typedef union
{
    uint32_t all;
    struct
    {
        uint32_t scalercnt  :10;
        uint32_t res0       :22;
    };
}scalercnt_reg_t;


typedef union
{
    uint32_t all;
    struct
    {
        uint32_t scalerload :10;
        uint32_t res0       :22;
    };
}scalerload_reg_t;

typedef struct
{
    timercnt1_t         timercnt1;
    timerload1_t       timerload1;
    timerctrl1_reg_t   timerctrl1;
    wdog_reg_t               wdog;
    timercnt2_reg_t     timercnt2;
    timerload2_t       timerload2;
    timerctrl2_reg_t   timerctrl2;
    scalercnt_reg_t     scalercnt;
    scalerload_reg_t   scalerload;
}timer_reg_t;

/*UART Registers*/
typedef union
{
    uint32_t all;
    struct
    {
        uint32_t rtd     :8;
        uint32_t res0   :24;
    };
}data_reg_t;

typedef union
{
    uint32_t all;
    struct
    {
        uint32_t dr    :1;
        uint32_t ts    :1;
        uint32_t th    :1;
        uint32_t br    :1;
        uint32_t ov    :1;
        uint32_t pe    :1;
        uint32_t fe    :1;
        uint32_t res0  :25;
    };
}status_reg_t;

typedef union
{
   uint32_t all;
   struct
   {
        uint32_t re    :1;
        uint32_t te    :1;
        uint32_t ri    :1;
        uint32_t ti    :1;
        uint32_t ps    :1;
        uint32_t pe    :1;
        uint32_t fl    :1;
        uint32_t lb    :1;
        uint32_t ec    :1;
        uint32_t res0 :23;
    };
}control_reg_t;

typedef union
{
    uint32_t all;
    struct
    {
        uint32_t srv   :12;
        uint32_t res0  :20;
    };
}scaler_reg_t;

typedef struct
{
    data_reg_t    uartdata1;
    status_reg_t  uartstatus1;
    control_reg_t uartctrl1;
    scaler_reg_t  uartscaler1;
    data_reg_t    uartdata2;
    status_reg_t  uartstatus2;
    control_reg_t uartctrl2;
    scaler_reg_t  uartscaler2;
}uart_reg_t;

struct registers
{
    memory_reg_t memory_reg;
    timer_reg_t  timer_reg;
    uart_reg_t   uart_reg;
};

struct bm3823_system_control_device_t
{
    conf_object_t* obj;
    struct registers regs;

    struct
    {
        conf_object_t          *obj;
        skyeye_uart_intf       *intf;
    }term1;

    struct
    {
        conf_object_t          *obj;
        skyeye_uart_intf       *intf;
    }term2;

    struct
    {
        conf_object_t          *obj;
        uart_file_intf         *intf;
    }uart_file;

    struct
    {
        int                    num;
        conf_object_t          *obj;
        core_signal_intf       *intf;
        general_signal_intf    *iface;
    }signal;

    int cpu_freq;
    int baudrate;
    int uart1_scaler;
    int uart2_scaler;
    int timer;
    int timer1_handle;
    int timer2_handle;
    RWLOCK_T lock;
};

typedef struct bm3823_system_control_device_t bm3823_system_control_device;

static char* regs_name[] =
{
    "mcfg1",
    "mcfg2",
    "mcfg3",
    "faddr",
    "mstat",
    "wpr1",
    "wpr2",
    "tcnt1",
    "trld1",
    "tctrl1",
    "wdog",
    "tcnt2",
    "trld2",
    "tctrl2",
    "scnt",
    "srld",
    "udata1",
    "usta1",
    "uctrl1",
    "uscal1",
    "udata2",
    "ustat2",
    "uctrl2",
    "uscal2",
     NULL
};


#define    LREGS    0x81000000
#define    MCFG1    0x00
#define    MCFG2    0x04
#define    MCFG3    0x08
#define    FADDR    0x0C
#define    MSTAT    0x10
#define    WPROT1   0x1C
#define    WPROT2   0x20
#define    TCNT1    0x40
#define    TRLD1    0x44
#define    TCTRL1   0x48
#define    WDOG     0x4C
#define    TCNT2    0x50
#define    TRLD2    0x54
#define    TCTRL2   0x58
#define    SCNT     0x60
#define    SRLD     0x64
#define    UDATA1   0x70
#define    USTAT1   0x74
#define    UCTRL1   0x78
#define    USCAL1   0x7c
#define    UDATA2   0x80
#define    USTAT2   0x84
#define    UCTRL2   0x88
#define    USCAL2   0x8c

static int regs_offset[] =
{
    MCFG1,
    MCFG2,
    MCFG3,
    FADDR,
    MSTAT,
    WPROT1,
    WPROT2,
    TCNT1,
    TRLD1,
    TCTRL1,
    WDOG,
    TCNT2,
    TRLD2,
    TCTRL2,
    SCNT,
    SRLD,
    UDATA1,
    USTAT1,
    UCTRL1,
    USCAL1,
    UDATA2,
    USTAT2,
    UCTRL2,
    USCAL2
};


exception_t uart_memory_read(conf_object_t *opaque, generic_address_t offset, uint32_t *val, size_t count);
exception_t uart_memory_write(conf_object_t *opaque, generic_address_t offset, uint32_t data, size_t count);

exception_t intc_memory_write(conf_object_t *opaque, generic_address_t offset, uint32_t data, size_t count);
exception_t intc_memory_read(conf_object_t *opaque, generic_address_t offset, uint32_t *val, size_t count);

exception_t timer_memory_write(conf_object_t *opaque, generic_address_t offset, uint32_t data, size_t count);
exception_t timer_memory_read(conf_object_t *opaque, generic_address_t offset, uint32_t *val, size_t count);

void uart_read_handler(conf_object_t *opaque);
void uart_tick_handler(conf_object_t *opaque);
void uart_interrupt_handler(conf_object_t *opaque);

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

#endif

