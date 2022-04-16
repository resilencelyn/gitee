#include "skyeye_module.h"

const char* skyeye_module = "uart_imx6";

extern void init_imx6_uart();

void module_init(){
	init_imx6_uart();
}

void module_fini(){
}
