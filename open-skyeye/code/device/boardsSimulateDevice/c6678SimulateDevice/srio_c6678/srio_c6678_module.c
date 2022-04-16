#include <skyeye_module.h>

const char* skyeye_module = "srio_c6678";
extern void init_srio_c6678();

void module_init()
{
	init_srio_c6678();
}

void module_fini()
{
}
