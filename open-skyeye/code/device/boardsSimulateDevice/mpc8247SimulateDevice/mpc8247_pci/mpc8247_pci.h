#ifndef __MPC8247_PCI_H__
#define __MPC8247_PCI_H__

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    "OMISR",
    "OMIMR",
    "IFQPR",
    "OFQPR",
    "IMR0",
    "IMR1",
    "OMR0",
    "OMR1",
    "ODR",
    "IDR",
    "IMISR",
    "IMIMR",
    "IFHPR",
    "IFTPR",
    "IPHPR",
    "IPTPR",
    "OFHPR",
    "OFTPR",
    "OPHPR",
    "OPTPR",
    "MUCR",
    "QBAR",
    "DMAMR0",
    "DMAMR1",
    "DMAMR2",
    "DMAMR3",
    "DMASR0",
    "DMASR1",
    "DMASR2",
    "DMASR3",
    "DMACDAR0",
    "DMACDAR1",
    "DMACDAR2",
    "DMACDAR3",
    "DMASAR0",
    "DMASAR1",
    "DMASAR2",
    "DMASAR3",
    "DMADAR0",
    "DMADAR1",
    "DMADAR2",
    "DMADAR3",
    "DMABCR0",
    "DMABCR1",
    "DMABCR2",
    "DMABCR3",
    "DMANDAR0",
    "DMANDAR1",
    "DMANDAR2",
    "DMANDAR3",
    "POTAR0",
    "POTAR1",
    "POTAR2",
    "POBAR0",
    "POBAR1",
    "POBAR2",
    "POCMR0",
    "POCMR1",
    "POCMR2",
    "PTCR",
    "GPCR",
    "PCI_GCR",
    "ESR",
    "EMR",
    "ECR",
    "PCI_EACR",
    "PCI_EDCR",
    "PCI_ECCR",
    "PITAR1",
    "PIBAR1",
    "PICMR1",
    "PITAR0",
    "PIBAR0",
    "PICMR0",
    "CFG_ADDR",
    "CFG_DATA",
    "INT_ACK",
    NULL
};

// 寄存器偏移地址
#define _OMISR 0x0
#define _OMIMR 0x4
#define _IFQPR 0x10
#define _OFQPR 0x14
#define _IMR0 0x20
#define _IMR1 0x24
#define _OMR0 0x28
#define _OMR1 0x2c
#define _ODR 0x30
#define _IDR 0x38
#define _IMISR 0x50
#define _IMIMR 0x54
#define _IFHPR 0x70
#define _IFTPR 0x78
#define _IPHPR 0x80
#define _IPTPR 0x88
#define _OFHPR 0x90
#define _OFTPR 0x98
#define _OPHPR 0xa0
#define _OPTPR 0xa8
#define _MUCR 0xb4
#define _QBAR 0xc0
#define _DMAMR0 0xd0
#define _DMAMR1 0x150
#define _DMAMR2 0x1d0
#define _DMAMR3 0x250
#define _DMASR0 0xd4
#define _DMASR1 0x154
#define _DMASR2 0x1d4
#define _DMASR3 0x254
#define _DMACDAR0 0xd8
#define _DMACDAR1 0x158
#define _DMACDAR2 0x1d8
#define _DMACDAR3 0x258
#define _DMASAR0 0xe0
#define _DMASAR1 0x160
#define _DMASAR2 0x1e0
#define _DMASAR3 0x260
#define _DMADAR0 0xe8
#define _DMADAR1 0x168
#define _DMADAR2 0x1e8
#define _DMADAR3 0x268
#define _DMABCR0 0xf0
#define _DMABCR1 0x170
#define _DMABCR2 0x1f0
#define _DMABCR3 0x270
#define _DMANDAR0 0xf4
#define _DMANDAR1 0x174
#define _DMANDAR2 0x1f4
#define _DMANDAR3 0x274
#define _POTAR0 0x3d0
#define _POTAR1 0x3e8
#define _POTAR2 0x400
#define _POBAR0 0x3d8
#define _POBAR1 0x3f0
#define _POBAR2 0x408
#define _POCMR0 0x3e0
#define _POCMR1 0x3f8
#define _POCMR2 0x410
#define _PTCR 0x448
#define _GPCR 0x44c
#define _PCI_GCR 0x450
#define _ESR 0x454
#define _EMR 0x458
#define _ECR 0x45c
#define _PCI_EACR 0x460
#define _PCI_EDCR 0x468
#define _PCI_ECCR 0x470
#define _PITAR1 0x4a0
#define _PIBAR1 0x4a8
#define _PICMR1 0x4b0
#define _PITAR0 0x4b8
#define _PIBAR0 0x4c0
#define _PICMR0 0x4c8
#define _CFG_ADDR 0x4d0
#define _CFG_DATA 0x4d4
#define _INT_ACK 0x4d8

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{
    _OMISR,
    _OMIMR,
    _IFQPR,
    _OFQPR,
    _IMR0,
    _IMR1,
    _OMR0,
    _OMR1,
    _ODR,
    _IDR,
    _IMISR,
    _IMIMR,
    _IFHPR,
    _IFTPR,
    _IPHPR,
    _IPTPR,
    _OFHPR,
    _OFTPR,
    _OPHPR,
    _OPTPR,
    _MUCR,
    _QBAR,
    _DMAMR0,
    _DMAMR1,
    _DMAMR2,
    _DMAMR3,
    _DMASR0,
    _DMASR1,
    _DMASR2,
    _DMASR3,
    _DMACDAR0,
    _DMACDAR1,
    _DMACDAR2,
    _DMACDAR3,
    _DMASAR0,
    _DMASAR1,
    _DMASAR2,
    _DMASAR3,
    _DMADAR0,
    _DMADAR1,
    _DMADAR2,
    _DMADAR3,
    _DMABCR0,
    _DMABCR1,
    _DMABCR2,
    _DMABCR3,
    _DMANDAR0,
    _DMANDAR1,
    _DMANDAR2,
    _DMANDAR3,
    _POTAR0,
    _POTAR1,
    _POTAR2,
    _POBAR0,
    _POBAR1,
    _POBAR2,
    _POCMR0,
    _POCMR1,
    _POCMR2,
    _PTCR,
    _GPCR,
    _PCI_GCR,
    _ESR,
    _EMR,
    _ECR,
    _PCI_EACR,
    _PCI_EDCR,
    _PCI_ECCR,
    _PITAR1,
    _PIBAR1,
    _PICMR1,
    _PITAR0,
    _PIBAR0,
    _PICMR0,
    _CFG_ADDR,
    _CFG_DATA,
    _INT_ACK,

};

// 设备寄存器结构体
struct registers
{
    uint32_t OMISR;
    uint32_t OMIMR;
    uint32_t IFQPR;
    uint32_t OFQPR;
    uint32_t IMR0;
    uint32_t IMR1;
    uint32_t OMR0;
    uint32_t OMR1;
    uint32_t ODR;
    uint32_t IDR;
    uint32_t IMISR;
    uint32_t IMIMR;
    uint32_t IFHPR;
    uint32_t IFTPR;
    uint32_t IPHPR;
    uint32_t IPTPR;
    uint32_t OFHPR;
    uint32_t OFTPR;
    uint32_t OPHPR;
    uint32_t OPTPR;
    uint32_t MUCR;
    uint32_t QBAR;
    uint32_t DMAMR[4];
    uint32_t DMASR[4];
    uint32_t DMACDAR[4];
    uint32_t DMASAR[4];
    uint32_t DMADAR[4];
    uint32_t DMABCR[4];
    uint32_t DMANDAR[4];
    uint32_t POTAR[3];
    uint32_t POBAR[3];
    uint32_t POCMR[3];
    uint32_t PTCR;
    uint32_t GPCR;
    uint32_t PCI_GCR;
    uint32_t ESR;
    uint32_t EMR;
    uint32_t ECR;
    uint32_t PCI_EACR;
    uint32_t PCI_EDCR;
    uint32_t PCI_ECCR;
    uint32_t PITAR1;
    uint32_t PIBAR1;
    uint32_t PICMR1;
    uint32_t PITAR0;
    uint32_t PIBAR0;
    uint32_t PICMR0;
    uint32_t CFG_ADDR;
    uint32_t CFG_DATA;
    uint32_t INT_ACK;
};

// 处理机中断信号接口
struct core_signal_s
{
    conf_object_t* obj;
    core_signal_intf* iface;
};

// 设备对象结构
struct mpc8247_pci_device_t
{
    conf_object_t* obj;
    // 接口对象和方法;
    struct core_signal_s core_signal;
    struct registers regs;
    // 属性;
    uint32_t intNumber;
};

typedef struct mpc8247_pci_device_t mpc8247_pci_device;

#endif
