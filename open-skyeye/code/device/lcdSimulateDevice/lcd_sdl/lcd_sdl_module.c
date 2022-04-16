#include <skyeye_module.h>
const char* skyeye_module = "lcd";
extern void init_lcd();
void module_init()
{
	init_lcd();
}
void module_fini()
{
}
