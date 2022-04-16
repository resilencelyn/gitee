#include <skyeye_module.h>

const char* skyeye_module = "p2020_ecm";

extern void init_p2020_ecm();

void module_init()
{
	init_p2020_ecm();
}

void module_fini()
{
}
