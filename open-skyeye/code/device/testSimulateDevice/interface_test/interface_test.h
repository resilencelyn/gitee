#ifndef __INTERFACE_TEST_H__
#define __INTERFACE_TEST_H__

// All equipment register names, the list order and struct registers registers inside order is kept consistent
static char* regs_name[] =
{
	NULL
};

// Register offset

// device register structure
struct registers
{
	
};


// Device object structure
struct interface_test_device_t
{
	
	conf_object_t* obj;
	
	// Interface objects and methods;
	
	uint32_t pc;
	double SimTime;
	char count;	
	char PrjPath[1024];
	int IsStop;
	time_handle_t handle;
	struct registers regs;
	
};
typedef struct interface_test_device_t interface_test_device;

#endif
