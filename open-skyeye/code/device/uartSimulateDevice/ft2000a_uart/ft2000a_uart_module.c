#include <skyeye_module.h>
const char* skyeye_module = "ft2000a_uart";
extern void init_ft2000a_uart();
void module_init()
{
	init_ft2000a_uart();
}
void module_fini()
{
}
