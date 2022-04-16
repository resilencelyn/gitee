#ifndef __CAN_EXC_BRIDGE_H__
#define __CAN_EXC_BRIDGE_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32:
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#include <skyeye_thread.h>

#define MAXLINE 1024
#define IP_LENGTH       16 

int create_client(char *server_ip, int server_port);
void close_client(int sockfd);

typedef enum {
	STATUS_NONE,
	STATUS_1, //after find # char
	STATUS_2, //after find msg type char
	STATUS_3, //after find @ type char
	STATUS_4, //after find $ char
} status_t;

typedef enum {
	NONE_ERR,
	MALLOC_ERR,
	INTER_OK_ERR,
} ret_t;

typedef enum {
	INTER_OK,
	INTER_NOT_OK,
} inter_status_t;

typedef int (*inter_context_fetch)(void *conf_obj, int msg_type, void *buf, int size, void *user_data);

typedef struct {
	inter_status_t inter_status;
	int msg_type;
	status_t context_status;
	char *context;
	int context_index;
	int context_size;
	void *user_data;
	inter_context_fetch fetch_func;
} inter_t;

typedef struct {
	unsigned int  id;
	unsigned char channel_id;
	unsigned char ide;
	unsigned char dlc;
	unsigned char data[8];
} can_t;

struct exc_bridge_device 
{
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	conf_object_t* signal;

	conf_object_t* can_linker;
	can_linker_intf *can_linker_iface;

	int   client_fd;
	char  server_ip[IP_LENGTH];
	int   server_port;

	pthread_t thread_id;

	inter_t inter_obj;
};

typedef struct exc_bridge_device can_exc_bridge_device;

#endif
