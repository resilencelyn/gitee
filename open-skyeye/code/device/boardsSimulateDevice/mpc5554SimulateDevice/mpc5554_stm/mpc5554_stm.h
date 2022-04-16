#ifndef __MPC5554_STM_H__
#define __MPC5554_STM_H__

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    "STM_CR",
    "STM_CNT",
    "STM_CCR0",
    "STM_CIR0",
    "STM_CMP0",
    "STM_CCR1",
    "STM_CIR1",
    "STM_CMP1",
    "STM_CCR2",
    "STM_CIR2",
    "STM_CMP2",
    "STM_CCR3",
    "STM_CIR3",
    "STM_CMP3",
    NULL
};

// 寄存器偏移地址
#define _STM_CR 0
#define _STM_CNT 0x4
#define _STM_CCR0 0x10
#define _STM_CIR0 0x14
#define _STM_CMP0 0x18
#define _STM_CCR1 0x20
#define _STM_CIR1 0x24
#define _STM_CMP1 0x28
#define _STM_CCR2 0x30
#define _STM_CIR2 0x34
#define _STM_CMP2 0x38
#define _STM_CCR3 0x40
#define _STM_CIR3 0x44
#define _STM_CMP3 0x48

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{
    _STM_CR,
    _STM_CNT,
    _STM_CCR0,
    _STM_CIR0,
    _STM_CMP0,
    _STM_CCR1,
    _STM_CIR1,
    _STM_CMP1,
    _STM_CCR2,
    _STM_CIR2,
    _STM_CMP2,
    _STM_CCR3,
    _STM_CIR3,
    _STM_CMP3,

};

// 设备寄存器结构体
struct registers
{
    uint32_t STM_CR;
    uint32_t STM_CNT;
    uint32_t STM_CCR0;
    uint32_t STM_CIR0;
    uint32_t STM_CMP0;
    uint32_t STM_CCR1;
    uint32_t STM_CIR1;
    uint32_t STM_CMP1;
    uint32_t STM_CCR2;
    uint32_t STM_CIR2;
    uint32_t STM_CMP2;
    uint32_t STM_CCR3;
    uint32_t STM_CIR3;
    uint32_t STM_CMP3;
};

// 设备对象结构
struct mpc5554_stm_device_t
{
    conf_object_t* obj;
    // 接口对象和方法;
    struct registers regs;

    conf_object_t* signal;
    general_signal_intf* signal_iface;

    uint8_t val_cr_cps;
    uint8_t stm_enable;
    int  stm_timer0_handle;
};

typedef struct mpc5554_stm_device_t mpc5554_stm_device;

#endif
