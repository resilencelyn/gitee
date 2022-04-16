#include <skyeye_module.h>
const char* skyeye_module = "sata_c6748";
extern void init_sata_c6748();
void module_init()
{
	init_sata_c6748();
}
void module_fini()
{
}
