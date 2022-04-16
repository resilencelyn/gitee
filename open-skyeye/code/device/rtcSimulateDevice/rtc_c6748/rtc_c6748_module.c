#include <skyeye_module.h>
const char* skyeye_module = "rtc_c6748";
extern void init_rtc_c6748();
void module_init()
{
	init_rtc_c6748();
}
void module_fini()
{
}
