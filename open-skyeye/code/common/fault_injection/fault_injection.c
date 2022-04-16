
/**
 * @file addr_space.c
 * @brief The addr_space class
 * @author Michael.Kang blackfin.kang@gmail.com
 * @version 0.1
 * @date 2011-07-11
 */

#ifdef DEBUG
#undef DEBUG
#endif
//#define DEBUG
#include <skyeye_log.h>

#include <skyeye_types.h>
#include <memory_space.h>
#include <skyeye_addr_space.h>
#include <skyeye_mm.h>
#include "skyeye_obj.h"
#include <skyeye_dev_intf.h>
#include <skyeye_iface.h>
#include <skyeye_interface.h>
#include "skyeye_class.h"
#include  <stdlib.h>
#include <skyeye_fault_injection.h>

exception_t memory_space_plug_read(conf_object_t * obj, generic_address_t addr, void *buf, size_t count)
{

    memory_space_plug_t *dev_memory_space = obj->obj;
    exception_t ret;

    ret = dev_memory_space->memory_space->read(dev_memory_space->dev_memory_space_obj, addr, buf, count);

    if (ret != No_exp)
    {
        skyeye_log(Error_log, __FUNCTION__, "read address:0x%x failed!\n", addr);
    }

    return ret;
}

exception_t memory_space_plug_write(conf_object_t * obj, generic_address_t addr, void *buf, size_t count)
{

    memory_space_plug_t *dev_memory_space = obj->obj;
    exception_t ret;

    ret =dev_memory_space->memory_space->write(dev_memory_space->dev_memory_space_obj, addr, buf, count);

    if (ret != No_exp)
    {
        skyeye_log(Error_log, __FUNCTION__, "write address:0x%x failed!\n", addr);
    }

    return ret;
}

get_page_t *memory_space_plug_get_page(conf_object_t * obj, generic_address_t addr)
{

    memory_space_plug_t *dev_memory_space = obj->obj;

    dev_memory_space->memory_space->get_page(dev_memory_space->dev_memory_space_obj, addr);

    return NULL;
}

static conf_object_t *new_memory_space_plug(char *obj_name)
{
    memory_space_plug_t *dev = skyeye_mm_zero(sizeof (memory_space_plug_t));

    dev->obj = new_conf_object(obj_name, dev);

    return dev->obj;
}

static exception_t free_memory_space_plug(conf_object_t * obj)
{
    skyeye_free(obj->objname);
    skyeye_free(obj->obj);
    skyeye_free(obj);

    return No_exp;
}

exception_t memory_space_save_plug_obj(conf_object_t * obj, conf_object_t * tmp_obj, generic_address_t addr, int bitnum,
                                       int mode)
{
    memory_space_plug_t *dev_memory_space = obj->obj;
    int count = dev_memory_space->fi_array.count;

    //first time set dev_memory_space_obj
    if (tmp_obj)
    {
        dev_memory_space->dev_memory_space_obj = tmp_obj;
        dev_memory_space->memory_space = SKY_get_iface(tmp_obj, MEMORY_SPACE_INTF_NAME);
    }

    dev_memory_space->fi[count].addr = addr;
    dev_memory_space->fi[count].bitnum = bitnum;
    dev_memory_space->fi[count].mode = mode;

    dev_memory_space->fi_array.fi[count][0] = addr;
    dev_memory_space->fi_array.fi[count][1] = (unsigned int) &dev_memory_space->fi[count];
    dev_memory_space->fi_array.count++;

    if (dev_memory_space->fi_array.count == DEVICE_FI_NUM)
    {
        dev_memory_space->fi_array.count = 0;
        dev_memory_space->fi_array.fi[dev_memory_space->fi_array.count][0] = addr;
        dev_memory_space->fi_array.fi[dev_memory_space->fi_array.count][1] =
            (unsigned int) &dev_memory_space->fi[count];
    }
}

conf_object_t *memory_space_clear_plug_obj(conf_object_t * obj, generic_address_t addr, int bitnum, int mode)
{
    memory_space_plug_t *dev_memory_space = obj->obj;
    int i, j;

    if (dev_memory_space->fi_array.count)
    {
        for (i = 0; i < dev_memory_space->fi_array.count; i++)
        {
            if (((fi_t *) (dev_memory_space->fi_array.fi[i][1]))->addr != addr)
            {
                continue;
            } else
            {
                if (((fi_t *) (dev_memory_space->fi_array.fi[i][1]))->bitnum == bitnum
                    && ((fi_t *) (dev_memory_space->fi_array.fi[i][1]))->mode == mode)
                {
                    dev_memory_space->fi_array.fi[i][0] = 0;
                    for (j = i; j < dev_memory_space->fi_array.count; j++)
                    {
                        dev_memory_space->fi_array.fi[j][0] = dev_memory_space->fi_array.fi[j + 1][0];
                        dev_memory_space->fi_array.fi[j][1] = dev_memory_space->fi_array.fi[j + 1][1];
                    }
                    dev_memory_space->fi_array.fi[dev_memory_space->fi_array.count][0] = 0;
                    i--;
                    dev_memory_space->fi_array.count--;
                }
            }
        }
    }
    return NULL;
}

void *memory_space_get_plug_obj(conf_object_t * obj)
{
    memory_space_plug_t *dev_memory_space = obj->obj;

    if (dev_memory_space->fi_array.count)
    {
        return &dev_memory_space->fi_array;
    }

    return NULL;
}

void memory_space_plug_register_interface(conf_class_t * clss)
{
    static const memory_space_intf io_memory = {
        .read = memory_space_plug_read,
        .write = memory_space_plug_write,
        .get_page = memory_space_plug_get_page,
    };
    SKY_register_iface(clss, MEMORY_SPACE_PLUG_INTF_NAME, &io_memory);

    static const memory_space_plug_save_intf save_plug = {
        .save_plug_obj = memory_space_save_plug_obj,
        .get_plug_obj = memory_space_get_plug_obj,
        .clear_plug_obj = memory_space_clear_plug_obj,
    };
    SKY_register_iface(clss, MEMORY_SPACE_PLUG_SAVE_INTF_NAME, &save_plug);
}

void init_memory_space_plug()
{
    static skyeye_class_t class_data = {
        .class_name = "memory_space_plug",
        .class_desc = "memory_space_plug",
        .new_instance = new_memory_space_plug,
        .free_instance = free_memory_space_plug,
        .get_attr = NULL,
        .set_attr = NULL
    };
    conf_class_t *clss = SKY_register_device_class(class_data.class_name, &class_data);

    memory_space_plug_register_interface(clss);
    return NULL;
}
