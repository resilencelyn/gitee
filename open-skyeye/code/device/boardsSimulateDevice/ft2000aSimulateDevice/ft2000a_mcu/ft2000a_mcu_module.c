#include <skyeye_module.h>
const char* skyeye_module = "ft2000a_mcu";
extern void init_ft2000a_mcu();
void module_init()
{
	init_ft2000a_mcu();
}
void module_fini()
{
}
