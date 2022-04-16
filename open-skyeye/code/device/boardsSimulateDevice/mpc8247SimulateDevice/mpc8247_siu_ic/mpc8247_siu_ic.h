#ifndef __MPC8247_SIU_IC_H__
#define __MPC8247_SIU_IC_H__

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    "SICR",
    "SIVEC",
    "SIPNR_H",
    "SIPNR_L",
    "SIPRR",
    "SCPRR_H",
    "SCPRR_L",
    "SIMR_H",
    "SIMR_L",
    "SIEXR",
    NULL
};

// 寄存器偏移地址
#define _SICR 0x00
#define _SIVEC 0x04
#define _SIPNR_H 0x08
#define _SIPNR_L 0x0c
#define _SIPRR 0x10
#define _SCPRR_H 0x14
#define _SCPRR_L 0x18
#define _SIMR_H 0x1c
#define _SIMR_L 0x20
#define _SIEXR 0x24

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{
    _SICR,
    _SIVEC,
    _SIPNR_H,
    _SIPNR_L,
    _SIPRR,
    _SCPRR_H,
    _SCPRR_L,
    _SIMR_H,
    _SIMR_L,
    _SIEXR,

};

// 设备寄存器结构体
struct registers
{
    uint16_t SICR;
    uint32_t SIVEC;
    uint32_t SIPNR_H;
    uint32_t SIPNR_L;
    uint32_t SIPRR;
    uint32_t SCPRR_H;
    uint32_t SCPRR_L;
    uint32_t SIMR_H;
    uint32_t SIMR_L;
    uint32_t SIEXR;
};

// 处理机中断信号接口
struct core_signal_s
{
    conf_object_t* obj;
    core_signal_intf* iface;
};

// 设备对象结构
struct mpc8247_siu_ic_device_t
{
    conf_object_t* obj;
    // 接口对象和方法;
    struct core_signal_s core_signal;
    struct registers regs;
    // 属性;
    uint32_t reserve;
    uint32_t int_number;
    // 定时器;
    time_handle_t timer_handle[1];
};

typedef struct mpc8247_siu_ic_device_t mpc8247_siu_ic_device;

#define SIPNRL_SMC1 (0x1 << 12)
#define SIPNRL_SMC2 (0x1 << 11)
#define SIMR_SMC1_PENING (1 << 12)

#define INTERRUPT_BIT_OFFSET(number) (number << 26)

#endif