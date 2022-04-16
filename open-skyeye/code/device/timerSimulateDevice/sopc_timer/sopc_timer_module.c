#include <skyeye_module.h>

const char* skyeye_module = "sopc_timer";

extern void init_sopc_timer();

void module_init()
{
	init_sopc_timer();
}

void module_fini()
{
}
