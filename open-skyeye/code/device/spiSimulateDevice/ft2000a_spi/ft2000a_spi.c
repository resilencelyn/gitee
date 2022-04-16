/*************************************************************************
  > File Name    : ft2000a_spi.c
  > Author       : jiachao
  > Mail         : jiachao@163.com
  > Created Time : 2019/11/27 15:10:41
  > This is ft-2000A SPI(for FLASH interface) register device
 ************************************************************************/

#include <skyeye_lock.h>
#include <skyeye_types.h>
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
#define DEBUG
#include <skyeye_log.h>
#include "ft2000a_spi.h"

static void spi_bus_receive(void *spi_bus, char *buf, int len)
{
    ft2000a_spi_device *dev = ((conf_object_t *)spi_bus)->obj;
}

static void erase_64k_flash(conf_object_t *opaque)
{
    ft2000a_spi_device *dev = opaque->obj;
    struct spi_registers *regs = &dev->regs;
    exception_t ret;
    int i;

    int erase_addr = regs->addr_port_reg;
    int erase_data = 0xff;

    for (i = 0; i < LEN_64K; i++)
    {
        if (dev->filename != NULL)
        {
            if(dev->wr_fp == NULL)
            {
                dev->wr_fp = fopen(dev->filename, "wb+");
            }

            if (dev->wr_fp == NULL)
            {
                skyeye_log(Error_log, __FUNCTION__, "can not open file: %s\n", dev->filename);
                return ;
            } else
            {
                fseek(dev->wr_fp, erase_addr + i, SEEK_SET);
                fwrite(&erase_data, 1, 1, dev->wr_fp);
                fflush(dev->wr_fp);
            }
        } else
        {
            if (dev->image_space)
            {
                ret = dev->image_space->write(dev->image, erase_addr + i, &erase_data, 1);

                if (ret != No_exp)
                {
                    skyeye_log(Error_log, __FUNCTION__, "Can not erase the flash addr: %x\n", erase_addr + i);
                    return ;
                }
            }
        }
    }
}

static void flash_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
    ft2000a_spi_device *dev = opaque->obj;
    struct spi_registers *regs = &dev->regs;
    uint32_t data = *(uint32_t *)buf;
    exception_t ret;

    if (dev->filename != NULL)
    {
        if(dev->wr_fp == NULL)
        {
            dev->wr_fp = fopen(dev->filename, "wb+");
        }

        if (dev->wr_fp == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "can not open file: %s\n", dev->filename);
            return ;
        } else
        {
            fseek(dev->wr_fp, offset, SEEK_SET);
            fwrite(&data, 1, count, dev->wr_fp);
            fflush(dev->wr_fp);
        }
    } else
    {
        if (dev->image_space)
        {
            ret = dev->image_space->write(dev->image, offset, &data, count);

            if (ret != No_exp)
            {
                skyeye_log(Error_log, __FUNCTION__, "Can not write the flash addr: %x\n", offset);
                return ;
            }
        }
    }
}

static void flash_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
    ft2000a_spi_device *dev = opaque->obj;
    struct spi_registers *regs = &dev->regs;

    if (dev->filename != NULL)
    {
        if (dev->rd_fp == NULL)
        {
            dev->rd_fp = fopen(dev->filename, "rb");
        }

        if (dev->rd_fp == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "can not open file: %s\n", dev->filename);
            if (dev->image_space)
            {
                dev->image_space->read(dev->image, offset, buf, count);
            }
        } else
        {
            fseek(dev->rd_fp, offset, SEEK_SET);
            fread(buf, 1, count, dev->rd_fp);
        }
    }
    else
    {
        if (dev->image_space)
        {
            dev->image_space->read(dev->image, offset, buf, count);
        }
    }

    return ;
}

static exception_t ft2000a_spi_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
    ft2000a_spi_device *dev = opaque->obj;
    struct spi_registers *regs = &dev->regs;
    uint32_t *data = (uint32_t *)buf;
    exception_t ret;

    switch (offset)
    {
        case SPI_CONFIG:
            *data = regs->spi_conf_reg;
            break;
        case WP_REG:
            *data = regs->wp_reg;
            break;
        case CS_DELAY:
            *data = regs->cs_delay_reg;
            break;
        case SFC_REG:
            *data = regs->set_flash_capacity_reg;
            break;
        case FLUSH_FIFO:
            *data = regs->flush_fifo_reg;
            break;
        case CMD_PORT:
            *data = regs->cmd_port_reg;
            break;
        case ADDR_PORT:
            *data = regs->addr_port_reg;
            break;
        case HIGH_DATA_PORT:
            *data = regs->high_data_reg;
            break;
        case LOW_DATA_PORT:
            *data = regs->low_data_reg;
            //使能spi 命令
            if ((regs->cmd_port_reg & SPI_CFG_CMD_SE_64KB) == SPI_CFG_CMD_SE_64KB)
            {
                //erase flash 64k
                erase_64k_flash(opaque);
            }
            break;
        case CS0_CFG_ADDR:
            *data = regs->cs0_cfg_addr;
            break;
        default:
            if (offset >= 0x600000)
            {
                flash_read(opaque, offset, data, count);
            } else
            {
                if (dev->image_space)
                {
                    ret = dev->image_space->read(dev->image, offset, data, count);
                    if(ret != No_exp)
                    {
                        skyeye_log(Error_log, __FUNCTION__, "Can't read the register at 0x%x in ft2000a_spi\n", offset);
                        return Not_found_exp;
                    }
                }
            }
            return No_exp;
    }

    if (offset >= SPI_CONFIG && offset <= CS0_CFG_ADDR)
    {
        SKYEYE_DEBUG("In %s, offset = %x data = %x\n", __func__, offset, *data);
    }
    return No_exp;
}

static exception_t ft2000a_spi_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
    ft2000a_spi_device *dev = opaque->obj;
    struct spi_registers *regs = &dev->regs;
    uint32_t data = *(uint32_t *)buf;
    exception_t ret;

    if (offset >= SPI_CONFIG && offset <= CS0_CFG_ADDR)
    {
        SKYEYE_DEBUG("In %s, offset = %x data = %x\n", __func__, offset, data);
    }

    switch (offset)
    {
        case SPI_CONFIG:
            regs->spi_conf_reg = data;
            break;
        case WP_REG:
            regs->wp_reg = data;
            break;
        case CS_DELAY:
            regs->cs_delay_reg = data;
            break;
        case SFC_REG:
            regs->set_flash_capacity_reg = data;
            break;
        case FLUSH_FIFO:
            regs->flush_fifo_reg = data;
            break;
        case CMD_PORT:
            regs->cmd_port_reg = data;
            if ((regs->cmd_port_reg & SPI_CFG_CMD_RSSR) == SPI_CFG_CMD_RSSR)
            {
                regs->low_data_reg &= ~0x1;
            }

            if ((regs->cmd_port_reg & SPI_CFG_CMD_WREN) == SPI_CFG_CMD_WREN)
            {
                regs->low_data_reg |= 0x2;
            }
            break;
        case ADDR_PORT:
            regs->addr_port_reg = data;
            break;
        case HIGH_DATA_PORT:
            regs->high_data_reg = data;
            break;
        case LOW_DATA_PORT:
            regs->low_data_reg = data;
            break;
        case CS0_CFG_ADDR:
            regs->cs0_cfg_addr = data;
            break;
        default:
            if (offset >= 0x600000)
            {
                flash_write(opaque, offset, &data, count);
            } else
            {
                if (dev->image_space)
                {
                    ret = dev->image_space->write(dev->image, offset, &data, count);
                    if(ret != No_exp)
                    {
                        skyeye_log(Error_log, __FUNCTION__, "Can't write the register at 0x%x in ft2000a_spi\n", offset);
                        return Not_found_exp;
                    }
                }
            }
    }

    return No_exp;
}

static char* ft2000a_spi_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_name[id];
}

static exception_t ft2000a_spi_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    ft2000a_spi_device *dev = conf_obj->obj;
    struct spi_registers *regs = &(dev->regs);
    *((uint32_t*)regs + id) = value;
    return No_exp;
}

static uint32_t ft2000a_spi_get_regnum(conf_object_t *conf_obj)
{
    return  sizeof(struct spi_registers) / 4;
}

static uint32_t ft2000a_spi_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    ft2000a_spi_device *dev = conf_obj->obj;
    struct spi_registers *regs = &(dev->regs);
    uint32_t* regs_value = (uint32_t*)regs + id;
    return *regs_value;
}

static uint32_t ft2000a_spi_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
    uint32_t regnum = sizeof(struct spi_registers) / 4, i;
    for(i = 0; i < regnum; i++) {
        if(strcmp(name, regs_name[i]) == 0)
            return i;
    }
    return regnum;
}

static uint32_t ft2000a_spi_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
    return regs_offset[id];
}

void init_spi_register(struct spi_registers *regs)
{
}

static conf_object_t* new_ft2000a_spi(const char *obj_name)
{
    ft2000a_spi_device* dev = skyeye_mm_zero(sizeof(ft2000a_spi_device));
    dev->obj = new_conf_object(obj_name, dev);
    struct spi_registers *regs = &(dev->regs);
    init_spi_register(regs);
    return dev->obj;
}

static exception_t free_ft2000a_spi(conf_object_t* opaque)
{
    ft2000a_spi_device *dev = opaque->obj;
    skyeye_free(opaque->objname);
    skyeye_free(opaque->obj);
    skyeye_free(opaque);
    return No_exp;
}

static exception_t config_ft2000a_spi(conf_object_t* opaque)
{
    ft2000a_spi_device *dev = opaque->obj;
    return No_exp;
}

static exception_t image_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    ft2000a_spi_device* dev = (ft2000a_spi_device*)obj->obj;
    dev->image = obj2;
    dev->image_space = (memory_space_intf*)SKY_get_iface(obj2, MEMORY_SPACE_INTF_NAME);

    return No_exp;
}

static exception_t image_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    ft2000a_spi_device* dev = (ft2000a_spi_device*)obj->obj;
    *obj2 = dev->image;
    *port = NULL;
    return No_exp;
}

static exception_t spi_device_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    ft2000a_spi_device *dev = obj->obj;

    if (index >= MAX_SLAVES || index < 0)
        return No_exp;

    dev->slaves[index].spi_dev_iface = (spi_device_intf *)SKY_get_iface(obj2, SPI_DEVICE_INTF_NAME);
    dev->slaves[index].slave_obj = obj2;

    if (dev->slaves[index].spi_dev_iface == NULL)
    {
        if (dev->slaves[index].slave_obj == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SPI_DEVICE_INTF_NAME);
        } else
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SPI_DEVICE_INTF_NAME, obj2->objname);

        return Not_found_exp;
    }
    return No_exp;
}

static exception_t spi_device_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    return No_exp;
}

static exception_t set_attr_filename(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    ft2000a_spi_device *dev = conf_obj->obj;
    dev->filename = skyeye_strdup(SKY_attr_string(*value));
    return No_exp;
}

static attr_value_t get_attr_filename(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    ft2000a_spi_device *dev = conf_obj->obj;
    attr_value_t value = SKY_make_attr_string(dev->filename);
    return value;
}

static exception_t set_attr_skyeye_debug(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    ft2000a_spi_device *dev = conf_obj->obj;
    dev->skyeye_debug = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_skyeye_debug(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    ft2000a_spi_device *dev = conf_obj->obj;
    attr_value_t value = SKY_make_attr_uinteger(dev->skyeye_debug);
    return value;
}

static void ft2000a_spi_register_attribute(conf_class_t* clss)
{
    SKY_register_attribute(clss, "filename", get_attr_filename, NULL, set_attr_filename, NULL, SKY_Attr_Optional, "string", "The file To Flash Data");
    SKY_register_attribute(clss, "skyeye_debug", get_attr_skyeye_debug, NULL, set_attr_skyeye_debug, NULL, SKY_Attr_Optional, "uinteger", "enable skyeye debug");
    return;
}

void init_ft2000a_spi(void)
{
    static skyeye_class_t class_data =
    {
        .class_name = "ft2000a_spi",
        .class_desc = "ft2000a_spi",
        .new_instance = new_ft2000a_spi,
        .free_instance = free_ft2000a_spi,
        .config_instance = config_ft2000a_spi
    };
    conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

    static memory_space_intf io_memory =
    {
        .read = ft2000a_spi_read,
        .write = ft2000a_spi_write
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

    static skyeye_reg_intf reg_intf =
    {
        .get_regvalue_by_id = ft2000a_spi_get_regvalue_by_id,
        .get_regname_by_id = ft2000a_spi_get_regname_by_id,
        .set_regvalue_by_id = ft2000a_spi_set_regvalue_by_id,
        .get_regnum = ft2000a_spi_get_regnum,
        .get_regid_by_name = ft2000a_spi_get_regid_by_name,
        .get_regoffset_by_id = ft2000a_spi_get_regoffset_by_id
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

    static spi_bus_intf spi_bus_intf =
    {
        .spi_bus_receive = spi_bus_receive
    };
    SKY_register_iface(clss, SPI_BUS_INTF_NAME, &spi_bus_intf);

    static const struct InterfaceDescription ifaces[] =
    {
        (struct InterfaceDescription)
        {
            .name = MEMORY_SPACE_INTF_NAME,
            .iface = &io_memory
        },
        (struct InterfaceDescription)
        {
            .name = SKYEYE_REG_INTF,
            .iface = &reg_intf
        },
        (struct InterfaceDescription)
        {
            .name = SPI_BUS_INTF_NAME,
            .iface = &spi_bus_intf
        },
    };

    static const struct ConnectDescription connects[] =
    {
        /* ConnectDescription */
        (struct ConnectDescription) {
            .name = SPI_DEVICE_INTF_NAME,
            .set = spi_device_set,
            .get = spi_device_get,
        },
        (struct ConnectDescription) {
            .name = MEMORY_SPACE_INTF_NAME,
            .set = image_set,
            .get = image_get,
        }
    };

    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);
    ft2000a_spi_register_attribute(clss);
}
