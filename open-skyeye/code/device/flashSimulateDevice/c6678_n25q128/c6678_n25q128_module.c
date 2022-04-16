#include <skyeye_module.h>
const char* skyeye_module = "c6678_n25q128";
extern void init_c6678_n25q128();
void module_init()
{
	init_c6678_n25q128();
}
void module_fini()
{
}
