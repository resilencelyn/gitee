#include <skyeye_module.h>
const char* skyeye_module = "mpc8378_ddr";
extern void init_mpc8378_ddr();
void module_init()
{
	init_mpc8378_ddr();
}
void module_fini()
{
}
