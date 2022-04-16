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
#include "mc146818_rtc.h"
#include <time.h>

static inline unsigned int bcd2bin(unsigned int val)
{
    return (val & 0x0f) + ((val & 0xff) >> 4) * 10;
}

static inline unsigned int bin2bcd(unsigned int val)
{
    return (((val / 10) << 4) | (val % 10));
}

void time_count_handler(conf_object_t *conf_obj)
{
    // TODO: Not implemented yet!
    mc146818_rtc_device *dev = (mc146818_rtc_device *)conf_obj->obj;
    dev->regs.RTC_SECONDS++;
    if (dev->regs.RTC_SECONDS >= 60)
    {
        dev->regs.RTC_MINUTES++;
        dev->regs.RTC_SECONDS = 0;
    }
}

// 寄存器读取操作方法
static exception_t mc146818_rtc_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mc146818_rtc_device *dev = (mc146818_rtc_device *)conf_obj->obj;

    // TODO: Not implemented yet!
    switch (offset)
    {
        case _RTC_SECONDS:
            *(uint32_t *)buf = dev->regs.RTC_SECONDS;
            break;
        case _RTC_SECONDS_ALARM:
            *(uint32_t *)buf = dev->regs.RTC_SECONDS_ALARM;
            break;
        case _RTC_MINUTES:
            *(uint32_t *)buf = dev->regs.RTC_MINUTES;
            break;
        case _RTC_MINUTES_ALARM:
            *(uint32_t *)buf = dev->regs.RTC_MINUTES_ALARM;
            break;
        case _RTC_HOURS:
            *(uint32_t *)buf = dev->regs.RTC_HOURS;
            break;
        case _RTC_HOURS_ALARM:
            *(uint32_t *)buf = dev->regs.RTC_HOURS_ALARM;
            break;
        case _RTC_DAY_OF_WEEK:
            *(uint32_t *)buf = dev->regs.RTC_DAY_OF_WEEK;
            break;
        case _RTC_DATE_OF_MONTH:
            *(uint32_t *)buf = dev->regs.RTC_DATE_OF_MONTH;
            break;
        case _RTC_MONTH:
            *(uint32_t *)buf = dev->regs.RTC_MONTH;
            break;
        case _RTC_YEAR:
            *(uint32_t *)buf = dev->regs.RTC_YEAR;
            break;
        case _RTC_CONFIG_A:
            *(uint32_t *)buf = dev->regs.RTC_CONFIG_A;
            break;
        case _RTC_CONFIG_B:
            *(uint32_t *)buf = dev->regs.RTC_CONFIG_B;
            break;
        case _RTC_CONFIG_C:
            *(uint32_t *)buf = dev->regs.RTC_CONFIG_C;
            break;
        case _RTC_CONFIG_D:
            *(uint32_t *)buf = dev->regs.RTC_CONFIG_D;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in mc146818_rtc\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器写入操作方法
static exception_t mc146818_rtc_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mc146818_rtc_device *dev = (mc146818_rtc_device *)conf_obj->obj;
    uint32_t val = *(uint32_t *)buf;

    // TODO: Not implemented yet!
    switch (offset)
    {
        case _RTC_SECONDS:
            dev->regs.RTC_SECONDS = val;
            break;
        case _RTC_SECONDS_ALARM:
            dev->regs.RTC_SECONDS_ALARM = val;
            break;
        case _RTC_MINUTES:
            dev->regs.RTC_MINUTES = val;
            break;
        case _RTC_MINUTES_ALARM:
            dev->regs.RTC_MINUTES_ALARM = val;
            break;
        case _RTC_HOURS:
            dev->regs.RTC_HOURS = val;
            break;
        case _RTC_HOURS_ALARM:
            dev->regs.RTC_HOURS_ALARM = val;
            break;
        case _RTC_DAY_OF_WEEK:
            dev->regs.RTC_DAY_OF_WEEK = val;
            break;
        case _RTC_DATE_OF_MONTH:
            dev->regs.RTC_DATE_OF_MONTH = val;
            break;
        case _RTC_MONTH:
            dev->regs.RTC_MONTH = val;
            break;
        case _RTC_YEAR:
            dev->regs.RTC_YEAR = val;
            break;
        case _RTC_CONFIG_A:
            dev->regs.RTC_CONFIG_A = val;
            break;
        case _RTC_CONFIG_B:
            dev->regs.RTC_CONFIG_B = val;
            break;
        case _RTC_CONFIG_C:
            dev->regs.RTC_CONFIG_C = val;
            break;
        case _RTC_CONFIG_D:
            dev->regs.RTC_CONFIG_D = val;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in mc146818_rtc\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器接口:根据寄存器ID获取寄存器名
static char *mc146818_rtc_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_name[id];
}

// 寄存器接口:根据寄存器ID设置寄存器值
static exception_t mc146818_rtc_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    mc146818_rtc_device *dev = (mc146818_rtc_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    *((uint32_t *)regs + id) = value;

    return No_exp;
}

// 寄存器接口:获取设备有多少寄存器
static uint32_t mc146818_rtc_get_regnum(conf_object_t *conf_obj)
{
    return sizeof(struct registers) / 4;
}

// 寄存器接口:根据寄存器ID获取寄存器值
static uint32_t mc146818_rtc_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    mc146818_rtc_device *dev = (mc146818_rtc_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    uint32_t *regs_value = (uint32_t *)regs + id;

    return *regs_value;
}

// 寄存器接口:根据寄存器名获取寄存器ID
static uint32_t mc146818_rtc_get_regid_by_name(conf_object_t *conf_obj, char *name)
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
static uint32_t mc146818_rtc_get_regoffset_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_offset[id];
}

// 实例化一个新设备对象，obj_name是在json文件中配置的名称
static conf_object_t *new_mc146818_rtc(const char *obj_name)
{
    mc146818_rtc_device *dev = skyeye_mm_zero(sizeof(mc146818_rtc_device));
    dev->obj = new_conf_object(obj_name, dev);
    // TODO: 在此添加自定义源码

    // 寄存器初始化
    struct registers *regs = &(dev->regs);
    regs->RTC_SECONDS = 0;
    regs->RTC_SECONDS_ALARM = 0;
    regs->RTC_MINUTES = 0;
    regs->RTC_MINUTES_ALARM = 0;
    regs->RTC_HOURS = 0;
    regs->RTC_HOURS_ALARM = 0;
    regs->RTC_DAY_OF_WEEK = 0;
    regs->RTC_DATE_OF_MONTH = 0;
    regs->RTC_MONTH = 0;
    regs->RTC_YEAR = 0;
    regs->RTC_CONFIG_A = 0x80;
    regs->RTC_CONFIG_B = 0;
    regs->RTC_CONFIG_C = 0;
    regs->RTC_CONFIG_D = 0;

    return dev->obj;
}

// 配置实例化的设备对象
static exception_t config_mc146818_rtc(conf_object_t *conf_obj)
{
    mc146818_rtc_device *dev = (mc146818_rtc_device *)conf_obj->obj;
    // TODO: 在此添加自定义源码
    system_register_timer_handler(conf_obj, 1, (time_sched_t)time_count_handler, SCHED_S | SCHED_NORMAL);

    return No_exp;
}

// 释放设备对象来做一些内存释放
static exception_t free_mc146818_rtc(conf_object_t *conf_obj)
{
    mc146818_rtc_device *dev = (mc146818_rtc_device *)conf_obj->obj;

    skyeye_free(conf_obj->objname);
    skyeye_free(conf_obj->obj);
    skyeye_free(conf_obj);

    return No_exp;
}

// 初始化模块类，并在SkyEye加载模块时执行以下代码
void init_mc146818_rtc(void)
{
    // 向SkyEye注册一个设备
    static skyeye_class_t class_data =
        {
            .class_name = "mc146818_rtc",
            .class_desc = "MC146818 (PIXX4) RTC for MPC750",
            .new_instance = new_mc146818_rtc,
            .free_instance = free_mc146818_rtc,
            .config_instance = config_mc146818_rtc,
            .module_type = SKYML_OFFCHIP,
        };
    conf_class_t *dev_class = SKY_register_device_class(class_data.class_name, &class_data);

    // 设置寄存器地址读写方法
    static memory_space_intf io_memory_interface =
        {
            .read = mc146818_rtc_read,
            .write = mc146818_rtc_write};
    SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

    // 注册访问接口(用于SkyEye图形控制接口)
    static skyeye_reg_intf reg_interface =
        {
            .get_regvalue_by_id = mc146818_rtc_get_regvalue_by_id,
            .get_regname_by_id = mc146818_rtc_get_regname_by_id,
            .set_regvalue_by_id = mc146818_rtc_set_regvalue_by_id,
            .get_regnum = mc146818_rtc_get_regnum,
            .get_regid_by_name = mc146818_rtc_get_regid_by_name,
            .get_regoffset_by_id = mc146818_rtc_get_regoffset_by_id};
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
            /* ConnectDescription */
        };
    class_register_ifaces(dev_class, ifaces);
    class_register_connects(dev_class, connects);
}
