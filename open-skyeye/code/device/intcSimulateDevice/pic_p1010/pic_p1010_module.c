#include <skyeye_module.h>
const char* skyeye_module = "pic_p1010";
extern void init_pic_p1010();
void module_init()
{
	init_pic_p1010();
}
void module_fini()
{
}
