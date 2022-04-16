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
#include "zynq7000_cptimer.h"

void cptimer_global_timer(conf_object_t *opaque)
{
    zynq7000_cptimer_device *dev = (zynq7000_cptimer_device *)opaque->obj;
    uint64_t tmp = 0;
    struct registers *regs = &dev->regs;

    if(regs->GTCONR.bits.TE == 0)//不计数时,或到达了单次触发模式的计数值。
    {
        system_disable_timer(dev->gthandle);
        return;
    }

//    dev->count ++;

    if(regs->GTCONR.bits.CE)//compare with Comparator Register
    {
        tmp = (uint64_t)regs->CVR0;
        tmp |= ((uint64_t)regs->CVR1 << 32);
        dev->count = tmp;

//      if (dev->count == tmp)//达到比较器中的计数值时
//      {
            regs->GTISR = 1;//置标志位
            if(regs->GTCONR.bits.IE)//允许中断时，产生中断
            {
                regs->GTISR = 1;
                if (dev->signal_iface)
                    dev->signal_iface->raise_signal(dev->signal, dev->global_timer_interrupt);
            }
            if(regs->GTCONR.bits.bit3)//触发方式为自动递增
            {
                tmp += regs->AIR;
                regs->CVR0 = tmp;
                regs->CVR1 = tmp >> 32;
            }
            else//单次触发
            {
                system_disable_timer(dev->gthandle);
            }
//      }
    } 
}

void cptimer_private_timer(conf_object_t *opaque)
{
    zynq7000_cptimer_device *dev = (zynq7000_cptimer_device *)opaque->obj;
    uint32_t tmp;
    struct registers *regs = &dev->regs;

    if(regs->PTCNR.bits.TE == 0 || (!regs->PTCNR.bits.AR && regs->PTCUR == 0))//不计数时,或Single shot mode 的count到0
    {
        system_disable_timer(dev->pthandle);
        return;
    }

    regs->PTCUR = 0;
    regs->PTISR.bits.bit0 = 1;

    if(regs->PTCNR.bits.IE)//允许中断时，产生中断
    {
        if (dev->signal_iface)
            dev->signal_iface->raise_signal(dev->signal, dev->cpu_private_timer_interrupt);
    }

    if(regs->PTCNR.bits.AR)//自动装载模式
        regs->PTCUR = regs->PTLR;
}

void cptimer_watchdog(conf_object_t *opaque)
{
    zynq7000_cptimer_device *dev = (zynq7000_cptimer_device *)opaque->obj;
    uint32_t tmp;
    struct registers *regs = &dev->regs;

    if(regs->WCNR.bits.IE == 0 || (!regs->WCNR.bits.AR && regs->WCUR == 0))//不计数时,或Single shot mode 的count到0
    {
        system_disable_timer(dev->wthandle);
        return;
    }

    regs->WCUR --;

    if(regs->WCUR == 0)
    {
        regs->WISR = 1;
        regs->WRSR = 1;
        if(regs->WCNR.bits.IE)//允许中断时，产生中断
        {
            if (dev->signal_iface)
                dev->signal_iface->raise_signal(dev->signal, dev->watchdog_timer_interrupt);
        }
        if(regs->WCNR.bits.AR)//自动装载模式
            regs->WCUR = regs->WLR;
    }
}

// 寄存器读取操作方法
static exception_t zynq7000_cptimer_read0(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    zynq7000_cptimer_device *dev = (zynq7000_cptimer_device *)conf_obj->obj;
    struct registers *regs = &dev->regs;
    switch (offset)
    {
        case _GTCONR:
            *(uint32_t *)buf = dev->regs.GTCONR.all;
        break;
        case _GTISR:
            *(uint32_t *)buf = dev->regs.GTISR;
        break;
        case _GTCOUR0:
            *(uint32_t *)buf = (uint32_t)dev->count;
        break;
        case _GTCOUR1:
            *(uint32_t *)buf = (uint32_t)(dev->count >> 32);
        break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in zynq7000_cptimer\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器写入操作方法
static exception_t zynq7000_cptimer_write0(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    zynq7000_cptimer_device *dev = (zynq7000_cptimer_device *)conf_obj->obj;
    uint32_t val = *(uint32_t *)buf;
    struct registers *regs = &dev->regs;
    uint64_t tmp = 0;
    switch (offset)
    {
        case _GTCOUR0:
            regs->GTCOUR0 = val;
            dev->count |= regs->GTCOUR0;
            break;
        case _GTCOUR1:
            regs->GTCOUR1 = val;
            dev->count |= ((uint64_t)regs->GTCOUR1 << 32);
            break;
        case _GTCONR:
            regs->GTCONR.all = val;
            if(regs->GTCONR.bits.TE)//启动定时器
            {
                dev->count = (uint64_t)dev->regs.GTCOUR0;
                dev->count |= ((uint64_t)dev->regs.GTCOUR1 << 32);
                double period_timer = 1000 * dev->count / DEFAULT_CLK;
                if (dev->gthandle == -1)
                {
                    printf("enable cptimer_global_timer, but have not test and finish\n");

                    if(period_timer > 1.0)
                        dev->gthandle = system_register_timer_handler(conf_obj, (uint32_t)period_timer, (time_sched_t)cptimer_global_timer, SCHED_MS|SCHED_NORMAL);
                    else if((period_timer * 1000) > 1.0)
                        dev->gthandle = system_register_timer_handler(conf_obj, (uint32_t)period_timer * 1000, (time_sched_t)cptimer_global_timer, SCHED_US|SCHED_NORMAL);
                    else
                        dev->gthandle = system_register_timer_handler(conf_obj, (uint32_t)period_timer * 1000000, (time_sched_t)cptimer_global_timer, SCHED_NS|SCHED_NORMAL);
                }
                dev->regs.PTCUR = dev->regs.PTLR;//global timer在启动timer时充填定时值
                system_enable_timer(dev->gthandle);
            }
            else if(dev->gthandle != -1)
                system_disable_timer(dev->gthandle);
            break;
        case _GTISR:
            if(val > 0)//写1清0
                regs->GTISR = 0;
            break;
        case _CVR0:
            regs->CVR0 = val;
            break;
        case _CVR1:
            regs->CVR1 = val;
            break;
        case _AIR:
            regs->AIR = val;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in zynq7000_cptimer\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器读取操作方法
static exception_t zynq7000_cptimer_read1(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    zynq7000_cptimer_device *dev = (zynq7000_cptimer_device *)conf_obj->obj;
    struct registers *regs = &dev->regs;
    switch (offset)
    {
        case _PTLR:
            *(uint32_t *)buf = regs->PTLR;
        break;
        case _PTCUR:
            *(uint32_t *)buf = regs->PTCUR;
        break;
        case _PTCNR:
            *(uint32_t *)buf = regs->PTCNR.all;
        break;
        case _PTISR:
            *(uint32_t *)buf = regs->PTISR.all;
        break;
        case _WLR:
            *(uint32_t *)buf = regs->WLR;
        break;
        case _WCUR:
            *(uint32_t *)buf = regs->WCUR;
        break;
        case _WCNR:
            *(uint32_t *)buf = regs->WCNR.all;
        break;
        case _WISR:
            *(uint32_t *)buf = regs->WISR;
        break;
        case _WRSR:
            *(uint32_t *)buf = regs->WRSR;
        break;
        case _WDR:
            *(uint32_t *)buf = regs->WDR;
        break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in zynq7000_cptimer\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器写入操作方法
static exception_t zynq7000_cptimer_write1(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    zynq7000_cptimer_device *dev = (zynq7000_cptimer_device *)conf_obj->obj;
    uint32_t val = *(uint32_t *)buf;
    struct registers *regs = &dev->regs;
    uint64_t tmp = 0;
    switch (offset)
    {
        case _PTLR:
            regs->PTLR = val;
            break;
        case _PTCUR:
            regs->PTCUR = val;
            break;
        case _PTCNR:
            regs->PTCNR.all = val;
            if(regs->PTCNR.bits.TE)//启动定时器
            {
                regs->PTCUR = regs->PTLR;
                double period_timer = 1000 * regs->PTLR / DEFAULT_CLK;

                if (dev->pthandle == -1)
                {
                    dev->pthandle = system_register_timer_handler(conf_obj, (uint32_t)5, (time_sched_t)cptimer_private_timer, SCHED_MS|SCHED_NORMAL);
                }
                system_enable_timer(dev->pthandle);
            }
            else if (dev->pthandle != -1)
                system_disable_timer(dev->pthandle);
            break;
        case _PTISR:
            regs->PTISR.all = copy_bits(regs->PTISR.all, val >> 1, 31, 1);
            if(bit(val, 0))//write 1 to clear
                regs->PTISR.all = clear_bit(regs->PTISR.all, 0);
            break;
        case _WLR:
            regs->WLR = val;
        break;
        case _WCUR:
            regs->WCUR = val;
            break;
        case _WCNR:
            if(!bit(val, 3) && regs->WCNR.bits.WM)
            {
                regs->WCNR.all = val;
                regs->WCNR.bits.WM = 1;
            }
            else
                regs->WCNR.all = val;
            if(regs->WCNR.bits.IE)//启动定时器
            {
                printf("enable cptimer_private_wdog, but have not test and finish\n");

                if (dev->wthandle == -1)
                {
                    double period_timer = (2 * 1000000000) / DEFAULT_CLK;//default is 1/2
                    dev->wthandle = system_register_timer_handler(conf_obj, (uint32_t)/*period_timer*/20, (time_sched_t)cptimer_watchdog, SCHED_US|SCHED_NORMAL);
                }
                
                regs->WCUR = regs->WLR;
                system_enable_timer(dev->wthandle);
            }
            else if(dev->wthandle != -1)
                system_disable_timer(dev->wthandle);
            break;
        case _WISR:
            if(bit(val, 0))//write 1 to clear
                regs->WISR = clear_bit(regs->WISR, 0);
        break;
        case _WRSR:
            if(bit(val, 0))//write 1 to clear
                regs->WRSR = clear_bit(regs->WRSR, 0);
            break;
        case _WDR:
            if(val == 0x12345678)
                dev->wdr_flag = 1;
            else if(val == 0x87654321 && dev->wdr_flag)
            {
                dev->regs.WCNR.bits.WM = 0;
            }
            else
                dev->wdr_flag = 0;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in zynq7000_cptimer\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器接口:根据寄存器ID获取寄存器名
static char* zynq7000_cptimer_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_name[id];
}

// 寄存器接口:根据寄存器ID设置寄存器值
static exception_t zynq7000_cptimer_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    zynq7000_cptimer_device *dev = (zynq7000_cptimer_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    *((uint32_t*)regs + id) = value;

    return No_exp;
}

// 寄存器接口:获取设备有多少寄存器
static uint32_t zynq7000_cptimer_get_regnum(conf_object_t *conf_obj)
{
    return sizeof(struct registers) / 4;
}

// 寄存器接口:根据寄存器ID获取寄存器值
static uint32_t zynq7000_cptimer_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    zynq7000_cptimer_device *dev = (zynq7000_cptimer_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    uint32_t* regs_value = (uint32_t*)regs + id;

    return *regs_value;
}

// 寄存器接口:根据寄存器名获取寄存器ID
static uint32_t zynq7000_cptimer_get_regid_by_name(conf_object_t *conf_obj, char *name)
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
static uint32_t zynq7000_cptimer_get_regoffset_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_offset[id];
}

// 实例化一个新设备对象，obj_name是在json文件中配置的名称
static conf_object_t* new_zynq7000_cptimer(const char *obj_name)
{
    zynq7000_cptimer_device* dev = skyeye_mm_zero(sizeof(zynq7000_cptimer_device));
    dev->obj = new_conf_object(obj_name, dev);
    // TODO: 在此添加自定义源码

    // 寄存器初始化
    struct registers *regs = &(dev->regs);
    dev->global_timer_interrupt = 27;
    dev->cpu_private_timer_interrupt = 29;
    dev->watchdog_timer_interrupt = 30;
    return dev->obj;
}

// 配置实例化的设备对象
static exception_t config_zynq7000_cptimer(conf_object_t* conf_obj)
{
    zynq7000_cptimer_device *dev = (zynq7000_cptimer_device *)conf_obj->obj;
    // TODO: 在此添加自定义源码

    return No_exp;
}

// 释放设备对象来做一些内存释放
static exception_t free_zynq7000_cptimer(conf_object_t* conf_obj)
{
    zynq7000_cptimer_device *dev = (zynq7000_cptimer_device *)conf_obj->obj;

    skyeye_free(conf_obj->objname);
    skyeye_free(conf_obj->obj);
    skyeye_free(conf_obj);

    return No_exp;
}

static cptimer_register_attribute(conf_class_t* clss)
{
}

static exception_t set_general_signal(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    zynq7000_cptimer_device *dev = obj->obj;
    dev->signal = obj2;
    dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);

    return No_exp;
}

static exception_t get_general_signal(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    zynq7000_cptimer_device *dev = obj->obj;
    *obj2 = dev->signal;
    *port = NULL;
    return No_exp;
}

// 初始化模块类，并在SkyEye加载模块时执行以下代码
void init_zynq7000_cptimer(void)
{
    // 向SkyEye注册一个设备
    static skyeye_class_t class_data =
    {
        .class_name      = "zynq7000_cptimer",
        .class_desc      = "",
        .new_instance    = new_zynq7000_cptimer,
        .free_instance   = free_zynq7000_cptimer,
        .config_instance = config_zynq7000_cptimer,
        .module_type     = SKYML_OFFCHIP,
    };
    conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);

    // 设置寄存器地址读写方法
    static memory_space_intf io_memory_interface[] =
    {
        {
            .read  = zynq7000_cptimer_read0,
            .write = zynq7000_cptimer_write0
        },
        {
            .read  = zynq7000_cptimer_read1,
            .write = zynq7000_cptimer_write1
        }
    };
    SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

    // 注册访问接口(用于SkyEye图形控制接口)
    static skyeye_reg_intf reg_interface =
    {
        .get_regvalue_by_id  = zynq7000_cptimer_get_regvalue_by_id,
        .get_regname_by_id   = zynq7000_cptimer_get_regname_by_id,
        .set_regvalue_by_id  = zynq7000_cptimer_set_regvalue_by_id,
        .get_regnum          = zynq7000_cptimer_get_regnum,
        .get_regid_by_name   = zynq7000_cptimer_get_regid_by_name,
        .get_regoffset_by_id = zynq7000_cptimer_get_regoffset_by_id
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
        }
    };

    // 接口连接方法列表
    static const struct ConnectDescription connects[] =
    {
        (struct ConnectDescription ){
            .name = GENERAL_SIGNAL_INTF_NAME,
            .set = set_general_signal,
            .get = get_general_signal,
        },
    };
    class_register_ifaces(dev_class, ifaces);
    class_register_connects(dev_class, connects);
    cptimer_register_attribute(dev_class);
}
