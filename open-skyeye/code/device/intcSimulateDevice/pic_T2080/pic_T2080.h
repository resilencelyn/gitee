#ifndef __PIC_T2080_H__ 
#define __PIC_T2080_H__ 

#define TimeBaseRateUS 75
struct pic_T2080_device_t 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;
	
	uint32_t pic_global[0xFFFF];
	uint32_t source_registers[0xFFFF];
	uint32_t global_access_registers[0xFFFF];
	FIFO *int_fifo;
	time_handle_t* handle[4];
	int isInt;
	uint32_t timer_cnt[4];
	uint32_t time_int_num[4];
	uint32_t time_int_enable[4];
	int time_flag[4];
	int first_flag[4];
};
typedef struct pic_T2080_device_t pic_T2080_device;
static char* regs_name[] = 
{
	NULL
};
#endif  
