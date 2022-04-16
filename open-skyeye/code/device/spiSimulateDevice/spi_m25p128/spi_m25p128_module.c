#include <skyeye_module.h>
const char* skyeye_module = "spi_m25p128";
extern void init_spi_m25p128();
void module_init()
{
	init_spi_m25p128();
}
void module_fini()
{
}
