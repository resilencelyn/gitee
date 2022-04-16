#include <skyeye_module.h>
const char* skyeye_module = "p2020_L2_cache";
extern void init_p2020_L2_cache();
void module_init()
{
	init_p2020_L2_cache();
}
void module_fini()
{
}
