#ifndef __MPC5554_PIT_H__
#define __MPC5554_PIT_H__

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    "PITMCR",
    "PIT_RTI_LDVAL",
    "PIT_RTI_CVAL",
    "PIT_RTI_TCTRL",
    "PIT_RTI_TFLG",
    "PIT_TIME0_LDVAL0",
    "PIT_TIME0_CVAL0",
    "PIT_TIME0_TCTRL0",
    "PIT_TIME0_TFLG0",
    "PIT_TIME1_LDVAL1",
    "PIT_TIME1_CVAL1",
    "PIT_TIME1_TCTRL1",
    "PIT_TIME1_TFLG1",
    "PIT_TIME2_LDVAL2",
    "PIT_TIME2_CVAL2",
    "PIT_TIME2_TCTRL2",
    "PIT_TIME2_TFLG2",
    "PIT_TIME3_LDVAL3",
    "PIT_TIME3_CVAL3",
    "PIT_TIME3_TCTRL3",
    "PIT_TIME3_TFLG3",
    NULL
};

// 寄存器偏移地址
#define _PITMCR 0x0
#define _PIT_RTI_LDVAL 0xF0
#define _PIT_RTI_CVAL 0xF4
#define _PIT_RTI_TCTRL 0xF8
#define _PIT_RTI_TFLG 0xFC
#define _PIT_TIME0_LDVAL0 0x100
#define _PIT_TIME0_CVAL0 0x104
#define _PIT_TIME0_TCTRL0 0x108
#define _PIT_TIME0_TFLG0 0x10C
#define _PIT_TIME1_LDVAL1 0x110
#define _PIT_TIME1_CVAL1 0x114
#define _PIT_TIME1_TCTRL1 0x118
#define _PIT_TIME1_TFLG1 0x11C
#define _PIT_TIME2_LDVAL2 0x120
#define _PIT_TIME2_CVAL2 0x124
#define _PIT_TIME2_TCTRL2 0x128
#define _PIT_TIME2_TFLG2 0x12C
#define _PIT_TIME3_LDVAL3 0x130
#define _PIT_TIME3_CVAL3 0x134
#define _PIT_TIME3_TCTRL3 0x138
#define _PIT_TIME3_TFLG3 0x13C

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{
    _PITMCR,
    _PIT_RTI_LDVAL,
    _PIT_RTI_CVAL,
    _PIT_RTI_TCTRL,
    _PIT_RTI_TFLG,
    _PIT_TIME0_LDVAL0,
    _PIT_TIME0_CVAL0,
    _PIT_TIME0_TCTRL0,
    _PIT_TIME0_TFLG0,
    _PIT_TIME1_LDVAL1,
    _PIT_TIME1_CVAL1,
    _PIT_TIME1_TCTRL1,
    _PIT_TIME1_TFLG1,
    _PIT_TIME2_LDVAL2,
    _PIT_TIME2_CVAL2,
    _PIT_TIME2_TCTRL2,
    _PIT_TIME2_TFLG2,
    _PIT_TIME3_LDVAL3,
    _PIT_TIME3_CVAL3,
    _PIT_TIME3_TCTRL3,
    _PIT_TIME3_TFLG3,

};

// 设备寄存器结构体
struct registers
{
    uint32_t PITMCR;
    uint32_t PIT_RTI_LDVAL;
    uint32_t PIT_RTI_CVAL;
    uint32_t PIT_RTI_TCTRL;
    uint32_t PIT_RTI_TFLG;
    uint32_t PIT_TIME0_LDVAL0;
    uint32_t PIT_TIME0_CVAL0;
    uint32_t PIT_TIME0_TCTRL0;
    uint32_t PIT_TIME0_TFLG0;
    uint32_t PIT_TIME1_LDVAL1;
    uint32_t PIT_TIME1_CVAL1;
    uint32_t PIT_TIME1_TCTRL1;
    uint32_t PIT_TIME1_TFLG1;
    uint32_t PIT_TIME2_LDVAL2;
    uint32_t PIT_TIME2_CVAL2;
    uint32_t PIT_TIME2_TCTRL2;
    uint32_t PIT_TIME2_TFLG2;
    uint32_t PIT_TIME3_LDVAL3;
    uint32_t PIT_TIME3_CVAL3;
    uint32_t PIT_TIME3_TCTRL3;
    uint32_t PIT_TIME3_TFLG3;
};

// 设备对象结构
struct mpc5554_pit_device_t
{
    conf_object_t* obj;
    // 接口对象和方法;

    conf_object_t* signal;
    general_signal_intf* signal_iface;

    time_handle_t pit_timer0_handle;

    uint8_t pit_module_enable;

    struct registers regs;
};

typedef struct mpc5554_pit_device_t mpc5554_pit_device;

#endif
