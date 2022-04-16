#include <skyeye_module.h>
const char* skyeye_module = "htey_hdlc_cmd_file";
extern void init_htey_hdlc_cmd_file();
void module_init()
{
	init_htey_hdlc_cmd_file();
}
void module_fini()
{
}
