#ifndef __SKYEYE_UART_FILE_H__
#define __SKYEYE_UART_FILE_H__


#define MAX_FILE_NAME_LEN 128
typedef struct skyeye_uart_file{
	conf_object_t* obj;
	FILE *fp;
	char filename[MAX_FILE_NAME_LEN];
}uart_file_device;

#endif
