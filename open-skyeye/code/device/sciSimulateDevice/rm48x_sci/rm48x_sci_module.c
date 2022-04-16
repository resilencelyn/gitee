#include <skyeye_module.h>
const char* skyeye_module = "rm48x_sci";
extern void init_rm48x_sci();

void module_init()
{
	init_rm48x_sci();
}

void module_fini()
{
}
