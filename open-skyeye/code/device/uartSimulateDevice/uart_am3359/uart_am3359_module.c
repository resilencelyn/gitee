#include <skyeye_module.h>
#include <stdio.h>

const char *skyeye_module = "uart_am3359";

extern void init_am3359_uart();

void module_init(){
	init_am3359_uart();
}

void module_fini(){
}
