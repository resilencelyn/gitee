#include <skyeye_module.h>
const char* skyeye_module = "msmc_c6678";
extern void init_msmc_c6678();
void module_init()
{
	init_msmc_c6678();
}
void module_fini()
{
}
