#ifndef __HTEY_HDLC_CMD_FILE_H__ 
#define __HTEY_HDLC_CMD_FILE_H__ 
struct registers 
{
	
};

typedef struct file_data_s {
	unsigned char *buf;
	int index;
	int file_len;
	int frme_len;
	int frme_cnt;
	char *filename;
	struct file_data_s *next;
}file_data_t;

struct hdlc_cmd_file_device 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;
	
	struct registers regs;

	conf_object_t* serial;
	skyeye_serial_device_t* serial_iface;

	file_data_t *file_data;
	
	FILE *tx_fp;
};
typedef struct hdlc_cmd_file_device htey_hdlc_cmd_file_device;
static char* regs_name[] = 
{
	NULL
};
#endif  
