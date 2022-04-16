#include "skyeye_module.h"

const char* skyeye_module = "skyeye_uart_pc";

extern void init_uart_pc();

void module_init(){
	init_uart_pc();
}

void module_fini(){
}
