#include <skyeye_module.h>
const char* skyeye_module = "mpc8378_pit";
extern void init_mpc8378_pit();
void module_init()
{
	init_mpc8378_pit();
}
void module_fini()
{
}
