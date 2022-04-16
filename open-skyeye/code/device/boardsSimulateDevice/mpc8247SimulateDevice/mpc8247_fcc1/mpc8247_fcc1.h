#ifndef __MPC8247_FCC1_H__
#define __MPC8247_FCC1_H__

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    "GFMR",
    "FPSMR",
    "FTODR",
    "FDSR",
    "FCCE",
    "FCCM",
    "FCCS",
    "FTIRR_PHY0",
    "FTIRR_PHY1",
    "FTIRR_PHY2",
    "FTIRR_PHY3",
    "FIRPER",
    "FIRER",
    "FIRSR_HI",
    "FIRSR_LO",
    "GFEMR",
    NULL
};

// 寄存器偏移地址
#define _GFMR 0x0
#define _FPSMR 0x4
#define _FTODR 0x8
#define _FDSR 0xc
#define _FCCE 0x10
#define _FCCM 0x14
#define _FCCS 0x18
#define _FTIRR_PHY0 0x1c
#define _FTIRR_PHY1 0x1d
#define _FTIRR_PHY2 0x1e
#define _FTIRR_PHY3 0x1f

// fcc1 extended register
#define _FIRPER 0x0
#define _FIRER 0x4
#define _FIRSR_HI 0x8
#define _FIRSR_LO 0xc
#define _GFEMR 0x10

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{
    _GFMR,
    _FPSMR,
    _FTODR,
    _FDSR,
    _FCCE,
    _FCCM,
    _FCCS,
    _FTIRR_PHY0,
    _FTIRR_PHY1,
    _FTIRR_PHY2,
    _FTIRR_PHY3,
    _FIRPER,
    _FIRER,
    _FIRSR_HI,
    _FIRSR_LO,
    _GFEMR,

};

// 设备寄存器结构体
struct registers
{
    uint32_t GFMR;
    uint32_t FPSMR;
    uint16_t FTODR;
    uint16_t FDSR;
    uint16_t FCCE;
    uint16_t FCCM;
    uint16_t FCCS;
    uint8_t FTIRR_PHY[4];
    uint32_t FIRPER;
    uint32_t FIRER;
    uint32_t FIRSR_HI;
    uint32_t FIRSR_LO;
    uint8_t GFEMR;
};

// 中断管理器中断信号接口
struct general_signal_s
{
    conf_object_t* obj;
    general_signal_iface_t* iface;
};

// 串口设备终端输出到虚拟串口接口
struct uart_com_s
{
    conf_object_t* obj;
    uart_com_intf* iface;
};

// 设备对象结构
struct mpc8247_fcc1_device_t
{
    conf_object_t* obj;
    // 接口对象和方法;
    struct general_signal_s general_signal;
    struct uart_com_s uart_com;
    struct registers regs;
};

typedef struct mpc8247_fcc1_device_t mpc8247_fcc1_device;

#endif
