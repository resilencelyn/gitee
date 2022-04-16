#include <skyeye_module.h>
#include <stdio.h>

const char *skyeye_module = "iic_am335x";

extern void init_am335x_iic();

void module_init(){
	init_am335x_iic();
}

void module_fini(){
}
