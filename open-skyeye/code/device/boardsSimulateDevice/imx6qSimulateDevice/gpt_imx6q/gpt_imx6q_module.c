#include <skyeye_module.h>
const char* skyeye_module = "gpt_imx6q";
extern void init_gpt_imx6q();
void module_init()
{
	init_gpt_imx6q();
}
void module_fini()
{
}
