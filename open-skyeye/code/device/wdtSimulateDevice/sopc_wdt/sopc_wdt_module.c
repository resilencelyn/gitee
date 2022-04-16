#include <skyeye_module.h>

const char* skyeye_module = "sopc_wdt";

extern void init_sopc_wdt();

void module_init()
{
	init_sopc_wdt();
}

void module_fini()
{
}
