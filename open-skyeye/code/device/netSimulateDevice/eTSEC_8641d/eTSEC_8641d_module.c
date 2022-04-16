#include <skyeye_module.h>
const char* skyeye_module = "eTSEC_8641d";
extern void init_eTSEC_8641d();
void module_init()
{
	init_eTSEC_8641d();
}
void module_fini()
{
}
