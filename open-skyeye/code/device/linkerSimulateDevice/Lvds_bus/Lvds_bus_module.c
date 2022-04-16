#include "skyeye_module.h"

const char* skyeye_module = "Lvds_bus";

extern void init_Lvds_bus();

void module_init(){
	init_Lvds_bus();
}

void module_fini(){
}
