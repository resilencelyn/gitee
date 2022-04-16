#include <skyeye_module.h>

const char* skyeye_module = "sopc_sysctrl";

extern void init_sopc_sysctrl();

void module_init()
{
	init_sopc_sysctrl();
}

void module_fini()
{
}
