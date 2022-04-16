#ifndef __AD_LINKER_H
#define __AD_LINKER_H


#define DEV_MAX_COUNT  31
typedef struct ad_linker
{
    conf_object_t *obj;

    conf_object_t *device_list[DEV_MAX_COUNT];       /*all device link the fifo*/
    ad_dev_intf *device_iface_list[DEV_MAX_COUNT];
    uint32_t dev_cnt;
    uint32_t get_dev_cnt;
}ad_linker_t;

#endif
