#include <skyeye_module.h>
const char* skyeye_module = "ddr3_c6678";
extern void init_ddr3_c6678();
void module_init()
{
	init_ddr3_c6678();
}
void module_fini()
{
}
