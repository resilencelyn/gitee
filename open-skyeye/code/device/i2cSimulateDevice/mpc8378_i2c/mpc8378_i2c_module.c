#include <skyeye_module.h>
const char* skyeye_module = "mpc8378_i2c";
extern void init_mpc8378_i2c();
void module_init()
{
	init_mpc8378_i2c();
}
void module_fini()
{
}
