#include <skyeye_module.h>

const char* skyeye_module = "i8259a_pic";
extern void init_i8259a_pic();

void module_init()
{
    init_i8259a_pic();
}

void module_fini()
{
}
