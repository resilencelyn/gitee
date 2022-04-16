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
#include "mpc5554_sm29lv160.h"

typedef enum
{
    NORMAL_READ_OPERATION = 0,
    WRITE_OPERATION = 1,
    ERASE_OPERATION = 2
}flash_operation;

#define IMAGE_BASE_ADDR_SM29LV160          0U
#define SM29LV160_SECTOR_BYTES       0x10000U      //64k bytes
#define SM29LV160_SECTOR_WORDS        0x8000U
#define SM29LV160_SECTOR_NUM              32U
#define SM29LV160_TOTAL_BYTES       0x200000U      //2M  bytes
#define SM29LV160_TOTAL_WORDS       0x100000U


int flash_erase(mpc5554_sm29lv160_device *dev, uint32_t LogicAddr)
{
    int i;
    uint8_t nbyte = 2;
    uint32_t val = 0xffff;
    uint8_t SecNum = 0;
    uint32_t PhyAddrSectorStart = 0;

    if ((dev->fp != NULL) && (dev->image_iface != NULL))
    {
        if (dev->image_iface != NULL)
        {
            SecNum =  LogicAddr / SM29LV160_SECTOR_WORDS;
            PhyAddrSectorStart = IMAGE_BASE_ADDR_SM29LV160 + SecNum * SM29LV160_SECTOR_WORDS;

            fseek(dev->fp, PhyAddrSectorStart, SEEK_SET);
            for (i=0; i<SM29LV160_SECTOR_BYTES;)
            {
                dev->image_iface->write(dev->image, (PhyAddrSectorStart+i), &val, nbyte);
                fwrite(&val, 1, 2, dev->fp);
                i = i + 2;
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

int flash_write(mpc5554_sm29lv160_device *dev, uint32_t LogicAddr, uint16_t val)
{
    int i;
    uint8_t nbyte = 2;

    if ((dev->fp != NULL) && (dev->image_iface != NULL))
    {
        if (dev->image_iface != NULL)
        {
            dev->image_iface->write(dev->image, (IMAGE_BASE_ADDR_SM29LV160 + LogicAddr), &val, nbyte);
            fseek(dev->fp, (IMAGE_BASE_ADDR_SM29LV160 + LogicAddr), SEEK_SET);
            fwrite(&val, 1, 2, dev->fp);
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
static exception_t mpc5554_sm29lv160_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc5554_sm29lv160_device *dev = (mpc5554_sm29lv160_device *)conf_obj->obj;
    uint16_t val;

    switch (offset)
    {
        #if 0
        case _REG_INSTR_0:
            (uint16_t *)buf = dev->regs.REG_INSTR_0;

            break;
        case _REG_INSTR_1:
            (uint16_t *)buf = dev->regs.REG_INSTR_1;
            break;
        #endif

        default:

             if(dev->CurrentStatus == NORMAL_READ_OPERATION)
             {
                 if(dev->image_iface != NULL)
                 {
                     dev->image_iface->read(dev->image, (offset + IMAGE_BASE_ADDR_SM29LV160), &val, 2);
                    *(uint16_t *)buf = val;
                 } else
                 {
                    printf("Func:%s, Line:%d, dev->image_iface Null Error\n", __func__, __LINE__);
                 }
                 #if 0
                 if(dev->fp != NULL)
                 {
                    fseek(dev->fp, (offset + IMAGE_BASE_ADDR_SM29LV160), SEEK_SET);
                    fread(&val, 1, 2, dev->fp);
                 } else
                 {
                    printf("Func:%s, Line:%d, dev->fp Null Error\n", __func__, __LINE__);
                 }
                #endif
             } else
             {
                *(uint16_t *)buf = 0x40;
                dev->ReadStatusCount++;
                if (dev->ReadStatusCount == 2)
                {
                    dev->ReadStatusCount = 0;
                    //printf("change status: Read\n");
                    dev->CurrentStatus = NORMAL_READ_OPERATION;
                }
             }

             if(offset > SM29LV160_TOTAL_BYTES)
             {
                skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in mpc5554_sm29lv160\n", offset);
             }

             break;
    }

    return No_exp;
}

// 寄存器写入操作方法
static exception_t mpc5554_sm29lv160_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
    mpc5554_sm29lv160_device *dev = (mpc5554_sm29lv160_device *)conf_obj->obj;
    uint16_t val = *(uint16_t *)buf;
    int ret;
    uint8_t i_index;

    switch (offset)
    {
        case _REG_INSTR_0:
        case _REG_INSTR_1:

            if (dev->CurrentStatus == WRITE_OPERATION)
            {
                if(offset = _REG_INSTR_0)
                {
                    dev->image_iface->write(dev->image, (IMAGE_BASE_ADDR_SM29LV160 + _REG_INSTR_0), &val, 2);
                } else if(offset = _REG_INSTR_1)
                {
                    dev->image_iface->write(dev->image, (IMAGE_BASE_ADDR_SM29LV160 + _REG_INSTR_1), &val, 2);
                }

                //fseek(dev->fp, (IMAGE_BASE_ADDR_SM29LV160 + offset), SEEK_SET);
                //fwrite(&val, 1, 2, dev->fp);
            } else
            {
                dev->regs.REG_INSTR[dev->RegInstrWriteIndex] = val;
                dev->RegInstrWriteIndex++;

               if ((dev->regs.REG_INSTR[0] == 0xAA) && (dev->regs.REG_INSTR[1] == 0x55)
               && (dev->regs.REG_INSTR[2] == 0xA0))
               {
                    //printf("change status: Write\n");
                    dev->CurrentStatus = WRITE_OPERATION;
                    dev->RegInstrWriteIndex = 0;
                    for(i_index=0; i_index<3; i_index++)
                    {
                        dev->regs.REG_INSTR[i_index] = 0x0;
                    }
               } else if ((dev->regs.REG_INSTR[0] == 0xAA) && (dev->regs.REG_INSTR[1] == 0x55)
               && (dev->regs.REG_INSTR[2] == 0x80) && (dev->regs.REG_INSTR[3] == 0xAA)
               && (dev->regs.REG_INSTR[4] == 0x55))
               {
                    dev->RegInstrWriteIndex = 0;
                    //printf("change status: Erase\n");
                    dev->CurrentStatus = ERASE_OPERATION;
                    for(i_index=0; i_index<5; i_index++)
                    {
                        dev->regs.REG_INSTR[i_index] = 0x0;
                    }
               } else
               {
                  if(dev->RegInstrWriteIndex == 5)
                  {
                      dev->RegInstrWriteIndex = 0;
                  }
               }
            }
            break;

        default:
            if ( dev->CurrentStatus == ERASE_OPERATION)
            {
                if (val == 0x30)
                {
                    ret = flash_erase(dev, offset);
                    if (ret < 0)
                    {
                        skyeye_log(Error_log, __FUNCTION__,"erse flash error: %d\n", ret);
                    }
                }
            } else if(dev->CurrentStatus == WRITE_OPERATION)
            {
                ret = flash_write(dev, offset, val);
                if (ret < 0)
                {
                    skyeye_log(Error_log, __FUNCTION__,"flash write error: %d\n", ret);
                }
            }

            if(offset > SM29LV160_TOTAL_BYTES)
            {
                skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in mpc5554_sm29lv160\n", offset);
            }
            break;
    }

    return No_exp;
}

// 寄存器接口:根据寄存器ID获取寄存器名
static char* mpc5554_sm29lv160_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_name[id];
}

// 寄存器接口:根据寄存器ID设置寄存器值
static exception_t mpc5554_sm29lv160_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    mpc5554_sm29lv160_device *dev = (mpc5554_sm29lv160_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    *((uint32_t*)regs + id) = value;

    return No_exp;
}

// 寄存器接口:获取设备有多少寄存器
static uint32_t mpc5554_sm29lv160_get_regnum(conf_object_t *conf_obj)
{
    //return sizeof(struct registers) / 4;
    return 0;
}

// 寄存器接口:根据寄存器ID获取寄存器值
static uint32_t mpc5554_sm29lv160_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    mpc5554_sm29lv160_device *dev = (mpc5554_sm29lv160_device *)conf_obj->obj;
    struct registers *regs = &(dev->regs);

    uint32_t* regs_value = (uint32_t*)regs + id;

    return *regs_value;
}

// 寄存器接口:根据寄存器名获取寄存器ID
static uint32_t mpc5554_sm29lv160_get_regid_by_name(conf_object_t *conf_obj, char *name)
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
static uint32_t mpc5554_sm29lv160_get_regoffset_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_offset[id];
}

// 实例化一个新设备对象，obj_name是在json文件中配置的名称
static conf_object_t* new_mpc5554_sm29lv160(const char *obj_name)
{
    mpc5554_sm29lv160_device* dev = skyeye_mm_zero(sizeof(mpc5554_sm29lv160_device));
    dev->obj = new_conf_object(obj_name, dev);
    uint8_t i = 0;
    // TODO: 在此添加自定义源码

    // 寄存器初始化
    struct registers *regs = &(dev->regs);
    fopen("flash.txt","w+");
    for (i=0; i<5; i++)
    {
        regs->REG_INSTR[i]  =  0x0;
    }
    //regs->REG_INSTR_0 = 0;
    //regs->REG_INSTR_1 = 0;

    return dev->obj;
}

// 配置实例化的设备对象
static exception_t config_mpc5554_sm29lv160(conf_object_t* conf_obj)
{
    uint16_t buf = 0xAAAA;

    mpc5554_sm29lv160_device *dev = (mpc5554_sm29lv160_device *)conf_obj->obj;
    // TODO: 在此添加自定义源码
    dev->fp = fopen("flash.txt","r+");
    #if 0
    dev->image_iface->write(dev->image, 0x04E1E, &buf, 2);
    dev->image_iface->write(dev->image, 0x14E1E, &buf, 2);
    dev->image_iface->write(dev->image, 0x27CFE, &buf, 2);
    dev->image_iface->write(dev->image, 0x300FE, &buf, 2);
    dev->image_iface->write(dev->image, 0x44E1E, &buf, 2);
    dev->image_iface->write(dev->image, 0x57CFE, &buf, 2);
    #endif
    return No_exp;
}

// 释放设备对象来做一些内存释放
static exception_t free_mpc5554_sm29lv160(conf_object_t* conf_obj)
{
    mpc5554_sm29lv160_device *dev = (mpc5554_sm29lv160_device *)conf_obj->obj;

    skyeye_free(conf_obj->objname);
    skyeye_free(conf_obj->obj);
    skyeye_free(conf_obj);

    return No_exp;
}

static exception_t memory_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    mpc5554_sm29lv160_device *dev = obj->obj;
    dev->image = obj2;
    dev->image_iface = (memory_space_intf *)SKY_get_iface(dev->image, MEMORY_SPACE_INTF_NAME);
    if (dev->image_iface == NULL)
    {
        if ( dev->image == NULL)
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

static exception_t memory_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    mpc5554_sm29lv160_device *dev = obj->obj;
    *obj2 = dev->image;
    *port = NULL;
    return No_exp;
}

// 初始化模块类，并在SkyEye加载模块时执行以下代码
void init_mpc5554_sm29lv160(void)
{
    // 向SkyEye注册一个设备
    static skyeye_class_t class_data =
    {
        .class_name      = "mpc5554_sm29lv160",
        .class_desc      = "nor flash",
        .new_instance    = new_mpc5554_sm29lv160,
        .free_instance   = free_mpc5554_sm29lv160,
        .config_instance = config_mpc5554_sm29lv160,
        .module_type     = SKYML_OFFCHIP,
    };
    conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);

    // 设置寄存器地址读写方法
    static memory_space_intf io_memory_interface =
    {
        .read  = mpc5554_sm29lv160_read,
        .write = mpc5554_sm29lv160_write
    };
    SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

    // 注册访问接口(用于SkyEye图形控制接口)
    static skyeye_reg_intf reg_interface =
    {
        .get_regvalue_by_id  = mpc5554_sm29lv160_get_regvalue_by_id,
        .get_regname_by_id   = mpc5554_sm29lv160_get_regname_by_id,
        .set_regvalue_by_id  = mpc5554_sm29lv160_set_regvalue_by_id,
        .get_regnum          = mpc5554_sm29lv160_get_regnum,
        .get_regid_by_name   = mpc5554_sm29lv160_get_regid_by_name,
        .get_regoffset_by_id = mpc5554_sm29lv160_get_regoffset_by_id
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
            .name = MEMORY_SPACE_INTF_NAME,
            .set = memory_set,
            .get = memory_get,
         }
    };
    class_register_ifaces(dev_class, ifaces);
    class_register_connects(dev_class, connects);
}
