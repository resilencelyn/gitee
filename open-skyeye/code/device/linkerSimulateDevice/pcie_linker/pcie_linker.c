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
#include "pcie_linker.h"



// 中断信号:上升沿
static int pcie_linker_raise_signal(conf_object_t *conf_obj, int line)
{
    pcie_linker_device *dev = (pcie_linker_device *)conf_obj->obj;

    return No_exp;
}

// 中断信号:下降沿
static int pcie_linker_lower_signal(conf_object_t *conf_obj, int line)
{
    pcie_linker_device *dev = (pcie_linker_device *)conf_obj->obj;

    return No_exp;
}

// 中断信号:事件
static int pcie_linker_event_signal(conf_object_t *conf_obj, int line, void *args)
{
    pcie_linker_device *dev = (pcie_linker_device *)conf_obj->obj;

    return No_exp;
}

int pcie_linker_write_data(conf_object_t *conf_obj, int dev_index, int bar_index, generic_address_t offset, void *buf, size_t count)
{
    pcie_linker_device *dev = (pcie_linker_device *)conf_obj->obj;

    if ((dev_index > MAX_PCIE_DEV_NUM) || (bar_index > MAX_BAR_NUM))
    {
        fprintf(stderr, "[%s] : [%d] : dev_index = %d, bar_index = %d!\n", __func__, __LINE__, dev_index, bar_index);
        fflush(stderr);
        return 0;
    }

    if (dev->memory_space[dev_index].iface[bar_index] != NULL)
    {
        dev->memory_space[dev_index].iface[bar_index]->write(dev->memory_space[dev_index].obj, offset, buf, count);
    }

    return 0;
}

int pcie_linker_read_data(conf_object_t *conf_obj, int dev_index, int bar_index, generic_address_t offset, void *buf, size_t count)
{
    pcie_linker_device *dev = (pcie_linker_device *)conf_obj->obj;

    if ((dev_index > MAX_PCIE_DEV_NUM) || (bar_index > MAX_BAR_NUM))
    {
        fprintf(stderr, "[%s] : [%d] : dev_index = %d, bar_index = %d!\n", __func__, __LINE__, dev_index, bar_index);
        fflush(stderr);
        return 0;
    }

    if (dev->memory_space[dev_index].iface[bar_index] != NULL)
    {
        dev->memory_space[dev_index].iface[bar_index]->read(dev->memory_space[dev_index].obj, offset, buf, count);
    }

    return 0;
}

int pcie_linker_get_info(conf_object_t *conf_obj, pcie_msg_t * pcie_info_buff,int dev_cnt)
{
    pcie_linker_device *dev = (pcie_linker_device *)conf_obj->obj;

    if ((pcie_info_buff == NULL) || (dev_cnt > MAX_PCIE_DEV_NUM))
    {
        fprintf(stderr, "[%s] : [%d] : pcie_info_buff = 0x%x,dev_cnt = %d\n", __FUNCTION__, __LINE__, pcie_info_buff, dev_cnt);
        fflush(stderr);
        return 0;
    }

    memcpy(pcie_info_buff, &(dev->pcie_dev_info[0]), sizeof(pcie_msg_t) * dev_cnt);
    return 0;
}

// 实例化一个新设备对象，obj_name是在json文件中配置的名称
static conf_object_t* new_pcie_linker(const char *obj_name)
{
    pcie_linker_device* dev = skyeye_mm_zero(sizeof(pcie_linker_device));
    dev->obj = new_conf_object(obj_name, dev);
    // TODO: 在此添加自定义源码
    int i;
    // 寄存器初始化
    struct registers *regs = &(dev->regs);
    // 属性初始化
    for (i = 0; i < MAX_PCIE_DEV_NUM;i++)
    {
        dev->pcie_dev_info[i].ep_config_space.vendor_id = 0xFFFF;
        dev->pcie_dev_info[i].ep_config_space.latency_timer = 0;
        dev->pcie_dev_info[i].ep_config_space.header_type = 0;
    }
    return dev->obj;
}

// 配置实例化的设备对象
static exception_t config_pcie_linker(conf_object_t* conf_obj)
{
    pcie_linker_device *dev = (pcie_linker_device *)conf_obj->obj;
    // TODO: 在此添加自定义源码


    return No_exp;
}

// 释放设备对象来做一些内存释放
static exception_t free_pcie_linker(conf_object_t* conf_obj)
{
    pcie_linker_device *dev = (pcie_linker_device *)conf_obj->obj;

    skyeye_free(conf_obj->objname);
    skyeye_free(conf_obj->obj);
    skyeye_free(conf_obj);

    return No_exp;
}

// 系统设置设备 memory_space 接口功能
static exception_t memory_space_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    pcie_linker_device *dev = (pcie_linker_device *)obj->obj;

    dev->memory_space[index].obj = obj2;
    if (obj2 == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from NULL object fail.\n", MEMORY_SPACE_INTF_NAME);
        return Not_found_exp;
    }
    dev->memory_space[index].iface[0] = (memory_space_iface_t *)SKY_get_iface(obj2, MEMORY_SPACE_INTF_NAME);

    if (dev->memory_space[index].iface[0] == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from %s fail.\n", MEMORY_SPACE_INTF_NAME, obj2->objname);
        return Not_found_exp;
    }

    return No_exp;
}

// 系统获取设备 memory_space 接口功能
static exception_t memory_space_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    pcie_linker_device *dev = (pcie_linker_device *)obj->obj;

    *obj2 = dev->memory_space[index].obj;
    *port = NULL;

    return No_exp;
}

// 系统设置设备 pcie_config 接口功能
static exception_t pcie_config_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    pcie_linker_device *dev = (pcie_linker_device *)obj->obj;

    //dev->pcie_dev_info[index].obj = obj2;
    if (obj2 == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from NULL object fail.\n", MEMORY_SPACE_INTF_NAME);
        return Not_found_exp;
    }
    dev->pcie_ep_iface[index] = (pcie_config_intf *)SKY_get_iface(obj2, PCIE_CONFIG_INTF);
    if (dev->pcie_ep_iface[index] == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from %s fail.\n", PCIE_CONFIG_INTF, obj2->objname);
        return Not_found_exp;
    }
    dev->pcie_ep_iface[index]->get_config_info(obj2,&dev->pcie_dev_info[index],1);
    return No_exp;
}

// 系统获取设备 memory_space 接口功能
static exception_t pcie_config_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    pcie_linker_device *dev = (pcie_linker_device *)obj->obj;

    *obj2 = dev->pcie_dev_info[index].pcie_ep_obj;
    *port = NULL;

    return No_exp;
}

// 注册设备模块属性
static void class_register_attribute(conf_class_t* dev_class)
{

    return;
}

// 初始化模块类，并在SkyEye加载模块时执行以下代码
void init_pcie_linker(void)
{
    // 向SkyEye注册一个设备
    static skyeye_class_t class_data =
    {
        .class_name      = "pcie_linker",
        .class_desc      = "pcie_linker",
        .new_instance    = new_pcie_linker,
        .free_instance   = free_pcie_linker,
        .config_instance = config_pcie_linker,
        .module_type     = SKYML_OFFCHIP,
    };
    conf_class_t* dev_class = SKY_register_linker_class(class_data.class_name, &class_data);

    // 中断管理器中断信号接口
    static const general_signal_iface_t general_signal_interface =
    {
        .raise_signal = pcie_linker_raise_signal,
        .lower_signal = pcie_linker_lower_signal,
        .event_signal = pcie_linker_event_signal
    };
    SKY_register_iface(dev_class, GENERAL_SIGNAL_INTF_NAME, &general_signal_interface);

    static const pcie_bus_intf pcie_bus_interface =
    {
        .write_pcie_data = pcie_linker_write_data,
        .read_pcie_data = pcie_linker_read_data
    };
    SKY_register_iface(dev_class, PCIE_BUS_INTF, &pcie_bus_interface);

    static const pcie_config_intf pcie_config_interface =
    {
        .get_config_info = pcie_linker_get_info
    };
    SKY_register_iface(dev_class, PCIE_CONFIG_INTF, &pcie_config_interface);

    // 接口方法列表
    static const struct InterfaceDescription ifaces[] =
    {
        (struct InterfaceDescription)
        {
            .name  = PCIE_BUS_INTF,
            .iface = &pcie_bus_interface
        },
        (struct InterfaceDescription)
        {
            .name  = PCIE_CONFIG_INTF,
            .iface = &pcie_config_interface
        },

    };

    // 接口连接方法列表
    static const struct ConnectDescription connects[] =
    {
        (struct ConnectDescription)
        {
            .name = MEMORY_SPACE_INTF_NAME,
            .set  = memory_space_set,
            .get  = memory_space_get
        },
        (struct ConnectDescription)
        {
            .name = PCIE_CONFIG_INTF,
            .set  = pcie_config_set,
            .get  = pcie_config_get
        },
    };
    class_register_ifaces(dev_class, ifaces);
    class_register_connects(dev_class, connects);
    class_register_attribute(dev_class);
}
