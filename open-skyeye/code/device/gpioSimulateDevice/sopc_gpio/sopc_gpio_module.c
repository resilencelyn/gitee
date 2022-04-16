#include <skyeye_module.h>

const char* skyeye_module = "sopc_gpio";

extern void init_sopc_gpio();

void module_init()
{
	init_sopc_gpio();
}

void module_fini()
{
}
