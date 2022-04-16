#include <skyeye_module.h>
const char* skyeye_module = "c6678_I2C";
extern void init_c6678_I2C();
void module_init()
{
	init_c6678_I2C();
}
void module_fini()
{
}
