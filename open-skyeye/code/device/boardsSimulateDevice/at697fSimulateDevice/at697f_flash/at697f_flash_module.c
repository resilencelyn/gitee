#include <skyeye_module.h>
const char* skyeye_module = "at697f_flash";
extern void init_at697f_flash();
void module_init()
{
	init_at697f_flash();
}
void module_fini()
{
}
