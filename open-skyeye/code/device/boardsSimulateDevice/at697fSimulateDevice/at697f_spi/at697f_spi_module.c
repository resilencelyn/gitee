#include <skyeye_module.h>
const char* skyeye_module = "at697f_spi";
extern void init_at697f_spi();
void module_init()
{
	init_at697f_spi();
}
void module_fini()
{
}
