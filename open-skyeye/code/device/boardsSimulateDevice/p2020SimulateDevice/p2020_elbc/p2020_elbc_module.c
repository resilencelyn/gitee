#include <skyeye_module.h>
const char* skyeye_module = "p2020_elbc";
extern void init_p2020_elbc();
void module_init()
{
	init_p2020_elbc();
}
void module_fini()
{
}
