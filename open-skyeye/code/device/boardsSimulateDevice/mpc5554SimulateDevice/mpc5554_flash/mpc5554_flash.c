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
#include "mpc5554_flash.h"


int flash_erase( mpc5554_flash_device *dev)
{
    int i,j;
    uint8_t nbyte = 4;
    uint32_t val = 0xffffffff;

    if (dev->fp != NULL &dev->memory != NULL)
    {
        if (dev->memory_iface != NULL)
        {
            for (i=0; i<SecNum; i++)
            {
                for (j=0; j<MAS_SEC_SIZE; j+=4)
                {
                     dev->memory_iface->write(dev->memory, SecAddr + MAS_SEC_SIZE * i + j, &val, nbyte);
                     fwrite(&val, 1, 4, dev->fp);
                }
            }
            return 0;
        } else
        {
            return -1;
        }
    } else
    {
        if (dev->fp == NULL)
        {
            return -2;
        } else
        {
            return -3;
        }
    }
}

// 寄存器读取操作方法
static exception_t mpc5554_flash_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc5554_flash_device *dev = (mpc5554_flash_device *)conf_obj->obj;

    switch (offset)
    {
        case _FLASH_MCR:
            *(uint32_t *)buf = dev->regs.FLASH_MCR;
            break;
        case _FLASH_LMIR:
            *(uint32_t *)buf = dev->regs.FLASH_LMIR;
            break;
        case _FLASH_HLR:
            *(uint32_t *)buf = dev->regs.FLASH_HLR;
            break;
        case _FLASH_SLMLR:
            *(uint32_t *)buf = dev->regs.FLASH_SLMLR;
            break;
        case _FLASH_LMSR:
            *(uint32_t *)buf = dev->regs.FLASH_LMSR;
            break;
        case _FLASH_HSR:
            *(uint32_t *)buf = dev->regs.FLASH_HSR;
            break;
        case _FLASH_AR:
            *(uint32_t *)buf = dev->regs.FLASH_AR;
            break;
        case _FLASH_BIUCR:
            *(uint32_t *)buf = dev->regs.FLASH_BIUCR;
            break;
        case _FLASH_BIUAPR:
            *(uint32_t *)buf = dev->regs.FLASH_BIUAPR;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in mpc5554_flash\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器写入操作方法
static exception_t mpc5554_flash_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc5554_flash_device *dev = (mpc5554_flash_device *)conf_obj->obj;
    uint32_t val = *(uint32_t *)buf;

    switch (offset)
    {
        case _FLASH_MCR:
            dev->regs.FLASH_MCR = val;
            if (val & MCR_ERS)
            {
                if (val & MCR_EHV)
                {
                    int ret;
                    if (ret = flash_erase(dev) < 0)
                    {
                        skyeye_log(Error_log, __FUNCTION__,"erse flash error: %d\n", ret);
                    }
                    dev->regs.FLASH_MCR |= MCR_PEG;
                }
            }

            if (val & MCR_PGM)
            {
                if (dev->regs.FLASH_MCR & MCR_EHV)
                {
                    dev->pgm_flag = 1;
                }
            }

            if ((!(val & MCR_PGM)) && (dev->pgm_flag == 1))
            {
                dev->regs.FLASH_MCR |= MCR_PEG;
                dev->pgm_flag = 0;
            }
            break;
        case _FLASH_LMIR:
            dev->regs.FLASH_LMIR = val;
            break;
        case _FLASH_HLR:
            dev->regs.FLASH_HLR = val;
            break;
        case _FLASH_SLMLR:
            dev->regs.FLASH_SLMLR = val;
            break;
        case _FLASH_LMSR:
            dev->regs.FLASH_LMSR = val;
            break;
        case _FLASH_HSR:
            dev->regs.FLASH_HSR = val;
            break;
        case _FLASH_AR:
            dev->regs.FLASH_AR = val;
            break;
        case _FLASH_BIUCR:
            dev->regs.FLASH_BIUCR = val;
            break;
        case _FLASH_BIUAPR:
            dev->regs.FLASH_BIUAPR = val;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in mpc5554_flash\n", offset);
            break;
    }

    return No_exp;
}

// 寄存器接口:根据寄存器ID获取寄存器名
static char* mpc5554_flash_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_name[id];
}

// 寄存器接口:根据寄存器ID设置寄存器值
static exception_t mpc5554_flash_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    mpc5554_flash_device *dev = (mpc5554_flash_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    *((uint32_t*)regs + id) = value;

    return No_exp;
}

// 寄存器接口:获取设备有多少寄存器
static uint32_t mpc5554_flash_get_regnum(conf_object_t *conf_obj)
{
    return sizeof(struct registers) / 4;
}

// 寄存器接口:根据寄存器ID获取寄存器值
static uint32_t mpc5554_flash_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    mpc5554_flash_device *dev = (mpc5554_flash_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    uint32_t* regs_value = (uint32_t*)regs + id;

    return *regs_value;
}

// 寄存器接口:根据寄存器名获取寄存器ID
static uint32_t mpc5554_flash_get_regid_by_name(conf_object_t *conf_obj, char *name)
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
static uint32_t mpc5554_flash_get_regoffset_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_offset[id];
}

// 实例化一个新设备对象，obj_name是在json文件中配置的名称
static conf_object_t* new_mpc5554_flash(const char *obj_name)
{
    mpc5554_flash_device* dev = skyeye_mm_zero(sizeof(mpc5554_flash_device));
    dev->obj = new_conf_object(obj_name, dev);
    // TODO: 在此添加自定义源码

    // 寄存器初始化
    struct registers *regs = &(dev->regs);
    regs->FLASH_MCR = 0x07c03c00;
    regs->FLASH_LMIR = 0;
    regs->FLASH_HLR = 0;
    regs->FLASH_SLMLR = 0;
    regs->FLASH_LMSR = 0;
    regs->FLASH_HSR = 0;
    regs->FLASH_AR = 0;
    regs->FLASH_BIUCR = 0;
    regs->FLASH_BIUAPR = 0;

    return dev->obj;
}

// 配置实例化的设备对象
static exception_t config_mpc5554_flash(conf_object_t* conf_obj)
{
    mpc5554_flash_device *dev = (mpc5554_flash_device *)conf_obj->obj;
    // TODO: 在此添加自定义源码


    return No_exp;
}

// 释放设备对象来做一些内存释放
static exception_t free_mpc5554_flash(conf_object_t* conf_obj)
{
    mpc5554_flash_device *dev = (mpc5554_flash_device *)conf_obj->obj;

    skyeye_free(conf_obj->objname);
    skyeye_free(conf_obj->obj);
    skyeye_free(conf_obj);

    return No_exp;
}

/* 中断控制器接口配置 */
static exception_t general_memory_space_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc5554_flash_device *dev = obj->obj;
    dev->memory = obj2;
    dev->memory_iface = (memory_space_intf *)SKY_get_iface(dev->memory, MEMORY_SPACE_INTF_NAME);
    if (dev->memory_iface == NULL)
    {
        if ( dev->memory == NULL)
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
    mpc5554_flash_device *dev = obj->obj;
    *obj2 = dev->memory;
    *port = NULL;
    return No_exp;
}


// 初始化模块类，并在SkyEye加载模块时执行以下代码
void init_mpc5554_flash(void)
{
    // 向SkyEye注册一个设备
    static skyeye_class_t class_data =
    {
        .class_name      = "mpc5554_flash",
        .class_desc      = "mpc5554_flash",
        .new_instance    = new_mpc5554_flash,
        .free_instance   = free_mpc5554_flash,
        .config_instance = config_mpc5554_flash,
        .module_type     = SKYML_OFFCHIP,
    };
    conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);

    // 设置寄存器地址读写方法
    static memory_space_intf io_memory_interface =
    {
        .read  = mpc5554_flash_read,
        .write = mpc5554_flash_write
    };
    SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

    // 注册访问接口(用于SkyEye图形控制接口)
    static skyeye_reg_intf reg_interface =
    {
        .get_regvalue_by_id  = mpc5554_flash_get_regvalue_by_id,
        .get_regname_by_id   = mpc5554_flash_get_regname_by_id,
        .set_regvalue_by_id  = mpc5554_flash_set_regvalue_by_id,
        .get_regnum          = mpc5554_flash_get_regnum,
        .get_regid_by_name   = mpc5554_flash_get_regid_by_name,
        .get_regoffset_by_id = mpc5554_flash_get_regoffset_by_id
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
        (struct ConnectDescription )
        {
            .name = MEMORY_SPACE_INTF_NAME,
            .set = general_memory_space_set,
            .get = general_memory_space_get,
        },
    };
    class_register_ifaces(dev_class, ifaces);
    class_register_connects(dev_class, connects);
}
