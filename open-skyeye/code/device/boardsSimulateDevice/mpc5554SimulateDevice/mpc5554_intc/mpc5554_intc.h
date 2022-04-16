#ifndef __mpc5554_INTC_H__
#define __mpc5554_INTC_H__

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    "MCR",
    "CPR",
    "IACKR",
    "EOIR",
    "SSCIR0",
    "SSCIR1",
    NULL
};

// 寄存器偏移地址
#define _MCR 0x0
#define _CPR 0x8
#define _IACKR 0x10
#define _EOIR 0x18
#define _SSCIR0 0x20
#define _SSCIR1 0x24



// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{
    _MCR,
    _CPR,
    _IACKR,
    _EOIR,
    _SSCIR0,
    _SSCIR1,

};

struct MCR_tag
{
    uint32_t R;
    struct MCR_B_tag
    {
        uint32_t HVEN :1;
        uint32_t RES2  :2;
        uint32_t VTES  :1;
        uint32_t RES1: 25;
    }B;
};

uint32_t INTC_reserved0;
struct CPR_tag
{
    uint32_t R;

    struct CPR_B_tag
    {
        uint32_t RES1:28;
        uint32_t PRI:4;
    }B;
};

struct IACKR_tag
{
    uint32_t R;

    struct IACKR_B_tag
    {
        uint32_t RES1:2;
        uint32_t INTVEC:9;
        uint32_t VTBA:21;
    }B;
};

struct EOIR_tag
{
    uint32_t R;
    struct EOIR_B_tag
    {
        uint32_t RES1:32;
    }B;
};

// 设备寄存器结构体
struct registers
{
    struct MCR_tag MCR;
    struct CPR_tag CPR;
    struct IACKR_tag IACKR;
    struct EOIR_tag EOIR;
    uint8_t SSCIR[8];
    uint8_t PSR[308];
};

struct core_signal_s
{
    conf_object_t* obj;
    core_signal_intf* iface;
};

struct mpc5554_intc_device_t
{
    conf_object_t* obj;
    double int_times;
    struct core_signal_s core_signal;
    struct registers regs;
};

typedef struct mpc5554_intc_device_t mpc5554_intc_device;

#define IACK_VECTOR_MASK(val)       ((val >> 2) & 0xff)

#endif
