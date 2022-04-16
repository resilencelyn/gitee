#include <skyeye_module.h>
const char* skyeye_module = "loongson_spi";

extern void init_loongson_spi();

void module_init()
{
	init_loongson_spi();
}

void module_fini()
{
}
