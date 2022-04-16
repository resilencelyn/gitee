#include <skyeye_module.h>
const char* skyeye_module = "rs422_bridge";
extern void init_rs422_bridge();
void module_init()
{
	init_rs422_bridge();
}
void module_fini()
{
}
