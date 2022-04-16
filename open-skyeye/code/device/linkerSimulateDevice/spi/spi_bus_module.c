#include "skyeye_module.h"

const char* skyeye_module = "skyeye_spi_bus";

extern void init_spi_bus();

void module_init(){
	init_spi_bus();
}

void module_fini(){
}
