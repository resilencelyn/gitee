#include <skyeye_module.h>
const char* skyeye_module = "ft2000a_spi";
extern void init_ft2000a_spi();
void module_init()
{
	init_ft2000a_spi();
}
void module_fini()
{
}
