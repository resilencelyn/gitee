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
#include <skyeye_emif.h>
#include <sim_control.h>
#include <skyeye_dev_bus_intf.h>

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

#define DEBUG
#include <skyeye_log.h>
#include "rs422_bridge.h"
#include "rs422_bridge_fifo.h"

#define MAX_CAN_FIFO_LEN 10000
#define BUFFER_SIZE 0x1024

int unpack_rs422_frame(unsigned char *dst, unsigned char *src, int length){
    int size = length + 8;
    dst[0] = '#';
    dst[1] = 1;
    dst[2] = '@';
    dst[3] = src[0];
    dst[4] = 0;
    dst[5] = 0;
    dst[6] = 0;
    memcpy(&dst[7], &src[1], length);
    dst[size - 1] = '$';
    return size;
}

int make_rs422_package(unsigned char *dst, unsigned char *src, int length){
    if (src[0] == '#' && src[1] == 1 && src[2] == '@' && src[length - 1] == '$'){
        dst[0] = src[3];
        memcpy(&dst[1], &src[7], length - 8);
        return length - 8;
    }

    return -1;
}
static int recv_data_from_rs422(conf_object_t *obj, unsigned char *buf, int length){
	rs422_bridge_device* dev = obj->obj;
    unsigned char frame_msg[1024] = {0};
	int size;
    
    size = unpack_rs422_frame(frame_msg, buf, length);
    send(dev->sv_socket, frame_msg, size, 0);
#ifdef __MINGW32__
    Sleep(10);
#endif

	return 0;
}

int start_recv_rs422_data(conf_object_t* obj){
	rs422_bridge_device* dev = obj->obj;

#ifdef __MINGW32__
    WORD sockVersion = MAKEWORD(2,2);
	WSADATA data;
    char connect_msg[] = {'#', 0, '@', 2, '$'};

	if(WSAStartup(sockVersion, &data) != 0)
	{
		return Invarg_exp;
	}

	dev->sv_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(dev->sv_socket == INVALID_SOCKET)
	{
		printf("In %s ,create socket failed!\n", __func__);
		return Invarg_exp;
	}

	struct sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(dev->server_port);
	serAddr.sin_addr.S_un.S_addr = inet_addr(dev->server_ip);
	if (connect(dev->sv_socket, (SOCKADDR *)&serAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		printf("In %s, connect stimulate ui failed !\n", __func__);
		closesocket(dev->sv_socket);
		return Invarg_exp;
	}
    //给数据激励界面发送握手命令
    send(dev->sv_socket, connect_msg, 5, 0);

	while(1){
        if (SIM_is_running()){
            unsigned char recData[1024] = {0};
            unsigned char sendData[1024] = {0};
            int i, length;
            int ret = recv(dev->sv_socket, recData, 128, 0);

            if (ret > 0){
                length = make_rs422_package(sendData, recData, ret);
                //发送数据给串口设备 
               	if(dev->bus_iface != NULL){
	                dev->bus_iface->write_data(dev->bus_obj, obj, sendData, length);
	            }
            }
        }
        Sleep(1000);
    }
#endif
}

static attr_value_t get_attr_server_ip(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	rs422_bridge_device* dev = conf_obj->obj;
	attr_value_t ip = SKY_make_attr_string(dev->server_ip);
	return ip;
}

static exception_t set_attr_server_ip(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	rs422_bridge_device* dev = conf_obj->obj;
	char *server_ip;
	server_ip = SKY_attr_string(*value);
    strncpy(dev->server_ip, server_ip, IP_LENGTH);

	return No_exp;
}

static attr_value_t get_attr_server_port(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	rs422_bridge_device* dev = conf_obj->obj;
	attr_value_t server_port = SKY_make_attr_uinteger(dev->server_port);
	return server_port;
}

static exception_t set_attr_server_port(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	rs422_bridge_device* dev = conf_obj->obj;
	dev->server_port = SKY_attr_uinteger(*value);
	return No_exp;
}

static load_file_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "server_ip", get_attr_server_ip, NULL, set_attr_server_ip, NULL, SKY_Attr_Optional, "string", "server ip");
	SKY_register_attribute(clss, "server_port", get_attr_server_port, NULL, set_attr_server_port, NULL, SKY_Attr_Optional, "uinteger", "server port");
}

static conf_object_t* new_rs422_bridge(char *obj_name)
{
	rs422_bridge_device* dev = skyeye_mm_zero(sizeof(rs422_bridge_device));
	dev->obj = new_conf_object(obj_name, dev);
	dev->rec_fifo = CreateFIFO(MAX_CAN_FIFO_LEN);
	return dev->obj;
}

static exception_t free_rs422_bridge(conf_object_t* obj)
{
	rs422_bridge_device* dev = obj->obj;
#ifdef __MINGW32__
    closesocket(dev->sv_socket);
    WSACleanup();
#endif
	FreeFIFO(dev->rec_fifo);
	skyeye_free(obj->objname);
	skyeye_free(obj->obj);
	skyeye_free(obj);
	return No_exp;
}

static exception_t config_rs422_bridge(conf_object_t* obj)
{        
#ifdef __MINGW32__
 	CreateThread(NULL, 0, start_recv_rs422_data, obj, 0, NULL);
#endif
}

static exception_t rs422_bridge_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	rs422_bridge_device* dev = (rs422_bridge_device*)(obj->obj);
	dev->bus_obj = obj2;
	dev->bus_iface = (Rs422_bus_intf*)SKY_get_iface(dev->bus_obj, RS422_BUS_INTF);

	if (dev->bus_iface == NULL)
	{
		if (dev->bus_obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", RS422_BUS_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", RS422_BUS_INTF, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t rs422_bridge_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	rs422_bridge_device* dev = (rs422_bridge_device*)(obj->obj);
	*obj2 = dev->bus_obj;
	*port = NULL;
	return No_exp;
}

void init_rs422_bridge(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "rs422_bridge",
		.class_desc = "rs422_bridge",
		.new_instance = new_rs422_bridge,
		.free_instance = free_rs422_bridge,
		.config_instance = config_rs422_bridge,
		.module_type = SKYML_OFFCHIP,
	};
	conf_class_t* clss = SKY_register_linker_class(class_data.class_name, &class_data);

	static const Rs422_emif_intf rs422_emif_interface = {
		.set_state = recv_data_from_rs422,
	};
	SKY_register_iface(clss, RS422_EMIF_INTF, &rs422_emif_interface);

	static const struct InterfaceDescription ifaces[] = 
	{
	};
	static const struct ConnectDescription connects[] = 
	{
		/* ConnectDescription */
		(struct ConnectDescription) {
			.name = RS422_BUS_INTF,
			.set = rs422_bridge_set,
			.get = rs422_bridge_get,
		},

	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

    load_file_register_attribute(clss);
}
