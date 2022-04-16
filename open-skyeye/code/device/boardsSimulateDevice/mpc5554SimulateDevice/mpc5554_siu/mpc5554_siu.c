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
#include "mpc5554_siu.h"

// 寄存器读取操作方法
static exception_t mpc5554_siu_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc5554_siu_device *dev = (mpc5554_siu_device *)conf_obj->obj;

    if(offset >= 0x40 && offset <= 0x20c)
    {
        uint8_t index = (offset - 0x40) / 2;
        *(uint32_t *)buf = dev->regs.PCR[index] & 0xffff;

        return No_exp;
    }


    if(offset >= 0x600 && offset <= 0x6d5)
    {
         uint8_t index = (offset - 0x600);
        *(uint32_t *)buf = dev->regs.GPDO[index] & 0xffff;

        return No_exp;
    }

     if(offset >= 0x800 && offset <= 0x8d5)
    {
         uint8_t index = (offset - 0x800);
        *(uint8_t *)buf = dev->regs.GPDI[index] & 0xff;

        return No_exp;
    }

    switch (offset)
    {
        case _MIDR:
            *(uint32_t *)buf = dev->regs.MIDR;
            break;
        case _RSP:
            *(uint32_t *)buf = dev->regs.RSP;
            break;
        case _SRCR:
            *(uint32_t *)buf = dev->regs.SRCR;
            break;
        case _EISR:
            *(uint32_t *)buf = dev->regs.EISR;
            break;
        case _DIRER:
        case _DIRER + 0x1:
        case _DIRER + 0x2:
        case _DIRER + 0x3:
            *(uint32_t *)buf = dev->regs.DIRER;
            break;
        case _DIRSR:
            *(uint32_t *)buf = dev->regs.DIRSR;
            break;
        case _OSR:
            *(uint32_t *)buf = dev->regs.OSR;
            break;
        case _ORER:
            *(uint32_t *)buf = dev->regs.ORER;
            break;
        case _IREER:
            *(uint32_t *)buf = dev->regs.IREER;
            break;
        case _IFEER:
        case _IFEER + 0x1:
        case _IFEER + 0x2:
        case _IFEER + 0x3:
            *(uint32_t *)buf = dev->regs.IFEER;
            break;
        case _IDFR:
        case _IDFR + 0x1:
        case _IDFR + 0x2:
        case _IDFR + 0x3:
            *(uint32_t *)buf = dev->regs.IDFR;
            break;
        case _ETISR:
        case _ETISR + 0x1:
        case _ETISR + 0x2:
        case _ETISR + 0x3:
            *(uint32_t *)buf = dev->regs.ETISR;
            break;
        case _EIISR:
            *(uint32_t *)buf = dev->regs.EIISR;
            break;
        case _DISR:
            *(uint32_t *)buf = dev->regs.DISR;
            break;
        case _CCR:
            *(uint32_t *)buf = dev->regs.CCR;
            break;
        case _ECCR:
            *(uint32_t *)buf = dev->regs.ECCR;
            break;
        case _CARH:
            *(uint32_t *)buf = dev->regs.CARH;
            break;
        case _CARL:
            *(uint32_t *)buf = dev->regs.CARL;
            break;
        case _CBRH:
            *(uint32_t *)buf = dev->regs.CBRH;
            break;
        case _CBRL:
            *(uint32_t *)buf = dev->regs.CBRL;
            break;
        case _HLT:
            *(uint32_t *)buf = dev->regs.HLT;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in mpc5554_siu\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器写入操作方法
static exception_t mpc5554_siu_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc5554_siu_device *dev = (mpc5554_siu_device *)conf_obj->obj;
    uint32_t val = *(uint32_t *)buf;

    if(offset >= 0x40 && offset <= 0x20c)
    {
        uint8_t index = (offset - 0x40) / 2;
        dev->regs.PCR[index] = val & 0xffff;

        return No_exp;
    }

    if(offset >= 0x600 && offset <= 0x6d5)
    {
         uint8_t index = (offset - 0x600);
         dev->regs.GPDO[index] = val & 0xff;

        return No_exp;
    }

     if(offset >= 0x800 && offset <= 0x8d5)
    {
         uint8_t index = (offset - 0x800);
         dev->regs.GPDI[index] = val & 0xff;

        return No_exp;
    }

    switch (offset)
    {
        case _MIDR:
            dev->regs.MIDR = val;
            break;
        case _RSP:
            dev->regs.RSP = val;
            break;
        case _SRCR:
            dev->regs.SRCR = val;
            break;
        case _EISR:
            dev->regs.EISR = val;
            break;
        case _DIRER:
            dev->regs.DIRER = val;
            break;
        case _DIRER + 0x1:
        case _DIRER + 0x2:
        case _DIRER + 0x3:
            dev->regs.DIRER |= val;
            break;
        case _DIRSR:
            dev->regs.DIRSR = val;
            break;
        case _OSR:
            dev->regs.OSR = val;
            break;
        case _ORER:
            dev->regs.ORER = val;
            break;
        case _IREER:
            dev->regs.IREER = val;
            break;
        case _IFEER:
            dev->regs.IFEER = val;
            break;
        case _IFEER+ 0x1:
        case _IFEER+ 0x2:
        case _IFEER+ 0x3:
            dev->regs.IFEER |= val;
            break;
        case _IDFR:
            dev->regs.IDFR = val;
            break;
        case _IDFR+ 0x1:
        case _IDFR+ 0x2:
        case _IDFR+ 0x3:
            dev->regs.IDFR |= val;
            break;
        case _ETISR:
            dev->regs.ETISR = val;
            break;
       case _EIISR:
            dev->regs.EIISR = val;
            break;
        case _EIISR+ 0x1:
        case _EIISR+ 0x2:
        case _EIISR+ 0x3:
            dev->regs.EIISR |= val;
            break;
        case _DISR:
            dev->regs.DISR = val;
            break;
        case _CCR:
            dev->regs.CCR = val;
            break;
        case _ECCR:
            dev->regs.ECCR = val;
            break;
        case _CARH:
            dev->regs.CARH = val;
            break;
        case _CARL:
            dev->regs.CARL = val;
            break;
        case _CBRH:
            dev->regs.CBRH = val;
            break;
        case _CBRL:
            dev->regs.CBRL = val;
            break;
        case _HLT:
            dev->regs.HLT = val;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in mpc5554_siu\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器接口:根据寄存器ID获取寄存器名
static char* mpc5554_siu_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_name[id];
}

// 寄存器接口:根据寄存器ID设置寄存器值
static exception_t mpc5554_siu_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    mpc5554_siu_device *dev = (mpc5554_siu_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    *((uint32_t*)regs + id) = value;

    return No_exp;
}

// 寄存器接口:获取设备有多少寄存器
static uint32_t mpc5554_siu_get_regnum(conf_object_t *conf_obj)
{
    return 0;
}

// 寄存器接口:根据寄存器ID获取寄存器值
static uint32_t mpc5554_siu_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    mpc5554_siu_device *dev = (mpc5554_siu_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    uint32_t* regs_value = (uint32_t*)regs + id;

    return *regs_value;
}

// 寄存器接口:根据寄存器名获取寄存器ID
static uint32_t mpc5554_siu_get_regid_by_name(conf_object_t *conf_obj, char *name)
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
static uint32_t mpc5554_siu_get_regoffset_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_offset[id];
}

// 实例化一个新设备对象，obj_name是在json文件中配置的名称
static conf_object_t* new_mpc5554_siu(const char *obj_name)
{
    mpc5554_siu_device* dev = skyeye_mm_zero(sizeof(mpc5554_siu_device));
    dev->obj = new_conf_object(obj_name, dev);
    // TODO: 在此添加自定义源码

    // 寄存器初始化
    struct registers *regs = &(dev->regs);
    regs->MIDR = 0;
    regs->RSP = 2147483648;
    regs->SRCR = 32768;
    regs->EISR = 0;
    regs->DIRER = 0;
    regs->DIRSR = 0;
    regs->OSR = 0;
    regs->ORER = 0;
    regs->IREER = 0;
    regs->IFEER = 0;
    regs->IDFR = 0;
    regs->ETISR = 0;
    regs->EIISR = 0;
    regs->DISR = 0;
    regs->CCR = 0;
    regs->ECCR = 0;
    regs->CARH = 0;
    regs->CARL = 0;
    regs->CBRH = 0;
    regs->CBRL = 0;

    return dev->obj;
}

// 配置实例化的设备对象
static exception_t config_mpc5554_siu(conf_object_t* conf_obj)
{
    mpc5554_siu_device *dev = (mpc5554_siu_device *)conf_obj->obj;
    // TODO: 在此添加自定义源码


    return No_exp;
}

// 释放设备对象来做一些内存释放
static exception_t free_mpc5554_siu(conf_object_t* conf_obj)
{
    mpc5554_siu_device *dev = (mpc5554_siu_device *)conf_obj->obj;

    skyeye_free(conf_obj->objname);
    skyeye_free(conf_obj->obj);
    skyeye_free(conf_obj);

    return No_exp;
}

// 初始化模块类，并在SkyEye加载模块时执行以下代码
void init_mpc5554_siu(void)
{
    // 向SkyEye注册一个设备
    static skyeye_class_t class_data =
    {
        .class_name      = "mpc5554_siu",
        .class_desc      = "mpc5554_siu",
        .new_instance    = new_mpc5554_siu,
        .free_instance   = free_mpc5554_siu,
        .config_instance = config_mpc5554_siu,
        .module_type     = SKYML_OFFCHIP,
    };
    conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);

    // 设置寄存器地址读写方法
    static memory_space_intf io_memory_interface =
    {
        .read  = mpc5554_siu_read,
        .write = mpc5554_siu_write
    };
    SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

    // 注册访问接口(用于SkyEye图形控制接口)
    static skyeye_reg_intf reg_interface =
    {
        .get_regvalue_by_id  = mpc5554_siu_get_regvalue_by_id,
        .get_regname_by_id   = mpc5554_siu_get_regname_by_id,
        .set_regvalue_by_id  = mpc5554_siu_set_regvalue_by_id,
        .get_regnum          = mpc5554_siu_get_regnum,
        .get_regid_by_name   = mpc5554_siu_get_regid_by_name,
        .get_regoffset_by_id = mpc5554_siu_get_regoffset_by_id
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
