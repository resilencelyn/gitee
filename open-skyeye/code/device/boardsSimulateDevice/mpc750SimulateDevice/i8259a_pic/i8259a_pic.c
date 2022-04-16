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
#include "i8259a_pic.h"
// uncommemt to block output
// #define printf(...)
// #define skyeye_log(...)

// eg. 5 to 0b10000
static uint8_t bin2bit(uint8_t data)
{
    switch (data)
    {
        case 0:
            return 0x1;
        case 1:
            return 0x2;
        case 2:
            return 0x4;
        case 3:
            return 0x8;
        case 4:
            return 0x10;
        case 5:
            return 0x20;
        case 6:
            return 0x40;
        case 7:
            return 0x80;
        default:
            break;
    }
    // return (int)pow(2, (data - 1));
}

// eg. 0b10000 to 5
static uint8_t bit2bin(uint8_t data)
{
    switch (data)
    {
        case 0x1:
            return 0;
        case 0x2:
            return 1;
        case 0x4:
            return 2;
        case 0x8:
            return 3;
        case 0x10:
            return 4;
        case 0x20:
            return 5;
        case 0x40:
            return 6;
        case 0x80:
            return 7;
        default:
            break;
    }
}

static int i8259a_pic_ask_for_interrupt(i8259a_pic_device *dev)
{
    if (dev->core_signal.iface)
    {
        dev->core_signal.iface->signal(dev->core_signal.obj, NULL);
    }
    else
    {
        skyeye_log(Error_log, __FUNCTION__, "I8259A PIC can't access to the Core, did you forget to connect them?\n");
    }
}

static int i8259a_pic_sent_intnum(i8259a_pic_device *dev)
{
    skyeye_log(Error_log, __FUNCTION__, "Called. \n");

    int line = bit2bin(dev->irr);
    int bit = bin2bit(line);

    if (!(dev->imr & bit)) // 中断屏蔽寄存器 IMR (Interrup Mask Register)用于保存被屏蔽的中断请求线对应的比特位，哪个比特位被置1就屏蔽哪一级中断请求
    {
        dev->isr |= bit;  // 把所选出的最高优先级中断请求保存到正在服务寄存器ISR中，即ISR中对应比特被置位
        dev->irr &= ~bit; // 与此同时，中断请求寄存器 IRR 中的对应比特位被复位，表示该中断请求开始被处理
        dev->current_irq = line;
    }
}

// 寄存器读取操作方法
static exception_t i8259a_pic_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    i8259a_pic_device *dev = (i8259a_pic_device *)conf_obj->obj;

    switch (offset)
    {
        case _CW_A0:
            if (dev->rr) // read register command
            {
                if (dev->ris)
                {
                    *(uint8_t *)buf = dev->isr;
                }
                else
                {
                    *(uint8_t *)buf = dev->irr;
                }
            }
            else if (dev->p) // poll command
            {
                dev->p = 0;
                i8259a_pic_sent_intnum(dev);
                *(uint8_t *)buf = 0x80 | dev->current_irq;
            }
            break;
        case _CW_A1:
            *(uint8_t *)buf = dev->imr;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in i8259a_pic\n", offset);
            break;
    }
    return No_exp;
}

// 寄存器写入操作方法
static exception_t i8259a_pic_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    i8259a_pic_device *dev = (i8259a_pic_device *)conf_obj->obj;
    uint8_t val = *(uint8_t *)buf;

    switch (offset)
    {
        case _CW_A0:
            switch (dev->init)
            {
                case 1:
                    if (val & 0x10) // ICW1
                    {
                        dev->init++;
                        dev->ic4 = val & 0x1;
                        dev->sngl = val & 0x2;
                        dev->adi = val & 0x4;
                        dev->ltim = val & 0x8;
                    }
                    break;
                case 3:
                case 4:
                    dev->init = 5;
                case 5:
                    if ((val & 0x18) == 0x0) // OCW2
                    {
                        dev->eoi = val & 0x20;
                        dev->sl = val & 0x40;
                        dev->r = val & 0x80;
                        if (dev->eoi)
                        {
                            if (dev->sl)
                            {
                                dev->isr &= ~bin2bit(val & 0x7);
                            }
                            else
                            {
                                dev->isr &= ~bin2bit(dev->current_irq);
                            }
                        }
                    }
                    else if ((val & 0x18) == 0x8) // OCW3
                    {
                        dev->ris = val & 0x1;
                        dev->rr = val & 0x2;
                        dev->p = val & 0x4;
                        dev->smm = val & 0x20;
                        dev->esmm = val & 0x40;
                    }
                    break;
                default:
                    break;
            }
            break;

        case _CW_A1:
            switch (dev->init)
            {
                case 2: // ICW2
                    dev->init++;
                    dev->vector_base = val >> 3;
                    break;
                case 3:
                    dev->init++;
                    if (!dev->sngl) // ICW3
                    {
                        break;
                    }
                case 4:
                    dev->init++;
                    if (dev->ic4) // ICW4
                    {
                        dev->upm = val & 0x1;
                        dev->aeoi = val & 0x2;
                        dev->ms = val & 0x4;
                        dev->buf = val & 0x8;
                        dev->sfnm = val & 0x10;
                        break;
                    }
                case 5: // OCW1
                    dev->imr = val;
                    // if (!(dev->imr & (0x1 | 0x10)))
                    // {
                    //     i8259a_pic_ask_for_interrupt(dev);
                    // }
                    break;
                default:
                    break;
            }
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in i8259a_pic\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器接口:根据寄存器ID获取寄存器名
static char *i8259a_pic_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_name[id];
}

// 寄存器接口:根据寄存器ID设置寄存器值
static exception_t i8259a_pic_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    i8259a_pic_device *dev = (i8259a_pic_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    *((uint32_t *)regs + id) = value;

    return No_exp;
}

// 寄存器接口:获取设备有多少寄存器
static uint32_t i8259a_pic_get_regnum(conf_object_t *conf_obj)
{
    return sizeof(struct registers) / 4;
}

// 寄存器接口:根据寄存器ID获取寄存器值
static uint32_t i8259a_pic_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    i8259a_pic_device *dev = (i8259a_pic_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    uint32_t *regs_value = (uint32_t *)regs + id;

    return *regs_value;
}

// 寄存器接口:根据寄存器名获取寄存器ID
static uint32_t i8259a_pic_get_regid_by_name(conf_object_t *conf_obj, char *name)
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
static uint32_t i8259a_pic_get_regoffset_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_offset[id];
}

// 中断信号:上升沿
static int i8259a_pic_raise_signal(conf_object_t *conf_obj, int line)
{
    i8259a_pic_device *dev = (i8259a_pic_device *)conf_obj->obj;
    if (dev->init != 5)
        return 0;

    // TODO: To avoid collision
    // check priority
    if (dev->irr && line != 4)
    {
        return;
    }
    int bit = bin2bit(line);
    // dev->irr |= bit;
    dev->irr = bit;
    i8259a_pic_ask_for_interrupt(dev);

    // dev->current_irq = line;
    // int bit = bin2bit(line);
    // dev->irr |= bit;
    // if (!(dev->imr & bit)) // 中断屏蔽寄存器 IMR (Interrup Mask Register)用于保存被屏蔽的中断请求线对应的比特位，哪个比特位被置1就屏蔽哪一级中断请求
    // {
    //     dev->isr |= bit;  // 把所选出的最高优先级中断请求保存到正在服务寄存器ISR中，即ISR中对应比特被置位
    //     dev->irr &= ~bit; // 与此同时，中断请求寄存器 IRR 中的对应比特位被复位，表示该中断请求开始被处理
    //     if (dev->core_signal.iface)
    //     {
    //         dev->core_signal.iface->signal(dev->core_signal.obj, NULL);
    //         skyeye_log(Error_log, __FUNCTION__, "do interrupt at line %d , currentirq %d \n", line, dev->current_irq);
    //     }
    //     else
    //     {
    //         skyeye_log(Error_log, __FUNCTION__, "I8259A PIC can't access to the Core, did you forget to connect them?\n");
    //     }
    //     if (dev->aeoi)
    //     {
    //         // dev->isr &= ~bit;
    //     }
    // }

    return No_exp;
}

// 中断信号:下降沿
static int i8259a_pic_lower_signal(conf_object_t *conf_obj, int line)
{
    i8259a_pic_device *dev = (i8259a_pic_device *)conf_obj->obj;

    return No_exp;
}

// 中断信号:事件
static int i8259a_pic_event_signal(conf_object_t *conf_obj, int line, void *args)
{
    i8259a_pic_device *dev = (i8259a_pic_device *)conf_obj->obj;

    return No_exp;
}

// 实例化一个新设备对象，obj_name是在json文件中配置的名称
static conf_object_t *new_i8259a_pic(const char *obj_name)
{
    i8259a_pic_device *dev = skyeye_mm_zero(sizeof(i8259a_pic_device));
    dev->obj = new_conf_object(obj_name, dev);
    // TODO: 在此添加自定义源码

    // 寄存器初始化
    struct registers *regs = &(dev->regs);
    dev->pr = 0;
    dev->imr = 0;
    dev->irr = 0;
    dev->isr = 0;
    dev->init = 1;
    return dev->obj;
}

// 配置实例化的设备对象
static exception_t config_i8259a_pic(conf_object_t *conf_obj)
{
    i8259a_pic_device *dev = (i8259a_pic_device *)conf_obj->obj;
    // TODO: 在此添加自定义源码

    return No_exp;
}

// 释放设备对象来做一些内存释放
static exception_t free_i8259a_pic(conf_object_t *conf_obj)
{
    i8259a_pic_device *dev = (i8259a_pic_device *)conf_obj->obj;

    skyeye_free(conf_obj->objname);
    skyeye_free(conf_obj->obj);
    skyeye_free(conf_obj);

    return No_exp;
}

// 系统设置设备 core_signal 接口功能
static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    i8259a_pic_device *dev = (i8259a_pic_device *)obj->obj;

    dev->core_signal.obj = obj2;
    if (obj2 == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from NULL object fail.\n", CORE_SIGNAL_INTF_NAME);
        return Not_found_exp;
    }
    dev->core_signal.iface = (core_signal_intf *)SKY_get_iface(obj2, CORE_SIGNAL_INTF_NAME);
    if (dev->core_signal.iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from %s fail.\n", CORE_SIGNAL_INTF_NAME, obj2->objname);
        return Not_found_exp;
    }

    return No_exp;
}

// 系统获取设备 core_signal 接口功能
static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    i8259a_pic_device *dev = (i8259a_pic_device *)obj->obj;

    *obj2 = dev->core_signal.obj;
    *port = NULL;

    return No_exp;
}

// 初始化模块类，并在SkyEye加载模块时执行以下代码
void init_i8259a_pic(void)
{
    // 向SkyEye注册一个设备
    static skyeye_class_t class_data =
        {
            .class_name = "i8259a_pic",
            .class_desc = "Intel 8259A Programmable Interrupt Controller",
            .new_instance = new_i8259a_pic,
            .free_instance = free_i8259a_pic,
            .config_instance = config_i8259a_pic,
            .module_type = SKYML_OFFCHIP,
        };
    conf_class_t *dev_class = SKY_register_device_class(class_data.class_name, &class_data);

    // 设置寄存器地址读写方法
    static memory_space_intf io_memory_interface =
        {
            .read = i8259a_pic_read,
            .write = i8259a_pic_write};
    SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

    // 注册访问接口(用于SkyEye图形控制接口)
    static skyeye_reg_intf reg_interface =
        {
            .get_regvalue_by_id = i8259a_pic_get_regvalue_by_id,
            .get_regname_by_id = i8259a_pic_get_regname_by_id,
            .set_regvalue_by_id = i8259a_pic_set_regvalue_by_id,
            .get_regnum = i8259a_pic_get_regnum,
            .get_regid_by_name = i8259a_pic_get_regid_by_name,
            .get_regoffset_by_id = i8259a_pic_get_regoffset_by_id};
    SKY_register_iface(dev_class, SKYEYE_REG_INTF, &reg_interface);

    // 中断管理器中断信号接口
    static const general_signal_iface_t general_signal_interface =
        {
            .raise_signal = i8259a_pic_raise_signal,
            .lower_signal = i8259a_pic_lower_signal,
            .event_signal = i8259a_pic_event_signal};
    SKY_register_iface(dev_class, GENERAL_SIGNAL_INTF_NAME, &general_signal_interface);

    // 接口方法列表
    static const struct InterfaceDescription ifaces[] =
        {
            (struct InterfaceDescription){
                .name = MEMORY_SPACE_INTF_NAME,
                .iface = &io_memory_interface},
            (struct InterfaceDescription){
                .name = SKYEYE_REG_INTF,
                .iface = &reg_interface},
            (struct InterfaceDescription){
                .name = GENERAL_SIGNAL_INTF_NAME,
                .iface = &general_signal_interface},
        };

    // 接口连接方法列表
    static const struct ConnectDescription connects[] =
        {
            (struct ConnectDescription){
                .name = CORE_SIGNAL_INTF_NAME,
                .set = core_signal_set,
                .get = core_signal_get},
        };
    class_register_ifaces(dev_class, ifaces);
    class_register_connects(dev_class, connects);
}
