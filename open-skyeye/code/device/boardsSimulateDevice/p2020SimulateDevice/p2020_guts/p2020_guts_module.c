#include <skyeye_module.h>
const char* skyeye_module = "guts_p2020";
extern void init_guts_p2020();
void module_init()
{
	init_guts_p2020();
}
void module_fini()
{
}
