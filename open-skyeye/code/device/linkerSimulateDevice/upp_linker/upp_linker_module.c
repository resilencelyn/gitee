#include <skyeye_module.h>
const char* skyeye_module = "upp_linker";
extern void init_upp_linker();
void module_init()
{
	init_upp_linker();
}
void module_fini()
{
}
