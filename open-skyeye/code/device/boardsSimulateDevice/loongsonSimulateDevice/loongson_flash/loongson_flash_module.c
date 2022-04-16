#include <skyeye_module.h>
const char* skyeye_module = "loongson_flash";
extern void init_loongson_flash();
void module_init()
{
	init_loongson_flash();
}
void module_fini()
{
}
