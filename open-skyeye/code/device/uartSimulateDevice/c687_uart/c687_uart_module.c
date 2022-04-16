#include <skyeye_module.h>
const char* skyeye_module = "c687_uart";
extern void init_c687_uart();
void module_init()
{
	init_c687_uart();
}
void module_fini()
{
}
