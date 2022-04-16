#include <skyeye_module.h>

const char* skyeye_module = "w5100_end";
extern void init_w5100_end();

void module_init()
{
	init_w5100_end();
}

void module_fini()
{
}
