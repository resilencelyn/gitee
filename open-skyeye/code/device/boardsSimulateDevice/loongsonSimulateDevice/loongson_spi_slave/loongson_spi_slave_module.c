#include <skyeye_module.h>
const char* skyeye_module = "loongson_spi_slave";

extern void init_loongson_spi_slave();

void module_init()
{
	init_loongson_spi_slave();
}

void module_fini()
{
}
