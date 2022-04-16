#include <skyeye_module.h>
const char* skyeye_module = "loongson_pwm";

extern void init_loongson_pwm();

void module_init()
{
	init_loongson_pwm();
}

void module_fini()
{
}
