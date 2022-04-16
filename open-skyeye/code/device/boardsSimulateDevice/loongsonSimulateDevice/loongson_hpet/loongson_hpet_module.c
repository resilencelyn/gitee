#include <skyeye_module.h>
const char* skyeye_module = "loongson_hpet";
extern void init_loongson_hpet();
void module_init()
{
	init_loongson_hpet();
}
void module_fini()
{
}
