#include <skyeye_module.h>
const char* skyeye_module = "pic_T2080";
extern void init_pic_T2080();
void module_init()
{
	init_pic_T2080();
}
void module_fini()
{
}
