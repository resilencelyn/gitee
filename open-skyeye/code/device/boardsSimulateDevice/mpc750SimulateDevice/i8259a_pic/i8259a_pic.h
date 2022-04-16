#ifndef __I8259A_PIC_H__
#define __I8259A_PIC_H__

#include <stdbool.h>

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char *regs_name[] =
    {
        "CW_A0",
        "CW_A1",
};

// 寄存器偏移地址
#define _CW_A0 0x0 // Initialization / Operation Command Words
#define _CW_A1 0x1 // Initialization / Operation Command Words

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
    {
        _CW_A0,
        _CW_A1,
};

// 设备寄存器结构体
struct registers
{
    uint8_t CW_A0;
    uint8_t CW_A1;
};

// 处理机中断信号接口
struct core_signal_s
{
    conf_object_t *obj;
    core_signal_intf *iface;
};

// 设备对象结构
struct i8259a_pic_device_t
{
    conf_object_t *obj;
    // 接口对象和方法;
    struct core_signal_s core_signal;
    struct registers regs;

    uint8_t pr;
    uint8_t imr;
    uint8_t irr;
    uint8_t isr;

    int current_irq;
    int init; // indecate ICW / OCW steps

    // ICW1
    bool ic4;  // 1 = ICW4 needed  0 = no ICW4 needed
    bool sngl; // 1 = single  0 = cascade mode
    bool adi;  // call address interval  1 = interval of 4  2 = interval of 8
    bool ltim; // 1 = level triggered mode  0 = edge triggered mode
    // ICW2
    int vector_base; // interrupt vector address
    // OCW2
    bool eoi;
    bool sl;
    bool r;
    // OCW3
    bool ris;
    bool rr;
    bool p;
    bool smm;
    bool esmm;
    // ICW4
    bool sfnm;
    bool buf;
    bool ms;
    bool aeoi;
    bool upm;
};

typedef struct i8259a_pic_device_t i8259a_pic_device;

#endif
