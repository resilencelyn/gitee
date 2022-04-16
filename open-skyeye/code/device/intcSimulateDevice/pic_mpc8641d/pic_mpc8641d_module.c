#include <skyeye_module.h>
const char* skyeye_module = "pic_mpc8641d";
extern void init_pic_mpc8641d();
void module_init()
{
	init_pic_mpc8641d();
}
void module_fini()
{
}
