#include <skyeye_module.h>

const char* skyeye_module = "sopc_uart";

extern void init_sopc_uart();

void module_init()
{
	init_sopc_uart();
}

void module_fini()
{
}
