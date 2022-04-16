#ifndef __MPC8247_BRG_H__
#define __MPC8247_BRG_H__

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    "BRGC0",
    "BRGC1",
    "BRGC2",
    "BRGC3",
    NULL
};

// 寄存器偏移地址
#define _BRGC0 0x0
#define _BRGC1 0x4
#define _BRGC2 0x8
#define _BRGC3 0xc

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{
    _BRGC0,
    _BRGC1,
    _BRGC2,
    _BRGC3,

};

// 设备寄存器结构体
struct registers
{
    uint32_t BRGC[4];
};

// 设备对象结构
struct mpc8247_brg_device_t
{
    conf_object_t* obj;
    // 接口对象和方法;
    struct registers regs;
};

typedef struct mpc8247_brg_device_t mpc8247_brg_device;

#define BRGC_RST 0x20000  /**< RST bit */

#endif
