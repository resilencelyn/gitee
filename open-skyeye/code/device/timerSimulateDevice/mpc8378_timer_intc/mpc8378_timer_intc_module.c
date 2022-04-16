#include <skyeye_module.h>

const char* skyeye_module = "mpc8378_timer_intc";

extern void init_mpc8378_timer_intc();

void module_init()
{
	init_mpc8378_timer_intc();
}

void module_fini()
{
}
