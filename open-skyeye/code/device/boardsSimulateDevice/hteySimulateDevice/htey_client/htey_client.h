#ifndef __HTEY_CLIENT_H__ 
#define __HTEY_CLIENT_H__ 

#ifdef WIN32
#include <Winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

struct registers 
{
	
};


struct client_device 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;
	
	struct registers regs;

	char *server_ip;
	int server_port;
	int sockfd;
};
typedef struct client_device htey_client_device;
static char* regs_name[] = 
{
};
#endif  
