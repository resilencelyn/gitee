#ifndef __MPC8247_IO_H__
#define __MPC8247_IO_H__

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    "pdira",
    "ppara",
    "psora",
    "podra",
    "pdata",
    "pdirb",
    "pparb",
    "psorb",
    "podrb",
    "pdatb",
    "pdirc",
    "pparc",
    "psorc",
    "podrc",
    "pdatc",
    "pdird",
    "ppard",
    "psord",
    "podrd",
    "pdatd",
    NULL
};

// 寄存器偏移地址
#define _PDIRA 0x0
#define _PPARA 0x4
#define _PSORA 0x8
#define _PODRA 0xc
#define _PDATA 0x10
#define _PDIRB 0x20
#define _PPARB 0x24
#define _PSORB 0x28
#define _PODRB 0x2c
#define _PDATB 0x30
#define _PDIRC 0x40
#define _PPARC 0x44
#define _PSORC 0x48
#define _PODRC 0x4c
#define _PDATC 0x50
#define _PDIRD 0x60
#define _PPARD 0x64
#define _PSORD 0x68
#define _PODRD 0x6c
#define _PDATD 0x70

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{
    _PDIRA,
    _PPARA,
    _PSORA,
    _PODRA,
    _PDATA,
    _PDIRB,
    _PPARB,
    _PSORB,
    _PODRB,
    _PDATB,
    _PDIRC,
    _PPARC,
    _PSORC,
    _PODRC,
    _PDATC,
    _PDIRD,
    _PPARD,
    _PSORD,
    _PODRD,
    _PDATD,

};

// 设备寄存器结构体
struct registers
{
    uint32_t pdira;
    uint32_t ppara;
    uint32_t psora;
    uint32_t podra;
    uint32_t pdata;
    uint32_t pdirb;
    uint32_t pparb;
    uint32_t psorb;
    uint32_t podrb;
    uint32_t pdatb;
    uint32_t pdirc;
    uint32_t pparc;
    uint32_t psorc;
    uint32_t podrc;
    uint32_t pdatc;
    uint32_t pdird;
    uint32_t ppard;
    uint32_t psord;
    uint32_t podrd;
    uint32_t pdatd;
};

// 设备对象结构
struct mpc8247_io_device_t
{
    conf_object_t* obj;
    // 接口对象和方法;
    struct registers regs;
};

typedef struct mpc8247_io_device_t mpc8247_io_device;

#endif
