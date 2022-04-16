#include <skyeye_module.h>
const char* skyeye_module = "ft2000a_lpc";
extern void init_ft2000a_lpc();
void module_init()
{
	init_ft2000a_lpc();
}
void module_fini()
{
}
