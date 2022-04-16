#include <skyeye_module.h>
#include <stdio.h>

const char *skyeye_module = "spi_xdht";

extern void init_xdht_spi();

void module_init(){
	init_xdht_spi();
}

void module_fini(){
}
