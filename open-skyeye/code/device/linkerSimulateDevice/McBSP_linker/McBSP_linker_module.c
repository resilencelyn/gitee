#include <skyeye_module.h>
const char* skyeye_module = "McBSP_linker";
extern void init_McBSP_linker();
void module_init()
{
	init_McBSP_linker();
}
void module_fini()
{
}
