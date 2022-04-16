#include <skyeye_module.h>
const char* skyeye_module = "sfifoc_mpc5121";
extern void init_sfifoc_mpc5121();
void module_init()
{
	init_sfifoc_mpc5121();
}
void module_fini()
{
}
