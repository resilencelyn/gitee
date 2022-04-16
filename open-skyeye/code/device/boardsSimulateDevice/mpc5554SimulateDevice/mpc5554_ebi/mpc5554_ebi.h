#ifndef __mpc5554_EBI_H__
#define __mpc5554_EBI_H__

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    "MCR",
    "TESR",
    "BMCR",
    "BR0",
    "OR0",
    "BR1",
    "OR1",
    "BR2",
    "OR2",
    "BR3",
    "OR3",
    "CAL_BR0",
    "CAL_OR0",
    "CAL_BR1",
    "CAL_OR1",
    "CAL_BR2",
    "CAL_OR2",
    "CAL_BR3",
    "CAL_OR3",
    NULL
};

// 寄存器偏移地址
#define _MCR 0x0
#define _TESR 0x8
#define _BMCR 0xc
#define _BR0 0x10
#define _OR0 0x14
#define _BR1 0x18
#define _OR1 0x1c
#define _BR2 0x20
#define _OR2 0x24
#define _BR3 0x28
#define _OR3 0x2c
#define _CAL_BR0 0x40
#define _CAL_OR0 0x44
#define _CAL_BR1 0x48
#define _CAL_OR1 0x4c
#define _CAL_BR2 0x50
#define _CAL_OR2 0x54
#define _CAL_BR3 0x58
#define _CAL_OR3 0x5c

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{
    _MCR,
    _TESR,
    _BMCR,
    _BR0,
    _OR0,
    _BR1,
    _OR1,
    _BR2,
    _OR2,
    _BR3,
    _OR3,
    _CAL_BR0,
    _CAL_OR0,
    _CAL_BR1,
    _CAL_OR1,
    _CAL_BR2,
    _CAL_OR2,
    _CAL_BR3,
    _CAL_OR3,

};

// 设备寄存器结构体
struct registers
{
    uint32_t MCR;
    uint32_t TESR;
    uint32_t BMCR;
    uint32_t BR0;
    uint32_t OR0;
    uint32_t BR1;
    uint32_t OR1;
    uint32_t BR2;
    uint32_t OR2;
    uint32_t BR3;
    uint32_t OR3;
    uint32_t CAL_BR0;
    uint32_t CAL_OR0;
    uint32_t CAL_BR1;
    uint32_t CAL_OR1;
    uint32_t CAL_BR2;
    uint32_t CAL_OR2;
    uint32_t CAL_BR3;
    uint32_t CAL_OR3;
};

// 设备对象结构
struct mpc5554_ebi_device_t
{
    conf_object_t* obj;
    // 接口对象和方法;
    struct registers regs;
};

typedef struct mpc5554_ebi_device_t mpc5554_ebi_device;

#endif
