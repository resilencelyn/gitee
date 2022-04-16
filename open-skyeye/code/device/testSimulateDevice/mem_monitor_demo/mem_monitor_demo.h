#ifndef __MEM_MONITOR_DEMO_H__
#define __MEM_MONITOR_DEMO_H__

struct registers
{
};

struct mem_monitor_demo_device_t
{
	conf_object_t* obj;

	core_signal_intf* signal_iface;
	conf_object_t* signal;

	struct registers regs;
};

typedef struct mem_monitor_demo_device_t mem_monitor_demo_device;
static char* regs_name[] =
{
	NULL
};
#endif
