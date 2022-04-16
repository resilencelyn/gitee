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
#include "mpc8247_memorycontroller.h"

// 寄存器读取操作方法
static exception_t mpc8247_memorycontroller_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc8247_memorycontroller_device *dev = (mpc8247_memorycontroller_device *)conf_obj->obj;

    switch (offset)
    {
        case _BR0:
            *(uint32_t *)buf = dev->regs.BR0;
            break;
        case _OR0:
            *(uint32_t *)buf = dev->regs.OR0;
            break;
        case _BR1:
            *(uint32_t *)buf = dev->regs.BR1;
            break;
        case _OR1:
            *(uint32_t *)buf = dev->regs.OR1;
            break;
        case _BR2:
            *(uint32_t *)buf = dev->regs.BR2;
            break;
        case _OR2:
            *(uint32_t *)buf = dev->regs.OR2;
            break;
        case _BR3:
            *(uint32_t *)buf = dev->regs.BR3;
            break;
        case _OR3:
            *(uint32_t *)buf = dev->regs.OR3;
            break;
        case _BR4:
            *(uint32_t *)buf = dev->regs.BR4;
            break;
        case _OR4:
            *(uint32_t *)buf = dev->regs.OR4;
            break;
        case _BR5:
            *(uint32_t *)buf = dev->regs.BR5;
            break;
        case _OR5:
            *(uint32_t *)buf = dev->regs.OR5;
            break;
        case _BR6:
            *(uint32_t *)buf = dev->regs.BR6;
            break;
        case _OR6:
            *(uint32_t *)buf = dev->regs.OR6;
            break;
        case _BR7:
            *(uint32_t *)buf = dev->regs.BR7;
            break;
        case _OR7:
            *(uint32_t *)buf = dev->regs.OR7;
            break;
        case _MAR:
            *(uint32_t *)buf = dev->regs.MAR;
            break;
        case _MAMR:
            *(uint32_t *)buf = dev->regs.MAMR;
            break;
        case _MBMR:
            *(uint32_t *)buf = dev->regs.MBMR;
            break;
        case _MCMR:
            *(uint32_t *)buf = dev->regs.MCMR;
            break;
        case _MPTPR:
            *(uint16_t *)buf = dev->regs.MPTPR;
            break;
        case _MDR:
            *(uint32_t *)buf = dev->regs.MDR;
            break;
        case _PSDMR:
            *(uint32_t *)buf = dev->regs.PSDMR;
            break;
        case _PURT:
            *(uint8_t *)buf = dev->regs.PURT;
            break;
        case _PSRT:
            *(uint8_t *)buf = dev->regs.PSRT;
            break;
        case _IMMR:
            *(uint32_t *)buf = dev->regs.IMMR;
            break;
        case _PCIBR0:
            *(uint32_t *)buf = dev->regs.PCIBR0;
            break;
        case _PCIBR1:
            *(uint32_t *)buf = dev->regs.PCIBR1;
            break;
        case _SECBR:
            *(uint32_t *)buf = dev->regs.SECBR;
            break;
        case _SECMR:
            *(uint32_t *)buf = dev->regs.SECMR;
            break;
        case _PCIMSK0:
            *(uint32_t *)buf = dev->regs.PCIMSK0;
            break;
        case _PCIMSK1:
            *(uint32_t *)buf = dev->regs.PCIMSK1;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in mpc8247_memorycontroller\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器写入操作方法
static exception_t mpc8247_memorycontroller_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc8247_memorycontroller_device *dev = (mpc8247_memorycontroller_device *)conf_obj->obj;
    uint32_t val = *(uint32_t *)buf;
    uint16_t val16 = *(uint16_t *)buf;
    uint8_t val8 = *(uint8_t *)buf;

    switch (offset)
    {
        case _BR0:
            dev->regs.BR0 = val;
            break;
        case _OR0:
            dev->regs.OR0 = val;
            break;
        case _BR1:
            dev->regs.BR1 = val;
            break;
        case _OR1:
            dev->regs.OR1 = val;
            break;
        case _BR2:
            dev->regs.BR2 = val;
            break;
        case _OR2:
            dev->regs.OR2 = val;
            break;
        case _BR3:
            dev->regs.BR3 = val;
            break;
        case _OR3:
            dev->regs.OR3 = val;
            break;
        case _BR4:
            dev->regs.BR4 = val;
            break;
        case _OR4:
            dev->regs.OR4 = val;
            break;
        case _BR5:
            dev->regs.BR5 = val;
            break;
        case _OR5:
            dev->regs.OR5 = val;
            break;
        case _BR6:
            dev->regs.BR6 = val;
            break;
        case _OR6:
            dev->regs.OR6 = val;
            break;
        case _BR7:
            dev->regs.BR7 = val;
            break;
        case _OR7:
            dev->regs.OR7 = val;
            break;
        case _MAR:
            dev->regs.MAR = val;
            break;
        case _MAMR:
            dev->regs.MAMR = val;
            break;
        case _MBMR:
            dev->regs.MBMR = val;
            break;
        case _MCMR:
            dev->regs.MCMR = val;
            break;
        case _MPTPR:
            dev->regs.MPTPR = val16;
            break;
        case _MDR:
            dev->regs.MDR = val;
            break;
        case _PSDMR:
            dev->regs.PSDMR = val;
            break;
        case _PURT:
            dev->regs.PURT = val8;
            break;
        case _PSRT:
            dev->regs.PSRT = val8;
            break;
        case _IMMR:
            dev->regs.IMMR = val;
            break;
        case _PCIBR0:
            dev->regs.PCIBR0 = val;
            break;
        case _PCIBR1:
            dev->regs.PCIBR1 = val;
            break;
        case _SECBR:
            dev->regs.SECBR = val;
            break;
        case _SECMR:
            dev->regs.SECMR = val;
            break;
        case _PCIMSK0:
            dev->regs.PCIMSK0 = val;
            break;
        case _PCIMSK1:
            dev->regs.PCIMSK1 = val;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in mpc8247_memorycontroller\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器接口:根据寄存器ID获取寄存器名
static char* mpc8247_memorycontroller_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_name[id];
}

// 寄存器接口:根据寄存器ID设置寄存器值
static exception_t mpc8247_memorycontroller_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    mpc8247_memorycontroller_device *dev = (mpc8247_memorycontroller_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    *((uint32_t*)regs + id) = value;

    return No_exp;
}

// 寄存器接口:获取设备有多少寄存器
static uint32_t mpc8247_memorycontroller_get_regnum(conf_object_t *conf_obj)
{
    return 32;
}

// 寄存器接口:根据寄存器ID获取寄存器值
static uint32_t mpc8247_memorycontroller_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    mpc8247_memorycontroller_device *dev = (mpc8247_memorycontroller_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    uint32_t* regs_value = (uint32_t*)regs + id;

    return *regs_value;
}

// 寄存器接口:根据寄存器名获取寄存器ID
static uint32_t mpc8247_memorycontroller_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
    uint32_t regnum = 32;
    int i;

    for (i = 0; i < regnum; i++)
    {
        if (strcmp(name, regs_name[i]) == 0)
            return i;
    }

    return regnum;
}

// 寄存器接口:根据寄存器ID获取寄存器偏移
static uint32_t mpc8247_memorycontroller_get_regoffset_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_offset[id];
}

// 实例化一个新设备对象，obj_name是在json文件中配置的名称
static conf_object_t* new_mpc8247_memorycontroller(const char *obj_name)
{
    mpc8247_memorycontroller_device* dev = skyeye_mm_zero(sizeof(mpc8247_memorycontroller_device));
    dev->obj = new_conf_object(obj_name, dev);
    // TODO: 在此添加自定义源码

    // 寄存器初始化
    struct registers *regs = &(dev->regs);
    regs->BR0 = 0;
    regs->OR0 = 0xfe000ef4;
    regs->BR1 = 0;
    regs->OR1 = 0;
    regs->BR2 = 0;
    regs->OR2 = 0;
    regs->BR3 = 0;
    regs->OR3 = 0;
    regs->BR4 = 0;
    regs->OR4 = 0;
    regs->BR5 = 0;
    regs->OR5 = 0;
    regs->BR6 = 0;
    regs->OR6 = 0;
    regs->BR7 = 0;
    regs->OR7 = 0;
    regs->MAR = 0;
    regs->MAMR = 0x40000;
    regs->MBMR = 0x40000;
    regs->MCMR = 0x40000;
    regs->MPTPR = 0;
    regs->MDR = 0;
    regs->PSDMR = 0;
    regs->PURT = 0;
    regs->PSRT = 0;
    regs->IMMR = 0;
    regs->PCIBR0 = 0;
    regs->PCIBR1 = 0;
    regs->SECBR = 0;
    regs->SECMR = 0;
    regs->PCIMSK0 = 0;
    regs->PCIMSK1 = 0;
    // 属性初始化
    dev->intNumber = 0;

    return dev->obj;
}

// 配置实例化的设备对象
static exception_t config_mpc8247_memorycontroller(conf_object_t* conf_obj)
{
    mpc8247_memorycontroller_device *dev = (mpc8247_memorycontroller_device *)conf_obj->obj;
    // TODO: 在此添加自定义源码

    return No_exp;
}

// 释放设备对象来做一些内存释放
static exception_t free_mpc8247_memorycontroller(conf_object_t* conf_obj)
{
    mpc8247_memorycontroller_device *dev = (mpc8247_memorycontroller_device *)conf_obj->obj;

    skyeye_free(conf_obj->objname);
    skyeye_free(conf_obj->obj);
    skyeye_free(conf_obj);

    return No_exp;
}

// 系统设置设备 intNumber 属性
static exception_t set_attr_intNumber(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    mpc8247_memorycontroller_device *dev = (mpc8247_memorycontroller_device *)conf_obj->obj;

    dev->intNumber = SKY_attr_uinteger(*value);

    return No_exp;
}

// 系统获取设备 intNumber 属性
static attr_value_t get_attr_intNumber(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    mpc8247_memorycontroller_device *dev = (mpc8247_memorycontroller_device *)conf_obj->obj;

    attr_value_t value = SKY_make_attr_uinteger(dev->intNumber);

    return value;
}

// 注册设备模块属性
static void class_register_attribute(conf_class_t* dev_class)
{
    SKY_register_attribute(dev_class,
            "intNumber",
            get_attr_intNumber, NULL,
            set_attr_intNumber, NULL,
            SKY_Attr_Optional,
            "uinteger",
            "this is intNumber describe");

    return;
}

// 初始化模块类，并在SkyEye加载模块时执行以下代码
void init_mpc8247_memorycontroller(void)
{
    // 向SkyEye注册一个设备
    static skyeye_class_t class_data =
    {
        .class_name      = "mpc8247_memorycontroller",
        .class_desc      = "mpc8247_MemoryController",
        .new_instance    = new_mpc8247_memorycontroller,
        .free_instance   = free_mpc8247_memorycontroller,
        .config_instance = config_mpc8247_memorycontroller,
        .module_type     = SKYML_OFFCHIP,
    };
    conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);

    // 设置寄存器地址读写方法
    static memory_space_intf io_memory_interface =
    {
        .read  = mpc8247_memorycontroller_read,
        .write = mpc8247_memorycontroller_write
    };
    SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

    // 注册访问接口(用于SkyEye图形控制接口)
    static skyeye_reg_intf reg_interface =
    {
        .get_regvalue_by_id  = mpc8247_memorycontroller_get_regvalue_by_id,
        .get_regname_by_id   = mpc8247_memorycontroller_get_regname_by_id,
        .set_regvalue_by_id  = mpc8247_memorycontroller_set_regvalue_by_id,
        .get_regnum          = mpc8247_memorycontroller_get_regnum,
        .get_regid_by_name   = mpc8247_memorycontroller_get_regid_by_name,
        .get_regoffset_by_id = mpc8247_memorycontroller_get_regoffset_by_id
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
    class_register_attribute(dev_class);
}
