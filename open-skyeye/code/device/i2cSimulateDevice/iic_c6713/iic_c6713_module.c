#include <skyeye_module.h>
const char* skyeye_module = "iic_c6713";
extern void init_iic_c6713();
void module_init()
{
	init_iic_c6713();
}
void module_fini()
{
}
