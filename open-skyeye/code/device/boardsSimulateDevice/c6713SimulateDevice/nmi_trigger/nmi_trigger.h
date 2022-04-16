#ifndef __NMI_TRIGGER_H__
#define __NMI_TRIGGER_H__
struct registers
{
};


struct trigger_device 
{

	conf_object_t* obj;

	core_signal_intf* signal_iface;
	conf_object_t* signal;
	
	struct registers regs;

	time_handle_t* handle;
};

typedef struct trigger_device nmi_trigger_device;

static char* regs_name[] =
{
	NULL
};
#endif
