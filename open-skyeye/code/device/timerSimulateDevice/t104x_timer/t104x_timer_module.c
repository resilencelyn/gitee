#include <skyeye_module.h>

const char* skyeye_module = "t104x_timer";

extern void init_timer_device();

void module_init()
{
	init_timer_device();
}

void module_fini()
{
}
