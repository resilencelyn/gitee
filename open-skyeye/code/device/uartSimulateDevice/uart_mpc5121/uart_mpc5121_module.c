#include <skyeye_module.h>
const char* skyeye_module = "uart_mpc5121";
extern void init_uart_mpc5121();
void module_init()
{
	init_uart_mpc5121();
}
void module_fini()
{
}
