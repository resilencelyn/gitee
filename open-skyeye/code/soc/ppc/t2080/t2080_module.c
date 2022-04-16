#include <skyeye_module.h>
const char* skyeye_module = "t2080";
extern void init_t2080();
void module_init()
{
	init_t2080();
}
void module_fini()
{
}
