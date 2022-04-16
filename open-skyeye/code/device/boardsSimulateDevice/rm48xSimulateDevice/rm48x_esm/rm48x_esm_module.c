#include <skyeye_module.h>
const char* skyeye_module = "rm48x_esm";
extern void init_rm48x_esm();
void module_init()
{
	init_rm48x_esm();
}
void module_fini()
{
}
