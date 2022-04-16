#ifndef __MPC8378_TIMER_INTC_H__
#define __MPC8378_TIMER_INTC_H__

struct mpc8378_timer_intc_device_t
{
	conf_object_t* obj;
	struct {
		int                     num;
		conf_object_t          *obj;
		general_signal_intf    *intf;
	}signal;
	int time;
};

typedef struct mpc8378_timer_intc_device_t mpc8378_timer_intc_device;

#endif
