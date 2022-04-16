#include <skyeye_module.h>
const char* skyeye_module = "can_bridge";
extern void init_can_bridge();
void module_init()
{
	init_can_bridge();
}
void module_fini()
{
}
