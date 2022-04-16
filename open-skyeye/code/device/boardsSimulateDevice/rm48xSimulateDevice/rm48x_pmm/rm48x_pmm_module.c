#include <skyeye_module.h>
const char* skyeye_module = "rm48x_pmm";
extern void init_rm48x_pmm();
void module_init()
{
	init_rm48x_pmm();
}
void module_fini()
{
}
