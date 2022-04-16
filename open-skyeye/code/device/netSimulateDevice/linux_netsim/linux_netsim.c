/* Copyright (C) 
* 2016 - Michael.Kang blackfin.kang@gmail.com
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
* @file skyeye_net_tuntap.c
* @brief 
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2016-05-06
*/

#include <net/if.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include <skyeye_class.h>
#include <skyeye_net_intf.h>
#include <skyeye_iface.h>
#include <skyeye_mm.h>
#include <skyeye_attr.h>
#include <skyeye_attribute.h>
#define DEBUG 0
#if DEBUG
#define DBG_PRINT(a...) fprintf(stderr, ##a)
#else
#define DBG_PRINT(a...)
#endif

#define DEFAULT_TUNTAP_IF_NAME "tap"

static int
tuntap_open (struct net_device *net_dev)
{
	int tapif_fd;
	struct ifreq ifr;
	char buf[128];
	char if_name[10];


	DBG_PRINT ("tapif_init begin\n");

	tapif_fd = open ("/dev/net/tun", O_RDWR | O_NONBLOCK);
	DBG_PRINT ("tapif_init: fd %d\n", tapif_fd);
	if (tapif_fd < 0) {
		printf ("-----------------------------------------------------------\n");
		printf ("NOTICE: you should be root at first !!!\n");
		printf ("NOTICE: you should inmod linux kernel net driver tun.o!!!\n");
		printf ("NOTICE: if you don't make device node, you should do commands:\n");
		printf ("NOTICE:    mkdir /dev/net; mknod /dev/net/tun c 10 200\n");
		printf ("NOTICE: now the net simulation function can not support!!!\n");
		printf ("NOTICE: Please read SkyEye.README and try again!!!\n");
		printf ("-----------------------------------------------------------\n");
		return 1;
	}

	snprintf (if_name, sizeof (if_name), "%s%d", DEFAULT_TUNTAP_IF_NAME,
		  net_dev->name_index);

	memset (&ifr, 0, sizeof (ifr));
	ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
	strncpy (ifr.ifr_name, if_name, IFNAMSIZ);

	if (ioctl (tapif_fd, TUNSETIFF, (void *) &ifr) < 0) {
		printf ("tapif_init: icotl TUNSETIFF error\n");
		printf ("NOTICE: you should be root at first !!!\n");
		return 1;
	}
	snprintf(buf, sizeof(buf), "ifconfig %s hw ether %02x:%02x:%02x:%02x:%02x:%02x",
			if_name, net_dev->macaddr[0], net_dev->macaddr[1],
			net_dev->macaddr[2], net_dev->macaddr[3],
			net_dev->macaddr[4], net_dev->macaddr[5]);
	DBG_PRINT ("tapif_init: system(\"%s\");\n", buf);
	system (buf);

	snprintf (buf, sizeof (buf), "ifconfig %s inet %d.%d.%d.%d",
		  if_name, net_dev->hostip[0], net_dev->hostip[1],
		  net_dev->hostip[2], net_dev->hostip[3]);

	DBG_PRINT ("tapif_init: system(\"%s\");\n", buf);

	system (buf);

	net_dev->net_fd = tapif_fd;

	net_dev->name_index++;

	DBG_PRINT ("tapif_init end\n");

	return 0;
}


static int
tuntap_close (struct net_device *net_dev)
{
	close (net_dev->net_fd);
	return 0;
}

static int
tuntap_read (struct net_device *net_dev, void *buf, size_t count)
{
	return read (net_dev->net_fd, buf, count);
}

static int
tuntap_write (struct net_device *net_dev, void *buf, size_t count)
{
	return write (net_dev->net_fd, buf, count);
}

static int
tuntap_wait_packet (struct net_device *net_dev, struct timeval *tv)
{
	fd_set frds;
	int ret;

	FD_ZERO(&frds);
	FD_SET(net_dev->net_fd, &frds);
	if((ret = select(net_dev->net_fd + 1, &frds, NULL, NULL, tv)) <= 0) return -1;
	if(!FD_ISSET(net_dev->net_fd, &frds)) return -1;

	return 0;
}

static conf_object_t* new_linux_netsim(const char* obj_name){
	struct net_device* dev = skyeye_mm_zero(sizeof(struct net_device));
	dev->obj = new_conf_object(obj_name, dev);
	dev->hostip[0] = 192;
	dev->hostip[1] = 168;
	dev->hostip[2] = 1;
	dev->hostip[3] = 55;

	dev->macaddr[0] = 0xc6;
	dev->macaddr[1] = 0xda;
	dev->macaddr[2] = 0xc0;
	dev->macaddr[3] = 0x0c;
	dev->macaddr[4] = 0x9d;
	dev->macaddr[5] = 0x75;
	return dev->obj;
}
static exception_t free_linux_netsim(conf_object_t* conf_obj){
	struct net_device *dev = conf_obj->obj;

	tuntap_close(dev);
	skyeye_free(conf_obj->objname);
	skyeye_free(conf_obj->obj);
	skyeye_free(conf_obj);
	return No_exp;
}

static attr_value_t get_attr_ip(void* arg, conf_object_t* conf_obj, attr_value_t* idx) {
	struct net_device *dev = conf_obj->obj;
	char ip[16] = {0};

	snprintf(ip, sizeof(ip), "%d.%d.%d.%d", dev->hostip[0], dev->hostip[1], dev->hostip[2], dev->hostip[3]);

	return SKY_make_attr_string(ip);
}
static exception_t set_attr_ip(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx) {
	struct net_device *dev = conf_obj->obj;
	const char *ip;

	ip = SKY_attr_string(*value);
	sscanf(ip, "%d.%d.%d.%d",
			(int *)(&dev->hostip[0]),
			(int *)(&dev->hostip[1]),
			(int *)(&dev->hostip[2]),
			(int *)(&dev->hostip[3]));
	return No_exp;
}
static void linux_netsim_register_attribute(conf_class_t* clss) {
	/* register some attribute, such as mac address */
#if 0
	SKY_register_attribute(clss,
			"ip",
			get_attr_ip,
			NULL,
			set_attr_ip,
			NULL,
			SKY_Attr_Optional,
			"string",
			"ip address for tap");
#endif
	return;
}

void init_linux_netsim(){
	static skyeye_class_t class_data = {
		.class_name = "netcard_simulation_level",
		.class_desc = "linux_netsim",
		.new_instance = new_linux_netsim,
		.free_instance = free_linux_netsim,
		.get_attr = NULL,
		.set_attr = NULL ,
		.module_type = SKYML_OFFCHIP,
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const skyeye_net_ops_intf net_ops_intf = {
		.net_open = tuntap_open,
		.net_close = tuntap_close,
		.net_read = tuntap_read,
		.net_write = tuntap_write,
		.net_wait_packet = tuntap_wait_packet,
	};

	SKY_register_iface(clss, SKYEYE_NET_INTF_NAME, &net_ops_intf);
	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription){
			.name = SKYEYE_NET_INTF_NAME,
			.iface = &net_ops_intf,
		},
 	};

	class_register_ifaces(clss, ifaces);

	linux_netsim_register_attribute(clss);
}
