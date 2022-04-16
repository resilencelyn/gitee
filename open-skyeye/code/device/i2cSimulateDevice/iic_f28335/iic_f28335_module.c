#include <skyeye_module.h>
#include <stdio.h>

const char *skyeye_module = "iic_f28335";

extern void init_f28335_iic();

void module_init(){
	init_f28335_iic();
}

void module_fini(){
}
