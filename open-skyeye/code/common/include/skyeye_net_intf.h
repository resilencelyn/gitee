#ifndef __SKYEYE_NET_INTF_H__
#define __SKYEYE_NET_INTF_H__
struct net_device
{
	conf_object_t* obj;
	int net_fd;
	unsigned char macaddr[6];
	unsigned char hostip[4];
	int ethmod;
	int name_index;

	/* private data */
	void *priv;
};

typedef struct net_ops{
	int (*net_open) (struct net_device * net_dev);
	int (*net_close) (struct net_device * net_dev);
	int (*net_read) (struct net_device * net_dev, void *buf, size_t count);
	int (*net_write) (struct net_device * net_dev, void *buf, size_t count);
	int (*net_wait_packet) (struct net_device * net_dev, struct timeval *tv);
}skyeye_net_ops_intf;

#define SKYEYE_NET_INTF_NAME "skyeye_net_ops_intf"
#endif
