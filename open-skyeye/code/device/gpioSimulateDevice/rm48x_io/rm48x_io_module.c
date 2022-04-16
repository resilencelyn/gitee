#include <skyeye_module.h>
const char* skyeye_module = "rm48x_io";
extern void init_rm48x_io();
void module_init()
{
	init_rm48x_io();
}
void module_fini()
{
}
