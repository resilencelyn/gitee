#include "skyeye_module.h"

const char* skyeye_module = "sc16550_uart";

extern void init_16550_uart();

void module_init(){
	init_16550_uart();
}

void module_fini(){
}
