#include "skyeye_module.h"

const char* skyeye_module = "Rs422_bus";

extern void init_Rs422_bus();

void module_init(){
	init_Rs422_bus();
}

void module_fini(){
}
