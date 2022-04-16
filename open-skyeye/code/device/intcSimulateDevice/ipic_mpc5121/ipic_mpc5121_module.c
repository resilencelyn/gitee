#include <skyeye_module.h>
const char* skyeye_module = "ipic_mpc5121";
extern void init_ipic_mpc5121();
void module_init()
{
	init_ipic_mpc5121();
}
void module_fini()
{
}
