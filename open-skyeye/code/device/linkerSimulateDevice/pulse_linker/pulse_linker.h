#ifndef __PULSE_LINKER_H
#define __PULSE_LINKER_H


#define DEV_MAX_COUNT  31
typedef struct pulse_linker{
    conf_object_t *obj; 

    conf_object_t *device_list[DEV_MAX_COUNT];       /*all device link the fifo*/
    pulse_dev_intf *device_iface_list[DEV_MAX_COUNT];
    char *send_buf[DEV_MAX_COUNT];
    char *recv_buf;
    uint32_t dev_cnt;
}pulse_linker_t;

#endif
