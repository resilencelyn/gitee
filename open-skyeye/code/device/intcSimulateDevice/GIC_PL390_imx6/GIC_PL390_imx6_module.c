#include <skyeye_module.h>
const char* skyeye_module = "GIC_PL390_imx6";
extern void init_GIC_PL390_imx6();

void module_init()
{
	init_GIC_PL390_imx6();
}

void module_fini()
{
}
