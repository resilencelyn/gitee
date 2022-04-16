#include <skyeye_module.h>
#include <stdio.h>

const char *skyeye_module = "spi_am335x";

extern void init_am335x_spi();

void module_init(){
	init_am335x_spi();
}

void module_fini(){
}
