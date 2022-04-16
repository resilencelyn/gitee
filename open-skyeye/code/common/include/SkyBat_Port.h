#ifndef _SKYBAT_PORT_H

#ifdef __MINGW32__
#include <windows.h>
#else
#include <sys/stat.h>
#define MAX_PATH 260
#endif

typedef enum {
	COMMAND,
	SCRIPT_START,
	SCRIPT_END,
} virDeviceDataType;

typedef struct virDeviceFomat{
	virDeviceDataType DataType; //数据类型
	char strCommand[MAX_PATH];
	char strParam[MAX_PATH];
} virDevMessage, *pVirDevMessage;

typedef struct SkyBat_Port_interface {
	exception_t (*script_data)(conf_object_t *conf_obj, void *data);
} SkyBat_Port_intf;

#define SKYEYE_BAT_PORT "SkyEye_Bat_Port"

#define _SKYBAT_PORT_H
#endif
