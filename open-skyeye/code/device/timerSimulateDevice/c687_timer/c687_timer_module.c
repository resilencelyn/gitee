#include <skyeye_module.h>
const char* skyeye_module = "c687_timer";
extern void init_c687_timer();
void module_init()
{
	init_c687_timer();
}
void module_fini()
{
}
