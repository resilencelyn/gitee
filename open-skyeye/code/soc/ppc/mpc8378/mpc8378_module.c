#include <skyeye_module.h>
const char* skyeye_module = "mpc8378";
extern void init_mpc8378();
void module_init()
{
	init_mpc8378();
}
void module_fini()
{
}
