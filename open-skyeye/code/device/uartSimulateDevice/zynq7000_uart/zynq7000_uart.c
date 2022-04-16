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
#include "zynq7000_uart.h"

static exception_t serial_write(conf_object_t *opaque, uint32_t value)
{
    zynq7000_uart_device *dev = (zynq7000_uart_device *)(opaque->obj);
    struct registers *regs = &(dev->regs);

    unsigned char buf = value;
    if (buf == 127)
        buf = '\b';
    else if(buf == 10 || buf == 13)
    {
        buf = '\r';
    }

    if(bit(regs->XUARTPS_MR_OFFSET, 8))//automatic mode or remote loopback mode
    {
        if(dev->term_iface)
            dev->term_iface->write(dev->term, &buf, 1);
        if (dev->uart_file)
            dev->uart_file_iface->write(dev->uart_file, &buf, 1);
    } 
    else//normal mode or Local Loopback mode
    {
        WriteFIFO(dev->fifo, &buf, 1);
        if(dev->fifo->Enteres == (regs->XUARTPS_RXWM_OFFSET, 5, 0))//recv over trriger
        {
            regs->XUARTPS_SR_OFFSET = set_bit(regs->XUARTPS_SR_OFFSET, 0);
            regs->XUARTPS_ISR_OFFSET = set_bit(regs->XUARTPS_ISR_OFFSET, 0);
        }
    }
    regs->XUARTPS_SR_OFFSET = clear_bit(regs->XUARTPS_SR_OFFSET, 1);
    regs->XUARTPS_SR_OFFSET = set_bit(regs->XUARTPS_SR_OFFSET, 2);

//    if(bit(regs->XUARTPS_IER_OFFSET, 0) && bit(regs->XUARTPS_IMR_OFFSET, 0))//RF(read buf full) inter,只要往串口接收端写了数据就置接收中断
//    {
        regs->XUARTPS_ISR_OFFSET = set_bit(regs->XUARTPS_ISR_OFFSET, 0);
        if(dev->signal.intf) 
            dev->signal.intf->raise_signal(dev->signal.obj, dev->signal.num);
//    }
    return Access_exp;
}

static uint32_t  serial_write_chars(conf_object_t *opaque, char *buf, uint32_t count)
{
    zynq7000_uart_device *dev = (zynq7000_uart_device *)(opaque->obj);
    struct registers *regs = &(dev->regs);
    uint32_t i;
    
    for(i = 0; i < count; i++)
        serial_write(opaque, buf[i]);
}

// 寄存器读取操作方法
static exception_t zynq7000_uart_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    zynq7000_uart_device *dev = (zynq7000_uart_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);
    switch (offset)
    {
        case _XUARTPS_CR_OFFSET:
            *(uint32_t *)buf = dev->regs.XUARTPS_CR_OFFSET;
            break;
        case _XUARTPS_MR_OFFSET:
            *(uint32_t *)buf = dev->regs.XUARTPS_MR_OFFSET;
            break;
        case _XUARTPS_IER_OFFSET:
            *(uint32_t *)buf = dev->regs.XUARTPS_IER_OFFSET;
            break;
        case _XUARTPS_IDR_OFFSET:
            *(uint32_t *)buf = dev->regs.XUARTPS_IDR_OFFSET;
            break;
        case _XUARTPS_IMR_OFFSET:
            *(uint32_t *)buf = dev->regs.XUARTPS_IMR_OFFSET;
            break;
        case _XUARTPS_ISR_OFFSET:
            *(uint32_t *)buf = dev->regs.XUARTPS_ISR_OFFSET;
            break;
        case _XUARTPS_BAUDGEN_OFFSET:
            *(uint32_t *)buf = dev->regs.XUARTPS_BAUDGEN_OFFSET;
            break;
        case _XUARTPS_RXTOUT_OFFSET:
            *(uint32_t *)buf = dev->regs.XUARTPS_RXTOUT_OFFSET;
            break;
        case _XUARTPS_RXWM_OFFSET:
            *(uint32_t *)buf = dev->regs.XUARTPS_RXWM_OFFSET;
            break;
        case _XUARTPS_MODEMCR_OFFSET:
            *(uint32_t *)buf = dev->regs.XUARTPS_MODEMCR_OFFSET;
            break;
        case _XUARTPS_MODEMSR_OFFSET:
            *(uint32_t *)buf = dev->regs.XUARTPS_MODEMSR_OFFSET;
            break;
        case _XUARTPS_SR_OFFSET:
            *(uint32_t *)buf = dev->regs.XUARTPS_SR_OFFSET;
            break;
        case _XUARTPS_FIFO_OFFSET:
            ReadFIFO(dev->fifo, buf, 1);
            if(dev->fifo->Enteres < (regs->XUARTPS_RXWM_OFFSET, 5, 0))//recv over trriger
            {
                regs->XUARTPS_SR_OFFSET = clear_bit(regs->XUARTPS_SR_OFFSET, 0);
            }
            if(dev->fifo->Enteres == 0)
            {
                regs->XUARTPS_SR_OFFSET = set_bit(regs->XUARTPS_SR_OFFSET, 1);//RE(read empty)
                regs->XUARTPS_SR_OFFSET = clear_bit(regs->XUARTPS_SR_OFFSET, 2);
                if(bit(regs->XUARTPS_IER_OFFSET, 1) && bit(regs->XUARTPS_IMR_OFFSET, 1))//RE
                {
                    regs->XUARTPS_ISR_OFFSET = set_bit(regs->XUARTPS_ISR_OFFSET, 1);
                    if(dev->signal.intf) 
                        dev->signal.intf->raise_signal(dev->signal.obj, dev->signal.num);
                }
            }
            break;
        case _BAUD_RATE_DIVIDER_REG0:
            *(uint32_t *)buf = dev->regs.Baud_rate_divider_reg0;
            break;
        case _FLOW_DELAY_REG0:
            *(uint32_t *)buf = dev->regs.Flow_delay_reg0;
            break;
        case _TX_FIFO_TRIGGER_LEVEL0:
            *(uint32_t *)buf = dev->regs.Tx_FIFO_trigger_level0;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in zynq7000_uart\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器写入操作方法
static exception_t zynq7000_uart_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    zynq7000_uart_device *dev = (zynq7000_uart_device *)conf_obj->obj;
    uint32_t val = *(uint32_t *)buf;
    struct registers *regs = &(dev->regs);
    switch (offset)
    {
        case _XUARTPS_CR_OFFSET:
            dev->regs.XUARTPS_CR_OFFSET = val & 0x1fc;
            break;
        case _XUARTPS_MR_OFFSET:
            dev->regs.XUARTPS_MR_OFFSET = val;
            break;
        case _XUARTPS_IER_OFFSET:
            dev->regs.XUARTPS_IER_OFFSET = val;
            regs->XUARTPS_IMR_OFFSET |= val;
            break;
        case _XUARTPS_IDR_OFFSET:
            dev->regs.XUARTPS_IDR_OFFSET = val;
            regs->XUARTPS_IMR_OFFSET &= ~val;
            break;
        case _XUARTPS_IMR_OFFSET:
            skyeye_log(Warning_log, __FUNCTION__, "Attempted to write the read-only register at 0x%x in zynq7000_uart\n", offset);
            break;
        case _XUARTPS_ISR_OFFSET:
            dev->regs.XUARTPS_ISR_OFFSET &= ~val;
            if(dev->signal.intf)
                dev->signal.intf->lower_signal(dev->signal.obj, dev->signal.num);
            break;
        case _XUARTPS_BAUDGEN_OFFSET:
            dev->regs.XUARTPS_BAUDGEN_OFFSET = val;
            break;
        case _XUARTPS_RXTOUT_OFFSET:
            dev->regs.XUARTPS_RXTOUT_OFFSET = val;
            break;
        case _XUARTPS_RXWM_OFFSET:
            dev->regs.XUARTPS_RXWM_OFFSET = val;
            break;
        case _XUARTPS_MODEMCR_OFFSET:
            dev->regs.XUARTPS_MODEMCR_OFFSET = val;
            break;
        case _XUARTPS_MODEMSR_OFFSET:
            dev->regs.XUARTPS_MODEMSR_OFFSET = val;
            break;
        case _XUARTPS_SR_OFFSET:
            skyeye_log(Warning_log, __FUNCTION__, "Attempted to write the read-only register at 0x%x in zynq7000_uart\n", offset);
            break;
        case _XUARTPS_FIFO_OFFSET:
            dev->regs.XUARTPS_FIFO_OFFSET = val;
            switch(bits(regs->XUARTPS_MR_OFFSET, 9, 8))
            {
                case 0://normal mode
                    if(dev->term_iface)
                        dev->term_iface->write(dev->term, &val, 1);
                    if (dev->uart_file)
                        dev->uart_file_iface->write(dev->uart_file, &val, 1);
                    break;
                case 1://automatic mode
                    if(dev->term_iface)
                        dev->term_iface->write(dev->term, &val, 1);
                    if (dev->uart_file)
                        dev->uart_file_iface->write(dev->uart_file, &val, 1);
                    break;
                case 2://local loopback mode
                    if(dev->term_iface)
                        dev->term_iface->write(dev->term, &val, 1);
                    if (dev->uart_file)
                        dev->uart_file_iface->write(dev->uart_file, &val, 1);
                    serial_write(conf_obj, val);
                    break;
                case 3://remote mode
                    if(dev->term_iface)
                        dev->term_iface->write(dev->term, &val, 1);
                    if (dev->uart_file)
                        dev->uart_file_iface->write(dev->uart_file, &val, 1);
                    break;
                default:
                    printf("error!\n");
            }  
            regs->XUARTPS_SR_OFFSET = clear_bit(regs->XUARTPS_SR_OFFSET, 11);//inactive state
            regs->XUARTPS_SR_OFFSET= clear_bit(regs->XUARTPS_SR_OFFSET, 4);
            regs->XUARTPS_SR_OFFSET = set_bit(regs->XUARTPS_SR_OFFSET, 3);
            if(bit(regs->XUARTPS_IER_OFFSET, 3) && bit(regs->XUARTPS_IMR_OFFSET, 3))//TE
            {
                regs->XUARTPS_ISR_OFFSET = set_bit(regs->XUARTPS_ISR_OFFSET, 3);//TE
                if(dev->signal.intf)
                    dev->signal.intf->raise_signal(dev->signal.obj, dev->signal.num);
            }
            break;
        case _BAUD_RATE_DIVIDER_REG0:
            dev->regs.Baud_rate_divider_reg0 = val;
            break;
        case _FLOW_DELAY_REG0:
            dev->regs.Flow_delay_reg0 = val;
            break;
        case _TX_FIFO_TRIGGER_LEVEL0:
            dev->regs.Tx_FIFO_trigger_level0 = val;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in zynq7000_uart\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器接口:根据寄存器ID获取寄存器名
static char *zynq7000_uart_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_name[id];
}

// 寄存器接口:根据寄存器ID设置寄存器值
static exception_t zynq7000_uart_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    zynq7000_uart_device *dev = (zynq7000_uart_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    *((uint32_t *)regs + id) = value;

    return No_exp;
}

// 寄存器接口:获取设备有多少寄存器
static uint32_t zynq7000_uart_get_regnum(conf_object_t *conf_obj)
{
    return sizeof(struct registers) / 4;
}

// 寄存器接口:根据寄存器ID获取寄存器值
static uint32_t zynq7000_uart_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    zynq7000_uart_device *dev = (zynq7000_uart_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    uint32_t *regs_value = (uint32_t *)regs + id;

    return *regs_value;
}

// 寄存器接口:根据寄存器名获取寄存器ID
static uint32_t zynq7000_uart_get_regid_by_name(conf_object_t *conf_obj, char *name)
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
static uint32_t zynq7000_uart_get_regoffset_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_offset[id];
}

// 实例化一个新设备对象，obj_name是在json文件中配置的名称
static conf_object_t *new_zynq7000_uart(const char *obj_name)
{
    zynq7000_uart_device *dev = skyeye_mm_zero(sizeof(zynq7000_uart_device));
    dev->obj = new_conf_object(obj_name, dev);
    // TODO: 在此添加自定义源码
    dev->fifo = CreateFIFO(100);

    // 寄存器初始化
    struct registers *regs = &(dev->regs);
    regs->XUARTPS_CR_OFFSET = 0x00000128;
    regs->XUARTPS_MR_OFFSET = 0;
    regs->XUARTPS_IER_OFFSET = 0;
    regs->XUARTPS_IDR_OFFSET = 0;
    regs->XUARTPS_IMR_OFFSET = 0;
    regs->XUARTPS_ISR_OFFSET = 0;
    regs->XUARTPS_BAUDGEN_OFFSET = 0x0000028B;
    regs->XUARTPS_RXTOUT_OFFSET = 0;
    regs->XUARTPS_RXWM_OFFSET = 0x00000020;
    regs->XUARTPS_MODEMCR_OFFSET = 0;
    regs->XUARTPS_MODEMSR_OFFSET = 0;
    regs->XUARTPS_SR_OFFSET = 0;
    regs->XUARTPS_FIFO_OFFSET = 0;
    regs->Baud_rate_divider_reg0 = 0x0000000F;
    regs->Flow_delay_reg0 = 0;
    regs->Tx_FIFO_trigger_level0 = 0x00000020;

    return dev->obj;
}

// 配置实例化的设备对象
static exception_t config_zynq7000_uart(conf_object_t *conf_obj)
{
    zynq7000_uart_device *dev = (zynq7000_uart_device *)conf_obj->obj;
    // TODO: 在此添加自定义源码

    return No_exp;
}

// 释放设备对象来做一些内存释放
static exception_t free_zynq7000_uart(conf_object_t *conf_obj)
{
    zynq7000_uart_device *dev = (zynq7000_uart_device *)conf_obj->obj;
    FreeFIFO(dev->fifo);

    skyeye_free(conf_obj->objname);
    skyeye_free(conf_obj->obj);
    skyeye_free(conf_obj);
    
    return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    zynq7000_uart_device *dev =  conf_obj->obj;
    attr_value_t int_number_attr = SKY_make_attr_uinteger(dev->signal.num);
    return int_number_attr;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    zynq7000_uart_device *dev =  conf_obj->obj;
    dev->signal.num = SKY_attr_uinteger(*value);
    return No_exp;
}

static void zynq7000_uart_register_attribute(conf_class_t* clss)
{
    SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of zynq7000 uart");
}

static exception_t signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    zynq7000_uart_device *dev =  obj->obj;

    dev->signal.obj = obj2;
    dev->signal.intf = (general_signal_intf *)SKY_get_iface(dev->signal.obj, GENERAL_SIGNAL_INTF_NAME);

    if (dev->signal.intf == NULL)
    {
        if (dev->signal.obj == NULL)
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

static exception_t signal_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    zynq7000_uart_device *dev =  obj->obj;
    *obj2 = dev->signal.obj;
    *port = NULL;
    return No_exp;
}

static exception_t uart_term_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    zynq7000_uart_device *dev = obj->obj;
    dev->term = obj2;
    dev->term_iface = (skyeye_uart_intf*)SKY_get_iface(obj2, SKYEYE_UART_INTF);
    if (dev->term_iface == NULL){
        if (dev->term == NULL)
            skyeye_log(Error_log, __FUNCTION__, "Get %s interface from NULL object fail.", SKYEYE_UART_INTF);
        else
            skyeye_log(Error_log, __FUNCTION__, "Get %s interface from %s fail.", SKYEYE_UART_INTF, obj2->objname);

        return Not_found_exp;
    }
    return No_exp;
}

static exception_t uart_term_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    zynq7000_uart_device *dev = obj->obj;
    *obj2 = dev->term;
    *port = NULL;
    return No_exp;
}

static exception_t uart_file_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    zynq7000_uart_device *dev =  obj->obj;
    dev->uart_file = obj2;
    dev->uart_file_iface = (uart_file_intf*)SKY_get_iface(obj2, UART_FILE_INTF);
    if (dev->uart_file_iface == NULL)
    {
        if (dev->uart_file == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", UART_FILE_INTF);
        } else
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", UART_FILE_INTF, obj2->objname);

        return Not_found_exp;
    }
    return No_exp;
}

static exception_t uart_file_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    zynq7000_uart_device *dev =  obj->obj;
    *obj2 = dev->uart_file;
    *port = NULL;
    return No_exp;
}

// 初始化模块类，并在SkyEye加载模块时执行以下代码
void init_zynq7000_uart(void)
{
    // 向SkyEye注册一个设备
    static skyeye_class_t class_data =
    {
            .class_name = "zynq7000_uart",
            .class_desc = "Universal Asynchronous Receiver Transmitter",
            .new_instance = new_zynq7000_uart,
            .free_instance = free_zynq7000_uart,
            .config_instance = config_zynq7000_uart,
            .module_type = SKYML_OFFCHIP,
    };
    conf_class_t *dev_class = SKY_register_device_class(class_data.class_name, &class_data);

    // 设置寄存器地址读写方法
    static memory_space_intf io_memory_interface =
    {
            .read = zynq7000_uart_read,
            .write = zynq7000_uart_write
    };
    SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

    // 注册访问接口(用于SkyEye图形控制接口)
    static skyeye_reg_intf reg_interface =
    {
            .get_regvalue_by_id = zynq7000_uart_get_regvalue_by_id,
            .get_regname_by_id = zynq7000_uart_get_regname_by_id,
            .set_regvalue_by_id = zynq7000_uart_set_regvalue_by_id,
            .get_regnum = zynq7000_uart_get_regnum,
            .get_regid_by_name = zynq7000_uart_get_regid_by_name,
            .get_regoffset_by_id = zynq7000_uart_get_regoffset_by_id
    };
    SKY_register_iface(dev_class, SKYEYE_REG_INTF, &reg_interface);

    static const skyeye_serial_device_t serial_device = {
        .write = serial_write,
        .receive_ready = NULL,
        .write_chars = serial_write_chars,
    };
    SKY_register_iface(dev_class, SERIAL_DEVICE_INTERFACE, &serial_device);
    
    // 接口方法列表
    static const struct InterfaceDescription ifaces[] =
    {
            (struct InterfaceDescription){
                .name = MEMORY_SPACE_INTF_NAME,
                .iface = &io_memory_interface
            },
            (struct InterfaceDescription){
                .name = SKYEYE_REG_INTF,
                .iface = &reg_interface
            },
            (struct InterfaceDescription) {
                .name = SERIAL_DEVICE_INTERFACE,
                .iface = &serial_device,
            }
    };

    // 接口连接方法列表
    static const struct ConnectDescription connects[] =
    {
        (struct ConnectDescription )
        {
            .name = SKYEYE_UART_INTF,
            .set = uart_term_set,
            .get = uart_term_get,
        },
        (struct ConnectDescription )
        {
            .name = UART_FILE_INTF,
            .set = uart_file_set,
            .get = uart_file_get,
        },
        (struct ConnectDescription )
        {
            .name = GENERAL_SIGNAL_INTF_NAME,
            .set = signal_set,
            .get = signal_get,
        }
    };
    class_register_ifaces(dev_class, ifaces);
    class_register_connects(dev_class, connects);
    zynq7000_uart_register_attribute(dev_class);
}
