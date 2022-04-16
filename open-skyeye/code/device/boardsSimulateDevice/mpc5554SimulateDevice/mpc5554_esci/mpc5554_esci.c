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
#include "mpc5554_esci.h"
#ifdef __WIN32__
#include <windows.h>
#endif


#ifdef DEBUG
#define debug_printf    printf
#else
#define debug_printf
#endif

static int recv_edma_trans_len(conf_object_t *conf_object, uint32_t TransLen)
{
    mpc5554_esci_device *dev = (mpc5554_esci_device *)conf_object->obj;

    dev->DMACurTransLen = TransLen;
    debug_printf("Translen = %d :", TransLen);
}

void uart_com_recv(conf_object_t* opaque)
{
    mpc5554_esci_device *uart = (mpc5554_esci_device *)(opaque->obj);
    exception_t ret = 0;
    unsigned char buf;

    if(uart->uart_com_iface != NULL)
    {
        if((ret = uart->uart_com_iface->read(uart->uart_com, &buf, 1)) == No_exp)
        {
            if(uart->edma_iface)
            {
                uart->edma_iface->recv_length(uart->edma, 1);
            }
        }
        if(buf == 127)
        {
            buf = '\b';
        } else if(buf == 10 || buf == 13)
        {
            buf = '\r';
        }

        WriteFIFO(uart->RxFIFO, &buf, 1);
    }
}

// 寄存器读取操作方法
static exception_t mpc5554_esci_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc5554_esci_device *dev = (mpc5554_esci_device *)conf_obj->obj;
    uint32_t data = 0;

    if(offset < 4 || offset > 7)
    {
        offset = (offset / 0x4) * 0x4;
    } else
    {
        offset = (offset / 0x2) * 0x2;
    }

    switch (offset)
    {
        case _CR1:
            data = dev->regs.CR1;
            break;
        case _CR2:
            data = dev->regs.CR2;
            break;
        case _DR:
            if(esci_status.tx_en)
            {
                if(dev->DMACurTransLen > 0)
                {
                    uint32_t tmpVal;
                    ReadFIFO(dev->RxFIFO, &tmpVal, 1);
                    dev->regs.DR = (uint8_t)tmpVal;
                    dev->DMACurTransLen --;
                }
            }
            data = dev->regs.DR;
            break;
        case _SR:
            data = dev->regs.SR;
            break;
        case _LCR:
            data = dev->regs.LCR;
            break;
        case _LTR:
            data = dev->regs.LTR;
            break;
        case _LRR:
            data = dev->regs.LRR;
            break;
        case _LPR:
            data = dev->regs.LPR;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in mpc5554_esci\n", offset);
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

    //printf("In %s | offset :  0x%x | val : 0x%x\n",__func__, offset, *(uint32_t *)buf);
    return No_exp;
}

// 寄存器写入操作方法
static exception_t mpc5554_esci_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc5554_esci_device *dev = (mpc5554_esci_device *)conf_obj->obj;
    uint32_t val = *(uint32_t *)buf;

    //printf("In %s | offset :  0x%x | val : 0x%x\n",__func__, offset, val);

    if(offset < 4 || offset > 7)
    {
        offset = (offset / 0x4) * 0x4;
    } else
    {
        offset = (offset / 0x2) * 0x2;
    }

    switch (offset)
    {
        case _CR1:
            dev->regs.CR1 = val;
            if(dev->regs.CR1 & ESCI_TRAN_EN)
            {
                /*使能发送*/
                esci_status.tx_en = 1;
            } else
            {
                /*禁止发送*/
                esci_status.tx_en = 0;
            }
            if(dev->regs.CR1 & ESCI_RECV_EN)
            {
                /*使能接收*/
                esci_status.rx_en = 1;
            } else
            {
                /*禁止接收*/
                esci_status.rx_en = 0;
            }
            if(dev->regs.CR1 & ESCI_RECV_EN)
            {
                if(dev->edma_iface)
                {
                    dev->edma_iface->send_flag(dev->edma, 0x3);
                }
            }
            break;
        case _CR2:
            dev->regs.CR2 = val;
            break;
        case _DR:
            {
                dev->regs.DR = val & 0xffff;
                if (esci_status.rx_en)
                {
                    uint8_t value = val & 0xff;
                    uint8_t buffer[32] = {0};
                    sprintf(buffer, "0x%x ", value);
                    uint8_t j = 0;
                    if (dev->uart_com)
                    {
                        dev->uart_com_iface->write(dev->uart_com, &value, 1);
                    }


                    while (buffer[j] != NULL)
                    {
                        if (dev->term.obj)
                        {
                            dev->term.intf->write(dev->term.obj, buffer + j, 1);
                        }


                        if (dev->uart_file.obj)
                        {
                            dev->uart_file.iface->write(dev->uart_file.obj, buffer + j, 1);
                        }

                        j++;
                    }

                    if ((dev->tail[0] == 0xbb) && (value == 0xbb))
                    {
                        dev->tail[0] = dev->tail[1]  = 0;
                        uint8_t tmp = '\n';
                        if(dev->term.obj)
                        {
                            dev->term.intf->write(dev->term.obj, &tmp, 1);
                        }
                    }
                    dev->tail[0] = value;
                }
            }
            break;
        case _DR + 0x1:
        case _SR:
            dev->regs.SR = val;
            break;
        case _LCR:
            dev->regs.LCR = val;
            break;
        case _LTR:
            dev->regs.LTR = val;
            break;
        case _LRR:
            dev->regs.LRR = val;
            break;
        case _LPR:
            dev->regs.LPR = val;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in mpc5554_esci\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器接口:根据寄存器ID获取寄存器名
static char* mpc5554_esci_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_name[id];
}

// 寄存器接口:根据寄存器ID设置寄存器值
static exception_t mpc5554_esci_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    mpc5554_esci_device *dev = (mpc5554_esci_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    *((uint32_t*)regs + id) = value;

    return No_exp;
}

// 寄存器接口:获取设备有多少寄存器
static uint32_t mpc5554_esci_get_regnum(conf_object_t *conf_obj)
{
    return 8;
}

// 寄存器接口:根据寄存器ID获取寄存器值
static uint32_t mpc5554_esci_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    mpc5554_esci_device *dev = (mpc5554_esci_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    uint32_t* regs_value = (uint32_t*)regs + id;

    return *regs_value;
}

// 寄存器接口:根据寄存器名获取寄存器ID
static uint32_t mpc5554_esci_get_regid_by_name(conf_object_t *conf_obj, char *name)
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
static uint32_t mpc5554_esci_get_regoffset_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_offset[id];
}

// 实例化一个新设备对象，obj_name是在json文件中配置的名称
static conf_object_t* new_mpc5554_esci(const char *obj_name)
{
    mpc5554_esci_device* dev = skyeye_mm_zero(sizeof(mpc5554_esci_device));
    dev->obj = new_conf_object(obj_name, dev);
    // TODO: 在此添加自定义源码

    // 寄存器初始化
    struct registers *regs = &(dev->regs);
    regs->CR1 = 0x40000;
    regs->CR2 = 0x2000;
    regs->DR = 0;
    regs->SR = 0xc0000000;
    regs->LCR = 0;
    regs->LTR = 0;
    regs->LRR = 0;
    regs->LPR = 0xc5990000;

    dev->RxFIFO = CreateFIFO(4096);
    dev->TxFIFO = CreateFIFO(4096);
    dev->PRGFIFO = CreateFIFO(2048);

    return dev->obj;
}

// 配置实例化的设备对象
static exception_t config_mpc5554_esci(conf_object_t* conf_obj)
{
    mpc5554_esci_device *dev = (mpc5554_esci_device *)conf_obj->obj;
    // TODO: 在此添加自定义源码

    int scheduler_id = 0;
    create_thread_scheduler(1000, Periodic_sched, uart_com_recv, conf_obj, &scheduler_id);

    return No_exp;
}

// 释放设备对象来做一些内存释放
static exception_t free_mpc5554_esci(conf_object_t* conf_obj)
{
    mpc5554_esci_device *dev = (mpc5554_esci_device *)conf_obj->obj;

    RWLOCK_DESTROY(dev->lock);
    FreeFIFO(dev->RxFIFO);
    FreeFIFO(dev->TxFIFO);
    FreeFIFO(dev->PRGFIFO);
    skyeye_free(conf_obj->objname);
    skyeye_free(conf_obj->obj);
    skyeye_free(conf_obj);

    return No_exp;
}

/* 虚拟串口接口配置 */
static exception_t uart_com_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc5554_esci_device *dev =  obj->obj;
    dev->uart_com = obj2;
    dev->uart_com_iface = (uart_com_intf *)SKY_get_iface(dev->uart_com, UART_COM_INTF);
    if (dev->uart_com_iface == NULL)
    {
        if (dev->uart_com == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", UART_COM_INTF);
        } else
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", UART_COM_INTF, obj2->objname);
        }

        return Not_found_exp;
    }

    return No_exp;
}

static exception_t uart_com_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    mpc5554_esci_device *dev =  obj->obj;
    *obj2 = dev->uart_com;
    *port = NULL;
    return No_exp;
}

/* 中断控制器接口配置 */
static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc5554_esci_device *dev =  obj->obj;
    dev->general_signal.obj = obj2;
    dev->general_signal.iface = (general_signal_iface_t *)SKY_get_iface(dev->general_signal.obj, GENERAL_SIGNAL_INTF_NAME);
    if (dev->general_signal.iface == NULL)
    {
        if ( dev->general_signal.obj == NULL)
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

static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    mpc5554_esci_device *dev = obj->obj;
    *obj2 = dev->general_signal.obj;
    *port = NULL;
    return No_exp;
}

/* edma接口配置 */
static exception_t edma_iface_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc5554_esci_device *dev =  obj->obj;
    dev->edma = obj2;
    dev->edma_iface = (esci_sendto_edma_intf *)SKY_get_iface(dev->edma, ESCI_SENDTO_EDMA);
    if (dev->edma_iface == NULL)
    {
        if (dev->edma == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", ESCI_SENDTO_EDMA);
        } else
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", ESCI_SENDTO_EDMA, obj2->objname);
        }

        return Not_found_exp;
    }

    return No_exp;
}

static exception_t edma_iface_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    mpc5554_esci_device *dev = obj->obj;
    *obj2 = dev->edma;
    *port = NULL;
    return No_exp;
}

/* 串口终端接口配置 */
static exception_t uart_term_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc5554_esci_device *dev =  obj->obj;
    dev->term.obj = obj2;
    dev->term.intf = (skyeye_uart_intf *)SKY_get_iface(dev->term.obj, SKYEYE_UART_INTF);
    //printf("111uart_term : %s\n",dev->term.obj->objname);
    if ( dev->term.intf == NULL)
    {
        if (dev->term.obj == NULL)
        {	
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SKYEYE_UART_INTF);
        } else
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SKYEYE_UART_INTF, obj2->objname);
        }

        return Not_found_exp;
    }

    //printf("222uart_term : %s\n",dev->term.obj->objname);

    return No_exp;
}

static exception_t uart_term_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    mpc5554_esci_device *dev = obj->obj;
    *obj2 = dev->term.obj;
    *port = NULL;
    return No_exp;
}

/* 串口输出文件接口配置 */
static exception_t uart_file_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc5554_esci_device *dev =  obj->obj;
    dev->uart_file.obj = obj2;
    dev->uart_file.iface = (uart_file_intf*)SKY_get_iface(obj2, UART_FILE_INTF);
    if (dev->uart_file.iface == NULL)
    {
        if (dev->uart_file.obj == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", UART_FILE_INTF);
        } else
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", UART_FILE_INTF, obj2->objname);
        }
        return Not_found_exp;
    }

    return No_exp;
}

static exception_t uart_file_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    mpc5554_esci_device *dev =  obj->obj;
    *obj2 = dev->uart_file.obj;
    *port = NULL;
    return No_exp;
}

// 初始化模块类，并在SkyEye加载模块时执行以下代码
void init_mpc5554_esci(void)
{
    // 向SkyEye注册一个设备
    static skyeye_class_t class_data =
    {
        .class_name      = "mpc5554_esci",
        .class_desc      = "mpc5554_esci",
        .new_instance    = new_mpc5554_esci,
        .free_instance   = free_mpc5554_esci,
        .config_instance = config_mpc5554_esci,
        .module_type     = SKYML_OFFCHIP,
    };
    conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);

    // 设置寄存器地址读写方法
    static memory_space_intf io_memory_interface =
    {
        .read  = mpc5554_esci_read,
        .write = mpc5554_esci_write
    };
    SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

    // 注册访问接口(用于SkyEye图形控制接口)
    static skyeye_reg_intf reg_interface =
    {
        .get_regvalue_by_id  = mpc5554_esci_get_regvalue_by_id,
        .get_regname_by_id   = mpc5554_esci_get_regname_by_id,
        .set_regvalue_by_id  = mpc5554_esci_set_regvalue_by_id,
        .get_regnum          = mpc5554_esci_get_regnum,
        .get_regid_by_name   = mpc5554_esci_get_regid_by_name,
        .get_regoffset_by_id = mpc5554_esci_get_regoffset_by_id
    };
    SKY_register_iface(dev_class, SKYEYE_REG_INTF, &reg_interface);

    static esci_recvfrom_edma_intf esci_intf =
    {
        .recv_edma_trans_len = recv_edma_trans_len
    };
    SKY_register_iface(dev_class, ESCI_RECVFROM_EDMA, &esci_intf);

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
            .name  = ESCI_RECVFROM_EDMA,
            .iface = &esci_intf
        },
    };

    // 接口连接方法列表
    static const struct ConnectDescription connects[] =
    {
        /* ConnectDescription */
        (struct ConnectDescription )
        {
            .name = SKYEYE_UART_INTF,
            .set = uart_term_set,
            .get = uart_term_get,
        },
        (struct ConnectDescription )
        {
            .name = GENERAL_SIGNAL_INTF_NAME,
            .set = general_signal_set,
            .get = general_signal_get,
        },
        (struct ConnectDescription )
        {
            .name = UART_FILE_INTF,
            .set = uart_file_set,
            .get = uart_file_get,
        },
        (struct ConnectDescription)
        {
            .name = UART_COM_INTF,
            .set = uart_com_set,
            .get = uart_com_get,
        },
        (struct ConnectDescription)
        {
            .name = ESCI_SENDTO_EDMA,
            .set = edma_iface_set,
            .get = edma_iface_get,
        },
    };
    class_register_ifaces(dev_class, ifaces);
    class_register_connects(dev_class, connects);
}
