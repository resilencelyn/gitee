#include <skyeye_module.h>
const char* skyeye_module = "flash_am29lv160B";
extern void init_flash_am29lv160B();
void module_init()
{
	init_flash_am29lv160B();
}
void module_fini()
{
}
