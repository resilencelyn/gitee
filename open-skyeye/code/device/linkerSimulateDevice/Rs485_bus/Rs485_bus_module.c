#include "skyeye_module.h"

const char* skyeye_module = "Rs485_bus";

extern void init_Rs485_bus();

void module_init(){
	init_Rs485_bus();
}

void module_fini(){
}
