#include <skyeye_module.h>
const char* skyeye_module = "rm48x_timer";
extern void init_rm48x_timer();
void module_init()
{
	init_rm48x_timer();
}
void module_fini()
{
}
