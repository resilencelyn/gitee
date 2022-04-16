#ifndef __mpc5554_ESCI_H__
#define __mpc5554_ESCI_H__

#include "mpc5554_esci.h"
#include "mpc5554_esci_fifo.h"

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    "CR1",
    "CR2",
    "DR",
    "SR",
    "LCR",
    "LTR",
    "LRR",
    "LPR",
    NULL
};

// 寄存器偏移地址
#define _CR1 0x0
#define _CR2 0x4
#define _DR  0x6
#define _SR  0x8
#define _LCR 0xC
#define _LTR 0x10
#define _LRR 0x14
#define _LPR 0x18

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{
    _CR1,
    _CR2,
    _DR,
    _SR,
    _LCR,
    _LTR,
    _LRR,
    _LPR,
};

// 设备寄存器结构体
struct registers
{
    uint32_t CR1;
    uint16_t CR2;
    uint16_t DR;
    uint32_t SR;
    uint32_t LCR;
    uint32_t LTR;
    uint32_t LRR;
    uint32_t LPR;
};

/*中断控制器接口*/
struct general_signal_s
{
    conf_object_t* obj;
    general_signal_iface_t* iface;
};

/*串口终端设备输出到数据文件接口*/
struct uart_file_s
{
    conf_object_t* obj;
    uart_file_intf* iface;
};

struct term_s
{
    conf_object_t *obj;
    skyeye_uart_intf* intf;
};

struct esci_data_t
{
    uint32_t cHead;
    uint32_t cLength;
    uint8_t data[0x100];
};

struct esci_stats_t
{
    uint32_t tx_en      :1;
    uint32_t rx_en      :1;
    uint32_t reserved  :30;
}esci_status;

// 设备对象结构
struct mpc5554_esci_device_t
{
    conf_object_t* obj;
    // 接口对象和方法;

    struct general_signal_s general_signal;
    struct uart_file_s uart_file;
    struct registers regs;
    struct term_s term;

    conf_object_t *uart_com;
    uart_com_intf *uart_com_iface;
    conf_object_t *memory;
    memory_space_intf *memory_iface;
    conf_object_t *edma;
    esci_sendto_edma_intf *edma_iface;

    uint32_t baud_rate;
    int DMATXEN;
    int DMARXEN;
    int InitDMAFlag;
    struct esci_data_t recvBuf;

    uint32_t DMACurTransLen;
    RWLOCK_T lock;
    uint8_t tail[2];
    FILE* fp;

    FIFO* RxFIFO;
    FIFO* TxFIFO;
    FIFO* PRGFIFO;

};

typedef struct mpc5554_esci_device_t mpc5554_esci_device;

#define ESCI_TRAN_EN            (1 << 3)
#define ESCI_RECV_EN            (1 << 2)

#define ESCI_RX_DMA             (1 << 11)
#define ESCI_TX_DMA             (1 << 11)

#define ESCI_SR_RDRF            (0x20)
#define ESCI_SR_TDRE            (0x80)


#endif
