#include <skyeye_module.h>
const char* skyeye_module = "semaphore_c6713";
extern void init_semaphore_c6713();
void module_init()
{
	init_semaphore_c6713();
}
void module_fini()
{
}
