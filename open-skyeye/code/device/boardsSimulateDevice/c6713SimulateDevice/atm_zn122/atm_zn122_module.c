#include "skyeye_module.h"

const char* skyeye_module = "atm_zn122";

extern void init_zn122_atm();

void module_init(){
	init_zn122_atm();
}

void module_fini(){
}
