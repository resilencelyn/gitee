#include <skyeye_module.h>
const char* skyeye_module = "gpio_c6748";
extern void init_gpio_c6748();
void module_init()
{
	init_gpio_c6748();
}
void module_fini()
{
}
