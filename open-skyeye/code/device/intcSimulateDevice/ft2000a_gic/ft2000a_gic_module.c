#include <skyeye_module.h>
const char* skyeye_module = "ft2000a_gic";
extern void init_ft2000a_gic();
void module_init()
{
	init_ft2000a_gic();
}
void module_fini()
{
}
