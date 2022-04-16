#ifndef __mpc5554_SIU_H__
#define __mpc5554_SIU_H__

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    "MIDR",
    "RSP",
    "SRCR",
    "EISR",
    "DIRER",
    "DIRSR",
    "OSR",
    "ORER",
    "IREER",
    "IFEER",
    "IDFR",
    "ETISR",
    "EIISR",
    "DISR",
    "CCR",
    "ECCR",
    "CARH",
    "CARL",
    "CBRH",
    "CBRL",
    "HLT",
    NULL
};

// 寄存器偏移地址
#define _MIDR 0x4
#define _RSP 0xc
#define _SRCR 0x10
#define _EISR 0x14
#define _DIRER 0x18
#define _DIRSR 0x1c
#define _OSR 0x20
#define _ORER 0x24
#define _IREER 0x28
#define _IFEER 0x2c
#define _IDFR 0x30
#define _ETISR 0x900
#define _EIISR 0x904
#define _DISR 0x908
#define _CCR 0x980
#define _ECCR 0x984
#define _CARH 0x988
#define _CARL 0x98c
#define _CBRH 0x990
#define _CBRL 0x994
#define _HLT  0x9a0

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{
    _MIDR,
    _RSP,
    _SRCR,
    _EISR,
    _DIRER,
    _DIRSR,
    _OSR,
    _ORER,
    _IREER,
    _IFEER,
    _IDFR,
    _ETISR,
    _EIISR,
    _DISR,
    _CCR,
    _ECCR,
    _CARH,
    _CARL,
    _CBRH,
    _CBRL,
    _HLT
};

// 设备寄存器结构体
struct registers
{
    uint32_t MIDR;
    uint32_t RSP;
    uint32_t SRCR;
    uint32_t EISR;
    uint32_t DIRER;
    uint32_t DIRSR;
    uint32_t OSR;
    uint32_t ORER;
    uint32_t IREER;
    uint32_t IFEER;
    uint32_t IDFR;
    uint16_t PCR[231];
    uint8_t GPDO[214];
    uint8_t GPDI[214];
    uint32_t ETISR;
    uint32_t EIISR;
    uint32_t DISR;
    uint32_t CCR;
    uint32_t ECCR;
    uint32_t CARH;
    uint32_t CARL;
    uint32_t CBRH;
    uint32_t CBRL;
    uint32_t HLT;
};

// 设备对象结构
struct mpc5554_siu_device_t
{
    conf_object_t* obj;
    // 接口对象和方法;
    struct registers regs;
};

typedef struct mpc5554_siu_device_t mpc5554_siu_device;

#endif
