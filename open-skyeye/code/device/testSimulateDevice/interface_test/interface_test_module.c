#include <skyeye_module.h>
const char* skyeye_module = "interface_test";
extern void init_interface_test();
void module_init()
{
	init_interface_test();
}
void module_fini()
{
}
