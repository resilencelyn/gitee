#include <skyeye_module.h>
const char* skyeye_module = "FMan_T2080";
extern void init_FMan_T2080();
void module_init()
{
	init_FMan_T2080();
}
void module_fini()
{
}
