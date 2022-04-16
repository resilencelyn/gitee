#ifndef __I8254_TIMER_H__
#define __I8254_TIMER_H__

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char *regs_name[] =
    {
        "counter_0",
        "counter_1",
        "counter_2",
        "command",
        NULL};

// 寄存器偏移地址
#define _COUNTER_0 0x0
#define _COUNTER_1 0x1
#define _COUNTER_2 0x2
#define _COMMAND   0x3

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
    {
        _COUNTER_0,
        _COUNTER_1,
        _COUNTER_2,
        _COMMAND,

};

// 设备寄存器结构体
struct registers
{
    uint32_t counter_0;
    uint32_t counter_1;
    uint32_t counter_2;
    uint32_t command;
};

// 中断管理器中断信号接口
struct general_signal_s
{
    conf_object_t *obj;
    general_signal_iface_t *iface;
};

// 设备对象结构
struct i8254_timer_device_t
{
    conf_object_t *obj;
    // 接口对象和方法;
    struct general_signal_s general_signal;
    struct registers regs;
    // 属性;
    uint32_t int_number;
    // 定时器;
    time_handle_t timer_handle[1];

    // Control Word
    int counter; // 0 for Select Counter 0 ; 1 for Select Counter 1 ; 2 for Select Counter 2 ; 3 for Read-Back Command
    int method;  // 0 for unter Latch Command ; 1 for Read/Write least significant byte only ; 2 for read/Write most significant byte only ; 3 for Read/Write least significant byte first, then most significant byte
    int mode;    // 0 for Interrupt On Terminal Count ; 1 for Hardware Retriggerable One-Shot ; 2 for Rate Generator ; 3 for Square Wave Mode ; 4 for Software Triggered Strobe ; 5 for Hardware Triggered Strobe (Retriggerable)
    int format;  // 0 for Binary Counter 16-bits ; 1 for Binary Coded Decimal (BCD) Counter (4 Decades)

    // Read-Back Command
    int count;
    int status;
    int readback_counter;
};

typedef struct i8254_timer_device_t i8254_timer_device;

#endif
