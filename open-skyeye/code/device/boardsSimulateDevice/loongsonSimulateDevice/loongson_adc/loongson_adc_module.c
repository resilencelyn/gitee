#include <skyeye_module.h>
const char* skyeye_module = "loongson_adc";
extern void init_loongson_adc();
void module_init()
{
	init_loongson_adc();
}
void module_fini()
{
}
