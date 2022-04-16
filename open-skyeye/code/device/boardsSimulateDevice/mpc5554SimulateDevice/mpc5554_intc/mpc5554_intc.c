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
#include "mpc5554_intc.h"

#define INTC_MCR_HVEN_MASK   0x01

// 寄存器读取操作方法
static exception_t mpc5554_intc_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc5554_intc_device *dev = (mpc5554_intc_device *)conf_obj->obj;
    uint32_t data = 0;

    if (offset >= 0x20 && offset <= 0x27)
    {
        uint8_t index = (offset - 0x20);
        *(uint32_t *) buf = dev->regs.SSCIR[index] & 0xff;
        return No_exp;
    }

    if (offset >= 0x40 && offset <= 0x1a8)
    {
        uint8_t index = (offset - 0x40);
        *(uint32_t *) buf = dev->regs.PSR[index] & 0xff;
        return No_exp;
    }
    switch (offset)
    {
        case _MCR:
        case _MCR + 0x1:
        case _MCR + 0x2:
        case _MCR + 0x3:
            data = dev->regs.MCR.R;
            break;
        case _CPR:
        case _CPR + 0x1:
        case _CPR + 0x2:
        case _CPR + 0x3:
            data = dev->regs.CPR.R;
            break;
        case _IACKR:
            data = dev->regs.IACKR.R;
            break;
        case _EOIR:
            data = dev->regs.EOIR.R;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in mpc5554_intc\n", offset);
            break;
    }

    if (count == 1)
    {
        *(uint8_t *)buf = data & 0xff;
    } else if(count == 2)
    {
        *(uint16_t *)buf = data & 0xffff;
    } else
    {
        *(uint32_t *)buf = data;
    }

    //printf("in func:%s, offset:0x%x, val:0x%x\n", __func__, offset, *(uint32_t *)buf);

    return No_exp;
}

// 寄存器写入操作方法
static exception_t mpc5554_intc_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc5554_intc_device *dev = (mpc5554_intc_device *)conf_obj->obj;
    uint32_t val = *(uint32_t *)buf;

    //printf("in func:%s, offset:0x%x, val:0x%x\n", __func__, offset, val);

    if (offset >= 0x20 && offset <= 0x27)
    {
        uint8_t index = (offset - 0x20);
        dev->regs.SSCIR[index] = (val & 0xff);
        return No_exp;
    }

    if (offset >= 0x40 && offset <= 0x1a8)
    {
        uint8_t index = (offset - 0x40);
        dev->regs.PSR[index] = (val & 0xff);
        return No_exp;
    }

    switch (offset)
    {
        case _MCR:
            dev->regs.MCR.R = val;
            break;
        case _MCR + 0x1:
        case _MCR + 0x2:
        case _MCR + 0x3:
            dev->regs.MCR.R |= val;
            break;
        case _CPR:
            dev->regs.CPR.R = val;
        case _CPR + 0x1:
        case _CPR + 0x2:
        case _CPR + 0x3:
             dev->regs.CPR.R |= val;
             break;
        case _IACKR:
            dev->regs.IACKR.R = val;
            break;
        case _EOIR:
            dev->regs.EOIR.R = val;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in mpc5554_intc\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器接口:根据寄存器ID获取寄存器名
static char* mpc5554_intc_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_name[id];
}

// 寄存器接口:根据寄存器ID设置寄存器值
static exception_t mpc5554_intc_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    mpc5554_intc_device *dev = (mpc5554_intc_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    *((uint32_t*)regs + id) = value;

    return No_exp;
}

// 寄存器接口:获取设备有多少寄存器
static uint32_t mpc5554_intc_get_regnum(conf_object_t *conf_obj)
{
    return 6;
}

// 寄存器接口:根据寄存器ID获取寄存器值
static uint32_t mpc5554_intc_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    mpc5554_intc_device *dev = (mpc5554_intc_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    uint32_t* regs_value = (uint32_t*)regs + id;

    return *regs_value;
}

// 寄存器接口:根据寄存器名获取寄存器ID
static uint32_t mpc5554_intc_get_regid_by_name(conf_object_t *conf_obj, char *name)
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
static uint32_t mpc5554_intc_get_regoffset_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_offset[id];
}

//中断信号
static int mpc5554_intc_raise_signal(conf_object_t *conf_obj, int line)
{
     mpc5554_intc_device *dev = (mpc5554_intc_device *)conf_obj->obj;
     interrupt_signal_t irq_signal;

     if (dev->regs.MCR.R & INTC_MCR_HVEN_MASK)
     {
        irq_signal.powerpc_signal.hardware_vector_mode = 1;
     } else
     {
        irq_signal.powerpc_signal.hardware_vector_mode = 0;
     }

     irq_signal.powerpc_signal.irq_vector_number = line;

     dev->regs.IACKR.R |= (line << 2);

     if ((dev->core_signal.obj))
     {
         dev->core_signal.iface->signal(dev->core_signal.obj, &irq_signal);
     }

     return No_exp;
}

// 实例化一个新设备对象，obj_name是在json文件中配置的名称
static conf_object_t* new_mpc5554_intc(const char *obj_name)
{
    mpc5554_intc_device* dev = skyeye_mm_zero(sizeof(mpc5554_intc_device));
    dev->obj = new_conf_object(obj_name, dev);
    // TODO: 在此添加自定义源码

    // 寄存器初始化
    struct registers *regs = &(dev->regs);
    regs->MCR.R = 0;
    regs->CPR.R = 0xf;
    regs->IACKR.R = 0;
    regs->EOIR.R = 0;
    regs->SSCIR[0] = 0;
    regs->SSCIR[1] = 0;

    return dev->obj;
}

// 配置实例化的设备对象
static exception_t config_mpc5554_intc(conf_object_t* conf_obj)
{
    mpc5554_intc_device *dev = (mpc5554_intc_device *)conf_obj->obj;
    // TODO: 在此添加自定义源码


    return No_exp;
}

// 释放设备对象来做一些内存释放
static exception_t free_mpc5554_intc(conf_object_t* conf_obj)
{
    mpc5554_intc_device *dev = (mpc5554_intc_device *)conf_obj->obj;

    skyeye_free(conf_obj->objname);
    skyeye_free(conf_obj->obj);
    skyeye_free(conf_obj);

    return No_exp;
}

/* 处理器接口配置 */
static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc5554_intc_device *dev =  obj->obj;
    dev->core_signal.obj = obj2;
    dev->core_signal.iface = (core_signal_intf *)SKY_get_iface(dev->core_signal.obj, CORE_SIGNAL_INTF_NAME);
    if (dev->core_signal.iface == NULL)
    {
        if ( dev->core_signal.obj == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", CORE_SIGNAL_INTF_NAME);
        } else
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", CORE_SIGNAL_INTF_NAME, obj2->objname);
        }

        return Not_found_exp;
    }

    return No_exp;
}

static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    mpc5554_intc_device *dev = obj->obj;
    *obj2 = dev->core_signal.obj;
    *port = NULL;
    return No_exp;
}

// 初始化模块类，并在SkyEye加载模块时执行以下代码
void init_mpc5554_intc(void)
{
    // 向SkyEye注册一个设备
    static skyeye_class_t class_data =
    {
        .class_name      = "mpc5554_intc",
        .class_desc      = "mpc5554_intc",
        .new_instance    = new_mpc5554_intc,
        .free_instance   = free_mpc5554_intc,
        .config_instance = config_mpc5554_intc,
        .module_type     = SKYML_OFFCHIP,
    };
    conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);

    // 设置寄存器地址读写方法
    static memory_space_intf io_memory_interface =
    {
        .read  = mpc5554_intc_read,
        .write = mpc5554_intc_write
    };
    SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

    // 注册访问接口(用于SkyEye图形控制接口)
    static skyeye_reg_intf reg_interface =
    {
        .get_regvalue_by_id  = mpc5554_intc_get_regvalue_by_id,
        .get_regname_by_id   = mpc5554_intc_get_regname_by_id,
        .set_regvalue_by_id  = mpc5554_intc_set_regvalue_by_id,
        .get_regnum          = mpc5554_intc_get_regnum,
        .get_regid_by_name   = mpc5554_intc_get_regid_by_name,
        .get_regoffset_by_id = mpc5554_intc_get_regoffset_by_id
    };
    SKY_register_iface(dev_class, SKYEYE_REG_INTF, &reg_interface);


    static const general_signal_iface_t general_signal_interface =
    {
        .raise_signal = mpc5554_intc_raise_signal,
    };
    SKY_register_iface(dev_class, GENERAL_SIGNAL_INTF_NAME, &general_signal_interface);

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
         (struct InterfaceDescription)
        {
            .name  = GENERAL_SIGNAL_INTF_NAME,
            .iface = &general_signal_interface
        },
    };

    // 接口连接方法列表
    static const struct ConnectDescription connects[] =
    {
        /* ConnectDescription */
        (struct ConnectDescription )
        {
            .name = CORE_SIGNAL_INTF_NAME,
            .set = core_signal_set,
            .get = core_signal_get,
        },
    };
    class_register_ifaces(dev_class, ifaces);
    class_register_connects(dev_class, connects);
}
