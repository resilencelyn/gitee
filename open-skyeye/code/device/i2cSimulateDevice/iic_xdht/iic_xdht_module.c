#include <skyeye_module.h>
#include <stdio.h>

const char *skyeye_module = "iic_xdht";

extern void init_xdht_iic();

void module_init(){
	init_xdht_iic();
}

void module_fini(){
}
