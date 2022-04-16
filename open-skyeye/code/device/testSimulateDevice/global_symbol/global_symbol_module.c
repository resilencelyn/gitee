#include <skyeye_module.h>
const char* skyeye_module = "global_symbol";
extern void init_global_symbol();
void module_init()
{
	init_global_symbol();
}
void module_fini()
{
}
