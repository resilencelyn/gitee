#include <skyeye_module.h>
const char* skyeye_module = "htey_GPS";
extern void init_htey_GPS();
void module_init()
{
	init_htey_GPS();
}
void module_fini()
{
}
