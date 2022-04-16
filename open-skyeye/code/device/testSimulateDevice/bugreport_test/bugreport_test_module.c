#include <skyeye_module.h>

const char* skyeye_module = "bugreport_test";
extern void init_bugreport_test();

void module_init()
{
	init_bugreport_test();
}

void module_fini()
{
}
