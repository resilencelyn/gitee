#include <skyeye_module.h>

const char* skyeye_module = "ft2000a_wdt";

extern void init_ft2000a_wdt();

void module_init()
{
	init_ft2000a_wdt();
}

void module_fini()
{
}
