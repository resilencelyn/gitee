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
#include "mpc5554_ebi.h"

// 寄存器读取操作方法
static exception_t mpc5554_ebi_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc5554_ebi_device *dev = (mpc5554_ebi_device *)conf_obj->obj;

    switch (offset)
    {
        case _MCR:
            *(uint32_t *)buf = dev->regs.MCR;
            break;
        case _TESR:
            *(uint32_t *)buf = dev->regs.TESR;
            break;
        case _BMCR:
            *(uint32_t *)buf = dev->regs.BMCR;
            break;
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
        case _CAL_BR0:
            *(uint32_t *)buf = dev->regs.CAL_BR0;
            break;
        case _CAL_OR0:
            *(uint32_t *)buf = dev->regs.CAL_OR0;
            break;
        case _CAL_BR1:
            *(uint32_t *)buf = dev->regs.CAL_BR1;
            break;
        case _CAL_OR1:
            *(uint32_t *)buf = dev->regs.CAL_OR1;
            break;
        case _CAL_BR2:
            *(uint32_t *)buf = dev->regs.CAL_BR2;
            break;
        case _CAL_OR2:
            *(uint32_t *)buf = dev->regs.CAL_OR2;
            break;
        case _CAL_BR3:
            *(uint32_t *)buf = dev->regs.CAL_BR3;
            break;
        case _CAL_OR3:
            *(uint32_t *)buf = dev->regs.CAL_OR3;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in mpc5554_ebi\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器写入操作方法
static exception_t mpc5554_ebi_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc5554_ebi_device *dev = (mpc5554_ebi_device *)conf_obj->obj;
    uint32_t val = *(uint32_t *)buf;

    switch (offset)
    {
        case _MCR:
            dev->regs.MCR = val;
            break;
        case _TESR:
            dev->regs.TESR = val;
            break;
        case _BMCR:
            dev->regs.BMCR = val;
            break;
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
        case _CAL_BR0:
            dev->regs.CAL_BR0 = val;
            break;
        case _CAL_OR0:
            dev->regs.CAL_OR0 = val;
            break;
        case _CAL_BR1:
            dev->regs.CAL_BR1 = val;
            break;
        case _CAL_OR1:
            dev->regs.CAL_OR1 = val;
            break;
        case _CAL_BR2:
            dev->regs.CAL_BR2 = val;
            break;
        case _CAL_OR2:
            dev->regs.CAL_OR2 = val;
            break;
        case _CAL_BR3:
            dev->regs.CAL_BR3 = val;
            break;
        case _CAL_OR3:
            dev->regs.CAL_OR3 = val;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in mpc5554_ebi\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器接口:根据寄存器ID获取寄存器名
static char* mpc5554_ebi_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_name[id];
}

// 寄存器接口:根据寄存器ID设置寄存器值
static exception_t mpc5554_ebi_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    mpc5554_ebi_device *dev = (mpc5554_ebi_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    *((uint32_t*)regs + id) = value;

    return No_exp;
}

// 寄存器接口:获取设备有多少寄存器
static uint32_t mpc5554_ebi_get_regnum(conf_object_t *conf_obj)
{
    return sizeof(struct registers) / 4;
}

// 寄存器接口:根据寄存器ID获取寄存器值
static uint32_t mpc5554_ebi_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    mpc5554_ebi_device *dev = (mpc5554_ebi_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    uint32_t* regs_value = (uint32_t*)regs + id;

    return *regs_value;
}

// 寄存器接口:根据寄存器名获取寄存器ID
static uint32_t mpc5554_ebi_get_regid_by_name(conf_object_t *conf_obj, char *name)
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
static uint32_t mpc5554_ebi_get_regoffset_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_offset[id];
}

// 实例化一个新设备对象，obj_name是在json文件中配置的名称
static conf_object_t* new_mpc5554_ebi(const char *obj_name)
{
    mpc5554_ebi_device* dev = skyeye_mm_zero(sizeof(mpc5554_ebi_device));
    dev->obj = new_conf_object(obj_name, dev);
    // TODO: 在此添加自定义源码

    // 寄存器初始化
    struct registers *regs = &(dev->regs);
    regs->MCR = 0;
    regs->TESR = 0;
    regs->BMCR = 0;
    regs->BR0 = 0;
    regs->OR0 = 0;
    regs->BR1 = 0;
    regs->OR1 = 0;
    regs->BR2 = 0;
    regs->OR2 = 0;
    regs->BR3 = 0;
    regs->OR3 = 0;
    regs->CAL_BR0 = 0;
    regs->CAL_OR0 = 0;
    regs->CAL_BR1 = 0;
    regs->CAL_OR1 = 0;
    regs->CAL_BR2 = 0;
    regs->CAL_OR2 = 0;
    regs->CAL_BR3 = 0;
    regs->CAL_OR3 = 0;

    return dev->obj;
}

// 配置实例化的设备对象
static exception_t config_mpc5554_ebi(conf_object_t* conf_obj)
{
    mpc5554_ebi_device *dev = (mpc5554_ebi_device *)conf_obj->obj;
    // TODO: 在此添加自定义源码


    return No_exp;
}

// 释放设备对象来做一些内存释放
static exception_t free_mpc5554_ebi(conf_object_t* conf_obj)
{
    mpc5554_ebi_device *dev = (mpc5554_ebi_device *)conf_obj->obj;

    skyeye_free(conf_obj->objname);
    skyeye_free(conf_obj->obj);
    skyeye_free(conf_obj);

    return No_exp;
}

// 初始化模块类，并在SkyEye加载模块时执行以下代码
void init_mpc5554_ebi(void)
{
    // 向SkyEye注册一个设备
    static skyeye_class_t class_data =
    {
        .class_name      = "mpc5554_ebi",
        .class_desc      = "mpc5554_ebi",
        .new_instance    = new_mpc5554_ebi,
        .free_instance   = free_mpc5554_ebi,
        .config_instance = config_mpc5554_ebi,
        .module_type     = SKYML_OFFCHIP,
    };
    conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);

    // 设置寄存器地址读写方法
    static memory_space_intf io_memory_interface =
    {
        .read  = mpc5554_ebi_read,
        .write = mpc5554_ebi_write
    };
    SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

    // 注册访问接口(用于SkyEye图形控制接口)
    static skyeye_reg_intf reg_interface =
    {
        .get_regvalue_by_id  = mpc5554_ebi_get_regvalue_by_id,
        .get_regname_by_id   = mpc5554_ebi_get_regname_by_id,
        .set_regvalue_by_id  = mpc5554_ebi_set_regvalue_by_id,
        .get_regnum          = mpc5554_ebi_get_regnum,
        .get_regid_by_name   = mpc5554_ebi_get_regid_by_name,
        .get_regoffset_by_id = mpc5554_ebi_get_regoffset_by_id
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
