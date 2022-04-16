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
#include "mpc5554_stm.h"

#define STM_CR_CPS_MASK      0x0000FF00
#define STM_CR_TEN_MASK      0x00000001
#define STM_CRR0_CEN_MASK    0x00000001

#define PLL_CLOCK_MHZ 120  //MHZ
#define IRQ_NUM_STM_TIMER0  200

void stm_CNT_handler(conf_object_t *conf_obj)
{
    mpc5554_stm_device *dev = conf_obj->obj;
    dev->regs.STM_CNT++;
}

void stm_timer0_interrupt_handler(conf_object_t *conf_obj)
{
    mpc5554_stm_device *dev = conf_obj->obj;

    if(dev->stm_enable == 1)
    {
        if(dev->regs.STM_CNT >= dev-> regs.STM_CMP0)
        {
            /*发送中断信号*/
            if (dev->signal_iface != NULL)
            {
                dev->signal_iface->raise_signal(dev->signal, IRQ_NUM_STM_TIMER0);
            }
        }
    }
}

// 寄存器读取操作方法
static exception_t mpc5554_stm_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc5554_stm_device *dev = (mpc5554_stm_device *)conf_obj->obj;

    switch (offset)
    {
        case _STM_CR:
            *(uint32_t *)buf = dev->regs.STM_CR;
            break;
        case _STM_CNT:
            *(uint32_t *)buf = dev->regs.STM_CNT;
            break;
        case _STM_CCR0:
            *(uint32_t *)buf = dev->regs.STM_CCR0;
            break;
        case _STM_CIR0:
            *(uint32_t *)buf = dev->regs.STM_CIR0;
            break;
        case _STM_CMP0:
            *(uint32_t *)buf = dev->regs.STM_CMP0;
            break;
        case _STM_CCR1:
            *(uint32_t *)buf = dev->regs.STM_CCR1;
            break;
        case _STM_CIR1:
            *(uint32_t *)buf = dev->regs.STM_CIR1;
            break;
        case _STM_CMP1:
            *(uint32_t *)buf = dev->regs.STM_CMP1;
            break;
        case _STM_CCR2:
            *(uint32_t *)buf = dev->regs.STM_CCR2;
            break;
        case _STM_CIR2:
            *(uint32_t *)buf = dev->regs.STM_CIR2;
            break;
        case _STM_CMP2:
            *(uint32_t *)buf = dev->regs.STM_CMP2;
            break;
        case _STM_CCR3:
            *(uint32_t *)buf = dev->regs.STM_CCR3;
            break;
        case _STM_CIR3:
            *(uint32_t *)buf = dev->regs.STM_CIR3;
            break;
        case _STM_CMP3:
            *(uint32_t *)buf = dev->regs.STM_CMP3;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in mpc5554_stm\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器写入操作方法
static exception_t mpc5554_stm_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc5554_stm_device *dev = (mpc5554_stm_device *)conf_obj->obj;
    uint32_t val = *(uint32_t *)buf;
    uint32_t period_timer;

    switch (offset)
    {
        case _STM_CR:
            dev->regs.STM_CR = val;
            dev->val_cr_cps = (dev->regs.STM_CR & STM_CR_CPS_MASK) >> 8;
            if (dev->regs.STM_CR & STM_CR_TEN_MASK)
            {
                dev->stm_enable = 1;
                period_timer = (dev->val_cr_cps + 1) / PLL_CLOCK_MHZ;
                //printf("in func: %s, period_timer:0x%x\n", __func__, period_timer);
                system_register_timer_handler(conf_obj, (uint32_t)period_timer, (time_sched_t)stm_CNT_handler, SCHED_US|SCHED_NORMAL);
            } else
            {
                dev->stm_enable = 0;
            }
            break;
        case _STM_CNT:
            dev->regs.STM_CNT = val;
            break;
        case _STM_CCR0:
            dev->regs.STM_CCR0 = val;
            if(dev->regs.STM_CCR0 & STM_CRR0_CEN_MASK)
            {
                if (dev->stm_enable == 1)
                {
                    period_timer = (dev->val_cr_cps + 1) / PLL_CLOCK_MHZ * dev->regs.STM_CMP0;
                    system_register_timer_handler(conf_obj, (uint32_t)period_timer, (time_sched_t)stm_timer0_interrupt_handler, SCHED_US|SCHED_NORMAL);
                }
            }
            break;
        case _STM_CIR0:
            dev->regs.STM_CIR0 = val;
            break;
        case _STM_CMP0:
            dev->regs.STM_CMP0 = val;
            break;
        case _STM_CCR1:
            dev->regs.STM_CCR1 = val;
            break;
        case _STM_CIR1:
            dev->regs.STM_CIR1 = val;
            break;
        case _STM_CMP1:
            dev->regs.STM_CMP1 = val;
            break;
        case _STM_CCR2:
            dev->regs.STM_CCR2 = val;
            break;
        case _STM_CIR2:
            dev->regs.STM_CIR2 = val;
            break;
        case _STM_CMP2:
            dev->regs.STM_CMP2 = val;
            break;
        case _STM_CCR3:
            dev->regs.STM_CCR3 = val;
            break;
        case _STM_CIR3:
            dev->regs.STM_CIR3 = val;
            break;
        case _STM_CMP3:
            dev->regs.STM_CMP3 = val;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in mpc5554_stm\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器接口:根据寄存器ID获取寄存器名
static char* mpc5554_stm_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_name[id];
}

// 寄存器接口:根据寄存器ID设置寄存器值
static exception_t mpc5554_stm_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    mpc5554_stm_device *dev = (mpc5554_stm_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    *((uint32_t*)regs + id) = value;

    return No_exp;
}

// 寄存器接口:获取设备有多少寄存器
static uint32_t mpc5554_stm_get_regnum(conf_object_t *conf_obj)
{
    //return sizeof(struct registers) / 4;
    return 0;
}

// 寄存器接口:根据寄存器ID获取寄存器值
static uint32_t mpc5554_stm_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    mpc5554_stm_device *dev = (mpc5554_stm_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    uint32_t* regs_value = (uint32_t*)regs + id;

    return *regs_value;
}

// 寄存器接口:根据寄存器名获取寄存器ID
static uint32_t mpc5554_stm_get_regid_by_name(conf_object_t *conf_obj, char *name)
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
static uint32_t mpc5554_stm_get_regoffset_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_offset[id];
}

// 实例化一个新设备对象，obj_name是在json文件中配置的名称
static conf_object_t* new_mpc5554_stm(const char *obj_name)
{
    mpc5554_stm_device* dev = skyeye_mm_zero(sizeof(mpc5554_stm_device));
    dev->obj = new_conf_object(obj_name, dev);
    // TODO: 在此添加自定义源码

    // 寄存器初始化
    struct registers *regs = &(dev->regs);
    regs->STM_CR = 0;
    regs->STM_CNT = 0;
    regs->STM_CCR0 = 0;
    regs->STM_CIR0 = 0;
    regs->STM_CMP0 = 0;
    regs->STM_CCR1 = 0;
    regs->STM_CIR1 = 0;
    regs->STM_CMP1 = 0;
    regs->STM_CCR2 = 0;
    regs->STM_CIR2 = 0;
    regs->STM_CMP2 = 0;
    regs->STM_CCR3 = 0;
    regs->STM_CIR3 = 0;
    regs->STM_CMP3 = 0;

    return dev->obj;
}

// 配置实例化的设备对象
static exception_t config_mpc5554_stm(conf_object_t* conf_obj)
{
    mpc5554_stm_device *dev = (mpc5554_stm_device *)conf_obj->obj;
    // TODO: 在此添加自定义源码


    return No_exp;
}

// 释放设备对象来做一些内存释放
static exception_t free_mpc5554_stm(conf_object_t* conf_obj)
{
    mpc5554_stm_device *dev = (mpc5554_stm_device *)conf_obj->obj;

    skyeye_free(conf_obj->objname);
    skyeye_free(conf_obj->obj);
    skyeye_free(conf_obj);

    return No_exp;
}

static exception_t timer_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc5554_stm_device* dev = (mpc5554_stm_device*)(obj->obj);
    dev->signal = obj2;
    dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);

    if (dev->signal_iface == NULL)
    {
        if (dev->signal == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
        } else
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);
        }

        return Not_found_exp;
    }

    return No_exp;
}

static exception_t timer_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    mpc5554_stm_device* dev = (mpc5554_stm_device*)(obj->obj);
    *obj2 = dev->signal;
    *port = NULL;
    return No_exp;
}

// 初始化模块类，并在SkyEye加载模块时执行以下代码
void init_mpc5554_stm(void)
{
    // 向SkyEye注册一个设备
    static skyeye_class_t class_data =
    {
        .class_name      = "mpc5554_stm",
        .class_desc      = "mpc5554_stm",
        .new_instance    = new_mpc5554_stm,
        .free_instance   = free_mpc5554_stm,
        .config_instance = config_mpc5554_stm,
        .module_type     = SKYML_OFFCHIP,
    };
    conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);

    // 设置寄存器地址读写方法
    static memory_space_intf io_memory_interface =
    {
        .read  = mpc5554_stm_read,
        .write = mpc5554_stm_write
    };
    SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

    // 注册访问接口(用于SkyEye图形控制接口)
    static skyeye_reg_intf reg_interface =
    {
        .get_regvalue_by_id  = mpc5554_stm_get_regvalue_by_id,
        .get_regname_by_id   = mpc5554_stm_get_regname_by_id,
        .set_regvalue_by_id  = mpc5554_stm_set_regvalue_by_id,
        .get_regnum          = mpc5554_stm_get_regnum,
        .get_regid_by_name   = mpc5554_stm_get_regid_by_name,
        .get_regoffset_by_id = mpc5554_stm_get_regoffset_by_id
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
        (struct ConnectDescription)
        {
            .name = GENERAL_SIGNAL_INTF_NAME,
            .set = timer_signal_set,
            .get = timer_signal_get,
        },
    };
    class_register_ifaces(dev_class, ifaces);
    class_register_connects(dev_class, connects);
}
