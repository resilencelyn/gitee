#include <skyeye_module.h>
const char* skyeye_module = "loongson_collect";
extern void init_loongson_collect();
void module_init()
{
	init_loongson_collect();
}
void module_fini()
{
}
