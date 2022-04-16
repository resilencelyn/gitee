#ifndef __ZYNQ7000_CPTIMER_H__
#define __ZYNQ7000_CPTIMER_H__

#define  DEFAULT_CLK  333333330

#define round_down(x, size) (x & -(int32_t)size)

#define bitmask(n) ((uint32_t)(1) << n)

#define bitsmask(n, m) ((~(uint32_t)0 ) >> (m) << (31 - (n) + (m)) >> (31 - (n)))

#define bit(x, n) (((x) >> n) & 1)

#define clear_bit(x, n) ((x) & ~bitmask(n))

#define set_bit(x, n) ((x) | bitmask(n))

#define bits(x, n, m) \
    ({        \
    uint32_t y = (x);    \
    y << (31 - (n)) >> (31 - (n) + (m)); \
    })

#define clear_bits(x, n, m) ((x) & ~bitsmask((n), (m)))

#define set_bits(x, n, m) ((x) | bitsmask((n), (m)))

#define copy_bits(x, y, n, m) (clear_bits((x), (n), (m)) | (bits((y), ((n) - (m)), 0) << m))//将y的[(n-m),0]为移动到x的[n,m]处

#define zero_extend(x, n) (bits((x), (n) - 1, 0))

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    "Global_Timer_Counter_Register0",
    "Global_Timer_Counter_Register1",
    "Global_Timer_Control_Register",
    "Global_Timer_Interrupt_Status_Register",
    "Comparator_Value_Register0",
    "Comparator_Value_Register1",
    "Auto_increment_Register",
    "Private_Timer_Load_Register",
    "Private_Timer_Counter_Register",
    "Private_Timer_Control_Register",
    "Private_Timer_Interrupt_Status_Register",
    "Watchdog_Load_Register",
    "Watchdog_Counter_Register",
    "Watchdog_Control_Register",
    "Watchdog_Interrupt_Status_Register",
    "Watchdog_Reset_Status_Register",
    "Watchdog_Disable_Register",
    NULL
};

#define _GTCOUR0 0x00
#define _GTCOUR1 0x04
#define _GTCONR  0x08
#define _GTISR   0x0c
#define _CVR0    0x10
#define _CVR1    0x14
#define _AIR     0x18

#define _PTLR    0x00
#define _PTCUR   0x04
#define _PTCNR   0x08
#define _PTISR   0x0c
#define _WLR     0x20
#define _WCUR    0x24
#define _WCNR    0x28
#define _WISR    0x2C
#define _WRSR    0x30
#define _WDR     0x34

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{
    _GTCOUR0,
    _GTCOUR1,
    _GTCONR,
    _GTISR,
    _CVR0,
    _CVR1,
    _AIR,
    _PTLR,
    _PTCUR,
    _PTCNR,
    _PTISR,
    _WLR,
    _WCUR,
    _WCNR,
    _WISR,
    _WRSR,
    _WDR
};

union GTCONR_T
{
    uint32_t all;
    struct
    {
        uint32_t TE:1;
        uint32_t CE:1;
        uint32_t IE:1;
        uint32_t bit3:1;
        uint32_t reserved:4;
        uint32_t Prescaler:8;
    }bits;
};

union PTCNR_T
{
    uint32_t all;
    struct
    {
        uint32_t TE:1;
        uint32_t AR:1;
        uint32_t IE:1;
        uint32_t US:5;
        uint32_t Prescaler:8;
        uint32_t SBZP:16;
    }bits;
};

union PTISR_T
{
    uint32_t all;
    struct
    {
        uint32_t bit0:1;
        uint32_t UNK_SBZP:30;
    }bits;
};

union WCNR_T
{
    uint32_t all;
    struct
    {
        uint32_t WE:1;
        uint32_t AR:1;
        uint32_t IE:1;
        uint32_t WM:1;
        uint32_t reserved:4;
        uint32_t Prescaler:8;
    }bits;
};

// 设备寄存器结构体
struct registers
{
    uint32_t GTCOUR0;
    uint32_t GTCOUR1;
    union GTCONR_T GTCONR;
    uint32_t GTISR;
    uint32_t CVR0;
    uint32_t CVR1;
    uint32_t AIR;
    uint32_t PTLR;
    uint32_t PTCUR;
    union PTCNR_T PTCNR;
    union PTISR_T PTISR;
    uint32_t WLR;
    uint32_t WCUR;
    union WCNR_T WCNR;
    uint32_t WISR;
    uint32_t WRSR;
    uint32_t WDR;
};

// 设备对象结构
struct zynq7000_cptimer_device_t
{
    conf_object_t* obj;
    // 接口对象和方法;
    struct registers regs;
    conf_object_t* signal;
    general_signal_intf* signal_iface;
    time_handle_t gthandle;
    time_handle_t pthandle;
    time_handle_t wthandle;
    uint64_t count;
    uint32_t wdr_flag;
    uint32_t global_timer_interrupt;
    uint32_t cpu_private_timer_interrupt;
    uint32_t watchdog_timer_interrupt;
};

typedef struct zynq7000_cptimer_device_t zynq7000_cptimer_device;

#endif
