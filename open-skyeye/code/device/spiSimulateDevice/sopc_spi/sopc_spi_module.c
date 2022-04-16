#include <skyeye_module.h>

const char* skyeye_module = "sopc_spi";

extern void init_sopc_spi();

void module_init()
{
	init_sopc_spi();
}

void module_fini()
{
}
