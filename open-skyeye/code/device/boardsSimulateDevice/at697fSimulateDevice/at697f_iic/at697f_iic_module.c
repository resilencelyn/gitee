#include <skyeye_module.h>
const char* skyeye_module = "at697f_iic";
extern void init_at697f_iic();
void module_init()
{
	init_at697f_iic();
}
void module_fini()
{
}
