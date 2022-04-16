#include <skyeye_module.h>
const char* skyeye_module = "ft2000a_gpio";
extern void init_ft2000a_gpio();
void module_init()
{
	init_ft2000a_gpio();
}
void module_fini()
{
}
