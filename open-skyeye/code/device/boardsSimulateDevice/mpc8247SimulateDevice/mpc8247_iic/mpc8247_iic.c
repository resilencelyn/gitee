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
#include "mpc8247_iic.h"

// initialize register
static void initRegister(conf_object_t *conf_obj)
{
    mpc8247_iic_device *dev = (mpc8247_iic_device *)conf_obj->obj;

    // 寄存器初始化
    struct registers *regs = &(dev->regs);
    regs->I2MOD = 0;
    regs->I2ADD = 0;
    regs->I2BRG = 0;
    regs->I2COM = 0;
    regs->I2CER = 0;
    regs->I2CMR = 0;
}


// 寄存器读取操作方法
static exception_t mpc8247_iic_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc8247_iic_device *dev = (mpc8247_iic_device *)conf_obj->obj;

    switch (offset)
    {
        case _I2MOD:
            *(uint8_t *)buf = dev->regs.I2MOD;
            break;
        case _I2ADD:
            *(uint8_t *)buf = dev->regs.I2ADD;
            break;
        case _I2BRG:
            *(uint8_t *)buf = dev->regs.I2BRG;
            break;
        case _I2COM:
            *(uint8_t *)buf = dev->regs.I2COM;
            break;
        case _I2CER:
            *(uint8_t *)buf = dev->regs.I2CER;
            break;
        case _I2CMR:
            *(uint8_t *)buf = dev->regs.I2CMR;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in mpc8247_iic\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器写入操作方法
static exception_t mpc8247_iic_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc8247_iic_device *dev = (mpc8247_iic_device *)conf_obj->obj;
    uint8_t val8 = *(uint8_t *)buf;

    switch (offset)
    {
        case _I2MOD:
            dev->regs.I2MOD = val8;
            break;
        case _I2ADD:
            dev->regs.I2ADD = val8;
            break;
        case _I2BRG:
            dev->regs.I2BRG = val8;
            break;
        case _I2COM:
            dev->regs.I2COM = val8;
            break;
        case _I2CER:
            dev->regs.I2CER = val8;
            break;
        case _I2CMR:
            dev->regs.I2CMR = val8;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in mpc8247_iic\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器接口:根据寄存器ID获取寄存器名
static char* mpc8247_iic_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_name[id];
}

// 寄存器接口:根据寄存器ID设置寄存器值
static exception_t mpc8247_iic_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    mpc8247_iic_device *dev = (mpc8247_iic_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    *((uint32_t*)regs + id) = value;

    return No_exp;
}

// 寄存器接口:获取设备有多少寄存器
static uint32_t mpc8247_iic_get_regnum(conf_object_t *conf_obj)
{
    return 6;
}

// 寄存器接口:根据寄存器ID获取寄存器值
static uint32_t mpc8247_iic_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    mpc8247_iic_device *dev = (mpc8247_iic_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    uint32_t* regs_value = (uint32_t*)regs + id;

    return *regs_value;
}

// 寄存器接口:根据寄存器名获取寄存器ID
static uint32_t mpc8247_iic_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
    uint32_t regnum = 6;
    int i;

    for (i = 0; i < regnum; i++)
    {
        if (strcmp(name, regs_name[i]) == 0)
            return i;
    }

    return regnum;
}

// 寄存器接口:根据寄存器ID获取寄存器偏移
static uint32_t mpc8247_iic_get_regoffset_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_offset[id];
}

// 实例化一个新设备对象，obj_name是在json文件中配置的名称
static conf_object_t* new_mpc8247_iic(const char *obj_name)
{
    mpc8247_iic_device* dev = skyeye_mm_zero(sizeof(mpc8247_iic_device));
    dev->obj = new_conf_object(obj_name, dev);
    // TODO: 在此添加自定义源码

    initRegister(dev->obj);

    // 属性初始化
    dev->int_number = 0;

    return dev->obj;
}

// 配置实例化的设备对象
static exception_t config_mpc8247_iic(conf_object_t* conf_obj)
{
    mpc8247_iic_device *dev = (mpc8247_iic_device *)conf_obj->obj;
    // TODO: 在此添加自定义源码

    return No_exp;
}

// 释放设备对象来做一些内存释放
static exception_t free_mpc8247_iic(conf_object_t* conf_obj)
{
    mpc8247_iic_device *dev = (mpc8247_iic_device *)conf_obj->obj;

    skyeye_free(conf_obj->objname);
    skyeye_free(conf_obj->obj);
    skyeye_free(conf_obj);

    return No_exp;
}

// 系统设置设备 general_signal 接口功能
static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc8247_iic_device *dev = (mpc8247_iic_device *)obj->obj;

    dev->general_signal.obj = obj2;
    if (obj2 == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from NULL object fail.\n", GENERAL_SIGNAL_INTF_NAME);
        return Not_found_exp;
    }
    dev->general_signal.iface = (general_signal_iface_t *)SKY_get_iface(obj2, GENERAL_SIGNAL_INTF_NAME);
    if (dev->general_signal.iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from %s fail.\n", GENERAL_SIGNAL_INTF_NAME, obj2->objname);
        return Not_found_exp;
    }

    return No_exp;
}

// 系统获取设备 general_signal 接口功能
static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    mpc8247_iic_device *dev = (mpc8247_iic_device *)obj->obj;

    *obj2 = dev->general_signal.obj;
    *port = NULL;

    return No_exp;
}

// 系统设置设备 i2c_bus 接口功能
static exception_t i2c_bus_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc8247_iic_device *dev = (mpc8247_iic_device *)obj->obj;

    dev->i2c_bus.obj = obj2;
    if (obj2 == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from NULL object fail.\n", I2C_BUS_INTF_NAME);
        return Not_found_exp;
    }
    dev->i2c_bus.iface = (i2c_bus_intf *)SKY_get_iface(obj2, I2C_BUS_INTF_NAME);
    if (dev->i2c_bus.iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from %s fail.\n", I2C_BUS_INTF_NAME, obj2->objname);
        return Not_found_exp;
    }

    return No_exp;
}

// 系统获取设备 i2c_bus 接口功能
static exception_t i2c_bus_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    mpc8247_iic_device *dev = (mpc8247_iic_device *)obj->obj;

    *obj2 = dev->i2c_bus.obj;
    *port = NULL;

    return No_exp;
}

// 系统设置设备 int_number 属性
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    mpc8247_iic_device *dev = (mpc8247_iic_device *)conf_obj->obj;

    dev->int_number = SKY_attr_uinteger(*value);

    return No_exp;
}

// 系统获取设备 int_number 属性
static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    mpc8247_iic_device *dev = (mpc8247_iic_device *)conf_obj->obj;

    attr_value_t value = SKY_make_attr_uinteger(dev->int_number);

    return value;
}

// 注册设备模块属性
static void class_register_attribute(conf_class_t* dev_class)
{
    SKY_register_attribute(dev_class,
            "int_number",
            get_attr_int_number, NULL,
            set_attr_int_number, NULL,
            SKY_Attr_Optional,
            "uinteger",
            "this is int_number describe");

    return;
}

// 初始化模块类，并在SkyEye加载模块时执行以下代码
void init_mpc8247_iic(void)
{
    // 向SkyEye注册一个设备
    static skyeye_class_t class_data =
    {
        .class_name      = "mpc8247_iic",
        .class_desc      = "mpc8247_iic",
        .new_instance    = new_mpc8247_iic,
        .free_instance   = free_mpc8247_iic,
        .config_instance = config_mpc8247_iic,
        .module_type     = SKYML_OFFCHIP,
    };
    conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);

    // 设置寄存器地址读写方法
    static memory_space_intf io_memory_interface =
    {
        .read  = mpc8247_iic_read,
        .write = mpc8247_iic_write
    };
    SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

    // 注册访问接口(用于SkyEye图形控制接口)
    static skyeye_reg_intf reg_interface =
    {
        .get_regvalue_by_id  = mpc8247_iic_get_regvalue_by_id,
        .get_regname_by_id   = mpc8247_iic_get_regname_by_id,
        .set_regvalue_by_id  = mpc8247_iic_set_regvalue_by_id,
        .get_regnum          = mpc8247_iic_get_regnum,
        .get_regid_by_name   = mpc8247_iic_get_regid_by_name,
        .get_regoffset_by_id = mpc8247_iic_get_regoffset_by_id
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
        (struct ConnectDescription)
        {
            .name = GENERAL_SIGNAL_INTF_NAME,
            .set  = general_signal_set,
            .get  = general_signal_get
        },
        (struct ConnectDescription)
        {
            .name = I2C_BUS_INTF_NAME,
            .set  = i2c_bus_set,
            .get  = i2c_bus_get
        },
    };
    class_register_ifaces(dev_class, ifaces);
    class_register_connects(dev_class, connects);
    class_register_attribute(dev_class);
}
