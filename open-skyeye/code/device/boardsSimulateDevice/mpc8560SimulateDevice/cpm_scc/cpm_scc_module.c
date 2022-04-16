#include <skyeye_module.h>
const char* skyeye_module = "cpm_scc";
extern void init_cpm_scc();
void module_init()
{
	init_cpm_scc();
}
void module_fini()
{
}
