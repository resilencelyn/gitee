#include <skyeye_module.h>
const char* skyeye_module = "lcd_omapl138";
extern void init_lcd_omapl138();
void module_init()
{
	init_lcd_omapl138();
}
void module_fini()
{
}
