#include <skyeye_module.h>
const char* skyeye_module = "ft2000a_iic";

extern void init_ft2000a_iic();
void module_init()
{
	init_ft2000a_iic();
}

void module_fini()
{
}
