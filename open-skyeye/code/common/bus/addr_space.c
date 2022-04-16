
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
#include <skyeye_fault_injection.h>
#include <skyeye_addr_space.h>
#include <skyeye_mm.h>
#include "skyeye_obj.h"
#include <skyeye_dev_intf.h>
#include <skyeye_iface.h>
#include <skyeye_interface.h>
#include "skyeye_class.h"
#include  <stdlib.h>

#define IS_EXTRA_CORE space->extra_core != NULL
#define IS_DMA_CONNECTED (space->dma != NULL)
static int ram_map_compare(const void *ptr1, const void *ptr2)
{
    if ((*(ram_array_t **) ptr1)->map->base_addr < (*(ram_array_t **) ptr2)->map->base_addr)
        return -1;
    else if ((*(ram_array_t **) ptr1)->map->base_addr == (*(ram_array_t **) ptr2)->map->base_addr)
        return 0;
    else
        return 1;
}

static int ram_op_compare(const void *ptr1, const void *ptr2)
{
    ram_array_t *iterator1 = *(ram_array_t **) ptr1;
    ram_array_t *iterator2 = *(ram_array_t **) ptr2;

    if (iterator1->map->base_addr < iterator2->map->base_addr)
    {
        return -1;
    } else
    {
        if (iterator1->map->base_addr >= iterator2->map->base_addr
            && (iterator1->map->base_addr <= iterator2->map->base_addr + iterator2->map->length))
            return 0;
    }
    return 1;
}

get_page_t *space_get_page(addr_space_t * space, generic_address_t addr)
{
    map_info_t map;

    map.base_addr = addr;
    ram_array_t key;
    ram_array_t *pkey;

    key.map = &map;
    pkey = &key;
    ram_array_t **iterator;

    iterator = bsearch(&pkey, space->ram_map_array, space->ram_map_count, sizeof (ram_array_t *), ram_op_compare);
    DBG("In %s, i=%d, addr=0x%x, base_addr=0x%x, length=0x%x\n", __FUNCTION__, i, addr, iterator->base_addr,
        iterator->length);
    if (iterator != NULL)
    {
        return (*iterator)->iface->get_page((*iterator)->map->obj, addr - (*iterator)->map->base_addr);
    } else
    {
        DBG("In %s, can't access the address 0x%x\n", __func__, addr);
    }
    return NULL;
}

static int ram_quick_access(addr_space_t * space, memory_space_intf * memory_space, map_info_t * map)
{
    char *class_name = map->obj->class_name;

    if (strncmp(class_name, "ram", strlen("ram")) != 0)
    {
        return -1;
    }
    space->ram_map_array = skyeye_realloc(space->ram_map_array, sizeof (ram_array_t *) * (++space->ram_map_count));
    space->ram_map_array[space->ram_map_count - 1] = skyeye_mm_zero(sizeof (ram_array_t));
    if (space->ram_map_array == NULL)
    {
        skyeye_log(Warning_log, __FUNCTION__, "skyeye_realloc failed\n");
        return Malloc_exp;
    }
    memory_space_intf *iface = (memory_space_intf *) SKY_get_iface(map->obj, MEMORY_SPACE_INTF_NAME);

    space->ram_map_array[space->ram_map_count - 1]->map = map;
    space->ram_map_array[space->ram_map_count - 1]->iface = iface;
    qsort(space->ram_map_array, space->ram_map_count, sizeof (ram_array_t *), ram_map_compare);
    return 0;
}

static int map_compare(const void *ptr1, const void *ptr2)
{
    if ((*(map_info_t **) ptr1)->base_addr < (*(map_info_t **) ptr2)->base_addr)
        return -1;
    else if ((*(map_info_t **) ptr1)->base_addr == (*(map_info_t **) ptr2)->base_addr)
        return 0;
    else
        return 1;
}

exception_t add_map(addr_space_t * space, generic_address_t base_addr, generic_address_t length,
                    generic_address_t start, memory_space_intf * memory_space, int priority, int swap_endian)
{
    map_info_t *map = skyeye_mm(sizeof (map_info_t));

    map->base_addr = base_addr;
    map->length = length;
    map->start = start;
    map->memory_space = memory_space;
    map->priority = priority;
    RWLOCK_INIT(map->lock);

    space->map_array = skyeye_realloc(space->map_array, sizeof (map_info_t *) * (++space->map_count));
    if (space->map_array == NULL)
    {
        printf("In %s, skyeye_realloc failed\n", __func__);
        return Malloc_exp;
    }
    space->map_array[space->map_count - 1] = map;
    DBG("In %s, map added successfully @%d\n", __FUNCTION__, i);
    qsort(space->map_array, space->map_count, sizeof (map_info_t *), map_compare);

    ram_quick_access(space, memory_space, map);
    return No_exp;
}

exception_t del_map()
{
    return No_exp;
}

static int op_compare(const void *ptr1, const void *ptr2)
{
    map_info_t *iterator1 = *(map_info_t **) ptr1;
    map_info_t *iterator2 = *(map_info_t **) ptr2;

    if (iterator1->base_addr < iterator2->base_addr)
    {
        return -1;
    } else
    {
        if (iterator1->base_addr >= iterator2->base_addr
            && (iterator1->base_addr <= iterator2->base_addr + iterator2->length))
            return 0;
    }
    return 1;
}

#define XN 6
static exception_t space_read(conf_object_t * addr_space, generic_address_t addr, void *buf, size_t count)
{
    addr_space_t *space = (addr_space_t *) (addr_space->obj);
    uint32_t c = space->map_count;
    map_info_t **array = space->map_array;

    if (c <= XN)
    {
        uint32_t i;
        map_info_t *p;
        generic_address_t baddr, len;

        for (i = 0; i < c; i++)
        {
            p = array[i];
            baddr = p->base_addr;
            len = p->length;
            if (addr >= baddr && addr <= baddr + len)
                break;
        }
        if (i < c)
        {
            return p->memory_space->read(p->memory_space->conf_obj, (addr - baddr), buf, count);
        }
    } else
    {
        map_info_t key;

        key.base_addr = addr;
        map_info_t *pkey = &key;
        map_info_t **iterator;

        iterator = bsearch(&pkey, array, c, sizeof (map_info_t *), op_compare);
        DBG("In %s, i=%d, addr=0x%x, base_addr=0x%x, length=0x%x\n", __FUNCTION__, i, addr, iterator->base_addr,
            iterator->length);
        if (iterator != NULL)
        {
            return (*iterator)->memory_space->read((*iterator)->memory_space->conf_obj, (addr - (*iterator)->base_addr),
                                                   buf, count);
        } else
        {
            DBG("In %s, can't access the address 0x%x\n", __func__, addr);
        }
    }

    return Not_found_exp;
}

exception_t SKY_space_read(conf_object_t * obj, generic_address_t addr, void *buf, size_t count)
{
    attr_value_t value = SKY_get_attribute(obj, "addr_space");
    addr_space_t *space = (addr_space_t *) SKY_attr_object(value);
    map_info_t key;

    key.base_addr = addr;
    map_info_t *pkey = &key;
    map_info_t **iterator;

    iterator = bsearch(&pkey, space->map_array, space->map_count, sizeof (map_info_t *), op_compare);
    if (iterator != NULL)
    {
        return (*iterator)->memory_space->read((*iterator)->obj, (addr - (*iterator)->base_addr), buf, count);
    } else
    {
        DBG("In %s, can't access the address 0x%x\n", __func__, addr);
    }

    return Not_found_exp;
}

static exception_t space_write(conf_object_t * addr_space, generic_address_t addr, void *buf, size_t count)
{
    addr_space_t *space = (addr_space_t *) (addr_space->obj);
    uint32_t c = space->map_count;
    map_info_t **array = space->map_array;

    if (c <= XN)
    {
        uint32_t i;
        map_info_t *p;
        generic_address_t baddr, len;

        for (i = 0; i < c; i++)
        {
            p = array[i];
            baddr = p->base_addr;
            len = p->length;
            if (addr >= baddr && addr <= baddr + len)
                break;
        }
        if (i < c)
        {
            return p->memory_space->write(p->memory_space->conf_obj, (addr - baddr), buf, count);
        }
    } else
    {
        map_info_t key;

        key.base_addr = addr;
        map_info_t *pkey = &key;
        map_info_t **iterator;

        iterator = bsearch(&pkey, array, c, sizeof (map_info_t *), op_compare);
        DBG("In %s, i=%d, addr=0x%x, base_addr=0x%x, length=0x%x\n", __FUNCTION__, i, addr, iterator->base_addr,
            iterator->length);
        if (iterator != NULL)
        {
            return (*iterator)->memory_space->write((*iterator)->memory_space->conf_obj,
                                                    (addr - (*iterator)->base_addr), buf, count);
        } else
        {
            DBG("In %s, can't access the address 0x%x\n", __func__, addr);
        }
    }

    return Not_found_exp;
}

exception_t SKY_space_write(conf_object_t * obj, generic_address_t addr, void *buf, size_t count)
{
    attr_value_t value = SKY_get_attribute(obj, "addr_space");
    addr_space_t *space = (addr_space_t *) SKY_attr_object(value);
    map_info_t key;

    key.base_addr = addr;
    map_info_t *pkey = &key;
    map_info_t **iterator;

    iterator = bsearch(&pkey, space->map_array, space->map_count, sizeof (map_info_t *), op_compare);
    DBG("In %s, addr=0x%x, base_addr=0x%x, length=0x%x\n", __FUNCTION__, addr, (*iterator)->base_addr,
        (*iterator)->length);
    if (iterator != NULL)
    {
        conf_object_t *map_obj = (*iterator)->obj;
        memory_space_intf *space_iface = SKY_get_iface(map_obj, MEMORY_SPACE_INTF_NAME);

        return space_iface->write(map_obj, (addr - (*iterator)->base_addr), buf, count);
    } else
    {
        DBG("In %s, can't access the address 0x%x\n", __func__, addr);
    }

    return Not_found_exp;
}

exception_t SKY_add_map(addr_space_t * space, conf_object_t * device, generic_address_t base_addr,
                        generic_address_t length, generic_address_t start, memory_space_intf * memory_space,
                        int priority, int swap_endian)
{
    map_info_t *map = skyeye_mm(sizeof (map_info_t));

    map->obj = device;
    map->base_addr = base_addr;
    map->length = length;
    map->start = start;
    map->memory_space = memory_space;
    map->priority = priority;
    RWLOCK_INIT(map->lock);

    space->map_array = skyeye_realloc(space->map_array, sizeof (map_info_t *) * (++space->map_count));
    if (space->map_array == NULL)
    {
        printf("In %s, skyeye_realloc failed\n", __func__);
        return Malloc_exp;
    }
    space->map_array[space->map_count - 1] = map;
    DBG("In %s, map added successfully @%d\n", __FUNCTION__, i);
    qsort(space->map_array, space->map_count, sizeof (map_info_t *), map_compare);

    ram_quick_access(space, memory_space, map);
    return No_exp;
}

exception_t space_obj_reset(addr_space_t * addr_space, const char *obj_name)
{
    conf_object_t *obj = NULL;          /* device object */
    conf_object_t *class_obj = NULL;
    skyeye_class_t *class_data = NULL;

    if (obj_name != NULL)
    {
        //reset obj_name;
        return No_exp;
    }
    // reset all object;
    int i = 0;

    for (; i < addr_space->map_count; i++)
    {
        map_info_t *iterator = addr_space->map_array[i];

        if (iterator == NULL)
            continue;
        obj = iterator->obj;
        class_obj = get_conf_obj(obj->class_name);
        class_data = class_obj->obj;
        if (class_data->reset_instance)
            class_data->reset_instance(obj, NULL);
    }
    return No_exp;
}

exception_t space_obj_free(addr_space_t * addr_space)
{
    conf_object_t *obj = NULL;          /* device object */
    conf_object_t *class_obj = NULL;
    skyeye_class_t *class_data = NULL;
    int i = 0;

    for (; i < addr_space->map_count; i++)
    {
        map_info_t *iterator = addr_space->map_array[i];

        obj = iterator->obj;
        class_obj = get_conf_obj(obj->class_name);
        class_data = class_obj->obj;
        if (class_data->free_instance)
            class_data->free_instance(obj);
        skyeye_free(iterator);
    }
    skyeye_free(addr_space->memory_space);
    skyeye_free(addr_space->map_array);
    skyeye_free(addr_space);
    return No_exp;
}

/**
 * @brief new instance for addr_space_t, Note that the instance is put to conf_obj hash table.
 *
 * @param obj_name the instance name
 *
 * @return  new instance
 */
addr_space_t *new_addr_space(char *obj_name)
{
    addr_space_t *space = skyeye_mm_zero(sizeof (addr_space_t));

    space->obj = new_map_object(obj_name, space, MapObject);
    space->memory_space = skyeye_mm_zero(sizeof (memory_space_intf));
    space->memory_space->conf_obj = space->obj;
    space->memory_space->read = space_read;
    space->memory_space->write = space_write;
    space->map_array = NULL;
    space->map_count = 0;
    space->ram_map_array = NULL;
    space->ram_map_count = 0;
    init_breakpoint_manager(&space->bp_manager);
    return space;
}

void init_addr_space(char *objname, addr_space_t * space)
{
    return NULL;
}

int initialize_addr_space(addr_space_t * space)
{
    space->memory_space = skyeye_mm_zero(sizeof (memory_space_intf));
    space->map_count = 0;
    space->map_array = NULL;
    space->ram_map_array = NULL;
    space->memory_space->read = SKY_space_read;
    space->memory_space->write = SKY_space_write;

    return 0;
}

void free_addr_space(char *obj_name)
{
    return;
}

exception_t _add_map(conf_object_t * obj, conf_object_t * device, uint64_t base_addr, generic_address_t length,
                     generic_address_t start, memory_space_intf * memory_space, int priority, int swap_endian)
{
    addr_space_t *space = obj->obj;
    map_info_t *map = skyeye_mm(sizeof (map_info_t));

    map->obj = device;
    map->base_addr = base_addr;
    map->length = length;
    map->start = start;
    map->memory_space = memory_space;
    map->priority = priority;
    RWLOCK_INIT(map->lock);

    space->map_array = skyeye_realloc(space->map_array, sizeof (map_info_t *) * (++space->map_count));
    if (space->map_array == NULL)
    {
        skyeye_log(Warning_log, __FUNCTION__, "skyeye_realloc failed\n");
        return Malloc_exp;
    }
    space->map_array[space->map_count - 1] = map;
    skyeye_log(Info_log, __FUNCTION__, "Map %s successfully\n", device->objname);
    qsort(space->map_array, space->map_count, sizeof (map_info_t *), map_compare);
    ram_quick_access(space, memory_space, map);
    return No_exp;
}

addr_space_t *memory_space_get_addr_space(conf_object_t * obj)
{
    addr_space_t *space = obj->obj;

    return space;

}

exception_t memory_space_add_map(conf_object_t * obj, conf_object_t * device, uint64_t base_addr,
                                 generic_address_t length)
{
    memory_space_intf *io_memory = (memory_space_intf *) SKY_get_iface(device, MEMORY_SPACE_INTF_NAME);

    if (io_memory == NULL)
    {
        skyeye_log(Warning_log, __FUNCTION__, "Get 'memory_space' interface from %s fail.\n", device->objname);
        return Not_found_exp;
    }
    return _add_map(obj, device, base_addr, length, 0x0, io_memory, 1, 1);
}

exception_t memory_space_add_map_group(conf_object_t * obj, conf_object_t * device, uint64_t base_addr,
                                       generic_address_t length, uint32_t index)
{
    memory_space_intf *io_memory = (memory_space_intf *) SKY_get_iface(device, MEMORY_SPACE_INTF_NAME);

    if (io_memory == NULL)
    {
        skyeye_log(Warning_log, __FUNCTION__, "Get 'memory_space' interface from %s fail.\n", device->objname);
        return Not_found_exp;
    }

    return _add_map(obj, device, base_addr, length, 0x0, &io_memory[index], 1, 1);
}

exception_t memory_space_read(conf_object_t * obj, generic_address_t addr, void *buf, size_t count)
{

    addr_space_t *space = obj->obj;
    exception_t ret;
    uint32_t inter;

    //for(inter = 0; inter < count; inter++)
    //       hit_pgd_item(space->mm_monitor, addr + inter);

    memory_read_hit_pgd_item(space->memory_monitor, addr, buf, count);

    if (IS_EXTRA_CORE)
    {
        return space->extra_iface->read(space->extra_core, addr, buf, count);
    }
    map_info_t key;

    key.base_addr = addr;
    map_info_t *pkey = &key;
    map_info_t **iterator;

    iterator = bsearch(&pkey, space->map_array, space->map_count, sizeof (map_info_t *), op_compare);
    if (iterator != NULL)
    {
        RW_RDLOCK(((*iterator)->lock));
        if ((*iterator)->memory_space == NULL)
        {
            skyeye_log(Warning_log, __FUNCTION__, "%s memory_space iface is NULL!\n", (*iterator)->obj->objname);
            return Not_found_exp;
        }
        ret = (*iterator)->memory_space->read((*iterator)->obj, (addr - (*iterator)->base_addr), buf, count);
        RW_UNLOCK(((*iterator)->lock));
        return ret;
    } else
    {
        skyeye_log(Warning_log, __FUNCTION__, "Can't access the address 0x%x\n", addr);
    }

    return Not_found_exp;
}

extern uint32_t common_class_var;
exception_t memory_space_write(conf_object_t * obj, generic_address_t addr, void *buf, size_t count)
{
    addr_space_t *space = obj->obj;
    exception_t ret;
    uint32_t inter;

    /*
     * if(common_class_var){
     * if((addr >= common_class_var) && (addr <= common_class_var + 10))
     * printf("addr = %x\n", addr);
     * }
     * 
     */
    /*
     * #ifdef __WIN32__
     * for(inter = 0; inter < count; inter++)
     * hit_pgd_item(space->mm_monitor, addr + inter, *((uint8_t *)buf + inter));
     * #endif
     */

    ret = Not_found_exp;

    if (IS_EXTRA_CORE)
        return space->extra_iface->write(space->extra_core, addr, buf, count);
    if (IS_DMA_CONNECTED)
    {
        space->dma_iface->dma_write(space->dma, addr, buf, count);
    }
    map_info_t key;

    key.base_addr = addr;
    map_info_t *pkey = &key;
    map_info_t **iterator;

    iterator = bsearch(&pkey, space->map_array, space->map_count, sizeof (map_info_t *), op_compare);
    if (iterator != NULL)
    {
        RW_WRLOCK(((*iterator)->lock));
        if ((*iterator)->memory_space == NULL)
        {
            skyeye_log(Warning_log, __FUNCTION__, "%s memory_space iface is NULL!\n", (*iterator)->obj->objname);
            return Not_found_exp;
        }
        ret = (*iterator)->memory_space->write((*iterator)->obj, (addr - (*iterator)->base_addr), buf, count);
        RW_UNLOCK(((*iterator)->lock));

    } else
    {

        skyeye_log(Warning_log, __FUNCTION__, "Can't access the address 0x%x\n", addr);
    }

    memory_write_hit_pgd_item(space->memory_monitor, addr, buf, count);

    return ret;
}

exception_t memory_space_read_hl(conf_object_t * obj, generic_address_t addr, void *buf, size_t count, int hl_flag)
{
    addr_space_t *space = obj->obj;
    exception_t ret;

    memory_read_hit_pgd_item(space->memory_monitor, addr, buf, count);

    if (IS_EXTRA_CORE)
    {
        return space->extra_iface->read(space->extra_core, addr, buf, count);
    }
    map_info_t key;

    key.base_addr = addr;
    map_info_t *pkey = &key;
    map_info_t **iterator;

    iterator = bsearch(&pkey, space->map_array, space->map_count, sizeof (map_info_t *), op_compare);
    if (iterator != NULL)
    {
        RW_RDLOCK(((*iterator)->lock));
        if ((*iterator)->memory_space == NULL)
        {
            skyeye_log(Warning_log, __FUNCTION__, "%s memory_space iface is NULL!\n", (*iterator)->obj->objname);
            return Not_found_exp;
        }
        ret = (*iterator)->memory_space->read_hl((*iterator)->obj, (addr - (*iterator)->base_addr), buf, count, hl_flag);
        RW_UNLOCK(((*iterator)->lock));
        return ret;
    } else
    {
        skyeye_log(Warning_log, __FUNCTION__, "Can't access the address 0x%x\n", addr);
    }

    return Not_found_exp;
}

exception_t memory_space_write_hl(conf_object_t * obj, generic_address_t addr, void *buf, size_t count, int hl_flag)
{
    addr_space_t *space = obj->obj;
    exception_t ret;

    if (IS_EXTRA_CORE)
        return space->extra_iface->write(space->extra_core, addr, buf, count);
    if (IS_DMA_CONNECTED)
    {
        space->dma_iface->dma_write(space->dma, addr, buf, count);
    }
    map_info_t key;

    key.base_addr = addr;
    map_info_t *pkey = &key;
    map_info_t **iterator;

    iterator = bsearch(&pkey, space->map_array, space->map_count, sizeof (map_info_t *), op_compare);
    if (iterator != NULL)
    {
        RW_WRLOCK(((*iterator)->lock));
        if ((*iterator)->memory_space == NULL)
        {
            skyeye_log(Warning_log, __FUNCTION__, "%s memory_space iface is NULL!\n", (*iterator)->obj->objname);
            return Not_found_exp;
        }
        ret = (*iterator)->memory_space->write_hl((*iterator)->obj, (addr - (*iterator)->base_addr), buf, count, hl_flag);
        RW_UNLOCK(((*iterator)->lock));
        return ret;
    } else
    {
        skyeye_log(Warning_log, __FUNCTION__, "Can't access the address 0x%x\n", addr);
    }

    memory_write_hit_pgd_item(space->memory_monitor, addr, buf, count);
    return Not_found_exp;
}

exception_t memory_space_write_addr64(conf_object_t * obj, uint64_t addr, void *buf, size_t count)
{
    addr_space_t *space = obj->obj;
    exception_t ret;

    ret = Not_found_exp;

    if (IS_EXTRA_CORE)
        return space->extra_iface->write(space->extra_core, addr, buf, count);
    if (IS_DMA_CONNECTED)
    {
        space->dma_iface->dma_write(space->dma, addr, buf, count);
    }
    map_info_t key;

    key.base_addr = addr;
    map_info_t *pkey = &key;
    map_info_t **iterator;

    iterator = bsearch(&pkey, space->map_array, space->map_count, sizeof (map_info_t *), op_compare);
    if (iterator != NULL)
    {
        RW_WRLOCK(((*iterator)->lock));
        if ((*iterator)->memory_space == NULL)
        {
            skyeye_log(Warning_log, __FUNCTION__, "%s memory_space iface is NULL!\n", (*iterator)->obj->objname);
            return Not_found_exp;
        }
        ret = (*iterator)->memory_space->write((*iterator)->obj, (addr - (*iterator)->base_addr), buf, count);
        RW_UNLOCK(((*iterator)->lock));

    } else
    {

        skyeye_log(Warning_log, __FUNCTION__, "Can't access the address 0x%x\n", addr);
    }

    memory_write_hit_pgd_item(space->memory_monitor, addr, buf, count);

    return ret;
}

exception_t memory_space_read_addr64(conf_object_t * obj, uint64_t addr, void *buf, size_t count)
{

    addr_space_t *space = obj->obj;
    exception_t ret;
    uint32_t inter;

    memory_read_hit_pgd_item(space->memory_monitor, addr, buf, count);

    if (IS_EXTRA_CORE)
    {
        return space->extra_iface->read(space->extra_core, addr, buf, count);
    }
    map_info_t key;

    key.base_addr = addr;
    map_info_t *pkey = &key;
    map_info_t **iterator;

    iterator = bsearch(&pkey, space->map_array, space->map_count, sizeof (map_info_t *), op_compare);
    if (iterator != NULL)
    {
        RW_RDLOCK(((*iterator)->lock));
        if ((*iterator)->memory_space == NULL)
        {
            skyeye_log(Warning_log, __FUNCTION__, "%s memory_space iface is NULL!\n", (*iterator)->obj->objname);
            return Not_found_exp;
        }
        ret = (*iterator)->memory_space->read((*iterator)->obj, (addr - (*iterator)->base_addr), buf, count);
        RW_UNLOCK(((*iterator)->lock));
        return ret;
    } else
    {
        skyeye_log(Warning_log, __FUNCTION__, "Can't access the address 0x%x\n", addr);
    }

    return Not_found_exp;
}

get_page_t *memory_space_get_page(conf_object_t * obj, generic_address_t addr)
{
    addr_space_t *space = obj->obj;

    if (IS_EXTRA_CORE)
        return space->extra_iface->get_page(space->extra_core, addr);
    map_info_t map;

    map.base_addr = addr;
    ram_array_t key;
    ram_array_t *pkey;

    key.map = &map;
    pkey = &key;
    ram_array_t **iterator;

    iterator = bsearch(&pkey, space->ram_map_array, space->ram_map_count, sizeof (ram_array_t *), ram_op_compare);
    DBG("In %s, i=%d, addr=0x%x, base_addr=0x%x, length=0x%x\n", __FUNCTION__, i, addr, iterator->base_addr,
        iterator->length);
    if (iterator != NULL)
    {
        return (*iterator)->iface->get_page((*iterator)->map->obj, addr - (*iterator)->map->base_addr);
    } else
    {
        DBG("In %s, can't access the address 0x%x\n", __func__, addr);
    }
    return NULL;
}

static conf_object_t *new_memory_space(char *obj_name)
{
    addr_space_t *dev = skyeye_mm_zero(sizeof (addr_space_t));

    dev->obj = new_conf_object(obj_name, dev);
    dev->map_count = 0;
    dev->map_array = NULL;
    dev->ram_map_count = 0;
    dev->ram_map_array = NULL;
    dev->extra_core = NULL;
    dev->extra_iface = NULL;
    dev->dma = NULL;
    dev->dma_iface = NULL;
    dev->mm_monitor = create_pgd();
    dev->memory_monitor = create_memory_pgd();
    return dev->obj;
}

static void free_memory_space(conf_object_t * obj)
{
    addr_space_t *dev = obj->obj;

    skyeye_free(dev->ram_map_array);
    skyeye_free(dev->extra_core);
    skyeye_free(dev->extra_iface);
    skyeye_free(dev);
    skyeye_free(obj);
    return;
}

static attr_value_t get_attr_device(void *arg, conf_object_t * conf_obj, attr_value_t * idx)
{
    addr_space_t *dev = conf_obj->obj;
    attr_value_t value;

    return value;
}

void memory_space_register_attribute(conf_class_t * clss)
{

}

exception_t *memory_space_set_fault_injection(conf_object_t * obj, generic_address_t addr, int bitnum, int mode)
{
    addr_space_t *space = obj->obj;
    map_info_t key;

    key.base_addr = addr;
    map_info_t *pkey = &key;
    map_info_t **iterator;
    char plug_name[256];

    iterator = bsearch(&pkey, space->map_array, space->map_count, sizeof (map_info_t *), op_compare);

    if (iterator != NULL)
    {
        conf_object_t *plug_obj;
        conf_object_t *tmp_obj = NULL;

        plug_obj = (*iterator)->obj;
        if (SKY_get_iface(plug_obj, MEMORY_SPACE_PLUG_INTF_NAME) == NULL)
        {
            tmp_obj = (*iterator)->obj;
            sprintf(plug_name, "%s_%s", "memroy_space_plug0", tmp_obj->objname);
            plug_obj = pre_conf_obj(plug_name, "memory_space_plug");
            (*iterator)->obj = plug_obj;
            (*iterator)->memory_space = SKY_get_iface(plug_obj, MEMORY_SPACE_PLUG_INTF_NAME);
        }

        memory_space_plug_save_intf *plug_intf = SKY_get_iface(plug_obj, MEMORY_SPACE_PLUG_SAVE_INTF_NAME);

        if (plug_intf == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can't get %s  from %s object\n", MEMORY_SPACE_PLUG_SAVE_INTF_NAME,
                       plug_obj);
        }
        plug_intf->save_plug_obj(plug_obj, tmp_obj, addr, bitnum, mode);

    } else
    {
        skyeye_log(Warning_log, __FUNCTION__, "Can't find device on address 0x%x\n", addr);
    }

    return No_exp;
}

exception_t *memory_space_clear_fault_injection(conf_object_t * obj, generic_address_t addr, int bitnum, int mode)
{
    addr_space_t *space = obj->obj;
    conf_object_t *tmp_obj;
    map_info_t key;

    key.base_addr = addr;
    map_info_t *pkey = &key;
    map_info_t **iterator;

    iterator = bsearch(&pkey, space->map_array, space->map_count, sizeof (map_info_t *), op_compare);

    if (iterator != NULL)
    {
        conf_object_t *plug_obj = (*iterator)->obj;
        memory_space_plug_save_intf *plug_intf = SKY_get_iface(plug_obj, MEMORY_SPACE_PLUG_SAVE_INTF_NAME);

        if (plug_intf)
        {
            tmp_obj = plug_intf->clear_plug_obj(plug_obj, addr, bitnum, mode);
        }
    } else
    {
        skyeye_log(Warning_log, __FUNCTION__, "Can't find device on address 0x%x\n", addr);
    }

    return No_exp;
}

void *memory_space_get_fault_injection(conf_object_t * obj, generic_address_t addr)
{
    addr_space_t *space = obj->obj;
    map_info_t key;

    key.base_addr = addr;
    map_info_t *pkey = &key;
    map_info_t **iterator;

    iterator = bsearch(&pkey, space->map_array, space->map_count, sizeof (map_info_t *), op_compare);

    if (iterator != NULL)
    {
        conf_object_t *plug_obj;

        plug_obj = (*iterator)->obj;
        memory_space_plug_save_intf *plug_intf = SKY_get_iface(plug_obj, MEMORY_SPACE_PLUG_SAVE_INTF_NAME);

        if (plug_intf)
        {
            return plug_intf->get_plug_obj(plug_obj);
        }

    } else
    {
        skyeye_log(Warning_log, __FUNCTION__, "Can't find device on address 0x%x\n", addr);
    }

    return NULL;

}

exception_t memory_space_set_extra_core(conf_object_t * obj, conf_object_t * core, memory_space_intf * iface)
{
    addr_space_t *space = obj->obj;

    space->extra_core = core;
    space->extra_iface = iface;

    return No_exp;
}

exception_t memory_space_remove_extra_core(conf_object_t * obj)
{
    addr_space_t *space = obj->obj;

    space->extra_core = NULL;
    space->extra_iface = NULL;

    return No_exp;
}

static exception_t dma_set(conf_object_t * obj, conf_object_t * obj2, const char *port, int index)
{
    addr_space_t *space = obj->obj;

    space->dma = obj2;
    space->dma_iface = (dma_intf *) SKY_get_iface(obj2, DMA_INTF);
    if (space->dma_iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get %s interface from %s fail.", DMA_INTF, obj2->objname);
    }
    return No_exp;
}

static exception_t dma_get(conf_object_t * obj, conf_object_t ** obj2, const char **port, int index)
{
    addr_space_t *space = obj->obj;

    *obj2 = space->dma;
    *port = NULL;
    return No_exp;
}

void init_memory_space()
{
    static skyeye_class_t class_data = {
        .class_name = "memory_space",
        .class_desc = "memory_space",
        .new_instance = new_memory_space,
        .free_instance = free_memory_space,
        .get_attr = NULL,
        .set_attr = NULL
    };
    conf_class_t *clss = SKY_register_device_class(class_data.class_name, &class_data);
    static const memory_space_intf io_memory = {
        .read = memory_space_read,
        .write = memory_space_write,
        .get_page = memory_space_get_page,
        .read_hl = memory_space_read_hl,
        .write_hl = memory_space_write_hl,
        .write_addr64 = memory_space_write_addr64,
        .read_addr64 = memory_space_read_addr64
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

    static const memory_space_set_plug_intf set_fault_injection = {
        .set_plug = memory_space_set_fault_injection,
        .get_plug = memory_space_get_fault_injection,
        .clear_plug = memory_space_clear_fault_injection
    };
    SKY_register_iface(clss, MEMORY_SPACE_SET_PLUG_INTF_NAME, &set_fault_injection);

    static const memory_space_extra_core_intf extra_core = {
        .set_core = memory_space_set_extra_core,
        .remove_core = memory_space_remove_extra_core
    };
    SKY_register_iface(clss, MEMORY_SPACE_EXTRA_CORE_NAME, &extra_core);

    static const memory_space_add_map_intf add_map = {
        .add_map = memory_space_add_map,
        .add_map_group = memory_space_add_map_group,
        .get_addr_space = memory_space_get_addr_space
    };
    SKY_register_iface(clss, MEMORY_SPACE_ADD_MAP_NAME, &add_map);

    static const struct InterfaceDescription ifaces[] = {
        (struct InterfaceDescription) {
            .name = MEMORY_SPACE_INTF_NAME,
            .iface = &io_memory,
        },
        (struct InterfaceDescription) {
            .name = MEMORY_SPACE_SET_PLUG_INTF_NAME,
            .iface = &set_fault_injection,
        },
        (struct InterfaceDescription) {
            .name = MEMORY_SPACE_EXTRA_CORE_NAME,
            .iface = &extra_core,
        },
        (struct InterfaceDescription) {
            .name = MEMORY_SPACE_ADD_MAP_NAME,
            .iface = &add_map,
        }
    };
    static const struct ConnectDescription connects[] = {
        (struct ConnectDescription) {
            .name = MEMORY_SPACE_INTF_NAME,
            .set = NULL,
            .get = NULL,
        },
        (struct ConnectDescription) {
            .name = DMA_INTF,
            .set = dma_set,
            .get = dma_get,
        }
    };
    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);

    memory_space_register_attribute(clss);
    return NULL;
}
