#include <skyeye_module.h>

const char* skyeye_module = "mpc755";
extern void init_mpc755();

void module_init()
{
	init_mpc755();
}

void module_fini()
{
}
