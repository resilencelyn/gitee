#include <skyeye_module.h>
const char* skyeye_module = "spi_omapl138";
extern void init_spi_omapl138();

void module_init()
{
	init_spi_omapl138();
}

void module_fini()
{
}
