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
#include "can_exc_bridge.h"

#ifndef __MINGW32__
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#else
#include <windows.h>
#include <winsock2.h>
#endif

void close_client(int sockfd);


ret_t inter_init(inter_t *obj, int context_size)
{
	obj->context_size = context_size;
	obj->context = malloc(obj->context_size);
	if(obj->context == NULL)
		return MALLOC_ERR;
	obj->context_index = 0;
	obj->user_data = NULL;
	obj->fetch_func = NULL;
	obj->inter_status = INTER_OK;
	obj->context_status = STATUS_NONE;
	return NONE_ERR;
}

ret_t inter_free(inter_t *obj)
{
	free(obj->context);
	obj->context = NULL;
}

ret_t inter_register_callback(inter_t *obj, inter_context_fetch func, void *user_data)
{
	if(obj->inter_status != INTER_OK)
		return INTER_OK_ERR;
	obj->fetch_func = func;
	obj->user_data = user_data;
	return NONE_ERR;
}

int inter_input(conf_object_t *conf_obj, inter_t *obj, void *buf, int size)
{
	can_exc_bridge_device *dev = conf_obj->obj;

	char *ptr = (char *)buf;
	int  i;
	char ch;

	for (i = 0; i < size; i++) {
		ch = *(ptr + i);
		switch (obj->context_status) {
			case STATUS_NONE:
				if(ch == '#')
					obj->context_status = STATUS_1;
				break;
			case STATUS_1:
				obj->msg_type = ch;
				obj->context_status = STATUS_2;
				break;
			case STATUS_2:
				if(ch == '@')
					obj->context_status = STATUS_3;
				else
					obj->context_status = STATUS_NONE;
				break;
			case STATUS_3:
				if (ch == '$') {
					if(obj->fetch_func)
						obj->fetch_func((void *)conf_obj, obj->msg_type, obj->context, obj->context_index, obj->user_data);
					obj->context_index = 0;
					obj->context_status = STATUS_NONE;
					break;
				}
				obj->context[obj->context_index++] = ch;
				if (obj->context_index >= 64) {
					obj->context_index = 0;
					obj->context_status = STATUS_NONE;
				}
				break;
		}
	}
}

int generate_can_pack(void *pack, can_t *can)
{
	char *ptr = (char *)pack;
	int offset = 0;
	ptr[offset++] = '#';
	ptr[offset++] = 1;
	ptr[offset++] = '@';
	memcpy(ptr+offset, &(can->id), 4);
	offset+=4;
	ptr[offset++] = can->channel_id;
	ptr[offset++] = can->ide;
	ptr[offset++] = can->dlc;
	memcpy(ptr+offset, can->data, can->dlc);
	offset+=can->dlc;
	ptr[offset++] = '$';
	return offset;
}

int fetch_context(void *conf_obj, int msg_type,void *buf, int size,void *user_data)
{
	can_exc_bridge_device *dev = ((conf_object_t *)conf_obj)->obj;
	int i, n;
	char *ptr = (char *)buf;
	can_t can_pack;

	int sockfd = (int)user_data;
	can_msg_t msg;

	if(msg_type == 1){  //can包
		memcpy(&can_pack, ptr, sizeof(can_t));
		// 转换为can_msg_t包
		msg.id 	= can_pack.id;
		msg.channel_id = can_pack.channel_id;
		msg.ide = can_pack.ide;
		msg.dlc = can_pack.dlc;
		memcpy(msg.data, can_pack.data, can_pack.dlc);

		// 发送到can总线
		if (dev->can_linker_iface) {
			dev->can_linker_iface->send_msg(dev->can_linker, (conf_object_t *)conf_obj, &msg);
		}
/*
		printf("ID:%x\n",can_pack.id);
		printf("ide:%d\n",can_pack.ide);
		printf("dlc:%d\n",can_pack.dlc);
		for(i = 0;i < can_pack.dlc;i++){
			printf("%x ",can_pack.data[i]);
			can_pack.data[i] += 1;
		}
		printf("\n");
*/
	}
	return 0;
}

int generate_can_msg_pack(void *pack, can_msg_t *can)
{
	char *ptr = (char *)pack;
	int offset = 0;
	ptr[offset++] = '#';
	ptr[offset++] = 1;
	ptr[offset++] = '@';
	memcpy(ptr+offset, &(can->id), 4);
	offset+=4;
	ptr[offset++] = can->channel_id;
	ptr[offset++] = can->ide;
	ptr[offset++] = can->dlc;
	memcpy(ptr+offset, can->data, can->dlc);
	offset+=can->dlc;
	ptr[offset++] = '$';
	return offset;
}

static int can_sja1000_receive(conf_object_t *conf_obj, can_msg_t *msg){
	can_exc_bridge_device *dev = conf_obj->obj;
/*
	printf("\n=====%s=====\n", __func__);
	printf("id:%d\n", msg->id);
	printf("ide:%d\n", msg->ide);
	printf("dlc:%d\n", msg->dlc);
	int i;
	for (i = 0; i < msg->dlc; i++) {
		printf("0x%x ", msg->data[i]);
	}
	printf("\n\n");
*/
	unsigned char pack_buf[256];
	int n;
	n = generate_can_msg_pack(pack_buf, msg);

	send(dev->client_fd, pack_buf, n, 0);
	#ifdef __MINGW32__
  		Sleep(10);
	#endif

	return 0;
}


void client_recv(void *arg)
{
	can_exc_bridge_device *dev = ((conf_object_t *)arg)->obj;
	inter_t *ptr;
	ptr = &(dev->inter_obj);
	char receline[MAXLINE + 1];

	int i, n;
	while (1) {
#ifdef __MINGW32__
		while((n = recv(dev->client_fd, receline, MAXLINE,0)) > 0)
#else
		while((n = read(dev->client_fd, receline, MAXLINE)) > 0)
#endif
		{ 
			receline[n]=0;
			inter_input((conf_object_t *)arg, ptr, receline, n);
		}
	}
}

static exception_t client_connect(conf_object_t* conf_obj)
{
	can_exc_bridge_device *dev = conf_obj->obj;

	struct sockaddr_in serveraddr;
	int ret;
	inter_t *ptr;
	ptr = &(dev->inter_obj);
	ret = inter_init(ptr, 64);
	if(ret != NONE_ERR)
		printf("%s, %d, error number:%d\n", __func__, __LINE__, ret);

#ifdef __MINGW32__
    	WORD sockVersion = MAKEWORD(2,2);
	WSADATA data;

	if(WSAStartup(sockVersion, &data) != 0)
	{
		return Invarg_exp;
	}

	dev->client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(dev->client_fd == INVALID_SOCKET)
	{
		printf("In %s ,create socket failed!\n", __func__);
		return Invarg_exp;
	}

	struct sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(dev->server_port);
	serAddr.sin_addr.S_un.S_addr = inet_addr(dev->server_ip);
	
	if ((ret = connect(dev->client_fd, (SOCKADDR *)&serAddr, sizeof(SOCKADDR))) == SOCKET_ERROR)
	{
		printf("In %s, connect can stimulate ui failed !\n", __func__);
		closesocket(dev->client_fd);
		return Invarg_exp;
	}
	
    	//给数据激励界面发送握手命�?
	// 发送数据给前端
	char can[] = {'#', 0, '@', 0, '$'};
	
	//连接
	while (1){
		if (ret != -1) {
		// 接受前端的数据
			inter_register_callback(ptr, fetch_context, (void*)(dev->client_fd));

			// 接受前端的数�?
			pthread_create(&(dev->thread_id), NULL, client_recv, (void *)conf_obj);

			// 发送数据给前端
			send(dev->client_fd, can, 5, 0);
		}
		
		Sleep(1000);
	}
#endif
	return No_exp;
}

void close_client(int sockfd)
{

#ifdef __MINGW32__
	//Winsows下关闭socket
	closesocket(sockfd);
	WSACleanup();
#else
	close(sockfd);
#endif

}
static conf_object_t* new_can_exc_bridge(char *obj_name)
{
	can_exc_bridge_device* dev = skyeye_mm_zero(sizeof(can_exc_bridge_device));
	dev->obj = new_conf_object(obj_name, dev);

	dev->server_port = 0;	

	return dev->obj;
}

static exception_t free_can_exc_bridge(conf_object_t* opaque)
{
	can_exc_bridge_device *dev = opaque->obj;

	close_client(dev->client_fd);
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);


	return No_exp;
}

static attr_value_t get_attr_server_ip(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	can_exc_bridge_device *dev = conf_obj->obj;
	attr_value_t server_ip = SKY_make_attr_string(dev->server_ip);
	return server_ip;
}

static exception_t set_attr_server_ip(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	can_exc_bridge_device *dev = conf_obj->obj;
	char *server_ip;
	server_ip = SKY_attr_string(*value);
    	strncpy(dev->server_ip, server_ip, IP_LENGTH);
	return No_exp;
}

static attr_value_t get_attr_server_port(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	can_exc_bridge_device *dev = conf_obj->obj;
	attr_value_t server_port = SKY_make_attr_uinteger(dev->server_port);
	return server_port;
}

static exception_t set_attr_server_port(void* arg, conf_object_t* conf_obj,attr_value_t* value, attr_value_t* idx)
{
	can_exc_bridge_device *dev = conf_obj->obj;
	dev->server_port = SKY_attr_uinteger(*value);
	
	return No_exp;
}

static exception_t config_can_exc_bridge(conf_object_t* conf_obj)
{
	can_exc_bridge_device *dev = conf_obj->obj;
	if (dev->server_ip != NULL) {
#ifdef __MINGW32__
		CreateThread(NULL, 0, client_connect, conf_obj, 0, NULL);
#endif
	}
}

static class_register_attribute(conf_class_t* clss)
{
	SKY_register_attribute(clss, "server_ip"  , get_attr_server_ip  , NULL, set_attr_server_ip  , NULL, SKY_Attr_Optional, "string"  , "server ip");
	SKY_register_attribute(clss, "server_port", get_attr_server_port, NULL, set_attr_server_port, NULL, SKY_Attr_Optional, "uinteger", "server port");
}

static exception_t can_linker_set(conf_object_t *conf_obj, conf_object_t *obj2, const char *port, int index)
{
	can_exc_bridge_device *dev = conf_obj->obj;
	dev->can_linker = obj2;
	dev->can_linker_iface = (can_linker_intf *)SKY_get_iface(dev->can_linker, CAN_LINKER_INTF);
	return No_exp;

}

static exception_t can_linker_get(conf_object_t *conf_obj, conf_object_t **obj2, const char **port, int index)
{
	can_exc_bridge_device *dev = conf_obj->obj;
	*obj2 = dev->can_linker;
	*port = NULL;
	return No_exp;
}

void init_can_exc_bridge(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "can_exc_bridge",
		.class_desc = "can_exc_bridge",
		.new_instance = new_can_exc_bridge,
		.free_instance = free_can_exc_bridge,
		.config_instance = config_can_exc_bridge,
		.module_type = SKYML_OFFCHIP,
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const can_dev_intf can_intf = {
		.receive_msg = can_sja1000_receive,
	};
	SKY_register_iface(clss, CAN_DEV_INTF, &can_intf);

	static const struct InterfaceDescription ifaces[] =
	{
		(struct InterfaceDescription) {
			.name = CAN_DEV_INTF,
			.iface = &can_intf,
		},
	};

	static const struct ConnectDescription connects[] =
	{
		(struct ConnectDescription) {
			.name = CAN_LINKER_INTF,
			.set = can_linker_set,
			.get = can_linker_get,
		}
	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	class_register_attribute(clss);
}
