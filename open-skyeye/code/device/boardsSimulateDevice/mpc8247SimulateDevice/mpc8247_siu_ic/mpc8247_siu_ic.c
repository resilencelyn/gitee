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
#include "mpc8247_siu_ic.h"

static void handle_interrupt(conf_object_t *conf_obj)
{
    mpc8247_siu_ic_device* dev = (mpc8247_siu_ic_device*)conf_obj->obj;

    if (dev->core_signal.iface)
    {
        dev->core_signal.iface->signal(dev->core_signal.obj, NULL);
    }
}

// 寄存器读取操作方法
static exception_t mpc8247_siu_ic_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc8247_siu_ic_device *dev = (mpc8247_siu_ic_device *)conf_obj->obj;

    switch (offset)
    {
        case _SICR:
            *(uint16_t *)buf = dev->regs.SICR;
            break;
        case _SIVEC:
            *(uint32_t *)buf = dev->regs.SIVEC;
            break;
        case _SIPNR_H:
            *(uint32_t *)buf = dev->regs.SIPNR_H;
            break;
        case _SIPNR_L:
            *(uint32_t *)buf = dev->regs.SIPNR_L;
            break;
        case _SIPRR:
            *(uint32_t *)buf = dev->regs.SIPRR;
            break;
        case _SCPRR_H:
            *(uint32_t *)buf = dev->regs.SCPRR_H;
            break;
        case _SCPRR_L:
            *(uint32_t *)buf = dev->regs.SCPRR_L;
            break;
        case _SIMR_H:
            *(uint32_t *)buf = dev->regs.SIMR_H;
            break;
        case _SIMR_L:
            *(uint32_t *)buf = dev->regs.SIMR_L;
            break;
        case _SIEXR:
            *(uint32_t *)buf = dev->regs.SIEXR;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in mpc8247_siu_ic\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器写入操作方法
static exception_t mpc8247_siu_ic_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc8247_siu_ic_device *dev = (mpc8247_siu_ic_device *)conf_obj->obj;

    uint32_t val32 = *(uint32_t *)buf;
    uint16_t val16 = *(uint32_t *)buf;

    switch (offset)
    {
        case _SICR:
            dev->regs.SICR = val16;
            break;
        case _SIVEC:
            dev->regs.SIVEC = val32;
            break;
        case _SIPNR_H:
            dev->regs.SIPNR_H = val32;
            break;
        case _SIPNR_L:
            dev->regs.SIPNR_L &= ~(val32);
            break;
        case _SIPRR:
            dev->regs.SIPRR = val32;
            break;
        case _SCPRR_H:
            dev->regs.SCPRR_H &= ~val32;
            break;
        case _SCPRR_L:
            dev->regs.SCPRR_L &= ~val32;
            break;
        case _SIMR_H:
            dev->regs.SIMR_H = val32;
            break;
        case _SIMR_L:
            dev->regs.SIMR_L = val32;
            break;
        case _SIEXR:
            dev->regs.SIEXR = val32;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in mpc8247_siu_ic\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器接口:根据寄存器ID获取寄存器名
static char* mpc8247_siu_ic_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_name[id];
}

// 寄存器接口:根据寄存器ID设置寄存器值
static exception_t mpc8247_siu_ic_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    mpc8247_siu_ic_device *dev = (mpc8247_siu_ic_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    *((uint32_t*)regs + id) = value;

    return No_exp;
}

// 寄存器接口:获取设备有多少寄存器
static uint32_t mpc8247_siu_ic_get_regnum(conf_object_t *conf_obj)
{
    return 10;
}

// 寄存器接口:根据寄存器ID获取寄存器值
static uint32_t mpc8247_siu_ic_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    mpc8247_siu_ic_device *dev = (mpc8247_siu_ic_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    uint32_t* regs_value = (uint32_t*)regs + id;

    return *regs_value;
}

// 寄存器接口:根据寄存器名获取寄存器ID
static uint32_t mpc8247_siu_ic_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
    uint32_t regnum = 10;
    int i;

    for (i = 0; i < regnum; i++)
    {
        if (strcmp(name, regs_name[i]) == 0)
            return i;
    }

    return regnum;
}

// 寄存器接口:根据寄存器ID获取寄存器偏移
static uint32_t mpc8247_siu_ic_get_regoffset_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_offset[id];
}

void SMCIntHandle(conf_object_t *conf_obj, int line)
{
    mpc8247_siu_ic_device *dev = (mpc8247_siu_ic_device *)conf_obj->obj;
    int IEFlag = 0;

    switch (line)
    {
        case 4:
            if(dev->regs.SIMR_L & SIPNRL_SMC1)
            {
                IEFlag = 1;
            }
            break;
        case 5:
            dev->regs.SIPNR_L |= dev->regs.SIPNR_L & (~SIPNRL_SMC2);
            if ((dev->regs.SIPNR_L & SIPNRL_SMC2)
                & (dev->regs.SIMR_L & SIPNRL_SMC2))
            {
                IEFlag = 1;
            }
            break;
        default:
            printf("In %s | IntNo : %d is invild. \n", __func__, line);
            break;
    }

    if (IEFlag == 1)
    {
        handle_interrupt(conf_obj);
    }
}

// 中断信号:上升沿
static int mpc8247_siu_ic_raise_signal(conf_object_t *conf_obj, int line)
{
    mpc8247_siu_ic_device *dev = (mpc8247_siu_ic_device *)conf_obj->obj;
    dev->regs.SIVEC = INTERRUPT_BIT_OFFSET(line);

    switch (line)
    {
        case 4:
        case 5:
            SMCIntHandle(conf_obj, line);
            break;
        defalut:
            printf("In %s | IntNo : %d is invild. \n", __func__, line);
            break;
    }

    return No_exp;
}

// 中断信号:下降沿
static int mpc8247_siu_ic_lower_signal(conf_object_t *conf_obj, int line)
{
    mpc8247_siu_ic_device *dev = (mpc8247_siu_ic_device *)conf_obj->obj;

    return No_exp;
}

// 中断信号:事件
static int mpc8247_siu_ic_event_signal(conf_object_t *conf_obj, int line, void *args)
{
    mpc8247_siu_ic_device *dev = (mpc8247_siu_ic_device *)conf_obj->obj;

    return No_exp;
}

// 实例化一个新设备对象，obj_name是在json文件中配置的名称
static conf_object_t* new_mpc8247_siu_ic(const char *obj_name)
{
    mpc8247_siu_ic_device* dev = skyeye_mm_zero(sizeof(mpc8247_siu_ic_device));
    dev->obj = new_conf_object(obj_name, dev);
    // TODO: 在此添加自定义源码

    // 寄存器初始化
    struct registers *regs = &(dev->regs);
    regs->SICR = 0;
    regs->SIVEC = 0;
    regs->SIPNR_H = 0;
    regs->SIPNR_L = 0;
    regs->SIPRR = 0x5309770;
    regs->SCPRR_H = 0x5309770;
    regs->SCPRR_L = 0x5309770;
    regs->SIMR_H = 0;
    regs->SIMR_L = 0;
    regs->SIEXR = 0;
    // 属性初始化
    dev->reserve = 0;
    dev->int_number = 0;

    return dev->obj;
}

// 配置实例化的设备对象
static exception_t config_mpc8247_siu_ic(conf_object_t* conf_obj)
{
    mpc8247_siu_ic_device *dev = (mpc8247_siu_ic_device *)conf_obj->obj;
    // TODO: 在此添加自定义源码

    return No_exp;
}

// 释放设备对象来做一些内存释放
static exception_t free_mpc8247_siu_ic(conf_object_t* conf_obj)
{
    mpc8247_siu_ic_device *dev = (mpc8247_siu_ic_device *)conf_obj->obj;

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

// 系统设置设备 core_signal 接口功能
static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc8247_siu_ic_device *dev = (mpc8247_siu_ic_device *)obj->obj;

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
    mpc8247_siu_ic_device *dev = (mpc8247_siu_ic_device *)obj->obj;

    *obj2 = dev->core_signal.obj;
    *port = NULL;

    return No_exp;
}

// 系统设置设备 reserve 属性
static exception_t set_attr_reserve(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    mpc8247_siu_ic_device *dev = (mpc8247_siu_ic_device *)conf_obj->obj;

    dev->reserve = SKY_attr_uinteger(*value);

    return No_exp;
}

// 系统获取设备 reserve 属性
static attr_value_t get_attr_reserve(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    mpc8247_siu_ic_device *dev = (mpc8247_siu_ic_device *)conf_obj->obj;

    attr_value_t value = SKY_make_attr_uinteger(dev->reserve);

    return value;
}

// 系统设置设备 int_number 属性
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    mpc8247_siu_ic_device *dev = (mpc8247_siu_ic_device *)conf_obj->obj;

    dev->int_number = SKY_attr_uinteger(*value);

    return No_exp;
}

// 系统获取设备 int_number 属性
static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    mpc8247_siu_ic_device *dev = (mpc8247_siu_ic_device *)conf_obj->obj;

    attr_value_t value = SKY_make_attr_uinteger(dev->int_number);

    return value;
}

// 注册设备模块属性
static void class_register_attribute(conf_class_t* dev_class)
{
    SKY_register_attribute(dev_class,
            "reserve",
            get_attr_reserve, NULL,
            set_attr_reserve, NULL,
            SKY_Attr_Optional,
            "uinteger",
            "this is reserve describe");
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
void init_mpc8247_siu_ic(void)
{
    // 向SkyEye注册一个设备
    static skyeye_class_t class_data =
    {
        .class_name      = "mpc8247_siu_ic",
        .class_desc      = "mpc8247_siu_ic",
        .new_instance    = new_mpc8247_siu_ic,
        .free_instance   = free_mpc8247_siu_ic,
        .config_instance = config_mpc8247_siu_ic,
        .module_type     = SKYML_OFFCHIP,
    };
    conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);

    // 设置寄存器地址读写方法
    static memory_space_intf io_memory_interface =
    {
        .read  = mpc8247_siu_ic_read,
        .write = mpc8247_siu_ic_write
    };
    SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

    // 注册访问接口(用于SkyEye图形控制接口)
    static skyeye_reg_intf reg_interface =
    {
        .get_regvalue_by_id  = mpc8247_siu_ic_get_regvalue_by_id,
        .get_regname_by_id   = mpc8247_siu_ic_get_regname_by_id,
        .set_regvalue_by_id  = mpc8247_siu_ic_set_regvalue_by_id,
        .get_regnum          = mpc8247_siu_ic_get_regnum,
        .get_regid_by_name   = mpc8247_siu_ic_get_regid_by_name,
        .get_regoffset_by_id = mpc8247_siu_ic_get_regoffset_by_id
    };
    SKY_register_iface(dev_class, SKYEYE_REG_INTF, &reg_interface);

    // 中断管理器中断信号接口
    static const general_signal_iface_t general_signal_interface =
    {
        .raise_signal = mpc8247_siu_ic_raise_signal,
        .lower_signal = mpc8247_siu_ic_lower_signal,
        .event_signal = mpc8247_siu_ic_event_signal
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
        (struct ConnectDescription)
        {
            .name = CORE_SIGNAL_INTF_NAME,
            .set  = core_signal_set,
            .get  = core_signal_get
        },
    };
    class_register_ifaces(dev_class, ifaces);
    class_register_connects(dev_class, connects);
    class_register_attribute(dev_class);
}
