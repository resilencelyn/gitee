#include <skyeye_module.h>
#include <stdio.h>

const char *skyeye_module = "eeprom_cat24c256";

extern void init_eeprom_cat24c256();

void module_init(){
	init_eeprom_cat24c256();
}

void module_fini(){
}
