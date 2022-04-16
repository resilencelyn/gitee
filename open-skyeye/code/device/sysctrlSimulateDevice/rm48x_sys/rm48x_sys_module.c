#include <skyeye_module.h>

const char* skyeye_module = "rm48x_sys";
extern void init_rm48x_sys();

void module_init()
{
	init_rm48x_sys();
}

void module_fini()
{
}
