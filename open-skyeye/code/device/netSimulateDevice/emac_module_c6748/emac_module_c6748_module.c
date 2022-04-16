#include <skyeye_module.h>
const char* skyeye_module = "emac_module_c6748";
extern void init_emac_module_c6748();
void module_init()
{
	init_emac_module_c6748();
}
void module_fini()
{
}
