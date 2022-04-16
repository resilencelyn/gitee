#ifndef __MPC8247_IIC_H__
#define __MPC8247_IIC_H__

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    "I2MOD",
    "I2ADD",
    "I2BRG",
    "I2COM",
    "I2CER",
    "I2CMR",
    NULL
};

// 寄存器偏移地址
#define _I2MOD 0x0
#define _I2ADD 0x4
#define _I2BRG 0x8
#define _I2COM 0xc
#define _I2CER 0x10
#define _I2CMR 0x14

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{
    _I2MOD,
    _I2ADD,
    _I2BRG,
    _I2COM,
    _I2CER,
    _I2CMR,

};

// 设备寄存器结构体
struct registers
{
    uint8_t I2MOD;
    uint8_t I2ADD;
    uint8_t I2BRG;
    uint8_t I2COM;
    uint8_t I2CER;
    uint8_t I2CMR;
};

// 中断管理器中断信号接口
struct general_signal_s
{
    conf_object_t* obj;
    general_signal_iface_t* iface;
};

// 通用I2C总线接口
struct i2c_bus_s
{
    conf_object_t* obj;
    i2c_bus_intf* iface;
};

// 设备对象结构
struct mpc8247_iic_device_t
{
    conf_object_t* obj;
    // 接口对象和方法;
    struct general_signal_s general_signal;
    struct i2c_bus_s i2c_bus;
    struct registers regs;
    // 属性;
    uint32_t int_number;
};

typedef struct mpc8247_iic_device_t mpc8247_iic_device;

#endif
