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
#include "mpc8247_scc.h"

static void doInterrupt(conf_object_t *conf_obj)
{
    mpc8247_scc_device *dev = (mpc8247_scc_device *)conf_obj->obj;

    if (dev->general_signal.iface)
    {
        dev->general_signal.iface->raise_signal(dev->obj, dev->int_number);
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "General_singal.iface not exist\n");
    }
}

// 寄存器读取操作方法
static exception_t mpc8247_scc_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc8247_scc_device *dev = (mpc8247_scc_device *)conf_obj->obj;

    switch (offset)
    {
        case _GSMR_L:
            *(uint32_t *)buf = dev->regs.GSMR_L.all;
            break;
        case _GSMR_H:
            *(uint32_t *)buf = dev->regs.GSMR_H.all;
            break;
        case _PSMR:
            *(uint16_t *)buf = dev->regs.PSMR;
            break;
        case _TODR:
            *(uint16_t *)buf = dev->regs.TODR.all;
            break;
        case _DSR:
            *(uint16_t *)buf = dev->regs.DSR;
            break;
        case _SCCE:
            *(uint16_t *)buf = dev->regs.SCCE;
            break;
        case _SCCM:
            *(uint16_t *)buf = dev->regs.SCCM;
            break;
        case _SCCS:
            *(uint8_t *)buf = dev->regs.SCCS;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in mpc8247_scc\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器写入操作方法
static exception_t mpc8247_scc_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc8247_scc_device *dev = (mpc8247_scc_device *)conf_obj->obj;
    uint32_t val = *(uint32_t *)buf;
    uint16_t val16 = *(uint16_t *)buf;
    uint8_t val8 = *(uint8_t *)buf;

    switch (offset)
    {
        case _GSMR_L:
            dev->regs.GSMR_L.all = val;
            break;
        case _GSMR_H:
            dev->regs.GSMR_H.all = val;
            break;
        case _PSMR:
            dev->regs.PSMR = val16;
            break;
        case _TODR:
            dev->regs.TODR.all = val16;
            break;
        case _DSR:
            dev->regs.DSR = val16;
            break;
        case _SCCE:
            dev->regs.SCCE = val16;
            break;
        case _SCCM:
            dev->regs.SCCM = val16;
            break;
        case _SCCS:
            dev->regs.SCCS = val8;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in mpc8247_scc\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器接口:根据寄存器ID获取寄存器名
static char* mpc8247_scc_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_name[id];
}

// 寄存器接口:根据寄存器ID设置寄存器值
static exception_t mpc8247_scc_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    mpc8247_scc_device *dev = (mpc8247_scc_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    *((uint32_t*)regs + id) = value;

    return No_exp;
}

// 寄存器接口:获取设备有多少寄存器
static uint32_t mpc8247_scc_get_regnum(conf_object_t *conf_obj)
{
    return 8;
}

// 寄存器接口:根据寄存器ID获取寄存器值
static uint32_t mpc8247_scc_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    mpc8247_scc_device *dev = (mpc8247_scc_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    uint32_t* regs_value = (uint32_t*)regs + id;

    return *regs_value;
}

// 寄存器接口:根据寄存器名获取寄存器ID
static uint32_t mpc8247_scc_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
    uint32_t regnum = 8;
    int i;

    for (i = 0; i < regnum; i++)
    {
        if (strcmp(name, regs_name[i]) == 0)
            return i;
    }

    return regnum;
}

// 寄存器接口:根据寄存器ID获取寄存器偏移
static uint32_t mpc8247_scc_get_regoffset_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_offset[id];
}

// 串口发送一个字节
static exception_t mpc8247_scc_serial_write(conf_object_t *conf_obj, uint32_t value)
{
    mpc8247_scc_device *dev = (mpc8247_scc_device *)conf_obj->obj;

    return No_exp;
}

// 确定是否可以接收数据
static exception_t mpc8247_scc_receive_ready(conf_object_t *conf_obj)
{
    mpc8247_scc_device *dev = (mpc8247_scc_device *)conf_obj->obj;

    return No_exp;
}

// 连续发送一组数据
static uint32_t mpc8247_scc_serial_write_chars(conf_object_t *conf_obj, char *buf, uint32_t cnt)
{
    mpc8247_scc_device *dev = (mpc8247_scc_device *)conf_obj->obj;

    return No_exp;
}

// 实例化一个新设备对象，obj_name是在json文件中配置的名称
static conf_object_t* new_mpc8247_scc(const char *obj_name)
{
    mpc8247_scc_device* dev = skyeye_mm_zero(sizeof(mpc8247_scc_device));
    dev->obj = new_conf_object(obj_name, dev);
    // TODO: 在此添加自定义源码

    // 寄存器初始化
    struct registers *regs = &(dev->regs);
    regs->GSMR_L.all = 0;
    regs->GSMR_H.all = 0;
    regs->PSMR = 0;
    regs->TODR.all = 0;
    regs->DSR = 0;
    regs->SCCE = 0;
    regs->SCCM = 0;
    regs->SCCS = 0;
    // 属性初始化
    dev->int_number = 0;
    dev->reverse = 0;

    return dev->obj;
}

// 配置实例化的设备对象
static exception_t config_mpc8247_scc(conf_object_t* conf_obj)
{
    mpc8247_scc_device *dev = (mpc8247_scc_device *)conf_obj->obj;
    // TODO: 在此添加自定义源码

    return No_exp;
}

// 释放设备对象来做一些内存释放
static exception_t free_mpc8247_scc(conf_object_t* conf_obj)
{
    mpc8247_scc_device *dev = (mpc8247_scc_device *)conf_obj->obj;

    skyeye_free(conf_obj->objname);
    skyeye_free(conf_obj->obj);
    skyeye_free(conf_obj);

    return No_exp;
}

// 系统设置设备 general_signal 接口功能
static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc8247_scc_device *dev = (mpc8247_scc_device *)obj->obj;

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
    mpc8247_scc_device *dev = (mpc8247_scc_device *)obj->obj;

    *obj2 = dev->general_signal.obj;
    *port = NULL;

    return No_exp;
}

// 系统设置设备 skyeye_uart 接口功能
static exception_t skyeye_uart_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc8247_scc_device *dev = (mpc8247_scc_device *)obj->obj;

    dev->skyeye_uart.obj = obj2;
    if (obj2 == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from NULL object fail.\n", SKYEYE_UART_INTF);
        return Not_found_exp;
    }
    dev->skyeye_uart.iface = (skyeye_uart_intf *)SKY_get_iface(obj2, SKYEYE_UART_INTF);
    if (dev->skyeye_uart.iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from %s fail.\n", SKYEYE_UART_INTF, obj2->objname);
        return Not_found_exp;
    }

    return No_exp;
}

// 系统获取设备 skyeye_uart 接口功能
static exception_t skyeye_uart_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    mpc8247_scc_device *dev = (mpc8247_scc_device *)obj->obj;

    *obj2 = dev->skyeye_uart.obj;
    *port = NULL;

    return No_exp;
}

// 系统设置设备 uart_file 接口功能
static exception_t uart_file_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc8247_scc_device *dev = (mpc8247_scc_device *)obj->obj;

    dev->uart_file.obj = obj2;
    if (obj2 == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from NULL object fail.\n", UART_FILE_INTF);
        return Not_found_exp;
    }
    dev->uart_file.iface = (uart_file_intf *)SKY_get_iface(obj2, UART_FILE_INTF);
    if (dev->uart_file.iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from %s fail.\n", UART_FILE_INTF, obj2->objname);
        return Not_found_exp;
    }

    return No_exp;
}

// 系统获取设备 uart_file 接口功能
static exception_t uart_file_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    mpc8247_scc_device *dev = (mpc8247_scc_device *)obj->obj;

    *obj2 = dev->uart_file.obj;
    *port = NULL;

    return No_exp;
}

// 系统设置设备 uart_com 接口功能
static exception_t uart_com_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc8247_scc_device *dev = (mpc8247_scc_device *)obj->obj;

    dev->uart_com.obj = obj2;
    if (obj2 == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from NULL object fail.\n", UART_COM_INTF);
        return Not_found_exp;
    }
    dev->uart_com.iface = (uart_com_intf *)SKY_get_iface(obj2, UART_COM_INTF);
    if (dev->uart_com.iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from %s fail.\n", UART_COM_INTF, obj2->objname);
        return Not_found_exp;
    }

    return No_exp;
}

// 系统获取设备 uart_com 接口功能
static exception_t uart_com_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    mpc8247_scc_device *dev = (mpc8247_scc_device *)obj->obj;

    *obj2 = dev->uart_com.obj;
    *port = NULL;

    return No_exp;
}

// 系统设置设备 int_number 属性
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    mpc8247_scc_device *dev = (mpc8247_scc_device *)conf_obj->obj;

    dev->int_number = SKY_attr_uinteger(*value);

    return No_exp;
}

// 系统获取设备 int_number 属性
static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    mpc8247_scc_device *dev = (mpc8247_scc_device *)conf_obj->obj;

    attr_value_t value = SKY_make_attr_uinteger(dev->int_number);

    return value;
}

// 系统设置设备 reverse 属性
static exception_t set_attr_reverse(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    mpc8247_scc_device *dev = (mpc8247_scc_device *)conf_obj->obj;

    dev->reverse = SKY_attr_uinteger(*value);

    return No_exp;
}

// 系统获取设备 reverse 属性
static attr_value_t get_attr_reverse(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    mpc8247_scc_device *dev = (mpc8247_scc_device *)conf_obj->obj;

    attr_value_t value = SKY_make_attr_uinteger(dev->reverse);

    return value;
}

// 注册设备模块属性
static void class_register_attribute(conf_class_t* dev_class)
{
    SKY_register_attribute(dev_class,
            "int_number",
            get_attr_int_number, NULL,
            set_attr_int_number, NULL,
            SKY_Attr_Optional,
            "uinteger",
            "this is int_number describe");
    SKY_register_attribute(dev_class,
            "reverse",
            get_attr_reverse, NULL,
            set_attr_reverse, NULL,
            SKY_Attr_Optional,
            "uinteger",
            "this is reverse describe");

    return;
}

// 初始化模块类，并在SkyEye加载模块时执行以下代码
void init_mpc8247_scc(void)
{
    // 向SkyEye注册一个设备
    static skyeye_class_t class_data =
    {
        .class_name      = "mpc8247_scc",
        .class_desc      = "mpc8247_scc",
        .new_instance    = new_mpc8247_scc,
        .free_instance   = free_mpc8247_scc,
        .config_instance = config_mpc8247_scc,
        .module_type     = SKYML_OFFCHIP,
    };
    conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);

    // 设置寄存器地址读写方法
    static memory_space_intf io_memory_interface =
    {
        .read  = mpc8247_scc_read,
        .write = mpc8247_scc_write
    };
    SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

    // 注册访问接口(用于SkyEye图形控制接口)
    static skyeye_reg_intf reg_interface =
    {
        .get_regvalue_by_id  = mpc8247_scc_get_regvalue_by_id,
        .get_regname_by_id   = mpc8247_scc_get_regname_by_id,
        .set_regvalue_by_id  = mpc8247_scc_set_regvalue_by_id,
        .get_regnum          = mpc8247_scc_get_regnum,
        .get_regid_by_name   = mpc8247_scc_get_regid_by_name,
        .get_regoffset_by_id = mpc8247_scc_get_regoffset_by_id
    };
    SKY_register_iface(dev_class, SKYEYE_REG_INTF, &reg_interface);

    // 串行设备设备单向传输接口
    static const skyeye_serial_device_t skyeye_serial_interface =
    {
        .write         = mpc8247_scc_serial_write,
        .write_chars   = mpc8247_scc_serial_write_chars,
        .receive_ready = mpc8247_scc_receive_ready
    };
    SKY_register_iface(dev_class, SERIAL_DEVICE_INTERFACE, &skyeye_serial_interface);

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
            .name  = SERIAL_DEVICE_INTERFACE,
            .iface = &skyeye_serial_interface
        },
    };

    // 接口连接方法列表
    static const struct ConnectDescription connects[] =
    {
        (struct ConnectDescription)
        {
            .name = GENERAL_SIGNAL_INTF_NAME,
            .set  = general_signal_set,
            .get  = general_signal_get
        },
        (struct ConnectDescription)
        {
            .name = SKYEYE_UART_INTF,
            .set  = skyeye_uart_set,
            .get  = skyeye_uart_get
        },
        (struct ConnectDescription)
        {
            .name = UART_FILE_INTF,
            .set  = uart_file_set,
            .get  = uart_file_get
        },
        (struct ConnectDescription)
        {
            .name = UART_COM_INTF,
            .set  = uart_com_set,
            .get  = uart_com_get
        },
    };
    class_register_ifaces(dev_class, ifaces);
    class_register_connects(dev_class, connects);
    class_register_attribute(dev_class);
}
