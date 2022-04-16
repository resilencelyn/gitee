#ifndef __ZYNQ7000_UART_H__
#define __ZYNQ7000_UART_H__
#include "fifo_queue.h"

#define round_down(x, size) (x & -(int32_t)size)

#define bitmask(n) ((uint32_t)(1) << n)

#define bitsmask(n, m) ((~(uint32_t)0 ) >> (m) << (31 - (n) + (m)) >> (31 - (n)))

#define bit(x, n) (((x) >> n) & 1)

#define clear_bit(x, n) ((x) & ~bitmask(n))

#define set_bit(x, n) ((x) | bitmask(n))

#define bits(x, n, m) \
    ({        \
    uint32_t y = (x);    \
    y << (31 - (n)) >> (31 - (n) + (m)); \
    })

#define clear_bits(x, n, m) ((x) & ~bitsmask((n), (m)))

#define set_bits(x, n, m) ((x) | bitsmask((n), (m)))

#define copy_bits(x, y, n, m) (clear_bits((x), (n), (m)) | (bits((y), ((n) - (m)), (int)0) << m))//将y的[(n-m),0]为移动到x的[n,m]处

#define zero_extend(x, n) (bits((x), (n) - 1, 0))

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char *regs_name[] =
    {
        "XUARTPS_CR_OFFSET",
        "XUARTPS_MR_OFFSET",
        "XUARTPS_IER_OFFSET",
        "XUARTPS_IDR_OFFSET",
        "XUARTPS_IMR_OFFSET",
        "XUARTPS_ISR_OFFSET",
        "XUARTPS_BAUDGEN_OFFSET",
        "XUARTPS_RXTOUT_OFFSET",
        "XUARTPS_RXWM_OFFSET",
        "XUARTPS_MODEMCR_OFFSET",
        "XUARTPS_MODEMSR_OFFSET",
        "XUARTPS_SR_OFFSET",
        "XUARTPS_FIFO_OFFSET",
        "Baud_rate_divider_reg0",
        "Flow_delay_reg0",
        "Tx_FIFO_trigger_level0",
        NULL};

// 寄存器偏移地址
#define _XUARTPS_CR_OFFSET      0x00
#define _XUARTPS_MR_OFFSET      0x04
#define _XUARTPS_IER_OFFSET     0x08
#define _XUARTPS_IDR_OFFSET     0x0C
#define _XUARTPS_IMR_OFFSET     0x10
#define _XUARTPS_ISR_OFFSET     0x14
#define _XUARTPS_BAUDGEN_OFFSET 0x18
#define _XUARTPS_RXTOUT_OFFSET  0x1C
#define _XUARTPS_RXWM_OFFSET    0x20
#define _XUARTPS_MODEMCR_OFFSET 0x24
#define _XUARTPS_MODEMSR_OFFSET 0x28
#define _XUARTPS_SR_OFFSET      0x2C
#define _XUARTPS_FIFO_OFFSET    0x30
#define _BAUD_RATE_DIVIDER_REG0 0x34
#define _FLOW_DELAY_REG0        0x38
#define _TX_FIFO_TRIGGER_LEVEL0 0x44

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
    {
        _XUARTPS_CR_OFFSET,
        _XUARTPS_MR_OFFSET,
        _XUARTPS_IER_OFFSET,
        _XUARTPS_IDR_OFFSET,
        _XUARTPS_IMR_OFFSET,
        _XUARTPS_ISR_OFFSET,
        _XUARTPS_BAUDGEN_OFFSET,
        _XUARTPS_RXTOUT_OFFSET,
        _XUARTPS_RXWM_OFFSET,
        _XUARTPS_MODEMCR_OFFSET,
        _XUARTPS_MODEMSR_OFFSET,
        _XUARTPS_SR_OFFSET,
        _XUARTPS_FIFO_OFFSET,
        _BAUD_RATE_DIVIDER_REG0,
        _FLOW_DELAY_REG0,
        _TX_FIFO_TRIGGER_LEVEL0,

};

// 设备寄存器结构体
struct registers
{
    uint32_t XUARTPS_CR_OFFSET;
    uint32_t XUARTPS_MR_OFFSET;
    uint32_t XUARTPS_IER_OFFSET;
    uint32_t XUARTPS_IDR_OFFSET;
    uint32_t XUARTPS_IMR_OFFSET;
    uint32_t XUARTPS_ISR_OFFSET;
    uint32_t XUARTPS_BAUDGEN_OFFSET;
    uint32_t XUARTPS_RXTOUT_OFFSET;
    uint32_t XUARTPS_RXWM_OFFSET;
    uint32_t XUARTPS_MODEMCR_OFFSET;
    uint32_t XUARTPS_MODEMSR_OFFSET;
    uint32_t XUARTPS_SR_OFFSET;
    uint32_t XUARTPS_FIFO_OFFSET;
    uint32_t Baud_rate_divider_reg0;
    uint32_t Flow_delay_reg0;
    uint32_t Tx_FIFO_trigger_level0;
};

// 串口设备设备双向传输接口
struct skyeye_uart_s
{
    conf_object_t *obj;
    skyeye_uart_intf *iface;
};

// 串口设备终端输出到数据文件接口
struct uart_file_s
{
    conf_object_t *obj;
    uart_file_intf *iface;
};

// 设备对象结构
struct zynq7000_uart_device_t
{
    conf_object_t *obj;
    skyeye_uart_intf *term_iface;
    conf_object_t *term;
    conf_object_t *uart_file;
    uart_file_intf *uart_file_iface;
    struct
    {
        int num;
        conf_object_t *obj;
        general_signal_intf *intf;
    }signal;
    time_handle_t handle;
    FIFO *fifo;
    struct registers regs;
};

typedef struct zynq7000_uart_device_t zynq7000_uart_device;
#endif
