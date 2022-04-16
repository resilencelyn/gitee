#include <skyeye_module.h>
const char* skyeye_module = "flash_interface_stm32f0x";
extern void init_flash_interface_stm32f0x();
void module_init()
{
	init_flash_interface_stm32f0x();
}
void module_fini()
{
}
