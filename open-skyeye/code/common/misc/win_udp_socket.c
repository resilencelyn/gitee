/*************************************************************************
  > File Name    : udp_socket.c
  > Author       : jiachao
  > Mail         : jiachao@163.com
  > Created Time : 2021/3/23 14:57:41
 ************************************************************************/
#include <skyeye_types.h>
#include <skyeye_log.h>
#include <windows.h>
#include <skyeye_socket.h>

/*函数名称：skyeye_udp_send
 *函数说明：UDP发送数据接口
 *输入参数：1) rt_socket：socket描述符 2)rt_serAddr: 网络数据结构体 3) msg：要发送的数据地址 4) msg_len：要发送的数据长度
 *输出参数：无
 *返回值：无
 * */
void skyeye_udp_send(int rt_socket, struct sockaddr_in rt_serAddr, void *msg, int msg_len)
{
    int len_socket = sizeof(struct sockaddr_in);
    sendto(rt_socket, msg, msg_len, 0, (struct sockaddr*)&(rt_serAddr), len_socket);

    return;
}

/*函数名称：skyeye_udp_recv
 *函数说明：UDP接收数据接口
 *输入参数：1) rt_socket：socket描述符
 *输出参数：1) msg：接收数据地址 2)msg_len: 接收到的数据长度
 *返回值：无
 * */
void skyeye_udp_recv(int rt_socket, void *msg, int *msg_len)
{
    struct sockaddr_in stSockAddr;
    int len_socket = sizeof(struct sockaddr_in);

    int recv_len = recvfrom(rt_socket, msg, MAX_RECV_LEN, 0, (struct sockaddr *)&stSockAddr, &len_socket);
    if (recv_len > 0)
    {
        *msg_len = recv_len;
    } else
    {
        *msg_len = 0;
    }

    return;
}

/*函数名称：skyeye_udp_init
 *函数说明：创建UDP客户端
 *输入参数：1) target_ip：目标IP地址 2)target_port: 目标端口号
 *输出参数：1) rt_socket：socket描述符 2)rt_serAddr: 网络数据结构体
 *返回值：No_exp表示成功，Invarg_exp表示失败
 * */
exception_t skyeye_udp_init(char *target_ip, int target_port, int *rt_socket, struct sockaddr_in *rt_serAddr)
{
    WORD versionRequested = MAKEWORD (1, 1);
    WSADATA wsaData;
    int tmp_socket;
    struct sockaddr_in tmp_serAddr;

    if (WSAStartup (versionRequested, & wsaData))
    {
        skyeye_log(Error_log, __FUNCTION__, "WSAStartup() Failed!\n");
        return Invarg_exp;
    }

    tmp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(tmp_socket == INVALID_SOCKET)
    {
        skyeye_log(Error_log, __FUNCTION__, "Create socket Failed!\n");
        return Invarg_exp;
    }

    tmp_serAddr.sin_family = AF_INET;
    tmp_serAddr.sin_port = htons(target_port);
    tmp_serAddr.sin_addr.S_un.S_addr = inet_addr(target_ip);

    *rt_socket = tmp_socket;
    *rt_serAddr = tmp_serAddr;

    return No_exp;
}
