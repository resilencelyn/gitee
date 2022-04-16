#include <skyeye_module.h>

const char* skyeye_module = "mc146818_rtc";
extern void init_mc146818_rtc();

void module_init()
{
    init_mc146818_rtc();
}

void module_fini()
{
}
