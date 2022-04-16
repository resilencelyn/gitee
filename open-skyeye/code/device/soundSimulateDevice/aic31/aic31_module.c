#include <skyeye_module.h>
#include <stdio.h>

const char *skyeye_module = "aic31";

extern void init_aic31();

void module_init(){
	init_aic31();
}

void module_fini(){
}
