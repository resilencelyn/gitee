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
#include "mpc5554_edma.h"

void edma_send(conf_object_t* conf_object)
{

    mpc5554_edma_device *dev = (mpc5554_edma_device *)conf_object->obj;

    /* 开始DMA传输 */
    if ((dev->ENTxRxCH == 1)
       && (dev->regs.SERQR & CONFIG_DMA_TX))
    {
        uint8_t buf = 0;
        int i;
        uint32_t src_addr = dev->regs.TCD[CONFIG_DMA_TX].SADDR;         /* DMA 源地址 */
        uint32_t biter = dev->regs.TCD[CONFIG_DMA_TX].BITER.B.BITER;    /* DMA 开始的 minor loop 字节数 */
        uint32_t citer = dev->regs.TCD[CONFIG_DMA_TX].CITER.B.CITER;    /* DMA 当前的 minor loop 字节数 */
        uint32_t slast = dev->regs.TCD[CONFIG_DMA_TX].SLAST;            /* DMA 传输完成后对源地址的修正值 */
        uint32_t dlast = dev->regs.TCD[CONFIG_DMA_TX].DLAST_SGA;        /* DMA 传输完成后对源地址的修正值 */
        uint32_t nbytes = dev->regs.TCD[CONFIG_DMA_TX].NBYTES;          /* DMA 每次传输的字节数 */
        uint32_t dst_addr = dev->regs.TCD[CONFIG_DMA_TX].DADDR;         /* DMA 传输目的地址 */
        uint32_t tx_len = biter;

        /* 从源地址读取数据写入目的地址 */
        for (i = 0; i < tx_len; i++)
        {
            dev->memory_iface->read(dev->memory, src_addr + i, &buf, nbytes);
            dev->memory_iface->write(dev->memory, dst_addr, &buf, nbytes);
        }

        dev->regs.TCD[CONFIG_DMA_TX].CITER.B.CITER += citer;

        dev->regs.TCD[CONFIG_DMA_TX].BITER.B.DONE = 1;
        dev->regs.SERQR &= ~(CONFIG_DMA_TX);
    }
}

/* esci 接收虚拟串口数据后，EDMA接收数据缓冲区SIZE - 1 */
static int edma_recv_length(conf_object_t* conf_object, int length)
{
     mpc5554_edma_device *dev = (mpc5554_edma_device *)conf_object->obj;
     uint32_t biter;
     uint32_t citer;
     uint8_t buf;
     int i;

     dev->SciRecvCount ++;
     dev->MinorLoopBytes = dev->regs.TCD[CONFIG_DMA_RX].NBYTES;

     /* 通知串口接收到数据 */
     dev->esci_iface->recv_edma_trans_len(dev->esci, 1);

    /* DMA 搬数据 */
    if ((dev->ENTxRxCH == 1) && (dev->regs.ERQRL & EN_DMA_RX))
    {
        /* 一个minor loop 完成 */
        if (dev->SciRecvCount % dev->MinorLoopBytes == 0)
        {
            dev->SciRxDMA_src_addr = dev->regs.TCD[CONFIG_DMA_RX].SADDR;                    /* DMA 源地址 */
            dev->SciRxDMA_dst_addr = dev->regs.TCD[CONFIG_DMA_RX].DADDR;                  /* DMA 目的地址 */
            biter = dev->regs.TCD[CONFIG_DMA_RX].BITER.B.BITER;           /* DMA 开始的 minor loop 字节数 */
            citer = dev->regs.TCD[CONFIG_DMA_RX].CITER.B.CITER;
        }

        /* 根据DOFF，NBYTES，DSIZE 设置完成一次Minorloop后加上偏移的目标地址 目前DOFF为1 */
        dev->SciRxDMA_dst_addr =  dev->SciRxDMA_dst_addr + (biter - citer ) * (dev->MinorLoopBytes);

        /* 根据SOFF，NBYTES，SSIZE 设置完成一次Minorloop后加上偏移的源地址 目前SOFF为 0 */
        /////

        /* 搬运一个MinLoop 的数据 */
        for (i = 0; i < dev->MinorLoopBytes; i++)
        {
            dev->memory_iface->read(dev->memory, dev->SciRxDMA_src_addr, &buf, 1);
            //printf("0x%x",buf);
            dev->SciRxDMA_dst_addr += i;
            dev->memory_iface->write(dev->memory, dev->SciRxDMA_dst_addr, &buf, 1);
        }
        //printf("\n");

        dev->regs.TCD[CONFIG_DMA_RX].CITER.B.CITER -= 1;
        if (dev->regs.TCD[CONFIG_DMA_RX].CITER.B.CITER == 0)
        {
            //printf("[**ESCI --> EDMA**] CITER: 0x%x | BITER: 0x%x\n", dev->regs.TCD[CONFIG_DMA_RX].CITER.B.CITER,
            //       dev->regs.TCD[CONFIG_DMA_RX].BITER.B.BITER );
            dev->regs.TCD[CONFIG_DMA_RX].CITER.B.CITER = dev->regs.TCD[CONFIG_DMA_RX].BITER.B.BITER;
        }
    }

}

/*********************************************************
*1.esi使能DMA后通过send_flag 发送标志位
*2.设置RecvDMAConf 为1，用于接收最开始初始化的值
*3.完成后设置RecvDMAConf为0
*********************************************************/
static int en_edma_send_flag(conf_object_t* conf_object, int val)
{

    mpc5554_edma_device *dev = (mpc5554_edma_device *)conf_object->obj;

    switch(val)
    {
        case DMAAllEn:
            break;
        case DMARxEn:
            break;
        case DMATxEn:
            break;
        default:
            break;
    }

    /* ESCI 已经使能DMA接收和发送，DMA 准备接收配置项 */
    dev->ENTxRxCH = 1;
}

static void En_ERQRx_Bit(mpc5554_edma_device *dev, uint8_t bit)
{
    if(bit > 31)
    {
        dev->regs.ERQRH |= (1 << (bit - 32));
    } else
    {
        dev->regs.ERQRL |= (1 << bit);
    }
}

static void Dis_ERQRx_Bit(mpc5554_edma_device *dev, uint8_t bit)
{
    if(bit > 31)
    {
        dev->regs.ERQRH &= ~(1 << (bit - 32));
    } else
    {
        dev->regs.ERQRL &= ~(1 << bit);
    }
}

// 寄存器读取操作方法
static exception_t mpc5554_edma_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc5554_edma_device *dev = (mpc5554_edma_device *)conf_obj->obj;

    if(offset >= 0x100 && offset <= 0x13f)
    {
        uint8_t index = (offset - 0x100);
        *(uint8_t *)buf = dev->regs.CPR[index] & 0xff;

        //printf("[1]In %s | offset :  0x%x | val : 0x%x\n",__func__, offset, *(uint32_t *)buf);

        return No_exp;
    }

    if(offset >= 0x1000 && offset <= 0x17E0)
    {
        //printf("[2]In %s | offset :  0x%x | val : 0x%x\n",__func__, offset, *(uint32_t *)buf);

        uint32_t OffSet = (offset - 0x1000);
        uint32_t index = (OffSet / 0x20);
        uint32_t OFF = (OffSet % 0x20);
        uint32_t data = 0;

        switch (OFF)
        {
            case SADDR_OFF:
                data = dev->regs.TCD[index].SADDR;
                break;
            case MS_OFF:
            case MS_OFF + 0x1:
            case MS_OFF + 0x2:
            case MS_OFF + 0x3:
                data =  dev->regs.TCD[index].MS.R;
                break;
            case NBYTES_OFF:
                data =  dev->regs.TCD[index].NBYTES;
                break;
            case SLAST_OFF:
                data =  dev->regs.TCD[index].SLAST;
                break;
            case DADDR_OFF:
                data =  dev->regs.TCD[index].DADDR;
                break;
            case CITER_OFF:
            case CITER_OFF + 0x1:
            case CITER_OFF + 0x2:
            case CITER_OFF + 0x3:
                data =  dev->regs.TCD[index].CITER.R;
                break;
            case DLAST_SGA_OFF:
                data =  dev->regs.TCD[index].DLAST_SGA;
                break;
            case BITER_OFF:
            case BITER_OFF + 0x1:
            case BITER_OFF + 0x2:
            case BITER_OFF + 0x3:
                data =  dev->regs.TCD[index].BITER.R;
                break;
            default:
                printf("In %s, read error case : 0x%x\n", __FUNCTION__, OFF);
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
            *(uint32_t *)buf = data & 0xffffffff;
        }

        //printf("[3]In %s | offset :  0x%x | val : 0x%x\n",__func__, offset, *(uint32_t *)buf);

        return No_exp;
    }

    switch (offset)
    {
        case _CR:
            *(uint32_t *)buf = dev->regs.CR;
            break;
        case _ESR:
            *(uint32_t *)buf = dev->regs.ESR;
            break;
        case _ERQRH:
            *(uint32_t *)buf = dev->regs.ERQRH;
            break;
        case _ERQRL:
            *(uint32_t *)buf = dev->regs.ERQRL;
            break;
        case _EEIRH:
            *(uint32_t *)buf = dev->regs.EEIRH;
            break;
        case _EEIRL:
            *(uint32_t *)buf = dev->regs.EEIRL;
            break;
        case _SERQR:
            *(uint32_t *)buf = dev->regs.SERQR;
            break;
        case _CERQR:
            *(uint32_t *)buf = dev->regs.CERQR;
            break;
        case _SEEIR:
            *(uint32_t *)buf = dev->regs.SEEIR;
            break;
        case _CEEIR:
            *(uint32_t *)buf = dev->regs.CEEIR;
            break;
        case _CIRQR:
            *(uint32_t *)buf = dev->regs.CIRQR;
            break;
        case _CER:
            *(uint32_t *)buf = dev->regs.CER;
            break;
        case _SSBR:
            *(uint32_t *)buf = dev->regs.SSBR;
            break;
        case _CDSBR:
            *(uint32_t *)buf = dev->regs.CDSBR;
            break;
        case _IRQRH:
            *(uint32_t *)buf = dev->regs.IRQRH;
            break;
        case _IRQRL:
            *(uint32_t *)buf = dev->regs.IRQRL;
            break;
        case _ERH:
            *(uint32_t *)buf = dev->regs.ERH;
            break;
        case _ERL:
            *(uint32_t *)buf = dev->regs.ERL;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in mpc5554_edma\n", offset);
            break;
    }

    //printf("[4]In %s | offset :  0x%x | val : 0x%x\n",__func__, offset, *(uint32_t *)buf);

    return No_exp;
}

// 寄存器写入操作方法
static exception_t mpc5554_edma_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc5554_edma_device *dev = (mpc5554_edma_device *)conf_obj->obj;
    uint32_t val = *(uint32_t *)buf;

    //printf("In %s | offset :  0x%x | val : 0x%x\n",__func__, offset, val);

    if(offset >= 0x100 && offset <= 0x13f)
    {
        uint8_t index = (offset - 0x100);
        dev->regs.CPR[index] = val & 0xff;

        return No_exp;
    }

    if(offset >= 0x1000 && offset <= 0x17E0)
    {
        uint32_t OffSet = (offset - 0x1000);
        uint32_t index = (OffSet / 0x20);
        uint32_t OFF = (OffSet % 0x20);
        uint32_t data = 0;

        switch (OFF)
        {
            case SADDR_OFF:
                dev->regs.TCD[index].SADDR = val;
                break;
            case MS_OFF:
                if(val & 0x7)
                {
                    dev->regs.TCD[index].MS.B.SSIZE = val;
                }
                if(val & 0xf8)
                {
                    dev->regs.TCD[index].MS.B.SMOD = val >> 3;
                }
                break;
            case MS_OFF + 0x1:
                if(val & 0x7)
                {
                    dev->regs.TCD[index].MS.B.DSIZE = val;
                }
                if(val & 0xf8)
                {
                    dev->regs.TCD[index].MS.B.DMOD = val >> 3;
                }
                break;
            case MS_OFF + 0x2:
                dev->regs.TCD[index].MS.B.SOFF = val;
                break;
            case NBYTES_OFF:
                dev->regs.TCD[index].NBYTES = val;
                break;
            case SLAST_OFF:
                dev->regs.TCD[index].SLAST = val;
                break;
            case DADDR_OFF:
                dev->regs.TCD[index].DADDR = val;
                break;
            case CITER_OFF:
                if(val & 0x8000)
                {
                     dev->regs.TCD[index].CITER.B.CITERE_LINK = 1;
                }
                if(val & 0x7fff)
                {
                     dev->regs.TCD[index].CITER.B.CITER = val;
                }
                break;
            case CITER_OFF + 0x2:
                 dev->regs.TCD[index].CITER.B.DOFF = val;
                 break;
            case DLAST_SGA_OFF:
                 dev->regs.TCD[index].DLAST_SGA = val;
                break;
            case BITER_OFF:
                if(val & 0x80)
                {
                    dev->regs.TCD[index].CITER.B.CITERE_LINK = (val > 0x15);
                }
                if(val & 0x7fff)
                {
                     dev->regs.TCD[index].BITER.B.BITER = val;
                }
                break;
            case BITER_OFF + 0x2:
                if(val & 0xc000)
                {
                    dev->regs.TCD[index].BITER.B.BWC = 2;
                }
                break;
            case BITER_OFF + 0x3:
                if(val & 0x80)
                {
                    dev->regs.TCD[index].BITER.B.DONE = 1;
                }
                if(val & 0x8)
                {
                    dev->regs.TCD[index].BITER.B.D_REQ = 1;
                }
                break;
            default:
                printf("In %s, write error case : 0x%x\n", __FUNCTION__, OFF);
                break;
        }
        return No_exp;
    }

    switch (offset)
    {
        case _CR:
             dev->regs.CR = val;
             break;
        case _ESR:
            dev->regs.ESR = val;
            break;
        case _ERQRH:
            dev->regs.ERQRH = val;
            break;
        case _ERQRL:
            dev->regs.ERQRL = val;
            break;
        case _EEIRH:
            dev->regs.EEIRH = val;
            break;
        case _EEIRL:
            dev->regs.EEIRL = val;
            break;
        case _SERQR:
            dev->regs.SERQR = val & 0xff;
            En_ERQRx_Bit(dev, dev->regs.SERQR);
            if(dev->regs.SERQR & CONFIG_DMA_RX)
            {
                dev->RxEnable = 1;
            }
            edma_send(conf_obj);
            break;
        case _CERQR:
            dev->regs.CERQR = val & 0xff;
            break;
        case _SEEIR:
            dev->regs.SEEIR = val & 0xff;
            break;
        case _CEEIR:
            dev->regs.CEEIR = val & 0xff;
            break;
        case _CIRQR:
            dev->regs.CIRQR = val & 0xff;
            break;
        case _CER:
            dev->regs.CER = val & 0xff ;
            break;
        case _SSBR:
            dev->regs.SSBR = val & 0xff;
            break;
        case _CDSBR:
            dev->regs.CDSBR = val & 0xff;
            break;
        case _IRQRH:
            dev->regs.IRQRH = val;
            break;
        case _IRQRL:
            dev->regs.IRQRL = val;
            break;
        case _ERH:
            dev->regs.ERH = val;
            break;
        case _ERL:
            dev->regs.ERL = val;
             break;
        default:
                skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in mpc5554_edma\n", offset);
                break;
    }
    return No_exp;
}

// 寄存器接口:根据寄存器ID获取寄存器名
static char* mpc5554_edma_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_name[id];
}

// 寄存器接口:根据寄存器ID设置寄存器值
static exception_t mpc5554_edma_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    mpc5554_edma_device *dev = (mpc5554_edma_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    *((uint32_t*)regs + id) = value;

    return No_exp;
}

// 寄存器接口:获取设备有多少寄存器
static uint32_t mpc5554_edma_get_regnum(conf_object_t *conf_obj)
{
    return 18;
}

// 寄存器接口:根据寄存器ID获取寄存器值
static uint32_t mpc5554_edma_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    mpc5554_edma_device *dev = (mpc5554_edma_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    uint32_t* regs_value = (uint32_t*)regs + id;

    return *regs_value;
}

// 寄存器接口:根据寄存器名获取寄存器ID
static uint32_t mpc5554_edma_get_regid_by_name(conf_object_t *conf_obj, char *name)
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
static uint32_t mpc5554_edma_get_regoffset_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_offset[id];
}

// 实例化一个新设备对象，obj_name是在json文件中配置的名称
static conf_object_t* new_mpc5554_edma(const char *obj_name)
{
    mpc5554_edma_device* dev = skyeye_mm_zero(sizeof(mpc5554_edma_device));
    dev->obj = new_conf_object(obj_name, dev);
    // TODO: 在此添加自定义源码

    // 寄存器初始化
    struct registers *regs = &(dev->regs);
    regs->CR = 0xe400;
    regs->ESR = 0;
    regs->ERQRH = 0;
    regs->ERQRL = 0;
    regs->EEIRH = 0;
    regs->EEIRL = 0;
    regs->SERQR = 0;
    regs->CERQR = 0;
    regs->SEEIR = 0;
    regs->CEEIR = 0;
    regs->CIRQR = 0;
    regs->CER = 0;
    regs->SSBR = 0;
    regs->CDSBR = 0;
    regs->IRQRH = 0;
    regs->IRQRL = 0;
    regs->ERH = 0;
    regs->ERL = 0;

    dev->ENTxRxCH = 1;
    dev->DMAReadLength = -1;
    dev->DMAReadFlag = -1;

    dev->sciFrameLen = 16;
    dev->sciRecvMaxLen = 4096;

    return dev->obj;
}

// 配置实例化的设备对象
static exception_t config_mpc5554_edma(conf_object_t* conf_obj)
{
    mpc5554_edma_device *dev = (mpc5554_edma_device *)conf_obj->obj;
    // TODO: 在此添加自定义源码

    return No_exp;
}

// 释放设备对象来做一些内存释放
static exception_t free_mpc5554_edma(conf_object_t* conf_obj)
{
    mpc5554_edma_device *dev = (mpc5554_edma_device *)conf_obj->obj;

    skyeye_free(conf_obj->objname);
    skyeye_free(conf_obj->obj);
    skyeye_free(conf_obj);

    return No_exp;
}

/* 中断控制器接口配置 */
static exception_t general_memory_space_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc5554_edma_device *dev = obj->obj;
    dev->memory = obj2;
    dev->memory_iface = (memory_space_intf *)SKY_get_iface(dev->memory, MEMORY_SPACE_INTF_NAME);
    if (dev->memory_iface == NULL)
    {
        if ( dev->memory_iface == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", MEMORY_SPACE_INTF_NAME);
        } else
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", MEMORY_SPACE_INTF_NAME, obj2->objname);
        }

        return Not_found_exp;
    }

    return No_exp;
}

static exception_t general_memory_space_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    mpc5554_edma_device *dev = obj->obj;
    *obj2 = dev->memory;
    *port = NULL;
    return No_exp;
}

static exception_t esci_iface_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc5554_edma_device *dev = obj->obj;
    dev->esci = obj2;
    dev->esci_iface = (memory_space_intf *)SKY_get_iface(dev->memory, ESCI_RECVFROM_EDMA);
    if (dev->esci == NULL)
    {
        if ( dev->esci_iface == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", ESCI_RECVFROM_EDMA);
        } else
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", ESCI_RECVFROM_EDMA, obj2->objname);
        }

        return Not_found_exp;
    }

    return No_exp;
}

static exception_t esci_iface_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    mpc5554_edma_device *dev = obj->obj;
    *obj2 = dev->esci;
    *port = NULL;
    return No_exp;
}

/* 中断控制器接口配置 */
static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc5554_edma_device *dev =  obj->obj;
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
    mpc5554_edma_device *dev = obj->obj;
    *obj2 = dev->general_signal.obj;
    *port = NULL;
    return No_exp;
}

/* 设置属性 */
static exception_t set_attr_sciFrameLen(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    mpc5554_edma_device *dev = (mpc5554_edma_device *)conf_obj->obj;
    dev->sciFrameLen = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_sciFrameLen(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    mpc5554_edma_device *dev = (mpc5554_edma_device *)conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(dev->sciFrameLen);
    return value;
}

static exception_t set_attr_sciRecvMaxLen(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    mpc5554_edma_device *dev =  conf_obj->obj;
    dev->sciRecvMaxLen = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_sciRecvMaxLen(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    mpc5554_edma_device *dev =  conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(dev->sciRecvMaxLen);
    return value;
}

static void class_register_attribute(conf_class_t* dev_class)
{
    SKY_register_attribute(dev_class, "sciFrameLen", get_attr_sciFrameLen, NULL, set_attr_sciFrameLen, NULL, SKY_Attr_Optional, "uinteger", "this is set sciFrameLen describe");
    SKY_register_attribute(dev_class, "sciRecvMaxLen", get_attr_sciRecvMaxLen, NULL, set_attr_sciRecvMaxLen, NULL, SKY_Attr_Optional, "uinteger", "this is set sciRecvMaxLen describe");
    return;
}


// 初始化模块类，并在SkyEye加载模块时执行以下代码
void init_mpc5554_edma(void)
{
    // 向SkyEye注册一个设备
    static skyeye_class_t class_data =
    {
        .class_name      = "mpc5554_edma",
        .class_desc      = "mpc5554_edma",
        .new_instance    = new_mpc5554_edma,
        .free_instance   = free_mpc5554_edma,
        .config_instance = config_mpc5554_edma,
        .module_type     = SKYML_OFFCHIP,
    };
    conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);

    // 设置寄存器地址读写方法
    static memory_space_intf io_memory_interface =
    {
        .read  = mpc5554_edma_read,
        .write = mpc5554_edma_write
    };
    SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

    // 注册访问接口(用于SkyEye图形控制接口)
    static skyeye_reg_intf reg_interface =
    {
        .get_regvalue_by_id  = mpc5554_edma_get_regvalue_by_id,
        .get_regname_by_id   = mpc5554_edma_get_regname_by_id,
        .set_regvalue_by_id  = mpc5554_edma_set_regvalue_by_id,
        .get_regnum          = mpc5554_edma_get_regnum,
        .get_regid_by_name   = mpc5554_edma_get_regid_by_name,
        .get_regoffset_by_id = mpc5554_edma_get_regoffset_by_id
    };
    SKY_register_iface(dev_class, SKYEYE_REG_INTF, &reg_interface);

    static esci_sendto_edma_intf edma_intf =
    {
        .send_flag = en_edma_send_flag,
        .recv_length = edma_recv_length
    };
    SKY_register_iface(dev_class, ESCI_SENDTO_EDMA, &edma_intf);

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
            .name  = ESCI_SENDTO_EDMA,
            .iface = &edma_intf
        }
    };

    // 接口连接方法列表
    static const struct ConnectDescription connects[] =
    {
        /* ConnectDescription */
        (struct ConnectDescription )
        {
            .name = GENERAL_SIGNAL_INTF_NAME,
            .set = general_signal_set,
            .get = general_signal_get,
        },
        (struct ConnectDescription )
        {
            .name = ESCI_RECVFROM_EDMA,
            .set = esci_iface_set,
            .get = esci_iface_get,
        },
        (struct ConnectDescription)
        {
            .name = MEMORY_SPACE_INTF_NAME,
            .set = general_memory_space_set,
            .get = general_memory_space_get,
        }
    };
    class_register_ifaces(dev_class, ifaces);
    class_register_connects(dev_class, connects);
}
