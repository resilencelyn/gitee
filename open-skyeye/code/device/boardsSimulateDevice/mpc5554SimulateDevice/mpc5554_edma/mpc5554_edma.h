#ifndef __mpc5554_EDMA_H__
#define __mpc5554_EDMA_H__


// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    "CR",
    "ESR",
    "ERQRH",
    "ERQRL",
    "EEIRH",
    "EEIRL",
    "SERQR",
    "CERQR",
    "SEEIR",
    "CEEIR",
    "CIRQR",
    "CER",
    "SSBR",
    "CDSBR",
    "IRQRH",
    "IRQRL",
    "ERH",
    "ERL",
    NULL
};

#define CONFIG_DMA_TX           18
#define CONFIG_DMA_RX           19
#define EN_DMA_TX               (1 << 18)
#define EN_DMA_RX               (1 << 19)
#define EN_DMA_TX_RX_CHANNEL    0xc

// 寄存器偏移地址
#define _CR 0x0
#define _ESR 0x4
#define _ERQRH 0x8
#define _ERQRL 0xc
#define _EEIRH 0x10
#define _EEIRL 0x14
#define _SERQR 0x18
#define _CERQR 0x19
#define _SEEIR 0x1a
#define _CEEIR 0x1b
#define _CIRQR 0x1c
#define _CER 0x1d
#define _SSBR 0x1e
#define _CDSBR 0x1f
#define _IRQRH 0x20
#define _IRQRL 0x24
#define _ERH 0x28
#define _ERL 0x2c

#define SADDR_OFF       0x0
#define MS_OFF          0x4
#define NBYTES_OFF      0x8
#define SLAST_OFF       0xC
#define DADDR_OFF       0x10
#define CITER_OFF       0x14
#define DLAST_SGA_OFF   0x18
#define BITER_OFF       0x1C

#define DMATxEn         1
#define DMARxEn         2
#define DMAAllEn        3

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{
    _CR,
    _ESR,
    _ERQRH,
    _ERQRL,
    _EEIRH,
    _EEIRL,
    _SERQR,
    _CERQR,
    _SEEIR,
    _CEEIR,
    _CIRQR,
    _CER,
    _SSBR,
    _CDSBR,
    _IRQRH,
    _IRQRL,
    _ERH,
    _ERL,

};

struct tcd_t
{
    uint32_t SADDR;

    union MS_tag
    {
        uint32_t R;
        struct MS_B_tag
        {
            uint32_t SOFF  :16;
            uint32_t DSIZE :3;
            uint32_t DMOD  :5;
            uint32_t SSIZE :3;
            uint32_t SMOD  :5;
        }B;
    }MS;

    uint32_t NBYTES;
    uint32_t SLAST;
    uint32_t DADDR;

    union CITER_tag
    {
        uint32_t R;
        struct CITER_B_tag
        {
            uint32_t CITER         :15;
            uint32_t CITERE_LINK    :1;
            uint32_t DOFF          :16;
        }B;
    }CITER;

    uint32_t DLAST_SGA;
    union BITER_tag
    {
        uint32_t R;
        struct BITER_B_tag
        {
            uint32_t START             :1;
            uint32_t INT_MAJ           :1;
            uint32_t INT_HALF          :1;
            uint32_t D_REQ             :1;
            uint32_t E_SG              :1;
            uint32_t MAJORE_LINK       :1;
            uint32_t ACTIVE            :1;
            uint32_t DONE              :1;
            uint32_t MAJORLINKCH       :6;
            uint32_t BWC               :2;
            uint32_t BITER             :15;
            uint32_t BITERE_LINK       :1;
        }B;
    }BITER;
};

// 设备寄存器结构体
struct registers
{
    uint32_t CR;
    uint32_t ESR;
    uint32_t ERQRH;
    uint32_t ERQRL;
    uint32_t EEIRH;
    uint32_t EEIRL;


    uint8_t SERQR;
    uint8_t CERQR;
    uint8_t SEEIR;
    uint8_t CEEIR;
    uint8_t CIRQR;
    uint8_t CER;
    uint8_t SSBR;
    uint8_t CDSBR;
    uint32_t IRQRH;
    uint32_t IRQRL;
    uint32_t ERH;
    uint32_t ERL;
    uint8_t CPR[64];
    struct tcd_t TCD[64];
};

struct general_signal_s
{

    conf_object_t* obj;
    general_signal_iface_t* iface;
};

// 设备对象结构
struct mpc5554_edma_device_t
{
    conf_object_t* obj;
    // 接口对象和方法;
    esci_recvfrom_edma_intf *esci_iface;
    conf_object_t *esci;
    memory_space_intf *memory_iface;
    conf_object_t *memory;

    struct general_signal_s general_signal;
    struct registers regs;

    int         RecvDMAConf;
    uint32_t    confTxCnt;
    uint32_t    confRxCnt;
    uint32_t    flag;

    int         ENTxRxCH;
    int         DMAReadFlag;
    uint32_t    DMAReadLength;
    int         RxEnable;
    uint32_t    SciRecvCount;

    RWLOCK_T    dec_lock;
    uint32_t    sciFrameLen;
    uint32_t    sciRecvMaxLen;

    uint32_t    MinorLoopBytes;
    uint32_t    SciRxDMA_src_addr;
    uint32_t    SciRxDMA_dst_addr;
    uint32_t    SciRxDMA_nbytes;

};

typedef struct mpc5554_edma_device_t mpc5554_edma_device;

#endif
