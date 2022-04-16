#ifndef __QMAN_T2080_H__ 
#define __QMAN_T2080_H__ 

struct QMan_T2080_device_t 
{
	conf_object_t* obj;

	// core_signal_intf* signal_iface;
	// conf_object_t* signal;

	memory_space_intf* image_space;
	conf_object_t* image;
	
	skyeye_net_ops_intf *net_iface;
	conf_object_t *net;
	
	struct {
		int tx_num;
		int rx_num;
		conf_object_t          *obj;
		general_signal_intf    *intf;
	}signal;

	uint32_t regs[0x2000];
	uint8_t sp_regs[0x2000000];
	struct qportal_enqueue *QPORTAL_ENQUEUE;
	struct qportal_dequeue *QPORTAL_DEQUEUE;

	// deque队列
	int queue16_Pin;
	uint8_t qportalDqVbit;

	uint32_t flag;
	uint32_t sub_index;
	uint32_t FqId;

};
typedef struct QMan_T2080_device_t QMan_T2080_device;
static char* regs_name[] = 
{
	NULL
};

#endif  


