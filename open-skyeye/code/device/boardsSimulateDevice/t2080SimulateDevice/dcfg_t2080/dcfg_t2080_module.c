#include <skyeye_module.h>
const char* skyeye_module = "dcfg_t2080";
extern void init_dcfg_t2080();
void module_init()
{
	init_dcfg_t2080();
}
void module_fini()
{
}
