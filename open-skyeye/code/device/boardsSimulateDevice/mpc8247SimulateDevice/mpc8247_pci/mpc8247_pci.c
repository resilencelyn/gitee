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
#include "mpc8247_pci.h"

// 寄存器读取操作方法
static exception_t mpc8247_pci_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc8247_pci_device *dev = (mpc8247_pci_device *)conf_obj->obj;

    switch (offset)
    {
        case _OMISR:
            *(uint32_t *)buf = dev->regs.OMISR;
            break;
        case _OMIMR:
            *(uint32_t *)buf = dev->regs.OMIMR;
            break;
        case _IFQPR:
            *(uint32_t *)buf = dev->regs.IFQPR;
            break;
        case _OFQPR:
            *(uint32_t *)buf = dev->regs.OFQPR;
            break;
        case _IMR0:
            *(uint32_t *)buf = dev->regs.IMR0;
            break;
        case _IMR1:
            *(uint32_t *)buf = dev->regs.IMR1;
            break;
        case _OMR0:
            *(uint32_t *)buf = dev->regs.OMR0;
            break;
        case _OMR1:
            *(uint32_t *)buf = dev->regs.OMR1;
            break;
        case _ODR:
            *(uint32_t *)buf = dev->regs.ODR;
            break;
        case _IDR:
            *(uint32_t *)buf = dev->regs.IDR;
            break;
        case _IMISR:
            *(uint32_t *)buf = dev->regs.IMISR;
            break;
        case _IMIMR:
            *(uint32_t *)buf = dev->regs.IMIMR;
            break;
        case _IFHPR:
            *(uint32_t *)buf = dev->regs.IFHPR;
            break;
        case _IFTPR:
            *(uint32_t *)buf = dev->regs.IFTPR;
            break;
        case _IPHPR:
            *(uint32_t *)buf = dev->regs.IPHPR;
            break;
        case _IPTPR:
            *(uint32_t *)buf = dev->regs.IPTPR;
            break;
        case _OFHPR:
            *(uint32_t *)buf = dev->regs.OFHPR;
            break;
        case _OFTPR:
            *(uint32_t *)buf = dev->regs.OFTPR;
            break;
        case _OPHPR:
            *(uint32_t *)buf = dev->regs.OPHPR;
            break;
        case _OPTPR:
            *(uint32_t *)buf = dev->regs.OPTPR;
            break;
        case _MUCR:
            *(uint32_t *)buf = dev->regs.MUCR;
            break;
        case _QBAR:
            *(uint32_t *)buf = dev->regs.QBAR;
            break;
        case _DMAMR0:
            *(uint32_t *)buf = dev->regs.DMAMR[0];
            break;
        case _DMAMR1:
            *(uint32_t *)buf = dev->regs.DMAMR[1];
            break;
        case _DMAMR2:
            *(uint32_t *)buf = dev->regs.DMAMR[2];
            break;
        case _DMAMR3:
            *(uint32_t *)buf = dev->regs.DMAMR[3];
            break;
        case _DMASR0:
            *(uint32_t *)buf = dev->regs.DMASR[0];
            break;
        case _DMASR1:
            *(uint32_t *)buf = dev->regs.DMASR[1];
            break;
        case _DMASR2:
            *(uint32_t *)buf = dev->regs.DMASR[2];
            break;
        case _DMASR3:
            *(uint32_t *)buf = dev->regs.DMASR[3];
            break;
        case _DMACDAR0:
            *(uint32_t *)buf = dev->regs.DMACDAR[0];
            break;
        case _DMACDAR1:
            *(uint32_t *)buf = dev->regs.DMACDAR[1];
            break;
        case _DMACDAR2:
            *(uint32_t *)buf = dev->regs.DMACDAR[2];
            break;
        case _DMACDAR3:
            *(uint32_t *)buf = dev->regs.DMACDAR[3];
            break;
        case _DMASAR0:
            *(uint32_t *)buf = dev->regs.DMASAR[0];
            break;
        case _DMASAR1:
            *(uint32_t *)buf = dev->regs.DMASAR[1];
            break;
        case _DMASAR2:
            *(uint32_t *)buf = dev->regs.DMASAR[2];
            break;
        case _DMASAR3:
            *(uint32_t *)buf = dev->regs.DMASAR[3];
            break;
        case _DMADAR0:
            *(uint32_t *)buf = dev->regs.DMADAR[0];
            break;
        case _DMADAR1:
            *(uint32_t *)buf = dev->regs.DMADAR[1];
            break;
        case _DMADAR2:
            *(uint32_t *)buf = dev->regs.DMADAR[2];
            break;
        case _DMADAR3:
            *(uint32_t *)buf = dev->regs.DMADAR[3];
            break;
        case _DMABCR0:
            *(uint32_t *)buf = dev->regs.DMABCR[0];
            break;
        case _DMABCR1:
            *(uint32_t *)buf = dev->regs.DMABCR[1];
            break;
        case _DMABCR2:
            *(uint32_t *)buf = dev->regs.DMABCR[2];
            break;
        case _DMABCR3:
            *(uint32_t *)buf = dev->regs.DMABCR[3];
            break;
        case _DMANDAR0:
            *(uint32_t *)buf = dev->regs.DMANDAR[0];
            break;
        case _DMANDAR1:
            *(uint32_t *)buf = dev->regs.DMANDAR[1];
            break;
        case _DMANDAR2:
            *(uint32_t *)buf = dev->regs.DMANDAR[2];
            break;
        case _DMANDAR3:
            *(uint32_t *)buf = dev->regs.DMANDAR[3];
            break;
        case _POTAR0:
            *(uint32_t *)buf = dev->regs.POTAR[0];
            break;
        case _POTAR1:
            *(uint32_t *)buf = dev->regs.POTAR[1];
            break;
        case _POTAR2:
            *(uint32_t *)buf = dev->regs.POTAR[2];
            break;
        case _POBAR0:
            *(uint32_t *)buf = dev->regs.POBAR[0];
            break;
        case _POBAR1:
            *(uint32_t *)buf = dev->regs.POBAR[1];
            break;
        case _POBAR2:
            *(uint32_t *)buf = dev->regs.POBAR[2];
            break;
        case _POCMR0:
            *(uint32_t *)buf = dev->regs.POCMR[0];
            break;
        case _POCMR1:
            *(uint32_t *)buf = dev->regs.POCMR[1];
            break;
        case _POCMR2:
            *(uint32_t *)buf = dev->regs.POCMR[2];
            break;
        case _PTCR:
            *(uint32_t *)buf = dev->regs.PTCR;
            break;
        case _GPCR:
            *(uint32_t *)buf = dev->regs.GPCR;
            break;
        case _PCI_GCR:
            *(uint32_t *)buf = dev->regs.PCI_GCR;
            break;
        case _ESR:
            *(uint32_t *)buf = dev->regs.ESR;
            break;
        case _EMR:
            *(uint32_t *)buf = dev->regs.EMR;
            break;
        case _ECR:
            *(uint32_t *)buf = dev->regs.ECR;
            break;
        case _PCI_EACR:
            *(uint32_t *)buf = dev->regs.PCI_EACR;
            break;
        case _PCI_EDCR:
            *(uint32_t *)buf = dev->regs.PCI_EDCR;
            break;
        case _PCI_ECCR:
            *(uint32_t *)buf = dev->regs.PCI_ECCR;
            break;
        case _PITAR1:
            *(uint32_t *)buf = dev->regs.PITAR1;
            break;
        case _PIBAR1:
            *(uint32_t *)buf = dev->regs.PIBAR1;
            break;
        case _PICMR1:
            *(uint32_t *)buf = dev->regs.PICMR1;
            break;
        case _PITAR0:
            *(uint32_t *)buf = dev->regs.PITAR0;
            break;
        case _PIBAR0:
            *(uint32_t *)buf = dev->regs.PIBAR0;
            break;
        case _PICMR0:
            *(uint32_t *)buf = dev->regs.PICMR0;
            break;
        case _CFG_ADDR:
            *(uint32_t *)buf = dev->regs.CFG_ADDR;
            break;
        case _CFG_DATA:
            *(uint32_t *)buf = dev->regs.CFG_DATA;
            break;
        case _INT_ACK:
            *(uint32_t *)buf = dev->regs.INT_ACK;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in mpc8247_pci\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器写入操作方法
static exception_t mpc8247_pci_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc8247_pci_device *dev = (mpc8247_pci_device *)conf_obj->obj;
    uint32_t val = *(uint32_t *)buf;

    switch (offset)
    {
        case _OMISR:
            dev->regs.OMISR = val;
            break;
        case _OMIMR:
            dev->regs.OMIMR = val;
            break;
        case _IFQPR:
            dev->regs.IFQPR = val;
            break;
        case _OFQPR:
            dev->regs.OFQPR = val;
            break;
        case _IMR0:
            dev->regs.IMR0 = val;
            break;
        case _IMR1:
            dev->regs.IMR1 = val;
            break;
        case _OMR0:
            dev->regs.OMR0 = val;
            break;
        case _OMR1:
            dev->regs.OMR1 = val;
            break;
        case _ODR:
            dev->regs.ODR = val;
            break;
        case _IDR:
            dev->regs.IDR = val;
            break;
        case _IMISR:
            dev->regs.IMISR = val;
            break;
        case _IMIMR:
            dev->regs.IMIMR = val;
            break;
        case _IFHPR:
            dev->regs.IFHPR = val;
            break;
        case _IFTPR:
            dev->regs.IFTPR = val;
            break;
        case _IPHPR:
            dev->regs.IPHPR = val;
            break;
        case _IPTPR:
            dev->regs.IPTPR = val;
            break;
        case _OFHPR:
            dev->regs.OFHPR = val;
            break;
        case _OFTPR:
            dev->regs.OFTPR = val;
            break;
        case _OPHPR:
            dev->regs.OPHPR = val;
            break;
        case _OPTPR:
            dev->regs.OPTPR = val;
            break;
        case _MUCR:
            dev->regs.MUCR = val;
            break;
        case _QBAR:
            dev->regs.QBAR = val;
            break;
        case _DMAMR0:
            dev->regs.DMAMR[0] = val;
            break;
        case _DMAMR1:
            dev->regs.DMAMR[1] = val;
            break;
        case _DMAMR2:
            dev->regs.DMAMR[2] = val;
            break;
        case _DMAMR3:
            dev->regs.DMAMR[3] = val;
            break;
        case _DMASR0:
            dev->regs.DMASR[0] = val;
            break;
        case _DMASR1:
            dev->regs.DMASR[1] = val;
            break;
        case _DMASR2:
            dev->regs.DMASR[2] = val;
            break;
        case _DMASR3:
            dev->regs.DMASR[3] = val;
            break;
        case _DMACDAR0:
            dev->regs.DMACDAR[0] = val;
            break;
        case _DMACDAR1:
            dev->regs.DMACDAR[1] = val;
            break;
        case _DMACDAR2:
            dev->regs.DMACDAR[2] = val;
            break;
        case _DMACDAR3:
            dev->regs.DMACDAR[3] = val;
            break;
        case _DMASAR0:
            dev->regs.DMASAR[0] = val;
            break;
        case _DMASAR1:
            dev->regs.DMASAR[1] = val;
            break;
        case _DMASAR2:
            dev->regs.DMASAR[2] = val;
            break;
        case _DMASAR3:
            dev->regs.DMASAR[3] = val;
            break;
        case _DMADAR0:
            dev->regs.DMADAR[0] = val;
            break;
        case _DMADAR1:
            dev->regs.DMADAR[1] = val;
            break;
        case _DMADAR2:
            dev->regs.DMADAR[2] = val;
            break;
        case _DMADAR3:
            dev->regs.DMADAR[3] = val;
            break;
        case _DMABCR0:
            dev->regs.DMABCR[0] = val;
            break;
        case _DMABCR1:
            dev->regs.DMABCR[1] = val;
            break;
        case _DMABCR2:
            dev->regs.DMABCR[2] = val;
            break;
        case _DMABCR3:
            dev->regs.DMABCR[3] = val;
            break;
        case _DMANDAR0:
            dev->regs.DMANDAR[0] = val;
            break;
        case _DMANDAR1:
            dev->regs.DMANDAR[1] = val;
            break;
        case _DMANDAR2:
            dev->regs.DMANDAR[2] = val;
            break;
        case _DMANDAR3:
            dev->regs.DMANDAR[3] = val;
            break;
        case _POTAR0:
            dev->regs.POTAR[0] = val;
            break;
        case _POTAR1:
            dev->regs.POTAR[1] = val;
            break;
        case _POTAR2:
            dev->regs.POTAR[2] = val;
            break;
        case _POBAR0:
            dev->regs.POBAR[0] = val;
            break;
        case _POBAR1:
            dev->regs.POBAR[1] = val;
            break;
        case _POBAR2:
            dev->regs.POBAR[2] = val;
            break;
        case _POCMR0:
            dev->regs.POCMR[0] = val;
            break;
        case _POCMR1:
            dev->regs.POCMR[1] = val;
            break;
        case _POCMR2:
            dev->regs.POCMR[2] = val;
            break;
        case _PTCR:
            dev->regs.PTCR = val;
            break;
        case _GPCR:
            dev->regs.GPCR = val;
            break;
        case _PCI_GCR:
            dev->regs.PCI_GCR = val;
            break;
        case _ESR:
            dev->regs.ESR = val;
            break;
        case _EMR:
            dev->regs.EMR = val;
            break;
        case _ECR:
            dev->regs.ECR = val;
            break;
        case _PCI_EACR:
            dev->regs.PCI_EACR = val;
            break;
        case _PCI_EDCR:
            dev->regs.PCI_EDCR = val;
            break;
        case _PCI_ECCR:
            dev->regs.PCI_ECCR = val;
            break;
        case _PITAR1:
            dev->regs.PITAR1 = val;
            break;
        case _PIBAR1:
            dev->regs.PIBAR1 = val;
            break;
        case _PICMR1:
            dev->regs.PICMR1 = val;
            break;
        case _PITAR0:
            dev->regs.PITAR0 = val;
            break;
        case _PIBAR0:
            dev->regs.PIBAR0 = val;
            break;
        case _PICMR0:
            dev->regs.PICMR0 = val;
            break;
        case _CFG_ADDR:
            dev->regs.CFG_ADDR = val;
            break;
        case _CFG_DATA:
            dev->regs.CFG_DATA = val;
            break;
        case _INT_ACK:
            dev->regs.INT_ACK = val;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in mpc8247_pci\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器接口:根据寄存器ID获取寄存器名
static char* mpc8247_pci_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_name[id];
}

// 寄存器接口:根据寄存器ID设置寄存器值
static exception_t mpc8247_pci_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    mpc8247_pci_device *dev = (mpc8247_pci_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    *((uint32_t*)regs + id) = value;

    return No_exp;
}

// 寄存器接口:获取设备有多少寄存器
static uint32_t mpc8247_pci_get_regnum(conf_object_t *conf_obj)
{
    return sizeof(struct registers) / 4;
}

// 寄存器接口:根据寄存器ID获取寄存器值
static uint32_t mpc8247_pci_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    mpc8247_pci_device *dev = (mpc8247_pci_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    uint32_t* regs_value = (uint32_t*)regs + id;

    return *regs_value;
}

// 寄存器接口:根据寄存器名获取寄存器ID
static uint32_t mpc8247_pci_get_regid_by_name(conf_object_t *conf_obj, char *name)
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
static uint32_t mpc8247_pci_get_regoffset_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_offset[id];
}

// 实例化一个新设备对象，obj_name是在json文件中配置的名称
static conf_object_t* new_mpc8247_pci(const char *obj_name)
{
    mpc8247_pci_device* dev = skyeye_mm_zero(sizeof(mpc8247_pci_device));
    dev->obj = new_conf_object(obj_name, dev);
    // TODO: 在此添加自定义源码

    // 寄存器初始化
    struct registers *regs = &(dev->regs);
    regs->OMISR = 0;
    regs->OMIMR = 0;
    regs->IFQPR = 0;
    regs->OFQPR = 0;
    regs->IMR0 = 0;
    regs->IMR1 = 0;
    regs->OMR0 = 0;
    regs->OMR1 = 0;
    regs->ODR = 0;
    regs->IDR = 0;
    regs->IMISR = 0;
    regs->IMIMR = 0;
    regs->IFHPR = 0;
    regs->IFTPR = 0;
    regs->IPHPR = 0;
    regs->IPTPR = 0;
    regs->OFHPR = 0;
    regs->OFTPR = 0;
    regs->OPHPR = 0;
    regs->OPTPR = 0;
    regs->MUCR = 0;
    regs->QBAR = 0;
    regs->DMAMR[0] = 0;
    regs->DMAMR[1] = 0;
    regs->DMAMR[2] = 0;
    regs->DMAMR[3] = 0;
    regs->DMASR[0] = 0;
    regs->DMASR[1] = 0;
    regs->DMASR[2] = 0;
    regs->DMASR[3] = 0;
    regs->DMACDAR[0] = 0;
    regs->DMACDAR[1] = 0;
    regs->DMACDAR[2] = 0;
    regs->DMACDAR[3] = 0;
    regs->DMASAR[0] = 0;
    regs->DMASAR[1] = 0;
    regs->DMASAR[2] = 0;
    regs->DMASAR[3] = 0;
    regs->DMADAR[0] = 0;
    regs->DMADAR[1] = 0;
    regs->DMADAR[2] = 0;
    regs->DMADAR[3] = 0;
    regs->DMABCR[0] = 0;
    regs->DMABCR[1] = 0;
    regs->DMABCR[2] = 0;
    regs->DMABCR[3] = 0;
    regs->DMANDAR[0] = 0;
    regs->DMANDAR[1] = 0;
    regs->DMANDAR[2] = 0;
    regs->DMANDAR[3] = 0;
    regs->POTAR[0] = 0;
    regs->POTAR[1] = 0;
    regs->POTAR[2] = 0;
    regs->POBAR[0] = 0;
    regs->POBAR[1] = 0;
    regs->POBAR[2] = 0;
    regs->POCMR[0] = 0;
    regs->POCMR[1] = 0;
    regs->POCMR[2] = 0;
    regs->PTCR = 0;
    regs->GPCR = 0;
    regs->PCI_GCR = 0;
    regs->ESR = 0;
    regs->EMR = 4095;
    regs->ECR = 255;
    regs->PCI_EACR = 0;
    regs->PCI_EDCR = 0;
    regs->PCI_ECCR = 0;
    regs->PITAR1 = 0;
    regs->PIBAR1 = 0;
    regs->PICMR1 = 0;
    regs->PITAR0 = 0;
    regs->PIBAR0 = 0;
    regs->PICMR0 = 0;
    regs->CFG_ADDR = 0;
    regs->CFG_DATA = 0;
    regs->INT_ACK = 0;
    // 属性初始化
    dev->intNumber = 0;

    return dev->obj;
}

// 配置实例化的设备对象
static exception_t config_mpc8247_pci(conf_object_t* conf_obj)
{
    mpc8247_pci_device *dev = (mpc8247_pci_device *)conf_obj->obj;
    // TODO: 在此添加自定义源码

    return No_exp;
}

// 释放设备对象来做一些内存释放
static exception_t free_mpc8247_pci(conf_object_t* conf_obj)
{
    mpc8247_pci_device *dev = (mpc8247_pci_device *)conf_obj->obj;

    skyeye_free(conf_obj->objname);
    skyeye_free(conf_obj->obj);
    skyeye_free(conf_obj);

    return No_exp;
}

// 系统设置设备 core_signal 接口功能
static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc8247_pci_device *dev = (mpc8247_pci_device *)obj->obj;

    dev->core_signal.obj = obj2;
    if (obj2 == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from NULL object fail.\n", CORE_SIGNAL_INTF_NAME);
        return Not_found_exp;
    }
    dev->core_signal.iface = (core_signal_intf *)SKY_get_iface(obj2, CORE_SIGNAL_INTF_NAME);
    if (dev->core_signal.iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from %s fail.\n", CORE_SIGNAL_INTF_NAME, obj2->objname);
        return Not_found_exp;
    }

    return No_exp;
}

// 系统获取设备 core_signal 接口功能
static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    mpc8247_pci_device *dev = (mpc8247_pci_device *)obj->obj;

    *obj2 = dev->core_signal.obj;
    *port = NULL;

    return No_exp;
}

// 系统设置设备 intNumber 属性
static exception_t set_attr_intNumber(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    mpc8247_pci_device *dev = (mpc8247_pci_device *)conf_obj->obj;

    dev->intNumber = SKY_attr_uinteger(*value);

    return No_exp;
}

// 系统获取设备 intNumber 属性
static attr_value_t get_attr_intNumber(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    mpc8247_pci_device *dev = (mpc8247_pci_device *)conf_obj->obj;

    attr_value_t value = SKY_make_attr_uinteger(dev->intNumber);

    return value;
}

// 注册设备模块属性
static void class_register_attribute(conf_class_t* dev_class)
{
    SKY_register_attribute(dev_class,
            "intNumber",
            get_attr_intNumber, NULL,
            set_attr_intNumber, NULL,
            SKY_Attr_Optional,
            "uinteger",
            "this is intNumber describe");

    return;
}

// 初始化模块类，并在SkyEye加载模块时执行以下代码
void init_mpc8247_pci(void)
{
    // 向SkyEye注册一个设备
    static skyeye_class_t class_data =
    {
        .class_name      = "mpc8247_pci",
        .class_desc      = "mpc8247_pci",
        .new_instance    = new_mpc8247_pci,
        .free_instance   = free_mpc8247_pci,
        .config_instance = config_mpc8247_pci,
        .module_type     = SKYML_OFFCHIP,
    };
    conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);

    // 设置寄存器地址读写方法
    static memory_space_intf io_memory_interface =
    {
        .read  = mpc8247_pci_read,
        .write = mpc8247_pci_write
    };
    SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

    // 注册访问接口(用于SkyEye图形控制接口)
    static skyeye_reg_intf reg_interface =
    {
        .get_regvalue_by_id  = mpc8247_pci_get_regvalue_by_id,
        .get_regname_by_id   = mpc8247_pci_get_regname_by_id,
        .set_regvalue_by_id  = mpc8247_pci_set_regvalue_by_id,
        .get_regnum          = mpc8247_pci_get_regnum,
        .get_regid_by_name   = mpc8247_pci_get_regid_by_name,
        .get_regoffset_by_id = mpc8247_pci_get_regoffset_by_id
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
        (struct ConnectDescription)
        {
            .name = CORE_SIGNAL_INTF_NAME,
            .set  = core_signal_set,
            .get  = core_signal_get
        },
    };
    class_register_ifaces(dev_class, ifaces);
    class_register_connects(dev_class, connects);
    class_register_attribute(dev_class);
}
