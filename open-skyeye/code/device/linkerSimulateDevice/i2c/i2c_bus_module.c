#include "skyeye_module.h"

const char* skyeye_module = "skyeye_i2c_bus";

extern void init_i2c_bus();

void module_init(){
	init_i2c_bus();
}

void module_fini(){
}
