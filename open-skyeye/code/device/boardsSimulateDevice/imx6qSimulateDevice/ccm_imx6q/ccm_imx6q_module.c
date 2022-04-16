#include <skyeye_module.h>
const char* skyeye_module = "ccm_imx6q";
extern void init_ccm_imx6q();
void module_init()
{
	init_ccm_imx6q();
}
void module_fini()
{
}
