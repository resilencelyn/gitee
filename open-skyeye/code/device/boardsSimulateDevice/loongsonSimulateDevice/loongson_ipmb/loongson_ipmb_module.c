#include <skyeye_module.h>
const char* skyeye_module = "loongson_ipmb";
extern void init_loongson_ipmb();
void module_init()
{
	init_loongson_ipmb();
}
void module_fini()
{
}
