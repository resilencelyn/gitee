#ifndef __API_TEST_DEV_H__
#define __API_TEST_DEV_H__

struct registers
{
};

struct api_test_dev_device_t
{
	conf_object_t* obj;

	struct registers regs;
};
typedef struct api_test_dev_device_t api_test_dev_device;

static char* regs_name[] =
{
	NULL
};
#endif
