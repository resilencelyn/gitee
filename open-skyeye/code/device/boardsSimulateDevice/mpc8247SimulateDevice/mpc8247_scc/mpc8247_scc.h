#ifndef __MPC8247_SCC_H__
#define __MPC8247_SCC_H__

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    "GSMR_L",
    "GSMR_H",
    "PSMR",
    "RES1"
    "TODR",
    "DSR",
    "SCCE",
    "SCCM",
    "RES2",
    "SCCS",
    "RES3",
    NULL
};

// 寄存器偏移地址
#define _GSMR_L 0x00
#define _GSMR_H 0x04
#define _PSMR   0x08
#define _RES1   0x0a
#define _TODR   0x0c
#define _DSR    0x0e
#define _SCCE   0x10
#define _SCCM   0x14
#define _RES2   0x16
#define _SCCS   0x17
#define _RES3   0x18

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{
    _GSMR_L,
    _GSMR_H,
    _PSMR,
    _RES1,
    _TODR,
    _DSR,
    _SCCE,
    _SCCM,
    _RES2,
    _SCCS,
    _RES3,

};

// 设备寄存器结构体
struct registers
{
    union
    {
        uint32_t all;
        struct
        {
            uint32_t res1 : 1;
            uint32_t EDGE : 2;
            uint32_t TCI : 1;
            uint32_t TSNC : 2;
            uint32_t RINV : 1;
            uint32_t TINV : 1;
            uint32_t TPL : 3;
            uint32_t TPP : 2;
            uint32_t TEND : 1;
            uint32_t TDCR : 2;
            uint32_t RDCR : 2;
            uint32_t RENC : 3;
            uint32_t TENC : 3;
            uint32_t DIAG : 2;
            uint32_t ENR : 1;
            uint32_t ENT : 1;
            uint32_t MODE : 4;
        } bit;
    } GSMR_L;

    union
    {
        uint32_t all;
        struct
        {
            uint32_t res1 : 16;
            uint32_t TCRC : 2;
            uint32_t REVD : 1;
            uint32_t TRX : 1;
            uint32_t TTX : 1;
            uint32_t CDP : 1;
            uint32_t CTSP : 1;
            uint32_t CDS : 1;
            uint32_t CTSS : 1;
            uint32_t TFL : 1;
            uint32_t RFW : 1;
            uint32_t TXSY : 1;
            uint32_t SYNL : 2;
            uint32_t RTSM : 1;
            uint32_t RSYN : 1;
        } bit;
    } GSMR_H;

    uint16_t PSMR;

    union
    {
        uint16_t all;
        struct
        {
            uint16_t TOD : 1;
        } bit;

    } TODR;

    uint16_t DSR;
    uint16_t SCCE;
    uint16_t SCCM;
    uint8_t SCCS;
};

// 中断管理器中断信号接口
struct general_signal_s
{
    conf_object_t* obj;
    general_signal_iface_t* iface;
};

// 串口设备设备双向传输接口
struct skyeye_uart_s
{
    conf_object_t* obj;
    skyeye_uart_intf* iface;
};

// 串口设备终端输出到数据文件接口
struct uart_file_s
{
    conf_object_t* obj;
    uart_file_intf* iface;
};

// 串口设备终端输出到虚拟串口接口
struct uart_com_s
{
    conf_object_t* obj;
    uart_com_intf* iface;
};

// 设备对象结构
struct mpc8247_scc_device_t
{
    conf_object_t* obj;
    // 接口对象和方法;
    struct general_signal_s general_signal;
    struct skyeye_uart_s skyeye_uart;
    struct uart_file_s uart_file;
    struct uart_com_s uart_com;
    struct registers regs;
    // 属性;
    uint32_t int_number;
    uint32_t reverse;
};

typedef struct mpc8247_scc_device_t mpc8247_scc_device;

#endif
