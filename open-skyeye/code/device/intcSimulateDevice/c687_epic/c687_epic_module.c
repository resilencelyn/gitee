#include <skyeye_module.h>
const char* skyeye_module = "c687_epic";
extern void init_c687_epic();
void module_init()
{
	init_c687_epic();
}
void module_fini()
{
}
