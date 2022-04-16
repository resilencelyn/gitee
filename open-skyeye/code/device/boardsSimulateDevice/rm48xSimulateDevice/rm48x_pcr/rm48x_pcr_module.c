#include <skyeye_module.h>
const char* skyeye_module = "rm48x_pcr";
extern void init_rm48x_pcr();
void module_init()
{
	init_rm48x_pcr();
}
void module_fini()
{
}
