#include <skyeye_module.h>
const char* skyeye_module = "fifo_McBSP_c6748";
extern void init_fifo_McBSP_c6748();
void module_init()
{
	init_fifo_McBSP_c6748();
}
void module_fini()
{
}
