#include <skyeye_module.h>
const char* skyeye_module = "mpc5121";
extern void init_mpc5121();
void module_init()
{
	init_mpc5121();
}
void module_fini()
{
}
