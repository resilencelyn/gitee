#include <skyeye_module.h>
const char* skyeye_module = "uc1608_lcd";
extern void init_uc1608_lcd();
void module_init()
{
	init_uc1608_lcd();
}
void module_fini()
{
}
