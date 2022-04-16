#include <skyeye_module.h>

const char* skyeye_module = "t4240";
extern void init_t4240();

void module_init()
{
	init_t4240();
}

void module_fini()
{
}
