#include "skyeye_module.h"
#include <stdio.h>

const char* skyeye_module = "skyeye_uart_file";

extern void init_uart_file();

void module_init(){
	init_uart_file();
}

void module_fini(){
}
