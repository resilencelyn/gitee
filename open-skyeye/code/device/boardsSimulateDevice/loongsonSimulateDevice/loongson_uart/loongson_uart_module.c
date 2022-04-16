#include <skyeye_module.h>
const char* skyeye_module = "loongson_uart";
extern void init_loongson_uart();
void module_init()
{
	init_loongson_uart();
}
void module_fini()
{
}
