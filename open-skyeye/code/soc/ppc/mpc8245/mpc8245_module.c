#include <skyeye_module.h>
const char* skyeye_module = "mpc8245";
extern void init_mpc8245();
void module_init()
{
	init_mpc8245();
}
void module_fini()
{
}
