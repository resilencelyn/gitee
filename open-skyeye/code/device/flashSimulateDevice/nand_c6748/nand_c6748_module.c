#include <skyeye_module.h>
const char* skyeye_module = "nand_c6748";
extern void init_nand_c6748();
void module_init()
{
	init_nand_c6748();
}
void module_fini()
{
}
