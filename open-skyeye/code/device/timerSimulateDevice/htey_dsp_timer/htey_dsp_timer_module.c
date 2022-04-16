#include <skyeye_module.h>
const char* skyeye_module = "htey_dsp_timer";
extern void init_htey_dsp_timer();
void module_init()
{
	init_htey_dsp_timer();
}
void module_fini()
{
}
