#include <skyeye_module.h>
const char* skyeye_module = "McBSP_c6748";
extern void init_McBSP_c6748();
void module_init()
{
	init_McBSP_c6748();
}
void module_fini()
{
}
