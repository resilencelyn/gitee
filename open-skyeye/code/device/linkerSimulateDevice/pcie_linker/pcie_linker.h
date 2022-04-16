#ifndef __PCIE_LINKER_H__
#define __PCIE_LINKER_H__

#include <skyeye_dev_bus_intf.h>

#define MAX_PCIE_DEV_NUM 16
#define MAX_BAR_NUM 6
// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    NULL
};

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{

};

// 设备寄存器结构体
struct registers
{
};

// 内存地址总线接口
struct memory_space_s
{
    conf_object_t* obj;
    memory_space_iface_t* iface[MAX_BAR_NUM];
};

// 设备对象结构
struct pcie_linker_device_t
{
    conf_object_t* obj;
    conf_object_t* pcie_rc;

    // 接口对象和方法;
    struct memory_space_s memory_space[MAX_PCIE_DEV_NUM];
    struct registers regs;
    pcie_msg_t pcie_dev_info[MAX_PCIE_DEV_NUM];
    pcie_config_intf* pcie_ep_iface[MAX_PCIE_DEV_NUM];
    // 相关参数;

};

typedef struct pcie_linker_device_t pcie_linker_device;

#endif
