#include <skyeye_module.h>
const char* skyeye_module = "gpio_linker";
extern void init_gpio_linker();
void module_init()
{
	init_gpio_linker();
}
void module_fini()
{
}
