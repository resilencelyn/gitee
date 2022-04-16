#include <skyeye_module.h>
#include <stdio.h>

const char *skyeye_module = "iic_omapl138";

extern void init_omapl138_iic();

void module_init(){
	init_omapl138_iic();
}

void module_fini(){
}
