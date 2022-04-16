#include "skyeye_module.h"

const char* skyeye_module = "rs422_linker";

extern void init_rs422_linker();

void module_init(){
	init_rs422_linker();
}

void module_fini(){
}
