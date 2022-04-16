#include <skyeye_module.h>
const char* skyeye_module = "usart_stm32f0x";
extern void init_usart_stm32f0x();
void module_init()
{
	init_usart_stm32f0x();
}
void module_fini()
{
}
