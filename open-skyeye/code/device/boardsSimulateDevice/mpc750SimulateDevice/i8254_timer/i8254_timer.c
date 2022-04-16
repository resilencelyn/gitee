#include <memory_space.h>
#include <skyeye_api.h>
#include <skyeye_class.h>
#include <skyeye_core.h>
#include <skyeye_dev_bus_intf.h>
#include <skyeye_dev_intf.h>
#include <skyeye_device.h>
#include <skyeye_iface.h>
#include <skyeye_interface.h>
#include <skyeye_lock.h>
#include <skyeye_log.h>
#include <skyeye_mm.h>
#include <skyeye_obj.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_system.h>
#include <skyeye_types.h>
#include <skyeye_uart_ops.h>
#define DEBUG
#include "i8254_timer.h"

// 定时器回调函数
void make_interrupt(conf_object_t *conf_obj, time_data_t *root_data)
{
    i8254_timer_device *dev = (i8254_timer_device *)conf_obj->obj;
    // TODO: 在此添加自定义源码
    if (dev->general_signal.iface)
    {
        dev->general_signal.iface->raise_signal(dev->general_signal.obj, dev->int_number);
    }
    else
    {
        skyeye_log(Error_log, __FUNCTION__, "I8254 Timer can't access to the Interrupt Controller, did you forget to connect them?\n");
    }
}

// 寄存器读取操作方法
static exception_t i8254_timer_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    i8254_timer_device *dev = (i8254_timer_device *)conf_obj->obj;

    // TODO !!!
    switch (offset)
    {
        case _COUNTER_0:
            *(uint32_t *)buf = dev->regs.counter_0;
            break;
        case _COUNTER_1:
            *(uint32_t *)buf = dev->regs.counter_1;
            break;
        case _COUNTER_2:
            *(uint32_t *)buf = dev->regs.counter_2;
            break;
        case _COMMAND:
            *(uint8_t *)buf = dev->regs.command;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in i8254_timer\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器写入操作方法
static exception_t i8254_timer_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    i8254_timer_device *dev = (i8254_timer_device *)conf_obj->obj;
    uint8_t val = *(uint8_t *)buf;

    // TODO !!!
    switch (offset)
    {
        case _COUNTER_0:
            dev->regs.counter_0 = val;
            break;
        case _COUNTER_1:
            dev->regs.counter_1 = val;
            break;
        case _COUNTER_2:
            dev->regs.counter_2 = val;
            break;
        case _COMMAND:
            // // Control Word
            // dev->counter = val >> 6 & 0x3;
            // if (dev->counter != 3)
            // {
            //     dev->method = val >> 4 & 0x3;
            //     dev->mode = val >> 1 & 0x7;
            //     dev->format = val & 0x1;
            // }
            // else //Read-Back Command
            // {
            //     dev->count = val >> 5 & 0x1;
            //     dev->status = val >> 4 & 0x1;
            //     dev->readback_counter = val >> 1 & 0x7;
            // }
            dev->regs.command = val;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in i8254_timer\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器接口:根据寄存器ID获取寄存器名
static char *i8254_timer_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_name[id];
}

// 寄存器接口:根据寄存器ID设置寄存器值
static exception_t i8254_timer_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    i8254_timer_device *dev = (i8254_timer_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    *((uint32_t *)regs + id) = value;

    return No_exp;
}

// 寄存器接口:获取设备有多少寄存器
static uint32_t i8254_timer_get_regnum(conf_object_t *conf_obj)
{
    return sizeof(struct registers) / 4;
}

// 寄存器接口:根据寄存器ID获取寄存器值
static uint32_t i8254_timer_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    i8254_timer_device *dev = (i8254_timer_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    uint32_t *regs_value = (uint32_t *)regs + id;

    return *regs_value;
}

// 寄存器接口:根据寄存器名获取寄存器ID
static uint32_t i8254_timer_get_regid_by_name(conf_object_t *conf_obj, char *name)
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
static uint32_t i8254_timer_get_regoffset_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_offset[id];
}

// 实例化一个新设备对象，obj_name是在json文件中配置的名称
static conf_object_t *new_i8254_timer(const char *obj_name)
{
    i8254_timer_device *dev = skyeye_mm_zero(sizeof(i8254_timer_device));
    dev->obj = new_conf_object(obj_name, dev);
    // TODO: 在此添加自定义源码

    // 寄存器初始化
    struct registers *regs = &(dev->regs);
    regs->counter_0 = 0;
    regs->counter_1 = 0;
    regs->counter_2 = 0;
    regs->command = 0;
    // 属性初始化
    dev->int_number = 0;

    return dev->obj;
}

// 配置实例化的设备对象
static exception_t config_i8254_timer(conf_object_t *conf_obj)
{
    i8254_timer_device *dev = (i8254_timer_device *)conf_obj->obj;
    // TODO: 在此添加自定义源码

    // TODO: Just for SylixOS instance!!!
    dev->timer_handle[0] = system_register_timer_handler(dev->obj, 1, (time_sched_t)make_interrupt, SCHED_MS | SCHED_NORMAL);
    system_enable_timer(dev->timer_handle[0]);

    return No_exp;
}

// 释放设备对象来做一些内存释放
static exception_t free_i8254_timer(conf_object_t *conf_obj)
{
    i8254_timer_device *dev = (i8254_timer_device *)conf_obj->obj;

    int i = 0;
    for (i = 0; i < 1; i++)
    {
        system_del_timer_handler(dev->timer_handle[i]);
    }

    skyeye_free(conf_obj->objname);
    skyeye_free(conf_obj->obj);
    skyeye_free(conf_obj);

    return No_exp;
}

// 系统设置设备 general_signal 接口功能
static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    i8254_timer_device *dev = (i8254_timer_device *)obj->obj;

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
    i8254_timer_device *dev = (i8254_timer_device *)obj->obj;

    *obj2 = dev->general_signal.obj;
    *port = NULL;

    return No_exp;
}

// 系统设置设备 int_number 属性
static exception_t set_attr_int_number(void *arg, conf_object_t *conf_obj, attr_value_t *value, attr_value_t *idx)
{
    i8254_timer_device *dev = (i8254_timer_device *)conf_obj->obj;

    dev->int_number = SKY_attr_uinteger(*value);

    return No_exp;
}

// 系统获取设备 int_number 属性
static attr_value_t get_attr_int_number(void *arg, conf_object_t *conf_obj, attr_value_t *idx)
{
    i8254_timer_device *dev = (i8254_timer_device *)conf_obj->obj;

    attr_value_t value = SKY_make_attr_uinteger(dev->int_number);

    return value;
}

// 注册设备模块属性
static void class_register_attribute(conf_class_t *dev_class)
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
void init_i8254_timer(void)
{
    // 向SkyEye注册一个设备
    static skyeye_class_t class_data =
        {
            .class_name = "i8254_timer",
            .class_desc = "Intel 8254 Programmable Interval Timer",
            .new_instance = new_i8254_timer,
            .free_instance = free_i8254_timer,
            .config_instance = config_i8254_timer,
            .module_type = SKYML_OFFCHIP,
        };
    conf_class_t *dev_class = SKY_register_device_class(class_data.class_name, &class_data);

    // 设置寄存器地址读写方法
    static memory_space_intf io_memory_interface =
        {
            .read = i8254_timer_read,
            .write = i8254_timer_write};
    SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

    // 注册访问接口(用于SkyEye图形控制接口)
    static skyeye_reg_intf reg_interface =
        {
            .get_regvalue_by_id = i8254_timer_get_regvalue_by_id,
            .get_regname_by_id = i8254_timer_get_regname_by_id,
            .set_regvalue_by_id = i8254_timer_set_regvalue_by_id,
            .get_regnum = i8254_timer_get_regnum,
            .get_regid_by_name = i8254_timer_get_regid_by_name,
            .get_regoffset_by_id = i8254_timer_get_regoffset_by_id};
    SKY_register_iface(dev_class, SKYEYE_REG_INTF, &reg_interface);

    // 接口方法列表
    static const struct InterfaceDescription ifaces[] =
        {
            (struct InterfaceDescription){
                .name = MEMORY_SPACE_INTF_NAME,
                .iface = &io_memory_interface},
            (struct InterfaceDescription){
                .name = SKYEYE_REG_INTF,
                .iface = &reg_interface},
        };

    // 接口连接方法列表
    static const struct ConnectDescription connects[] =
        {
            (struct ConnectDescription){
                .name = GENERAL_SIGNAL_INTF_NAME,
                .set = general_signal_set,
                .get = general_signal_get},
        };
    class_register_ifaces(dev_class, ifaces);
    class_register_connects(dev_class, connects);
    class_register_attribute(dev_class);
}
