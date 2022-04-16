#include <skyeye_module.h>
const char* skyeye_module = "L2cache_c6713";
extern void init_L2cache_c6713();
void module_init()
{
	init_L2cache_c6713();
}
void module_fini()
{
}
