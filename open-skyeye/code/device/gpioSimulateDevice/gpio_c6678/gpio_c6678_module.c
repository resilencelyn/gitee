#include <skyeye_module.h>
const char* skyeye_module = "gpio_c6678";
extern void init_gpio_c6678();
void module_init()
{
	init_gpio_c6678();
}
void module_fini()
{
}
