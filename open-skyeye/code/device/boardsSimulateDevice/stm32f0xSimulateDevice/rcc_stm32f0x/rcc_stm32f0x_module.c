#include <skyeye_module.h>
const char* skyeye_module = "rcc_stm32f0x";
extern void init_rcc_stm32f0x();
void module_init()
{
	init_rcc_stm32f0x();
}
void module_fini()
{
}
