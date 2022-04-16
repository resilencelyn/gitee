#include <skyeye_module.h>

const char* skyeye_module = "sopc_ictl";

extern void init_sopc_ictl();

void module_init()
{
	init_sopc_ictl();
}

void module_fini()
{
}
