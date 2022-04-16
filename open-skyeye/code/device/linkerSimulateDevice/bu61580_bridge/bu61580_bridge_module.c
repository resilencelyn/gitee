#include <skyeye_module.h>
const char* skyeye_module = "bu61580_bridge";
extern void init_bu61580_bridge();
void module_init()
{
	init_bu61580_bridge();
}
void module_fini()
{
}
