#include <skyeye_module.h>
const char* skyeye_module = "duart_p1010";
extern void init_duart_p1010();
void module_init()
{
	init_duart_p1010();
}
void module_fini()
{
}
