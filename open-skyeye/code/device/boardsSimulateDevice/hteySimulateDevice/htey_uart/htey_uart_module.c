#include <skyeye_module.h>
const char* skyeye_module = "htey_uart";
extern void init_htey_uart();
void module_init()
{
	init_htey_uart();
}
void module_fini()
{
}
