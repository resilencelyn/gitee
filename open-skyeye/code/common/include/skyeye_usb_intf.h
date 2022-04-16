#ifndef __SKYEYE_USB_INTF_H__
#define __SKYEYE_USB_INTF_H__
#include <stdint.h>

typedef struct usb_host_ops{
    int (*usb_dev_connect)(conf_object_t *conf_obj);
    int (*usb_dev_disconnect)(conf_object_t *conf_obj);
} skyeye_usb_host_ops_intf;

typedef struct usb_sim_ops {
    int (*usb_handle_control)(conf_object_t *obj, uint8_t *src, int slen, uint8_t *buf, size_t *len);
    int (*usb_handle_scsi)(conf_object_t *obj, uint8_t *src, int slen, int ep_type, uint8_t *recvbuf, uint32_t *rlen);
} skyeye_usb_sim_ops_intf;

#define SKYEYE_USB_HOST_INTF "skyeye_usb_host_intf"
#define SKYEYE_USB_SIM_INTF "skyeye_usb_sim_intf"

#endif
