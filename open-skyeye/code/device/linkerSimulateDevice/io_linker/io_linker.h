#ifndef __IO_LINKER_H
#define __IO_LINKER_H


#define DEV_MAX_COUNT  31
typedef struct io_linker
{
    conf_object_t *obj;

    conf_object_t *device_list[DEV_MAX_COUNT];       /*all device link the fifo*/
    io_dev_intf *device_iface_list[DEV_MAX_COUNT];
    char *send_buf[DEV_MAX_COUNT];
    char *recv_buf;
    uint32_t dev_cnt;
}io_linker_t;

#endif
