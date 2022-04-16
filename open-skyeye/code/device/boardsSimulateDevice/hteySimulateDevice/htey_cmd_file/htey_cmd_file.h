#ifndef __HTEY_CMD_FILE_H__ 
#define __HTEY_CMD_FILE_H__ 
#include <skyeye_uart_ops.h>
struct registers 
{
	
};


struct cmd_file_device 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;

	conf_object_t *serial;

	skyeye_serial_device_t *serial_iface;
	
	uint32_t send_index;
	struct registers regs;
	FILE *tx_fp;
};
typedef struct cmd_file_device htey_cmd_file_device;
static char* regs_name[] = 
{
	NULL
};
#endif  
