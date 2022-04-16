#include <skyeye_module.h>
const char* skyeye_module = "htey_kgl";
extern void init_htey_kgl();
void module_init()
{
	init_htey_kgl();
}
void module_fini()
{
}
