#include <skyeye_module.h>
const char* skyeye_module = "ft2000a_cru";
extern void init_ft2000a_cru();
void module_init()
{
	init_ft2000a_cru();
}
void module_fini()
{
}
