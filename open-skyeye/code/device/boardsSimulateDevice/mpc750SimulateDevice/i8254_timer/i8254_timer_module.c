#include <skyeye_module.h>

const char* skyeye_module = "i8254_timer";
extern void init_i8254_timer();

void module_init()
{
    init_i8254_timer();
}

void module_fini()
{
}
