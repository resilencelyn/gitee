#include <skyeye_module.h>
const char* skyeye_module = "QMan_T2080";
extern void init_QMan_T2080();
void module_init()
{
	init_QMan_T2080();
}
void module_fini()
{
}
