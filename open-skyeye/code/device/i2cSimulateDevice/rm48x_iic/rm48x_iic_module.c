#include <skyeye_module.h>
const char* skyeye_module = "rm48x_iic";
extern void init_rm48x_iic();
void module_init()
{
	init_rm48x_iic();
}
void module_fini()
{
}
