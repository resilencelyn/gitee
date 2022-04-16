#ifndef __MPC8247_CLOCKANDRESET_H__
#define __MPC8247_CLOCKANDRESET_H__

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    "SCCR",
    "SCMR",
    "RSR",
    "RMR",
    NULL
};

// 寄存器偏移地址
#define _SCCR 0x0
#define _SCMR 0x8
#define _RSR 0x10
#define _RMR 0x14

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{
    _SCCR,
    _SCMR,
    _RSR,
    _RMR,

};

// 设备寄存器结构体
struct registers
{
    uint32_t SCCR;
    uint32_t SCMR;
    uint32_t RSR;
    uint32_t RMR;
};

// 设备对象结构
struct mpc8247_clockandreset_device_t
{
    conf_object_t* obj;
    // 接口对象和方法;
    struct registers regs;
    // 属性;
    uint32_t intNumber;
};

typedef struct mpc8247_clockandreset_device_t mpc8247_clockandreset_device;

#endif
