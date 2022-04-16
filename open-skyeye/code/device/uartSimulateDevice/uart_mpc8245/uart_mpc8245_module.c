#include "skyeye_module.h"

const char* skyeye_module = "uart_mpc8245";

extern void init_16550_uart();

void module_init(){
	init_16550_uart();
}

void module_fini(){
}
