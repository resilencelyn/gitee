#include <skyeye_module.h>
const char* skyeye_module = "at697f_spi_device";
extern void init_at697f_spi_device();
void module_init()
{
	init_at697f_spi_device();
}
void module_fini()
{
}