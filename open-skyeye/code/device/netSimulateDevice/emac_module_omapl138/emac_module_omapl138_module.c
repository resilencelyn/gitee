#include <skyeye_module.h>
const char* skyeye_module = "emac_module_omapl138";
extern void init_emac_module_omapl138();
void module_init()
{
	init_emac_module_omapl138();
}
void module_fini()
{
}
