#include <skyeye_module.h>
const char* skyeye_module = "rm48x_pbist";
extern void init_rm48x_pbist();
void module_init()
{
	init_rm48x_pbist();
}
void module_fini()
{
}
