#include <skyeye_module.h>

const char* skyeye_module = "bm3823_intc";
extern void init_bm3823_intc();

void module_init()
{
    init_bm3823_intc();
}

void module_fini()
{
}
