#include "skyeye_module.h"

const char* skyeye_module = "Rs232_bus";

extern void init_Rs232_bus();

void module_init(){
	init_Rs232_bus();
}

void module_fini(){
}
