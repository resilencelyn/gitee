#ifndef __mpc5554_FMPLL_H__
#define __mpc5554_FMPLL_H__

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    "SYNCR",
    "SYNSR",
    NULL
};

// 寄存器偏移地址
#define _SYNCR   0x0
#define _SYNSR   0x4
#define _ESYNCR1 0x8
#define _ESYNCR2 0xC
#define _SYNFMMR 0x18

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{
    _SYNCR,
    _SYNSR,
    _ESYNCR1,
    _ESYNCR2,
    _SYNFMMR
};

// 设备寄存器结构体
struct registers
{
    uint32_t SYNCR;
    uint32_t SYNSR;
    uint32_t ESYNCR1;
    uint32_t ESYNCR2;
    uint32_t SYNFMMR;
};

// 设备对象结构
struct mpc5554_fmpll_device_t
{
    conf_object_t* obj;
    // 接口对象和方法;
    struct registers regs;
};

typedef struct mpc5554_fmpll_device_t mpc5554_fmpll_device;

#endif
