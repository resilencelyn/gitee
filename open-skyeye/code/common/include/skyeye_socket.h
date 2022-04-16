#ifndef __SKYEYE_SOCKET_H__
#define __SKYEYE_SOCKET_H__
#include "skyeye_types.h"

#ifdef __WIN32__
#include <windows.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

#define MAX_RECV_LEN  8192 //4096

/*函数名称：skyeye_udp_init
 *函数说明：创建UDP客户端
 *输入参数：1) target_ip：目标IP地址 2)target_port: 目标端口号
 *输出参数：1) rt_socket：socket描述符 2)rt_serAddr: 网络数据结构体
 *返回值：No_exp表示成功，Invarg_exp表示失败
 * */
exception_t skyeye_udp_init(char *target_ip, int target_port, int *rt_socket, struct sockaddr_in *rt_serAddr);

/*函数名称：skyeye_udp_send
 *函数说明：UDP发送数据接口
 *输入参数：1) rt_socket：socket描述符 2)rt_serAddr: 网络数据结构体 3) msg：要发送的数据地址 4) msg_len：要发送的数据长度
 *输出参数：无
 *返回值：无
 * */
void skyeye_udp_send(int rt_socket, struct sockaddr_in rt_serAddr, void *msg, int msg_len);

/*函数名称：skyeye_udp_recv
 *函数说明：UDP接收数据接口
 *输入参数：1) rt_socket：socket描述符
 *输出参数：1) msg：接收数据地址 2)msg_len: 接收到的数据长度
 *返回值：无
 * */
void skyeye_udp_recv(int rt_socket, void *msg, int *msg_len);

#endif
