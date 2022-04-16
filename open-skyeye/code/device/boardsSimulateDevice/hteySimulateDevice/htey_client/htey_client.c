#include <skyeye_lock.h>
#include <skyeye_types.h>
#include <skyeye_sched.h>
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
#include "htey_client.h"

static exception_t htey_client_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	htey_client_device *dev = opaque->obj;
	switch (offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in htey_client\n", offset);
			break;
	}
	return 0;
}
static exception_t htey_client_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	htey_client_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in htey_client\n", offset);
			break;
	}
	return 0;
}
static char* htey_client_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t htey_client_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	htey_client_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t htey_client_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t htey_client_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	htey_client_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t htey_client_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_htey_client(char *obj_name)
{
	htey_client_device* dev = skyeye_mm_zero(sizeof(htey_client_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	dev->server_ip = NULL;	
	dev->server_port = 0;	
	return dev->obj;
}

static exception_t client_connect(conf_object_t* opaque)
{
	htey_client_device *dev = opaque->obj;
	
	struct sockaddr_in serveraddr;
	WSADATA wsaData;
	int err;

	err = WSAStartup(MAKEWORD(1, 0), &wsaData);
	if (err != 0) {
		perror("WSAStartup fail\n");
	}

	if ((dev->sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket fail\n");
	}

	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_addr.s_addr = inet_addr(dev->server_ip);
	serveraddr.sin_family = AF_INET;	
	serveraddr.sin_port = htons(dev->server_port);

	if (connect(dev->sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1) {
		perror("connect fail\n");
	}

	//send(dev->sockfd, "Hello", 5, 0);

	return No_exp;

}
static exception_t free_htey_client(conf_object_t* opaque)
{
	htey_client_device *dev = opaque->obj;
	closesocket(dev->sockfd);
	WSACleanup();
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

int htey_client_send(conf_object_t *conf_obj, void *buf, int len){
	htey_client_device *dev = conf_obj->obj;
	send(dev->sockfd, &buf, len, 0);

	return 0;
}

static attr_value_t get_ip(void* arg, conf_object_t* obj, attr_value_t* idx){
	htey_client_device *dev = obj->obj;
	attr_value_t name = SKY_make_attr_string("");
	return name;	
}

static exception_t set_ip(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx){
	htey_client_device *dev = obj->obj;
	dev->server_ip = SKY_attr_string(*value);	
	//printf("%s server_ip: %s\n", __func__, dev->server_ip);
	if (dev->server_port != 0) {
		client_connect(obj);
	}
	return No_exp;
}

static attr_value_t get_port(void* arg, conf_object_t* obj, attr_value_t* idx){
	htey_client_device *dev = obj->obj;
	attr_value_t name = SKY_make_attr_string("");
	return name;
}

static exception_t set_port(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx){
	htey_client_device *dev = obj->obj;
	dev->server_port = SKY_attr_uinteger(*value);
	//printf("%s server_port: %d\n", __func__, dev->server_port);
	if (dev->server_ip != NULL) {
		client_connect(obj);
	}
	return No_exp;
}

static void class_register_attribute(conf_class_t* clss) {
	SKY_register_attribute(clss, "server_ip", get_ip, NULL, set_ip, NULL, SKY_Attr_Optional, "string", "server ip");
	SKY_register_attribute(clss, "server_port", get_port, NULL, set_port, NULL, SKY_Attr_Optional, "uinteger", "server port");
	return;
}

void init_htey_client(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "htey_client",
		.class_desc = "htey_client",
		.new_instance = new_htey_client,
		.free_instance = free_htey_client
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = htey_client_read,
		.write = htey_client_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = htey_client_get_regvalue_by_id,
		.get_regname_by_id = htey_client_get_regname_by_id,
		.set_regvalue_by_id = htey_client_set_regvalue_by_id,
		.get_regnum = htey_client_get_regnum,
		.get_regid_by_name = htey_client_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);
	static htey_client_intf client_intf =
	{
		.client_send = htey_client_send,
	};
	SKY_register_iface(clss, HTEY_CLIENT_INTF, &client_intf);	
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
			.name = HTEY_CLIENT_INTF,
			.iface = &client_intf
		},		
	};
	static const struct ConnectDescription connects[] = 
	{
		/* ConnectDescription */
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	class_register_attribute(clss);	
}
