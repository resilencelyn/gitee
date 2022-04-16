/* Copyright (C) 
 * 2014 - Michael.Kang blackfin.kang@gmail.com
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */
/**
 * @file cat24c64.c
 * @brief 
 * @author Michael.Kang blackfin.kang@gmail.com
 * @version 7849
 * @date 2014-12-05
 */

#include <stdio.h>
#include <stdlib.h>
#include <skyeye_config.h>
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_exec.h>
#include <skyeye_cell.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_iface.h>
#include <skyeye_log.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_dev_intf.h>
#include "skyeye_thread.h"
#include "eeprom.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static uint32_t bswap_word(uint32_t data)
{
    return (data<<24)|((data<<8) & 0xff0000) | (data>>24) | ((data>>8) & 0xff00);
}

static exception_t eeprom_sparc_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
    eeprom_dev *dev = (eeprom_dev *)opaque->obj;
    if (dev->image_space)
    {
        dev->image_space->read(dev->image, offset, buf, count);
    }

    *(uint32_t *)buf = bswap_word(*(uint32_t *)buf);
}

static exception_t eeprom_sparc_write(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
    eeprom_dev *dev=  (eeprom_dev *)opaque->obj;
    uint32_t data = *(uint32_t *)buf;
    int flag = 0;

    if (offset == CHIP1_KEY_ADDR1 || offset == CHIP2_KEY_ADDR1 || offset == CHIP3_KEY_ADDR1)
    {
        if (dev->KeyAddr1 == KEY1_VALUE && data == KEY3_VALUE)
        {
            dev->KeyAddr3 = data;
            flag = 1;
        } else if (data == KEY1_VALUE)
        {
            dev->KeyAddr1 = data;
        } else if (data == 0)
        {
            dev->KeyAddr1 = 0;
            dev->KeyAddr3 = 0;
        } else
        {
            //do nothing
        }
        dev->chip_num = (offset >> 16) / 8;
    } else if (offset == CHIP1_KEY_ADDR2 || offset == CHIP2_KEY_ADDR2 || offset == CHIP3_KEY_ADDR2)
    {
        if (data == KEY2_VALUE)
        {
            dev->KeyAddr2 = data;
        } else if (data == 1)
        {
            dev->KeyAddr2 = data;
        } else
        {
            //do nothing
        }
    } else
    {
        //do nothing
    }

    *(uint32_t *)buf = bswap_word(data);
    if (dev->KeyAddr1 == KEY1_VALUE && dev->KeyAddr2 == KEY2_VALUE && dev->KeyAddr3 == KEY3_VALUE)
    {
        dev->write_chip_flag = 1;
    } else
    {
        dev->write_chip_flag = 0;
    }

    if (dev->write_chip_flag)
    {
        if (dev->chip_num == EEPROM_CHIP_1)
        {
            //write eeprom chip1
            if (dev->fp[0] != NULL && flag == 0)
            {
                fwrite(buf, 1, count, dev->fp[0]);
                fflush(dev->fp[0]);
            }
        } else if (dev->chip_num == EEPROM_CHIP_2)
        {
            //write eeprom chip2 
            if (dev->fp[1] != NULL && flag == 0)
            {
                fwrite(buf, 1, count, dev->fp[1]);
                fflush(dev->fp[1]);
            }
        } else if (dev->chip_num == EEPROM_CHIP_3)
        {
            //write eeprom chip3 
            if (dev->fp[2] != NULL && flag == 0)
            {
                fwrite(buf, 1, count, dev->fp[2]);
                fflush(dev->fp[2]);
            }
        } else
        {
            //do nothing
        }
    }

    if (dev->image_space)
    {
        dev->image_space->write(dev->image, offset, buf, count);
    }
}

static conf_object_t* new_eeprom(const char* obj_name)
{
    eeprom_dev* dev = skyeye_mm_zero(sizeof(eeprom_dev));
    dev->obj = new_conf_object(obj_name, dev);

    return dev->obj;
}

static exception_t reset_eeprom(conf_object_t* opaque, const char* args)
{
    eeprom_dev *dev = opaque->obj;
    return No_exp;
}

static exception_t free_eeprom(conf_object_t* conf_obj)
{
    eeprom_dev *dev = conf_obj->obj;

    return No_exp;
}

static exception_t memory_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    eeprom_dev *dev = obj->obj;
    dev->image = obj2;
    dev->image_space = (memory_space_intf*)SKY_get_iface(dev->image, MEMORY_SPACE_INTF_NAME);
    return No_exp;
}

static exception_t memory_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    eeprom_dev *dev = obj->obj;
    *obj2 = dev->image;
    *port = NULL;
    return No_exp;
}

static void create_file(eeprom_dev* dev, char* filename, int chip_num)
{
    dev->fp[chip_num] = fopen(filename, "wb+");
    if(dev->fp[chip_num] == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "can not open file: %s\n", filename);
        return;
    }
    return;
}

static attr_value_t get_attr_eeprom_chip1(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    eeprom_dev* dev = conf_obj->obj;
    attr_value_t value = SKY_make_attr_string(dev->filename1);
    return value;
}

static exception_t set_attr_eeprom_chip1(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    eeprom_dev* dev = conf_obj->obj;
    dev->filename1 = skyeye_strdup(SKY_attr_string(*value));
    create_file(dev, dev->filename1, EEPROM_CHIP_1);
    return No_exp;
}

static attr_value_t get_attr_eeprom_chip2(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    eeprom_dev* dev = conf_obj->obj;
    attr_value_t value = SKY_make_attr_string(dev->filename2);
    return value;
}

static exception_t set_attr_eeprom_chip2(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    eeprom_dev* dev = conf_obj->obj;
    dev->filename2 = skyeye_strdup(SKY_attr_string(*value));
    create_file(dev, dev->filename2, EEPROM_CHIP_2);
    return No_exp;
}

static attr_value_t get_attr_eeprom_chip3(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    eeprom_dev* dev = conf_obj->obj;
    attr_value_t value = SKY_make_attr_string(dev->filename3);
    return value;
}

static exception_t set_attr_eeprom_chip3(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    eeprom_dev* dev = conf_obj->obj;
    dev->filename3 = skyeye_strdup(SKY_attr_string(*value));
    create_file(dev, dev->filename3, EEPROM_CHIP_3);
    return No_exp;
}

void eeprom_register_attribute(conf_class_t* clss)
{
    SKY_register_attribute(clss, "eeprom_chip1_file", get_attr_eeprom_chip1, NULL, set_attr_eeprom_chip1, NULL, SKY_Attr_Optional, "string", "eeprom chip1 file name");
    SKY_register_attribute(clss, "eeprom_chip2_file", get_attr_eeprom_chip2, NULL, set_attr_eeprom_chip2, NULL, SKY_Attr_Optional, "string", "eeprom chip2 file name");
    SKY_register_attribute(clss, "eeprom_chip3_file", get_attr_eeprom_chip3, NULL, set_attr_eeprom_chip3, NULL, SKY_Attr_Optional, "string", "eeprom chip3 file name");

}

void init_eeprom()
{
    static skyeye_class_t class_data =
    {
        .class_name = "eeprom_sparc",
        .class_desc = "eeprom_sparc",
        .new_instance = new_eeprom,
        .reset_instance = reset_eeprom,
        .free_instance = free_eeprom,
        .get_attr = NULL,
        .set_attr = NULL
    };

    conf_class_t* class = SKY_register_device_class(class_data.class_name, &class_data);

    static const memory_space_intf io_memory =
    {
        .read = eeprom_sparc_read,
        .write = eeprom_sparc_write,
    };
    SKY_register_iface(class, MEMORY_SPACE_INTF_NAME, &io_memory);

    static const struct InterfaceDescription ifaces[] =
    {
        (struct InterfaceDescription)
        {
            .name = MEMORY_SPACE_INTF_NAME,
            .iface = &io_memory,
        }
    };

    static const struct ConnectDescription connects[] =
    {
        (struct ConnectDescription )
        {
            .name = MEMORY_SPACE_INTF_NAME,
            .set = memory_set,
            .get = memory_get,
        }
    };
    class_register_ifaces(class, ifaces);
    class_register_connects(class, connects);

    eeprom_register_attribute(class);
}

