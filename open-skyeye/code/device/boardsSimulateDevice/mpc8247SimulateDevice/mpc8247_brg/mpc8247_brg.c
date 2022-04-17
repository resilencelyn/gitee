#include <skyeye_lock.h>
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_core.h>
#include <skyeye_signal.h>
#include <skyeye_log.h>
#include <skyeye_dev_bus_intf.h>
#include <skyeye_dev_intf.h>
#include <skyeye_uart_ops.h>
#include <skyeye_system.h>
#include <skyeye_api.h>
#define DEBUG
#include "mpc8247_brg.h"

// initialize brg_register value
static void initRegister(conf_object_t *conf_obj)
{
    mpc8247_brg_device *dev = (mpc8247_brg_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    regs->BRGC[0] = 0;
    regs->BRGC[1] = 0;
    regs->BRGC[2] = 0;
    regs->BRGC[3] = 0;
}

// 寄存器读取操作方法
static exception_t mpc8247_brg_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc8247_brg_device *dev = (mpc8247_brg_device *)conf_obj->obj;

    switch (offset)
    {
        case _BRGC0:
            *(uint32_t *)buf = dev->regs.BRGC[0];
            break;
        case _BRGC1:
            *(uint32_t *)buf = dev->regs.BRGC[1];
            break;
        case _BRGC2:
            *(uint32_t *)buf = dev->regs.BRGC[2];
            break;
        case _BRGC3:
            *(uint32_t *)buf = dev->regs.BRGC[3];
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in mpc8247_brg\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器写入操作方法
static exception_t mpc8247_brg_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc8247_brg_device *dev = (mpc8247_brg_device *)conf_obj->obj;
    uint32_t val = *(uint32_t *)buf;

    if (val & BRGC_RST)
    {
        val &= ~(BRGC_RST);
        initRegister(conf_obj);
    }

    switch (offset)
    {
        case _BRGC0:
            dev->regs.BRGC[0] = val;
            break;
        case _BRGC1:
            dev->regs.BRGC[1] = val;
            break;
        case _BRGC2:
            dev->regs.BRGC[2] = val;
            break;
        case _BRGC3:
            dev->regs.BRGC[3] = val;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in mpc8247_brg\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器接口:根据寄存器ID获取寄存器名
static char* mpc8247_brg_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_name[id];
}

// 寄存器接口:根据寄存器ID设置寄存器值
static exception_t mpc8247_brg_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    mpc8247_brg_device *dev = (mpc8247_brg_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    *((uint32_t*)regs + id) = value;

    return No_exp;
}

// 寄存器接口:获取设备有多少寄存器
static uint32_t mpc8247_brg_get_regnum(conf_object_t *conf_obj)
{
    return sizeof(struct registers) / 4;
}

// 寄存器接口:根据寄存器ID获取寄存器值
static uint32_t mpc8247_brg_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    mpc8247_brg_device *dev = (mpc8247_brg_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    uint32_t* regs_value = (uint32_t*)regs + id;

    return *regs_value;
}

// 寄存器接口:根据寄存器名获取寄存器ID
static uint32_t mpc8247_brg_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
    uint32_t regnum = sizeof(struct registers) / 4;
    int i;

    for (i = 0; i < regnum; i++)
    {
        if (strcmp(name, regs_name[i]) == 0)
            return i;
    }

    return regnum;
}

// 寄存器接口:根据寄存器ID获取寄存器偏移
static uint32_t mpc8247_brg_get_regoffset_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_offset[id];
}

// 实例化一个新设备对象，obj_name是在json文件中配置的名称
static conf_object_t* new_mpc8247_brg(const char *obj_name)
{
    mpc8247_brg_device* dev = skyeye_mm_zero(sizeof(mpc8247_brg_device));
    dev->obj = new_conf_object(obj_name, dev);
    // TODO: 在此添加自定义源码

    initRegister(dev->obj);
    return dev->obj;
}

// 配置实例化的设备对象
static exception_t config_mpc8247_brg(conf_object_t* conf_obj)
{
    mpc8247_brg_device *dev = (mpc8247_brg_device *)conf_obj->obj;
    // TODO: 在此添加自定义源码


    return No_exp;
}

// 释放设备对象来做一些内存释放
static exception_t free_mpc8247_brg(conf_object_t* conf_obj)
{
    mpc8247_brg_device *dev = (mpc8247_brg_device *)conf_obj->obj;

    skyeye_free(conf_obj->objname);
    skyeye_free(conf_obj->obj);
    skyeye_free(conf_obj);

    return No_exp;
}

// 初始化模块类，并在SkyEye加载模块时执行以下代码
void init_mpc8247_brg(void)
{
    // 向SkyEye注册一个设备
    static skyeye_class_t class_data =
    {
        .class_name      = "mpc8247_brg",
        .class_desc      = "mpc8247_brg",
        .new_instance    = new_mpc8247_brg,
        .free_instance   = free_mpc8247_brg,
        .config_instance = config_mpc8247_brg,
        .module_type     = SKYML_OFFCHIP,
    };
    conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);

    // 设置寄存器地址读写方法
    static memory_space_intf io_memory_interface =
    {
        .read  = mpc8247_brg_read,
        .write = mpc8247_brg_write
    };
    SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

    // 注册访问接口(用于SkyEye图形控制接口)
    static skyeye_reg_intf reg_interface =
    {
        .get_regvalue_by_id  = mpc8247_brg_get_regvalue_by_id,
        .get_regname_by_id   = mpc8247_brg_get_regname_by_id,
        .set_regvalue_by_id  = mpc8247_brg_set_regvalue_by_id,
        .get_regnum          = mpc8247_brg_get_regnum,
        .get_regid_by_name   = mpc8247_brg_get_regid_by_name,
        .get_regoffset_by_id = mpc8247_brg_get_regoffset_by_id
    };
    SKY_register_iface(dev_class, SKYEYE_REG_INTF, &reg_interface);

    // 接口方法列表
    static const struct InterfaceDescription ifaces[] =
    {
        (struct InterfaceDescription)
        {
            .name  = MEMORY_SPACE_INTF_NAME,
            .iface = &io_memory_interface
        },
        (struct InterfaceDescription)
        {
            .name  = SKYEYE_REG_INTF,
            .iface = &reg_interface
        },
    };

    // 接口连接方法列表
    static const struct ConnectDescription connects[] =
    {
        /* ConnectDescription */
    };
    class_register_ifaces(dev_class, ifaces);
    class_register_connects(dev_class, connects);
}