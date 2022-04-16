#include <skyeye_module.h>
const char* skyeye_module = "spi_mx25l128";
extern void init_spi_mx25l128();
void module_init()
{
	init_spi_mx25l128();
}
void module_fini()
{
}
