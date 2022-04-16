#include <skyeye_module.h>
const char* skyeye_module = "rm48x_gio";
extern void init_rm48x_gio();

void module_init()
{
	init_rm48x_gio();
}

void module_fini()
{
}
