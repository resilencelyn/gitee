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
#include "mpc8247_dpram.h"

// 函数声明
static exception_t mpc8247_dpram_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count);

// 寄存器读取操作方法
static exception_t mpc8247_dpram_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc8247_dpram_device *dev = (mpc8247_dpram_device *)conf_obj->obj;

    if (dev->ram_space.obj && dev->ram_space.iface)
    {
        dev->ram_space.iface->read(dev->ram_space.obj, offset, buf, count);
    }

    return Not_found_exp;
}

static void doUartTx(conf_object_t *conf_obj, generic_address_t offset)
{
    mpc8247_dpram_device *dev = (mpc8247_dpram_device *)conf_obj->obj;

    if (dev->ram_space.obj == NULL && dev->ram_space.iface == NULL)
    {
        return;
    }

    // char *pcbd_buf;
    uint32_t index, bufptr;
    uint16_t sc, datlen;
    uint8_t bd;

    dev->ram_space.iface->read(dev->ram_space.obj, offset, &sc, 2);
    dev->ram_space.iface->read(dev->ram_space.obj, offset + 2, &datlen, 2);
    dev->ram_space.iface->read(dev->ram_space.obj, offset + 4, &bufptr, 4);

    bufptr -= 0xf0000000; //手动减去基地址
    dev->ram_space.iface->read(dev->ram_space.obj, bufptr, &bd, 1);

    for (index = 0; index < datlen; index++)
    {
        if (dev->skyeye_uart.obj && dev->skyeye_uart.iface)
        {
            dev->skyeye_uart.iface->write(dev->skyeye_uart.obj, &bd, 1);
        }

        if (dev->uart_file.iface && dev->uart_file.obj)
        {
            dev->uart_file.iface->write(dev->uart_file.obj, &bd, 1);
        }
    }

#define SMC_SMCMR 0x1
#define SMC_SMCE  0x2
#define SMC_SMCM  0x3

#define SMCE_TX 0x2
#define SMCE_RX 0x1

    /*BD Status中断标志位为1, 手册page847*/
    if (sc & BD_SC_INTRPT)
    {
        /*BD Status中I标志位置1之后设置SMCE_TX, 在SMC模块中触发中断*/
        dev->smc_reg.iface->set_regvalue_by_id(dev->smc_reg.obj, SMCE_TX, SMC_SMCE);
    }

    sc &= ~BD_SC_READY;
    mpc8247_dpram_write(conf_obj, offset, &sc, 2);
}

static void doUartRx(conf_object_t *conf_obj, uint8_t value)
{
    mpc8247_dpram_device *dev = (mpc8247_dpram_device *)conf_obj->obj;

#define SMC_SMCMR 0x1
#define SMC_SMCE  0x2
#define SMC_SMCM  0x3

#define SMCE_RX 0x1

    // struct cbd varible
    uint16_t sc, datlen, bufaddr;
    uint32_t bufptr;

    // 获取smc1 RXBD 第一个所在地址
    uint16_t offset;
    offset = dev->smc_ram_parameter[0].rbptr;

    if (dev->ram_space.obj == NULL && dev->ram_space.iface == NULL)
    {
        return;
    }

    if (value == 0x7f)
    {
        value = '\b';
    }

    // read current RXBD value
    dev->ram_space.iface->read(dev->ram_space.obj, offset, &sc, 2);
    dev->ram_space.iface->read(dev->ram_space.obj, offset + 2, &datlen, 2);
    dev->ram_space.iface->read(dev->ram_space.obj, offset + 4, &bufptr, 4);
    bufptr -= 0xf0000000; // modify bufptr in actual address of ram

    if (sc & BD_SC_CM)
    {
        // overwrite and don't clear E bit
        dev->ram_space.iface->write(dev->ram_space.obj, bufptr, &value, 1); //write value in buffer
        sc |= BD_SC_INTRPT;
        if (sc & BD_SC_INTRPT)
        {
            dev->smc_reg.iface->set_regvalue_by_id(dev->smc_reg.obj, SMCE_RX, SMC_SMCE);
        }

        mpc8247_dpram_write(conf_obj, offset, &sc, 2);
    }
    else
    {
        // when buffer is filled, clear E bit and trigger interrupt
        if (dev->receiver_bd_char_count < datlen)
        {
            bufptr = bufptr + dev->receiver_bd_char_count;
            dev->ram_space.iface->write(dev->ram_space.obj, bufptr, &value, 1);
            (dev->receiver_bd_char_count)++;
        }

        if (datlen == dev->receiver_bd_char_count)
        {
            dev->receiver_bd_char_count = 0; // clear the bd count
            dev->smc_ram_parameter[0].rbptr += 8;

            if (sc & BD_SC_WRAP)
            {
                dev->smc_ram_parameter[0].rbptr = dev->smc_ram_parameter[0].rbase;
            }
            sc &= ~BD_SC_EMPTY; // clear E bit
            mpc8247_dpram_write(conf_obj, offset, &sc, 2); // write sc to ram

            if (sc & BD_SC_INTRPT)
            {
                dev->smc_reg.iface->set_regvalue_by_id(dev->smc_reg.obj, SMCE_RX, SMC_SMCE);
            }
        }
    }
}

// 寄存器写入操作方法
static exception_t mpc8247_dpram_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc8247_dpram_device *dev = (mpc8247_dpram_device *)conf_obj->obj;
    uint32_t val = *(uint32_t *)buf;
    uint8_t val8 = *(uint8_t *)buf;

    exception_t ret;
    uint32_t index, off, i, base;
    uint16_t cbd_sc;

    skyeye_reg_intf *reg_iface;
    conf_object_t *reg;

    uint16_t regid, smcmr, smcmr_sm;
    uint8_t smc_smce;

    if (dev->ram_space.iface == NULL || dev->ram_space.obj == NULL)
    {
        return No_exp;
    }

    /* SCM1 : 0x87FC
     * SCM2 : 0x88FC */
    if (offset >= SMC1_RAM_BASE && offset < SMC2_RAM_BASE + 2)
    {
        if (offset == SMC1_RAM_BASE)
        {
            dev->smc1_rambase_by_ram = *(uint16_t *)buf;
        }
        if (offset == SMC2_RAM_BASE)
        {
            dev->smc2_rambase_by_ram = *(uint16_t *)buf;
        }
    }

    /*SCM1 parameter ram的地址,手册 page 836*/
    if (offset >= dev->smc1_rambase_by_ram && offset <= dev->smc1_rambase_by_ram + 0x34)
    {
        off = offset - dev->smc1_rambase_by_ram;

        switch (off)
        {
            case RBASE:
                dev->smc_ram_parameter[0].rbase = *((uint16_t *)buf);
                break;
            case TBASE:
                dev->smc_ram_parameter[0].tbase = *((uint16_t *)buf);
                break;
            case MRBLR:
                dev->smc_ram_parameter[0].rx_fifosize = *((uint16_t *)buf);
                break;
            case RBPTR:
                dev->smc_ram_parameter[0].rbptr = *((uint16_t *)buf);
                break;
            default:
                printf("cannot write scm1 parameter ram, offset : 0x%x, val : 0x%x\n", off, *(uint32_t *)buf);
                break;
        }
    }

    /*SCM2 parameter ram的地址,手册 page 836*/
    if (offset >= dev->smc2_rambase_by_ram && offset <= dev->smc2_rambase_by_ram + 0x34)
    {
        off = offset - dev->smc1_rambase_by_ram;

        switch (off)
        {
            case RBASE:
                dev->smc_ram_parameter[1].rbase = *((uint16_t *)buf);
                break;
            case TBASE:
                dev->smc_ram_parameter[1].tbase = *((uint16_t *)buf);
                break;
            case MRBLR:
                dev->smc_ram_parameter[1].rx_fifosize = *((uint16_t *)buf);
                break;
            default:
                printf("cannot write scm2 parameter ram, offset : 0x%x, val : 0x%x\n", off, *(uint32_t *)buf);
                break;
        }
    }

    // 数据写入ram地址空间
    ret = dev->ram_space.iface->write(dev->ram_space.obj, offset, buf, count);

    // 根据parameter ram个数做循环
    for (i = 0;
         i < (sizeof(dev->smc_ram_parameter) / sizeof(struct smc_parameter_ram));
         i++)
    {
        // 数据放在TXBD中 前置条件
        if (offset >= dev->smc_ram_parameter[i].tbase && offset < dev->smc_ram_parameter[i].tbase + sizeof(cbd_t))
        {
            base = offset - offset % 8; // 偏移值需等于cbd.cbd_sc，表示处于TXBD起始位置

            if (offset == base)
            {
                if (dev->smc_reg.obj == NULL || dev->smc_reg.iface == NULL)
                {
                    continue;
                }

                /*BD Status*/
                cbd_sc = *((uint16_t *)buf);

                if ((cbd_sc & BD_SC_READY) == 0)
                {
                    continue;
                }

                reg_iface = dev->smc_reg.iface;
                reg = dev->smc_reg.obj;

                regid = reg_iface->get_regid_by_name(reg, "SMCMR");
                smcmr = (uint16_t)reg_iface->get_regvalue_by_id(reg, regid);
                smcmr_sm = (smcmr >> 4) & 0x3; // sm is mode bit

                switch (smcmr_sm)
                {
                    case SMCMR_SM_GCIorSCIT:
                        break;
                    case SMCMR_SM_RES:
                        break;
                    case SMCMR_SM_UART:
                        doUartTx(conf_obj, offset);
                        break;
                    case SMCMR_SM_TRANSPARENT:
                        break;
                    default:
                        printf("[%s:%d] unknown smc mode.\n", __FILE__, __LINE__);
                        break;
                }
            }
        }
    }

    return No_exp;
}

// 实例化一个新设备对象，obj_name是在json文件中配置的名称
static conf_object_t *new_mpc8247_dpram(const char *obj_name)
{
    mpc8247_dpram_device *dev = skyeye_mm_zero(sizeof(mpc8247_dpram_device));
    dev->obj = new_conf_object(obj_name, dev);
    // TODO: 在此添加自定义源码
    dev->receiver_bd_char_count = 0;

    return dev->obj;
}

// 配置实例化的设备对象
static exception_t config_mpc8247_dpram(conf_object_t *conf_obj)
{
    mpc8247_dpram_device *dev = (mpc8247_dpram_device *)conf_obj->obj;
    // TODO: 在此添加自定义源码

    return No_exp;
}

// 释放设备对象来做一些内存释放
static exception_t free_mpc8247_dpram(conf_object_t *conf_obj)
{
    mpc8247_dpram_device *dev = (mpc8247_dpram_device *)conf_obj->obj;

    skyeye_free(conf_obj->objname);
    skyeye_free(conf_obj->obj);
    skyeye_free(conf_obj);

    return No_exp;
}
static exception_t ram_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc8247_dpram_device *dev = (mpc8247_dpram_device *)obj->obj;

    dev->ram_space.obj = obj2;
    if (dev->ram_space.obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from NULL object fail.\n", MEMORY_SPACE_INTF_NAME);

        return Not_found_exp;
    }
    dev->ram_space.iface = (memory_space_intf *)SKY_get_iface(obj2, MEMORY_SPACE_INTF_NAME);
    if (dev->ram_space.iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", MEMORY_SPACE_INTF_NAME, obj2->objname);
        return Not_found_exp;
    }

    return No_exp;
}

static exception_t ram_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    mpc8247_dpram_device *dev = (mpc8247_dpram_device *)obj->obj;

    *obj2 = dev->ram_space.obj;
    *port = NULL;

    return No_exp;
}

static exception_t dpram_set_smc(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc8247_dpram_device *dev = (mpc8247_dpram_device *)obj->obj;

    dev->smc_dev.obj = obj2;
    if (dev->smc_dev.obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from NULL object fail.\n", DPRAM_GET_SMC_REG);

        return Not_found_exp;
    }
    dev->smc_dev.iface = (dpram_get_smc_reg_intf *)SKY_get_iface(obj2, DPRAM_GET_SMC_REG);
    if (dev->smc_dev.iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", DPRAM_GET_SMC_REG, obj2->objname);
        return Not_found_exp;
    }

    return No_exp;
}

static exception_t dpram_get_smc(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    mpc8247_dpram_device *dev = (mpc8247_dpram_device *)obj->obj;

    *obj2 = dev->smc_dev.obj;
    *port = NULL;

    return No_exp;
}

// 系统设置设备 skyeye_uart 接口功能
static exception_t skyeye_uart_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc8247_dpram_device *dev = (mpc8247_dpram_device *)obj->obj;

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
    mpc8247_dpram_device *dev = (mpc8247_dpram_device *)obj->obj;

    *obj2 = dev->skyeye_uart.obj;
    *port = NULL;

    return No_exp;
}

// 配置 skyeye_regs_set
static exception_t skyeye_regs_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc8247_dpram_device *dev = (mpc8247_dpram_device *)obj->obj;

    dev->smc_reg.obj = obj2;
    if (obj2 == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from NULL object fail.\n", SKYEYE_REG_INTF);
        return Not_found_exp;
    }
    dev->smc_reg.iface = (skyeye_reg_intf *)SKY_get_iface(obj2, SKYEYE_REG_INTF);
    if (dev->smc_reg.iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from %s fail.\n", SKYEYE_REG_INTF, obj2->objname);
        return Not_found_exp;
    }

    return No_exp;
}

// 配置 skyeye_regs_set
static exception_t skyeye_regs_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    mpc8247_dpram_device *dev = (mpc8247_dpram_device *)obj->obj;

    *obj2 = dev->smc_reg.obj;
    *port = NULL;

    return No_exp;
}

// 系统设置设备 uart_file 接口功能
static exception_t uart_file_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc8247_dpram_device *dev = (mpc8247_dpram_device *)obj->obj;

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
    mpc8247_dpram_device *dev = (mpc8247_dpram_device *)obj->obj;

    *obj2 = dev->uart_file.obj;
    *port = NULL;

    return No_exp;
}

// 系统设置设备 general_signal 接口功能
static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc8247_dpram_device *dev = (mpc8247_dpram_device *)obj->obj;

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
    mpc8247_dpram_device *dev = (mpc8247_dpram_device *)obj->obj;

    *obj2 = dev->general_signal.obj;
    *port = NULL;

    return No_exp;
}

// 串口发送一个字节
static exception_t dpram_serial_write(conf_object_t *conf_obj, uint32_t value)
{
    mpc8247_dpram_device *dev = (mpc8247_dpram_device *)conf_obj->obj;
    uint8_t val = (uint8_t)value;

    doUartRx(conf_obj, value);

    return No_exp;
}

// 确定是否可以接收数据
static exception_t dpram_receive_ready(conf_object_t *conf_obj)
{
    mpc8247_dpram_device *dev = (mpc8247_dpram_device *)conf_obj->obj;

    return No_exp;
}

// 连续发送一组数据
static uint32_t dpram_serial_write_chars(conf_object_t *conf_obj, char *buf, uint32_t cnt)
{
    mpc8247_dpram_device *dev = (mpc8247_dpram_device *)conf_obj->obj;

    return No_exp;
}

// 初始化模块类，并在SkyEye加载模块时执行以下代码
void init_mpc8247_dpram(void)
{
    // 向SkyEye注册一个设备
    static skyeye_class_t class_data =
        {
            .class_name = "mpc8247_dpram",
            .class_desc = "mpc8247_dpram",
            .new_instance = new_mpc8247_dpram,
            .free_instance = free_mpc8247_dpram,
            .config_instance = config_mpc8247_dpram,
            .module_type = SKYML_OFFCHIP,
        };
    conf_class_t *dev_class = SKY_register_device_class(class_data.class_name, &class_data);

    // 串行设备设备单向传输接口
    static const skyeye_serial_device_t skyeye_serial_interface =
        {
            .write = dpram_serial_write,
            .write_chars = dpram_serial_write_chars,
            .receive_ready = dpram_receive_ready};
    SKY_register_iface(dev_class, SERIAL_DEVICE_INTERFACE, &skyeye_serial_interface);

    // 设置寄存器地址读写方法
    static memory_space_intf io_memory_interface =
        {
            .read = mpc8247_dpram_read,
            .write = mpc8247_dpram_write};
    SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

    // 接口方法列表
    static const struct InterfaceDescription ifaces[] =
        {
            (struct InterfaceDescription){
                .name = MEMORY_SPACE_INTF_NAME,
                .iface = &io_memory_interface},
            (struct InterfaceDescription){
                .name = SERIAL_DEVICE_INTERFACE,
                .iface = &skyeye_serial_interface},
        };

    // 接口连接方法列表
    static const struct ConnectDescription connects[] =
        {
            (struct ConnectDescription){
                .name = GENERAL_SIGNAL_INTF_NAME,
                .set = general_signal_set,
                .get = general_signal_get},
            (struct ConnectDescription){
                .name = MEMORY_SPACE_INTF_NAME,
                .set = ram_set,
                .get = ram_get},
            (struct ConnectDescription){
                .name = DPRAM_GET_SMC_REG,
                .set = dpram_set_smc,
                .get = dpram_get_smc},
            (struct ConnectDescription){
                .name = UART_FILE_INTF,
                .set = uart_file_set,
                .get = uart_file_get},
            (struct ConnectDescription){
                .name = SKYEYE_UART_INTF,
                .set = skyeye_uart_set,
                .get = skyeye_uart_get},
            (struct ConnectDescription){
                .name = SKYEYE_REG_INTF,
                .set = skyeye_regs_set,
                .get = skyeye_regs_get},
        };

    class_register_ifaces(dev_class, ifaces);
    class_register_connects(dev_class, connects);
}
