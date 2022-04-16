#ifndef __mpc5554_FLASH_H__
#define __mpc5554_FLASH_H__

#define SecNum                  0x9
#define SecAddr                 0x80000
#define LAS_SEC16K_SIZE         0x04000
#define LAS_SEC64k_SIZE         0x10000
#define LAS_SEC48K_SIZE         0x0C000
#define MAS_SEC_SIZE            0x20000
#define HAS_SEC_SIZE            0X20000


// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    "FLASH_MCR",
    "FLASH_LMIR",
    "FLASH_HLR",
    "FLASH_SLMLR",
    "FLASH_LMSR",
    "FLASH_HSR",
    "FLASH_AR",
    "FLASH_BIUCR",
    "FLASH_BIUAPR",
    NULL
};

// 寄存器偏移地址
#define _FLASH_MCR 0x0
#define _FLASH_LMIR 0x4
#define _FLASH_HLR 0x8
#define _FLASH_SLMLR 0xc
#define _FLASH_LMSR 0x10
#define _FLASH_HSR 0x14
#define _FLASH_AR 0x18
#define _FLASH_BIUCR 0x1c
#define _FLASH_BIUAPR 0x20

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{
    _FLASH_MCR,
    _FLASH_LMIR,
    _FLASH_HLR,
    _FLASH_SLMLR,
    _FLASH_LMSR,
    _FLASH_HSR,
    _FLASH_AR,
    _FLASH_BIUCR,
    _FLASH_BIUAPR,

};

// 设备寄存器结构体
struct registers
{
    uint32_t FLASH_MCR;
    uint32_t FLASH_LMIR;
    uint32_t FLASH_HLR;
    uint32_t FLASH_SLMLR;
    uint32_t FLASH_LMSR;
    uint32_t FLASH_HSR;
    uint32_t FLASH_AR;
    uint32_t FLASH_BIUCR;
    uint32_t FLASH_BIUAPR;
};

// 设备对象结构
struct mpc5554_flash_device_t
{
    conf_object_t *obj;
    struct registers regs;

    memory_space_intf *memory_iface;
    conf_object_t *memory;

    FILE *fp;
    uint8_t pgm_flag;

};

typedef struct mpc5554_flash_device_t mpc5554_flash_device;

#define MCR_EHV         (1 << 0)
#define MCR_ERS         (1 << 2)
#define MCR_PEG         (1 << 9)
#define MCR_PGM         (1 << 11)

#endif
