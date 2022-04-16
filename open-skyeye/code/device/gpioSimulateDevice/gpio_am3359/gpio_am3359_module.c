#include <skyeye_module.h>
#include <stdio.h>

const char *skyeye_module = "gpio_am3359";
extern void init_am3359_gpio();

void module_init(){ init_am3359_gpio(); }
void module_fini() {}
