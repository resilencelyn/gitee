
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include "lic.h"
#include "skyeye_internal.h"
#ifdef __WIN32__
#include <WINSOCK2.H>
#include <pthread.h>
SOCKET sclient = -1;
#endif

#define true 1;
#define false 0;

char *old_ip = NULL;
int old_port = 0;

int check_data(char *des)
{
    if (des[0] == '#' && des[4] == '@' && des[strlen(des) - 1] == '$')
        return true;
    return false;
}

#ifdef __WIN32__
int *thread(void *arg)
{
    pthread_t newthid;

    newthid = pthread_self();
    char temp[10] = { 0 };
    int nNetTimeout = 2000;
    char recData[2048] = { 0 };
    setsockopt(arg, SOL_SOCKET, (int) recData, (char *) &nNetTimeout, sizeof (int));
    while (1)
    {
        memset(recData, 0, sizeof (recData));
        int ret = recv(arg, recData, 2048, 0);

        if (ret <= 0)
        {
            memset(allow_cpu_list, 0, strlen(allow_cpu_list));
            printf("The server lost connection!\n");
            skyeye_log(Error_log, __FUNCTION__, "The server lost connection\n");
            verify_status = 0;
            return false;
        } else
        {
            if (check_data(recData))
            {
                strncpy(temp, recData + 1, 3);
                if (strcmp(temp, "030") == 0)
                {
                    memset(allow_cpu_list, 0, strlen(allow_cpu_list));
                    printf("Server down\n");
                    skyeye_log(Error_log, __FUNCTION__, "Server down\n");
                    verify_status = 0;
                    return false;
                } else if (strcmp(temp, "040") == 0)
                {
                    send(arg, recData, strlen(recData), 0);
                    continue;
                }
            }
        }
    }
}

void connect_failed(char *info, SOCKET client)
{
    printf(info);
    skyeye_log(Error_log, __FUNCTION__, info);
    verify_status = 0;
    if (client != NULL)
    {
        closesocket(client);
    }
}
#endif

void close_network_lic()
{
#ifdef __WIN32__
    if (sclient != -1)
    {
        char *sendData = "quit";

        send(sclient, sendData, strlen(sendData), 0);
        free(old_ip);
    }
#endif
}

int network_lic_validation(char *ip, int port)
{
#ifndef __WIN32__
    return 3;
#else
    int i;

    if (verify_status == 1 && strcmp(ip, old_ip) == 0 && old_port == port)
    {
        return 3;
    }
    old_ip = (char *) calloc(strlen(ip), sizeof (char));
    strcpy(old_ip, ip);
    old_port = port;
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA data;

    if (WSAStartup(sockVersion, &data) != 0)
    {
        connect_failed("Execute the WSAStartup fail\n", NULL);
        return 10;
    }

    sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sclient == INVALID_SOCKET)
    {
        connect_failed("Create socket error!\n", NULL);
        return 11;
    }
    unsigned long on = 1;

    if (ioctlsocket(sclient, FIONBIO, &on) < 0)
    {
        connect_failed("ioctlsocket failed\n", NULL);
        return 12;
    }
    struct sockaddr_in serAddr;

    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(port);
    serAddr.sin_addr.S_un.S_addr = inet_addr(ip);
    int ret = connect(sclient, (SOCKADDR *) & serAddr, sizeof (SOCKADDR));

    if (ret < 0 && WSAGetLastError() != WSAEWOULDBLOCK)
    {
        WSACleanup();
        connect_failed("Connection failed!\n", sclient);
        return 12;
    }
    fd_set writeset;

    FD_ZERO(&writeset);
    FD_SET(sclient, &writeset);
    struct timeval tv;

    tv.tv_sec = 3;
    tv.tv_usec = 0;
    ret = select(sclient + 1, NULL, &writeset, NULL, &tv);
    if (ret == 0)
    {
        WSACleanup();
        connect_failed("Connection timeout!\n", sclient);
        return 12;
    } else if (ret < 0)
    {
        WSACleanup();
        connect_failed("Connection failed!\n", sclient);
        return 12;
    } else
    {
        //printf("connect success2\n");
    }
    unsigned long off = 0;

    if (ioctlsocket(sclient, FIONBIO, &off) < 0)
    {
        connect_failed("ioctlsocket failed!\n", sclient);
        return 12;
    }
    char *sendData = "can";

    send(sclient, sendData, strlen(sendData), 0);

    char recData[2048] = { 0 };
    char temp[10] = { 0 };
    ret = recv(sclient, recData, 2048, 0);
    if (ret > 0)
    {
        if (check_data(recData))
        {
            strncpy(temp, recData + 1, 3);
            if (strcmp(temp, "010") == 0)
            {
                for (i = 0; i < (ret - 6) / 2; i++)
                {
                    memset(temp, 0, sizeof (temp));
                    strncpy(temp, recData + 5 + i * 2, 2);
                    sscanf(temp, "%x", &allow_cpu_list[i]);
                }
            } else if (strcmp(temp, "020") == 0)
            {
                WSACleanup();
                connect_failed("Link exceeds maximum connectable number!\n", sclient);
                return 13;
            }
        }
    } else
    {
        WSACleanup();
        connect_failed("Return information is empty!\n", sclient);
        return 14;
    }
    ret = 0;
    pthread_t thid;

    ret = pthread_create(&thid, NULL, (void *) thread, sclient);
    if (ret != 0)
    {
        connect_failed("The heartbeat thread creation failed!", NULL);
        return 15;
    }
    verify_status = 1;
    return 3;
#endif

}
