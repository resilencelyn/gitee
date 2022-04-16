#include <skyeye_module.h>
const char* skyeye_module = "loongson_config";

extern void init_loongson_config();

void module_init()
{
	init_loongson_config();
}

void module_fini()
{
}
