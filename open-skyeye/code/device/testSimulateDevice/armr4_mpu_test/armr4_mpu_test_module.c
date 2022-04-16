#include <skyeye_module.h>

const char* skyeye_module = "armr4_mpu_test";
extern void init_armr4_mpu_test();

void module_init()
{
	init_armr4_mpu_test();
}

void module_fini()
{
}
