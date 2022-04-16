#include <skyeye_module.h>
const char* skyeye_module = "timer1_stm32f0x";
extern void init_timer1_stm32f0x();
void module_init()
{
	init_timer1_stm32f0x();
}
void module_fini()
{
}
