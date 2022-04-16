#include <skyeye_module.h>
const char* skyeye_module = "pic_mpc837x";
extern void init_pic_mpc837x();
void module_init()
{
	init_pic_mpc837x();
}
void module_fini()
{
}
