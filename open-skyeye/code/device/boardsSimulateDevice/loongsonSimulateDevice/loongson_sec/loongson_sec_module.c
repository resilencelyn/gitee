#include <skyeye_module.h>
const char* skyeye_module = "loongson_sec";
extern void init_loongson_sec();
void module_init()
{
	init_loongson_sec();
}
void module_fini()
{
}
