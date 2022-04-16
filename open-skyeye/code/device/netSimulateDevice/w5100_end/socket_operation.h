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

#ifdef WIN32:
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#include <skyeye_thread.h>


typedef enum _NetSocketType
{
	ENET_SOCKET_TYPE_STREAM   = 1,
	ENET_SOCKET_TYPE_DATAGRAM = 2
} NetSocketType;

#include "w5100_end.h"

int net_initialize (void);
void net_deinitialize (void);
int net_socket(conf_object_t* conf_obj, uint32_t end_port, NetSocketType type);
ssize_t net_sendto(conf_object_t* conf_obj, uint32_t port_num, const void *buf, uint32_t send_len);
void net_recv_handle(void *arg);
