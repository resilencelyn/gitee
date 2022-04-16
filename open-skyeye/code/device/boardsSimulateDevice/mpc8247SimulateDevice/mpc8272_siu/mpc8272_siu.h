#ifndef __MPC8272_SIU_H__
#define __MPC8272_SIU_H__

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    "siumcr",
    "sypcr",
    "swsr",
    "bcr",
    "ppc_acr",
    "ppc_alrh",
    "ppc_alrl",
    "tescr1",
    "tescr2",
    "pdtea",
    "pdtem",
    NULL
};

// 寄存器偏移地址
#define _SIUMCR 0x0
#define _SYPCR 0x4
#define _SWSR 0x8
#define _BCR 0x24
#define _PPC_ACR 0x28
#define _PPC_ALRH 0x2c
#define _PPC_ALRL 0x30
#define _TESCR1 0x40
#define _TESCR2 0x44
#define _PDTEA 0x50
#define _PDTEM 0x54

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{
    _SIUMCR,
    _SYPCR,
    _SWSR,
    _BCR,
    _PPC_ACR,
    _PPC_ALRH,
    _PPC_ALRL,
    _TESCR1,
    _TESCR2,
    _PDTEA,
    _PDTEM,

};

// 设备寄存器结构体
struct registers
{
    uint32_t siumcr;
    uint32_t sypcr;
    uint16_t swsr;
    uint32_t bcr;
    uint8_t ppc_acr;
    uint32_t ppc_alrh;
    uint32_t ppc_alrl;
    uint32_t tescr1;
    uint32_t tescr2;
    uint32_t pdtea;
    uint8_t pdtem;
};

// 设备对象结构
struct mpc8272_siu_device_t
{
    conf_object_t* obj;
    // 接口对象和方法;
    struct registers regs;
};

typedef struct mpc8272_siu_device_t mpc8272_siu_device;

#endif
