#ifndef __SE_PORT_INTF_H__
#define __SE_PORT_INTF_H__
#include "skyeye_types.h"

typedef struct {
#define READ_DATA  1
#define WRITE_DATA 2
#define READ_OK_F  3
	uint32_t mode;

	uint32_t addr;
	uint32_t data;
	uint32_t length;
} port_data_protocol;

typedef struct se_port_intf_interface {
	int32_t (*AddPort)(conf_object_t* conf_obj, conf_object_t* usr_data, char *port_name, Port_Call_Back_t Recv_Call_Back);
	int32_t (*WriteData)(conf_object_t* conf_obj, se_int32_t ulPortIdx, const void* lpData, se_uint32_t ulDataSize);
} se_port_intf;

#define SE_PORT_INTF "se_port_intf"

#endif
