#ifndef __MPC5554_SM29LV160_H__
#define __MPC5554_SM29LV160_H__

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    "REG_INSTR_0",
    "REG_INSTR_1",
    NULL
};

// 寄存器偏移地址
#define _REG_INSTR_0 0xAAA
#define _REG_INSTR_1 0x554

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{
    _REG_INSTR_0,
    _REG_INSTR_1,

};

// 设备寄存器结构体
struct registers
{
    //uint32_t REG_INSTR_0;
    //uint32_t REG_INSTR_1;
    uint32_t REG_INSTR[5];
};

// 设备对象结构
struct mpc5554_sm29lv160_device_t
{
    conf_object_t* obj;
    uint8_t CurrentStatus;
    uint8_t RegInstrWriteIndex;
    uint8_t ReadStatusCount;

    memory_space_intf *image_iface;
    conf_object_t *image;

    FILE *fp;

    // 接口对象和方法;
    struct registers regs;
};

typedef struct mpc5554_sm29lv160_device_t mpc5554_sm29lv160_device;

#endif
