#ifndef __MPC8272_CPM_MUX_H__
#define __MPC8272_CPM_MUX_H__

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    "cmxsi2cr",
    "cmxfcr",
    "cmxscr",
    "cmxsmr",
    "cmxuar",
    NULL
};

// 寄存器偏移地址
#define _CMXSI2CR 0x2
#define _CMXFCR 0x4
#define _CMXSCR 0x8
#define _CMXSMR 0xc
#define _CMXUAR 0xe

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{
    _CMXSI2CR,
    _CMXFCR,
    _CMXSCR,
    _CMXSMR,
    _CMXUAR,

};

// 设备寄存器结构体
struct registers
{
    uint8_t cmxsi2cr;
    uint32_t cmxfcr;
    uint32_t cmxscr;
    uint8_t cmxsmr;
    uint16_t cmxuar;
};

// 设备对象结构
struct mpc8272_cpm_mux_device_t
{
    conf_object_t* obj;
    // 接口对象和方法;
    struct registers regs;
};

typedef struct mpc8272_cpm_mux_device_t mpc8272_cpm_mux_device;

#endif
