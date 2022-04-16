#include "skyeye_module.h"

const char* skyeye_module = "sysctrl_at91rm9200";

extern void init_at91rm9200_sysctrl();

void module_init(){
	init_at91rm9200_sysctrl();
}

void module_fini(){
}
