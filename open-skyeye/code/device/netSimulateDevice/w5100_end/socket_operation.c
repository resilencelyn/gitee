#include "socket_operation.h"

extern void *order_memcpy(void *dest, const void *src, size_t n);

#define MAXLINE 1024

void create_net_recv(conf_object_t* conf_obj, uint32_t port_num, void * (*net_recv_handle)(void *arg))
{
	w5100_end_device *dev = (w5100_end_device *)conf_obj->obj;

	thread_args_t *thread_args = (thread_args_t *)skyeye_mm_zero(sizeof(thread_args_t));
	thread_args->device_obj   = conf_obj;
	thread_args->tmp_port_num = port_num;
	// 接受前端的数据
	pthread_create(&(dev->end_port[port_num].thread_id), NULL, net_recv_handle, (void *)thread_args);
}

void * net_recv_udp_handle(void *arg)
{
	thread_args_t *thread_args = (thread_args_t *)arg;
	w5100_end_device *dev = ((conf_object_t *)(thread_args->device_obj))->obj;

	char receline_data[MAXLINE + 1 + 8] = { 0 };
	char *receline;
	int16_t  recv_len    = 0;
	uint32_t read_offset = 0; // 缓存指针位置
	uint32_t offset_size = 0; // 缓存区大小
	uint32_t rx_rsr      = 0; // 缓存区数据字节数
	uint32_t write_len   = 0; // 待写入缓存区数据字节长度

	// 设备端口号
	uint32_t port_num    = thread_args->tmp_port_num;

	struct sockaddr_in stSockAddr;
	stSockAddr.sin_family 	   = AF_INET;
	stSockAddr.sin_port        = dev->regs.s[port_num].dport0 | (dev->regs.s[port_num].dport1 << 8);
	stSockAddr.sin_addr.s_addr = dev->regs.s[port_num].dipr0        | \
	                         (dev->regs.s[port_num].dipr1 <<  8) | \
	                         (dev->regs.s[port_num].dipr2 << 16) | \
	                         (dev->regs.s[port_num].dipr3 << 24);
	
	uint32_t nSinSize = sizeof(struct sockaddr_in);

	receline = receline_data+8;

	while (1) {
		recv_len = recvfrom(dev->end_port[port_num].socket_fd, receline, MAXLINE, 0, (struct sockaddr *)&stSockAddr, &nSinSize);
		if (recv_len > 0)
		{
			receline[recv_len] = 0;

			/*
			printf("\033[1;33m");
			printf("recv_len: %d port_num: %d\n", recv_len, port_num);
			int i;
			for (i = 0; i < recv_len;i++) {
				printf("0x%0x ", receline[i]);
				fflush(stdout);
			}
			printf("\n\n");
			printf("\033[0m");
			*/

			//uint8_t head[8] = {192, 168, 0, 136, 0, 88, 0, 8};
			//memcpy(dev->end_port[1].rx_buff  , head, 8);
			//memcpy(dev->end_port[1].rx_buff+8, receline, recv_len);

			/*
			 * 0-3: IP地址
			 * 4-5: 端口
			 * 6-7: 长度
			 * 8— : 数据
			 */
			memcpy(receline_data        , &stSockAddr.sin_addr.s_addr, 4);
			memcpy(receline_data+4      , &stSockAddr.sin_port       , 2);
			order_memcpy(receline_data+6, &recv_len                  , 2);
			memcpy(receline_data+8      , receline                   , recv_len);

			// 计算缓存区指针位置
			read_offset = dev->end_port[port_num].rx_buf_h;  // 环形数组头部
			offset_size = dev->end_port[port_num].rx_size;   // 通道最大空间
			write_len   = 8 + recv_len;

			// 写入数据大小超过通道缓存的大小的处理方式
			if ((read_offset + write_len) > offset_size) {
				uint32_t l_ui32_size;
				uint8_t  *tmp_receline_data = receline_data;

				l_ui32_size = offset_size - read_offset;
				memcpy(dev->end_port[port_num].rx_buff+read_offset, tmp_receline_data, l_ui32_size);

				// 移动指针
				tmp_receline_data += l_ui32_size;
				l_ui32_size        = write_len - l_ui32_size;

				// 剩余的部分, 拷贝到环形数据的零地址上
				memcpy(dev->end_port[port_num].rx_buff, tmp_receline_data, l_ui32_size);

				// 更新环形数组下标
				dev->end_port[port_num].rx_buf_h = l_ui32_size; // 环形数组头部
			} else {
				memcpy(dev->end_port[port_num].rx_buff+read_offset, receline_data, write_len);

				// 更新环形数组下标
				dev->end_port[port_num].rx_buf_h += write_len; // 环形数组头部
			}

			// 指示端口接收数据缓冲区中接收数据的字节数
			rx_rsr  = dev->regs.s[port_num].rx_rsr1 | (dev->regs.s[port_num].rx_rsr0 << 8);
			rx_rsr += write_len; // 计算数据长度

			dev->regs.s[port_num].rx_rsr0   = (rx_rsr>>8) & 0xFF;
			dev->regs.s[port_num].rx_rsr1   = (rx_rsr   ) & 0xFF;
		}
	}

	skyeye_free(arg);
}

void * net_recv_tcp_handle(void *arg)
{
	thread_args_t *thread_args = (thread_args_t *)arg;
	w5100_end_device *dev = ((conf_object_t *)(thread_args->device_obj))->obj;

	char receline[MAXLINE + 1];
	int16_t  recv_len    = 0;
	uint32_t read_offset = 0; // 缓存指针位置
	uint32_t offset_size = 0; // 缓存区大小

	uint32_t port_num    = thread_args->tmp_port_num;

	struct sockaddr_in stSockAddr;
	uint32_t nSinSize = sizeof(struct sockaddr_in);

	while (1) {
#ifdef WIN32
		while((recv_len = recv(dev->end_port[port_num].socket_fd, receline, MAXLINE, 0)) > 0)
#else
		while((recv_len = read(dev->end_port[port_num].socket_fd, receline, MAXLINE)) > 0)
#endif
		{
			receline[recv_len] = 0;
/*
			printf("\033[1;33m");
			printf("recv_len: %d port_num: %d\n", recv_len, port_num);
			int i;
			for (i = 0; i < recv_len;i++) {
				printf("0x%0x ", receline[i]);
				fflush(stdout);
			}
			printf("\n\n");
			printf("\033[0m");
*/
		}
	}

	skyeye_free(arg);
}

int net_socket(conf_object_t* conf_obj, uint32_t port_num, NetSocketType type)
{
	w5100_end_device *dev = (w5100_end_device *)conf_obj->obj;

	dev->end_port[port_num].socket_fd = -1;

	// 建立socket

	switch (type) {
	case ENET_SOCKET_TYPE_STREAM:    // TCP
	{
		dev->end_port[port_num].socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		break;
	}
	case ENET_SOCKET_TYPE_DATAGRAM:  // UDP
	{
		dev->end_port[port_num].socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
		// 接受前端的数据
		create_net_recv(conf_obj, port_num, net_recv_udp_handle);

		struct sockaddr_in socket_addr;
		// 固定UDP端口号
		memset(&socket_addr, 0, sizeof(socket_addr));
		socket_addr.sin_family      = AF_INET;
		socket_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 自动分配IP
		if (port_num == 2)
		{
			socket_addr.sin_port    = htons(dev->display_port);
		}
		else
		{
			socket_addr.sin_port    = 0;                 // 自动分配端口号
		}

		if (bind(dev->end_port[port_num].socket_fd, (struct sockaddr *)&socket_addr, sizeof(struct sockaddr_in)) == -1) {
			skyeye_log(Error_log, __FUNCTION__, "bind error, Unable to fix UDP Port, port: %d\n", socket_addr.sin_port);
		}
		break;
	}
	default:
		skyeye_log(Error_log, __FUNCTION__, "type: %d, not Support type\n", type);
		break;
	}

	if (dev->end_port[port_num].socket_fd == -1) {
		skyeye_log(Error_log, __FUNCTION__, "socket() fail\n");
		return 0;
	}


	return 1;
}

int net_connect(conf_object_t* conf_obj, uint32_t port_num, int32_t socket_fd, uint8_t *ip, uint32_t socket_port)
{
	w5100_end_device *dev = (w5100_end_device *)conf_obj->obj;

	int ret;
	struct sockaddr_in socket_addr;

	// 设置协议
	memset(&socket_addr, 0, sizeof(socket_addr));
	socket_addr.sin_family = AF_INET;
	// 设置端口
	socket_addr.sin_port   = htons(socket_port);
	// 设置IP
	//socket_addr.sin_addr.s_addr = inet_addr("192.168.0.123");
	memcpy(socket_addr.sin_addr.s_addr, ip, 4);

	// 连接
	ret = connect(socket_fd, (struct sockaddr *)&socket_addr, sizeof(socket_addr));
	if (ret != -1) {

		// 接受前端的数据
		create_net_recv(conf_obj, port_num, net_recv_tcp_handle);
	} else if (ret == -1) {
		skyeye_log(Error_log, __FUNCTION__, "connect() fail\n");
	}
}

ssize_t net_sendto(conf_object_t* conf_obj, uint32_t port_num, const void *buf, uint32_t send_len)
{
	w5100_end_device *dev = (w5100_end_device *)conf_obj->obj;

	struct sockaddr_in sock_in;
	ssize_t	 sock_len = sizeof(struct sockaddr_in);

	sock_in.sin_family      = AF_INET;
	//sock_in.sin_port        = htons(60000);
	//sock_in.sin_addr.s_addr = inet_addr("192.168.0.103");

	sock_in.sin_port        = dev->regs.s[port_num].dport0 | (dev->regs.s[port_num].dport1 << 8);
	sock_in.sin_addr.s_addr = dev->regs.s[port_num].dipr0        | \
	                         (dev->regs.s[port_num].dipr1 <<  8) | \
	                         (dev->regs.s[port_num].dipr2 << 16) | \
	                         (dev->regs.s[port_num].dipr3 << 24);

	return sendto(dev->end_port[port_num].socket_fd, buf, send_len, 0, (struct sockaddr* )&sock_in, sock_len);
}

int net_initialize (void)
{
#ifdef WIN32
	WORD versionRequested = MAKEWORD (1, 1);
	WSADATA wsaData;

	if (WSAStartup (versionRequested, & wsaData))
	{
		skyeye_log(Error_log, __FUNCTION__, "WSAStartup() fail\n");
		return -1;
	}

	if (LOBYTE (wsaData.wVersion) != 1||
		HIBYTE (wsaData.wVersion) != 1)
	{
		skyeye_log(Error_log, __FUNCTION__, "wVersion not is 1.1\n");
		WSACleanup ();
		return -1;
	}
#endif
	return 0;
}

void net_deinitialize (void)
{
#ifdef WIN32
	WSACleanup ();
#endif
}
