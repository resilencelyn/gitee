#ifndef __MPC8247_MEMORYCONTROLLER_H__
#define __MPC8247_MEMORYCONTROLLER_H__

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    "BR0",
    "OR0",
    "BR1",
    "OR1",
    "BR2",
    "OR2",
    "BR3",
    "OR3",
    "BR4",
    "OR4",
    "BR5",
    "OR5",
    "BR6",
    "OR6",
    "BR7",
    "OR7",
    "MAR",
    "MAMR",
    "MBMR",
    "MCMR",
    "MPTPR",
    "MDR",
    "PSDMR",
    "PURT",
    "PSRT",
    "IMMR",
    "PCIBR0",
    "PCIBR1",
    "SECBR",
    "SECMR",
    "PCIMSK0",
    "PCIMSK1",
    NULL
};

// 寄存器偏移地址
#define _BR0 0x0
#define _OR0 0x4
#define _BR1 0x8
#define _OR1 0xc
#define _BR2 0x10
#define _OR2 0x14
#define _BR3 0x18
#define _OR3 0x1c
#define _BR4 0x20
#define _OR4 0x24
#define _BR5 0x28
#define _OR5 0x2c
#define _BR6 0x30
#define _OR6 0x34
#define _BR7 0x38
#define _OR7 0x3c
#define _MAR 0x68
#define _MAMR 0x70
#define _MBMR 0x74
#define _MCMR 0x78
#define _MPTPR 0x84
#define _MDR 0x88
#define _PSDMR 0x90
#define _PURT 0x98
#define _PSRT 0x9c
#define _IMMR 0xa8
#define _PCIBR0 0xac
#define _PCIBR1 0xb0
#define _SECBR 0xb4
#define _SECMR 0xbc
#define _PCIMSK0 0xc4
#define _PCIMSK1 0xc8

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{
    _BR0,
    _OR0,
    _BR1,
    _OR1,
    _BR2,
    _OR2,
    _BR3,
    _OR3,
    _BR4,
    _OR4,
    _BR5,
    _OR5,
    _BR6,
    _OR6,
    _BR7,
    _OR7,
    _MAR,
    _MAMR,
    _MBMR,
    _MCMR,
    _MPTPR,
    _MDR,
    _PSDMR,
    _PURT,
    _PSRT,
    _IMMR,
    _PCIBR0,
    _PCIBR1,
    _SECBR,
    _SECMR,
    _PCIMSK0,
    _PCIMSK1,

};

// 设备寄存器结构体
struct registers
{
    uint32_t BR0;
    uint32_t OR0;
    uint32_t BR1;
    uint32_t OR1;
    uint32_t BR2;
    uint32_t OR2;
    uint32_t BR3;
    uint32_t OR3;
    uint32_t BR4;
    uint32_t OR4;
    uint32_t BR5;
    uint32_t OR5;
    uint32_t BR6;
    uint32_t OR6;
    uint32_t BR7;
    uint32_t OR7;
    uint32_t MAR;
    uint32_t MAMR;
    uint32_t MBMR;
    uint32_t MCMR;
    uint16_t MPTPR;
    uint32_t MDR;
    uint32_t PSDMR;
    uint8_t PURT;
    uint8_t PSRT;
    uint32_t IMMR;
    uint32_t PCIBR0;
    uint32_t PCIBR1;
    uint32_t SECBR;
    uint32_t SECMR;
    uint32_t PCIMSK0;
    uint32_t PCIMSK1;
};

// 设备对象结构
struct mpc8247_memorycontroller_device_t
{
    conf_object_t* obj;
    // 接口对象和方法;
    struct registers regs;
    // 属性;
    uint32_t intNumber;
};

typedef struct mpc8247_memorycontroller_device_t mpc8247_memorycontroller_device;

#endif
