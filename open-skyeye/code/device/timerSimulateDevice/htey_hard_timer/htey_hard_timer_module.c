#include <skyeye_module.h>
const char* skyeye_module = "htey_hard_timer";
extern void init_htey_hard_timer();
void module_init()
{
	init_htey_hard_timer();
}
void module_fini()
{
}
