#ifndef __BUGREPORT_TEST_H__
#define __BUGREPORT_TEST_H__

struct registers
{
};

struct bugreport_test_device_t
{
	conf_object_t* obj;

	core_signal_intf* signal_iface;
	conf_object_t* signal;

	struct registers regs;
};
typedef struct bugreport_test_device_t bugreport_test_device;

static char* regs_name[] =
{
	NULL
};
#endif
