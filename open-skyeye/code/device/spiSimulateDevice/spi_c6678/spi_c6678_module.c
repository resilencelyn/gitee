#include <skyeye_module.h>

const char* skyeye_module = "spi_c6678";
extern void init_spi_c6678();

void module_init()
{
	init_spi_c6678();
}

void module_fini()
{
}
