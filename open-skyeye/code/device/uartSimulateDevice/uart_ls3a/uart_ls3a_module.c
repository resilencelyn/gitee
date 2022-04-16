#include "skyeye_module.h"

const char* skyeye_module = "uart_ls3a";

extern void init_ls3a_uart();

void module_init(){
	init_ls3a_uart();
}

void module_fini(){
}
