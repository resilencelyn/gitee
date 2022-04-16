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
#include "bu61580_bridge.h"
#include "bu61580_bridge_fifo.h"

#define MAX_FIFO_LEN 4096
#define BUFFER_SIZE 0x1024

int unpack_bu61580_frame(unsigned char *dst, network_mesg_t *msg, int count){
    int size, i;

    dst[0] = '#';
    dst[1] = 1;
    dst[2] = '@';
    dst[3] = msg->rt_addr;
    dst[4] = msg->sub_addr;
    dst[5] = msg->tx_rx_flag;

    if (count == BYTE_SIZE){
    	dst[6] = msg->data_length & 0xff;
    	dst[7] = msg->data_length >> 8;
    	size = msg->data_length + 9;
	for(i = 0; i < msg->data_length;i++){
        	dst[i + 8] = msg->buffer[i];
	}
    }else if (count == HALFWORD_SIZE){
	msg->data_length <<= 1;
    	dst[6] = msg->data_length & 0xff;
    	dst[7] = msg->data_length >> 8;
    	size = msg->data_length + 9;
	memcpy(&dst[8], msg->buffer, msg->data_length);
    }else {
    	printf("In %s, %d, Not implement data count=%d\n frame message!\n", __func__, __LINE__, count);
	return 0;
    }

    dst[size - 1] = '$';
    return size;
}

void make_bu61580_package(network_mesg_t *msg, unsigned char *src, int length){
    if (src[0] == '#' && src[1] == 1 && src[2] == '@' && src[length - 1] == '$'){
        msg->rt_addr = src[3]; //RT地址
        msg->sub_addr = src[4]; //1553b子地址
        msg->tx_rx_flag = src[5]; //接收类型
        msg->rx_bx_flag = 0;
	msg->data_length = (src[6] | (src[7] << 8))>>1; //接收数据长度
        memcpy(msg->buffer, &src[8], msg->data_length*2);
    }
}

static int recv_data_from_bu61580(conf_object_t *obj, network_mesg_t *msg, int count){
	bu61580_bridge_device* dev = obj->obj;
    	//unsigned char frame_msg[1024] = {0};
	//int size;
	if (dev->dev_cnt > 1){
		int i;
		for (i = 0; i < dev->dev_cnt; i++)
		{
			if (dev->bridge_device_list[i] != NULL){
				obj = dev->bridge_device_list[i];
				dev->bridge_device_iface_list[i]->receive_msg(obj, msg, 1);
			}	
		}
	}
    
	RW_WRLOCK(dev->lock);
	msg->count = count;
	WriteFIFO(dev->rec_fifo, msg, 1);
	RW_UNLOCK(dev->lock);
    	//size = unpack_bu61580_frame(frame_msg, msg, count);
    	//send(dev->sv_socket, frame_msg, size, 0);

	return 0;
}

void bu61580_msg_send_callback(conf_object_t *obj){
    	bu61580_bridge_device* dev = (bu61580_bridge_device*)(obj->obj);
    	network_mesg_t msg;
    	int size, count;
    	unsigned char frame_msg[1024] = {0};

	while(1){
    		if(CheckFIFOReadNum(dev->rec_fifo) == 0){
	    		goto out;
    		}

    		ReadFIFO(dev->rec_fifo, &msg, 1);
		count = msg.count;
  		size = unpack_bu61580_frame(frame_msg, &msg, count);

    		send(dev->sv_socket, frame_msg, size, 0);
out:
#ifdef __WIN32__
			Sleep(10);
#else
			usleep(10);
#endif
	}
}

int start_recv_bu61580_data(conf_object_t* obj){
	bu61580_bridge_device* dev = obj->obj;
    	network_mesg_t msg;

#ifdef __MINGW32__
    	WORD sockVersion = MAKEWORD(2,2);
	WSADATA data;
    char connect_msg[] = {'#', 0, '@', 1, '$'};

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
            int ret = recv(dev->sv_socket, recData, 256, 0);

            if (ret > 0){
                make_bu61580_package(&msg, recData, ret);
		conf_object_t *obj;
            	int i;
		for (i = 0; i < dev->dev_cnt; i++)
		{
			if (dev->bridge_device_list[i] != NULL){
				obj = dev->bridge_device_list[i];
				dev->bridge_device_iface_list[i]->receive_msg(obj, &msg, 1);
			}	
		}
            }
        }
#ifdef __WIN32__
		Sleep(5);
#else
		usleep(5);
#endif
    }
#endif
}

static attr_value_t get_attr_server_ip(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	bu61580_bridge_device* dev = conf_obj->obj;
	attr_value_t ip = SKY_make_attr_string(dev->server_ip);
	return ip;
}

static exception_t set_attr_server_ip(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	bu61580_bridge_device* dev = conf_obj->obj;
	char *server_ip;
	server_ip = SKY_attr_string(*value);
    	strncpy(dev->server_ip, server_ip, IP_LENGTH);

	return No_exp;
}

static attr_value_t get_attr_server_port(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	bu61580_bridge_device* dev = conf_obj->obj;
	attr_value_t server_port = SKY_make_attr_uinteger(dev->server_port);
	return server_port;
}

static exception_t set_attr_server_port(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	bu61580_bridge_device* dev = conf_obj->obj;
	dev->server_port = SKY_attr_uinteger(*value);
	return No_exp;
}

static load_file_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "server_ip", get_attr_server_ip, NULL, set_attr_server_ip, NULL, SKY_Attr_Optional, "string", "server ip");
	SKY_register_attribute(clss, "server_port", get_attr_server_port, NULL, set_attr_server_port, NULL, SKY_Attr_Optional, "uinteger", "server port");
}

static conf_object_t* new_bu61580_bridge(char *obj_name)
{
	bu61580_bridge_device* dev = skyeye_mm_zero(sizeof(bu61580_bridge_device));
	dev->obj = new_conf_object(obj_name, dev);
	dev->rec_fifo = CreateFIFO(MAX_FIFO_LEN);

	RWLOCK_INIT(dev->lock);
	dev->dev_cnt = 0;
	return dev->obj;
}

static exception_t free_bu61580_bridge(conf_object_t* obj)
{
	bu61580_bridge_device* dev = obj->obj;
	FreeFIFO(dev->rec_fifo);
	RWLOCK_DESTROY(dev->lock);
	skyeye_free(obj->objname);
	skyeye_free(obj->obj);
	skyeye_free(obj);
	return No_exp;
}

static exception_t config_bu61580_bridge(conf_object_t* obj)
{        
#ifdef __MINGW32__
 	CreateThread(NULL, 0, start_recv_bu61580_data, obj, 0, NULL);
	CreateThread(NULL, 0, bu61580_msg_send_callback, obj, 0, NULL);
#endif
}

static exception_t bu61580_bridge_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	bu61580_bridge_device* dev = (bu61580_bridge_device*)(obj->obj);

	dev->bridge_device_list[dev->dev_cnt] = obj2; 

	dev->bridge_device_iface_list[dev->dev_cnt] = (bu61580_bridge_intf*)SKY_get_iface(obj2, BU61580_BRIDGE_INTF);
	if (dev->bridge_device_iface_list[dev->dev_cnt] == NULL){
		if (obj2 == NULL)
			skyeye_log(Error_log, __FUNCTION__, "Get 'bu61580_bridge_intf' interface from NULL object fail.");
		else
			skyeye_log(Error_log, __FUNCTION__, "Get 'bu61580_bridge_intf' interface from %s fail.", obj2->objname);
		return Error;
	}
	dev->dev_cnt ++;
	return No_exp;
}
 
static exception_t bu61580_bridge_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	bu61580_bridge_device* dev = (bu61580_bridge_device*)(obj->obj);
	dev->dev_cnt --;
	*obj2 = dev->bridge_device_list[dev->dev_cnt];
	*port = NULL;
	return No_exp;
}

void init_bu61580_bridge(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "bu61580_bridge",
		.class_desc = "bu61580_bridge",
		.new_instance = new_bu61580_bridge,
		.free_instance = free_bu61580_bridge,
		.config_instance = config_bu61580_bridge,
		.module_type = SKYML_OFFCHIP,
	};
	conf_class_t* clss = SKY_register_linker_class(class_data.class_name, &class_data);

	static const bu61580_bridge_intf bu61580_bridge_interface = {
		.receive_msg = recv_data_from_bu61580,
	};
	SKY_register_iface(clss, BU61580_BRIDGE_INTF, &bu61580_bridge_interface);

	static const struct InterfaceDescription ifaces[] = 
	{
		(struct InterfaceDescription) {
			.name = BU61580_BRIDGE_INTF, 
			.iface = &bu61580_bridge_interface,
		},
	};
	static const struct ConnectDescription connects[] = 
	{
		/* ConnectDescription */
		(struct ConnectDescription) {
			.name = BU61580_BRIDGE_INTF,
			.set = bu61580_bridge_set,
			.get = bu61580_bridge_get,
		},

	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

    load_file_register_attribute(clss);
}
