#ifndef __TIMER_DEVICE_H__
#define __TIMER_DEVICE_H__

struct registers
{
	uint32_t timer_ctrl;
	uint32_t timer_intc;
};

struct timer_device_t
{
	conf_object_t* obj;
	struct {
		int                     num[2];
		conf_object_t          *obj;
		general_signal_intf    *intf;
	}signal;
	struct registers regs;
	time_handle_t* handle[2];
	int time[2];
};

typedef struct timer_device_t timer_device;

static char* regs_name[] =
{
	"timer_ctrl",
	"timer_intc",
	NULL
};

#define TIMER_CTRL 0
#define TIMER_INTC 4

#define TIMER_ENABLE 1
#define INT_ENABLE 1

#endif
