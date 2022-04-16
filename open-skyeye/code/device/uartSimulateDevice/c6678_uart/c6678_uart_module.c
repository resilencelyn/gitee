#include <skyeye_module.h>
const char* skyeye_module = "c6678_uart";
extern void init_c6678_uart();
void module_init()
{
	init_c6678_uart();
}
void module_fini()
{
}
