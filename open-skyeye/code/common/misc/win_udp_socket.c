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

/*�������ƣ�skyeye_udp_send
 *����˵����UDP�������ݽӿ�
 *���������1) rt_socket��socket������ 2)rt_serAddr: �������ݽṹ�� 3) msg��Ҫ���͵����ݵ�ַ 4) msg_len��Ҫ���͵����ݳ���
 *�����������
 *����ֵ����
 * */
void skyeye_udp_send(int rt_socket, struct sockaddr_in rt_serAddr, void *msg, int msg_len)
{
    int len_socket = sizeof(struct sockaddr_in);
    sendto(rt_socket, msg, msg_len, 0, (struct sockaddr*)&(rt_serAddr), len_socket);

    return;
}

/*�������ƣ�skyeye_udp_recv
 *����˵����UDP�������ݽӿ�
 *���������1) rt_socket��socket������
 *���������1) msg���������ݵ�ַ 2)msg_len: ���յ������ݳ���
 *����ֵ����
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

/*�������ƣ�skyeye_udp_init
 *����˵��������UDP�ͻ���
 *���������1) target_ip��Ŀ��IP��ַ 2)target_port: Ŀ��˿ں�
 *���������1) rt_socket��socket������ 2)rt_serAddr: �������ݽṹ��
 *����ֵ��No_exp��ʾ�ɹ���Invarg_exp��ʾʧ��
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
