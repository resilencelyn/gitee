#include <skyeye_module.h>
const char* skyeye_module = "BMan_T2080";
extern void init_BMan_T2080();
void module_init()
{
	init_BMan_T2080();
}
void module_fini()
{
}
