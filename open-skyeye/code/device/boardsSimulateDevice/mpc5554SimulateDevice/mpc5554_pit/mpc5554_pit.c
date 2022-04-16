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
#include "mpc5554_pit.h"

#define PIT_MCR_MDIS_MASK     0x02
#define PIT_TCTRL_TIE_MASK    0x02
#define PIT_TCTRL_TEN_MASK    0x02

#define BUS_CLK_FREQ_MHZ      120

#define IRQ_NUM_PIT_TIMER0    301

void pit_timer0_interrupt_handler(conf_object_t *conf_obj)
{
    mpc5554_pit_device *dev = conf_obj->obj;

    if (dev->signal_iface != NULL)
    {
        dev->signal_iface->raise_signal(dev->signal, IRQ_NUM_PIT_TIMER0);
    }
}

// 寄存器读取操作方法
static exception_t mpc5554_pit_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc5554_pit_device *dev = (mpc5554_pit_device *)conf_obj->obj;

    switch (offset)
    {
        case _PITMCR:
            *(uint32_t *)buf = dev->regs.PITMCR;
            break;
        case _PIT_RTI_LDVAL:
            *(uint32_t *)buf = dev->regs.PIT_RTI_LDVAL;
            break;
        case _PIT_RTI_CVAL:
            *(uint32_t *)buf = dev->regs.PIT_RTI_CVAL;
            break;
        case _PIT_RTI_TCTRL:
            *(uint32_t *)buf = dev->regs.PIT_RTI_TCTRL;
            break;
        case _PIT_RTI_TFLG:
            *(uint32_t *)buf = dev->regs.PIT_RTI_TFLG;
            break;
        case _PIT_TIME0_LDVAL0:
            *(uint32_t *)buf = dev->regs.PIT_TIME0_LDVAL0;
            break;
        case _PIT_TIME0_CVAL0:
            *(uint32_t *)buf = dev->regs.PIT_TIME0_CVAL0;
            break;
        case _PIT_TIME0_TCTRL0:
            *(uint32_t *)buf = dev->regs.PIT_TIME0_TCTRL0;
            break;
        case _PIT_TIME0_TFLG0:
            *(uint32_t *)buf = dev->regs.PIT_TIME0_TFLG0;
            break;
        case _PIT_TIME1_LDVAL1:
            *(uint32_t *)buf = dev->regs.PIT_TIME1_LDVAL1;
            break;
        case _PIT_TIME1_CVAL1:
            *(uint32_t *)buf = dev->regs.PIT_TIME1_CVAL1;
            break;
        case _PIT_TIME1_TCTRL1:
            *(uint32_t *)buf = dev->regs.PIT_TIME1_TCTRL1;
            break;
        case _PIT_TIME1_TFLG1:
            *(uint32_t *)buf = dev->regs.PIT_TIME1_TFLG1;
            break;
        case _PIT_TIME2_LDVAL2:
            *(uint32_t *)buf = dev->regs.PIT_TIME2_LDVAL2;
            break;
        case _PIT_TIME2_CVAL2:
            *(uint32_t *)buf = dev->regs.PIT_TIME2_CVAL2;
            break;
        case _PIT_TIME2_TCTRL2:
            *(uint32_t *)buf = dev->regs.PIT_TIME2_TCTRL2;
            break;
        case _PIT_TIME2_TFLG2:
            *(uint32_t *)buf = dev->regs.PIT_TIME2_TFLG2;
            break;
        case _PIT_TIME3_LDVAL3:
            *(uint32_t *)buf = dev->regs.PIT_TIME3_LDVAL3;
            break;
        case _PIT_TIME3_CVAL3:
            *(uint32_t *)buf = dev->regs.PIT_TIME3_CVAL3;
            break;
        case _PIT_TIME3_TCTRL3:
            *(uint32_t *)buf = dev->regs.PIT_TIME3_TCTRL3;
            break;
        case _PIT_TIME3_TFLG3:
            *(uint32_t *)buf = dev->regs.PIT_TIME3_TFLG3;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in mpc5554_pit\n", offset);
            break;
    }

    //printf("in func:%s, offset:0x%x, val:0x%x\n", __func__, offset, *(uint32_t *)buf);

    return No_exp;
}

// 寄存器写入操作方法
static exception_t mpc5554_pit_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc5554_pit_device *dev = (mpc5554_pit_device *)conf_obj->obj;
    uint32_t val = *(uint32_t *)buf;
    uint32_t pit_timer_0_period;

    //printf("in func:%s, offset:0x%x, val:0x%x\n", __func__, offset, val);

    switch (offset)
    {
        case _PITMCR:
            dev->regs.PITMCR = val;

            if(!(dev->regs.PITMCR & PIT_MCR_MDIS_MASK))
            {
                dev->pit_module_enable = 1;
                if ( dev->pit_timer0_handle != 0) 
                {
                    system_enable_timer(dev->pit_timer0_handle);
                }
            } else
            {
                dev->pit_module_enable = 0;
                if ( dev->pit_timer0_handle != 0) 
                {
                    system_disable_timer(dev->pit_timer0_handle);
                }
            }
            break;
        case _PIT_RTI_LDVAL:
            dev->regs.PIT_RTI_LDVAL = val;
            break;
        case _PIT_RTI_CVAL:
            dev->regs.PIT_RTI_CVAL = val;
            break;
        case _PIT_RTI_TCTRL:
            dev->regs.PIT_RTI_TCTRL = val;

            break;
        case _PIT_RTI_TFLG:
            dev->regs.PIT_RTI_TFLG = val;
            break;
        case _PIT_TIME0_LDVAL0:
            dev->regs.PIT_TIME0_LDVAL0 = val;
            break;
        case _PIT_TIME0_CVAL0:
            dev->regs.PIT_TIME0_CVAL0 = val;
            break;
        case _PIT_TIME0_TCTRL0:
            dev->regs.PIT_TIME0_TCTRL0 = val;

            if(dev->regs.PIT_TIME0_TCTRL0 & PIT_TCTRL_TEN_MASK)
            {
                if(dev->pit_module_enable == 1)
                {
                    if(dev->regs.PIT_TIME0_TCTRL0 & PIT_TCTRL_TIE_MASK)
                    {
                        pit_timer_0_period = (dev->regs.PIT_TIME0_LDVAL0 + 1) / BUS_CLK_FREQ_MHZ;
                        //printf("in func:%s, pit_timer_0_period: %d\n",__func__, pit_timer_0_period);
                        dev->pit_timer0_handle = system_register_timer_handler(conf_obj, (uint32_t)(pit_timer_0_period), (time_sched_t)pit_timer0_interrupt_handler, SCHED_US|SCHED_NORMAL);
                    }
                }
            } else
            {
                if ( dev->pit_timer0_handle != 0) 
                {
                    system_disable_timer(dev->pit_timer0_handle);
                }
            }

            break;
        case _PIT_TIME0_TFLG0:
            dev->regs.PIT_TIME0_TFLG0 = val;
            break;
        case _PIT_TIME1_LDVAL1:
            dev->regs.PIT_TIME1_LDVAL1 = val;
            break;
        case _PIT_TIME1_CVAL1:
            dev->regs.PIT_TIME1_CVAL1 = val;
            break;
        case _PIT_TIME1_TCTRL1:
            dev->regs.PIT_TIME1_TCTRL1 = val;
            break;
        case _PIT_TIME1_TFLG1:
            dev->regs.PIT_TIME1_TFLG1 = val;
            break;
        case _PIT_TIME2_LDVAL2:
            dev->regs.PIT_TIME2_LDVAL2 = val;
            break;
        case _PIT_TIME2_CVAL2:
            dev->regs.PIT_TIME2_CVAL2 = val;
            break;
        case _PIT_TIME2_TCTRL2:
            dev->regs.PIT_TIME2_TCTRL2 = val;
            break;
        case _PIT_TIME2_TFLG2:
            dev->regs.PIT_TIME2_TFLG2 = val;
            break;
        case _PIT_TIME3_LDVAL3:
            dev->regs.PIT_TIME3_LDVAL3 = val;
            break;
        case _PIT_TIME3_CVAL3:
            dev->regs.PIT_TIME3_CVAL3 = val;
            break;
        case _PIT_TIME3_TCTRL3:
            dev->regs.PIT_TIME3_TCTRL3 = val;
            break;
        case _PIT_TIME3_TFLG3:
            dev->regs.PIT_TIME3_TFLG3 = val;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in mpc5554_pit\n", offset);
            break;
    }

    return No_exp;
}

static exception_t timer_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc5554_pit_device* dev = (mpc5554_pit_device*)(obj->obj);
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
    mpc5554_pit_device* dev = (mpc5554_pit_device*)(obj->obj);
    *obj2 = dev->signal;
    *port = NULL;
    return No_exp;
}


// 寄存器接口:根据寄存器ID获取寄存器名
static char* mpc5554_pit_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_name[id];
}

// 寄存器接口:根据寄存器ID设置寄存器值
static exception_t mpc5554_pit_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    mpc5554_pit_device *dev = (mpc5554_pit_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    *((uint32_t*)regs + id) = value;

    return No_exp;
}

// 寄存器接口:获取设备有多少寄存器
static uint32_t mpc5554_pit_get_regnum(conf_object_t *conf_obj)
{
    //return sizeof(struct registers) / 4;
    return 0;
}

// 寄存器接口:根据寄存器ID获取寄存器值
static uint32_t mpc5554_pit_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    mpc5554_pit_device *dev = (mpc5554_pit_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    uint32_t* regs_value = (uint32_t*)regs + id;

    return *regs_value;
}

// 寄存器接口:根据寄存器名获取寄存器ID
static uint32_t mpc5554_pit_get_regid_by_name(conf_object_t *conf_obj, char *name)
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
static uint32_t mpc5554_pit_get_regoffset_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_offset[id];
}

// 实例化一个新设备对象，obj_name是在json文件中配置的名称
static conf_object_t* new_mpc5554_pit(const char *obj_name)
{
    mpc5554_pit_device* dev = skyeye_mm_zero(sizeof(mpc5554_pit_device));
    dev->obj = new_conf_object(obj_name, dev);
    // TODO: 在此添加自定义源码

    // 寄存器初始化
    struct registers *regs = &(dev->regs);
    regs->PITMCR = 0;
    regs->PIT_RTI_LDVAL = 0;
    regs->PIT_RTI_CVAL = 0;
    regs->PIT_RTI_TCTRL = 0;
    regs->PIT_RTI_TFLG = 0;
    regs->PIT_TIME0_LDVAL0 = 0;
    regs->PIT_TIME0_CVAL0 = 0;
    regs->PIT_TIME0_TCTRL0 = 0;
    regs->PIT_TIME0_TFLG0 = 0;
    regs->PIT_TIME1_LDVAL1 = 0;
    regs->PIT_TIME1_CVAL1 = 0;
    regs->PIT_TIME1_TCTRL1 = 0;
    regs->PIT_TIME1_TFLG1 = 0;
    regs->PIT_TIME2_LDVAL2 = 0;
    regs->PIT_TIME2_CVAL2 = 0;
    regs->PIT_TIME2_TCTRL2 = 0;
    regs->PIT_TIME2_TFLG2 = 0;
    regs->PIT_TIME3_LDVAL3 = 0;
    regs->PIT_TIME3_CVAL3 = 0;
    regs->PIT_TIME3_TCTRL3 = 0;
    regs->PIT_TIME3_TFLG3 = 0;

    return dev->obj;
}

// 配置实例化的设备对象
static exception_t config_mpc5554_pit(conf_object_t* conf_obj)
{
    mpc5554_pit_device *dev = (mpc5554_pit_device *)conf_obj->obj;
    // TODO: 在此添加自定义源码


    return No_exp;
}

// 释放设备对象来做一些内存释放
static exception_t free_mpc5554_pit(conf_object_t* conf_obj)
{
    mpc5554_pit_device *dev = (mpc5554_pit_device *)conf_obj->obj;

    skyeye_free(conf_obj->objname);
    skyeye_free(conf_obj->obj);
    skyeye_free(conf_obj);

    return No_exp;
}

// 初始化模块类，并在SkyEye加载模块时执行以下代码
void init_mpc5554_pit(void)
{
    // 向SkyEye注册一个设备
    static skyeye_class_t class_data =
    {
        .class_name      = "mpc5554_pit",
        .class_desc      = "mpc5554_pit",
        .new_instance    = new_mpc5554_pit,
        .free_instance   = free_mpc5554_pit,
        .config_instance = config_mpc5554_pit,
        .module_type     = SKYML_OFFCHIP,
    };
    conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);

    // 设置寄存器地址读写方法
    static memory_space_intf io_memory_interface =
    {
        .read  = mpc5554_pit_read,
        .write = mpc5554_pit_write
    };
    SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

    // 注册访问接口(用于SkyEye图形控制接口)
    static skyeye_reg_intf reg_interface =
    {
        .get_regvalue_by_id  = mpc5554_pit_get_regvalue_by_id,
        .get_regname_by_id   = mpc5554_pit_get_regname_by_id,
        .set_regvalue_by_id  = mpc5554_pit_set_regvalue_by_id,
        .get_regnum          = mpc5554_pit_get_regnum,
        .get_regid_by_name   = mpc5554_pit_get_regid_by_name,
        .get_regoffset_by_id = mpc5554_pit_get_regoffset_by_id
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
