#include <skyeye_module.h>
const char* skyeye_module = "p2020_pic";
extern void init_p2020_pic();
void module_init()
{
	init_p2020_pic();
}
void module_fini()
{
}
