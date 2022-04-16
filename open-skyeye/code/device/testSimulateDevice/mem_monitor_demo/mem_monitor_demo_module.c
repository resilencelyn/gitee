#include <skyeye_module.h>
const char* skyeye_module = "mem_monitor_demo";

extern void init_mem_monitor_demo();

void module_init()
{
	init_mem_monitor_demo();
}

void module_fini()
{
}
