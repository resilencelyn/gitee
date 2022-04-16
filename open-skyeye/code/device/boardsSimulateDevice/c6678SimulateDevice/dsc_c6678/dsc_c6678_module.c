#include <skyeye_module.h>
const char* skyeye_module = "dsc_c6678";
extern void init_dsc_c6678();
void module_init()
{
	init_dsc_c6678();
}
void module_fini()
{
}
