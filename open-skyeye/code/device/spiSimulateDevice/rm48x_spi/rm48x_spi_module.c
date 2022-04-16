#include <skyeye_module.h>
const char* skyeye_module = "rm48x_spi";
extern void init_rm48x_spi();

void module_init()
{
	init_rm48x_spi();
}

void module_fini()
{
}
