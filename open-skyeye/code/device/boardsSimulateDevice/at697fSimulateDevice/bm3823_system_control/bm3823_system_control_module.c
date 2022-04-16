#include <skyeye_module.h>

const char* skyeye_module = "bm3823_system_control";

extern void init_bm3823_system_control();

void module_init()
{
	init_bm3823_system_control();
}

void module_fini()
{
}
